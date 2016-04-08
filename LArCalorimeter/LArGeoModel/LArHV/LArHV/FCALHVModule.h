/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FCALHVMODULE_H_HEADER_INCLUDED_B6E380D9
#define FCALHVMODULE_H_HEADER_INCLUDED_B6E380D9
class HECHVModule;
class HECHVManager;
class FCALHVManager;
#include "GeoModelKernel/ConstLink.h"
#include "GeoModelKernel/RCBase.h"
#include "LArHV/FCALHVLineConstLink.h"

//##ModelId=47ABB2AD00A4
class FCALHVModule : public RCBase
{
  public:
    //##ModelId=47ABB2AD00A9
    FCALHVModule(const FCALHVManager *manager, unsigned int iSide, unsigned int iSector, unsigned int iSampling);

    // Side Index (0=Negative, 1=Positive)
    //##ModelId=47ABB2AD00B2
    unsigned int getSideIndex() const;

    // Get eta Index
    //##ModelId=47ABB2AD00B4
    unsigned int getSamplingIndex() const;

    // Get sector index
    //##ModelId=47ABB2AD00B6
    unsigned int getSectorIndex() const;

    // Get num electrodes
    //##ModelId=47ABB2AD00B8
    unsigned int getNumHVLines() const;

    // Gets an electrode
    //##ModelId=47ABB2AD00BA
    FCALHVLineConstLink getHVLine(unsigned int iLine) const;

    //##ModelId=47ABB3E7009B
    const FCALHVManager *getManager() const;

  private:
    // Destructor
    //##ModelId=47ABB2AD00BD
    virtual ~FCALHVModule();

    // Illegal operation
    //##ModelId=47ABB2AD00BF
    FCALHVModule(const FCALHVModule& right);

    // Illegal operation
    //##ModelId=47ABB2AD00C1
    FCALHVModule& operator=(const FCALHVModule& right);

    class Clockwork;
    Clockwork *m_c;

};



#endif /* FCALHVMODULE_H_HEADER_INCLUDED_B6E380D9 */
