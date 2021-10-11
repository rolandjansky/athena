/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_MDTDIGITOOLOUTPUT_H
#define MDT_DIGITIZATION_MDTDIGITOOLOUTPUT_H

/*-----------------------------------------------

   Created 7-5-2004 by Niels van Eldik

Class to store output produced by MDT_Digitization tools:
 - was tube efficient
 - drift time
 - adc counts

-----------------------------------------------*/

class MdtDigiToolOutput {
public:
    MdtDigiToolOutput(bool wasEff, double time, double adc) : m_wasEff(wasEff), m_time(time), m_adc(adc) {}


    double driftTime() const { return m_time; }
    double adc() const { return m_adc; }
    double wasEfficient() const { return m_wasEff; }

private:
    bool m_wasEff;
    double m_time;
    double m_adc;
};

#endif
