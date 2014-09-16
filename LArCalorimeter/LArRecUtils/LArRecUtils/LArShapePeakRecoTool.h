/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBUTILS_LARSHAPEPEAKRECOTOOL_H
#define LARCALIBUTILS_LARSHAPEPEAKRECOTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

//#include "StoreGate/StoreGateSvc.h"

static const InterfaceID IID_LArShapePeakRecoTool("LArShapePeakRecoTool", 1 , 0); 

class LArShapePeakRecoTool: public AthAlgTool

{
 public:
  
  // constructor
  LArShapePeakRecoTool(const std::string& type, 
		       const std::string& name, 
		       const IInterface* parent); 
  
  // destructor 
  virtual ~LArShapePeakRecoTool() {};

  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_LArShapePeakRecoTool; }

  
  std::vector<float> peak (const std::vector<float>& samples, const std::vector<double>& wave ) const ;
  void GetShapeParMax(float &xmax, float &ymax, float x1, float y1, float x2, float y2, float x3, float y3) const;
  void GetShapeSampleLimits(int &s1, int &s2, float sample_max, float delay_max, int nbin, int nsample) const;

  // initialize and finalize methods
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  float ParabolaRawToTrueTime(float& QT_fittime, int& layer) const ;
  float ParabolaRawToTrueADC(float& QT_true_time, float& ADCref, float& PEDref, int& layer) const ;
};


#endif
