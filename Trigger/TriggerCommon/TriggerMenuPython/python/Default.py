# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

StreamConfig = {
    'bphysics':    [ 'bphysics' ],
    'combined':    [ 'combined',
                     # egamma
                     'comb__ej', 'comb__emet', 'comb__ejmet', 'comb__eg',
                     'comb__gmet', 
                     'comb__emu', 'comb__gmu',
                     'comb__muj', 'comb__mumet', 'comb__mujmet',
                     'comb__tauj', 'comb__taumet', 'comb__taujmet',
                     'comb__jmet',
                     'comb__taub', 'comb__taue', 
                     'comb__taue', 'comb__taumu',
                     'comb__mue', 'comb__multimu',
                     ], 
    'egamma':      [ 'egamma' ], 
    'jets':        [ 'jets' ], 
    'minbias':     [ 'minbias' ], 
    'muons':       [ 'muons' ], 
    'tauEtmiss':   [ 'tauEtmiss' ], 
    'BeamSpotTag': ['BeamSpotTag'], 
    'calibration': [ 'calibration' ], 
    'express':     [ 'express' ], 
    }

