/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGET_P2_H
#define RECTPCNV_MISSINGET_P2_H

/********************************************************************

NAME:     MissingET_p2
PACKAGE:  offline/Reconstruction/RecTPCnv

PURPOSE:  Persistent rapresentation of MissingET class
********************************************************************/
#include <vector>

// forward declarations
class MissingETCnv_p2;

class MissingET_p2{
 friend class MissingETCnv_p2;
 
 public:
	MissingET_p2() {};
       ~MissingET_p2() = default;
 protected:
  	std::vector<float> m_AllTheData;
 
};


#endif
