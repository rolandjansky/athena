/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBPresamplerHVMODULE_H_HEADER_INCLUDED_C40C54F8
#define EMBPresamplerHVMODULE_H_HEADER_INCLUDED_C40C54F8
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/ConstLink.h"
class EMBPresamplerHVManager;

class EMBPresamplerHVModule : public RCBase
{
  public:

  // Constructor:
  EMBPresamplerHVModule(const EMBPresamplerHVManager *manager, unsigned int iSide, unsigned int iEta, unsigned int iPhi);

    // Get eta Index
    unsigned int getEtaIndex() const;

    // Get phi index
    unsigned int getPhiIndex() const;

    // Side Index (0=Negative, 1=Positive)
    unsigned int getSideIndex() const;

    double getEtaMin() const;


    double getEtaMax() const;


    double getPhiMin() const;

  
    double getPhiMax() const;

    // HV Status
    bool hvOn(int iGap) const;
    
    // Voltage
    double voltage(int iGap) const;

    // Current
    double current(int iGap) const;

    // Both at the same time...
    void voltage_current(int iGap, double& v, double& i) const;

    // HVLine no
    int hvLineNo(int iGap) const;

    const EMBPresamplerHVManager *getManager() const;
  

  private:

    // Destructor
    virtual ~EMBPresamplerHVModule();


    // Illegal operation
    EMBPresamplerHVModule& operator=(const EMBPresamplerHVModule& right);

    // Illegal operation
    EMBPresamplerHVModule(const EMBPresamplerHVModule& right);

    class Clockwork;
    Clockwork *m_c;

    friend class ImaginaryFriend;


};

#endif /* EMBPresamplerHVMODULE_H_HEADER_INCLUDED_C40C54F8 */
