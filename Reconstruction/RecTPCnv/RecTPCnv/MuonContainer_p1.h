///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainer_p1.h 
// Header file for class MuonContainer_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006

// Author: Ketevi A. Assamagan
//         Integration of Calo Muon Identification
//         February 2007
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINER_P1_H 
#define RECTPCNV_MUONCONTAINER_P1_H 

// STL includes
#include <vector>

// RecTPCnv includes
#include "RecTPCnv/Muon_p1.h"

class MuonContainer_p1 : public std::vector<Muon_p1>
{};

#endif //> RECTPCNV_MUONCONTAINER_P1_H
