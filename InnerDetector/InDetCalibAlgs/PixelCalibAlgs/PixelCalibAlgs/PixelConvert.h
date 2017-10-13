/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __PIXEL_CONVERT_H__
#define __PIXEL_CONVERT_H__

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>


/*
  This set of functions provide conversions tools between different
  conventions of the Pixel modle ID:
  - online module ID, described in ATL-IP-ES-0007
  - offline module ID, described in ATL-SOFT-2001-004
  - hash ID, 32 bits integer word, used internally in Athena 

  All functions are defined in PixelConvert namespace, 
  therefore can be accessed by prefixing them with PixelConvert::
  or by using the PixelConvert namespace.

  Some of the funtions provide a consistency check of the input, 
  signalling an illegal module ID.
 */

namespace PixelConvert {

  extern const int NLayer;
  extern const int BSMax[];
  extern const int EtaMax;
  extern const int NinSector;
  extern const int NinDisk;
  extern const std::string Error;

  /*
    UTILITY FUNCTIONS
    to encode and decode the integers described in ATL-SOFT-2001-004
    and the corresponding string the "/" separators.

    No consistency check is performed on the arguments to this fuctions.

    PixelConvert::UnpackOfflineID return 0 if an error occurs while 
    decoding the string (too few fields), and 1 if decoding succeeds.
    Since no consistency check is performed, this indicates only
    that the string was decoded, not that the unpacked content
    correspond to a real channel.
   */

  std::string PackOfflineID(int system,
			    int subSystem,
			    int endcap,
			    int layer,
			    int phi,
			    int eta);

  int UnpackOfflineID(std::string offlineID,
		      int& system,
		      int& subSystem,
		      int& endcap,
		      int& layer,
		      int& phi,
		      int& eta);

  /*
    ONLINE ID
    All the PixelConvert::OnlineID functions provide the string 
    defined according to ATL-IP-ES-0007 which defines a certain 
    module.

    A consistency check on the input data is performed and
    all these functions return the string PixelConvert::Error
    if the input data do not correspond to a real channel.

    The conversion from the offline ID to the online ID can be 
    performed by using either the unpacked or packed forms. 
   */

  std::string OnlineID(int system, 
		  int subSystem, 
		  int endcap, 
		  int layer,
		  int phi,
		  int eta);

  std::string OnlineID(std::string offlineID);

  std::string OnlineID(int hashID);

  std::string OnlineIDfromDCSID(std::string DCSID);

  /*
    OFFLINE ID
    All the PixelConvert::OfflineID functions provide either the 
    string defined according to ATL-SOFT-2001-004 which defines a 
    certain module, or the corresponding set of unpacked integer
    values.


    A consistency check on the input data is performed for all
    the functions that translate the online ID.
    If the requested return value is a string, the sring
    PixelConvert::Error is given back 
    if the input data do not correspond to a real channel.
    If the set of unpacked integers is used, a 0 return value
    is provided if the input data do not correspond to a 
    real channel.

    No consistency check is performed if the conversion start from the
    hash ID.
   */

  int OfflineID(std::string onlineID,
	       int& system, 
	       int& subSystem, 
	       int& endcap, 
	       int& layer,
	       int& phi,
	       int& eta);

  std::string OfflineID(std::string onlineID);

  int OfflineID(int hashID,
	       int& system, 
	       int& subSystem, 
	       int& endcap, 
	       int& layer,
	       int& phi,
	       int& eta);

  std::string OfflineID(int hashID);

  /*
    HASH ID
    All the PixelConvert::HashID functions provide 32-bits hash code
    which defines a certain module internally in Athena.

    If the conversion start from a string, either the online ID or 
    offline ID can be provided, and the function will try to decode 
    the one used. A failure is detected by a return code of -1.
    A consistency check on the input data is performed if the online ID
    is used. If an offline ID string is provided, this is simply unpacked
    and a return code of -1 happens only if the string cannot be unpacked,
    but does not provide any check on the consistency of the provided
    module identifiers.

    If the conversion starts from the unpacked offline ID, no consistency 
    check is performed. 
   */

  int HashID(std::string ID);

  int HashID(int system,
	     int subSystem,
	     int endcap,
	     int layer,
	     int phi,
	     int eta);

  /* 
     DCSID

     This is the internal format of channel ID used in DCS.
     With respect to the OnlineID, sector labelling is changed:
     S01 -> B01_S02
     S02 -> B02_S01
     S03 -> B02_S02
     S04 -> B03_S01
     S05 -> B03_S02
     S06 -> B04_S01
     S07 -> B04_S02    
     S08 -> B01_S01

     Moreover '-' characters, which are illegal in DCS, are replaced by 
     a '_' character.

     Conversion to DCSID is provided only from OnlineID and a compatibility
     check is done on sector number, returning a PixelConvert::Error in 
     case the range cheching fails.

  */
  std::string DCSID(std::string OnlineID);

  int ReadMap(const std::string filename);
  unsigned int GetID(const unsigned int moduleID);

}

#endif
