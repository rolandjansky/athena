# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/TruthParticleD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Dec, 2009
# @brief Define D3PD object for a collection of TruthParticle's.
#


from TruthD3PDMaker.TruthParticleD3PDObject import TruthParticleD3PDObject

from AthenaCommon.Logging import logging
TruthParticleD3PDObject_logger = logging.getLogger( "TruthParticleD3PDObject" )
TruthParticleD3PDObject_logger.warning( "TruthParticleD3PDObject now lives in TruthD3PDMaker. "
                                      "Please import it from there in the future." )
