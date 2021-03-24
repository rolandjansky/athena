# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the ALFA_LocRec package
#==============================================================
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.AlgSequence         import AlgSequence
TopLocRecSeq  = AlgSequence()

#-- SiD part ------------------------------------------------------------

from AFP_LocReco.AFP_LocRecoConf      import AFP_SIDLocReco
TopLocRecSeq += AFP_SIDLocReco("AFP_SIDLocReco")

# select between a real data or a simulation mode - Simulation = 0, RealData = 1
if globalflags.DataSource()=='data':
        AFP_SIDLocReco.DataType = 1
else:
        AFP_SIDLocReco.DataType = 0

# select algo for tracker reco
AFP_SIDLocReco.ListAlgoSID = ["SIDBasicKalman"]
AFP_SIDLocReco.AmpThresh = 5000.	

#-- TiD part ------------------------------------------------------------

from AFP_LocReco.AFP_LocRecoConf      import AFP_TDLocReco
TopLocRecSeq += AFP_TDLocReco("AFP_TDLocReco")

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

