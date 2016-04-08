/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMECPresamplerHVMODULE_H_HEADER_INCLUDED_8AB76742
#define EMECPresamplerHVMODULE_H_HEADER_INCLUDED_8AB76742
#include "GeoModelKernel/RCBase.h"
class EMECPresamplerHVManager;
// Describes one HV Module within the EMECPresampler
class EMECPresamplerHVModule : public RCBase
{
  public:


    EMECPresamplerHVModule(const EMECPresamplerHVManager *manager, unsigned int iSide, unsigned int iPhi);

    double getEtaMin() const;

    double getEtaMax() const;

    double getPhiMin() const;
  
    double getPhiMax() const;

    unsigned int getPhiIndex() const;

    // Side Index (0=Negative, 1=Positive)
    unsigned int getSideIndex() const;


    // HV Status
    bool hvOn(int iGap) const;
    
    // Voltage
    double voltage(int iGap) const;

    // Current
    double current(int iGap) const;

    // Current and voltage at the same time:
    void voltage_current(int iGap, double& v, double& i) const;

    // HVLine no
    int hvLineNo(int iGap) const;

    const EMECPresamplerHVManager *getManager() const;


  private:

    virtual ~EMECPresamplerHVModule();

    EMECPresamplerHVModule(const EMECPresamplerHVModule& right);

    EMECPresamplerHVModule& operator=(const EMECPresamplerHVModule& right);

    class Clockwork;
    Clockwork *m_c;

};



#endif /* EMECPresamplerHVMODULE_H_HEADER_INCLUDED_8AB76742 */
