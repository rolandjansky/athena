/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBDATA_H
#define PIXELCALIBDATA_H

#include "CLIDSvc/CLASS_DEF.h"

#include <algorithm>
#include <vector>
#include "Identifier/Identifier.h"
#include "Identifier/Identifier32.h"
#include "PixelCoralClientUtils/PixelCalibData.hh"

/** @class PixelCalibData 

    The PixelCalibData is a class that designed to hold the calibration data for each module that contains 
    16 FE PixelChipSummaryData. The calibration data consist of threshold, timewalk information and Time over 
    threshold calibration that averages per FE chip. The details can be found at 
    https://twiki.cern.ch/twiki/bin/view/Atlas/PixelCOOLoffline#Calibrations
    .

    @author  Weiming Yao <WMYAO@LBL.GOV>
*/  

namespace PixelCalib {


  class PixelCalibData : public PixelCoralClientUtils::PixelCalibData {

  public:
  /** Constructor:*/
  PixelCalibData(); 
  PixelCalibData(const Identifier & ident, int nmax); 
  PixelCalibData(Identifier & ident, int nmax);  

  /** default destructor */
  ~PixelCalibData ();

  const Identifier& getModuleID() const; 

 private: 
  Identifier m_id; //<! module identifier
}; 

//CLASS_DEF(PixelCalibData, 27089939, 1)


inline PixelCalibData::PixelCalibData() : 
  PixelCoralClientUtils::PixelCalibData(),
  m_id(PixelCoralClientUtils::PixelCalibData::getModuleID()) 
{

} 

inline PixelCalibData::PixelCalibData(const Identifier& ident,int nmax)
  : 
  PixelCoralClientUtils::PixelCalibData(),
  m_id(ident)
  {
    m_ident =  m_id.get_identifier32().get_compact(); 
    m_nfeix = nmax;
  }
                                                                                
inline PixelCalibData::PixelCalibData(Identifier& ident,int nmax)
  : 
  PixelCoralClientUtils::PixelCalibData(),
  m_id(ident)
  {
    m_ident = m_id.get_identifier32().get_compact();
    m_nfeix = nmax;
  }
                                                                                                                

//================ Destructor =================================================
inline PixelCalibData::~PixelCalibData(){}


inline const Identifier& PixelCalibData::getModuleID() const { return m_id;} 

}
#endif 








