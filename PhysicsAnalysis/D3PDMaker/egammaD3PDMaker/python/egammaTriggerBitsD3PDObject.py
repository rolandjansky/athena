# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file  egammaD3PDMaker/python/egammaTriggerBitsD3PDObject.py
# @author  Haifeng Li <Haifeng.Li@cern.ch>, sss
# @date    Sep, 2009
# @brief   Define trigger bit blocks for egamma.
#


import D3PDMakerCoreComps
import TriggerD3PDMaker
from D3PDMakerCoreComps.D3PDObject  import make_Void_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags


# Create the object type.
egammaTriggerBitsD3PDObject = \
  make_Void_D3PDObject (default_name = 'egammaTriggerBitsFiller')


#
# The egamma trigger bits are now added in ElectronD3PDObject and
# PhotonD3PDObject; this file is kept just for backwards compatibility.
#
