/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetRecToolInterfaces_ITrtDriftCircleCutTool_H
#define InDetRecToolInterfaces_ITrtDriftCircleCutTool_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

namespace InDet
{
 
 static const InterfaceID IID_ITrtDriftCircleCutTool("ITrtSegmentSelectorTool", 1, 0);

 /** @brief The abstract interface base class for the trt segment selector tools.

     This tool provides the minimum number of TRT drit circles for a specific eta value.
     Most evident clients: All TRT pattern reconstruction tools, scoring tools

     @author Thomas Koffas, April 2009
 */

 
 class ITrtDriftCircleCutTool : virtual public IAlgTool
 {
   public:

    static const InterfaceID& interfaceID() 
    {
     return IID_ITrtDriftCircleCutTool;
    }
  
    /** @brief Parameterization from the track scoring tool. */
    virtual int minNumberDCs(const Trk::TrackParameters*) const =0;
      
 };//end of class definitions
}//end of namespace definitions


#endif
