# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file   PscConfig.py
## @brief  Python flags that are used to configure C++ Psc
## @author Frank Winklmeier
##
## This module is loaded in Psc.cxx and flags are set in TrigPSCPython*Setup.py
## Add flags here that are used to configure the C++ instance

## In interactive mode, call AppMgr::initialize() in connect instead of configure
interactive = False

## Dump job properties (used in TrigPSCPythonSetup.py)
dumpJobProperties = False

## string:string map equivalent to TrigPsc/Config.h (filled in TrigPsc.cxx)
optmap = {}
