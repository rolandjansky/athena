///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollection_p1.h 
// Header file for class JetCollection_p1
// Author: S.Binet<binet@cern.ch>
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_JETCOLLECTION_P1_H 
#define RECTPCNV_JETCOLLECTION_P1_H 

// STL includes
#include <vector>
#include <string>

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p1.h"

// forward declare
class JetCollectionCnv_p1;

class JetCollection_p1 : public std::vector<Jet_p1>
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetCollectionCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private:
  std::string m_roiAuthor;
  short m_ordered;
};

#endif //> RECTPCNV_JETCOLLECTION_P1_H
