/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MMDIGITTOOLOUTPUT_H
#define MM_DIGITIZATION_MMDIGITTOOLOUTPUT_H

/*-----------------------------------------------

Created March 2013 by Nektarios Chr. Benekos
    Karakostas Konstantinos   <Konstantinos.Karakostas@cern.ch>

Class to store output produced by MDT_Digitization tools:
- was hit efficient
- strip position
- strip charge
- strip time
- first strip position [for Trigger study]
- first strip time [for Trigger study]

-----------------------------------------------*/

class MmDigitToolOutput {
 public:
    MmDigitToolOutput(bool hitWasEff, std::vector <int> strpos, std::vector<float> time, std::vector<float> charge, int strTrig, float strTimeTrig )
     :  m_hitWasEff(hitWasEff),
        m_strpos(strpos),
        m_time(time),
        m_charge(charge),
        m_stripForTrigger(strTrig),
        m_stripTimeForTrigger(strTimeTrig),
        m_isValid(false)
    {
        if(m_strpos.size() > 0 && m_time.size() > 0 && m_charge.size() > 0) m_isValid = true;
    }

    ~MmDigitToolOutput() {}

    bool hitWasEfficient()  const { return m_hitWasEff; }
    std::vector<int> stripPos() const { return m_strpos; }
    std::vector<float> stripTime() const { return m_time; }
    std::vector<float> stripCharge() const { return m_charge; }
    int stripForTrigger() const { return m_stripForTrigger; }
    float stripTimeForTrigger() const { return m_stripTimeForTrigger; }
    void setStripForTrigger(int val)  { m_stripForTrigger = val; }
    void setStripTimeForTrigger(float val) { m_stripTimeForTrigger = val; }
    bool isValid() { return m_isValid; }

 private:
    bool  m_hitWasEff;
    std::vector<int> m_strpos;
    std::vector<float>  m_time;
    std::vector<float> m_charge;
    int m_stripForTrigger;
    float m_stripTimeForTrigger;
    bool m_isValid;
};

#endif
