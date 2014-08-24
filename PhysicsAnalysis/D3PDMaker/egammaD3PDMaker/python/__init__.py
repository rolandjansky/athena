# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import egammaD3PDMakerConf
for k, v in egammaD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

