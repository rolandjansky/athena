/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileConditions_page TileConditions

Calibration constants and cabling description for the TileCalorimeter.

@author Nils Gollub <Nils.Gollub@cern.ch>
@author Alexander Solodkov <Sanya.Solodkov@cern.ch>
@author Karl Gellerstedt <Karl.Gellerstedt@cern.ch>
@author Andrei Artamonov <Andrei.Artamonov@cern.ch>

@section TileConditions_Package_Structure Package Structure

This package consists of a set of tools which provide TileCal
conditions data to clients running in the Athena framework. In
addition to AlgTools also two Services are provided: TileCablingSvc
(see also \ref TileCablingSvcSection below) and TileInfoLoader.

@section TileConditions_TileCondTools TileCondTools

In the following list the currently available TileCondTools are
listed, describing briefly their purpose. For more details see the
individual descriptions within each tool.

- TileBadChanTool: Status information for each TileCal channel and ADC. 
- TileCondToolEmscale: Application of CIS, Laser and Cesium calibration constants, calibration to the EM scale.
- TileCondToolTiming: Signal phase for each TileCal ADC.
- TileCondToolNoiseSample: High and low frequency noise and pedestal level for each ADC.
- TileCondToolNoiseRawChn: Response of reco method (OF, Fit, ...) to pedestal like events.
- TileCellNoiseTool: Access to noise level expected in each TileCal cell (uses TileCondToolNoise tools internally).
- TileCondToolOfc: calculates Optimal Filter Coefficients (OFCs) for each ADC on the fly 
- TileCondToolOfcCool: extracts Optimal Filter Coefficients (OFCs) for each ADC
from COOL database.
- TileCondToolPulseShape: Expected pulse shapes in each ADC.
- TileCondToolMuID: extracts muon thresholds from COOL.

All the tools listed above use the following three (two) unsigned
integers to address a specific ADC (channel):

- drawerIdx [0,...,275]: The drawer index (hash) as described in TileCalibBlobObjs/TileCalibUtils
- channel [0,...,47]: The channel number
- adc [0,1]: The gain (0=low gain, 1=high gain)

The helper tool TileCondIdTransforms provides functions to translate
HWIdentifiers to the (drawerIdx, channel, adc) triplet. This helper
tool should be used in the user code if only HWIdentifiers are
available and need to be translated. Since the translation is time
consuming if repeated many times, the decision was taken to expose the
user to this step, rather than to provide additional TileCondTool
functions which accept HWIdentifiers as input.


@section TileConditions_Data_Sources Sources of Conditions Data

For reconstruction jobs, the conditions data is usually read from the
COOL conditions database where it is associated to an Interval Of
Validity (IOV). The currently valid IOV is automatically selected and
the associated data loaded to StoreGate by the IOVDbSvc. TileCal
stores conditions data in Binary Large OBjects (BLOBs) in the COOL
database using a set of translator classes which provided de- and
encoding of data into the BLOBs (see TileCalibBlobObjs package).

For MC production, default conditions data values are read from text
files (share/TileDefault.*), which can also be used for testing
purposes in reconstruction jobs. In order to decouple the \ref
TileCondTools code from the source of conditions data (COOL DB or text
file), the \ref TileCondTools access conditions data BLOBs through
TileCondProxies: Either a TileCondProxyCool or a
TileCondProxyFile. The TileCondProxies are themselves AlgTools and are
set as Properties to the different \ref TileCondTools. A Python
function (see \ref TileCondProxyConf) is usually used to instantiate
TileCondProxies, specifying the technology to be used ("FILE" or
"COOL"), the type of the TileCalibDrawer object (see
TileCalib/TileCalibBlobObjs package) and the source of the conditions
data. If "COOL" technology is chosen, the source specifies the COOL
folder path. If "FILE" is selected, then an arbitrary filename can be
specified.

For each TileCondTool, a "getter" function is provided which returns a
fully configured tool, reading input either from COOL or a text file
(see \ref TileCondToolConf). Of course, the individual TileCondProxies
attached to the TileCondTool as Properties can be exchanged later,
reading conditions data from a mix of COOL databases and text
files. See the example configuration shown in the \ref
TileCondToolConf section.


@subsection text_file_format Text File Format 

The format of the text file (usually containing default conditions
data used for simulation) follows two rules:

- All lines not starting with either "0x" or "OBJVERSION" are ignored.  
- At least one line specifying the OBJVERSION of the BLOB has to be present.  

Individual ADCs are addressed by specifying 3 numbers: fragId, channel
and gain. All following numbers are interpreted as conditions data. In
interpreting the data specified in the text files, the TileCal
conditions data default policy is followed as specified in the
TileCondBlobObjs package.  The TileDefault.* files in the share
subdirectory should give good examples of valid formats. The following
specifies the detector-wide CIS constant defaults (in
share/TileDefault.cisFitLin):

@verbatim
------------------------------------------------------------------
- This file contains the global CIS defaults:
- low  gain:  1.27875 ( = 1023./800.       )
- high gain: 81.84    ( = 1023./800. * 64. )
------------------------------------------------------------------
OBJVERSION 1
0x000	0	0	1.27875
0x000	0	1	81.84
@endverbatim


@section TileConditions_TileInfoDump TileInfoDump Algorithm

An Athena Algorithm called TileInfoDump is provided to print TileCal
conditions data as it is seen by the Athena framework for a given run
number. Example job options exist in share/TileInfoDump_jobOptions.py,
giving also hints about how to configure \ref TileCondTools and the
access to the COOL database.


@section TileConditions_TileInfo TileInfo, TileInfoLoader, etc...

In the past, a single monolithic TileInfo object was created and
placed into the Detector Store. This instance of the TileInfo class
held all possible TileCal conditions data and provided access to
it. In going to the new more modular package structure, providing
AlgTools to load and access only specific parts of the total
conditions data volume, the TileInfo object (and its TileInfoLoader)
should be phased out. However, TileInfo is used in various places
throughout the TileCal code and a clean break transition was not
possible. Therefore, TileInfo is kept alive and provides wrapper
functions around the individual \ref TileCondTools. The usage of
TileInfo is strongly discouraged for new code developments and old
code should be migrated to use the \ref TileCondTools directly were
possible.




@section TileConditions_Package_Configuration Package Configuration

Python helper classes and functions exist in the python subdirectory,
helping with the configuration of the \ref TileCondTools and
Services. Their usage is illustrated in TileInfoDump_jobOptions.

@subsection TileCoolMgr TileCoolMgr.py 

This Python module is a mini-database, keeping track of TileCal
conditions data sources. It assigns a logical name to the data source
and attaches the COOL folder name, the default database connection
string and the default COOL folder tag to it. If this Python module is
included, an instance called "tileCoolMgr" is automatically created
and is used for all configuration purposes. By default, the database
connection string is looked up from the IOVDbSvc.CondDB object
(providing automatic configuration based on the input data type). No
individual folder tags are set, triggering the use of the
IOVDbSvc.GlobalTag property instead. The TileCoolMgr class provides
functions to modify the folder path, database connection string and
folder tag, enabling the configuration of the source of conditions
data individually for each logical data name.

All existing COOL folders should be entered into the tileCoolMgr. The
currently known setup is:

@verbatim
#=== determine default DB connection string from IOVDbSvc
from IOVDbSvc.CondDB import conddb
defConnStr = conddb.dblist['TILE']

#=== fill with default folders:
tileCoolMgr = TileCoolMgr()
#--- energy calibration
tileCoolMgr.addSource('oflCisFitLin', '/TILE/OFL01/CALIB/CIS/FIT/LIN', defConnStr, "")           
tileCoolMgr.addSource('oflCisFitNln', '/TILE/OFL01/CALIB/CIS/FIT/NLN', defConnStr, "")           
tileCoolMgr.addSource('oflLasLin'   , '/TILE/OFL01/CALIB/LAS/LIN'    , defConnStr, "")           
tileCoolMgr.addSource('oflLasNln'   , '/TILE/OFL01/CALIB/LAS/NLN'    , defConnStr, "")           
tileCoolMgr.addSource('oflCes'      , '/TILE/OFL01/CALIB/CES'        , defConnStr, "")           
tileCoolMgr.addSource('oflEms'      , '/TILE/OFL01/CALIB/EMS'        , defConnStr, "")           
tileCoolMgr.addSource('onlCisLin'   , '/TILE/ONL01/CALIB/CIS/LIN'    , defConnStr, "")
tileCoolMgr.addSource('onlLasLin'   , '/TILE/ONL01/CALIB/LAS/LIN'    , defConnStr, "")           
tileCoolMgr.addSource('onlCes'      , '/TILE/ONL01/CALIB/CES'        , defConnStr, "")           
tileCoolMgr.addSource('onlEms'      , '/TILE/ONL01/CALIB/EMS'        , defConnStr, "")           
#--- noise
tileCoolMgr.addSource('oflNoiseAdc', '/TILE/OFL01/NOISE/SAMPLE', defConnStr, "")
tileCoolMgr.addSource('oflNoiseAcr', '/TILE/OFL01/NOISE/AUTOCR', defConnStr, "")
tileCoolMgr.addSource('oflNoiseFit', '/TILE/OFL01/NOISE/FIT'   , defConnStr, "")
tileCoolMgr.addSource('oflNoiseOf2', '/TILE/OFL01/NOISE/OF2'   , defConnStr, "")
#--- timing
tileCoolMgr.addSource('oflTimeCfib', '/TILE/OFL01/TIME/CELLFIBERLENGTH'  , defConnStr, "")
tileCoolMgr.addSource('oflTimeDlas', '/TILE/OFL01/TIME/DRAWEROFFSET/LAS' , defConnStr, "")
tileCoolMgr.addSource('oflTimeClas', '/TILE/OFL01/TIME/CHANNELOFFSET/LAS', defConnStr, "")
#--- status
tileCoolMgr.addSource('oflStatAdc', '/TILE/OFL01/STATUS/ADC', defConnStr, "")
tileCoolMgr.addSource('onlStatAdc', '/TILE/ONL01/STATUS/ADC', defConnStr, "")
@endverbatim

@subsection TileCondToolConf TileCondToolConf.py

This Python module provides "getter" functions to obtain fully
configured \ref TileCondTools, specifying either "COOL" or "FILE" as
technology. The following section of TileInfoDump_jobOptions.py
illustrates the use of these functions:

@verbatim
#========================================================
#=== configure TileCondTools
#===
#=== The lines below show how to configure individual 
#=== tools directly, without the use of TileInfoConfigurator
#===
#========================================================
from TileConditions.TileCondToolConf import *
ToolSvc += getTileCondToolEmscale()
#--- to switch off the usage of online cache, do
#    ToolSvc.TileCondToolEmscale.OnlCacheUnit="Invalid"
#--- a single proxy can be exchanged, for example like this... 
#    from TileConditions.TileCondProxyConf import getTileCondProxy
#    ToolSvc.TileCondToolEmscale.ProxyOflEms = getTileCondProxy('FILE','Flt','TileDefault.ems','TileCondProxyFile_OflEms')
#--- ... or directly in the creation of the Tool like this:
#    from TileConditions.TileCondProxyConf import getTileCondProxy
#    ToolSvc += getTileCondToolEmscale('COOL',
#                                      ProxyOflLasLin = getTileCondProxy('FILE','Flt','TileDefault.cisFitLin','TileCondProxyFile_OflLasLin'),
#                                      ProxyOflCisLin = getTileCondProxy('FILE','Flt','TileDefault.cisFitLin','TileCondProxyFile_OflCisLin'))
ToolSvc += getTileBadChanTool()
ToolSvc += getTileCondToolNoiseSample()
ToolSvc += getTileCondToolTiming()
@endverbatim

@subsection TileCondProxyConf TileCondProxyConf.py 

This Python module provides a "getter" function for TileCondProxies. A
TileCondProxyFile is returned if "FILE" is passed in the technology
argument and a TileCondProxyCool is returned if "COOL" is
specified. If "COOL" is specified, the COOL folder path (specified in
the source argument) is automatically added to the list of COOL
folders in IOVDbSvc.Folders. If the folder has already been added to
this list, it is not added again, preventing multiple requests for the
same COOL folder.


@section TileConditions_TileCablingSvcSection TileCablingSvc

The class providing actual TileCal cabling information is called
TileCablingService and is placed in the Detector Store using a
singleton pattern. However, TileCablingService is not an Athena
Service and thus not initialized at the beginning of the job. In order
to keep the current TileCablingService class and its functionality, a
wrapper Athena Service TileCablingSvc has been created which does
nothing else than creating the singleton instance of the
TileCablingService class and configuring it properly. It has to be
added to the ServiceMgr in the jobOptions by doing something like
this:

@verbatim
svcMgr += CfgMgr.TileCablingSvc()
@endverbatim



 

*/
