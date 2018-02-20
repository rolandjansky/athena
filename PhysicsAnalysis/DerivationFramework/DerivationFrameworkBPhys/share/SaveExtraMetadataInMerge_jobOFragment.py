# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# JobOption fragment to be used during DAOD merging. It takes care of
# propagating all B-physics metadata objects to the output file.
#

# Python import(s):
import re

# Core import(s):
from RecExConfig.InputFilePeeker import inputFileSummary
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from AthenaCommon.Logging import logging

# Create a logger:
_logger = logging.getLogger( "SaveExtraMetadataInMerge_jobOFragment" )

# Find the exact name of xAOD::FileMetaData_vX in the inputFileSummary
# dictionary:
mdType = ""
for key in inputFileSummary[ 'metadata_itemsDic' ].keys():
    if re.match( 'xAOD::FileMetaData_v[0-9]+', key ):
        mdType = key
        break
    pass

# If there is, then let's do the rest of the setup:
if mdType != "":

    # Loop over the keys of all the xAOD::FileMetaData objects:
    for key in inputFileSummary[ 'metadata_itemsDic' ][ mdType ]:

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
