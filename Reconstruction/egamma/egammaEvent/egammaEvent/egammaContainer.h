/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EGAMMACONTAINER_H
#define EGAMMAEVENT_EGAMMACONTAINER_H
/**
  @class egammaContainer
          This is a data object, containing a collection of egamma Objects
   @author H. Ma
*/

/********************************************************************
NAME:     egammaContainer.h
PACKAGE:  offline/Reconstruction/egammaRec
CREATED:  Nov 2000
PURPOSE:  This is a data object, containing a collection of egamma Objects.

Updated:  Jan 13, 2001   (MW)
          add some more comments

          July 13, 2005 (S. Laplace)
          added constructor with SG::OwnerShipPolicy

Updated: moved to egammaEvent (SR)
          Jun 14, 2007 (FD) Doxygen documentation
********************************************************************/

// INCLUDE HEADER FILES:

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "egammaEvent/egamma.h"

#include "NavFourMom/IParticleContainer.h"
// Needed for constructing the mirror container hierarchy
// This tells StoreGate that DV<egamma> derives from DV<IParticle>
DATAVECTOR_BASE( egamma, IParticle );

class egammaContainer : public DataVector<egamma> 
{
 public:
  virtual ~egammaContainer() {};

  /** Main constructor */
  egammaContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;

  void print();

  /** wether it owns or not the cel SG::OWN_ELEMENTS or VIEW_ELEMENTS */
  SG::OwnershipPolicy  m_ownPolicy ;

  /** reimplementation of push_back (temporary)*/
  void push_back(const egamma *);

  void push_back(egamma *);

};

SG_BASE(egammaContainer, DataVector<egamma>);
CLASS_DEF(egammaContainer, 3127, 1)

#endif

