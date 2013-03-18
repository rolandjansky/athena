# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file L1CaloD3PDMaker/python/L1CaloD3PDProblemChannel.py
# @author John Morris <john.morris@cern.ch>
# @date Aptil 2010
# @brief Construct a L1Calo Problem Channel D3PD

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.DoTrigger = False
from L1CaloD3PDMaker.L1CaloProblemChannel_D3PDObject import L1CaloProblemChannel_D3PDObject

def L1CaloD3PDProblemChannel(alg,
                             level = 0):

  alg += L1CaloProblemChannel_D3PDObject(level = 0)
