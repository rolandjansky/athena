# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/GenParticleD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2009
# @brief Define D3PD object for all the GenParticle's in an event.
#
# The data are stored as a collection of GenEvent's, each of which
# holds a collection of GenParticle's.  So the outermost object
# is a GenEvent, and we use an associator to loop over the contained
# particles.
#

from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject

from AthenaCommon.Logging import logging
GenParticleD3PDObject_logger = logging.getLogger( "GenParticleD3PDObject" )
GenParticleD3PDObject_logger.warning( "GenParticleD3PDObject now lives in TruthD3PDMaker. "
                                      "Please import it from there in the future." )
