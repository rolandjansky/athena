# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        LongLivedDPDFlags
##
## Author:      Nick Barlow, Christian Ohm
## Created:     November 2009
##
## Description: Define all neccessary job flags for the LongLivedDPDMaker
##              
##
##=============================================================================

__doc__ = """job flags for LongLivedDPDMaker."""
__version__ = "0.0.2"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class LongLivedDPDFlags(JobPropertyContainer):
    """ The LongLivedParticleDPDMaker jobproperty container """

jobproperties.add_Container(LongLivedDPDFlags)

from PrimaryDPDMaker.PrimaryDPDFlags import listESDtoDPD
from PrimaryDPDMaker.PrimaryDPDFlags import listAllKnownDPD

from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

########################################################
#                                                      #
# Turn on/off different streams                        #
#                                                      #
########################################################

class WriteRPVLLStream(JobProperty):
    """Produce RPV/LL DESD """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    StreamName = "StreamDESDM_RPVLL"
    FileName = ""
    DPDMakerScript = "PrimaryDPDMaker/esdtodpd.py"
    TriggerNames = []
primDPD.add_JobProperty(WriteRPVLLStream)
listESDtoDPD.append(WriteRPVLLStream.StreamName)


from RecExConfig.RecoFunctions import AddValidItemToList
AddValidItemToList(listESDtoDPD,listAllKnownDPD)

