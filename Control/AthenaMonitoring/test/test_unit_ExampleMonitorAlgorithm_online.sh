#!/bin/bash

echo $PWD
python -m AthenaMonitoring.ExampleMonitorAlgorithm Common.isOnline=True Output.HISTFileName=ExampleMonitorOutputOnline.root
# Grep to avoid RooFit lines
hist_file_dump.py -r name --no_onfile --hash ExampleMonitorOutputOnline.root | grep '^T' | tee hist-content-online
get_files -symlink test_unit_ExampleMonitorAlgorithm_online.ref
diff hist-content-online test_unit_ExampleMonitorAlgorithm_online.ref
exit $?