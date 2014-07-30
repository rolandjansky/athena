# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/MCTruthClassifierConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Jan, 2010
# @brief Import this to configure the MCTruthClassifier tool for D3PD making.
#

# Create the classifier tool, and set the MC collection name to match
# what's in D3PDMakerFlags.

import TruthD3PDMaker.MCTruthClassifierConfig

from AthenaCommon.Logging import logging
MCTruthClassifierConfig_logger = logging.getLogger( "MCTruthClassifierConfig" )
MCTruthClassifierConfig_logger.warning( "MCTruthClassifierConfig now lives in TruthD3PDMaker. "
                                      "Please import it from there in the future." )
