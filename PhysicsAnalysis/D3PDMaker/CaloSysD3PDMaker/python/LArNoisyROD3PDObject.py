# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file CaloSysD3PDMaker/python/LArNoisROD3PDObject.py
# @author Laurent Duflot <duflot at lal.in2p3.fr>
# @date Apr, 2010
# @brief D3PD object for LAr noisy FEB
#


import CaloSysD3PDMaker
from D3PDMakerCoreComps.D3PDObject     import make_SG_D3PDObject

LArNoisyROD3PDObject = \
           make_SG_D3PDObject ('LArNoisyROSummary',
#                               D3PDMakerFlags.LArNoisyROSGKey(),
                               'LArNoisyROSummary',
                               'larnro_',
                               'LArNoisyROD3PDObject',
                               default_allowMissing = True)

LArNoisyROD3PDObject.defineBlock (0,
                                  'Basic',
                                  CaloSysD3PDMaker.LArNoisyROFillerTool,
                                  SaveNB    = True,
                                  SaveFEBID = False,
                                  SavePAID  = False)

LArNoisyROD3PDObject.defineBlock (1,
                                  'FEBIDs',
                                  CaloSysD3PDMaker.LArNoisyROFillerTool,
                                  SaveNB    = False,
                                  SaveFEBID = True,
                                  SavePAID  = False)

LArNoisyROD3PDObject.defineBlock (2,
                                  'PAIDs',
                                  CaloSysD3PDMaker.LArNoisyROFillerTool,
                                  SaveNB    = False,
                                  SaveFEBID = False,
                                  SavePAID  = True)
