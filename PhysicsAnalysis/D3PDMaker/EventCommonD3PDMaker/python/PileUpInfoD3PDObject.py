# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TruthD3PDMaker.PileUpInfoD3PDObject import PileUpInfoD3PDObject

from AthenaCommon.Logging import logging
PileUpInfoD3PDObject_logger = logging.getLogger( "PileUpInfoD3PDObject" )
PileUpInfoD3PDObject_logger.warning( "PileUpInfoD3PDObject now lives in TruthD3PDMaker. "
                                      "Please import it from there in the future." )
