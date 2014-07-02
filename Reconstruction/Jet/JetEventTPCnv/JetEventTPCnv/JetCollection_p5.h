///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollection_p5.h
// Header file for class JetCollection_p5
// Author: S.Binet<binet@cern.ch>
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCOLLECTION_P5_H
#define JETEVENTTPCNV_JETCOLLECTION_P5_H

// STL includes
#include <vector>
#include <string>

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p5.h"

// DataModelAthenaPool
#include "DataModelAthenaPool/DataLink_p1.h"

// forward declare
class JetCollectionCnv_p5;

class JetCollection_p5 : public std::vector<TPObjRef>
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetCollectionCnv_p5;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private:
  short m_ordered;
  DataLink_p1 m_keyStore;
};

#endif //> JETEVENTTPCNV_JETCOLLECTION_P5_H
