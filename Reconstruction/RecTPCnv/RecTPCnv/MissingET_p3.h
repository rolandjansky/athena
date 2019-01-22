/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGET_P3_H
#define RECTPCNV_MISSINGET_P3_H

/********************************************************************

NAME:     MissingET_p3
PACKAGE:  offline/Reconstruction/RecTPCnv
PURPOSE:  Persistent rapresentation of MissingET class
********************************************************************/

// forward declarations
class MissingETCnv_p3;

class MissingET_p3{
 friend class MissingETCnv_p3;
 
 public:
	MissingET_p3() {};
       ~MissingET_p3() = default;
 protected:
  	std::vector<float> m_AllTheData;
 
};


#endif
