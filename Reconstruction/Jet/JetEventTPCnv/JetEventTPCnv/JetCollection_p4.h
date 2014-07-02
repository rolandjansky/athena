///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollection_p4.h 
// Header file for class JetCollection_p4
// Author: S.Binet<binet@cern.ch>
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCOLLECTION_P4_H 
#define JETEVENTTPCNV_JETCOLLECTION_P4_H 

// STL includes
#include <vector>
#include <string>

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p4.h"

// DataModelAthenaPool
#include "DataModelAthenaPool/DataLink_p1.h"

// forward declare
class JetCollectionCnv_p4;

class JetCollection_p4 : public std::vector<TPObjRef>
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetCollectionCnv_p4;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private:
  std::string m_roiAuthor;
  short m_ordered;
  DataLink_p1 m_keyStore;
};

#endif //> JETEVENTTPCNV_JETCOLLECTION_P1_H
