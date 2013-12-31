# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# This legacy file is just kept here for backward compatibility. The python
# objects which were defined in this file in the past, now live in other
# python files in this same directory.

# Include the classes/objects which were defined here in the past:
from TrigBJetD3PDMaker.TrigL2BjetD3PDObject import TrigL2BjetD3PDObject
from TrigBJetD3PDMaker.TrigEFBjetD3PDObject import TrigEFBjetD3PDObject
from TrigBJetD3PDMaker.TrigBJetD3PD import TrigBJetD3PDObjects

# Print a warning message, that the users should update their jobOptions:
from AthenaCommon.Logging import logging
TrigBJetD3PDObject_logger = logging.getLogger( "TrigBJetD3PDObject" )
TrigBJetD3PDObject_logger.warning( "TrigBJetD3PDMaker.TrigBJetD3PDObject is now empty. "
                                   "Please import the things that you need from other "
                                   "places in this package." )
