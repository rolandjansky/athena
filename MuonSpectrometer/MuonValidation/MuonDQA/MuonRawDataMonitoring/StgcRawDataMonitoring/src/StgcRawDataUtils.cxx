/*                                                                             
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration                              
*/

///////////////////////////////////////////////////////////////////////////                          
// Utils for the main sTGCRawDataMonAlg.cxx                                                            
// Part of StgcRawDataMonAlg.h                                                                         
// see StgcRawDataMonAlg.cxx                                
///////////////////////////////////////////////////////////////////////////                      
         
#include "StgcRawDataMonitoring/StgcRawDataMonAlg.h"

int sTgcRawDataMonAlg::get_sectorPhi_from_stationPhi_stName(int stationPhi, const std::string &stName) const 
{ 
  int iStation = (stName == "STS") ? 1 : 0;
  
  return 2*stationPhi - iStation;
}

