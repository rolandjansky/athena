# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



from JetRec.JetAlgConfiguration import checkAndUpdateOptions
from IOVDbSvc.CondDB import conddb

from AthenaCommon.Logging import logging
_logger = logging.getLogger('JetConfig::JetCalibDB')

JetDBFolder = "/CALO/JetCalib"



loaded_folders = set([])
def loadJetFolder(folder, baseFolder=JetDBFolder,schema='CALO', ):
    """Load the appropriate DB folder if needed.
    returns the full folder path in DB"""
    fullFolder = baseFolder+'/'+folder
    _logger.info(' loading DB folder : '+fullFolder)
    if not fullFolder in loaded_folders:
        conddb.addFolder(schema, fullFolder)
        loaded_folders.add(fullFolder)
    return fullFolder

    
# options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
