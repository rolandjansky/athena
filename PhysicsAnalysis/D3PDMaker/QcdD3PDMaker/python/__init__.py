# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import QcdD3PDMakerConf
for k, v in QcdD3PDMakerConf.__dict__.items():
    if k.startswith ('QcdD3PD__'):
        globals()[k[9:]] = v

