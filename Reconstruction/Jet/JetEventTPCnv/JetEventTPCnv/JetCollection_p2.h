///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollection_p1.h 
// Header file for class JetCollection_p1
// Author: S.Binet<binet@cern.ch>
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCOLLECTION_P1_H 
#define JETEVENTTPCNV_JETCOLLECTION_P1_H 

// STL includes
#include <vector>
#include <string>

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p2.h"

// DataModelAthenaPool
#include "DataModelAthenaPool/DataLink_p1.h"

// forward declare
class JetCollectionCnv_p2;

class JetCollection_p2 : public std::vector<Jet_p2>
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetCollectionCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private:
  std::string m_roiAuthor;
  short m_ordered;
  DataLink_p1 m_keyStore;
};

#endif //> JETEVENTTPCNV_JETCOLLECTION_P1_H
