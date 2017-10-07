
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class CavernPropertyCalculator(object):

    xvert_low =  -1000.*200. # 200 m
    xvert_high =  1000.*200. # 200 m
    zvert_low =  -1000.*200. # 200 m
    zvert_high =  1000.*200. # 200 m
    radius =      10000.#barrel length ~22m
    emin  =       10.*1000. # 10 GeV
    emax  =      100.*1000. # 100 GeV
    EminDict = {'slice1':   10.*1000., #  10 GeV
                'slice2':  100.*1000., # 100 GeV
                'slice3':  300.*1000., # 300 GeV
                'slice4': 1000.*1000., #   1 TeV
                'NONE':  10.*1000. }
    EmaxDict = {'slice1':  100.*1000., # 100 GeV
                'slice2':  300.*1000., # 300 GeV
                'slice3': 1000.*1000., #   1 TeV
                'slice4': 5000.*1000., #   5 TeV
                'NONE': 5000.*1000. }
    xvert_lowDict = {'slice1':  -1000.*200., # 200 m
                     'slice2':  -1000.*600., # 600 m
                     'slice3': -1000.*1000., #   1 km
                     'slice4': -1000.*3000., #   3 km
                     'NONE':  -200000. }
    xvert_highDict = {'slice1':  1000.*200., # 200 m
                      'slice2':  1000.*600., # 600 m
                      'slice3': 1000.*1000., #   1 km
                      'slice4': 1000.*3000., #   3 km
                      'NONE': 200000. }
    zvert_lowDict = {'slice1':  -1000.*200., # 200 m
                     'slice2':  -1000.*600., # 600 m
                     'slice3': -1000.*1000., #   1 km
                     'slice4': -1000.*3000., #   3 km
                     'NONE': -200000. }
    zvert_highDict = {'slice1':  1000.*200., # 200 m
                      'slice2':  1000.*600., # 600 m
                      'slice3': 1000.*1000., #   1 km
                      'slice4': 1000.*3000., #   3 km
                      'NONE': 200000. }

    def recalculate(self):
        from G4AtlasApps.SimFlags import simFlags
        if not simFlags.CosmicPtSlice.statusOn:
            if simFlags.CosmicFilterVolumeName.statusOn and simFlags.CosmicFilterVolumeName() == 'Muon':
                self.xvert_low  = -301700.
                self.xvert_high =  298300.
                self.zvert_low  = -1000.*300.
                self.zvert_high =  1000.*300.
                self.radius     =  20000.
            if (simFlags.CosmicFilterVolumeName.statusOn and simFlags.CosmicFilterVolumeName == "Pixel") or (simFlags.CosmicFilterVolumeName2.statusOn and simFlags.CosmicFilterVolumeName2 == "Pixel"):
                self.radius     =  2000.
        else:
            n_value = simFlags.CosmicPtSlice.get_Value()
            self.xvert_low  = self.xvert_lowDict[n_value]
            self.xvert_high = self.xvert_highDict[n_value]
            self.zvert_low  = self.zvert_lowDict[n_value]
            self.zvert_high = self.zvert_highDict[n_value]
            #self.radius     =
            self.emin       = self.EminDict[n_value]
            self.emax       = self.EmaxDict[n_value]

    def CosmicEmin(self):
        """
        """
        self.recalculate()
        return self.emin


    def CosmicEmax(self):
        """
        """
        self.recalculate()
        return self.emax


    def CosmicLowVertex_X(self):
        """
        """
        self.recalculate()
        return self.xvert_low


    def CosmicHighVertex_X(self):
        """
        """
        self.recalculate()
        return self.xvert_high


    def CosmicLowVertex_Z(self):
        """
        """
        self.recalculate()
        return self.zvert_low


    def CosmicHighVertex_Z(self):
        """
        """
        self.recalculate()
        return self.zvert_high


    def CosmicRadius(self):
        """
        """
        self.recalculate()
        return self.radius

    def BedrockDX(self):
        """
        """
        self.recalculate()
        return (self.xvert_high - self.xvert_low)

    def BedrockDZ(self):
        """
        """
        self.recalculate()
        return (self.zvert_high - self.zvert_low)

    def reconfigureCavernGeometry(self):
        """
        Note that in this coordinate frame the y-axis points upward
        such that the cosmics arrive from upward to downward in y.
        """
        newSize = max( self.BedrockDX() , self.BedrockDZ() )
        if (newSize > 350000.):
            print "Resizing bedrock (mm) to fit cosmic generator:",newSize
            from AthenaCommon.Configurable import Configurable
            if Configurable.allConfigurables.get('GeoModelSvc'):
                GeoModelSvc = Configurable.allConfigurables.get('GeoModelSvc')
            else:
                from AthenaCommon.AppMgr import theApp
                GeoModelSvc = theApp.service('GeoModelSvc')
            if (newSize <=  500000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock500'
            elif (newSize <= 1000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock1000'
            elif (newSize <= 1500000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock1500'
            elif (newSize <= 2000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock2000'
            elif (newSize <= 3000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock3000'
            elif (newSize <= 4000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock4000'
            elif (newSize <= 5000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock5000'
        else :
            print "No need to resize the bedrock for cosmic generation"


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

    from CosmicGenerator.CosmicGeneratorConfig import CavernPropertyCalculator
    theCavern = CavernPropertyCalculator()

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

    kwargs.setdefault('emin', theCavern.CosmicEmin())
    kwargs.setdefault('emax', theCavern.CosmicEmax())
    #kwargs.setdefault('emin', 10000)  # default =10000 #10 GeV
    #kwargs.setdefault('emax', 5000*1000) # 2 TeV - FIXME?!

    kwargs.setdefault('xvert_low', theCavern.CosmicLowVertex_X() )
    kwargs.setdefault('xvert_hig', theCavern.CosmicHighVertex_X())
    kwargs.setdefault('zvert_low', theCavern.CosmicLowVertex_Z() )
    kwargs.setdefault('zvert_hig', theCavern.CosmicHighVertex_Z())
    kwargs.setdefault('Radius',    theCavern.CosmicRadius())
    kwargs.setdefault('yvert_val', 57300.+41000.)
    kwargs.setdefault('ctcut',    0.)
    kwargs.setdefault('OptimizeForCavern', True)
    kwargs.setdefault('IPx', 0.)
    kwargs.setdefault('IPy', 0.)
    kwargs.setdefault('IPz', 0.)

    if simFlags.CosmicFilterVolumeName.statusOn:
        print 'Using %s Volume setup of Cosmic Generator...' % simFlags.CosmicFilterVolumeName.get_Value()
    #special settings from Juerg Beringer
    if simFlags.CosmicFilterVolumeName == "Pixel" or simFlags.CosmicFilterVolumeName2 == "Pixel":
        kwargs.setdefault('doPathLengthCut', True)         # Optimization based on box cut in pixel detector plane
        kwargs.setdefault('energyCutThreshold', 100.)      # - margin of error for energy loss calculation (in MeV)
        kwargs.setdefault('doAimedAtPixelsCut', True)      # Optimization based on box cut in pixel detector plane
        kwargs.setdefault('pixelplane_maxx', 1150.)        # - require |x| < value in mm
        kwargs.setdefault('pixelplane_maxz', 1650.)        # - require |y| < value in mm
        kwargs.setdefault('doReweighting', True)           # Whether to use reweighting for cosmic ray generation
        kwargs.setdefault('rvert_max', 300000.)            # - radius in mm for generating primary vertex

    if simFlags.CosmicPtSlice.statusOn and simFlags.CosmicPtSlice() is not 'NONE':
        print "Configuring cosmic pT slice: %s" % simFlags.CosmicPtSlice.get_Value()
        theCavern.reconfigureCavernGeometry()

    from AthenaCommon import CfgMgr
    algorithm = CfgMgr.CosmicGenerator(**kwargs)
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

    #fix for bug: 49362
    import sys
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.EventSelector.EventsPerRun = int(2**31 - 1) #sys.maxint on a 32-bit machine

    return getInput_GenericCosmicGenerator(name, **kwargs)

