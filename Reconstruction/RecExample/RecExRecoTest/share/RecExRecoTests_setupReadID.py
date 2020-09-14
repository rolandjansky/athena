#This is based on the new style configuration settings:
#https://gitlab.cern.ch/atlas/athena/blob/master/Tracking/TrkConfig/python/TrackCollectionReadConfig.py


from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()

if (nThreads >= 1):
  
  from AthenaConfiguration.ComponentFactory import CompFactory
  AthReadAlg=CompFactory.AthReadAlg
  AthReadAlg_RecExRecoTests = AthReadAlg ('TrackCollectionRead_CombinedInDetTracks',
                              Key = 'TrackCollection/CombinedInDetTracks',
                              Aliases = [],
                              ExtraInputs = [('InDetDD::SiDetectorElementCollection', 'ConditionStore+PixelDetectorElementCollection'),
                                             ('InDetDD::SiDetectorElementCollection', 'ConditionStore+SCT_DetectorElementCollection'),
                                             ( 'InDetDD::TRT_DetElementContainer' , 'ConditionStore+TRT_DetElementContainer' ) ])

  topSequence += AthReadAlg_RecExRecoTests



