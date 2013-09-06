///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainer_p5.h 
// Header file for class MuonContainer_p5
// Author: Ketevi A. Assamagan <keetvi@bnl.gov>
// Date:   September 2007 

/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINER_P5_H 
#define RECTPCNV_MUONCONTAINER_P5_H 1

// STL includes
#include <vector>

// RecTPCnv includes
#include "RecTPCnv/Muon_p5.h"

class MuonContainer_p5 : public std::vector<Muon_p5>
{};

#endif //> RECTPCNV_MUONCONTAINER_P5_H
