# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from . import MuonD3PDMakerConf
for k, v in MuonD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

