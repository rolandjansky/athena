# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the ALFA_LocRec package
#==============================================================
from AthenaCommon.GlobalFlags import globalflags

#-- SiD part ------------------------------------------------------------

# get AFP_GeometryTool
from AthenaConfiguration.ComponentFactory import CompFactory
AFP_Geometry_tool = CompFactory.getComp("AFP_GeometryTool")("AFP_Geometry_tool")

# select between a real data or a simulation mode - Simulation = 0, RealData = 1
if globalflags.DataSource()=='data':
        AFP_SIDLocReco_DataType = 1
else:
        AFP_SIDLocReco_DataType = 0

AFP_SIDLocReco_ListAlgoSID = ["SIDBasicKalman"]
AFP_SIDLocReco_AmpThresh = 5000.

# convert EventInfo to xAOD::EventInfo
if( not globalflags.InputFormat.is_bytestream() and ( not objKeyStore.isInInput( "xAOD::EventInfo") ) and ( not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ) ) ):
        from xAODEventInfoCnv.xAODEventInfoCnvAlgDefault import xAODEventInfoCnvAlgDefault
        xAODEventInfoCnvAlgDefault (sequence = topSequence, OutputLevel=WARNING)

# actually setup the track reco
from AFP_LocReco.AFP_LocRecoConf      import AFP_SIDLocReco
topSequence += AFP_SIDLocReco("AFP_SIDLocReco", OutputLevel=WARNING, AFP_Geometry = AFP_Geometry_tool, vecListAlgoSID = AFP_SIDLocReco_ListAlgoSID, DataType = AFP_SIDLocReco_DataType, AmpThresh = AFP_SIDLocReco_AmpThresh)

#-- TiD part ------------------------------------------------------------

from AFP_LocReco.AFP_LocRecoConf      import AFP_TDLocReco
topSequence += AFP_TDLocReco("AFP_TDLocReco")

# select between a real data or a simulation mode - Simulation = 0, RealData = 1
if globalflags.DataSource()=='data':
        AFP_TDLocReco.DataType = 1
else:
        AFP_TDLocReco.DataType = 0

# select algo for timing reco and parameters
AFP_TDLocReco.ListAlgoTD = ["TDZVertex"]
AFP_TDLocReco.AmpThresh = 10.

#-- SiAlignDBTester part ------------------------------------------------------------
# this is a setup for ReadCondHandle (see AFP_DBTools/SiAlignDBTester)

# from IOVDbSvc.CondDB import conddb

# conddb.addFolder("CALO","/CALO/HadCalibration2/CaloEMFrac", className='CaloLocalHadCoeff')
# conddb.addFolderSplitOnline("INDET","/Indet/Onl/Beampos", "/Indet/Beampos", className='AthenaAttributeList')

# schema = "<db>sqlite://;schema=Example.db;dbname=CONDBR2</db>"
# folder = "/FWD/AFP/LocalAlignment"
# locTag="<tag>AFPAlignLoc-00</tag>"

# conddb.addFolder("", schema+folder+locTag, className='CondAttrListCollection')

# from AFP_DBTools.AFP_DBToolsConf      import SiAlignDBTester
# TopLocRecSeq += SiAlignDBTester("SiAlignDBTester", OutputLevel=DEBUG)

