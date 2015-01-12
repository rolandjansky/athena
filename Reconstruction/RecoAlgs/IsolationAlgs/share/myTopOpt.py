from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/offline/test/mc11_valid.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1310_s1300_d622.RDO.10evts.pool.root"]
athenaCommonFlags.PoolAODOutput = "xAOD.pool.root"

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.TrackParticleSlimmer=False 
AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False

from JetRec.JetRecFlags import jetFlags
jetFlags.useTruth = False
jetFlags.useTracks = True
jetFlags.useMuonSegments = False
#jetFlags.usePflow = True 
#jetFlags.separateJetAlgs = True
#jetFlags.applyCalibration = False

#switch off b-tagging (fed up of all these warnings ;-) )
from BTagging.BTaggingFlags import BTaggingFlags as btf
btf.Active = False

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)

from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock( False )
#rec.doMuon.set_Value_and_Lock( False )
rec.doTau.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock(  True )
rec.doWriteESD.set_Value_and_Lock(  False )
#rec.doFloatingPointException.set_Value_and_Lock(True)
#rec.doNameAuditor.set_Value_and_Lock(  True )

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

from IsolationAlgs.pflowisoGetter import pflowisoGetter
pflowisoGetter()

index = -1
for i,comp in enumerate(topSequence):
 print i, comp
 if comp.getName() == "METReconstruction":
   index = i
   break
print 'index of met algo',index
print 'putting pflow isol alg just after...'
myAlg = [ topSequence.EDPFlowCentralAlg, topSequence.EDPFlowForwardAlg, topSequence.PFlowIsolation ]
moveToAfterMET = ['EDPFlowCentralAlg', 'EDPFlowForwardAlg', 'PFlowIsolation']
jnd = 1
for e in moveToAfterMET:
    delattr(topSequence,e)
    topSequence.insert(index+jnd,myAlg[jnd-1])
    jnd = jnd + 1

theApp.EvtMax = -1

#print MSMgr.StreamList
#streamAOD=MSMgr.StreamList[0] # stream aod
#streamAOD.AddItem( "xAOD::EventShape_v1#*" )
#streamAOD.AddItem( "xAOD::EventShapeAuxInfo_v1#*" )
