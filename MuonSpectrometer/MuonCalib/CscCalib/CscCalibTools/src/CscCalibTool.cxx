/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** Author: Ketevi A. Assamagan */

#include "CscCalibTool.h"
#include "StoreGate/DataHandle.h"
#include "EventInfo/TagInfo.h"
#include <sstream>

#include <cmath>

using namespace MuonCalib;
using std::ostringstream;
using std::setw;

CscCalibTool::CscCalibTool
( const std::string& t, const std::string& n, const IInterface*  p )
  : base_class(t,n,p),
    m_cscCoolStrSvc("MuonCalib::CscCoolStrSvc", n)
{
  declareProperty( "Slope", m_slope = 0.19 );
  declareProperty( "Noise", m_noise = 3.5 );
  declareProperty( "Pedestal", m_pedestal = 2048.0 );
  declareProperty( "ReadFromDatabase", m_readFromDatabase = true);
  declareProperty( "integrationNumber", m_integrationNumber = 12.0);
  declareProperty( "integrationNumber2", m_integrationNumber2 = 11.66);
  declareProperty( "samplingTime", m_samplingTime = 50. ); //ns
  declareProperty( "signalWidth", m_signalWidth = 14.40922); // 50/3.47ns
  declareProperty( "SlopeFromDatabase", m_slopeFromDatabase=false);
  declareProperty( "timeOffset", m_timeOffset = 46.825 );

  declareProperty( "IsOnline"  , m_onlineHLT = true); // This will be fed from jO
  
  // acceptable range for time offset for a successfull fit
  declareProperty( "TimeOffsetRange", m_timeOffsetRange = 1.0); 
 
  // new latency starting from 2010...
  declareProperty( "Latency", m_latency = 100 ); // ns.....
  declareProperty( "NSamples", m_nSamples = 4); // number of samples

  declareProperty( "Use2Samples", m_use2Samples = false); // force 2 sample
}

// ROOT USER Function
Double_t bipfunc(const Double_t *x, const Double_t *par){
  if (x[0] < par[1]) return 0.;
  Double_t integrationNumber  = par[2]; //12
  Double_t integrationNumber2 = par[3]; //11.66
  //  Double_t samplingTime = par[4]; //50 ns
  Double_t signalWidth = par[4]; //50 ns
  Double_t sum = integrationNumber+integrationNumber2;
  Double_t z0 = 0.5*( (sum+2)
                      -sqrt(std::pow(sum+2,2)
                            -4*integrationNumber*(integrationNumber2+1))
                      );

  Double_t norm = (1.0 - z0 / (1 + integrationNumber2))
    * TMath::Power(z0, 1.0 * integrationNumber)
    * TMath::Exp(-z0);

  Double_t z = (x[0]-par[1])/signalWidth;//*3.47/samplingTime;
  Double_t amplitude =
    par[0]*(1-z/(1+integrationNumber2))
    *TMath::Power(z,integrationNumber)*TMath::Exp(-1.0*z)/norm;
  return amplitude;
}
// To add two bipolar function and get a distribution, this function is defined
Double_t dualbipfunc(const Double_t *x, const Double_t *par){
  return ( bipfunc(x,par) + bipfunc(x,&par[5]) );
}




StatusCode CscCalibTool::initialize() {

  ATH_MSG_DEBUG ( "Initializing Initializing CscCalibTool");

  ATH_MSG_DEBUG ( "Default slope (if DB is not available)    =" << m_slope );
  ATH_MSG_DEBUG ( "Default noise (if DB is not available)    =" << m_noise );
  ATH_MSG_DEBUG ( "Default pedestal (if DB is not available) =" << m_pedestal );
  ATH_MSG_DEBUG ( "Calib Constants are from DB ?             =" << m_readFromDatabase );
  ATH_MSG_DEBUG ( "Slope Constants are from DB ?             =" << m_slopeFromDatabase );
  ATH_MSG_DEBUG ( "Bipolar function integrationNumber(N_1)   =" << m_integrationNumber);
  ATH_MSG_DEBUG ( "Bipolar function integrationNumber(N_2)   =" << m_integrationNumber2);
  ATH_MSG_DEBUG ( "SamplingTime                              =" << m_samplingTime);
  ATH_MSG_DEBUG ( "Signalwidth                               =" << m_signalWidth);
  ATH_MSG_DEBUG ( "Parabola time offset                      =" << m_timeOffsetRange);
  ATH_MSG_DEBUG ( "timeOffset  (digitization)                =" << m_timeOffset);
  ATH_MSG_DEBUG ( "Is OnlineAccess (HLT) ??                  =" << m_onlineHLT);
  ATH_MSG_DEBUG ( "Force the use of the 2 sample charge?     =" << m_use2Samples);

  if (m_onlineHLT) {
    ATH_MSG_DEBUG( "T0BaseFolder and T0PhaseFolder are not loaded!!! HLT COOLDB does not have it!!");
  }
  
  if ( m_cscCoolStrSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve pointer to the CSC COLL Conditions Service" );
    return StatusCode::FAILURE;
  }


  m_addedfunc   =new TF1("addedfunc", dualbipfunc, 0,500,10);
  m_bipolarFunc =new TF1("bipolarFunc",   bipfunc, -500,500,5);

  m_messageCnt_t0base=0;
  m_messageCnt_t0phase=0;

  return StatusCode::SUCCESS;
}

float CscCalibTool::getPSlope(uint32_t stripHashId) const {

  
  float slope = m_slope;
  if ( m_readFromDatabase && m_slopeFromDatabase ) {
     if ( !m_cscCoolStrSvc->getSlope(slope,stripHashId) ) {
       ATH_MSG_WARNING ( " failed to access CSC conditions database - slope - " 
                         << "strip hash id = " << stripHashId );
     }
  }
  ATH_MSG_DEBUG ( "The slope is " << slope << " for strip hash = " << stripHashId );

  return slope;

}


///////
int CscCalibTool::numberOfElectronsToADCCount(uint32_t stripHashId, const int numberOfElecEquiv) const 
{

  //ATH_MSG_VERBOSE ( "Using CscCalibTool::numberOfElectronsToADCCount" );

  double conversionFactor = 1.602e-4;  // 1 ee in femtoCoulomb
  double femtoCoulombs    = conversionFactor*numberOfElecEquiv;

  float slope = getPSlope(stripHashId);
  
  int adcValue = int ( func(femtoCoulombs,slope) );
  return adcValue;
}

int CscCalibTool::femtoCoulombToADCCount(uint32_t stripHashId, const double femtoCoulombs) const
{

  //ATH_MSG_VERBOSE ( "Using CscCalibTool::femtoCoulombToADCCount" );

  float slope = getPSlope(stripHashId);
  
  int adcValue = int ( func(femtoCoulombs,slope) );
  return adcValue;
}

/** conversion from ADC value to number of equivalent electrons */
double CscCalibTool::adcCountToNumberOfElectrons(const float adcValue, const float slope) const
{
  double conversionFactor = 1.602e-4;  // 1 ee in femtoCoulomb
  double femtoCoulombs = this->adcCountToFemtoCoulomb(adcValue, slope);
  return (femtoCoulombs/conversionFactor);
}

/** return the noise on the readout strip in ADC count or Number of Electrons */
double CscCalibTool::stripNoise ( uint32_t stripHashId, const bool convert ) const
{
  ATH_MSG_VERBOSE ( "The strip hash id is " <<  stripHashId );

  float noise = m_noise;  /// ADC counts
  if ( m_readFromDatabase ) {
    if ( !m_cscCoolStrSvc->getNoise(noise,stripHashId) ) {
      ATH_MSG_DEBUG ( " failed to access CSC conditions database - noise - " 
                      << "strip hash id = " << stripHashId );
      noise = m_noise;
    } 
  } 
  
  ATH_MSG_VERBOSE ( "The noise is " << noise << " for strip hash = " << stripHashId ); 

  if ( convert ) {
    float slope = getPSlope(stripHashId);
    return this->adcCountToNumberOfElectrons( noise, slope );
  }  else {
    return noise;
  }
}


/** return the rms on the readout strip in ADC count or Number of Electrons */
double CscCalibTool::stripRMS ( uint32_t stripHashId, const bool convert ) const
{
  ATH_MSG_VERBOSE ( "The strip hash id is " <<  stripHashId );

  float rms = m_noise;  /// ADC counts initialized with m_noise...
  if ( m_readFromDatabase ) {
    if ( !m_cscCoolStrSvc->getRMS(rms,stripHashId) ) {
      ATH_MSG_DEBUG ( " failed to access CSC conditions database - rms - " 
                      << "strip hash id = " << stripHashId );
      rms = m_noise;
    } 
  } 
  
  ATH_MSG_VERBOSE ( "The RMS is " << rms << " for strip hash = " << stripHashId ); 

  if ( convert ) {
    float slope = getPSlope(stripHashId);
    return this->adcCountToNumberOfElectrons( rms, slope );
  }  else {
    return rms;
  }
  
}

/** return the F001 on the readout strip in ADC count or Number of Electrons */
// NOTE: f001 is raw ADC count...+1 
double CscCalibTool::stripF001 ( uint32_t stripHashId, const bool convert ) const
{
  ATH_MSG_VERBOSE ( "The strip hash id is " <<  stripHashId );

  float f001 = m_noise+m_pedestal;  /// ADC counts initialized with m_noise...
  if ( m_readFromDatabase ) {
    if ( !m_cscCoolStrSvc->getF001(f001,stripHashId) ) {
      ATH_MSG_DEBUG ( " failed to access CSC conditions database - f001 - " 
                      << "strip hash id = " << stripHashId );
      f001 = 3.251*m_noise+m_pedestal;
    } 
  } 
  
  ATH_MSG_VERBOSE ( "The F001 is " << f001 << " for strip hash = " << stripHashId ); 

  if ( convert ) {
    float slope = getPSlope(stripHashId);
    return this->adcCountToNumberOfElectrons( f001, slope );
  }  else {
    return f001;
  }
}

/** return the pedestal on the readout strip in ADC counts or number of electrons */
double CscCalibTool::stripPedestal ( uint32_t stripHashId, const bool convert ) const
{
  ATH_MSG_VERBOSE ( "The strip hash id is " <<  stripHashId );

  float pedestal = m_pedestal;
  if ( m_readFromDatabase ) {
    if ( !m_cscCoolStrSvc->getPedestal(pedestal,stripHashId) ) {
      ATH_MSG_DEBUG ( " failed to access CSC conditions database - pedestal - " 
                      << "strip hash id = " << stripHashId );
      pedestal = m_pedestal;
    }
  }
  ATH_MSG_VERBOSE ( "The pedestal is " << pedestal << " for strip hash = " << stripHashId );
  
  if ( convert ) {
    float slope = getPSlope(stripHashId);
    return this->adcCountToNumberOfElectrons( pedestal, slope );
  }  else {
    return pedestal;
  }
}

/** return the status of this strip, good channel, dead channel, noisy channel -  
    it will return true for strip that working fine, false is returned for 
    dead/noisy channels */
bool CscCalibTool::isGood ( uint32_t stripHashId ) const
{
  
  ATH_MSG_VERBOSE ( "The strip hash id is " <<  stripHashId );
  
  unsigned int status = stripStatusBit(stripHashId);
  bool is_good = !( (status & 0x1) || ((status >> 1) & 0x1) ); // test for hot/dead channel
  return is_good;
}


int CscCalibTool::stripStatusBit ( uint32_t stripHashId ) const {
  
  uint32_t status = 0x0;
  if ( m_readFromDatabase ) {
    if ( !m_cscCoolStrSvc->getStatus(status,stripHashId) ) {
      ATH_MSG_WARNING ( " failed to access CSC conditions database - status - "
                        << "strip hash id = " << stripHashId );

      uint8_t status2 = 0x0;
      if ( (m_cscCoolStrSvc->getStatus(status2,stripHashId)).isFailure() ) {
        ATH_MSG_WARNING ( " failed to access CSC conditions database old way - status - "
                          << "strip hash id = " << stripHashId );
      }else{
        ATH_MSG_INFO ( " Accessed CSC conditions database old way - status - "
                          << "strip hash id = " << stripHashId );
      }
    } else {
      ATH_MSG_VERBOSE ( "The status word is " << std::hex << status 
                        << " for strip hash = " << std::dec << stripHashId );
    }
  }
  return status;
}



bool CscCalibTool::stripT0phase ( uint32_t stripHashId ) const {

  bool t0phase = 0;

  if (! m_onlineHLT ) {
    if ( m_readFromDatabase ) {
      if ( !m_cscCoolStrSvc->getT0Phase(t0phase,stripHashId) ) {
        if (m_messageCnt_t0phase < 3) {
          ATH_MSG_WARNING ( " failed to access CSC conditions database - t0phase - "
                            << "strip hash id = " << stripHashId );
          ATH_MSG_WARNING ( " This WARNING Message can be temporarily until COOL DB is filled");
          ++m_messageCnt_t0phase;
        }
      } else {
        ATH_MSG_VERBOSE ( "The t0phase is " << t0phase << " for stripHashId " << stripHashId );
      }
    }
  }
  return t0phase;
}


double CscCalibTool::stripT0base ( uint32_t stripHashId ) const {

  float t0base = 0.0;
  if (! m_onlineHLT ) {
    if ( m_readFromDatabase ) {
      if ( !m_cscCoolStrSvc->getT0Base(t0base,stripHashId) ) {
        
        if (m_messageCnt_t0base < 3) {
          ATH_MSG_WARNING ( " failed to access CSC conditions database - t0base - "
                            << "strip hash id = " << stripHashId );
          ATH_MSG_WARNING ( " This WARNING Message can be temporarily until COOL DB is filled");
          ++m_messageCnt_t0base;
        }
      } else {
        ATH_MSG_VERBOSE ( "The t0base is " << t0base << " for stripHashId " << stripHashId );
      }
    }
  }
  return t0base;
}



double CscCalibTool::adcCountToFemtoCoulomb(const float adc, const float slope) const
{

  double charge = adc * slope;
  ATH_MSG_VERBOSE ( "Using CscCalibTool::adcCountToFemtoCoulomb - adc = " 
                  << adc << " charge(fC) = " << charge );
  return charge;
} 

double CscCalibTool::func(const double x, const float slope) const 
{
  int val = 0;
  if ( slope != 0 ) val = int ( (x / slope) + 0.5);
  //  else
  //    ATH_MSG_WARNING ( "CscCalibTool::femtoCoulombToADC - slope = 0" );
  //  if ( val < 0 ) {
  //    ATH_MSG_WARNING ( "CscCalibTool::femtoCoulombToADC - ADC cannot be < 0 - " << val );
  //val = 0;
  //  }
  
  return (1.0*val);
}

double CscCalibTool::func_prime(const double x, const float slope) const
{
  double val = slope;
  if ( slope == 0.0 )
    ATH_MSG_WARNING ( "CscCalibTool - slope = 0 for x = " << x );
  return val;
}

// 
bool CscCalibTool::findCharge(const float samplingTime, const unsigned int samplingPhase,
                             const std::vector<float>& samples, double & charge, double & time) const {
  
  time = 0.0;
  charge = 0.0;

  int numberOfSamplings = samples.size();
  // no samples given
  if (numberOfSamplings==0) return false;
  
  // MS: The case of only 2 samples:
  if ((numberOfSamplings==2) || m_use2Samples) { // no parabola possible
    int i = numberOfSamplings/2-1;
    charge = 0.5*(samples[i]+samples[i+1]); // 1+2 for 4 samples, 0+1 for 2

    double asym = 0.;
    if (fabs(samples[i]+samples[i+1])>0.0001)  asym = (samples[i+1]-samples[i])/(samples[i]+samples[i+1]);
    /********************* No charge correction now.
    // charge correction:
    double chargecor = 77.85 + 1.415*asym - 44.97*asym*asym; // in percent
    charge = charge / chargecor * 100.0;
    *******************/

    // time = i+0.5; // midpoint beween the 2 samples: no interpolation
    // time *= samplingTime;
    time = 76 + 47.85*asym + 6.629*asym*asym; // in ns, for 50ns sampling
    if ( samplingPhase == 1 ) time -= 25;
    return true;
  }

  /** find the maximum */
  float max = -4096000;
  int maxIndex = -1;

  if ( numberOfSamplings >10) { // for x5 data
    maxIndex = 2;
    max = samples[maxIndex];
  }
  
  for (int i=0; i<numberOfSamplings; i++) {
    if ( numberOfSamplings >10 && (i<2 || i>6) ) continue; // for x5 data
    if (samples[i] > max) { 
      max = samples[i];
      maxIndex = i;
    } 
  }
  
  /** all the samples are zeros */
  //  if (max == -4096000) return false; //not possible
  

  /** now the parabolic interpolation */
  
  double a, b, c;

  int midIndex = maxIndex;
  if (maxIndex == 0) { // peaks on the first sample
    a = samples[maxIndex];
    b = samples[maxIndex+1];
    c = samples[maxIndex+2];
    midIndex +=1; 
  } else if (maxIndex == numberOfSamplings-1) { // last sample but it won't happen if third and 4th samples are same...
    a = samples[maxIndex-2];
    b = samples[maxIndex-1];
    c = samples[maxIndex];
    midIndex -=1;
  } else { // normal case
    a = samples[maxIndex-1];
    b = samples[maxIndex];
    c = samples[maxIndex+1];
  } 

  //#D!!!!! Need to carefully check the case of 250 * 23898.136 * 28535.650 * 33587.949 * 33587.949 *
  
  double aa = 0.5*(c+a-2*b); // p2 (coeff for x^2)
  double bb = 0.5*(c-a);

  
  /** a==0: 3 points on a line, no parabola possible */
  /** a>0:  convex parabola isn't useful for peak finding  */
  if ( aa >= 0 ) { 
    time = midIndex;
    time *= samplingTime;
    if ( samplingPhase == 1 ) time -= 25; // works for both 20MHz and 40MHz
    //    time =0.0;
    charge = max;

    if (maxIndex ==0 || maxIndex ==1)
      time -= 1000;
    else if (maxIndex ==2 || maxIndex ==3)
      time += 1000;

    ATH_MSG_VERBOSE("WP aa is positive");
    return false;
  }
  
  double timeOffset = -0.5*bb/aa;

  ATH_MSG_VERBOSE("WP " << timeOffset << " " << m_timeOffsetRange); 
  /** if the time offset is out of range **/ 
  if ( ( maxIndex == 0 && timeOffset < -2.0 )
       || ( maxIndex == 3 && timeOffset > 2.0) ) {
    //       || ( ( maxIndex == 1 || maxIndex ==2) && fabs(timeOffset) > m_timeOffsetRange) ) {
    time = midIndex;
    time *= samplingTime;
    if ( samplingPhase == 1 ) time -= 25; // works for both 20MHz and 40MHz
    //    time =0.0;
    charge = max;

    if (maxIndex ==0 || maxIndex ==1)
      time -= 1000;
    else if (maxIndex ==2 || maxIndex ==3)
      time += 1000;

    ATH_MSG_VERBOSE("time is out of range");
    
    return true;
  }

  /** successful interpolation */
  charge = aa*timeOffset*timeOffset + bb*timeOffset + b;
  time = timeOffset;
  
  /** Find the peaking time: this is no longer assuming a 4-time sample system
      The peaking time is the time of the largest sample. 
      This is corrected by the timeOffset interpolation, 
      which should range from -0.5 to 0.5. 
      @todo find out if a larger range can be used, such as +-1.0
      
      Multiply by sampling time to convert to nanoseconds.
      Finally subtract 25ns if the sampling phase is 1.
  */
  time += midIndex;
  time *= samplingTime;
  if ( samplingPhase == 1 ) time -= 25;
  
  return true;
}

double CscCalibTool::adcCountToFemtoCoulomb(uint32_t stripHashId, const float adcValue) const
{
  ATH_MSG_VERBOSE ( "The strip hash id is " <<  stripHashId );

  /** the strip hash id will be used to access the data base */

  /** subtract the pedestal */
  float pedestal = m_pedestal;
  float slope    = getPSlope(stripHashId);

  if ( m_readFromDatabase ) {
    if ( !m_cscCoolStrSvc->getPedestal(pedestal,stripHashId) ) {
      ATH_MSG_DEBUG ( " failed to access CSC conditions database - pedestal - " 
                      << "strip hash id = " << stripHashId );
      pedestal = m_pedestal;
    }
  }
  ATH_MSG_VERBOSE ( "Pedestal is " << pedestal << " For strip hash id " << stripHashId );
  
  // allowed negative adc values for bipolar fit
  float adc = adcValue-pedestal;
  return this->adcCountToFemtoCoulomb( adc, slope );
  
}

double CscCalibTool::adcCountToNumberOfElectrons(uint32_t stripHashId, const float adcValue) const
{
  ATH_MSG_VERBOSE ( "The strip hash id is " <<  stripHashId );

  /** the strip hash id will be used to access the data base */

  /** subtract the pedestal */
  float pedestal = m_pedestal;
  float slope    = getPSlope(stripHashId);
  if ( m_readFromDatabase ) {
     if ( !m_cscCoolStrSvc->getPedestal(pedestal,stripHashId) ) {
       ATH_MSG_DEBUG ( "failed to access CSC Conditions database - pedestal - " 
                       << "strip hash id = " << stripHashId );
       pedestal = m_pedestal;
     }
  }
  ATH_MSG_VERBOSE ( "Pedestal is " << pedestal << " For strip hash id " << stripHashId );

  // allowed negative adc values for bipolar fit
  float adc = adcValue-pedestal;
  return this->adcCountToNumberOfElectrons( adc, slope );
}

bool CscCalibTool::adcToCharge(const std::vector<uint16_t>& samples, uint32_t stripHashId,
                               std::vector<float>& charges) const {
   ATH_MSG_VERBOSE ( "The strip hash id is " <<  stripHashId );

   charges.clear();

   /** subtract the pedestal */
   float pedestal = m_pedestal;
   float slope    = getPSlope(stripHashId);
   if ( m_readFromDatabase ) {
      if ( !m_cscCoolStrSvc->getPedestal(pedestal,stripHashId) ) {
        ATH_MSG_DEBUG ( "failed to access CSC Conditions database - pedestal - " 
                        << "strip hash id = " << stripHashId );
        pedestal = m_pedestal; 
      }     
   } 
   ATH_MSG_VERBOSE ( "Pedestal is " << pedestal << " For strip hash id " << stripHashId );

   unsigned max = samples.size();
   if ( max == 0 ) return false;
   for (unsigned int i=0; i<max; i++) {
     //     if ( samples[i] > pedestal ) {
     float adc = samples[i]-pedestal;
     float charge = static_cast<float> ( this->adcCountToNumberOfElectrons( adc, slope ) );
     charges.push_back( charge );
        // } else charges.push_back( 0.0 );
   }
   return true;
}

/** ROOT version of bipolar function **/
// define bipolar functional shape
// Parameters are from Kostas presentation at the following link (09/2007)
// http://indico.cern.ch/getFile.py/access?contribId=1&resId=1&materialId=slides&confId=18787

double CscCalibTool::getZ0() const{
  double sum = m_integrationNumber+m_integrationNumber2;
  double z0 = 0.5*( (sum+2)
                    -sqrt(std::pow(sum+2,2)
                          -4*m_integrationNumber*(m_integrationNumber2+1))
                    );
  return z0;
}

double CscCalibTool::signal( const double z ) const{
  double amplitude = (1.0 - z / (1 + m_integrationNumber2))
    * std::pow(z, 1.0 * m_integrationNumber)
    * exp(-z);
  return amplitude;
}

double CscCalibTool::signal_amplitude(const double driftTime, const double samplingTime) const{
  double z0 = getZ0();
  double norm = signal(z0);
  if (samplingTime <= driftTime) return 0.;
  Double_t z = (samplingTime-driftTime)/m_signalWidth;
  return signal(z)/norm;
}


// new schema starting from 09/2010...
std::vector<float> CscCalibTool::getSamplesFromBipolarFunc(const double driftTime0, const double stripCharge0) const {
  
  std::vector<float> result;
  if ( stripCharge0==0.0 ) {
    result.push_back(0.0);
    result.push_back(0.0);
    result.push_back(0.0);
    result.push_back(0.0);
    return result;
  }
  
  m_bipolarFunc->SetParameters(stripCharge0, driftTime0,
                               m_integrationNumber,m_integrationNumber2,m_signalWidth);


  for (unsigned int i=0; i<m_nSamples; ++i) {

    float sampleCharge = m_bipolarFunc->Eval(m_latency + i*m_samplingTime);
    result.push_back( sampleCharge );

  }
  return result;
}


std::pair<double,double> CscCalibTool::addBipfunc(const double driftTime0,
                                                      const double stripCharge0,
                                                      const double driftTime1,
                                                      const double stripCharge1) const {
  
  std::pair<double,double> result;
  // To get a nomalization constant...
  result.first =driftTime0;
  result.second = stripCharge0;

  if ( (stripCharge0==0.0 && stripCharge1==0.0)||
       (stripCharge0>0.0 && stripCharge1==0.0))
    return result;

  if (stripCharge0==0.0 && stripCharge1>0.0) {
    result.first =driftTime1;
    result.second = stripCharge1;
    return result;
  }
  
  m_addedfunc->SetParameters(stripCharge0, driftTime0,
                           m_integrationNumber,m_integrationNumber2,m_signalWidth,
                           stripCharge1, driftTime1,
                           m_integrationNumber,m_integrationNumber2,m_signalWidth);
  result.second =m_addedfunc->GetMaximum(); // ==>stripCharges of added bipolars
  float tmax =m_addedfunc->GetX(result.second);
  result.first = tmax - getZ0()*m_signalWidth;

  if (stripCharge0>0.0 && stripCharge1>0.0)
    return result;

  float bipmin = m_addedfunc->GetMinimum(); // ==>stripCharges of added bipolars
  float tmin = m_addedfunc->GetX(bipmin);
  if (tmin<tmax) {
    result.first = tmin-getZ0()*m_signalWidth;
    result.second = bipmin;
  }
    

  
  //  To check out conversion is correct...
  ATH_MSG_VERBOSE ( "(" << driftTime0 << ":" << int(stripCharge0) << ")"
                    << "+(" << driftTime1 << ":" << int(stripCharge1) << ")"
                    << " ==> " << result.first << ":" << int(result.second)
                    << " e-  which was " << int(stripCharge0+stripCharge1) );


  
  return result;
}

double CscCalibTool::getSamplingTime()      const {return m_samplingTime;}
double CscCalibTool::getLatency()      const {return m_latency;}
double CscCalibTool::getTimeOffset()        const {return m_timeOffset;}
double CscCalibTool::getSignalWidth()       const {return m_signalWidth;}
double CscCalibTool::getNumberOfIntegration()  const {return m_integrationNumber;}
double CscCalibTool::getNumberOfIntegration2() const {return m_integrationNumber2;}


std::string CscCalibTool::getDetDescr() const {

  std::string detdescr = "";

  const DataHandle<TagInfo> tagInfo;
  if (detStore()->retrieve(tagInfo).isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve tag info  from TDS. - abort ..." );
    return detdescr;
  }

  tagInfo->findTag("GeoAtlas", detdescr);
  ATH_MSG_VERBOSE ( "DetDescr tag = " << detdescr);
  return detdescr;

}

