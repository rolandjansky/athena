///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonContainer_p1.h 
// Header file for class PhotonContainer_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCONTAINER_P1_H 
#define EGAMMAEVENTTPCNV_PHOTONCONTAINER_P1_H 

// STL includes

// egammaEventTPCnv includes
#include "egammaEventTPCnv/Photon_p1.h"

class PhotonContainer_p1 : public std::vector<Photon_p1>
{};

#endif //> EGAMMAEVENTTPCNV_PHOTONCONTAINER_P1_H
