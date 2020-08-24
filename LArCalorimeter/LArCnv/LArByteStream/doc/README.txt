	This note summarizes the current status of ByteStream output for
LAr Raw Data Object (RDO). 

	This package provides conversion of LArRawChannels into ATLAS raw
data in ByteStream format, and conversion from BS to LArRawChannels or
LArCells.  See Event/ByteStreamCnvSvc/doc/ByteStream.txt
for details on conversion service and raw event format.  

	The ATLAS raw data is defined by a set of hierarchical fragments.
Only the bottom level fragment, ROD-fragment, is defined by the detector
group. The rest is defined by the Trig-DAQ readout system.

	The raw data format for LAr is not finalized.  Instead, we have
defined a temporary format, to the best of our knowledge.  Each ROD data
block consists of one or two FEB blocks. FEB blocks are defined in the 
following way: 

------------------------------------------------------------------------------

The first three words are allways:
 WORD COUNT (32 bits words,  including this word) 
 BLOCK TYPE: format of the data  
 FEB_ID:  Identifier for FEB 

For BLOCK_TYPE=0 the data have the following format:
 HEADER made of the following 16, 16-bit words.
  NWHead       = Number of 32-bit words in the header including the three above
  NWEnergy     = Number of 32-bit words in the energy block
  NWtQ         = Number of 32-bit words in the tQ block
  NWRawData    = Number of 32-bit words in the raw data block
  Dummy1       = not used
  TTCEventID   = TTC Event ID 
  TTCBCID      = TTC Bunch Crossing ID
  CTL1         = ROD Control word 1
  CTL2         = ROD Control word 2
  NGains       = Number of gains read
  NSamples     = Number of samples read
  EventStatus1 = ROD Event status 1
  EventStatus2 = ROD Event status 2
  CTL3         = ROD Control word 3
  Dummy2       = not used
 ENERGY BLOCK
  MAP1, 4x32-bit words (one bit for each of the 128 channels)
  ENERGY, 32-bit integer for each masked channel
 TQ BLOCK
  MAP2, 4x32-bit words (one bit for each of the 128 channels)
  TIME AND QUALITY, 2x16-bit integer for each masked channel
 RAW DATA BLOCK
  MAP3: 4x32-bit words (one bit for each of the 128 channels)
  RAW DATA: (number of samples [+1 if odd nb of samples])x16-bit words
	- gain is packed on the 2 highest bits of the first word
	- if (number of samples) is odd, gain is packed alone

------------------------------------------------------------------------------

	The encoding to BS from LArRawChannel is implemented in in the 
class LArROD_Encoder.  It can take a group of LArRawChannels in one
ROD and turn them into an ROD block.   Conversely, 
LArROD_Decoder class  provides conversion from a ROD fragment (a block of 
memory) to either  LArRawChannels or LArCells. 

	Each ROD is assigned a ROD ID.  The format of the ID is defined
by TDAQ group.  The assignment of the ID is done by the class
Hid2RESrcID.  The current mapping is that all FEBs in one Feedthrough is
put into one ROD.  This class also provides transformation of ROD ID to
ROS ID, as it is needed for assembling higher level Fragments, with
FullEventAssembler in ByteStreamCnvSvc package.

	The LAr RDO, LArRawChannels, are stored in LArRawChannelCollection.  
These collections can be accessed through LArRawChannelContainer 
( subclass of IdentifiableContainer ). 

	Currently, the LArRawChannels in a LArRawCollection is ordered
according to temporary definition of LAr Trigger Tower Identifier
(defined in LArIdentifier package).   This provides access to Region of
Interests expressed as a group of Trigger Towers.  To access all
LArRawChannels in a RoI, the user can use the selector class, 
LArTT_Selector in LArRawUtils package, given a LArRawChannelContainer,
and a vector of TT Identifiers. 

	Even though in the standard offline data model, LArCells are
stored in CaloCellContainer, in granularity of subdetectors (EM, HEC,
FCAL), a special Collection/Container, similar to that for
LArRawChannels is provided for L2 algorithms, because it is desirable to
create LArCell directly from BS for efficiency reason.  
The calibration of LArCells, normally applied by algorithms in
LArCellRec packages, have to be applied.  This is achieved by defining a
LArCellCorrection base class, a subclass of AlgTool, in LArRecUtils
package, and the converter will retrieve a set of these AlgTools
from ToolSvc to apply the corrections to the LArCells.  This guarrantees
the LArCells created by the ByteStream converter is identical to the
LArCells created in the offline data processing sequence. 

	There are a few converters in this package.  
Two for the containers (LArRawChannelContByteStreamCnv, 
and LArCellIDC_ByteStreamCnv) 
One template Tool class for the Collections (both LArRawChannel and
LArCell). 

	LArRawChannelContByteStreamCnv converts all LArRawChannels in
all Collections into RODFragments. A template collection converter is
provided in ByteStreamCnvSvc package, and each detector is supposed to
provide an AlgTool, to be used by the templated collection converter.
The AlgTool for LAr is  LArRawChannelCollByteStreamTool, which 
creates LArRawChannels or LArCells in one Collection from ROBfragments. 

	Using a mapping class, Hid2RESrcID, LArRawChannelContByteStreamCnv 
also provide ByteStreamAddress objects for the Collections. 

	The conversion is implemented in Gaudi AlgTool classes, such that it
can be used outside of the converters.    

	Two top level jobOptions are provided for testing. 
share/jobOptions_WriteLArROD.txt creates ByteStream data from Zebra
input, while share/jobOptions_ReadLArROD.txt reads back the ByteStream
file, and process the LArRawChannels.   

	A few jobOptions fragments are used by the general BS top
jobOptions to write/read the BS of the whole ATLAS detector. LAr
ByteStream provides, 

ReadLArBS_jobOptions        Read in LArRawChannels from BS. 
ReadLArCellBS_jobOptions    Read in LArCell from BS. 
WriteLArBS_jobOptions       Write in LArRawChannel to BS. 

	It should be noted that PRDO_BS_jobOptions.txt in
ByteStreamCnvSvc uses both WriteLArBS_jobOptions and
ReadLArCellBS_jobOptions in one job to achieve 
Zebra --> LArRawChannel --> BS --> LArCell . 
	
	When reading back from BS, the LArDetDescr is needed.
We use the ROOT persistency of LArDetDescr to recreate the transient
DetDescr objects. 

	The runtime dependency for running both Write and Read jobs are: 

 use AthenaCommon       AthenaCommon-*  Control

 use LArROD             LArROD-*        LArCalorimeter
 use LArSim             LArSim-*        LArCalorimeter
 use LArBookkeeping     LArBookkeeping-*        LArCalorimeter
 use LArDigitization    LArDigitization-*       LArCalorimeter
 use LArConditions      LArConditions-* LArCalorimeter
 use LArHitZebraCnv     LArHitZebraCnv-*        LArCalorimeter/LArCnv
 use CBNT_Athena        CBNT_Athena-*   Reconstruction

 use LArByteStream      LArByteStream-*  LArCalorimeter/LArCnv
 use TDRDetDescrAthenaRoot TDRDetDescrAthenaRoot-* DetectorDescription
 use LArDetMgrDetDescrCnv LArDetMgrDetDescrCnv-* LArCalorimeter/LArCnv/

 apply_pattern install_runtime


---------------------

	How to modify LArByteStream for other Detector 
		ByteStream converter package. 

	This following is meant to give step-by-step instruction for
converting the LArByteStream package into a ByteStream Converter
package for another detector. 

	LArByteStream package has the following classes defined:

LArRawChannelContByteStreamCnv.h
LArRawChannelContByteStreamTool.h
LArRawChannelCollByteStreamTool.h
Hid2RESrcID.h
LArRC2Bytes.h
LArROD_Decoder.h


1) Hid2RESrcID

	A class for mapping Collection ID to RODID, and RODID to 
ROBID, RODID to ROSID, etc. 

	This is specific for LAr, but you need the equivalent class for
detector XYZ.

	The main functionality is 

 a) given a Collection ID, return a vector of ROBID.  For Detector with
    Collection that does not match exactly ROB, you may need to associate
    with each ROB with an index, to indicate which sub-component of the ROB
    corresponds to this Collection. 

 b) mapping the Source ID from lower level to high level. 
	Source ID is defined in 
http://doc.cern.ch//archive/electronic/cern/others/atlnot/Note/daq/daq-98-129.pdf

	To use FullEventAssemler class in ByteStreamCnvSvc package, you
need to tell it how to assign a ROD fragment to a ROB fragment, ROB to
ROS, etc. 
	
2) LArRC2Bytes and LArROD_Decoder 
   LAr specific class for ROD format.

3) LArRawChannelContByteStreamTool: 

An AlgTool class to convert LArRawChannelContainer into ByteStream.
This is  LAr specific, but you should follow this example, and define
another AlgTool class for detector XYZ (XYZ_ContByteStreamTool).   The method 

    StatusCode convert(CONTAINER* cont, RawEvent* re, MsgStream& log ); 

should do all the job of converting the content of the Container into
ROD fragments, and append them to RawEvent (which is the top level
fragment Event, see typedef in ByteStreamData/RawEvent.h ) 

    In LArRawChannelContByteStreamTool::convert(...), we use
LArROD_Decoder to convert the LArRawChannels into ROD fragment data.
Then use FullEventAssemlber + Hid2RESrcID to append the ROD data to the
Event fragment.

4) LArRawChannelContByteStreamCnv: 

	This is the Gaudi Converter class.  Only limited LAr stuff.

 a) Replace LArRawChannelContainer everywhere with XYZ_Container.
 b) Replace LArRawChannelByteStreamTool with XYZ_ByteStreamContTool.
 c) in createObj(...) method, replace the code after the line 
   // create all address Objects 
   with appropriate code the detector XYZ. 
   For each Collection, we need to find the corresponding ROBIDs,
   and then create ByteStreamAddress objects and record them in SG. 
   For LAr, we use Hid2RESrcID to map the Collection ID to ROBID.  See
   above for Hid2RESrcID.

   
	With the modified Tool and Cnv classes above, you should be able
to write the ByteStream.   See below for changes to jobOptions file.
	

5) LArRawChannelCollByteStreamTool

	This is the only class needed for making a Converter for the
Collection.    Each detector is required to provide this AlgTool class.  
Note it is a template class for ROBData.

  a) copy LArRawChannelCollByteStreamTool.h and .icc to 
	  XYZ_CollByteStreamTool.h and .icc .  

  b) Replace LArRawChannelCollection with XYZ_Collection.
 
  c) Change the implementmentation of 
     convert(VROBDATA& vRobData,
         COLLECTION*& coll, const unsigned long* ipar, MsgStream& log )
     method to convert vector of ROBData to COLLECTION. 
     For LAr, we use LArROD_Decoder class to convert the ROD block into 
     a LArRawChannelCollection.   Replace it with appropriate class.

     Note that if ROB does not match exactly a Collection, you need to 
     d) use the second int parameter in ByteStreamAddress.  ipar is a pointer to 
     unsigned int,  first int is for hashID, second int ( *(ipar+1) ) can be 
     used for this purpose.  It has to be put into the ByteStreamAddress class
     in the Container converter.
     e) You may need to create other Collections in addition to the requested
       Collection.  The additional Collections can be put into SG with updateProxy 
       method.


6) src/RMS directory, cmt/requirements, etc
    src/RMS directory have _entries and _load files for the Collection 
    converter. Change all the package and class names accordingly. 
    Same for cmt/requirements. 
  
7) jobOptions for writing. 
    Make a jobOptions in share directory, WriteXYZBS_jobOptions.txt
    For LAr, it looks like this, 

// jobOptions for creating MC LArRawChannel, then write LAr ByteStream 
#include "$LARRODROOT/share/LArROD_MC_jobOptions.txt"
ApplicationMgr.TopAlg += { "ReadLArRaw" };
ReadLArRaw.LArRawChannelContainerName = "LArRawChannels" ;
ReadLArRaw.HistogramID = 128 ;
ReadLArRaw.NtupleLocID = "/FILE1/LARG" ;

// specify the LAr Converter and LArROD type and key for output. 
ApplicationMgr.Dlls   += { "LArByteStream" };
StreamBS.ItemList +={"2721#*"};

The first part should be replaced by jobOptions that can make your RDOs.
In the second part, replace LArByteStream with XYZ_ByteStream, and 2721 with 
the CLID for the Container.    

Then in run directory, modify  WriteBS_jobOptions.txt (from ByteStreamCnvSvc 
package) to include this WriteXYZBS_jobOptions.txt ( and optionally turn off 
other detector if you want to.)  The default version has LArByteStream only). 
Run Athena with WriteBS_jobOptions.txt with ZEBRA input.

8) jobOptions for reading.

Make a jobOptions in share directory, ReadXYZBS_jobOptions.txt. Similar to above, 
it should consist of two parts, an algorithm that accesses the RDOs (making ntuple, 
for example), and loading XYZ_ByteStream shared library.  Note without an 
algorithm that accesses RDOs, the converter will not be called.

For LAr, we also include a jobOptions that reads in the DetDescr from AthenaRoot.
See DetectorDescription/TDRDetDescrAthenaRoot/doc/README if you need it. 

Then in run directory, modify ReadBS_jobOptions.txt to inlcude this 
ReadXYZBS_jobOptions.txt file.  
Run Athena with ReadBS_jobOptions.txt 

When a ByteStream converter package is in release, we will put the corresponding 
jobOptions in ReadBS_jobOptions.txt and WriteBS_jobOptions.txt in ByteStreamCnvSvc
package.




  
