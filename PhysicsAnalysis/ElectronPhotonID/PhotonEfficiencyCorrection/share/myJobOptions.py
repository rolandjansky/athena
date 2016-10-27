#specify how many events it should process. Set it equal to -1 to process all events
theApp.EvtMax = 100

#set input files - svcMgr has a property that lets you specify the input files, as a list:
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [ " /afs/cern.ch/atlas/project/PAT/xAODs/r6630/mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.AOD.e3601_s2576_s2132_r6630_tid05358812_00/AOD.05358812._000010.pool.root.1" ]

#One of the existing master sequences where one should attach all algorithms
algSeq = CfgMgr.AthSequencer("AthAlgSeq")

# Add the test algorithm. the alg is a part of the tool, so it should be imported:
from PhotonEfficiencyCorrection.PhotonEfficiencyCorrectionConf import testAthenaPhotonAlg
alg = testAthenaPhotonAlg()

#define input files, for now it uses my own directory where the tool is, since it not a working part of the athena, please note that the current recomendation of PhotonID WG can be found here: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PhotonEfficiencyCorrection

#Set inputs for ID efficiency
alg.PhotonEfficiencyCorrectionTool.CorrectionFileNameConv = "PhotonEfficiencyCorrection/v1/efficiencySF.offline.Tight.2015.13TeV.rel20.con.v01.root";
alg.PhotonEfficiencyCorrectionTool.CorrectionFileNameUnconv = "PhotonEfficiencyCorrection/v1/efficiencySF.offline.Tight.2015.13TeV.rel20.unc.v01.root";

#Or set inputs for track isolation efficiency (comment previous)
#alg.PhotonEfficiencyCorrectionTool.CorrectionFileNameConv = "PhotonEfficiencyCorrection/efficiencySF.offline.isolFixedCutLoose.2015.13TeV.rel20.con.v01.root";
#alg.PhotonEfficiencyCorrectionTool.CorrectionFileNameUnconv = "PhotonEfficiencyCorrection/efficiencySF.offline.isolFixedCutLoose.2015.13TeV.rel20.unc.v01.root";

#Set DataType: for data use 0 (or don't run the tool - faster), for FULLSIM use 1, and for FASTSIM use 3, please note that the input files are also should be different
alg.PhotonEfficiencyCorrectionTool.ForceDataType = 1


#comment this line
alg.OutputLevel = DEBUG
algSeq += alg

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
outStream = MSMgr.NewPoolRootStream( "MyXAODStream", "myXAOD.pool.root" )
outStream.AddItem(['xAOD::PhotonContainer_v1#MyPhotons'])
outStream.AddItem(['xAOD::PhotonAuxContainer_v1#MyPhotonsAux'])
