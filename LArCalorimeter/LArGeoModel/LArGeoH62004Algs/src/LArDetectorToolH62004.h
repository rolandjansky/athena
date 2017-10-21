/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArDetectorToolH62004_H
#define LArDetectorToolH62004_H

#include "GeoModelUtilities/GeoModelTool.h"

class LArDetectorToolH62004 : public GeoModelTool {

public:

    // Standard Constructor
  LArDetectorToolH62004( const std::string& type, const std::string& name, const IInterface* parent );
  
  // Standard Destructor
  virtual ~LArDetectorToolH62004() override final;
  
  virtual StatusCode create() override final;
private:

  float m_xcryo;
  float m_ytable;
  bool m_isrun1;
  bool m_emec;
  bool m_hec;
  bool m_fcal;
  bool m_coldnose;
  bool m_printstep;
  bool m_checkprim;
  bool m_checkother;
  
};

#endif 
