# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

## to decode the MuCTPI bytestream
from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import MuCTPIByteStreamTool,RecMuCTPIByteStreamTool
from TrigT1Muctpi.TrigT1MuctpiConf import CBNTAA_ReadMuCTPI_RDO
from TrigT1Muctpi.TrigT1MuctpiConf import CBNTAA_ReadMuCTPI_RIO
from TrigT1Muctpi.TrigT1MuctpiConf import CBNTAA_MuctpiRoI

if not hasattr( svcMgr, "THistSvc" ):
  from GaudiSvc.GaudiSvcConf import THistSvc
  svcMgr += THistSvc()
#svcMgr.THistSvc.Output = ["file1 DATAFILE='HistFile.root' OPT='RECREATE'"];

if not hasattr( theApp.Dlls, "RootHistCnv" ):
  theApp.Dlls += [ "RootHistCnv" ]
  theApp.HistogramPersistency = "ROOT"

if not hasattr( svcMgr, "NTupleSvc" ):
  from GaudiSvc.GaudiSvcConf import NTupleSvc
  svcMgr += NTupleSvc()
#svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='NtupleFile.root' OPT='NEW'" ]

if not hasattr( topSequence, "CBNT_AthenaAware" ):
  from CBNT_Athena.CBNT_AthenaConf import *
  from CBNT_Utils.CBNT_UtilsConf import *
  CBNT_AthenaAware = CBNT_AthenaAware()
#  CBNT_AthenaAware.OutputLevel = 2
  topSequence += CBNT_AthenaAware

if not "CBNTAA_EventInfo" in topSequence.CBNT_AthenaAware.Members :
  from CBNT_Athena.CBNT_AthenaConf import *
  from CBNT_Utils.CBNT_UtilsConf import *
  topSequence.CBNT_AthenaAware.Members += ["CBNTAA_EventInfo"]
        
if not "CBNTAA_ReadMuCTPI_RDO" in topSequence.CBNT_AthenaAware.Members :
  CBNTAA_ReadMuCTPI_RDO = CBNTAA_ReadMuCTPI_RDO()
  CBNTAA_ReadMuCTPI_RDO.StoreRawData = False
  topSequence.CBNT_AthenaAware.Members += ["CBNTAA_ReadMuCTPI_RDO"]

if not "CBNTAA_ReadMuCTPI_RIO" in topSequence.CBNT_AthenaAware.Members :
  topSequence.CBNT_AthenaAware.Members += ["CBNTAA_ReadMuCTPI_RIO"]

if not "CBNTAA_MuctpiRoI" in topSequence.CBNT_AthenaAware.Members :
  CBNTAA_MuctpiRoI = CBNTAA_MuctpiRoI()
  #      CBNT_MuctpiRoI.RoISource = "roi"
  topSequence.CBNT_AthenaAware.Members += ["CBNTAA_MuctpiRoI"]

# print topSequence

# class CBNTAA_Algorithms():
#   """
#     Configurable module to setup ntuple-producing algorithms
#   """
#   def __init__( self, name = "CBNTAA_Algorithms" ):
#     #
#     # CBNT part
#     #
