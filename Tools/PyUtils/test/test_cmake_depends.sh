#!/usr/bin/env bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Test for cmake_depends.py.
# Run this script with/without changes and diff output.
#

# Dependencies:
cmds=("acmd.py cmake depends IOVDbSvc --long")
cmds+=("acmd.py cmake depends xAODEventInfo --dot")
cmds+=("acmd.py cmake depends xAODEventInfo --recursive --dot")
cmds+=("acmd.py cmake depends xAODEventInfo --regex --dot")
cmds+=("acmd.py cmake depends xAODEventInfo --recursive --regex --dot")
# Clients:
cmds+=("acmd.py cmake depends LArReadoutGeometry --clients --dot")
cmds+=("acmd.py cmake depends LArReadoutGeometry --clients --target --dot")
cmds+=("acmd.py cmake depends IOVDbDataModel --clients --recursive --dot")
# Test of an external library:
cmds+=("acmd.py cmake depends ActsCore --clients --target --recursive --external --dot")
cmds+=("acmd.py cmake depends ActsCore --clients --target --recursive")

for c in "${cmds[@]}"; do
   echo "${c}"
   eval "${c}"
done
