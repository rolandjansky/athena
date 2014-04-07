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

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
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

  /** @brief wether it owns or not the cel SG::OWN_ELEMENTS or VIEW_ELEMENTS */
  SG::OwnershipPolicy  m_ownPolicy ;

  /** reimplementation of push_back (temporary)*/
  void push_back(const Analysis::Photon*);

  void push_back(Analysis::Photon*);

  void print();
};

SG_BASE(PhotonContainer, DataVector<Analysis::Photon>);
CLASS_DEF(PhotonContainer, 1094973728, 1)

#endif

