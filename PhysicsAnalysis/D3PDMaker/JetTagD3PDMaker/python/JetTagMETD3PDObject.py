# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagMETD3PDObject.py
## @brief MET D3PD object configured for the btagging D3PD
## @author Georges Aad
## @date Nov, 2010
##
import re

from MissingETD3PDMaker.MissingETD3PDObject import RefFinalMETD3PDObject

#
## get the METD3PDObject
## specify the MET sgkey you want
## multiple call to add different MET objects
#

def getJetTagMETD3PDObject(sgkey, level=0, exclude=['MET_Regions'], **kw):

    prefix = re.sub('_','',sgkey)
    prefix = prefix.lower()
    prefix = prefix+"_"

    return RefFinalMETD3PDObject(level,sgkey=sgkey, 
                                 prefix=prefix,
                                 exclude=exclude,
                                 **kw)

####MET_TopoObj
