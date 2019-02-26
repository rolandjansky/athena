# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags

class CaloDef:

    @staticmethod
    def defineGlobalSettings(tc, tag=None):

        
        from TriggerMenu.l1.CaloInfo import MinimumTOBPt, CaloInfo

        caloInfo = tc.menu.CaloInfo
        menuName = tc.menuName
        # global scale
        em_scale=2
        if hasattr(TriggerFlags, 'useRun1CaloEnergyScale'):
            if TriggerFlags.useRun1CaloEnergyScale :
                em_scale=1

        caloInfo.setGlobalEmScale(em_scale)
        caloInfo.setGlobalJetScale(1)

        # XS parameterization
        # updated by ATR-10664
        caloInfo.setXsParams( xsSigmaScale=1264, xsSigmaOffset=1496, xeMin=11, xeMax=63, teSqrtMin=4, teSqrtMax=63 )
        #caloInfo.setXsParams( xsSigmaScale=660, xsSigmaOffset=1561, xeMin=11, xeMax=63, teSqrtMin=4, teSqrtMax=63 )
        #caloInfo.setXsParams( xsSigmaScale=1150, xsSigmaOffset=1640, xeMin=11, xeMax=63, teSqrtMin=4, teSqrtMax=63 )

        # isolation parametrization
        """
        (from Alan Watson)
        Isolation <= ET/m + offset
        where
        offset: signed integer 100 MeV precision (offset = 25 means 2.5 GeV)
        slope: signed integer for a slope of m the isolation cut loosens by 1 GeV for every m GeV increase in the cluster ET
        
        E.g: with offset=-52 slope=35 means isolation <= ET/3.5 - 5.2


        The following defaults are applied if not specified
        mincut     : 0,
        upperlimit : 255,
        etamin     : -49,
        etamax     : 49,
        priority   : 0
        """


        # for unused bits, fill with the loosest existing option (ATR-13892)
        caloInfo.isolation["EMIsoForEMthr"] .addIsolation( isobit=1, slope=0, offset=0, upperlimit=0)\
                                            .addIsolation( isobit=2, slope=80, offset=-18, mincut=20, upperlimit=50)\
                                            .addIsolation( isobit=3, slope=80, offset=-18, mincut=20, upperlimit=50)\
                                            .addIsolation( isobit=4, slope=80, offset=-20, mincut=10, upperlimit=50)\
                                            .addIsolation( isobit=5, slope=80, offset=-18, mincut=15, upperlimit=50)

        caloInfo.isolation["HAIsoForEMthr"] .addIsolation( isobit=1, slope=230, offset=-2, mincut=10, upperlimit=50)\
                                            .addIsolation( isobit=2, slope=0, offset=0, upperlimit=0)\
                                            .addIsolation( isobit=3, slope=230, offset=-2, mincut=10, upperlimit=50)\
                                            .addIsolation( isobit=4, slope=230, offset=-2, mincut=10, upperlimit=50)\
                                            .addIsolation( isobit=5, slope=230, offset=-2, mincut=10, upperlimit=50)

        caloInfo.isolation["EMIsoForTAUthr"] .addIsolation( isobit=1, slope=100, offset=30, upperlimit=60 )\
                                             .addIsolation( isobit=2, slope=100, offset=20, upperlimit=60 )\
                                             .addIsolation( isobit=3, slope=100, offset=15, upperlimit=60 )\
                                             .addIsolation( isobit=4, slope=0, offset=40, upperlimit=124 )\
                                             .addIsolation( isobit=5, slope=100, offset=30, upperlimit=60 )



        # min PT for TOBs
        if "HI_" in menuName:
            caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="EM", ptmin=7) ]
            caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="TAU", ptmin=1) ]
            caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="JETS", ptmin=12, window=4) ]
            caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="JETL", ptmin=12, window=8) ]
        else:
            caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="EM", ptmin=3) ]
            caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="TAU", ptmin=5) ]
            caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="JETS", ptmin=12, window=4) ]
            caloInfo.minTOBPt += [ MinimumTOBPt(thrtype="JETL", ptmin=12, window=8) ]
            
        # jet weights
        if len( tc.menu.thresholds.allThresholdsOf('JET') ) <= 8:
            caloInfo.setJetWeights( CaloInfo.calcJetWeightsFromThresholds(tc) )  # no more jet weights in the new menu

