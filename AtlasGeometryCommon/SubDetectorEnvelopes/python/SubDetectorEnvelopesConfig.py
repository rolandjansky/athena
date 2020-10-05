# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
SubDetectorEnvelopes configurations for AtlasGeometryCommon
Elmar Ritsch, 27/09/2013
"""

def getEnvelopeDefSvc(name="AtlasGeometry_EnvelopeDefSvc", **kwargs):
    kwargs.setdefault("DBInDetNode"       , 'InDetEnvelope'    )
    kwargs.setdefault("DBBeamPipeNode"    , 'BeamPipeEnvelope' )
    kwargs.setdefault("DBCaloNode"        , 'CaloEnvelope'     )
    kwargs.setdefault("DBMSNode"          , 'MuonEnvelope'     )
    kwargs.setdefault("DBCavernNode"      , 'CavernEnvelope'   )

    # allow for python-based envelope definitions (see below)
    #  -> TODO: set this to False once the envelope definitions are tagged in the DDDB
    kwargs.setdefault("EnableFallback"  , True             )

    # setup fallback BeamPipeEnvelope
    BeamPipe = Volume()
    BeamPipe.addRZ(   34.3,   3475.0 )
    from AthenaCommon.DetFlags import DetFlags
    if hasattr(DetFlags.simulate, 'HGTD_on') and DetFlags.simulate.HGTD_on():
        BeamPipe.addRZ(   47.0,   3475.0 )
        BeamPipe.addRZ(   47.0,   3535.0 )
        BeamPipe.addRZ(  120.0,   3535.0 )
    else:
        BeamPipe.addRZ(  120.0,   3475.0 )
    BeamPipe.addRZ(  120.0,   4185.0 )
    BeamPipe.addRZ(   41.0,   4185.0 )
    BeamPipe.addRZ(   41.0,   6783.0 )
    BeamPipe.addRZ(   70.0,   6783.0 )
    BeamPipe.addRZ(   70.0,  12900.0 )
    BeamPipe.addRZ(  279.0,  12900.0 )
    BeamPipe.addRZ(  279.0,  18650.0 )
    BeamPipe.addRZ(  436.0,  18650.0 )
    BeamPipe.addRZ(  436.0,  22030.0 )
    BeamPipe.addRZ( 1050.0,  22030.0 )
    BeamPipe.addRZ( 1050.0,  26046.0 )
    BeamPipe.addRZ(    0.0,  26046.0 )

    kwargs.setdefault("FallbackBeamPipeR"    ,  BeamPipe.getRs()        )
    kwargs.setdefault("FallbackBeamPipeZ"    ,  BeamPipe.getZs()        )


    # setup fallback IDEnvelope
    InDet = Volume()
    InDet.addRZ( 1148.,  3475. )
    InDet.addRZ(  34.3,  3475. )
    kwargs.setdefault("FallbackInDetR"  , InDet.getRs()        )
    kwargs.setdefault("FallbackInDetZ"  , InDet.getZs()        )


    # setup fallback CaloEnvelope
    Calo = Volume()
    Calo.addRZ( 1148.0,  3475.0 )
    from AthenaCommon.DetFlags import DetFlags
    if hasattr(DetFlags.simulate, 'HGTD_on') and DetFlags.simulate.HGTD_on():
        Calo.addRZ(   47.0,  3475.0 )
        Calo.addRZ(   47.0,  3535.0 )
        Calo.addRZ(  120.0,  3535.0 )
    else:
        Calo.addRZ(  120.0,  3475.0 )
    Calo.addRZ(  120.0,  4185.0 )
    Calo.addRZ(   41.0,  4185.0 )
    Calo.addRZ(   41.0,  6783.0 )
    Calo.addRZ(  420.0,  6783.0 )
    Calo.addRZ(  420.0,  6736.0 )
    Calo.addRZ( 3800.0,  6736.0 )
    Calo.addRZ( 3800.0,  6550.0 )
    Calo.addRZ( 4255.0,  6550.0 )

    kwargs.setdefault("FallbackCaloR"   , Calo.getRs()        )
    kwargs.setdefault("FallbackCaloZ"   , Calo.getZs()        )


    # setup fallback MuonEnvelope
    Muon = Volume()
    Muon.addRZ(  4255.0 ,  4000.0 )
    Muon.addRZ(  4255.0 ,  6550.0 )
    Muon.addRZ(  3800.0 ,  6550.0 )
    Muon.addRZ(  3800.0 ,  6736.0 )
    Muon.addRZ(   420.0 ,  6736.0 )
    Muon.addRZ(   420.0 ,  6783.0 )
    Muon.addRZ(    70.0 ,  6783.0 )
    Muon.addRZ(    70.0 , 12900.0 )
    Muon.addRZ(   279.0 , 12900.0 )
    Muon.addRZ(   279.0 , 18650.0 )
    Muon.addRZ(   436.7 , 18650.0 )
    Muon.addRZ(   436.7 , 22030.0 )
    Muon.addRZ(  1050.0 , 22030.0 )
    Muon.addRZ(  1050.0 , 26046.0 )
    Muon.addRZ(  1500.0 , 26046.0 )
    Muon.addRZ(  1500.0 , 23001.0 )
    Muon.addRZ(  2750.0 , 23001.0 )
    Muon.addRZ(  2750.0 , 22030.0 )
    Muon.addRZ( 12650.0 , 22030.0 )
    Muon.addRZ( 12650.0 , 18650.0 )
    Muon.addRZ( 13400.0 , 18650.0 )
    Muon.addRZ( 13400.0 , 12900.0 )
    Muon.addRZ( 14200.0 , 12900.0 )
    Muon.addRZ( 14200.0 ,  4000.0 )
    Muon.addRZ( 13000.0 ,  4000.0 )
    kwargs.setdefault("FallbackMuonR"   , Muon.getRs()        )
    kwargs.setdefault("FallbackMuonZ"   , Muon.getZs()        )


    # setup fallback CavernEnvelope
    Cavern = Volume()
    Cavern.addRZ( 13000.0 , 4000.0 )
    Cavern.addRZ( 14200.0 , 4000.0 )
    Cavern.addRZ( 14200.0 , 12900.0 ) # boundary with MS
    Cavern.addRZ( 13400.0 , 12900.0 ) # boundary with MS
    Cavern.addRZ( 13400.0 , 18650.0 ) # boundary with MS
    Cavern.addRZ( 12650.0 , 18650.0 ) # boundary with MS
    Cavern.addRZ( 12650.0 , 22030.0 ) # boundary with MS
    Cavern.addRZ(  2750.0 , 22030.0 ) # boundary with MS
    Cavern.addRZ(  2750.0 , 23001.0 ) # boundary with MS
    Cavern.addRZ(  1500.0 , 23001.0 ) # boundary with MS
    Cavern.addRZ(  1500.0 , 26046.0 ) # boundary with MS
    Cavern.addRZ(     0.0 , 26046.0 ) #

    # the outer dimesions differ between collision and cosmics jobs
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() != 'cosmics':
      #  -> for collision jobs the 'cavern' envelope is much smaller
      Cavern.addRZ(      0.0 , 500000.0 ) # z= +500m
      Cavern.addRZ( 500000.0  , 500000.0 ) # r=  500m
    else:
      #  -> for cosmics simulation the 'cavern' envelope spans much further
      kwargs.setdefault("DBCavernNode"    , 'CavernEnvelopeNONE'   )
      kwargs.setdefault("EnableFallback"  , True                   )
      Cavern.addRZ(       0.0 , 3000000.0 ) # z= +3km
      Cavern.addRZ( 3000000.0 , 3000000.0 ) # r=  3km
    kwargs.setdefault("FallbackCavernR"   , Cavern.getRs()        )
    kwargs.setdefault("FallbackCavernZ"   , Cavern.getZs()        )

    from SubDetectorEnvelopes.SubDetectorEnvelopesConf import DetDescrDBEnvelopeSvc
    return DetDescrDBEnvelopeSvc(name, **kwargs)


# class used for more readable definitions of envelope volumes
#  -> create new volume boundary definitions
#  -> return the coordinate vectors in R and Z and VolumeID
class Volume:
  # initialize empy
  def __init__(self):
    self.r  = [ ]
    self.z  = [ ]

  def addRZ(self, r, z):
    self.r  += [ r  ]
    self.z  += [ z  ]

  def getRs(self):
    return self.r

  def getZs(self):
    return self.z

# For new configuration - doesn't have access to DetFlags as of writing, so all HGTD stuff is missing.
# Just copied from above.

#New configuration in SubDetectorEnvelopesConfigNew.py
def EnvelopeDefSvc(flags,  **kwargs):
    kwargs.setdefault("DBInDetNode"       , 'InDetEnvelope'    )
    kwargs.setdefault("DBBeamPipeNode"    , 'BeamPipeEnvelope' )
    kwargs.setdefault("DBCaloNode"        , 'CaloEnvelope'     )
    kwargs.setdefault("DBMSNode"          , 'MuonEnvelope'     )
    kwargs.setdefault("DBCavernNode"      , 'CavernEnvelope'   )

    # allow for python-based envelope definitions (see below)
    #  -> TODO: set this to False once the envelope definitions are tagged in the DDDB
    kwargs.setdefault("EnableFallback"  , True             )

    # setup fallback BeamPipeEnvelope
    BeamPipe = Volume()
    BeamPipe.addRZ(   34.3,   3475.0 )
    BeamPipe.addRZ(  120.0,   3475.0 ) # TODO - re-add HGTD stuff once we can configure 
    BeamPipe.addRZ(  120.0,   4185.0 )
    BeamPipe.addRZ(   41.0,   4185.0 )
    BeamPipe.addRZ(   41.0,   6783.0 )
    BeamPipe.addRZ(   70.0,   6783.0 )
    BeamPipe.addRZ(   70.0,  12900.0 )
    BeamPipe.addRZ(  279.0,  12900.0 )
    BeamPipe.addRZ(  279.0,  18650.0 )
    BeamPipe.addRZ(  436.0,  18650.0 )
    BeamPipe.addRZ(  436.0,  22030.0 )
    BeamPipe.addRZ( 1050.0,  22030.0 )
    BeamPipe.addRZ( 1050.0,  26046.0 )
    BeamPipe.addRZ(    0.0,  26046.0 )

    kwargs.setdefault("FallbackBeamPipeR"    ,  BeamPipe.getRs()        )
    kwargs.setdefault("FallbackBeamPipeZ"    ,  BeamPipe.getZs()        )


    # setup fallback IDEnvelope
    InDet = Volume()
    InDet.addRZ( 1148.,  3475. )
    InDet.addRZ(  34.3,  3475. )
    kwargs.setdefault("FallbackInDetR"  , InDet.getRs()        )
    kwargs.setdefault("FallbackInDetZ"  , InDet.getZs()        )


    # setup fallback CaloEnvelope
    Calo = Volume()
    Calo.addRZ( 1148.0,  3475.0 )
    Calo.addRZ(  120.0,  3475.0 ) # TODO - re-add HGTD stuff once we can configure 
    Calo.addRZ(  120.0,  4185.0 )
    Calo.addRZ(   41.0,  4185.0 )
    Calo.addRZ(   41.0,  6783.0 )
    Calo.addRZ(  420.0,  6783.0 )
    Calo.addRZ(  420.0,  6736.0 )
    Calo.addRZ( 3800.0,  6736.0 )
    Calo.addRZ( 3800.0,  6550.0 )
    Calo.addRZ( 4255.0,  6550.0 )

    kwargs.setdefault("FallbackCaloR"   , Calo.getRs()        )
    kwargs.setdefault("FallbackCaloZ"   , Calo.getZs()        )


    # setup fallback MuonEnvelope
    Muon = Volume()
    Muon.addRZ(  4255.0 ,  4000.0 )
    Muon.addRZ(  4255.0 ,  6550.0 )
    Muon.addRZ(  3800.0 ,  6550.0 )
    Muon.addRZ(  3800.0 ,  6736.0 )
    Muon.addRZ(   420.0 ,  6736.0 )
    Muon.addRZ(   420.0 ,  6783.0 )
    Muon.addRZ(    70.0 ,  6783.0 )
    Muon.addRZ(    70.0 , 12900.0 )
    Muon.addRZ(   279.0 , 12900.0 )
    Muon.addRZ(   279.0 , 18650.0 )
    Muon.addRZ(   436.7 , 18650.0 )
    Muon.addRZ(   436.7 , 22030.0 )
    Muon.addRZ(  1050.0 , 22030.0 )
    Muon.addRZ(  1050.0 , 26046.0 )
    Muon.addRZ(  1500.0 , 26046.0 )
    Muon.addRZ(  1500.0 , 23001.0 )
    Muon.addRZ(  2750.0 , 23001.0 )
    Muon.addRZ(  2750.0 , 22030.0 )
    Muon.addRZ( 12650.0 , 22030.0 )
    Muon.addRZ( 12650.0 , 18650.0 )
    Muon.addRZ( 13400.0 , 18650.0 )
    Muon.addRZ( 13400.0 , 12900.0 )
    Muon.addRZ( 14200.0 , 12900.0 )
    Muon.addRZ( 14200.0 ,  4000.0 )
    Muon.addRZ( 13000.0 ,  4000.0 )
    kwargs.setdefault("FallbackMuonR"   , Muon.getRs()        )
    kwargs.setdefault("FallbackMuonZ"   , Muon.getZs()        )


    # setup fallback CavernEnvelope
    Cavern = Volume()
    Cavern.addRZ( 13000.0 , 4000.0 )
    Cavern.addRZ( 14200.0 , 4000.0 )
    Cavern.addRZ( 14200.0 , 12900.0 ) # boundary with MS
    Cavern.addRZ( 13400.0 , 12900.0 ) # boundary with MS
    Cavern.addRZ( 13400.0 , 18650.0 ) # boundary with MS
    Cavern.addRZ( 12650.0 , 18650.0 ) # boundary with MS
    Cavern.addRZ( 12650.0 , 22030.0 ) # boundary with MS
    Cavern.addRZ(  2750.0 , 22030.0 ) # boundary with MS
    Cavern.addRZ(  2750.0 , 23001.0 ) # boundary with MS
    Cavern.addRZ(  1500.0 , 23001.0 ) # boundary with MS
    Cavern.addRZ(  1500.0 , 26046.0 ) # boundary with MS
    Cavern.addRZ(     0.0 , 26046.0 ) #

    # the outer dimesions differ between collision and cosmics jobs
    if flags.Beam.Type != 'cosmics':
      #  -> for collision jobs the 'cavern' envelope is much smaller
      Cavern.addRZ(      0.0 , 500000.0 ) # z= +500m
      Cavern.addRZ( 500000.0  , 500000.0 ) # r=  500m
    else:
      #  -> for cosmics simulation the 'cavern' envelope spans much further
      kwargs.setdefault("DBCavernNode"    , 'CavernEnvelopeNONE'   )
      kwargs.setdefault("EnableFallback"  , True                   )
      Cavern.addRZ(       0.0 , 3000000.0 ) # z= +3km
      Cavern.addRZ( 3000000.0 , 3000000.0 ) # r=  3km
    kwargs.setdefault("FallbackCavernR"   , Cavern.getRs()        )
    kwargs.setdefault("FallbackCavernZ"   , Cavern.getZs()        )

    from SubDetectorEnvelopes.SubDetectorEnvelopesConf import DetDescrDBEnvelopeSvc
    return DetDescrDBEnvelopeSvc(**kwargs)
    
