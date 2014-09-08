# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



class CaloDef:

    @staticmethod
    def defineGlobalSettings(tc, tag=None):

        
        from TriggerMenu.l1.CaloInfo import MinimumTOBPt, CaloInfo
        from TriggerMenu.l1.Limits import CaloLimits, Limits
        from TriggerMenu.l1.Lvl1Flags import Lvl1Flags


        run1 = Lvl1Flags.CTPVersion()<=3


        caloInfo = tc.menu.CaloInfo

        # global scale
        caloInfo.setGlobalScale(1.0)

        # XS parameterization
        caloInfo.setXsParams( xsSigmaScale=1150, xsSigmaOffset=1640, xeMin=11, xeMax=63, teSqrtMin=4, teSqrtMax=63 )

        # isolation parametrization
        """
        (from Alan Watson)
        Isolation <= ET/m + offset
        where
        offset: signed integer 100 MeV precision (offset = 25 means 2.5 GeV)
        slope: signed integer for a slope of m the isolation cut loosens by 1 GeV for every m GeV increase in the cluster ET
        
        E.g: with offset=-52 slope=35 means isolation <= ET/3.5 - 5.2
        """

        caloInfo.isolation["EMIsoForEMthr"] .addIsolation( isobit=1, slope=0, offset=2 )\
                                            .addIsolation( isobit=2, slope=3.5, offset=-5.2 )

        caloInfo.isolation["HAIsoForEMthr"] .addIsolation( isobit=1, slope=0, offset=1 )

        caloInfo.isolation["EMIsoForTAUthr"].addIsolation( isobit=1, slope=0, offset=1 )


        # min PT for TOBs
        caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="EM", ptmin=4) ]
        caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="TAU", ptmin=4) ]
        caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="JETS", ptmin=8, window=4) ]
        caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="JETL", ptmin=12, window=8) ]

        # jet weights
        if len( tc.menu.thresholds.allThresholdsOf('JET') ) <= 8:
            caloInfo.setJetWeights( CaloInfo.calcJetWeightsFromThresholds(tc) )  # no more jet weights in the new menu

