#!/bin/bash

# art-description: HelloWorld in athenaHLT with OH monitoring
# art-type: build
# art-include: master/Athena

export NAME=HelloWorld_OHMonitoring
# use export ART_FILE_NAME=... to set the INPUT_FILE value
export ART_CMD="athenaHLT.py -M -n 10 -f INPUT_FILE AthExHelloWorld/HelloWorldOptions.py"
export ART_TIMEOUT="20m"
export ART_NO_COUNT="true"

exec_art_trigp1test.sh

