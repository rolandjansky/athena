/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMECHVMODULE_H_HEADER_INCLUDED_8AB76742
#define EMECHVMODULE_H_HEADER_INCLUDED_8AB76742
#include "GeoModelKernel/RCBase.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
class EMECHVManager;
// Describes one HV Module within the EMEC
//##ModelId=478D18650310
class EMECHVModule : public RCBase
{
  public:
    //##ModelId=47A288FA019F
    enum IOType { INNER = 0,  OUTER = 1};

    //##ModelId=478D18650316
    EMECHVModule(const EMECHVManager *manager, unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector);

    //##ModelId=47A7A9AD000E
    double getEtaMin() const;


    //##ModelId=47A7A9BF0293
    double getEtaMax() const;

  
    //##ModelId=47A7A9C10344
    double getPhiMin() const;

  
    //##ModelId=47A7A9C400D5
    double getPhiMax() const;

    //##ModelId=478D18650319
    unsigned int getEtaIndex() const;

    //##ModelId=478D1865031B
    unsigned int getPhiIndex() const;

    //##ModelId=478D1865031D
    unsigned int getNumElectrodes() const;

    //##ModelId=478D1865031F
    EMECHVElectrodeConstLink getElectrode(unsigned int iElectrode) const;

    // Side Index (0=Negative, 1=Positive)
    //##ModelId=47A07B14017E
    unsigned int getSideIndex() const;


    // Gets the Wheel, 0 for the Outer Wheel HV Manager and 1 for the inner
    // Wheel HV Manager.
    //##ModelId=47A28F660267
    EMECHVModule::IOType getWheelIndex() const;


    // Get sector index
    //##ModelId=47A07AFD0301
    unsigned int getSectorIndex() const;

    //##ModelId=47AB6F880176
    const EMECHVManager *getManager() const;


  private:
    //##ModelId=478D18650322
    virtual ~EMECHVModule();

    //##ModelId=478D18650324
    EMECHVModule(const EMECHVModule& right);

    //##ModelId=478D18650326
    EMECHVModule& operator=(const EMECHVModule& right);

    class Clockwork;
    Clockwork *m_c;

};



#endif /* EMECHVMODULE_H_HEADER_INCLUDED_8AB76742 */
