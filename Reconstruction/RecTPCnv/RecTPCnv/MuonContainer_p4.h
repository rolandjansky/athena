///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainer_p4.h 
// Header file for class MuonContainer_p4
// Author: Ketevi A. Assamagan <keetvi@bnl.gov>
// Date:   September 2007 

/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINER_P4_H 
#define RECTPCNV_MUONCONTAINER_P4_H 

// STL includes
#include <vector>

// RecTPCnv includes
#include "RecTPCnv/Muon_p4.h"

class MuonContainer_p4 : public std::vector<Muon_p4>
{};

#endif //> RECTPCNV_MUONCONTAINER_P4_H
