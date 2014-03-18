/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorFacilityBaseT_H
#define DetectorFacilityBaseT_H

#include "FadsGeometry/DetectorFacilityBase.h"
#include "FadsGeometry/DetectorFacilityCatalog.h"

#ifdef HAVE_NEW_IOSTREAMS
#include <iostream>
#else
#include <iostream.h>
#endif

#include <map>

namespace FADS {

  template <class T> class DetectorFacilityBaseT: public DetectorFacilityBase {
  private:
    std::map<std::string, DetectorFacilityBase*, std::less<std::string> > theDets;
    T* theDetector;
  public:
    DetectorFacilityBaseT(std::string n): DetectorFacilityBase(n)
    {
      // std::cout<<"c-tor called for "<<name<<std::endl;
      DetectorFacilityCatalog *dfs=DetectorFacilityCatalog::GetInstance();
      // std::cout<<"Registering detector facility "<<name<<" "<<std::endl;
      dfs->RegisterFacility(this);
      theDetector=0;
    }
    T* construct(std::string n="*")
    {
      std::string tname=name;
      if (n!="*") tname=n;
      // if (!theDetector) theDetector=new T(tname);
      if (theDets.find(n)==theDets.end())
        {
          theDetector=new T(tname);
          theDets[n]=theDetector;
        }
      else
        {
          std::cout<<" Warning! Detector "<<n<<" already exist!"
                   <<" returning the existing copy! "<<std::endl;
          theDetector=(T*)theDets[n];
        }

      return theDetector;
    }
    void SetAsWorld()
    {
      if (!theDetector) theDetector=new T(name);
      theDetector->SetAsWorld();
    }
    Envelope& Build()
    {
      if (!theDetector) theDetector=new T(name);
      return theDetector->Build();
    }
  };

} // end namespace

#endif // DetectorFacilityBaseT
