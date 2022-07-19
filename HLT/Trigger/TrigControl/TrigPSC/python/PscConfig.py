# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

## @file   PscConfig.py
## @brief  Python flags that are used to configure C++ Psc
## @author Frank Winklmeier
##
## This module is loaded in Psc.cxx and some of the flags here are
## are used to configure the C++ instance.

## In interactive mode, call AppMgr::initialize() in connect instead of configure
interactive = False

## Dump job properties (used in TrigPSCPythonSetup.py)
dumpJobProperties = False

## Exit after dumping job properties
exitAfterDump = False

## Load configuration from file and restart process
reloadAfterDump = False

## Read PSK from database and ignore COOL
forcePSK = False

## string:string map equivalent to TrigPsc/Config.h (filled in TrigPsc.cxx)
optmap = {}
