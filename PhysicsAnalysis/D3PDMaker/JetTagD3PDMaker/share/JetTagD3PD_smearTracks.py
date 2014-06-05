####
## JO fragment to smear tracks and rerun PV before rerunning btagging and dumping the btag d3pd
####

if not 'JetTagD3PD_InsituDBDataName' in dir():
  JetTagD3PD_InsituDBDataName = "insituDatabase4_data12_8TeV.00207620.physics_JetTauEtmiss.merge.NTUP_BTAGSLIM.r4065_p1278_p1344.root"

if not 'JetTagD3PD_InsituDBMCName' in dir():
  JetTagD3PD_InsituDBMCName= "insituDatabase_mc12_8TeV.147914.Pythia8_AU2CT10_jetjet_JZ4W.merge.NTUP_BTAGSLIM.e1126_s1469_s1470_r3542_r3549_p1075.root"

from AthenaCommon.AppMgr import topSequence

from ImpactParameterTuningAlgs.ImpactParameterTuningAlgsConf import Analysis__AdjustTracks
JetTagD3PDAdjustTracks = Analysis__AdjustTracks(name="JetTagD3PDAdjustTracks")

JetTagD3PDAdjustTracks.TrackContainerName           = "TrackParticleCandidate"        
JetTagD3PDAdjustTracks.OutputTrackContainerName     = "AdjustedTrackParticleCandidate"
JetTagD3PDAdjustTracks.OutputTruthTrackName         = "AdjustedTrackParticleTruthCandidate"
JetTagD3PDAdjustTracks.RemoveVxContainer            = False     
JetTagD3PDAdjustTracks.RemovePVLinks                = True 
JetTagD3PDAdjustTracks.RemoveFirstTrackParam        = True
JetTagD3PDAdjustTracks.SmearAtPV                    = False 
JetTagD3PDAdjustTracks.UseDBForSmearing             = True 
JetTagD3PDAdjustTracks.CreateTruthTrackContainer    = True
# JetTagD3PDAdjustTracks.Efficiency                   = 0.978                           
# JetTagD3PDAdjustTracks.EfficiencyEndcap1            = 0.968                           
# JetTagD3PDAdjustTracks.EfficiencyEndcap2            = 0.959                           
# JetTagD3PDAdjustTracks.EfficiencyEndcap3            = 0.929                           
# JetTagD3PDAdjustTracks.EtaEndcap1                   = 1.3                             
# JetTagD3PDAdjustTracks.EtaEndcap2                   = 1.9                             
# JetTagD3PDAdjustTracks.EtaEndcap3                   = 2.3                             
JetTagD3PDAdjustTracks.SmearD0                      = True                           
JetTagD3PDAdjustTracks.SmearZ0                      = True                           
JetTagD3PDAdjustTracks.DoEfficiency                 = False                           
JetTagD3PDAdjustTracks.D0SmearBarrel                = 0.1                          
JetTagD3PDAdjustTracks.D0SmearEndcap                = 0.1                           
JetTagD3PDAdjustTracks.D0SmearLongRange             = 0.                            
JetTagD3PDAdjustTracks.D0LongRangeFrac              = 0.     
JetTagD3PDAdjustTracks.Z0SmearBarrel                = 0.1    
JetTagD3PDAdjustTracks.Z0SmearEndcap                = 0.2
JetTagD3PDAdjustTracks.Z0SmearLongRange             = 0.                            
JetTagD3PDAdjustTracks.Z0LongRangeFrac              = 0.                            
JetTagD3PDAdjustTracks.EtaEndap                     = 1.2                             
JetTagD3PDAdjustTracks.DoFake                       = False                          
# JetTagD3PDAdjustTracks.FakeRate                     = 0.5   
JetTagD3PDAdjustTracks.DoShared                     = False                           
# JetTagD3PDAdjustTracks.SharedRate                   = 0.5     
#JetTagD3PDAdjustTracks.NameInsituDB_DATA	    = "insituDatabaseDATA.root"	
#JetTagD3PDAdjustTracks.NameInsituDB_MC	            = "insituDatabaseMC.root"
JetTagD3PDAdjustTracks.NameInsituDB_DATA= JetTagD3PD_InsituDBDataName
JetTagD3PDAdjustTracks.NameInsituDB_MC= JetTagD3PD_InsituDBMCName
JetTagD3PDAdjustTracks.MakeNtuple                   = False


topSequence += JetTagD3PDAdjustTracks

myrerunPrimVtx=True
if myrerunPrimVtx:

  from InDetRecExample.InDetJobProperties import InDetFlags
  InDetFlags.disableInDetReco.set_Value_and_Lock(False)
  InDetFlags.disableTracking.set_Value_and_Lock(True)
  InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
  InDetFlags.doConversions.set_Value_and_Lock(False)
  InDetFlags.doParticleCreation.set_Value_and_Lock(False)
  InDetFlags.preProcessing.set_Value_and_Lock(False)
##InDetFlags.postProcessing.set_Value_and_Lock(False)
  InDetFlags.doV0Finder.set_Value_and_Lock(False)
  InDetFlags.doSlimming.set_Value_and_Lock(False)
  InDetFlags.doStatistics.set_Value_and_Lock(False)

  #rec.Commissioning.set_Value_and_Lock(True)
  #from AthenaCommon.BeamFlags import jobproperties
  #jobproperties.Beam.energy.set_Value_and_Lock(3500*Units.GeV)
  #jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(8.0)
  #jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)



BTaggingFlags.TrackParticleCollectionName = "AdjustedTrackParticleCandidate"
BTaggingFlags.TrackParticleTruthCollectionName = "AdjustedTrackParticleTruthCandidate"
if myrerunPrimVtx:
    BTaggingFlags.PrimaryVertexCollectionName = "AdjustedVxPrimaryCandidate"

JetTagD3PD_smearTracks=True



 
