/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTrackSelectorTool_InDetTrtDriftCircleCutTool_H
#define InDetTrackSelectorTool_InDetTrtDriftCircleCutTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ITrtDriftCircleCutTool.h"
#include "TRT_ConditionsData/ActiveFraction.h"
#include "StoreGate/ReadCondHandleKey.h"

/**
 * A tool to be used for trt segment pre-selection
 *
 * Thomas Koffas <Thomas.Koffas@cern.ch>
 * April 2009
 */


namespace InDet{

  class InDetTrtDriftCircleCutTool : virtual public ITrtDriftCircleCutTool, public AthAlgTool  
    {
      
    public:
  
      StatusCode initialize();
      
      StatusCode finalize();
      
      InDetTrtDriftCircleCutTool(const std::string& t, const std::string& n, const IInterface*  p); 
      
      ~InDetTrtDriftCircleCutTool();
      
      /** @brief Minimum number of drift circles using the track scoring tool */
      int minNumberDCs(const Trk::TrackParameters*) const;
      
    private:
      
      SG::ReadCondHandleKey<TRTCond::ActiveFraction> m_strawReadKey{this,"ActiveWriteKey","ActiveFraction","ActiveFraction in-key"};
      /** Properties for track selection:all cuts are ANDed */
      int  m_minOffset;  //!< Minimum number of TRT drit circles required
      bool m_param;      //!< Use the new or the old parameterization
      bool m_useTRT;     //!< Use the TRT active fraction services to correct for dead straws

    }; //end of class definitions
} //end of namespace

#endif
