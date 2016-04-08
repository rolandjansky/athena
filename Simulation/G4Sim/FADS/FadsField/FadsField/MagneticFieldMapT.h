/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MagneticFieldMapT_H
#define MagneticFieldMapT_H

#include "FadsField/MagneticFieldMap.h"
#include "FadsField/FieldManager.h"

#include <map>
#include <string>

#include <iostream>

namespace FADS {

template <class T> class MagneticFieldMapT: public MagneticFieldMap {

private:
  MagneticFieldMap *g;
  std::map<std::string, MagneticFieldMap *, std::less<std::string> > fMaps;

public:
  MagneticFieldMapT(std::string n): MagneticFieldMap(n),g(0)
  {
    FieldManager::GetFieldManager()->RegisterMagneticField(this);
  }

  ~MagneticFieldMapT() {if (g) delete g;}

  MagneticFieldMap* Create() {
    //std::cout<<" This is "<<GetName()<<std::endl;
    if (g) return g;
    g=new T;
    return g;       
  }

  MagneticFieldMap* Create(std::string n) {
    //std::cout<<" This is "<<GetName()<<" field name "<<n<<std::endl;
    //std::cout<<" building the map ";
    if (fMaps.find(n) != fMaps.end() )
      return fMaps[n];
    else
    {
      //std::cout<<" Map not found in the list: building it ";
      fMaps[n]=new T(n);
      if (g==0) g=fMaps[n];
      //std::cout<<fMaps[n]<<std::endl;
      return fMaps[n];
    }     
  }

  void Initialize() {
    if (g) g->Initialize();
  }

  void Terminate() {
    g->Terminate();
    delete g;
    g=0;
  }   

  void FieldValue(const double *xyzPos, double *xyzField) const {
    if (g) g->FieldValue(xyzPos, xyzField);
    else
      for (int i=0;i<3;i++) xyzField[i] = 0.;
  }

};

} // end namespace

#endif 
