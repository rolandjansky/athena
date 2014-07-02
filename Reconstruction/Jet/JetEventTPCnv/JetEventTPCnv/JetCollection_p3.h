///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollection_p3.h 
// Header file for class JetCollection_p3
// Author: S.Binet<binet@cern.ch>
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCOLLECTION_P3_H 
#define JETEVENTTPCNV_JETCOLLECTION_P3_H 

// STL includes
#include <vector>
#include <string>

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p3.h"

// DataModelAthenaPool
#include "DataModelAthenaPool/DataLink_p1.h"

// forward declare
class JetCollectionCnv_p3;

class JetCollection_p3 : public std::vector<Jet_p3>
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetCollectionCnv_p3;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private:
  std::string m_roiAuthor;
  short m_ordered;
  DataLink_p1 m_keyStore;
};

#endif //> JETEVENTTPCNV_JETCOLLECTION_P1_H
