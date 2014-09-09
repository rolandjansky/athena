# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: persTypeToTransType.py,v 1.4 2007-12-04 18:56:38 ssnyder Exp $
#
# @file AthenaROOTAccess/python/persTypeToTransType.py
# @author sss
# @date Jul 2007
# @brief Translate from a persistent type to a transient type
#        by looking at the list of available converters.
#

import ROOT


#
# Given the name PERS of a persistent type, return the name
# of the corresponding transient type.  If no match, just return
# the persistent type name.
#
def persTypeToTransType (pers):
    out = ROOT.AthenaROOTAccess.TPCnv.Registry.instance().p2t_name (pers)
    if not out:
        return pers
    return out

