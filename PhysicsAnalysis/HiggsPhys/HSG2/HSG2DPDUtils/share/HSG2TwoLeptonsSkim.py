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


athenaCommonFlags.PoolAODInput=["/tmp/disimone/mc09_7TeV.105300.PythiaH130zz4l.recon.AOD.e505_s765_s767_r1302_tid137118_00/AOD.137118._000066.pool.root.1"]

#athenaCommonFlags.PoolAODInput=["/tmp/disimone/"]


if not vars().has_key('EvtMax'):     EvtMax     = -1
if not vars().has_key('SkipEvents'): SkipEvents = 0


from PrimaryDPDMaker.PrimaryDPDFlags_EGammaStream import primEGammaDPD
from PrimaryDPDMaker.ElectronFilter import ElectronFilter



# pre-pend an external electron filter
# filter just for robust loose, leave all the rest blank,
# to be checked by ourselves later

# ---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
from ROOT import egammaPID
from ROOT import egammaParameters

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
                                        qualityCuts=["any","any"],\
                                        mass=10*Units.GeV,
                                        collectionName='RobustLooseElectronCollection')

theJob += H4lDPDMaker.H4lMuonFilter("H4l_2StacoMuonFilter",nMu=2,\
                                    pTCuts=[10*Units.GeV,10*Units.GeV],\
                                    qualityCuts=["inMS","inMS"],\
                                    mass=5*Units.GeV)

theJob += H4lDPDMaker.H4lMuonFilter("H4l_2MuidMuonFilter",nMu=2,\
                                    pTCuts=[10*Units.GeV,10*Units.GeV],\
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=["inMS","inMS"],\
                                    mass=5*Units.GeV)


mixedFilterStaco=H4lDPDMaker.H4lMixedFilter("H4lMixedFilterStaco", nLept=2,\
                                            pTCuts=[0,10*Units.GeV],\
                                            eTCuts=[10*Units.GeV,0],\
                                            #electron quality checked before
                                            qualityCuts=["any","inMS"],\
                                            types=["e","mu"],\
                                            collections=["RobustLooseElectronCollection","StacoMuonCollection"],\
                                            mass=5*Units.GeV)

mixedFilterMuid=H4lDPDMaker.H4lMixedFilter("H4lMixedFilterMuid", nLept=2,\
                                           pTCuts=[0,10*Units.GeV],\
                                           eTCuts=[10*Units.GeV,0],\
                                           qualityCuts=["Loose","inMS"],\
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

streamOut = MSMgr.NewPoolStream("HSG2_DPD","/tmp/disimone/HSG2_DAOD_2lskim.00142383._000001.pool.root")
streamOut.AcceptAlgs(["H4l_2ElectronFilter","H4l_2StacoMuonFilter","H4l_2MuidMuonFilter","H4lMixedFilterStaco","H4lMixedFilterMuid"])

dpdOutHelper.addAllItemsFromInputExceptExcludeList("HSG2_DPD",[])


theApp.EvtMax=1000
