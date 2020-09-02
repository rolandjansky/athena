# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##############################
# L2 muCombMT Fex Algorithm Configuration:
# Stefano Giagu <stefano.giagu@cern.ch>
##############################


from TrigmuComb.TrigmuCombConf import muCombMT
from AthenaCommon.SystemOfUnits import GeV
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.GlobalFlags import globalflags
from TrigmuComb.TrigmuCombMonitoring import TrigMuCombMonitoring

class TrigmuCombMTConfig (muCombMT):
    __slots__ = []
    def __new__( cls, *args, **kwargs ):
      newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1]) ] + list(args)
      if len(args) == 3:
        if args[2] == 'OLDextr':
          newargs = ['%s_%s_%s_%s' % (cls.getType(),args[0],args[1],args[2]) ] + list(args)
      return super( TrigmuCombMTConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
      super( TrigmuCombMTConfig, self ).__init__( name )

      self.MuCombStrategy = 0

      self.UseBackExtrapolatorG4 = True
      if len(args) == 3:
        if args[2] == 'OLDextr':
          self.UseBackExtrapolatorG4 = False

      self.MinPtTRK = 0.* GeV

      # matchign windows parameters tuned in 2016 and 2015 data (T&P Z  and J/psi samples)
      self.WinEtaSigma_g4 = 7.0
      self.WinPhiSigma_g4 = 7.0
      self.Chi2Weight_g4  = 2.0
      if TriggerFlags.run2Config=='2016':
        self.WinEtaSigma_g4 = 4.0
        self.WinPhiSigma_g4 = 4.0
        self.Chi2Weight_g4  = 1.0

      # pt resolution parameters for ID-SAmuon match from SAmuon developers (2016 data)
      if globalflags.DetDescrVersion().find('CSC')!=-1:
        self.IDSCANBarrelRes  = [0.02169,0.0004186]
        self.IDSCANEndcap1Res = [0.03054,0.000325]
        self.IDSCANEndcap2Res = [0.03557,0.0005383]
        self.IDSCANEndcap3Res = [0.04755,0.0007718]
        self.IDSCANEndcap4Res = [0.06035,0.0001145]

      self.MonTool = TrigMuCombMonitoring()

