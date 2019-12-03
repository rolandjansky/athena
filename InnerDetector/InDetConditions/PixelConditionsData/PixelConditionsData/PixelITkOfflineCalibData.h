/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelITkOfflineCalibData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELITKOFFLINECALIBDATA_H
#define PIXELITKOFFLINECALIBDATA_H

#include "CLIDSvc/CLASS_DEF.h"

#include "PixelConditionsData/PixelITkClusterErrorData.h"

#include "AthenaKernel/CondCont.h"

/** @class PixelITkOfflineCalibData 

    The PixelITkOfflineCalibData is a class that designed to hold the 
    data used by ITk pixel offline algorithms.

*/  



namespace PixelCalib {

class PixelITkOfflineCalibData{

  public:
  /** Constructor:*/
  PixelITkOfflineCalibData(); 
  PixelITkOfflineCalibData(const PixelITkOfflineCalibData& rhs);
  PixelITkOfflineCalibData& operator=(const PixelITkOfflineCalibData& rhs); 

  /** default destructor */
  ~PixelITkOfflineCalibData ();

  bool update(const PixelITkClusterErrorData& idat);

  // get the pointer to pixel cluster error data
  PixelITkClusterErrorData* getPixelITkClusterErrorData() const; 

  std::vector<float> getConstants() const;
  void setConstants(const std::vector<float> &constants);

  void dump();

 private: 
  PixelITkClusterErrorData* m_clustererrordata; 


};


inline PixelITkOfflineCalibData::PixelITkOfflineCalibData() {
  m_clustererrordata = new PixelITkClusterErrorData();
} 

inline PixelITkOfflineCalibData::PixelITkOfflineCalibData(const PixelITkOfflineCalibData& rhs){
  m_clustererrordata = rhs.m_clustererrordata;
}

inline PixelITkOfflineCalibData& PixelITkOfflineCalibData::operator=(const PixelITkOfflineCalibData& rhs){
  if(this != &rhs){
    m_clustererrordata = rhs.m_clustererrordata;
  }
  return (*this);
}
  
inline bool PixelITkOfflineCalibData::update(const PixelITkClusterErrorData& idat){
  *m_clustererrordata = idat;
  return true;
}

inline PixelITkClusterErrorData* PixelITkOfflineCalibData::getPixelITkClusterErrorData() const {
  return m_clustererrordata;
}



inline PixelITkOfflineCalibData::~PixelITkOfflineCalibData(){
  delete m_clustererrordata;
}

}


CLASS_DEF( PixelCalib::PixelITkOfflineCalibData , 222566141 , 1 )
CLASS_DEF( CondCont<PixelCalib::PixelITkOfflineCalibData> , 115484743 , 1 )

#endif 

