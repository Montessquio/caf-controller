#!/bin/bash
JLinkExe -device EFR32BG22CXXXF512 -if SWD -speed 4000 -autoconnect 1 -CommanderScript flash.jlink

