# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ExoticsD3PDMakerConf
for k, v in ExoticsD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v
