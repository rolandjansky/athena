# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

############## Input: GenericGenerator ###############
def getInput_GenericCosmicGenerator(name="GenericCosmicGenerator", **kwargs):
    ## Configuring the Athena application for a 'generator' job
    from G4AtlasApps.SimFlags import simFlags
    simFlags.load_cosmics_flags()

    ## Set up standard algorithms and random seeds
    from AthenaCommon.AppMgr import ServiceMgr
    from PartPropSvc.PartPropSvcConf import PartPropSvc
    ServiceMgr += PartPropSvc()
    if not simFlags.RandomSeedList.checkForExistingSeed( "COSMICS"):
        simFlags.RandomSeedList.addSeed( "COSMICS", 2040160768, 443921183 )
    kwargs.setdefault('AtRndmGenSvc', simFlags.RandomSvc.get_Value())

    ##--------------------------------------------------------------
    ## CosmicGenerator parameters
    ##--------------------------------------------------------------
    ##
    ## Note that in this coordinate frame the y-axis points upward
    ## such that the cosmics arrive from upward to downward in y.
    ## The production vertex of cosmics is randomly distributed (flat)
    ## in the x-z plane with boundaries given below.
    ## The energy range is given as well.
    ##
    ## The following settings are tuned to scintillators of dimensions
    ## 140 x 0.5 x 100 cm^3 placed at +-115.0 cm
    ##
    ## NOTE: From G4AtlasApps-04-00-00 onwards, IDET-* cosmics
    ## commissioning layouts are not supported!
    xvert_low = -301700.
    xvert_hig =  298300.
    zvert_low = -300000.
    zvert_hig =  300000.
    
    if simFlags.CosmicPtSlice.statusOn:
        print "Configuring cosmic pT slice: %s" % simFlags.CosmicPtSlice.get_Value()
        if simFlags.CosmicPtSlice == 'slice1':
            kwargs.setdefault('emin',  10.*1000.) #  10 GeV
            kwargs.setdefault('emax', 100.*1000.) # 100 GeV
            xvert_low = -1000.*200. # 200 m
            xvert_hig =  1000.*200. # 200 m
            zvert_low = -1000.*200. # 200 m
            zvert_hig =  1000.*200. # 200 m
        elif simFlags.CosmicPtSlice == 'slice2':
            kwargs.setdefault('emin', 100.*1000.) # 100 GeV
            kwargs.setdefault('emax', 300.*1000.) # 300 GeV
            xvert_low =  -1000.*600. # 600 m
            xvert_hig =   1000.*600. # 600 m
            zvert_low =  -1000.*600. # 600 m
            zvert_hig =   1000.*600. # 600 m
        elif simFlags.CosmicPtSlice == 'slice3':
            kwargs.setdefault('emin',  300.*1000.)  #  300 GeV
            kwargs.setdefault('emax', 1000.*1000.)  # 1000 GeV
            xvert_low =  -1000.*1000. # 1 km
            xvert_hig =   1000.*1000. # 1 km
            zvert_low =  -1000.*1000. # 1 km
            zvert_hig =   1000.*1000. # 1 km
        elif simFlags.CosmicPtSlice == 'slice4':
            kwargs.setdefault('emin', 1000.*1000.) # 1 TeV
            kwargs.setdefault('emax', 5000.*1000.) # 5 TeV
            xvert_low =  -1000.*3000. # 3 km
            xvert_hig =   1000.*3000. # 3 km
            zvert_low =  -1000.*3000. # 3 km
            zvert_hig =   1000.*3000. # 3 km
        elif simFlags.CosmicPtSlice != 'NONE':
            print 'Slice name incorrect!'
            # TODO: theApp.exit(1)?
            import sys
            sys.exit(1)
        kwargs.setdefault('xvert_low', xvert_low)
        kwargs.setdefault('xvert_hig', xvert_hig)
        kwargs.setdefault('zvert_low', zvert_low)
        kwargs.setdefault('zvert_hig', zvert_hig)

        bedrockDX = (xvert_hig - xvert_low)/2.
        bedrockDZ = (zvert_hig - zvert_low)/2.

        if (bedrockDX > 350000 or bedrockDZ > 350000) :
            newSize = max( bedrockDX , bedrockDZ )
            print "Resizing bedrock (mm) to fit cosmic generator:",newSize
            from AthenaCommon.Configurable import Configurable
            if Configurable.allConfigurables.get('GeoModelSvc'):
                GeoModelSvc = Configurable.allConfigurables.get('GeoModelSvc')
            else:
                GeoModelSvc = theApp.service('GeoModelSvc')
            if (newSize <=  500000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock500'
            elif (newSize <= 1000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock1000'
            elif (newSize <= 1500000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock1500'
            elif (newSize <= 2000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock2000'
            elif (newSize <= 3000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock3000'
            elif (newSize <= 4000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock4000'
            elif (newSize <= 5000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock5000'
        else :
            print "No need to resize the bedrock for cosmic generation"

    ## Adding the CosmicGenerator to the list of algs
    from CosmicGenerator.CosmicGeneratorConf import CosmicGenerator
    algorithm = CosmicGenerator(**kwargs)
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if not hasattr(topSequence, 'CosmicGenerator'):
        topSequence += algorithm

    return algorithm

############## Input: Creating cosmics from scratch ###############
def getInput_EvgenCosmicGenerator(name="EvgenCosmicGenerator", **kwargs):
    ## Configuring the Athena application for a 'generator' job
    import AthenaCommon.AtlasUnixGeneratorJob

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.PoolEvgenInput.set_Off()

    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.CosmicFilterVolumeName.statusOn:
        print "Warning CosmicFilterVolumeName not set using default (CaloEntryLayer)"
        simFlags.CosmicFilterVolumeName = "CaloEntryLayer"
    
    kwargs.setdefault('emin', 10000)  # default =10000 #10 GeV
    kwargs.setdefault('emax', 5000*1000) # 2 TeV - FIXME?!

    if simFlags.CosmicFilterVolumeName == "Muon":
        print 'Using muon Volume setup of Cosmic Generator...'
        kwargs.setdefault('xvert_low',   -301700.)
        kwargs.setdefault('xvert_hig',    298300.)
        kwargs.setdefault('zvert_low',   -300000.)
        kwargs.setdefault('zvert_hig',    300000.)
        kwargs.setdefault('Radius',        20000.)
    else:
        print 'Using Non-muon Volume setup of Cosmic Generator...'
        kwargs.setdefault('xvert_low',   -200000.)
        kwargs.setdefault('xvert_hig',    200000.)
        kwargs.setdefault('zvert_low',   -200000.)
        kwargs.setdefault('zvert_hig',    200000.)
        kwargs.setdefault('Radius',        10000.) #barrel length ~22m


    kwargs.setdefault('yvert_val',    57300+41000.)
    kwargs.setdefault('ctcut',    0.)
    kwargs.setdefault('OptimizeForCavern', True)
    kwargs.setdefault('IPx', 0.)
    kwargs.setdefault('IPy', 0.)
    kwargs.setdefault('IPz', 0.)
    kwargs.setdefault('ReadTR', simFlags.ReadTR.statusOn)

    #special settings from Juerg Beringer
    if simFlags.CosmicFilterVolumeName == "Pixel" or simFlags.CosmicFilterVolumeName2 == "Pixel":
        kwargs.setdefault('Radius', 2000.)
        kwargs.setdefault('doPathLengthCut', True)         # Optimization based on box cut in pixel detector plane
        kwargs.setdefault('energyCutThreshold', 100.)      # - margin of error for energy loss calculation (in MeV)
        kwargs.setdefault('doAimedAtPixelsCut', True)      # Optimization based on box cut in pixel detector plane
        kwargs.setdefault('pixelplane_maxx', 1150.)        # - require |x| < value in mm
        kwargs.setdefault('pixelplane_maxz', 1650.)        # - require |y| < value in mm
        kwargs.setdefault('doReweighting', True)           # Whether to use reweighting for cosmic ray generation
        kwargs.setdefault('rvert_max', 300000.)            # - radius in mm for generating primary vertex


    #fix for bug: 49362
    import sys
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.EventSelector.EventsPerRun = int(2**31 - 1) #sys.maxint on a 32-bit machine

    return getInput_GenericCosmicGenerator(name, **kwargs)

############## Input: Reading Cosmics from TrackRecord Input File ###############
def getInput_TrackRecordCosmicGenerator(name="TrackRecordCosmicGenerator", **kwargs):
    ## Configuring the Athena application for a 'track record' job
    import G4AtlasApps.AtlasCosmicTrackRecordJob
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == "cosmics":
        kwargs.setdefault('emin', 10000)  # 10 GeV
        kwargs.setdefault('emax', 2000*1000) # 2 TeV
        kwargs.setdefault('xvert_low', -301700.)
        kwargs.setdefault('xvert_hig',  298300.)
        kwargs.setdefault('zvert_low', -300000.)
        kwargs.setdefault('zvert_hig',  300000.)
        kwargs.setdefault('Radius', 2000.0)
        kwargs.setdefault('yvert_val',  (57300. + 41000.))
        kwargs.setdefault('ctcut',  0.0)
        kwargs.setdefault('OptimizeForCavern', True)
        kwargs.setdefault('IPx', 0.0)
        kwargs.setdefault('IPy', 0.0)
        kwargs.setdefault('IPz', 0.0)

    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('ReadTR', simFlags.ReadTR.statusOn)
    kwargs.setdefault('TRSmearing', -1) #in millimeters, e.g. 10
    kwargs.setdefault('TRPSmearing', -1) #in radians, e.g. 0.01
    #kwargs.setdefault('OutputLevel', DEBUG) # for turning up output during testing
    
    return getInput_GenericCosmicGenerator(name, **kwargs)

