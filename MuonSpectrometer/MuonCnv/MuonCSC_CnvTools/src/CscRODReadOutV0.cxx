/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CscRODReadOutV0.h"

// constructor
CscRODReadOutV0::CscRODReadOutV0() :
    m_cscHelper(nullptr),
    m_sourceID(0),
    m_moduleType(0),
    m_rodId(0),
    m_subDetectorId(0),
    m_amp1(0),
    m_amp2(0),
    m_amp3(0),
    m_amp4(0),
    m_address(0) {
    m_TIME_OFFSET = 46.825;  // ns
    m_SIGNAL_WIDTH = 16.08;  // ns
    m_SAMPLING_TIME = 50.0;  // ns
    m_Z0 = 9.394;            // time bin at the maximum
                             // obtained by setting the derivative = 0
                             // this gives 2 solutions: Z0=9.394 and 16.606
                             // 9.394 is for positive amplitude

    m_NUMBER_OF_INTEGRATION = 12;

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

    m_CHARGE_TO_ADC_COUNT = (0.32e-15) / (1.602e-19);
    m_norm = signal(m_Z0);
}

// destructor
CscRODReadOutV0::~CscRODReadOutV0() {}

void CscRODReadOutV0::encodeFragments(const uint16_t* amplitude, const uint32_t& address, std::vector<uint32_t>& v) const {
    uint16_t amp[2] = {0, 0};
    uint32_t v32 = 0;

    for (int i = 0; i < 2; i++) amp[i] = (BODY_AMPLITUDE << 12) | *(amplitude + i);
    set32bits(amp, v32);
    v[0] = v32;
    v32 = 0;
    for (int i = 2; i < 4; i++) amp[i - 2] = (BODY_AMPLITUDE << 12) | *(amplitude + i);
    set32bits(amp, v32);
    v[1] = v32;

    v[2] = (BODY_ADDRESS << 28) | address;
}

double CscRODReadOutV0::findCharge() {
    const int N_SAMPLE = 4;

    // the sampling times
    double time[N_SAMPLE];
    for (int i = 0; i < N_SAMPLE; i++) time[i] = (i + 1) * m_SAMPLING_TIME + m_TIME_OFFSET;

    // the corresponding amplitudes
    uint16_t amp[N_SAMPLE];
    amp[0] = m_amp1;
    amp[1] = m_amp2;
    amp[2] = m_amp3;
    amp[3] = m_amp4;

    // to be replaced by a fitting procedure
    double adcCount = 0.0;
    int n = 0;
    for (int i = 0; i < N_SAMPLE; i++) {
        adcCount = adcCount + amp[i] / signal_amplitude(time[i]);
        if (amp[i] > 0) n++;
    }
    adcCount = adcCount / n;

    return adcCount;
}
