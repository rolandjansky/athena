/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FCALHVMANAGER_H_HEADER_INCLUDED_AC244FA8
#define FCALHVMANAGER_H_HEADER_INCLUDED_AC244FA8

#include "GeoModelKernel/RCBase.h"
#include "FCALHVModule.h"
typedef ConstLink<FCALHVModule> FCALHVModuleConstLink;
class FCALHVDescriptor;

struct FCALHVPayload;

//##ModelId=475307C201C5
class FCALHVManager : public RCBase
{
  public:
    //##ModelId=47ABAE9302D3
    FCALHVManager();

    //##ModelId=47ABAE930373
    virtual ~FCALHVManager();

    //##ModelId=47ABAF5E0079
    const FCALHVDescriptor *getDescriptor() const;

    // Begin side index (0=negative and 1= positive)
    //##ModelId=47ABAF5E0092
    unsigned int beginSideIndex() const;

    // End side index (0=negative and 1= positive)
    //##ModelId=47ABAF5E009F
    unsigned int endSideIndex() const;

    //##ModelId=47ABAF5E00AD
    unsigned int beginSectorIndex(unsigned int iSampling) const;

    //##ModelId=47ABAF5E00BB
    unsigned int endSectorIndex(unsigned int iSampling) const;

    //##ModelId=47ABAF5E00C9
    unsigned int beginSamplingIndex() const;

    //##ModelId=47ABAF5E00D9
    unsigned int endSamplingIndex() const;

    //##ModelId=47ABAF5E00E8
    FCALHVModuleConstLink getHVModule(unsigned int iSide, unsigned int iSector, unsigned int iSampling) const;

    // Refresh from the database if needed
    void update() const;

    // Make the data stale.  Force update of data.
    void reset() const;

    // Get the database payload
    FCALHVPayload *getPayload(const FCALHVLine &) const;


  private:
    //##ModelId=47ABAE93032E
    FCALHVManager(const FCALHVManager& right);

    //##ModelId=47ABAE930392
    FCALHVManager& operator=(const FCALHVManager& right);

    class Clockwork;
    Clockwork *m_c;
};



#endif /* FCALHVMANAGER_H_HEADER_INCLUDED_AC244FA8 */
