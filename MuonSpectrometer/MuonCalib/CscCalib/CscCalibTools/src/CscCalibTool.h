/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBTOOLBASE_H
#define CSCCALIBTOOLBASE_H

/*******************************************************************************
 Name    : CscCalibToolBase.h
 Package : offline/MuonSpectrometer/MuonCalib/MuonCalibTools
 Author  : Ketevi A. Assamagan
 Created : March 2005

 DESCRIPTION:	
     This class is the base class for run time CSC calibration tools. 
     These calibration tools need theCSC conditions data. The conditions data
     are written and read with a different set of tools. The production of the
     conditions data can be done via ATHENA or standalone
*******************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCondData/CscCondDataContainer.h"
#include "MuonCondInterface/CscICoolStrSvc.h"
#include "CscCalibTools/ICscCalibTool.h"

#include <inttypes.h>
#include <vector>
#include "TMath.h"
#include "TF1.h"
#include "TH1.h"


class CscCalibTool : virtual public ICscCalibTool, public AthAlgTool {

public:
  CscCalibTool(const std::string&, const std::string&, const IInterface*);
  virtual ~CscCalibTool ();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  
  /** given a charge on the CSC strip, convert that to ADC counts
      this is needed in the digitization for example where it is the charges
      on the strips that are simulated first and converter to AOD samples in
      a subsequent step. */
  virtual int femtoCoulombToADCCount (uint32_t stripHashId, const double femtoCoulombs) const ;
  
  /** Here the charge on the CSC strip is given in number of equivalent electrons;
      conversion to ADC counts */
  virtual int numberOfElectronsToADCCount(uint32_t stripHashId, const int numberOfElecEquiv) const ;
  
  /** given one CSC ADC sample value, convert that to charge in femtoCoulomb */ 
  virtual double adcCountToFemtoCoulomb (const float adcCounts, const float slope) const ;     
  virtual double adcCountToFemtoCoulomb (uint32_t stripHashId, const float adcCounts) const;     
  
  /** given one CSC ADC sample value, convert that to charge in number of equivalent electrons */
  virtual double adcCountToNumberOfElectrons (const float adcValue, const float slope) const;
  virtual double adcCountToNumberOfElectrons (uint32_t stripHashId, const float adcValue) const ;
  
  /** Conversion of ADC value to charge -
      Here the charges is returned in numbers of equivalent electrons */
  virtual bool adcToCharge(const std::vector<uint16_t>& samples, uint32_t stripHashId,
                                 std::vector<float>& charges) const;
  
  /** Given sampling values for a CSC strip, find the corresponding charge by fitting the time
      samples. By default, a parabolic fit is done. The charge and the time are returned.
      The time is calculated with respect to the time of the first sample. */
  virtual bool findCharge(const float samplingTime, const unsigned int samplingPhase,
                                const std::vector<float>& samples,double & charge, double & time) const;
  
  
  /** return the noise(sigma) on the readout strip in ADC counts or Number of Electrons 
      number of electrons by default: convert=true */
  virtual double stripNoise ( uint32_t stripHashId, const bool convert=true ) const;

  /** return the RMS on the readout strip in ADC counts or Number of Electrons 
      number of electrons by default: convert=true */
  virtual double stripRMS ( uint32_t stripHashId, const bool convert=true ) const;
  
  /** return the F001 on the readout strip in ADC counts or Number of Electrons 
      number of electrons by default: convert=true */
  virtual double stripF001 ( uint32_t stripHashId, const bool convert=true ) const;
  
  /** return the pedestal on the readout strip in ADC counts or Number of Electrons 
      - number of electrons by default: convert=true */
  virtual double stripPedestal ( uint32_t stripHashId, const bool convert=true ) const;

  /** return the status of this strip, good channel, dead channel, noisy channel -  
      it will return true for strip that working fine, false is returned for dead/noisy channels */
  virtual bool isGood ( uint32_t stripHashId ) const;

  /** return status bit **/
  virtual int stripStatusBit ( uint32_t stripHashId ) const;

  /** return T0phase related to 5 ASM. For convenience, we use stripHashId to get it **/
  virtual bool stripT0phase ( uint32_t stripHashId ) ;

  /** return T0base related to 5 ASM. For convenience, we use stripHashId to get it **/
  virtual double stripT0base ( uint32_t stripHashId ) ;

  /** these function used in the AOD <-> conversion; may not be needed once 
      we integrate the calibration service */
  virtual double func (const double x, const float slope) const;     
  virtual double func_prime (const double x, const float slope) const;     

  virtual double signal (const double z) const;     
  virtual double signal_amplitude (const double driftTime, const double samplingTime) const;     
  virtual double getZ0() const;
  
  virtual double getSamplingTime() const;
  virtual double getTimeOffset() const;
  virtual double getSignalWidth() const;
  virtual double getNumberOfIntegration() const;
  virtual double getNumberOfIntegration2() const;

  //  virtual Double_t bipfunc (const Double_t *x, const Double_t *par);
  //  virtual Double_t dualbipfunc (const Double_t *x, const Double_t *par);

  // To add bipolar functions and extract drftTime and stripCharge after addition,
  // this function is defined. Return value is pair and the first one is driftTime.
  virtual std::pair<double,double> addBipfunc(const double driftTime0, const double stripCharge0,
                                              const double driftTime1, const double stripCharge1);
  virtual std::string getDetDescr() const;

  // 09/2010
  virtual std::vector<float> getSamplesFromBipolarFunc(const double driftTime0, const double stripCharge0);
  virtual double getLatency() const;


  int m_messageCnt_t0base;
  int m_messageCnt_t0phase;
  //private:
  //  ../src/CscCalibTool.cxx: In member function 'virtual bool CscCalibTool::stripT0phase(uint32_t) const':
  //  ../src/CscCalibTool.cxx:351: error: increment of data-member 'CscCalibTool::m_messageCnt_t0phase' in read-only structure
  //  ../src/CscCalibTool.cxx: In member function 'virtual double CscCalibTool::stripT0base(uint32_t) const':
  //  ../src/CscCalibTool.cxx:371: error: increment of data-member 'CscCalibTool::m_messageCnt_t0base' in read-only structure
  // due to const in format?

private:
  float getPSlope(uint32_t stripHashId) const;
  
  
protected:
  
  ServiceHandle<MuonCalib::CscICoolStrSvc> m_cscCoolStrSvc;

  bool  m_readFromDatabase;
  bool  m_slopeFromDatabase;
 
  float m_slope;
  float m_noise;
  float m_pedestal;

  /** ROOT version of bipolar function **/
  // define bipolar functional shape
  // Parameters are from Kostas presentation at the following link (09/2007)
  // http://indico.cern.ch/getFile.py/access?contribId=1&resId=1&materialId=slides&confId=18787
  float m_integrationNumber;
  float m_integrationNumber2;

  float m_samplingTime;
  float m_signalWidth;
  float m_timeOffset; // bipolar's start time (or drift time)
  float m_timeOffsetRange;

  float m_latency; // new in 2010....latency may be controlled in CscCalibTool.
  float m_latencyInDigitization; // new in 12/2010 for New Digitization package...
  
  unsigned int m_nSamples; 
  TF1* m_addedfunc;
  TF1* m_bipolarFunc;

  bool m_onlineHLT;
  bool m_use2Samples; // for the use of only 2 samples for strip charge
  
};

#endif


