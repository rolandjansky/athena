# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

class TrigValOutputBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
     TrigValJobOptionBlock.__init__(self, 'None', ['None','BS','ESDAOD','AOD','ESD','TAG'])

     self.name = "Output"
    
     if( current == "" ) : current = "None"
     self.setCurrent(current)  # this calls initializeBlocks()


   def jobNameBlock(self) :
      if( self.current() == 'None' ) :
          return ""
      else:
          return "to"+self.current()


   def  initializeBlocks(self) :

     ####################         ###############################
     if(    self.current() == 'None' ) :
        self.jobProperties     = """
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD=False 
doTAG=False
"""
     elif ( self.current() == 'ESD' ) :
        self.jobProperties     = """
rec.doWriteAOD=False
rec.doWriteESD=True
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD=True 
doTAG=False
"""

        self.extraTopOptions   = """
## to remove any particular class uncomment and modify  :
#StreamESD.ItemList.remove("LVL1_ROI#*")
"""

     elif ( self.current() == 'ESDAOD' ) :
        self.jobProperties     = """
rec.doWriteAOD=True
rec.doWriteESD=True
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD=True 
doTAG=False
"""
#        self.jobFlags     += """
#TriggerFlags.outputLVL1configFile = "/tmp/outputLVL1config.xml"
#TriggerFlags.outputHLTconfigFile = "/tmp/outputHLTconfig.xml"
#"""
        self.extraTopOptions   = """
## to remove any particular class uncomment and modify  :
#StreamESD.ItemList.remove("LVL1_ROI#*")
#StreamAOD.ItemList.remove("LVL1_ROI#*")
"""

     elif ( self.current() == 'AOD' ) :
        self.jobProperties     = """
rec.doWriteAOD=True
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=True 
rec.doESD=False 
doTAG=False

"""
#        self.jobFlags     += """
#TriggerFlags.outputLVL1configFile = "/tmp/outputLVL1config.xml"
#TriggerFlags.outputHLTconfigFile = "/tmp/outputHLTconfig.xml"
#"""
        self.extraTopOptions   = """
## to remove any particular class uncomment and modify  :
#StreamAOD.ItemList.remove("LVL1_ROI#*")
"""

     elif ( self.current() == 'TAG' ) :

        self.jobProperties     = """
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=TAG
rec.doAOD=False
rec.doESD=False 
doTAG=TAG
"""

     elif ( self.current() == 'BS' ) :

        self.jobProperties     = """
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doESD=False 
doTAG=False
"""

        self.extraTopOptions     = """
ServiceMgr.LVL1ConfigSvc.CreateLegacyObjects=True

include( "ByteStreamCnvSvc/WriteByteStream_EventStorage_jobOptions.py" )
#ByteStreamEventStorageOutputSvc = ServiceMgr.ByteStreamEventStorageOutputSvc
#ByteStreamEventStorageOutputSvc.DumpFlag    = True
StreamBS = AthenaOutputStream("StreamBS",
                              EvtConversionSvc = "ByteStreamCnvSvc")
StreamBS.ForceRead=True
StreamBS.ItemList  += [ "ROIB::RoIBResult#*" ]
#StreamBS.ItemList +=["2542#*"]
#StreamBS.ItemList +=["2541#*"]
#StreamBS.ItemList +=["2540#*"]
StreamBS.ItemList += [ "TRT_RDO_Container#*" ]
StreamBS.ItemList += [ "SCT_RDO_Container#*" ]
StreamBS.ItemList += [ "PixelRDO_Container#*" ]
StreamBS.ItemList +=["2721#*"]
StreamBS.ItemList +=["2927#*"]
#StreamBS.ItemList +=["MdtCsmContainer#*"]
#StreamBS.ItemList +=["RpcPadContainer#*"]
#StreamBS.ItemList +=["TgcRdoContainer#*"]
#StreamBS.ItemList +=["CscRawDataContainer#*"]

# -------------------------------------------------------------
# Dectector BS converters
# -------------------------------------------------------------
theApp.Dlls += [ "TrigT1ResultByteStream" ]
theApp.Dlls += [ "LArCalibUtils" ]
theApp.Dlls += [ "LArByteStream" ]
theApp.Dlls += [ "TileByteStream" ]
theApp.Dlls += [ "MuonByteStream" ]

theApp.CreateSvc  += ["StoreGateSvc/StoreGateSvc" ]
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += [ 
	"PixelRDO_Container/PixelRDOs", 
	"SCT_RDO_Container/SCT_RDOs", 
	"TRT_RDO_Container/TRT_RDOs", 
	"LArRawChannelContainer/LArRawChannels", 
	"TileRawChannelContainer/TileRawChannelCnt", 
	"RpcPadContainer/RPCPAD",
	"MdtCsmContainer/MDTCSM",
	"TgcRdoContainer/TGCRDO",
	"CscRawDataContainer/CSCRDO",
	"ROIB::RoIBResult/RoIBResult",
	"MuCTPI_RDO/MUCTPI_RDO",
        "CTP_RDO/CTP_RDO"
	] 
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ] 



"""

     ####################         ###############################
     else :
         self.jobProperties  = "#WARNING job block output option "+self.current()+" is not implemented yet !"

    

