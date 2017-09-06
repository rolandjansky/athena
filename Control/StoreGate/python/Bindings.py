# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: StoreGate/python/Bindings.py
# Author: Wim Lavrijsen <WLavrijsen@lbl.gov>

### data
__version__ = '2.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

import warnings,os
msg = "'StoreGate.Bindings' module has been retired ! " + \
      "Please use StoreGateBindings.Bindings instead:"  + \
      "   'import StoreGateBindings.Bindings'"
warnings.warn(msg, DeprecationWarning, stacklevel=2)

### pythonization for StoreGateSvc
import StoreGateBindings.Bindings

### enable proper redirection...
import sys
sys.modules['StoreGate.Bindings'] = sys.modules['StoreGateBindings.Bindings']
