# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
#@file     MissingETD3PDMaker/python/MissingETD3PDTriggerBitsObject.py
#@author   Jet Goodson <jgoodson@cern.ch> (copied Haifeng Li's & Scott Snyder's tool in egamma)
#@date     12 Nov, 2009
#@brief    Define trigger bit blocks for MissingET
#

from D3PDMakerCoreComps.D3PDObject  import make_Void_D3PDObject


METD3PDTriggerBitsObject = \
                         make_Void_D3PDObject (default_name = 'MissingETTriggerBitsFiller')

#
# The MET trigger bits are now added in MissingETD3PDObject;
# this file is kept just for backwards compatibility.
