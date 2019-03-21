#!/bin/bash

# art-description: HelloWorld in athenaHLT with start/stop/start transition
# art-type: build
# art-include: master/Athena

export NAME=HelloWorldHLT_run_stop_run
# use export ART_FILE_NAME=... to set the INPUT_FILE value
export TRANSITIONS="run-stop-run-saveHist.trans"
export ART_CMD="get_files -data ${TRANSITIONS}; athenaHLT.py -i -M --rewind -f INPUT_FILE AthExHelloWorld/HelloWorldOptions.py < ${TRANSITIONS}"

exec_art_trigp1test.sh -c "${ART_CMD}" -t "20m"

