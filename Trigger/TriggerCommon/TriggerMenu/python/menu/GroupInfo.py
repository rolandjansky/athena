# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.GroupInfo' )

AllowedGroups = ['Muon',
                 'Jet',
                 'BJet',
                 'MET',
                 'Tau',
                 'Egamma',
                 'Bphys',
                 'BeamSpot',
                 'MinBias',
                 'Detector',
                 'Other',
                 'DISCARD',
                 'ID',
                 'TauOverlay',
                 'ZeroBias',
                 ]



AllowedGroup_HI = ['UltraCentral',
                   'EventShape',
                   'UPC',
                   'MinBiasOverlay',
                   'SingleMuon',
                   'SingleElectron',
                   ]

def getAllAllowedGroups(menu):    
    if 'pp_v6' in menu or 'pp_v7' in menu:
        return AllowedGroups
    elif 'HI' in menu:
        return AllowedGroups+AllowedGroup_HI
    else:
        log.error("No list of allowed groupd for "+menu)
    
