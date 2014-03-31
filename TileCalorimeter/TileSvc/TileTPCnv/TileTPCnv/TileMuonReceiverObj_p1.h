///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuonReceiverObj_p1.h 
// Header file for class TileMuonReceiverObj_p1
// Author: Joao Gentil Saraiva <joao.gentil.saraiva@cern.ch>
// Date:   March 2014
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEMUONRECEIVEROBJ_P1_H
#define TILETPCNV_TILEMUONRECEIVEROBJ_P1_H

#include <vector>

// forward declarations
class TileMuonReceiverObjCnv_p1;

class TileMuonReceiverObj_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class TileMuonReceiverObjCnv_p1;

public:

  // Default constructor: 
  TileMuonReceiverObj_p1(): m_id(0), m_data() {}

private:

  unsigned int m_id;
  std::vector<float> m_data;

};

#endif
