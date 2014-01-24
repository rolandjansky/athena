# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: __init__.py 470583 2011-11-25 10:33:45Z krasznaa $

import ForwardDetectorsD3PDMakerConf
for k, v in ForwardDetectorsD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v
