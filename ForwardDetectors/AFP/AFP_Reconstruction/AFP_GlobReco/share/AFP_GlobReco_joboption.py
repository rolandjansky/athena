# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_GlobReco package
#==============================================================

from AthenaConfiguration.ComponentFactory import CompFactory

# side A = 0, side C = 1
afpProtonRecoToolA = CompFactory.getComp("AFP_ProtonRecoAnalytical")("AFP_ProtonRecoAnalyticalA", parametrizationFileName="param_mad_b1_def.txt", side=0)
afpProtonRecoToolC = CompFactory.getComp("AFP_ProtonRecoAnalytical")("AFP_ProtonRecoAnalyticalC", parametrizationFileName="param_mad_b2_def.txt", side=1)

protonsToolsList=[afpProtonRecoToolA, afpProtonRecoToolC]

# collect all output names and make a list with unique names for write handle keys; if this goes wrong AFP_GlobRecoTool::initialize() will complain
outputProtonList=[]
for protonTool in protonsToolsList:
        try:
                # in case the "protonsContainerName" is set
                contName=getattr(protonTool, "protonsContainerName")
        except AttributeError:
                # in case the "protonsContainerName" is not set
                contName=protonTool.getDefaultProperty("protonsContainerName")
        if contName not in outputProtonList:
                outputProtonList.append(contName)

# prepare proton reco tool
protonRecoTool = CompFactory.getComp("AFP_GlobRecoTool")("AFP_GlobRecoTool", RecoToolsList=protonsToolsList, AFPProtonContainerList=outputProtonList )

# actually setup the track reco
from AFP_GlobReco.AFP_GlobRecoConf import AFP_GlobReco
topSequence += AFP_GlobReco("AFP_GlobReco", recoTool = protonRecoTool)


