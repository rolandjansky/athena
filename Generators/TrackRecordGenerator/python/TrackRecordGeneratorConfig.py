# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

############## Input: Reading Particles from TrackRecord Input File ###############
def getInput_TrackRecordGenerator(name="TrackRecordGenerator", **kwargs):
    ## Configuring the Athena application for a 'track record' job
    import G4AtlasApps.AtlasCosmicTrackRecordJob  # noqa: F401
    kwargs.setdefault('TRCollection','CosmicRecord') # Collection name
    kwargs.setdefault('TRSmearing', -1) #in millimeters, e.g. 10
    kwargs.setdefault('TRPSmearing', -1) #in radians, e.g. 0.01
    kwargs.setdefault('StopParticles', False) # Stop the particles or not
    kwargs.setdefault('stopped_tminus', -25.) # in ns, lower bound
    kwargs.setdefault('stopped_tplus', 25.) # in ns, upper bound
    #kwargs.setdefault('OutputLevel', DEBUG) # for turning up output during testing

    ## Adding the TrackRecordGenerator to the list of algs
    from TrackRecordGenerator.TrackRecordGeneratorConf import TrackRecordGenerator
    algorithm = TrackRecordGenerator(**kwargs)
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if not hasattr(topSequence, 'TrackRecordGenerator'):
        topSequence += algorithm

    return algorithm

def getTrackRecordCosmicGenerator(name="TrackRecordCosmicGenerator", **kwargs):
    return getInput_TrackRecordGenerator(name, **kwargs)
