#!/bin/bash
echo "skip_if_dbg.sh:  command line provided: \"$@\""
if [[ "$CMTCONFIG" != *dbg* ]]; then
  echo "skip_if_dbg.sh:  will execute provided command line"
  $@
  # exit with the return value from the script
  exit $?
else
  echo "skip_if_dbg.sh:  running in debug build, not executing provided command line"
fi
