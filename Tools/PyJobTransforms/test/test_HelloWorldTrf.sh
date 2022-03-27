#!/bin/bash

set -e

# ST test
HelloWorld_tf.py --maxEvents=5 --CA 

grep 'runArgs.threads = 0' runargs.athena.py
grep 'runArgs.concurrentEvents = 0' runargs.athena.py

# MT test 1
HelloWorld_tf.py --maxEvents=5 --CA --athenaopts="--threads=2 --concurrent-events=1"

grep 'runArgs.threads = 2' runargs.athena.py
grep 'runArgs.concurrentEvents = 1' runargs.athena.py

# MT test 2
ATHENA_CORE_NUMBER=2 HelloWorld_tf.py --maxEvents=5 --CA --multithreaded

grep 'runArgs.threads = 2' runargs.athena.py
grep 'runArgs.concurrentEvents = 2' runargs.athena.py

# MT test 3
ATHENA_CORE_NUMBER=2 HelloWorld_tf.py --maxEvents=5 --CA --multithreaded=True --multiprocess=False

grep 'runArgs.threads = 2' runargs.athena.py
grep 'runArgs.concurrentEvents = 2' runargs.athena.py

# MP test
ATHENA_CORE_NUMBER=2 HelloWorld_tf.py --maxEvents=5 --CA --multiprocess=True

grep 'runArgs.nprocs = 2' runargs.athena.py
grep 'runArgs.threads = 0' runargs.athena.py
grep 'runArgs.concurrentEvents = 0' runargs.athena.py

# postInclude/Exec
HelloWorld_tf.py --maxEvents=5 --CA --postInclude 'AthenaConfiguration.JobOptsDumper.JobOptsDumperCfg,PyJobTransforms.DumpPickle'

# CA arg test 1
HelloWorld_tf.py --maxEvents=5 --CA HelloWorld:True

# CA arg test 2
set +e

if HelloWorld_tf.py --maxEvents=5; then
  exit 1
fi
