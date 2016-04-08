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

#define input when reading from castor
castorDir = "/castor/cern.ch/grid/atlas/atlasdatadisk/data09_900GeV/DESD_COLLCAND/r988_p62/data09_900GeV.00142174.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102162_00/"
import os
import string
import commands
datestr=commands.getoutput('date +%N')
os.system("rfdir "+castorDir+" |grep DESD |grep \".root\">> filelist."+datestr)
FileList=open("filelist."+datestr,"r").readlines()
FileListGlobal=["root://castoratlas/"+castorDir+file.split()[8]
                for file in FileList
                if file.split()[4]!="0"]
os.system("rm filelist."+datestr)


#PoolESDInput=FileListGlobal
athenaCommonFlags.PoolESDInput=["root://castoratlas//castor/cern.ch/grid/atlas/atlasdatadisk/data09_900GeV/DESD_COLLCAND/r988_p62/data09_900GeV.00142174.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102162_00/DESD_COLLCAND.102162._000002.root.1"]

#PoolESDInput=["/afs/cern.ch/user/r/rosati/scratch0/athena/dev/AtlasProduction-15.6.1.5/PhysicsAnalysis/HiggsPhys/HiggsToFourLeptons/HiggsToFourLeptons-00-03-01/run/AOD.107164._000002.pool.root.1"]

# Turn off most of RecExCommon... (optional)
rec.doCBNT       = False
rec.doWriteESD   = False
rec.doWriteAOD   = False
rec.doAOD        = False
rec.doWriteTAG   = False 
rec.doPerfMon    = False
rec.doHist       = False
rec.doTruth      = False

rec.AutoConfiguration.append('FieldAndGeo')
rec.AutoConfiguration.append('ConditionsTag')
rec.AutoConfiguration.append('RealOrSim')

# Mandatory for ESD->DPD
rec.doDPD   = True
rec.readAOD = False
rec.readESD = True

#PoolAODInput=pycastor.pool_nsls("/castor/cern.ch/grid/atlas/atlasgroupdisk/phys-top/user/RichardHawkings/user.RichardHawkings.0108173.topmix_Muon.AOD.v2/*.pool.root")
# The job starts here!
include ("RecExCommon/RecExCommon_topOptions.py")



if not vars().has_key('EvtMax'):     EvtMax     = -1
if not vars().has_key('SkipEvents'): SkipEvents = 0

theJob += H4lDPDMaker.H4lElectronFilter("H4l_2ElectronFilter",nElec=2,\
                                        eTCuts=[0*Units.GeV,0*Units.GeV],\
                                        etaCuts=[2.5,2.5],\
                                        qualityCuts=["Loose","Loose"],\
                                        collectionName="ElectronCollection")

theJob += H4lDPDMaker.H4lMuonFilter("H4l_2StacoMuonFilter",nMu=2,\
                                    pTCuts=[0*Units.GeV,0*Units.GeV],\
                                    etaCuts=[2.5,2.5],\
                                    qualityCuts=["any","any"])

theJob += H4lDPDMaker.H4lMuonFilter("H4l_2MuidMuonFilter",nMu=2,\
                                    pTCuts=[0*Units.GeV,0*Units.GeV],\
                                    etaCuts=[2.5,2.5],
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=["any","any"])


filter1e1mStaco= AthSequencer("H4l_1e1mStacoFilter")

# use sequencer to combine filters for 2e2m

filter1e1mStaco += H4lDPDMaker.H4lMuonFilter("H4l_1MuonStacoFilter",nMu=1,\
                                        pTCuts=[0*Units.GeV],\
                                        etaCuts=[2.5],\
                                        qualityCuts=["any"])

filter1e1mStaco += H4lDPDMaker.H4lElectronFilter("H4l_1ElectronFilter",nElec=1,\
                                            eTCuts=[0*Units.GeV],\
                                            etaCuts=[2.5],\
                                            qualityCuts=["Loose"],\
                                            collectionName="ElectronCollection")

theJob+=filter1e1mStaco

filter1e1mMuid= AthSequencer("H4l_1e1mMuidFilter")

# use sequencer to combine filters for 2e2m

filter1e1mMuid += H4lDPDMaker.H4lMuonFilter("H4l_1MuonMuidFilter",nMu=1,\
                                        pTCuts=[0*Units.GeV],\
                                        etaCuts=[2.5],\
                                        collectionName="MuidMuonCollection",\
                                        qualityCuts=["any"])

filter1e1mMuid += H4lDPDMaker.H4lElectronFilter("H4l_1ElectronFilter2",nElec=1,\
                                            eTCuts=[0*Units.GeV],\
                                            etaCuts=[2.5],\
                                            qualityCuts=["Loose"],\
                                            collectionName="ElectronCollection")

theJob+=filter1e1mMuid



# define streams

streamOut = MSMgr.NewPoolStream("HSG2_DPD","HSG2_DESD.pool.root")
streamOut.AcceptAlgs(["H4l_2ElectronFilter","H4l_2StacoMuonFilter","H4l_2MuidMuonFilter","H4l_1e1mMuidFilter","H4l_1e1mStacoFilter"])

dpdOutHelper.addAllItemsFromInputExceptExcludeList("HSG2_DPD",[])


theApp.EvtMax=-1
