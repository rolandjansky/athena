#specify how many events it should process. Set it equal to -1 to process all events
theApp.EvtMax = 15

#set input files - svcMgr has a property that lets you specify the input files, as a list:
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/work/k/krasznaa/public/xAOD/mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e2928_s1982_s2008_r5787_tid01572118_00/AOD.01572118._003538.pool.root.6" ]

#One of the existing master sequences where one should attach all algorithms
from AthenaCommon.AlgSequence import AlgSequence
algSeq = AlgSequence()

# Add the test algorithm. the alg is a part of the tool, so it should be imported:
from ElectronEfficiencyCorrection.ElectronEfficiencyCorrectionConf import testAthenaElectronAlg
alg = testAthenaElectronAlg()

#define input files, for now it uses my own directory where the tool is, since it not a working part of the athena, please note that the current recomendation of PhotonID WG can be found here: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PhotonEfficiencyCorrection
input_files=['../data/efficiencySF.offline.Loose.2012.8TeV.rel17p2.v07.root']

#get files path using PathResolver
#from PathResolver import PathResolver
#inputfiles=[]
#for inp in set_input_files:
#  inputfiles.append(PathResolver.FindCalibFile('PhotonEfficiencyCorrection/v1/'+inp))
alg.ElectronEfficiencyCorrectionTool.CorrectionFileNameList = input_files;

#Set DataType: for data use 0 (or don't run the tool - faster), for FULLSIM use 1, and for FASTSIM use 3, please note that the input files are also should be different
alg.ElectronEfficiencyCorrectionTool.ForceDataType = 1

#alg.PhotonEfficiencyCorrectionTool.CorrectionFileNameList = ['../data/efficiencySF.offline.Tight.2012.8TeV.rel17.geo21.unc.v03.root','../data/efficiencySF.offline.Tight.2012.8TeV.rel17.geo21.con.v03.root']

#comment this line
alg.OutputLevel = DEBUG
algSeq += alg

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
outStream = MSMgr.NewPoolRootStream( "MyXAODStream", "myXAOD.pool.root" )
outStream.AddItem(['xAOD::ElectronContainer_v1#MyElectrons'])
outStream.AddItem(['xAOD::ElectronAuxContainer_v1#MyElectronsAux'])
