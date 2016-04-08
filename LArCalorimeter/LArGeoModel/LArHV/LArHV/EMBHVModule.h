/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBHVMODULE_H_HEADER_INCLUDED_C40C54F8
#define EMBHVMODULE_H_HEADER_INCLUDED_C40C54F8
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/ConstLink.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
class EMBHVManager;

//##ModelId=4753092700BA
class EMBHVModule : public RCBase
{
  public:
    //##ModelId=475656A60228
    EMBHVModule(const EMBHVManager *manager, unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector);

    // Get eta Index
    //##ModelId=475659560392
    unsigned int getEtaIndex() const;

    // Get phi index
    //##ModelId=4756597E0324
    unsigned int getPhiIndex() const;

    // Get sector index
    //##ModelId=479B9A7301DC
    unsigned int getSectorIndex() const;

    // Get num electrodes
    //##ModelId=4756967601F7
    unsigned int getNumElectrodes() const
    {
      return 64;
    }

    // Gets an electrode
    //##ModelId=475696D40106
    EMBHVElectrodeConstLink getElectrode(unsigned int iElectrode) const;
    // Side Index (0=Negative, 1=Positive)
    //##ModelId=479BBABD034D
    unsigned int getSideIndex() const;
    //##ModelId=47AA68EE020D
    double getEtaMin() const;


    //##ModelId=47AA68EE0223
    double getEtaMax() const;


    //##ModelId=47AA68EE0236
    double getPhiMin() const;

  
    //##ModelId=47AA68EE024B
    double getPhiMax() const;

    //##ModelId=47AB6F3600D2
    const EMBHVManager *getManager() const;
  

  private:
    // Destructor
    //##ModelId=475656A60278
    virtual ~EMBHVModule();


    // Illegal operation
    EMBHVModule& operator=(const EMBHVModule& right);
    // Illegal operation
    EMBHVModule(const EMBHVModule& right);

    class Clockwork;
    Clockwork *m_c;

    friend class ImaginaryFriend;


};

#endif /* EMBHVMODULE_H_HEADER_INCLUDED_C40C54F8 */
