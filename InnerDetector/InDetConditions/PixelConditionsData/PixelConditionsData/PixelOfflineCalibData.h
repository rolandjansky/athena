/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOfflineCalibData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELOFFLINECALIBDATA_H
#define PIXELOFFLINECALIBDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "PixelConditionsData/PixelClusterErrorData.h"
#include "PixelConditionsData/PixelClusterOnTrackErrorData.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"

#include "AthenaKernel/CondCont.h"


/** @class PixelOfflineCalibData 

    The PixelOfflineCalibData is a class that designed to hold the 
    data used by pixel offline algorithms. This includes 
    Pixel Cluster error parametrizations, and other stuff will follow

    The details can be found at 
    https://twiki.cern.ch/twiki/bin/view/Atlas/AWikiPageNotExistingYet
    
    @author Tommaso Lari <lari@mi.infn.it>
*/  

namespace PixelCalib {

class PixelOfflineCalibData{

  public:
  /** Constructor:*/
  PixelOfflineCalibData(); 
  PixelOfflineCalibData(const PixelOfflineCalibData& rhs);
  PixelOfflineCalibData& operator=(const PixelOfflineCalibData& rhs);
  // PixelOfflineCalibData(const Identifier & ident); 
  // PixelOfflineCalibData(Identifier & ident);  

  /** default destructor */
  ~PixelOfflineCalibData ();

  bool update(const PixelClusterErrorData& idat);
  bool update(const PixelChargeInterpolationParameters& idat);
  bool update(const PixelClusterOnTrackErrorData& idat);
  
  int size() const;
  
  // get the pointer to pixel cluster error data
  PixelClusterErrorData* getPixelClusterErrorData() const; 
  PixelChargeInterpolationParameters* getPixelChargeInterpolationParameters() const; 
  PixelClusterOnTrackErrorData* getPixelClusterOnTrackErrorData() const;

  // Get/Set the numerical constants 
  int GetNumberOfConstants() const;

  std::vector<float> GetConstants() const;
  void SetConstants(const std::vector<float> &constants);

  void Dump();

 private: 
  //  Identifier m_ident; 
  PixelClusterErrorData* m_clustererrordata; 
  PixelChargeInterpolationParameters* m_chargeinterpolationparameters; 
  PixelClusterOnTrackErrorData* m_clusterontrackerrordata;
}; 

//CLASS_DEF(PixelOfflineCalibData, 27089939, 1)

inline PixelOfflineCalibData::PixelOfflineCalibData() {
  m_clustererrordata = new PixelClusterErrorData();
  m_clusterontrackerrordata = new PixelClusterOnTrackErrorData();
  m_chargeinterpolationparameters = new PixelChargeInterpolationParameters();
} 

inline PixelOfflineCalibData::PixelOfflineCalibData(const PixelOfflineCalibData& rhs){
  m_clustererrordata = rhs.m_clustererrordata;
  m_clusterontrackerrordata = rhs.m_clusterontrackerrordata;
  m_chargeinterpolationparameters = rhs.m_chargeinterpolationparameters;
}

inline PixelOfflineCalibData& PixelOfflineCalibData::operator=(const PixelOfflineCalibData& rhs){
  if(this != &rhs){
    m_clustererrordata = rhs.m_clustererrordata;
    m_clusterontrackerrordata = rhs.m_clusterontrackerrordata;
    m_chargeinterpolationparameters = rhs.m_chargeinterpolationparameters;
  }
  return (*this);
}

inline bool PixelOfflineCalibData::update(const PixelClusterErrorData& idat){
  *m_clustererrordata = idat;
  return true;
}

inline bool PixelOfflineCalibData::update(
  const PixelChargeInterpolationParameters& idat){
  *m_chargeinterpolationparameters = idat;
  return true;
}

inline bool PixelOfflineCalibData::update(const PixelClusterOnTrackErrorData& idat){
  *m_clusterontrackerrordata = idat;
  return true;
}

inline PixelClusterErrorData* PixelOfflineCalibData::getPixelClusterErrorData() const {
  return m_clustererrordata;
}

inline PixelChargeInterpolationParameters* PixelOfflineCalibData::getPixelChargeInterpolationParameters() const {
  return m_chargeinterpolationparameters;
}

inline PixelClusterOnTrackErrorData* 
    PixelOfflineCalibData::getPixelClusterOnTrackErrorData() const {
  return m_clusterontrackerrordata;
}


//================ Destructor =================================================
inline PixelOfflineCalibData::~PixelOfflineCalibData(){
delete m_clustererrordata;
delete m_chargeinterpolationparameters;
delete m_clusterontrackerrordata;
}

}

CLASS_DEF(PixelCalib::PixelOfflineCalibData, 209342487, 1)
CLASS_DEF(CondCont<PixelCalib::PixelOfflineCalibData> , 213651723 , 1)

#endif 

