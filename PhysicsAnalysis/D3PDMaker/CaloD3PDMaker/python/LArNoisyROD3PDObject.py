# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/LArNoisROD3PDObject.py
# @author Laurent Duflot <duflot at lal.in2p3.fr>
# @date Apr, 2010
# @brief D3PD object for LAr noisy FEB
#


import CaloD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags    import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject     import make_SG_D3PDObject
from RecExConfig.ObjKeyStore           import cfgKeyStore

LArNoisyROD3PDObject = \
           make_SG_D3PDObject ('LArNoisyROSummary',
#                               D3PDMakerFlags.LArNoisyROSGKey(),
                               'LArNoisyROSummary',
                               'larnro_',
                               'LArNoisyROD3PDObject',
                               default_allowMissing = True)

LArNoisyROD3PDObject.defineBlock (0,
                                  'Basic',
                                  CaloD3PDMaker.LArNoisyROFillerTool,
                                  SaveNB    = True,
                                  SaveFEBID = False,
                                  SavePAID  = False)

LArNoisyROD3PDObject.defineBlock (1,
                                  'FEBIDs',
                                  CaloD3PDMaker.LArNoisyROFillerTool,
                                  SaveNB    = False,
                                  SaveFEBID = True,
                                  SavePAID  = False)

LArNoisyROD3PDObject.defineBlock (2,
                                  'PAIDs',
                                  CaloD3PDMaker.LArNoisyROFillerTool,
                                  SaveNB    = False,
                                  SaveFEBID = False,
                                  SavePAID  = True)
