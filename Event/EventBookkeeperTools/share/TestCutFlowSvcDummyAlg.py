# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# setup the input
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if 'inputFiles' in dir():
    athenaCommonFlags.FilesInput = inputFiles.split(',')
    del inputFiles
else:
    athenaCommonFlags.FilesInput = [
        '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1'
    ]

theApp.EvtMax = 10

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg()

# setup the CutFlowSvc
from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
CreateCutFlowSvc(svcName="CutFlowSvc", seq=topSequence, addMetaDataToAllOutputFiles=True)

# setup the test alg
from EventBookkeeperTools.PyFilterTester import PyFilterTester
topSequence += PyFilterTester('myFilter')
