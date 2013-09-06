///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainer_p2.h 
// Header file for class MuonContainer_p2
// Author: Ketevi A. Assamagan <keetvi@bnl.gov>
// Date:   September 2007 

/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINER_P2_H 
#define RECTPCNV_MUONCONTAINER_P2_H 

// STL includes
#include <vector>

// RecTPCnv includes
#include "RecTPCnv/Muon_p2.h"

class MuonContainer_p2 : public std::vector<Muon_p2>
{};

#endif //> RECTPCNV_MUONCONTAINER_P2_H
