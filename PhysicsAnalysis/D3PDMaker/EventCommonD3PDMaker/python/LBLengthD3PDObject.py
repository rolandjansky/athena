# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: LBLengthD3PDObject.py 515218 2012-08-28 08:50:53Z krasznaa $
#
# D3PDObject filling the length of the luminosity blocks into a D3PD.
#

# D3PDMaker import(s):
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
import EventCommonD3PDMaker

# Hook setting up the connection to the DB folder:
def __lblbFolderAccess( c, **kw ):
    __folder = "/TRIGGER/LUMI/LBLB"
    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested( __folder ):
        conddb.addFolderWithTag( "TRIGGER", __folder, "HEAD" )
        pass
    return

# Create the object:
LBLengthD3PDObject = make_Void_D3PDObject( "", "LBLengthD3PDObject",
                                           default_name = "LBLengthFiller" )

# Set up its hook:
LBLengthD3PDObject.defineHook( __lblbFolderAccess )

# Add the only tool to it:
LBLengthD3PDObject.defineBlock( 0, "LBLength",
                                EventCommonD3PDMaker.LumiBlockLengthFillerTool )
