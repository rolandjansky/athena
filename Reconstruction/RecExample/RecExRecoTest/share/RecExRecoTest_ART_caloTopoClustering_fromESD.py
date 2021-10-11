from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.ConcurrencyFlags import jobproperties
nThreads = jobproperties.ConcurrencyFlags.NumThreads()
if nThreads >=1 :
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.OutputLevel( INFO )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )
    AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )
    if not hasattr(topSequence, "SGInputLoader"):
        from SGComps.SGCompsConf import SGInputLoader
        topSequence+=SGInputLoader(OutputLevel=DEBUG, ShowEventDump=False)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-MC16-SDR-RUN2-08")

from RecExConfig.RecFlags import rec
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

#this turns off CaloCluster2xAOD
rec.doWritexAOD.set_Value_and_Lock(False)
#nothing to say on these
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)
rec.doAODCaloCells.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
#Turns off xAODRingSetConfWriter
rec.doCaloRinger.set_Value_and_Lock(False)
#disables VertexCnvAlg
from InDetRecExample.InDetJobProperties import jobproperties
jobproperties.InDetJobProperties.doxAOD.set_Value_and_Lock(False)
#Disables AllExecutedEvents
rec.doFileMetaData.set_Value_and_Lock(False)
#Disable thinning:
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.egammaTrackSlimmer.set_Value_and_Lock(False)
AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)
AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False)
AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False)
AODFlags.AddEgammaMuonTracksInAOD.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=10
include ("RecExCommon/RecExCommon_topOptions.py")
