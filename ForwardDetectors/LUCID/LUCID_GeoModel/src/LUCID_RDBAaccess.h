/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RDBAaccess_h
#define LUCID_RDBAaccess_h

#include "GaudiKernel/Algorithm.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class LUCID_RDBAccess { 

 public: 
  
  LUCID_RDBAccess();  
  ~LUCID_RDBAccess(); 
  
  void ReadDB(); 
  void SetParameters(); 
  void Print();
  void CloseDB(); 
  
  double distanceToIP;  

  double VJdistanceToIP;
  double VJconelength;
  double VJconeRadiusFront;
  double VJconeRadiusBack;
  double VJconeThickness;
  double VJconeFrontRingThickness;
  double VJconeFrontRingLength;
  double VJconeFrontRingOverlap;

  double vesselInnerRadius;  
  double vesselInnerThickness;  
  double vesselOuterRadMin;  
  double vesselOuterRadMax;  
  double vesselOuterThickness;  
  double vesselLength;  
  
  double bulkheadThickness;  
  
  double coolingRadius;  
  double coolingThickness;  
  
  double layerRadius1;  
  double layerRadius2;  
  
  double tubeRadius;  
  double tubeThickness;  
  double tubeLength;  
  
  double pmtThickness;  
  
  double gasPressure;  
  double gasDensity;  
  double gasTemperature;  
  
  double quartzDensity;    

  double tubePolish;    

  double waveLengthStep;  
  double waveLengthMin;  
  double waveLengthMax;  

 private: 
  
  IRDBRecordset_ptr m_lucidParams; 
  ISvcLocator* m_svcLocator; 
}; 

#endif
