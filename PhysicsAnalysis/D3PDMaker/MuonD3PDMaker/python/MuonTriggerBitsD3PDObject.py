# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file  MuonD3PDMaker/python/MuonTriggerBitsD3PDObject.py
# @author  Srivas Prasad <srivas.prasad@cern.ch>
# @date    Jan, 2010
# @brief   Define muon trigger bit blocks



from D3PDMakerCoreComps.D3PDObject  import make_Void_D3PDObject


MuonTriggerBitsD3PDObject = \
  make_Void_D3PDObject (default_name = 'MuonTriggerBitsFiller')


#
# The muon trigger bits are now added in MuonD3PDObject;
# this file is kept just for backwards compatibility.
#
