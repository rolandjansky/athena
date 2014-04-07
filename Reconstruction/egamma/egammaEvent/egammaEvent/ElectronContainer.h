// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_ELECTRONCONTAINER_H
#define EGAMMAEVENT_ELECTRONCONTAINER_H 1
/**
  @class ElectronContainer 
   Class to define the ElectronContainer
  @author 
*/

/********************************************************************

NAME:     ElectronContainer.h
PACKAGE:  offline/Reconstruction/egammaEvent

********************************************************************/

// INCLUDE HEADER FILES:

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/egammaContainer.h"

DATAVECTOR_BASE(Analysis::Electron, egamma);

class ElectronContainer : public DataVector<Analysis::Electron> 
{
 public:
  /** @brief destructor */
  virtual ~ElectronContainer() {};

  /** @brief Main constructor */
  ElectronContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;

  /** @brief wether it owns or not the el SG::OWN_ELEMENTS or VIEW_ELEMENTS */
  SG::OwnershipPolicy  m_ownPolicy ;

  /** reimplementation of push_back (temporary)*/
  void push_back(const Analysis::Electron*);

  void push_back(Analysis::Electron*);

  void print();
};

SG_BASE(ElectronContainer, DataVector<Analysis::Electron>);
CLASS_DEF(ElectronContainer, 1341992142, 1)

#endif

