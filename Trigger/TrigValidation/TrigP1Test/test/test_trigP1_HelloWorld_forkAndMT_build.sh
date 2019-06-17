#!/bin/bash

# art-description: HelloWorld in athenaHLT with forking into 2 children, with 2 threads and 2 events processed in parallel
# art-type: build
# art-include: master/Athena

export NAME=HelloWorld_forkAndMT
# use export ART_FILE_NAME=... to set the INPUT_FILE value
export ART_CMD="athenaHLT.py --nprocs 2 --threads 2 --concurrent-events 2 -n 20 -f INPUT_FILE AthExHelloWorld/HelloWorldOptions.py"
export ART_TIMEOUT="20m"
export ART_NO_COUNT="true"

exec_art_trigp1test.sh

