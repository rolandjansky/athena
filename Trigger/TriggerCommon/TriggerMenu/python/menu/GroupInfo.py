# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.GroupInfo' )

AllowedGroups = ['Muon',
                 'Jet',
                 'BJet',
                 'BJet_combined',
                 'MET',
                 'Tau',
                 'ElectronTau',
                 'MuonTau',
                 'Egamma',
                 'Bphys',
                 'BeamSpot',
                 'AFP',
                 'MinBias',
                 'Detector',
                 'Other',
                 'DISCARD',
                 'ID',
                 'TauOverlay',
                 'ZeroBias',
                 'MinBiasOverlay',
                 ]



AllowedGroup_HI = ['UltraCentral',
                   'EventShape',
                   'UPC',
                   'MinBiasOverlay',
                   'SingleMuon',
                   'SingleElectron',
                   'PC',
                   'PCpeb',
                   'CC',
                   'CCpeb',
                   ]

def getAllAllowedGroups(menu):    
    if 'pp_v6' in menu or 'pp_v7' in menu or 'pp_v8' in menu or 'PhaseII' in menu:
        return AllowedGroups
    elif 'HI' in menu:
        return AllowedGroups+AllowedGroup_HI
    else:
        log.error("No list of allowed group for "+menu)
    
