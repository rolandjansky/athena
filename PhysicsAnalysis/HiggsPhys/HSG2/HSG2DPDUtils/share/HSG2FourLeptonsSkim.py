from AthenaCommon.AlgSequence import AlgSequence
from GaudiAlg.GaudiAlgConf import Sequencer
from AthenaCommon.AlgSequence import AthSequencer
import PyUtils.castor as pycastor

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

PoolAODInput=pycastor.pool_nsls("/castor/cern.ch/grid/atlas/atlasgroupdisk/phys-top/user/RichardHawkings/user.RichardHawkings.0108173.topmix_Muon.AOD.v2/*.pool.root")


if not vars().has_key('EvtMax'):     EvtMax     = -1
if not vars().has_key('SkipEvents'): SkipEvents = 0

theJob += H4lDPDMaker.H4lElectronFilter("H4l_4ElectronFilter",nElec=4,\
                                        eTCuts=[6*Units.GeV,6*Units.GeV,6*Units.GeV,6*Units.GeV],\
                                        etaCuts=[2.5,2.5,2.5,2.5],\
                                        qualityCuts=["Loose","Loose","Medium","Medium"])

theJob += H4lDPDMaker.H4lMuonFilter("H4l_4StacoMuonFilter",nMu=4,\
                                    pTCuts=[6*Units.GeV,6*Units.GeV,6*Units.GeV,6*Units.GeV],\
                                    etaCuts=[2.5,2.5,2.5,2.5],\
                                    qualityCuts=["combined+lowpt","combined+lowpt","combined+lowpt","combined+lowpt"])

theJob += H4lDPDMaker.H4lMuonFilter("H4l_4MuidMuonFilter",nMu=4,\
                                    pTCuts=[6*Units.GeV,6*Units.GeV,6*Units.GeV,6*Units.GeV],\
                                    etaCuts=[2.5,2.5,2.5,2.5],
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=["combined+lowpt","combined+lowpt","combined+lowpt","combined+lowpt"])


filter2e2mStaco= AthSequencer("H4l_2e2mStacoFilter")

# use sequencer to combine filters for 2e2m

filter2e2mStaco += H4lDPDMaker.H4lMuonFilter("H4l_2MuonStacoFilter",nMu=2,\
                                        pTCuts=[6*Units.GeV,6*Units.GeV],\
                                        etaCuts=[2.5,2.5],\
                                        qualityCuts=["combined+lowpt","combined+lowpt"])

filter2e2mStaco += H4lDPDMaker.H4lElectronFilter("H4l_2ElectronFilter",nElec=2,\
                                            eTCuts=[6*Units.GeV,6*Units.GeV],\
                                            etaCuts=[2.5,2.5],\
                                            qualityCuts=["Loose","Loose"])

theJob+=filter2e2mStaco

filter2e2mMuid= AthSequencer("H4l_2e2mMuidFilter")

# use sequencer to combine filters for 2e2m

filter2e2mMuid += H4lDPDMaker.H4lMuonFilter("H4l_2MuonMuidFilter",nMu=2,\
                                        pTCuts=[6*Units.GeV,6*Units.GeV],\
                                        etaCuts=[2.5,2.5],\
                                        qualityCuts=["combined+lowpt","combined+lowpt"])

filter2e2mMuid += H4lDPDMaker.H4lElectronFilter("H4l_2ElectronFilter2",nElec=2,\
                                            eTCuts=[6*Units.GeV,6*Units.GeV],\
                                            etaCuts=[2.5,2.5],\
                                            qualityCuts=["Loose","Loose"])

theJob+=filter2e2mMuid


# The job starts here!
include ("RecExCommon/RecExCommon_topOptions.py")

# define streams

streamOut = MSMgr.NewPoolStream("HSG2_DPD","HSG2_DPD.pool.root")
streamOut.AcceptAlgs(["H4l_4ElectronFilter","H4l_4StacoMuonFilter","H4l_4MuidMuonFilter","H4l_2e2mMuidFilter","H4l_2e2mStacoFilter"])

dpdOutHelper.addAllItemsFromInputExceptExcludeList("HSG2_DPD",[])


theApp.EvtMax=100
