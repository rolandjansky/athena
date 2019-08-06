# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from PyUtils.MetaReader import read_metadata

#Module level cache of file-metadata:
_fileMetaData=dict()

def GetFileMD(filenames):
    from AthenaCommon.Logging import logging
    msg = logging.getLogger('AutoConfigFlags')
    filename=filenames[0]
    if filename not in _fileMetaData:
        if len(filenames)>1:
            msg.info("Multiple input files. Use the first one for auto-configuration")
        msg.info("Obtaining metadata of auto-configuration by peeking into %s", filename)


        thisFileMD=read_metadata(filename,None,'peeker')
        _fileMetaData.update(thisFileMD)

    return _fileMetaData[filename]
    
