/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsGeometry/DetectorFacilityCatalog.h"

namespace FADS {

  DetectorFacilityCatalog* DetectorFacilityCatalog::thePointer=0;

  DetectorFacilityCatalog::DetectorFacilityCatalog()
  {
    // std::cout<<"Detector facility catalog being built now!!"<<std::endl;
  }

  DetectorFacilityCatalog* DetectorFacilityCatalog::GetInstance()
  {
    if (!thePointer) thePointer=new DetectorFacilityCatalog;
    return thePointer;
  }

  void DetectorFacilityCatalog::RegisterFacility(DetectorFacilityBase *d)
  {
    // cout<<"registering facility "<<d->GetName()<<endl;
    if (theFacilities.find(d->GetName())!=theFacilities.end())
      std::cout<<"Redefining Facility "<<d->GetName()<<std::endl;
    else
      theFacilities[d->GetName()]=d;
  }

  DetectorFacility* DetectorFacilityCatalog::GetFacility(std::string name,
                                                         std::string newname)
  {
    if (theFacilities.find(name)==theFacilities.end())
      {
        std::cout<<"Detector facility "<<name<<"notfound!"
                 <<std::endl;
        return 0;
      }
    else
      {
        DetectorFacility* df=(DetectorFacility*)
          (theFacilities[name])->construct(newname);
        return df;
      }
  }
  void DetectorFacilityCatalog::RegisterDetector(DetectorFacility *d)
  {
    if (theCatalog.find(d->GetName())!=theCatalog.end())
      std::cout<<"Redefining detector "<<d->GetName()<<std::endl;
    else
      theCatalog[d->GetName()]=d;
  }

  DetectorFacility* DetectorFacilityCatalog::GetDetector(std::string name)
  {
    if (theCatalog.find(name)==theCatalog.end())
      {
        std::cout<<"Detector "<<name<<"notfound!"
                 <<std::endl;
        return 0;
      }
    else
      {
        DetectorFacility* df=theCatalog[name];
        return df;
      }
  }
  void DetectorFacilityCatalog::PrintAll()
  {
    std::cout<<"Printing the list of pre-defined detector facilities:"
             <<std::endl;
    std::cout<<theFacilities.size()<<std::endl;
    facilityCatalog::const_iterator it;
    for (it=theFacilities.begin();it!=theFacilities.end();it++)
      {
        std::cout<<" --- detector "<<(*it).first<<std::endl;
      }
  }

} // end namespace
