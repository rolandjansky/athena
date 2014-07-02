///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollection_p6.h
// Header file for class JetCollection_p6
// Author: S.Binet<binet@cern.ch>
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCOLLECTION_P6_H
#define JETEVENTTPCNV_JETCOLLECTION_P6_H

// STL includes
#include <vector>
#include <string>

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p6.h"

// DataModelAthenaPool
#include "DataModelAthenaPool/DataLink_p2.h"

// forward declare
class JetCollectionCnv_p6;

class JetCollection_p6 : public std::vector<TPObjRef>
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 
public:
  JetCollection_p6():m_jetIdBugFixed(false){}
  // Make the AthenaPoolCnv class our friend
  friend class JetCollectionCnv_p6;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private:
  short m_ordered;
  DataLink_p2 m_keyStore;
  //DataLink_p2 m_momentMapLink;
  unsigned short m_jetAuthor;
  // Only to fix reading of very early release 16 validation sample
  bool m_jetIdBugFixed;
};

#endif //> JETEVENTTPCNV_JETCOLLECTION_P6_H
