/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_JETMOMENTMAP_P1_H
#define JETEVENTTPCNV_JETMOMENTMAP_P1_H

#include "CLIDSvc/CLASS_DEF.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

#include <vector>

class JetMomentMap_p1
{
  template <class T>
    friend class JetMomentMapConverterBase;
  
public:

  JetMomentMap_p1() { };
  virtual ~JetMomentMap_p1() { };
  
 protected:
  
  //  std::vector<ElementLink<JetCollection> > m_jets;
  ElementLinkVector_p1<uint32_t>           m_jetsV;
  std::vector<std::vector<unsigned int> >  m_mtypV;
  std::vector<std::vector<double> >        m_mvalV;
  
};

#endif
