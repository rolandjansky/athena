/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CalorTower.cxx
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  S. Rajagopalan
CREATED:  Oct 2000

PURPOSE:  Tower data class for Calorimeter.

Updated:  Feb 5, 2001    (HM)
	  Made it to be a concrete class. 

          Jan 16, 2003   (P.Loch)
          Introduce new navigation system, needed map of cell weights

********************************************************************/

#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloCell.h"



////////////////
// Destructor //
////////////////

CaloTower::~CaloTower()
{ }

///////////////////////
// Kinematics Update //
///////////////////////

void CaloTower::updateKine(const CaloCell* theCell, double weight)
{
  // updates E and Et (no change of tower direction!)
  this->setE( this->e() + weight * theCell->e() );
}

/////////////////////////
// Basic Energy Signal //
/////////////////////////


double
CaloTower::getBasicEnergy() const { return this->e(); }

void
CaloTower::setBasicEnergy(double theEnergy) { this->setE(theEnergy); }
