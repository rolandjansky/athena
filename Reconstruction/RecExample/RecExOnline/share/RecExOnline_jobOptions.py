## -------------------------------------------------------------------
## File: RecExOnline_jobOptions.py
##
## Authors: Anadi Canepa, Max Baak
## Date: 04 April 2009
##
## This JO is intended for online reconstruction.
## It can be run over a BSRDO via EMON service
## See instructions line by line
##
## -------------------------------------------------------------------

## General run configuration (bs emon, filename, datatype, magnetic field, etc.)
## Reconstruction flags for online running
## Monitoring flags for online running
if useEmon:
  include ("RecExOnline/RecExOnline_emonsvc.py")
include ("RecExOnline/RecExOnline_globalconfig.py")
include ("RecExOnline/RecExOnline_recoflags.py")
include ("RecExOnline/RecExOnline_monitoring.py")

## Pre-exec && preinclude, taken from ami tag
if (not 'preExec' in dir()):
  preExec = []
for cmd in preExec:
  exec(cmd)

## Usual main reco top options
include ("RecExCommon/RecExCommon_topOptions.py")

## New protections against crashes
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
AthenaEventLoopMgr.FailureMode=2

## Post configuration, taken from ami tag
if (not 'postInclude' in dir()):
  postInclude = [] 
for fragment in postInclude: include(fragment)

## debug outputlevel, etc
include ( "RecExOnline/RecExOnline_postconfig.py" )

## -------------------------------------------------------------------
## End of RecExOnline_jobOptions.py

