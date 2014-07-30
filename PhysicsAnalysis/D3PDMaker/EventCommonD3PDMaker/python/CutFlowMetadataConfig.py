# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: CutFlowMetadataConfig.py 339578 2011-01-11 11:18:56Z krasznaa $
#
# This module holds the code that should be used to add event bookkeeper 
# metadata to the D3PD.

##
# @short Function creating a correctly configured CutFlowMetadataTool
#
# The D3PD::CutFlowMetadataTool has to be configured carefully, so it's better
# to do it centrally. You can add information about the cut flow of your job
# by adding something like this to your jobOptions:
#
# <code>
# d3pdalg.MetadataTools += [ CutFlowMetadataConfig( d3pdalg ) ]
# </code>
#
# @author Jet Goodson <jetgoodson@gmail.com>
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 339578 $
# $Date: 2011-01-11 12:18:56 +0100 (Tue, 11 Jan 2011) $
def CutFlowMetadataConfig( d3pdalg ):

    from AthenaCommon.AppMgr import ServiceMgr
    from EventBookkeeperTools.EventBookkeeperToolsConf import CutFlowSvc

    # Add the cut flow service if it's not in the job yet:
    if not hasattr( ServiceMgr, "CutFlowSvc" ):
        ServiceMgr += CutFlowSvc()

    # Create a separate D3PDSvc:
    _d3pdSvcName = "CutFlowD3PDSvc"
    if not hasattr( ServiceMgr, _d3pdSvcName ):
        from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
        ServiceMgr += D3PD__RootD3PDSvc( _d3pdSvcName )
    _d3pdSvc = getattr( ServiceMgr, _d3pdSvcName )
    _d3pdSvc.MasterTree = ""
    _d3pdSvc.IndexMajor = ""

    # Now create the metadata tool with the correct configuration:
    import EventCommonD3PDMaker
    return EventCommonD3PDMaker.CutFlowMetadataTool( "CutFlowMetadataTool",
                                                     MetaKey = 'CutFlow',
                                                     D3PDSvc = _d3pdSvc,
                                                     MetaDir = d3pdalg.TuplePath + "Meta" )
