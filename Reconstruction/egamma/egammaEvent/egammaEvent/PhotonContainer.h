/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_PHOTONCONTAINER_H
#define EGAMMAEVENT_PHOTONCONTAINER_H
/**
  @class PhotonContainer 
   Class to define the PhotonContainer
  @author 
*/

/********************************************************************

NAME:     PhotonContainer.h
PACKAGE:  offline/Reconstruction/egammaEvent

********************************************************************/

// INCLUDE HEADER FILES:

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/egammaContainer.h"

DATAVECTOR_BASE(Analysis::Photon, egamma);

class PhotonContainer : public DataVector<Analysis::Photon> 
{
 public:
  /** @brief destructor */
  virtual ~PhotonContainer() {};

  /** @brief Main constructor */
  PhotonContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;

  void print();
};

SG_BASE(PhotonContainer, DataVector<Analysis::Photon>);
CLASS_DEF(PhotonContainer, 1094973728, 1)

#endif

