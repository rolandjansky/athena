#!/bin/bash
SCRIPTNAME=$(basename $0)
echo "$SCRIPTNAME:  command provided: \"$@\""
if [[ "$ATLAS_RELEASE_BASE" != *"AthSimulation"* ]]; then
  echo "$SCRIPTNAME:  will execute provided command as this is not an AthSimulation environment"
  $@
  # take return value from executed command
  exit $?
else
  echo "$SCRIPTNAME:  this is an AthSimulation build environment, *not* executing provided command"
fi
