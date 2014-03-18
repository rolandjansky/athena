/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorFacilityCatalog_H
#define DetectorFacilityCatalog_H

#include "FadsGeometry/DetectorFacilityBase.h"
#include "FadsGeometry/DetectorFacility.h"
#include <map>
#include <string>

namespace FADS {

  typedef std::map<std::string,DetectorFacilityBase *,std::less<std::string> > facilityCatalog;
  typedef std::map<std::string,DetectorFacility *,std::less<std::string> >  detectorCatalog;

  class DetectorFacilityCatalog {
  private:
    facilityCatalog theFacilities;
    detectorCatalog theCatalog;
    static DetectorFacilityCatalog* thePointer;
    inline DetectorFacilityCatalog();
  public:
    static DetectorFacilityCatalog* GetInstance();
    void RegisterFacility(DetectorFacilityBase *d);
    DetectorFacility *GetFacility(std::string name,std::string newname="*");
    void RegisterDetector(DetectorFacility *d);
    DetectorFacility *GetDetector(std::string name);
    void PrintAll();
  };

} // end namespace

#endif // DetectorFacilityCatalog
