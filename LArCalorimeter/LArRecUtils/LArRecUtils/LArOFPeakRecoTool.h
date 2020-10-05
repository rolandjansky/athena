//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBUTILS_LAROFPEAKRECOTOOL_H
#define LARCALIBUTILS_LAROFPEAKRECOTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArShape.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawEvent/LArOFIterResults.h"
#include "StoreGate/ReadCondHandleKey.h"

class LArOnlineID_Base;

/**
  @brief Given a set of multiple samples, this class finds a peak using OFC.

*/

static const InterfaceID IID_LArOFPeakRecoTool("LArOFPeakRecoTool", 1 , 0); 


class LArOFPeakRecoTool: public AthAlgTool

{
 public:

  /*
  struct Result{
    Result(): valid(false),converged(false) {}  
    bool  valid;
    bool  converged;
    float amplitude;  //in ADC counts	
    float tau;        //in ns
    float quality;  
    float delay;      //in ns
    unsigned peakSample;
  } ; 
  */
  typedef LArOFIterResults Result;
  // constructor
  LArOFPeakRecoTool(const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent); 
  
  // destructor 
  virtual ~LArOFPeakRecoTool();
  
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_LArOFPeakRecoTool;}

  LArOFIterResults peak (const std::vector<short>& samples, HWIdentifier chID, CaloGain::CaloGain gain, int delay) const;

  LArOFIterResults peak(const std::vector<float>& samples, // raw data after pedestal subtraction
			const HWIdentifier chID,           // online channel id
			const CaloGain::CaloGain gain,     // gain 
			const float delayIn,               // initial delay for Shape and OFC  
			const unsigned  nIter=0,		  // number of iteration 
			const unsigned npeak=2,            // initial peak position.                  
			unsigned peak_low=2,         // lower limit for peak position
			unsigned peak_high=2        // upper limit for peak position
			) const;

  // initialize and finalize methods
  virtual StatusCode initialize();
  virtual StatusCode finalize() {return StatusCode::SUCCESS ; } ;

  // virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  enum { ERROR = -99999999 } ;

 private:
  bool m_iterate;
  SG::ReadCondHandleKey<ILArOFC>    m_keyOFC {this, "OFCKey", "LArOFC", "SG key for OFC object"};
  SG::ReadCondHandleKey<ILArShape>  m_keyShape {this, "ShapeKey", "LArShape", "SG key for Shape object"};
  bool m_useShape;
  bool m_useShapeDer;
  int m_delayShift;
  float m_samplingPeriod;
  bool m_forceHighGain;
  bool m_isSC;
  const LArOnlineID_Base*        m_lar_on_id; 

};


#endif
