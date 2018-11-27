/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_DUP_TRACK_REMOVAL_TOOL_H__
#define __TRIG_L2_DUP_TRACK_REMOVAL_TOOL_H__

#include<vector>

#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetRecoTools/ITrigL2DupTrackRemovalTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class TrigInDetTrackCollection;


  
class TrigL2DupTrackRemovalTool: public AthAlgTool, virtual public ITrigL2DupTrackRemovalTool { 
  
 public: 
  TrigL2DupTrackRemovalTool(const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2DupTrackRemovalTool(); 
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  void removeDupTracks(TrigInDetTrackCollection* trackColl, std::vector<unsigned int> placeHolder);

  private:
  int m_mode; // mode 1: use all tracks & DR criterion
              // mode 2: use tracks from different z vertices & SP criterion -- default

};



#endif


 
