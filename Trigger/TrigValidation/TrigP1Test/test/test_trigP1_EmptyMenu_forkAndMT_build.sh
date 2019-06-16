#!/bin/bash

# art-description: Same as empty_menu test from TrigUpgradeTest, but with athenaHLT
# art-type: build
# art-include: master/Athena

export NAME=EmptyMenu
export ART_CMD="athenaHLT.py --nprocs 2 --threads 2 --concurrent-events 2 -n 50 -f INPUT_FILE -c 'doEmptyMenu=True;BFieldAutoConfig=False' TrigUpgradeTest/full_menu.py"
export ART_TIMEOUT="20m"

exec_art_trigp1test.sh
