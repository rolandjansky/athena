/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HECHVMODULE_H_HEADER_INCLUDED_DB6688D8
#define HECHVMODULE_H_HEADER_INCLUDED_DB6688D8
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/ConstLink.h"
#include "LArHV/HECHVSubgapConstLink.h"
class HECHVManager;


//##ModelId=47A078E8027A
class HECHVModule : public RCBase
{
  public:
    //##ModelId=47A078E80280
    HECHVModule(const HECHVManager *manager, unsigned int iSide, unsigned int iPhi, unsigned int iSampling);

    // Side Index (0=Negative, 1=Positive)
    //##ModelId=47A078E80285
    unsigned int getSideIndex() const;

    // Get eta Index
    //##ModelId=47A078E80287
    unsigned int getSamplingIndex() const;

    // Get phi index
    //##ModelId=47A078E80289
    unsigned int getPhiIndex() const;

    // Get num electrodes
    //##ModelId=47A078E8028D
    unsigned int getNumSubgaps() const;

    // Gets an electrode
    //##ModelId=47A078E8028F
    HECHVSubgapConstLink getSubgap(unsigned int iElectrode) const;
    //##ModelId=47AB88CD00F2
    double getPhiMin() const;


    //##ModelId=47AB88CD0105
    double getPhiMax() const;

    //##ModelId=47ABB40E02DF
    const HECHVManager *getManager() const;

  private:
    // Destructor
    //##ModelId=47A078E80292
    virtual ~HECHVModule();

    // Illegal operation
    //##ModelId=47A078E80294
    HECHVModule(const HECHVModule& right);

    // Illegal operation
    //##ModelId=47A078E80296
    HECHVModule& operator=(const HECHVModule& right);

    class Clockwork;
    Clockwork *m_c;

};


#endif /* HECHVMODULE_H_HEADER_INCLUDED_DB6688D8 */
