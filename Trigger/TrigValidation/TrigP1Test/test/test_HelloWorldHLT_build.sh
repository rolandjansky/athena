#!/bin/bash

# art-description: HelloWorld in athenaHLT
# art-type: build
# art-include: master/Athena

export NAME=HelloWorldHLT
# use export ART_FILE_NAME=... to set the INPUT_FILE value 
export ART_CMD="athenaHLT.py -n 10 -f INPUT_FILE AthExHelloWorld/HelloWorldOptions.py"

exec_art_trigp1test.sh -c "${ART_CMD}" -t "20m"

