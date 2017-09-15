/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
@page TGC_CondCabling_page TGC_CondCabling Package

TGC_CondCabling package contains a class (TGCCablingDbTool) to retrieve TGC cabling map database from COOL. 

@author Monica Verducci
@author Susumu Oda <Susumu.Oda@cern.ch>

@section TGC_CondCabling_TGC_CondCablingIntro Introduction
This package has been created to retrieve the TGC cabling map database in COOL. 
The database describes wrong cabling connections between ASD and Patch Panel (PP). 
The database was MuonSpectrometer/MuonCablings/TGCcabling12/share/ASD2PP_diff_12.db. 
However, the wrong cabling connections are time-dependent and we moved the file to the COOL database.   
There are no found wrong connections as of August 30, 2009. 

@section TGC_CondCabling_TGCCablingDbTool TGCCablingDbTool
@subsection TGCCablingDbToolloadASD2PP_DIFF_12 loadASD2PP_DIFF_12 
TGCCablingDbTool retrieves the TGC wrong connection map from the COOL database by the loadASD2PP_DIFF_12 method with Interval of Validity (IoV =Run*2^32+LumiBlock). 
This loadASD2PP_DIFF_12 method is called by MuonSpectrometer/MuonCablings/MuonTGC_Cabling/src/MuonTGC_CablingSvc.cxx as call-back. 
- The folder name is /TGC/CABLING/MAP_SCHEMA. 
- The locations are 
 - "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TGC;dbname=OFLP200;user=ATLAS_COOLOFL_TGC" for simulation (ATLR server) 
 - "oracle://ATONR_COOL;schema=ATLAS_COOLONL_TGC;dbname=COMP200;user=ATLAS_COOLONL_TGC" for real data (ATONR server) 
- There are currently (as of August 30, 2009) two tags 
 - TgcCablingMapschema-20090722-ASD2PPOFL for simulation (Version 0.01)
 - TgcCablingMapschema-20090722-ASD2PPONL for real data (Version 0.02)
 - There is only one IoV range (from 0 to 2^63-1 (9223372036854775807)) in each tag 
- The configuration is written in MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCablingConfig.py 
\code
# COOL setting for MuonTGC_Cabling 
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitMC('TGC','/TGC/CABLING/MAP_SCHEMA','/TGC/CABLING/MAP_SCHEMA')
\endcode
- The format of the database is as follows: 
\code
//----------------------------------------------------------------------
// ASD2PP_diff.db
//-----------
// Connection information between ASDs and Patch Panels (PPs)
// This file describes diff from ASD2PP.db
//
// Table contents: iii nnnn
//                 iii  = PP type
//                 nnnn = Number of channels
//
//                     |-  ASD -||-- PP -|
//                 q r  s t uuu   x y zz
//                 q    = Side ( 0: A , 1:C )
//                 r    = Sector ID
//                 s    = Layer ID
//                 t    = Chamber ID
//                 uuu  = Wire-group ID
//                 x    = PP ID
//                 y    = PP input-connector ID
//                 zz   = PP input-channel ID of a connector
//---------------------------------------------------------------------- 
// OWNER:  Hisaya Kurashige          SECTION: Connection
//
//               
//                  Date      Who  Explanation of Changes
//               ----------- ----- ---------------------------------------
// Version  0.00 29-OCT-2008  HK    Creation
// Version  0.01 22-JUL-2009  Susumu Update for simulationd; just added
//                                   this message
// Version  0.02 22-JUL-2009  Susumu Update for data; just added this
//                                   message
//
//--------------------------------------------------------------------
//
EWT 0
EWD 0
EST 0
ESD 0
FWT 0
FWD 0
FST 0
FSD 0 
EWI 0
ESI 0
FWI 0
FSI 0
//
\endcode
@subsection TGCCablingDbToolgiveASD2PP_DIFF_12 giveASD2PP_DIFF_12 
The giveASD2PP_DIFF_12 method provides the wrong connection map to the client, MuonTGC_CablingSvc, as std::vector<std::string>*. 
@subsection TGCCablingDbToolgetFolderName getFolderName 
The getFolderName method provides the folder name, /TGC/CABLING/MAP_SCHEMA, to the client. 
The folder name is required for call-back. 
@subsection TGCCablingDbToolreadASD2PP_DIFF_12FromText readASD2PP_DIFF_12FromText
The readASD2PP_DIFF_12FromText method reads the wrong connections from user InstallArea/share/ or MuonSpectrometer/MuonCablings/TGCcabling12/share/ASD2PP_diff_12.db. 
The file name is defined in 
\code
// ASD2PP_diff_12.db is the text database for the TGCcabling12 package
declareProperty("filename_ASD2PP_DIFF_12", m_filename="ASD2PP_diff_12.db");
\endcode
This method is used for debug and test. 

@section TGC_CondCabling_TGCCablingDbToolHowToUpdataDatabase How to update database?
The current tags are ideal cabling connections without wrong connections.
We will keep the map database ideal for simulation. We will update the map database for real data. 
The following is the update prodedure.  
@subsection TGCCablingDbToolHowToPrepare Prepare database text file
You need to prepare an updated database text file. The format is shown above. 
The database is written with OnlineID and the Sector ID starts from 0 (sector 01 phi0) and ends at 47 (sector 12 phi3). 

Susumu Oda and Hisaya Kurashige will take care of this. 
@subsection TGCCablingDbToolHowToTestFromText Test the new database using readASD2PP_DIFF_12FromText
You need to install the updated database text file in your $TestArea/InstallArea/share with the name defined in the readASD2PP_DIFF_12FromText method. 
You need to check out the appropriate TGC_CondCabling tag and change the constructor of TGCCablingDbTool from 
\code
m_readASD2PP_DIFF_12FromText=false 
\endcode
to 
\code
m_readASD2PP_DIFF_12FromText=true
\endcode
to use the readASD2PP_DIFF_12FromText method through the loadASD2PP_DIFF_12 method. 
Then, you need to compile the TGC_CondCabling package.  

Susumu Oda and Hisaya Kurashige will take care of this. 
@subsection TGCCablingDbToolHowToTestAtINTR Test the new database at INTR 
Monica Verducci will insert the new database into INTR server. 

You need to add the following lines to your jobOptions:
\code
#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------

dbConn_Tgc="oracle://intr;schema=ATLAS_COOL_TGCMAP;dbname=TGCMAP;user=ATLAS_COOL_TGCMAP"
folder_Tgc="/TGC/CABLING/MAP_SCHEMA"

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn_Tgc
ServiceMgr.IOVDbSvc.Folders+=[folder_Tgc+" <tag>test</tag> <dbConnection>"+dbConn_Tgc+"</dbConnection>"]
\endcode  
The tag is required to be correctly configured.  
You also need to have authentication.xml file in your current (run) directory:
\code
<?xml version="1.0" ?>
<connectionlist>

<connection name="oracle://intr/ATLAS_COOL_TGCMAP">
 <parameter name="user" value="ATLAS_COOL_TGCMAP"/>
 <parameter name="password" value="ATLAS4TGCMAP"/>
</connection>

</connectionlist>
\endcode
@subsection TGCCablingDbToolHowToInsert Insert the new database into the ATONR server 
David Front will insert the new database into the ATONR server, will set the appropriate IoV range and will name the leaf tag (TgcCablingMapschema-YYYYMMDD-ASD2PPONL). 

@subsection TGCCablingDbToolHowToAssociate Associate the new database to global tags
Paul Laycock and Richard Hawkings will associate the leaf tag of the new TGC cabling map database to all relevant global tags (COMCOND-*). 
The global tags are described in https://twiki.cern.ch/twiki/bin/view/Atlas/CoolProdTags 
The official authentication.xml file will be updated automatically. 

It is better to associate the leaf tag to a global tag and test the database with the global tag first. 
If there is no problem, then you ask Paul and Richard to associate the leaf tag to other global tags.  

*/
