/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBUTILS_LARPARABOLAPEAKRECOTOOL_H
#define LARCALIBUTILS_LARPARABOLAPEAKRECOTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

/**
  @brief Given a set of multiple samples, this class finds a peak using Parabola fit.

*/

static const InterfaceID IID_LArParabolaPeakRecoTool("LArParabolaPeakRecoTool", 1 , 0); 

class LArParabolaPeakRecoTool: public AthAlgTool

{
 public:
  
  // constructor
  LArParabolaPeakRecoTool(const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent); 
  
  // destructor 
  virtual ~LArParabolaPeakRecoTool() {};

  static const InterfaceID& interfaceID() { return IID_LArParabolaPeakRecoTool;}
  
  // returns a vector: vector[0]=ADCReco, vector[1]=TimeReco
  std::vector<float> peak (const std::vector<float>& samples) const;
  // short needed for backward compatability 
  std::vector<float> peak (const std::vector<short>& samples) const;
  // second method in case one want to apply bias correction
  std::vector<float> peak (const std::vector<float>& samples, int layer, float pedestal) const;
  std::vector<float> peak (const std::vector<short>& samples, int layer, float pedestal) const;



  // initialize and finalize methods
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  float ParabolaRawToTrueTime(float& QT_fittime, int& layer) const ;
  float ParabolaRawToTrueADC(float& QT_true_time, double& ADCref, float& PEDref, int& layer) const ;

  // Avoid ambiguity.
  //static const InterfaceID& interfaceID() { return IAlgTool::interfaceID(); }

 private:

  // jobOptions for bias correction 
  bool m_correctBias;
  std::string m_fileShapeName, m_fileADCcorName;

  // file and tables for corrections
  FILE* m_fileShape = nullptr, *m_fileADCcor = nullptr;
  float m_QT_Shape[4][26]{};
  float m_QT_ADCcor[4][25]{};
};


#endif
