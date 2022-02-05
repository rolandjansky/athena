/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRecEvent/LArCell.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "FourMomUtils/P4Helpers.h"

#include <iostream>


LArCell::~LArCell () 
{  }


double LArCell::deltaR(const LArCell* other) const
{
  
  return P4Helpers::deltaR(this,other);
  
}


void LArCell::print() const
{
  std::cout << "ID         = " << this->ID().get_compact()<< std::endl;
  std::cout << "Energy     = " << this->e()<< std::endl;
  std::cout << "Eta        = " << this->eta() << std::endl;
  std::cout << "Phi        = " << this->phi() << std::endl;
}

std::unique_ptr<CaloCell> LArCell::clone() const
{
  return std::make_unique<LArCell>(this->caloDDE(),
				   this->ID(),
				   this->energy(),
				   this->time(),
				   this->quality(),
				   this->provenance(),
				   this->gain()); 
}


bool LArCell::badcell() const
{
  if (this->provenance() & 0x0800) return true;
  return false;
}
