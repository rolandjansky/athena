# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################################
# @file:   Utils.py
# @brief:  Python utilties for TrigValTools scripts
# @author: Frank Winklmeier
#
# $Id: Utils.py,v 1.1 2008-07-28 11:47:34 fwinkl Exp $
######################################################################

import os

def findInstallDir( dir="TrigValTools",
                    searchpath="DATAPATH" ):
   """Find the installation area of the package"""

   if not os.environ.has_key(searchpath): return None

   dirs = os.environ[searchpath].split(":")
   if not dirs: return None
   for d in dirs:
      path = os.path.join(d,dir)
      if os.path.exists(path): return path

   return None

   
   
