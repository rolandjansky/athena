# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/GenEventD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2009
# @brief Define D3PD object for GenEvent variables.
#
# This defines event-level data for GenEvent.
# The particles are stored separately.
#

from TruthD3PDMaker.GenEventD3PDObject import GenEventD3PDObject

from AthenaCommon.Logging import logging
GenEventD3PDObject_logger = logging.getLogger( "GenEventD3PDObject" )
GenEventD3PDObject_logger.warning( "GenEventD3PDObject now lives in TruthD3PDMaker. "
                                   "Please import it from there in the future." )
