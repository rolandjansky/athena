/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCRAWDATA_STREAMER_P0_H
#define CSCRAWDATA_STREAMER_P0_H

#include "AthenaPoolServices/T_AthenaRootConverter.h"
#include "CscRawData_p0.h"
#include "MuonRDO/CscRawData.h"

//#include <iostream>

class CscRawDataStreamer_p0
   : public T_AthenaRootConverter<CscRawData, CscRawData_p0 >
{
public:
  CscRawDataStreamer_p0() {
     SetClassNameAndChecksum("CscRawData",  3995976199U);
  }          

  void CopyOldToNew(const CscRawData_p0 &old_obj, CscRawData &new_obj )
  {
     // somewhat inefficient data filling, can be sped up using some tricks
     if( old_obj.getSamples().size() == 0 ) {
	//old data in ROOT3
	const uint16_t *ampls = old_obj.getAmplitudes();
	std::vector<uint16_t> 	amplitudes;
	for( int i=0; i<4; i++ ) {
	   amplitudes.push_back( *ampls++ );
	}
	CscRawData	obj(amplitudes,
			    old_obj.getAddress(),
			    old_obj.identify(),
			    old_obj.dpuID());
	new_obj = obj;
	//std::cout << "CscRawDataStreamer_p0 ROOT3 " << obj;
     } else {
	CscRawData	obj(old_obj.getSamples(),
			    old_obj.getAddress(),
			    old_obj.identify(),
			    old_obj.dpuID()); 
	new_obj = obj;
	//std::cout << "CscRawDataStreamer_p0 ROOT4 " << obj;
     }
  }
  
};



#endif
