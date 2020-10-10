/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecEvent/LArCellContainer.h"
#include "LArRecEvent/LArCell.h"

#include <iostream>
void LArCellContainer::print()
{ 

  LArCellContainer::iterator c_itr; 
  float etotal = 0; 

  for (c_itr=begin(); c_itr!=end(); ++c_itr) 
  {
    (*c_itr)->print();
    etotal+=(*c_itr)->energy(); 
  }

  // FIXME:  SHould not use a cout, but cannot use Athena logging services

  std::cout << "Number of cells found " << this->size() 
	    << " - Total energy =" << etotal
	    << std::endl;

}







