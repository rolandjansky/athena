# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
SubDetectorEnvelopes configurations for AtlasGeometryCommon
Elmar Ritsch, 27/09/2013
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, LHCPeriod


def EnvelopeDefSvcCfg(ConfigFlags, name="AtlasGeometry_EnvelopeDefSvc", **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("DBBeamPipeNode", 'BeamPipeEnvelope')
    kwargs.setdefault("DBInDetNode"   , 'InDetEnvelope'
                                        if ConfigFlags.GeoModel.Run in [LHCPeriod.Run1, LHCPeriod.Run2, LHCPeriod.Run3]
                                        else 'ITkEnvelope')
    kwargs.setdefault("DBCaloNode"    , 'CaloEnvelope'    )
    kwargs.setdefault("DBMSNode"      , 'MuonEnvelope'    )
    kwargs.setdefault("DBCavernNode"  , 'CavernEnvelope'  )

    # allow for python-based envelope definitions (see below)
    #  -> TODO: set this to False once the envelope definitions are tagged in the DDDB
    kwargs.setdefault("EnableFallback"  , True             )

    # setup fallback BeamPipeEnvelope
    BeamPipe = Volume()

    if ConfigFlags.GeoModel.Run not in [LHCPeriod.Run1, LHCPeriod.Run2, LHCPeriod.Run3]:
        BeamPipe.addRZ(   28.8,   3545.0 )
        BeamPipe.addRZ(  120.0,   3545.0 )
    else:
        BeamPipe.addRZ(   34.3,   3475.0 )
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
    # InDet should include HGTD when it's turned on
    if ConfigFlags.GeoModel.Run not in [LHCPeriod.Run1, LHCPeriod.Run2, LHCPeriod.Run3]:
        InDet.addRZ( 1148.,  3545. )
        InDet.addRZ(  28.8,  3545. )
    else:
        InDet.addRZ( 1148.,  3475. )
        InDet.addRZ(  34.3,  3475. )
    kwargs.setdefault("FallbackInDetR"  , InDet.getRs()        )
    kwargs.setdefault("FallbackInDetZ"  , InDet.getZs()        )


    # setup fallback CaloEnvelope
    Calo = Volume()

    if ConfigFlags.GeoModel.Run not in [LHCPeriod.Run1, LHCPeriod.Run2, LHCPeriod.Run3]:
        Calo.addRZ( 1148.0,  3545.0 )
        Calo.addRZ(  120.0,  3545.0 )
    else:
        Calo.addRZ( 1148.0,  3475.0 )
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
    if ConfigFlags.Beam.Type is not BeamType.Cosmics:
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

    #set the GeoModelSvc
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(ConfigFlags)
    kwargs.setdefault("GeoModelSvc", acc.getService("GeoModelSvc"))
    result.merge(acc)

    result.addService(CompFactory.DetDescrDBEnvelopeSvc(name, **kwargs),primary=True)
    return result


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
