# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

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

    def recalculate(self, flags):
        if flags.Sim.CosmicPtSlice == 'Off':
            if 'Muon' in flags.Sim.CosmicFilterVolumeNames:
                self.xvert_low  = -301700.
                self.xvert_high =  298300.
                self.zvert_low  = -1000.*300.
                self.zvert_high =  1000.*300.
                self.radius     =  20000.
            if 'Pixel' in flags.Sim.CosmicFilterVolumeNames:
                self.radius     =  2000.
        else:
            n_value = flags.Sim.CosmicPtSlice
            self.xvert_low  = self.xvert_lowDict[n_value]
            self.xvert_high = self.xvert_highDict[n_value]
            self.zvert_low  = self.zvert_lowDict[n_value]
            self.zvert_high = self.zvert_highDict[n_value]
            #self.radius     =
            self.emin       = self.EminDict[n_value]
            self.emax       = self.EmaxDict[n_value]

    def CosmicEmin(self, flags):
        """
        """
        self.recalculate(flags)
        return self.emin


    def CosmicEmax(self, flags):
        """
        """
        self.recalculate(flags)
        return self.emax


    def CosmicLowVertex_X(self, flags):
        """
        """
        self.recalculate(flags)
        return self.xvert_low


    def CosmicHighVertex_X(self, flags):
        """
        """
        self.recalculate(flags)
        return self.xvert_high


    def CosmicLowVertex_Z(self, flags):
        """
        """
        self.recalculate(flags)
        return self.zvert_low


    def CosmicHighVertex_Z(self, flags):
        """
        """
        self.recalculate(flags)
        return self.zvert_high


    def CosmicRadius(self, flags):
        """
        """
        self.recalculate(flags)
        return self.radius

    def BedrockDX(self, flags):
        """
        """
        self.recalculate(flags)
        return (self.xvert_high - self.xvert_low)

    def BedrockDZ(self, flags):
        """
        """
        self.recalculate(flags)
        return (self.zvert_high - self.zvert_low)

    def reconfigureCavernGeometry(self, flags, cfg):
        """
        Note that in this coordinate frame the y-axis points upward
        such that the cosmics arrive from upward to downward in y.
        """
        newSize = max( self.BedrockDX(flags) , self.BedrockDZ(flags) )
        if (newSize > 350000.):
            print ("Resizing bedrock (mm) to fit cosmic generator:",newSize)
            GeoModelSvc = cfg.getService('GeoModelSvc')
            if (newSize <=  500000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock500'
            elif (newSize <= 1000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock1000'
            elif (newSize <= 1500000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock1500'
            elif (newSize <= 2000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock2000'
            elif (newSize <= 3000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock3000'
            elif (newSize <= 4000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock4000'
            elif (newSize <= 5000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-03-Bedrock5000'
        else :
            print ("No need to resize the bedrock for cosmic generation")


############## Input: CosmicGenerator ###############
def CosmicGeneratorCfg(flags, name="CosmicGenerator", **kwargs):
    ## Configuring the Athena application for a 'generator' job
    result = ComponentAccumulator()
    result.addService(CompFactory.PartPropSvc(InputFile="PDGTABLE.MeV"))

    ## Set up random seeds FIXME
    seed = 'COSMICS OFFSET 1234 2040160768 443921183'
    from RngComps.RandomServices import dSFMT
    kwargs.setdefault('AtRndmGenSvc', result.getPrimaryAndMerge(dSFMT(seed)).name)

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

    kwargs.setdefault('emin', theCavern.CosmicEmin(flags))
    kwargs.setdefault('emax', theCavern.CosmicEmax(flags))
    #kwargs.setdefault('emin', 10000)  # default =10000 #10 GeV
    #kwargs.setdefault('emax', 5000*1000) # 2 TeV - FIXME?!

    kwargs.setdefault('xvert_low', theCavern.CosmicLowVertex_X(flags) )
    kwargs.setdefault('xvert_hig', theCavern.CosmicHighVertex_X(flags))
    kwargs.setdefault('zvert_low', theCavern.CosmicLowVertex_Z(flags) )
    kwargs.setdefault('zvert_hig', theCavern.CosmicHighVertex_Z(flags))
    kwargs.setdefault('Radius',    theCavern.CosmicRadius(flags))
    kwargs.setdefault('yvert_val', 57300.+41000.)
    kwargs.setdefault('ctcut',    0.)
    kwargs.setdefault('OptimizeForCavern', True)
    kwargs.setdefault('IPx', 0.)
    kwargs.setdefault('IPy', 0.)
    kwargs.setdefault('IPz', 0.)

    if len(flags.Sim.CosmicFilterVolumeNames)>0:
        print ('Using %s Volume setup of Cosmic Generator...' % flags.Sim.CosmicFilterVolumeNames[0])
    #special settings from Juerg Beringer
    if 'Pixel' in flags.Sim.CosmicFilterVolumeNames:
        kwargs.setdefault('doPathLengthCut', True)         # Optimization based on box cut in pixel detector plane
        kwargs.setdefault('energyCutThreshold', 100.)      # - margin of error for energy loss calculation (in MeV)
        kwargs.setdefault('doAimedAtPixelsCut', True)      # Optimization based on box cut in pixel detector plane
        kwargs.setdefault('pixelplane_maxx', 1150.)        # - require |x| < value in mm
        kwargs.setdefault('pixelplane_maxz', 1650.)        # - require |y| < value in mm
        kwargs.setdefault('doReweighting', True)           # Whether to use reweighting for cosmic ray generation
        kwargs.setdefault('rvert_max', 300000.)            # - radius in mm for generating primary vertex

    result.addEventAlgo(CompFactory.CosmicGenerator('CosmicGenerator', **kwargs))
    return result


def postIncludeCosmicGenerator(flags, cfg):
    theCavern = CavernPropertyCalculator()
    if flags.Sim.CosmicPtSlice != 'NONE':
        print ("Configuring cosmic pT slice: %s" % flags.Sim.CosmicPtSlice)
        theCavern.reconfigureCavernGeometry(flags,cfg)
