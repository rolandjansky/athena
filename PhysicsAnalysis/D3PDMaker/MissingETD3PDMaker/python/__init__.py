# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from . import MissingETD3PDMakerConf
for k, v in MissingETD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

