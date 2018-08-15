#!/usr/bin/runghc

module Main where

import Data.Char
import Data.List
import System.IO
import System.Process

data Build = Build { build_platform :: String
                   , build_type     :: String }

data Tools = Tools { ar :: String
                   , as :: String
                   , cc :: String
                   , ld :: String }

data Flags = Flags { ar_flags :: [String]
                   , as_flags :: [String]
                   , cc_flags :: [String]
                   , ld_flags :: [String]
                   , qemu_flags :: [String] }

data Project = Project { name :: String
                       , scm  :: String
                       , version    :: String
                       , categories :: [String]
                       , build_root :: String
                       , build_types :: [String] }

data Platform = Platform { platform_name  :: String
                         , platform_flags :: Flags
                         , platform_qemu  :: String }

project = Project { name    = "awooos"
                  , scm     = "todo"
                  , version = "{{scm.tag_or_revision}}-{{build.type}}"
                  , categories = ["executables", "libraries"]
                  , build_root = "./src"
                  , build_types = ["test", "debug", "release"] }

tools = Tools { ar="ar"
              , as="nasm"
              , cc="clang"
              , ld="ld" }

build = Build { build_platform  = "i386"
              , build_type      = "debug" }

flags = Flags { ar_flags    = []
              , as_flags    = []
              , cc_flags    = ["-std=c11", "-pedantic-errors", "-gdwarf-2",
                               "-nostdinc", "-ffreestanding",
                               "-fno-stack-protector", "-fno-builtin",
                               "-fdiagnostics-show-option",
                               "-Werror", "-Weverything", "-Wno-cast-qual",
                               "-Wno-missing-prototypes", "-Wno-vla"]
              , ld_flags    = []
              , qemu_flags  = [] }

i386_flags = Flags { ar_flags   = []
                   , as_flags   = ["-felf32"]
                   , cc_flags   = ["-m32"]
                   , ld_flags   = ["-melf_i386"]
                   , qemu_flags = ["-no-reboot", "-device",
                                  "isa-debug-exit,iobase=0xf4,iosize=0x04"] }

-- TODO: "qemu" is a tool. it should be identified as such.
i386 = Platform { platform_name="i386"
                , platform_flags=i386_flags
                , platform_qemu="qemu-system-i386" }


command (xs:".c") = ""
--command xs:".":"c" = ""

{-
[artifacts:.c]
# artifacts.c_includes is an array which contains
# "-I /path/to/:category/:artifact_name/include" for each artifact.
command =
    ${tools:cc} -c ${flags.cc} ${platform:flags.cc}
      {artifacts.c_includes} -o {name}.o {name}.c
suffix = .o

[artifacts:.asm]
command = {tools.as} {platform:flags.as} -o {name}.o {name}.asm
suffix = .o

[categories:libraries]
requires =
    artifacts.{category}.c
    artifacts.{category}.asm
command = {tools.ar} rcs {output_file} {build.artifacts}
suffix = .a

[categories:executables]
require =
    libraries
command =
    ${tools:ld} -o {artifact.file} -Lsrc/libraries
      ${flags.ld} -T src/link-${build.platform}.ld
      {artifacts.asm} {artifacts.c} {category.libraries.artifacts}
suffix = .exe

# TODO: This feels a bit clumsy, to put it mildly.
[file:isofs]
copy =
    assets/isofs/               isofs/
    {categories.executables}  isofs/system/

[file:iso]
require =
    executables:kernel
    file:isofs
command =
    xorriso -as mkisofs -boot-info-table -R -b boot/grub/stage2_eltorito
      -no-emul-boot -boot-load-size 4 -input-charset utf-8
      -o ${project:name}-${build.platform}-${build.type}.iso isofs/
suffix = .iso

[task:qemu]
require =
    file:iso
# Not sure about the {artifacts.file.iso} thing.
command =
  ${platform:tools.qemu} ${flags.qemu} ${platform:flags.qemu} -vga std
    -serial stdio -cdrom {artifacts.file.iso}

-}



run :: [String] -> IO(String)
run cmd = do
    (_, Just hout, _, _) <- createProcess (proc tool args){ std_out = CreatePipe }
    putStrLn ("$ " ++ cmd_str)
    output <- hGetContents hout
    putStrLn output
    return output
  where cmd_str = intercalate " " cmd
        tool    = head cmd
        args    = tail cmd


platform = i386
artifact_dir = "artifacts/"

cc_include_for lib = "-Isrc/libraries/" ++ lib ++ "/include"
cc_includes libs = map cc_include_for libs

splitOn :: ([Char] -> Bool) -> [Char] -> [[Char]]
splitOn _ [] = []
splitOn f list =
    first : splitOn f (dropWhile f rest)
  where
    (first, rest) = break f list

object_for file =
    name ++ ".o"
  where
    name = head $ splitOn (== ".") file

libs = ["ali", "cadel", "dmm", "flail", "greeter", "hal", "shell", "tests", "tinker"]

build_object file =
  run $ [cc tools] ++ cc_flags flags ++ (cc_flags $ platform_flags platform) ++ cc_includes libs ++
          ["-c", file, "-o", object_for file]

dir_objects category name =
  getDirectoryContents ("src/" ++ category ++ "/" ++ name ++ "/src/")


library_objects name = ["a"]
objects "library" name = do
  return $ map build_object (library_objects name)

library name = do
  artifacts <- objects "library" name
  run ([ar tools, "rcs", artifact_dir ++ name ++ ".a"] ++ artifacts)

executable name = do
  ali     <- library "ali"
  dmm     <- library "dmm"
  greeter <- library "greeter"
  run ["echo awoo"]

-- Aliases
all    = kernel
kernel = executable "kernel"
--iso    = files "iso"

main = do
  run "libraries.test" ["echo", "hi"]
