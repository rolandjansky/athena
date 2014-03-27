/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboySortingTool_H
#define IMboySortingTool_H

#include "GaudiKernel/IAlgTool.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"


namespace Muon {

static const InterfaceID IID_IMboySortingTool("IMboySortingTool", 1, 0);

  /**
   @class IMboySortingTool

  @author samusog@cern.ch
  
  */

class IMboySortingTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Sort Positions */
   virtual void SortPositions(
                              int DoDbg ,
                              std::vector<int>&  LinearIndexSorted ,
                              std::vector<int>&  AreaTagSorted ,
                              double XrefStart,
                              double YrefStart,
                              double ZrefStart,
                              std::vector<double> PointXdca ,
                              std::vector<double> PointYdca ,
                              std::vector<double> PointZdca ,
                              std::vector<int>    PointInde  
                             ) = 0 ;

};

inline const InterfaceID& Muon::IMboySortingTool::interfaceID() { return IID_IMboySortingTool;}

}

#endif

