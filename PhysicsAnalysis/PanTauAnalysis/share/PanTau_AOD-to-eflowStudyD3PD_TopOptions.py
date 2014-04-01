
#
# JobOptions to produce a D3PD tailored for Peter's eflow Studies
# 
#   Input: Any AOD with eflowRec container
#
#   Output: A D3PD usable for detailed eflowStudies
#
#   Note: These JO can be submitted via pathena!


SGKEY_eflowObjects = "eflowObjects_tauMode"
#//////////////////////////////////
#   Input Files
#//////////////////////////////////

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if athenaCommonFlags.FilesInput.isDefault(): # check if not already set upstream
    
    #--------------------------------  A O D
    #generic test AOD for changing purposes...
    #athenaCommonFlags.FilesInput=['AOD.pool.root']
    
    #Test AOD that has EFOs with cell info assigned to them
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/l/limbach/scratch0/SavedAODs/MarksTestAOD_EFOsWithCellInfo/AOD.pool.root']
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/l/limbach/scratch0/SavedAODs/MarksTestAOD_Oct22/AOD_Oct22.pool.root']
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/l/limbach/scratch0/SavedAODs/MarksTestAOD_17252_CalHits/AOD_17252_CalHits.pool.root']
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/m/mhodgkin/public/AOD_17252_CalHits_Latest.pool.root']
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/m/mhodgkin/work/AOD_17214_500Events_Oct31.root']
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/m/mhodgkin/work/AOD_17214_500Events_Nov19_NoPileup.root'] #no pileup
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/m/mhodgkin/public/AOD_test12Dec.pool.root']
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/m/mhodgkin/work/AODFiles_16Dec/AOD_1.pool.root']
    #
    #athenaCommonFlags.FilesInput=['AOD_withNewEflowObjects.pool.root']
    
    
    # Sample with Split Shower Alg and the k_1 parameter set to 0
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/m/mhodgkin/work/AODFiles_17Dec_k1_0/AOD_1.pool.root']
    
    # Sample with Split Shower Alg
    athenaCommonFlags.FilesInput=['/afs/cern.ch/user/m/mhodgkin/work/AODFiles_16Dec/AOD_1.pool.root']
    
    # Sample without Split Shower Alg. A bug was fixed, 
    #### NOTE: eflowObjects are called: eflowObjects01 (!)
    #athenaCommonFlags.FilesInput=['/afs/cern.ch/user/m/mhodgkin/work/AODFiles_1Feb_noSplotShowerRecoverAlg/AOD_1.pool.root']
    #SGKEY_eflowObjects = "eflowObjects01"
    
athenaCommonFlags.EvtMax=10#250  # number of events to process run on all file

from tauRec.tauRecFlags import jobproperties
jobproperties.tauRecFlags.Enabled.set_Value_and_Lock(False)
jobproperties.tauRecFlags.doRunTauDiscriminant.set_Value_and_Lock(False)

#...add truth info in D3PD
doTrigger=False
doTAG=False

from RecExConfig.RecFlags import rec,Rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False) 
rec.doESD.set_Value_and_Lock(False) 
rec.doAOD.set_Value_and_Lock(False)
#rec.doTruth.set_Value_and_Lock( True )

#disable the AODFix (should resolve problems with tau->pt() being 0.00191628)
rec.doApplyAODFix.set_Value_and_Lock(False)

include( "RecExCommon/RecExCommon_topOptions.py" )




####  ####  ####  ####  ####  ####  ####  ####  ####
#
# Define the D3PD Stream
#
####  ####  ####  ####  ####  ####  ####  ####  ####

## ////////////////////////////////////////////////
## Define the D3PD Stream

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream("EFLOWD3PDSTREAM", "Test_eflowD3PD_16Dec_WithSplitShower_10Evts.root", "eflowD3PD")

# algs to run prior to dumping
from eflowD3PDMaker.eflowD3PDMakerConf import eflowClusterSelector
eflowClusSelector = eflowClusterSelector()
topSequence += eflowClusSelector


from eflowRec.eflowTrackToCaloTrackExtrapolatorTool import eflowTrackToCaloTrackExtrapolatorToolDefault
TrackToCaloTrackExtrapolatorToolDefault=eflowTrackToCaloTrackExtrapolatorToolDefault()



####  ####  ####  ####  ####  ####  ####  ####  ####
#
#                       Configure the D3PD
#
####  ####  ####  ####  ####  ####  ####  ####  ####


from D3PDMakerCoreComps.D3PDObject import D3PDObject


#### The basic kinematics ####
from eflowD3PDMaker.eflowD3PDObject import *
eflowD3PDObject = D3PDObject (makeeflowD3PDObject, 'eflowBase_')
eflowD3PDObject.SGKey = SGKEY_eflowObjects
eflowD3PDObject.defineBlock(0, 'Kinematics', eflowD3PDMakerConf.D3PD__eflowFillerTool)
eflowD3PDObject.defineBlock (0, 'Details', eflowD3PDMakerConf.D3PD__eflowObjectDetailsFillerTool)
alg += eflowD3PDObject(level=10)


#### The track variables ####
from eflowD3PDMaker.eflowTrackD3PDObject import *
eflowTrackD3PDObject = D3PDObject (makeeflowTrackD3PDObject, 'eflowTrack_')
eflowTrackD3PDObject.SGKey = SGKEY_eflowObjects
eflowTrackD3PDObject.defineBlock(   0,'eflowTrack', eflowD3PDMakerConf.D3PD__eflowTrackFillerTool,
                                    eflowTrackToCaloTrackExtrapolatorTool = TrackToCaloTrackExtrapolatorToolDefault)
alg += eflowTrackD3PDObject(10)


#### The calibration hits ####
from calCellInfoD3PDMaker.calCellInfoD3PDObject import *
calCellInfoD3PDObject = D3PDObject(makecalCellInfoD3PDObject,'CalCellInfo_')
calCellInfoD3PDObject.defineBlock(0,'CallCellInfo', calCellInfoD3PDMakerConf.D3PD__calCellInfoFillerTool)
alg += calCellInfoD3PDObject(10)


#### The calibration hits ####
from calCellInfoD3PDMaker.calCellInfoD3PDObject import *
calCellInfoD3PDObject = D3PDObject(makecalCellInfoD3PDObject,'CalCellInfoPreSub_')
calCellInfoD3PDObject.defineBlock(0,'CallCellInfoPreSub', calCellInfoD3PDMakerConf.D3PD__calCellInfoFillerTool)
alg += calCellInfoD3PDObject(10, sgkey='CalCellInfo_preSubtraction')


#### TauRec block ####
import TrackD3PDMaker
from D3PDMakerCoreComps.ContainedVectorMultiAssociation     import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.SimpleAssociation                   import SimpleAssociation

#Test A
from D3PDMakerCoreComps.D3PDObject                          import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags                         import D3PDMakerFlags
from TauD3PDMaker.TauD3PDMakerConf                          import D3PD__TauPIDFillerTool
from TauD3PDMaker.TauD3PDMakerConf                          import D3PD__TauJetFillerTool
from TauD3PDMaker.TauD3PDMakerConf                          import D3PD__TauTrackAssociationTool
from TauD3PDMaker.TauD3PDMakerConf                          import D3PD__TauTrackToOriginFillerTool
import EventCommonD3PDMaker

TauD3PDObject = make_SGDataVector_D3PDObject( "Analysis::TauJetContainer", D3PDMakerFlags.TauSGKey(), "tau_", "TauD3PDObject" )
TauD3PDObject.defineBlock (0, 'Kinematics', EventCommonD3PDMaker.FourMomFillerTool, WriteEt = True, WritePt = True)
TauD3PDObject.defineBlock (0, 'Charge', EventCommonD3PDMaker.ChargeFillerTool)
TauD3PDObject.defineBlock (0, 'PID', D3PD__TauPIDFillerTool)
TauD3PDObject.defineBlock (0, 'TauJet', D3PD__TauJetFillerTool)
TauD3PDObject.defineBlock (0, 'TracksToOrigin', D3PD__TauTrackToOriginFillerTool)
# Associate tau with its tracks
TauTrackAssoc = ContainedVectorMultiAssociation \
            (TauD3PDObject,
             D3PD__TauTrackAssociationTool,prefix='track_')
TrackParticlePerigeePVAssoc = SimpleAssociation \
        (TauTrackAssoc,
         TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
         prefix = 'atPV_',
             blockname = 'SeedCaloTrkPVAssoc')
TrackParticlePerigeePVAssoc.defineBlock (2, 'TrkPV',
                                       TrackD3PDMaker.TrackPerigeeFillerTool,
                                       FillMomentum = True)
alg += TauD3PDObject(10)

#### Add the primary vertex ####
from TrackD3PDMaker.VertexD3PDObject                import PrimaryVertexD3PDObject
alg += PrimaryVertexD3PDObject (0, allowMissing = True,
                                sgkey = D3PDMakerFlags.VertexSGKey(),
                                prefix = 'vxp_')


#### Prepare for truth info ####
from D3PDMakerConfig.D3PDMakerFlags                 import D3PDMakerFlags
from TruthD3PDAnalysis.truthParticleConfig          import truthParticleConfig
D3PDMakerFlags.TruthParticlesSGKey = 'INav4MomTruthEvent'
truthParticleConfig(topSequence, sgkey=D3PDMakerFlags.TruthParticlesSGKey(), writeHadrons=True, writePartons=True, writeGeant=False)


#### Even more truth information ####
from TruthD3PDMaker.GenEventD3PDObject              import GenEventD3PDObject
from TruthD3PDMaker.TruthParticleD3PDObject         import TruthParticleD3PDObject
alg += TruthParticleD3PDObject (10, sgkey=D3PDMakerFlags.TruthParticlesSGKey())


#### Get the true taus into d3pd ####
from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
topSequence += TruthTausToSG()
from TauD3PDMaker.TruthTauD3PDObject                import TruthTauD3PDObject
alg += TruthTauD3PDObject(10)


#### Get the clusters ####
from CaloD3PDMaker.ClusterD3PDObject                import ClusterD3PDObject
alg += ClusterD3PDObject (10, include='Kinematics')




#end of file
