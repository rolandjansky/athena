///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainer_p5.h 
// Header file for class egammaContainer_p5
// New cnv to include Ringer: W.S.Freund<wsfreund@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACONTAINER_P5_H 
#define EGAMMAEVENTTPCNV_EGAMMACONTAINER_P5_H 1

// STL includes
#include <vector>

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egamma_p5.h"

class egammaContainer_p5 : public std::vector<egamma_p5>
{};

#endif //> EGAMMAEVENTTPCNV_EGAMMACONTAINER_P5_H
