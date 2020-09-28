#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os,psutil,sys

from AthenaCommon.Logging import logging
log = logging.getLogger('L1TopoSimulationTest.py')
         
fmenu ,fTOBs = 'L1Topoconfig_MC_pp_v8_NewNaming.xml','eventdump_new.txt'

print ('File for menu :',fmenu)
print ('File for TOBs :',fTOBs)
   
from AthenaCommon.AppMgr import ServiceMgr as svcMgr, theApp

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#---------------------------------------------------------------------------------#
# MT-specific code
# Get number of processes and threads
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
nProc = jp.ConcurrencyFlags.NumProcs()

if nThreads >=1 :
   from AthenaCommon.AlgScheduler import AlgScheduler
   AlgScheduler.OutputLevel( INFO )
   AlgScheduler.ShowControlFlow( True )
   AlgScheduler.ShowDataDependencies( True )
   AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )

   # Support for the MT-MP hybrid mode
   if (nProc > 0) :
      from AthenaCommon.Logging import log as msg
      if (theApp.EvtMax == -1) : 
         msg.fatal('EvtMax must be >0 for hybrid configuration')
         sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)
         if ( theApp.EvtMax % nProc != 0 ) :
            msg.warning('EvtMax[%s] is not divisible by nProcs[%s]: MP Workers will not process all requested events',theApp.EvtMax,nProc)
         chunkSize = int (theApp.EvtMax / nProc)
         from AthenaMP.AthenaMPFlags import jobproperties as jps 
         jps.AthenaMPFlags.ChunkSize= chunkSize
         msg.info('AthenaMP workers will process %s events each',chunkSize)
            
   ## force loading of data. make sure this alg is at the front of the
   ## AlgSequence
   #
   from SGComps.SGCompsConf import SGInputLoader
   topSequence+=SGInputLoader(OutputLevel=DEBUG, ShowEventDump=False)
   # ThreadPoolService thread local initialization
   from GaudiHive.GaudiHiveConf import ThreadPoolSvc
   svcMgr += ThreadPoolSvc("ThreadPoolSvc")
   svcMgr.ThreadPoolSvc.ThreadInitTools = ["ThreadInitTool"]

# MT-specific code
#---------------------------------------------------------------------------------#
from L1TopoSimulation.L1TopoSimulationTestConfig import L1TopoSimulationTest

topSequence += L1TopoSimulationTest()
topSequence.L1TopoSimulationTest.InputASCIIFile = fTOBs
topSequence.L1TopoSimulationTest.InputXMLFile = fmenu

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'"]

#---------------------------------------------------------------------------------#
# MT-specific code

#  set algCardinality = 1 to disable cloning for all Algs
algCardinality = nThreads
#  Cloning can be disable for any alg  

if (algCardinality > 1):
   for alg in topSequence:      
      name = alg.name()
      if name in ["L1TopoSimulation"] :
         # suppress INFO message about Alg unclonability
         # set alg.Cardinality = 1 to disable cloning for specific Alg
         alg.Cardinality = 1
      else:
         alg.Cardinality = algCardinality

# MT-specific code
#---------------------------------------------------------------------------------#
