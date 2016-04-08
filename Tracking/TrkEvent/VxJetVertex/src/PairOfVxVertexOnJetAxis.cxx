/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           PairOfVxVertexOnJetAxis.h  -  Description
                             -------------------
    begin   : Februar 2007
    authors : Giacinto Piacquadio (University of Freiburg) 
              Christian Weiser (University of Freiburg)
    e-mails:  giacinto.piacquadio@physik.uni-freiburg.de)
              christian.weiser@cern.ch
    changes: new!

   (c) Atlas Collaboration 2007

   Class representing a pair of VxVertexOnJetAxis.

   The ordering is always so that the first of the pair has a smaller 
   

   explicit class seems to be needed because of default constructor for std::map.
********************************************************************/

#include "VxJetVertex/PairOfVxVertexOnJetAxis.h"
//#include "VxJetVertex/VxVertexOnJetAxis.h"


namespace Trk {

  PairOfVxVertexOnJetAxis::PairOfVxVertexOnJetAxis() :
    std::pair<VxVertexOnJetAxis*,VxVertexOnJetAxis*>() 
  {
    this->first=0;
    this->second=0;
  }


  PairOfVxVertexOnJetAxis::PairOfVxVertexOnJetAxis(VxVertexOnJetAxis* first,VxVertexOnJetAxis* second):
    std::pair<VxVertexOnJetAxis*,VxVertexOnJetAxis*>()
  {
    this->first=first;
    this->second=second;
  }


}
