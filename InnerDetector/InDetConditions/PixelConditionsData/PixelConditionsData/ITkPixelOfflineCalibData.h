/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITkPixelOfflineCalibData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ITKPIXELOFFLINECALIBDATA_H
#define ITKPIXELOFFLINECALIBDATA_H

#include "AthenaKernel/CLASS_DEF.h"

#include "PixelConditionsData/ITkPixelClusterErrorData.h"

#include "AthenaKernel/CondCont.h"

/** @class ITkPixelOfflineCalibData 

    The ITkPixelOfflineCalibData is a class that designed to hold the 
    data used by ITk pixel offline algorithms.

*/  

namespace ITkPixelCalib {

class ITkPixelOfflineCalibData{

  public:
  /** Constructor:*/
  ITkPixelOfflineCalibData(); 
  ITkPixelOfflineCalibData(const ITkPixelOfflineCalibData& rhs);
  ITkPixelOfflineCalibData& operator=(const ITkPixelOfflineCalibData& rhs); 

  /** default destructor */
  ~ITkPixelOfflineCalibData ();

  bool update(const ITkPixelClusterErrorData& idat);

  // get the pointer to pixel cluster error data
  ITkPixelClusterErrorData* getITkPixelClusterErrorData();
  const ITkPixelClusterErrorData* getITkPixelClusterErrorData() const;

  std::vector<float> getConstants() const;
  void setConstants(const std::vector<float> &constants);

  void dump();

 private: 
  ITkPixelClusterErrorData* m_clustererrordata; 


};


inline ITkPixelOfflineCalibData::ITkPixelOfflineCalibData() {
  m_clustererrordata = new ITkPixelClusterErrorData();
} 

inline ITkPixelOfflineCalibData::ITkPixelOfflineCalibData(const ITkPixelOfflineCalibData& rhs){
  m_clustererrordata = rhs.m_clustererrordata;
}

inline ITkPixelOfflineCalibData& ITkPixelOfflineCalibData::operator=(const ITkPixelOfflineCalibData& rhs){
  if(this != &rhs){
    m_clustererrordata = rhs.m_clustererrordata;
  }
  return (*this);
}
  
inline bool ITkPixelOfflineCalibData::update(const ITkPixelClusterErrorData& idat){
  *m_clustererrordata = idat;
  return true;
}

inline ITkPixelClusterErrorData* ITkPixelOfflineCalibData::getITkPixelClusterErrorData() {
  return m_clustererrordata;
}

inline const ITkPixelClusterErrorData* ITkPixelOfflineCalibData::getITkPixelClusterErrorData() const {
  return m_clustererrordata;
}



inline ITkPixelOfflineCalibData::~ITkPixelOfflineCalibData(){
  m_clustererrordata = nullptr; //Needed to avoid segfault when destructor is called
  delete m_clustererrordata;
}

}


CLASS_DEF( ITkPixelCalib::ITkPixelOfflineCalibData , 222566141 , 1 )
CLASS_DEF( CondCont<ITkPixelCalib::ITkPixelOfflineCalibData> , 115484743 , 1 )

#endif 


