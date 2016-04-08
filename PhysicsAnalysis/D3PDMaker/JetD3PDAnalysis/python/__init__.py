# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import JetD3PDAnalysisConf
for k, v in JetD3PDAnalysisConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

