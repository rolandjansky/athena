# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: GSFElectronD3PDObject.py 523036 2012-10-24 15:50:12Z ssnyder $
#
# @file egammaD3PDMaker/python/GSFElectronD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date 2009
# @brief Configure GSF electron D3PD object.
#

from egammaD3PDMaker.defineBlockAndAlg      import defineBlockAndAlg
from D3PDMakerCoreComps.D3PDObject          import DeferArg
from D3PDMakerCoreComps.resolveSGKey        import resolveSGKey
from D3PDMakerConfig.D3PDMakerFlags         import D3PDMakerFlags
from AthenaCommon.AlgSequence               import AlgSequence
from RecExConfig.RecFlags                   import rec
import egammaD3PDMaker
import egammaD3PDAnalysis
import EventCommonD3PDMaker
import D3PDMakerCoreComps


from egammaD3PDMaker.ElectronD3PDObject import ElectronD3PDObject

GSFElectronD3PDObject = ElectronD3PDObject.copy()
