# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# MagFieldServices
#--------------------------------------------------------------

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags


# initialise required conditions DB folders
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitMC('GLOBAL','/GLOBAL/BField/Maps <noover/>','/GLOBAL/BField/Maps <noover/>', className="CondAttrListCollection")
if not athenaCommonFlags.isOnline():
    conddb.addFolder('DCS_OFL','/EXT/DCS/MAGNETS/SENSORDATA', className="CondAttrListCollection")
    
from AthenaCommon.CfgGetter import getAlgorithm
from AthenaCommon.AlgSequence import AthSequencer             
condSequence = AthSequencer("AthCondSeq")             
condSequence += getAlgorithm( "AtlasFieldMapCondAlg" )
condSequence += getAlgorithm( "AtlasFieldCacheCondAlg" )

