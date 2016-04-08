# get the logger
from AthenaCommon.Logging import *
hsg2Skimlog = logging.getLogger('hsg2DiLeptonSkim')

hsg2Skimlog.info('************** STARTING HSG2 Dilepton Skim **************')
#==============================================================
# Job definition parameters:
#==============================================================
evtMax = runArgs.maxEvents
InputFiles = runArgs.inputAODFile
OutputFile2L = runArgs.outputDAOD_2LHSG2File
muQual=runArgs.muonQuality
eQual=runArgs.electronQuality

hsg2Skimlog.info( '**** Transformation run arguments ****' )
hsg2Skimlog.info( str(runArgs) )


from AthenaCommon.AlgSequence import AlgSequence
from GaudiAlg.GaudiAlgConf import Sequencer
from AthenaCommon.AlgSequence import AthSequencer
import PyUtils.castor as pycastor
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutHelper

theJob = AlgSequence()

# our own skimming algs
from HSG2DPDUtils import H4lDPDMaker

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from RecExConfig.RecFlags import rec
# Turn off most of RecExCommon... (optional)
rec.doCBNT       = False
rec.doWriteESD   = False
rec.doWriteAOD   = False
rec.doAOD        = False
rec.doWriteTAG   = False
rec.doPerfMon    = False
rec.doHist       = False
rec.doTruth      = False

# Mandatory for ESD->DPD
rec.doDPD   = True
rec.readAOD = True
rec.readESD = False

athenaCommonFlags.PoolAODInput=InputFiles

# Import the needed algorithms and tools
from AnalysisUtils.AnalysisUtilsConf import ElectronIDSelector
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector

ToolSvc += ElectronIDSelector( "RobustLooseElectronFilterSelector",
                               isEM = "RobustLoose"
                               )

theJob += D2PDElectronSelector( "RobustLooseElectronFilter",
                                     OutputLevel            = INFO,
                                     inputCollection        = 'ElectronAODCollection',
                                     outputCollection   = 'RobustLooseElectronCollection',
                                     minNumberPassed        = 1,
                                     electronVetoAuthorList = [],
                                     iSelectorToolList      = [ ToolSvc.RobustLooseElectronFilterSelector ],
                                     clusterEtMin           = 0.,
                                     clusterEtaMax          = 1000
                                     )





theJob += H4lDPDMaker.H4lElectronFilter("H4l_2ElectronFilter",nElec=2,\
                                        eTCuts=[10*Units.GeV,10*Units.GeV],\
                                        # electron quality checked before
                                        qualityCuts=[eQual,eQual],\
                                        mass=10*Units.GeV,
                                        collectionName='RobustLooseElectronCollection')

theJob += H4lDPDMaker.H4lMuonFilter("H4l_2StacoMuonFilter",nMu=2,\
                                    pTCuts=[10*Units.GeV,10*Units.GeV],\
                                    qualityCuts=[muQual,muQual],\
                                    mass=5*Units.GeV)

theJob += H4lDPDMaker.H4lMuonFilter("H4l_2MuidMuonFilter",nMu=2,\
                                    pTCuts=[10*Units.GeV,10*Units.GeV],\
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=[muQual,muQual],\
                                    mass=5*Units.GeV)


mixedFilterStaco=H4lDPDMaker.H4lMixedFilter("H4lMixedFilterStaco", nLept=2,\
                                            pTCuts=[0,10*Units.GeV],\
                                            eTCuts=[10*Units.GeV,0],\
                                            #electron quality checked before
                                            qualityCuts=[eQual,muQual],\
                                            types=["e","mu"],\
                                            collections=["RobustLooseElectronCollection","StacoMuonCollection"],\
                                            mass=5*Units.GeV)

mixedFilterMuid=H4lDPDMaker.H4lMixedFilter("H4lMixedFilterMuid", nLept=2,\
                                           pTCuts=[0,10*Units.GeV],\
                                           eTCuts=[10*Units.GeV,0],\
                                           qualityCuts=[eQual,muQual],\
                                           types=["e","mu"],\
                                           collections=["RobustLooseElectronCollection","MuidMuonCollection"],\
                                           mass=5*Units.GeV)

theJob+=mixedFilterStaco
theJob+=mixedFilterMuid


# The job starts here!
rec.AutoConfiguration.append('FieldAndGeo')
#rec.AutoConfiguration.append('BeamType')
rec.AutoConfiguration.append('ConditionsTag')
rec.AutoConfiguration.append('RealOrSim')


include ("RecExCommon/RecExCommon_topOptions.py")

# define streams

streamOut = MSMgr.NewPoolStream("HSG2_DPD",OutputFile2L)
streamOut.AcceptAlgs(["H4l_2ElectronFilter","H4l_2StacoMuonFilter","H4l_2MuidMuonFilter","H4lMixedFilterStaco","H4lMixedFilterMuid"])

dpdOutHelper.addAllItemsFromInputExceptExcludeList("HSG2_DPD",[])

theApp.EvtMax=evtMax
