/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboySortingTool_H
#define MboySortingTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/IMboySortingTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MboySortingTool

  @author samusog@cern.ch
  
  */

namespace Muon {
  class IMboyDetectorLimitsTool ;
}

namespace Muon {

class MboySortingTool: public AthAlgTool, virtual public IMboySortingTool{
public:
    MboySortingTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboySortingTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

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
                             ) ;

private:
///////////////////////////////////

   int m_DoDbg ; //!< Control special debug
   
   std::string m_ToolName ;

   ToolHandle< Muon::IMboyDetectorLimitsTool > p_IMboyDetectorLimitsTool ; //!< Pointer On IMboyDetectorLimitsTool

};

}

#endif

