/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBTOOLS_ICSCCALIBTOOL
#define MUONCALIBTOOLS_ICSCCALIBTOOL

/*********************************************************************************
 Name    : ICscCalibTool.h
 Package : offline/MuonSpectrometer/MuonCalib/CscCalib/CscCalibTools
 Author  : Ketevi A. Assamagan
 Created : March 2005

 DESCRIPTION:
     Abstract interface for run CSC calibration tools. Theses need the conditions
     data. The Tools to write and read the conditions data are elsewhere
*********************************************************************************/

#include "GaudiKernel/IAlgTool.h"
#include <inttypes.h>

#include <vector>

class ICscCalibTool : virtual public IAlgTool {

public:

  DeclareInterfaceID(ICscCalibTool, 1, 0);

  /** given a charge on the CSC strip, convert that to ADC counts
      this is needed in the digitization for example where it is the charges
      on the strips that are simulated first and converter to AOD samples in
      a subsequent step. */
  virtual int femtoCoulombToADCCount (uint32_t stripHashId, const double femtoCoulombs) const = 0;

  /** Here the charge on the CSC strip is given in number of equivalent electrons;
      conversion to ADC counts */
  virtual int numberOfElectronsToADCCount(uint32_t stripHashId, const int numberOfElecEquiv) const = 0;

  /** given one CSC ADC sample value, convert that to charge in femtoCoulomb */
  virtual double adcCountToFemtoCoulomb (const float adcValue, const float slope) const = 0;
  virtual double adcCountToFemtoCoulomb (uint32_t stripHashId, const float adcValue) const = 0;

  /** given one CSC ADC sample value, convert that to charge in number of equivalent electrons */
  virtual double adcCountToNumberOfElectrons (const float adcValue, const float slope) const = 0;
  virtual double adcCountToNumberOfElectrons (uint32_t stripHashId, const float adcValue) const = 0;

  /** Conversion of ADC value to charge -
      Here the charges is returned in numbers of equivalent electrons */
  virtual bool adcToCharge(const std::vector<uint16_t>& samples, uint32_t stripHashId,
                           std::vector<float>& charges) const = 0;

  /** Given sampling values for a CSC strip, find the corresponding charge by fitting the time
      samples. By default, a parabolic fit is done. The charge and the time are returned.
      The time is calculated with respect to the time of the first sample */
  virtual bool findCharge(const float samplingTime, const unsigned int samplingPhase,
                          const std::vector<float>& samples, double & charge, double & time) const = 0;

  /** return the noise (sigma) on the readout strip in ADC counts or Number of Electrons
      - number of electrons by default: convert=true */
  virtual double stripNoise ( uint32_t stripHashId, const bool convert=true ) const = 0;

  /** return the RMS on the readout strip in ADC counts or Number of Electrons
      - number of electrons by default: convert=true */
  virtual double stripRMS ( uint32_t stripHashId, const bool convert=true ) const = 0;

  /** return the F001 on the readout strip in ADC counts or Number of Electrons
      - number of electrons by default: convert=true */
  virtual double stripF001 ( uint32_t stripHashId, const bool convert=true ) const = 0;

  /** return the pedestal on the readout strip in ADC counts or Number of Electrons
      - in number of electrons by default: convert=true */
  virtual double stripPedestal ( uint32_t stripHashId, const bool convert=true ) const = 0;

  /** return the status of this strip, good channel, dead channel, noisy channel -
      it will return true for strip that working fine, false is returned for dead/noisy channels */
  virtual bool isGood ( uint32_t stripHashId ) const = 0;
  virtual int stripStatusBit ( uint32_t stripHashId ) const =0;

  /** these function used in the AOD <-> conversion; may not be needed once we integrate the calibration
      service */
  virtual double func (const double x, const float slope) const = 0;
  virtual double func_prime (const double x, const float slope) const = 0;

  virtual double signal (const double z) const=0;
  virtual double signal_amplitude (const double driftTime, const double samplingTime) const=0;
  virtual double getZ0() const=0;
  virtual double getSamplingTime() const=0;
  virtual double getSignalWidth() const=0;
  virtual double getTimeOffset() const=0;
  virtual double getNumberOfIntegration() const=0;
  virtual double getNumberOfIntegration2() const=0;

  virtual bool stripT0phase ( uint32_t stripHashId ) const =0;
  virtual double stripT0base ( uint32_t stripHashId ) const =0;


  virtual std::pair<double,double> addBipfunc(const double driftTime0, const double stripCharge0,
                                              const double driftTime1, const double stripCharge1) const =0;
  virtual std::string getDetDescr() const =0;

  virtual std::vector<float> getSamplesFromBipolarFunc(const double driftTime0, const double stripCharge0) const =0;
  virtual double getLatency() const =0;

protected:

};

#endif // MUONCALIBTOOLS_ICSCCALIBTOOL
