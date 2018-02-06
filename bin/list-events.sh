#!/bin/bash

grep -E 'event_trigger|event_watch|REGISTER' $(find -name '*.c' -o -name '*.h') | grep '"' | cut -d '"' -f 1-2 | cut -d ':' -f 2 | sed 's/^ *//' | sed 's/#define .* e/e/' | sed 's/("/ /' | sed 's/watch/watch  /' | sed 's/REGISTER_HANDLER/event_watch  /' | sed 's/ /    /' | sort
