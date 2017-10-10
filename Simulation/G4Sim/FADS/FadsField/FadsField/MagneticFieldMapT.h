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
  MagneticFieldMap *m_g;
  std::map<std::string, MagneticFieldMap *, std::less<std::string> > m_fMaps;

public:
  MagneticFieldMapT(std::string n): MagneticFieldMap(n),m_g(0)
  {
    FieldManager::GetFieldManager()->RegisterMagneticField(this);
  }

  ~MagneticFieldMapT() {if (m_g) delete m_g;}

  MagneticFieldMap* Create() {
    //std::cout<<" This is "<<GetName()<<std::endl;
    if (m_g) return m_g;
    m_g=new T;
    return m_g;       
  }

  MagneticFieldMap* Create(std::string n) {
    //std::cout<<" This is "<<GetName()<<" field name "<<n<<std::endl;
    //std::cout<<" building the map ";
    if (m_fMaps.find(n) != m_fMaps.end() )
      return m_fMaps[n];
    else
    {
      //std::cout<<" Map not found in the list: building it ";
      m_fMaps[n]=new T(n);
      if (m_g==0) m_g=m_fMaps[n];
      //std::cout<<m_fMaps[n]<<std::endl;
      return m_fMaps[n];
    }     
  }

  void Initialize() {
    if (m_g) m_g->Initialize();
  }

  void Terminate() {
    m_g->Terminate();
    delete m_g;
    m_g=0;
  }   

  void FieldValue(const double *xyzPos, double *xyzField) const {
    if (m_g) m_g->FieldValue(xyzPos, xyzField);
    else
      for (int i=0;i<3;i++) xyzField[i] = 0.;
  }

};

} // end namespace

#endif 
