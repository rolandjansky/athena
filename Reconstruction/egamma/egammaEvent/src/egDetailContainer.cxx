/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egDetailContainer.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

********************************************************************/


// INCLUDE HEADER FILES:

#include "egammaEvent/egDetailContainer.h"
#include "egammaEvent/egDetail.h"

#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

egDetailContainer::egDetailContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<egDetail>(ownPolicy)//,
  //m_ownPolicy(ownPolicy)
{}

