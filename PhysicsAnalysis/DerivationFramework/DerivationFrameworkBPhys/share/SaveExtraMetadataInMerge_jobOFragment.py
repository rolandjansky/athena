# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# JobOption fragment to be used during DAOD merging. It takes care of
# propagating all B-physics metadata objects to the output file.
#

# Core import(s):
from RecExConfig.InputFilePeeker import inputFileSummary
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from AthenaCommon.Logging import logging

# Create a logger:
_logger = logging.getLogger( "SaveExtraMetadataInMerge_jobOFragment" )

# Check if there are any xAOD::FileMetaData objects in the input file's metadata
# payload:
if 'xAOD::FileMetaData' in inputFileSummary[ 'metadata_itemsDic' ]:

    # Loop over the keys of all the xAOD::FileMetaData objects:
    for key in inputFileSummary[ 'metadata_itemsDic' ][ 'xAOD::FileMetaData' ]:

        # If it doesn't look like a b-physics metadata object, then leave
        # it alone:
        if not key.endswith( '_MetaData' ):
            continue

        # Create the metadata tool for propagating this info:
        toolName = "BPhysFileMetadataTool_%s" % key
        ToolSvc += CfgMgr.xAODMaker__FileMetaDataTool( toolName,
                                                       InputKey = key,
                                                       OutputKey = key )
        svcMgr.MetaDataSvc.MetaDataTools += [ getattr( ToolSvc, toolName ) ]
        _logger.info( "Created tool: %s" % toolName )

        # Add the metadata to the output stream(s):
        outputItems = [ 'xAOD::FileMetaData#%s' % key,
                        'xAOD::FileMetaDataAuxInfo#%sAux.' % key ]
        MSMgr.AddMetaDataItemToAllStreams( outputItems )
        _logger.info( "Added metatata items: %s" % str( outputItems ) )

        pass
    pass
