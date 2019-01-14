/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETRECOTOOLS_TRIGL2DUPTRACKREMOVALTOOL_H
#define TRIGINDETRECOTOOLS_TRIGL2DUPTRACKREMOVALTOOL_H

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
  virtual void removeDupTracks(TrigInDetTrackCollection* trackColl, std::vector<unsigned int> placeHolder) override;

  private:
  int m_mode; // mode 1: use all tracks & DR criterion
              // mode 2: use tracks from different z vertices & SP criterion -- default

};



#endif


 
