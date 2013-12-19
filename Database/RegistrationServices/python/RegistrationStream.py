# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file RegistrationStream.py
## @brief Job options file for registration stream setup
## @author RD Schaffer <R.D.Schaffer@cern.ch>
## $Id: RegistrationStream.py,v 1.1 2007-07-13 17:22:54 schaffer Exp $
###############################################################
#
# Job options file for registration stream setup
#
#==============================================================
#

def _loadBasicRegistrationStream():
    """Loads the basic services for RegistrationStreamSvc"""

    ## make sure we have been -at least- minimally correctly configured
    import AthenaCommon.AtlasUnixStandardJob

    ## get a handle on the top-level Algorithm sequence
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    #--------------------------------------------------------------
    # Example for Event Collection Registration
    #--------------------------------------------------------------
    #
    # For each output collection, use a separate registration stream
    #
    # Run RegistrationStream as an algorithm
    from RegistrationServices.RegistrationServicesConf import RegistrationStream
    topSequence += RegistrationStream( "RegStream1",
                                       CollectionType   = "ExplicitROOT",
                                       OutputCollection = "NewEventCollection",
                                       OutputLevel      = INFO )

    # # backward compat
    RegStream1 = topSequence.RegStream1

    # List of DO's to register:

    # First element is used to obtain a token to the DataHeader 
    RegStream1.ItemList += [ "DataHeader#*" ]
    # The second element is the AthenaAttributeList to be written into the
    # collection as the tag:
    RegStream1.ItemList += [ "AthenaAttributeList#SimpleTag" ]


    msg.debug( "Loading basic services for RegistrationStream... [DONE]" )
    return

## load basic services configuration at module import
_loadBasicRegistrationStream()

## clean-up: avoid running multiple times this method
del _loadBasicRegistrationStream




