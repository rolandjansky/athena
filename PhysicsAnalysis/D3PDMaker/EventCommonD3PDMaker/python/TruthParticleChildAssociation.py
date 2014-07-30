# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/TruthParticleChildAssociation.py
# @author Ryan Reece <ryan.reece@cern.ch>
# @date Mar, 2010
# @brief Helper for setting up an association for truth particle 
#   children by index.
#


from TruthD3PDMaker.TruthParticleChildAssociation import TruthParticleChildAssociation

from AthenaCommon.Logging import logging
TruthParticleChildAssociation_logger = logging.getLogger( "TruthParticleChildAssociation" )
TruthParticleChildAssociation_logger.warning( "TruthParticleChildAssociation now lives in TruthD3PDMaker. "
                                      "Please import it from there in the future." )
