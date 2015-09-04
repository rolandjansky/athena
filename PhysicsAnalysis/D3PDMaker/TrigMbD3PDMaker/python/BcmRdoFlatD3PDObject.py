# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TrigMbD3PDMaker/python/BcmRdoFlatD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Oct, 2010
# @brief Fill from the BCM container,
#        flattening out the intermediate collections.
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.ContainedMultiAssociation import \
     ContainedMultiAssociation
import TrigMbD3PDMaker

BcmRdoFlatD3PDObject = make_SGDataVector_D3PDObject ('BCM_RDO_Container',
                                                     'BCM_RDOs',
                                                     'bcmRDO_',
                                                     'BcmRdoFlatD3PDObject')
BcmRdoRawDataAssoc = \
  ContainedMultiAssociation (BcmRdoFlatD3PDObject,
                             TrigMbD3PDMaker.BCM_RDO_CollectionRawDataAssocTool)

BcmRdoRawDataAssoc.defineBlock (0, 'BCM_RDO',
                                TrigMbD3PDMaker.BCM_RawDataFillerTool)
