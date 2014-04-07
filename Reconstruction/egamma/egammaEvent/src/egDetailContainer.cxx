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

#ifdef HAVE_NEW_IOSTREAMS
  #include <iomanip>
  #include <iostream>
#else
  #include <iomanip.h>
  #include <iostream.h>
#endif

//  END OF HEADER FILES INCLUDE

egDetailContainer::egDetailContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<egDetail>(ownPolicy),
  m_ownPolicy(ownPolicy)
{}

