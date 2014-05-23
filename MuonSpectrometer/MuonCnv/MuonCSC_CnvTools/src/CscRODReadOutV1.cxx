/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CscRODReadOutV1.h"

// constructor
CscRODReadOutV1::CscRODReadOutV1() {  

  TIME_OFFSET     = 46.825;  // ns
  SIGNAL_WIDTH    = 16.08;   // ns
  SAMPLING_TIME   = 50.0;    // ns
  NUMBER_OF_INTEGRATION = 12;
  Z0 = (NUMBER_OF_INTEGRATION+1) 
       -sqrt(NUMBER_OF_INTEGRATION+1);// time bin at the maximum 
                                      // obtained by setting the derivative = 0
                                      // this gives 2 solutions: 
                                      // Z0=9.394 and 16.606
                                      // 9.394 is for positive amplitude

  // conversion factor from ee charge to ADC count
  // assuming for now 1 ADC count = 0.32 femtoCoulomb!

  /* From Valeri Tcherniatine --- April 11, 2004
    conversion= ( e*G*k*m*d*1.6e-19)/2000
    e=75 - average number ionization e in CSC
    G=10^5 - gas gain
    k=0.15 - factor taking to account electronic time integration (charge
    deficit) and only one cathode readout
    m=0.5 - part of induce charge contained in max. strip
    d=7 - value of dynamic range expressed in average particle ionization
    deposition in CSC. At this value (7) out of region inefficiency is <2%.
    1.6e-19 - e charge
    2000 - max. ADC counts for positive part of signal
    Collect all numbers together conversion = 0.32 femtoCoulomb per ADC count
  */


  CHARGE_TO_ADC_COUNT = (0.32e-15) / (1.602e-19); 
  m_norm = signal(Z0);

  // trigger info : TDC, time, etc
  for (int i=0; i<3; i++) TRIGGER_INFO[i] = 0;

}

CscRODReadOutV1::CscRODReadOutV1(double startTime, uint16_t samplingTime,
                             double signalWidth, uint16_t numIntegration) {

  TIME_OFFSET     = startTime;            // ns
  SIGNAL_WIDTH    = signalWidth;          // ns
  SAMPLING_TIME   = samplingTime;         // ns
  NUMBER_OF_INTEGRATION = numIntegration;
  Z0 = (NUMBER_OF_INTEGRATION+1) 
       -sqrt(NUMBER_OF_INTEGRATION+1);    // time bin at the maximum 
                                          // obtained by setting the derivative = 0
                                          // this gives 2 solutions: 
                                          // Z0=9.394 and 16.606
                                          // 9.394 is for positive amplitude

  CHARGE_TO_ADC_COUNT = (0.32e-15) / (1.602e-19); 
  m_norm = signal(Z0);

  // trigger info : TDC, time, etc
  for (int i=0; i<3; i++) TRIGGER_INFO[i] = 0;

}

// destructor
CscRODReadOutV1::~CscRODReadOutV1()
{}

void CscRODReadOutV1::encodeFragments(const std::vector<uint16_t>  amplitude,  
				    std::vector<uint32_t>& v) const {
  
  int numberOfFragments = amplitude.size();
  
  // now the data
  /** pedestal */

  int j=0;
  while (j < numberOfFragments) {
    uint32_t v32=0;
    uint16_t amp[2]={0,0};
    for (int i=0; i<2; i++) {
      amp[i] = (BODY_AMPLITUDE << 12) | amplitude[i+j];
    }
    set32bits(amp,v32);
    v.push_back( v32 );
    j += 2; 
  }
}

int CscRODReadOutV1::findCharge(std::vector<uint16_t> amplitude, double& time) {

  // very crude - to be done better

  int charge = 0;
  time = 0.0;

  int numberOfSamplings = amplitude.size();

  uint16_t max = 0;
  int maxIndex = -1;
  for (int i=0; i<numberOfSamplings; i++) {
    if (amplitude[i] > max) {
      max = amplitude[i];
      maxIndex = i;
    } 
  }

  if (max == 0) return charge;
  if (maxIndex < 0 || maxIndex >= numberOfSamplings) return charge;
 
  /// do a parabolic fit - from Nir and David
  if ( maxIndex == 0 ) return amplitude[0];
  else if ( maxIndex == (numberOfSamplings-1) ) return amplitude[numberOfSamplings-1];
  else {
    double a, b, c;
    double y1 = amplitude[maxIndex-1];
    double y2 = amplitude[maxIndex];
    double y3 = amplitude[maxIndex+1];
    a = 0.5*(y3+y1-2*y2);
    b = 0.5*(y3-y1);
    c = y2;

    /// calculate the charge and the time
    /// need to use the correct calibration
    double offset = (a == 0) ? 0 : -b/(2*a); 
    charge = static_cast<int> ( a*offset*offset + b*offset + c - amplitude[0] ); /// assuming amplitude[0] gives the pedestal
    time = (maxIndex+offset)*SAMPLING_TIME;
    return charge;
  }
}
