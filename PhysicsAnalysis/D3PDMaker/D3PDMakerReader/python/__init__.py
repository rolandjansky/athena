# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file D3PDMakerReader/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2010
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   D3PDMakerReader.D3PDMakerReaderConf.D3PD__ReaderAlg
#
# we can write
#
#   D3PDMakerReader.ReaderAlg
#

import D3PDMakerReaderConf
for k, v in D3PDMakerReaderConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

# Bring the reader algorithms into this scope as well.
from ReaderAlg import ReaderAlg             # noqa: F401
from MultiReaderAlg import MultiReaderAlg   # noqa: F401
