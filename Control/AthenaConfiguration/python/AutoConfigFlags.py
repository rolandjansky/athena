from AthenaCommon.Logging import log
from FilePeeker.FilePeeker import PeekFiles


#Module level cache of file-metadata:
_fileMetaData=dict()

def GetFileMD(filenames):
    filename=filenames[0]
    if not filename in _fileMetaData:
        if len(filenames)>1:
            log.info("Multiple input files. Use the first one for auto-configuration")
        log.info("Obtaining metadata of auto-configuration by peeking into %s" % filename)
        thisFileMD=PeekFiles([filename,])
        _fileMetaData.update(thisFileMD)

    return _fileMetaData[filename]
    
