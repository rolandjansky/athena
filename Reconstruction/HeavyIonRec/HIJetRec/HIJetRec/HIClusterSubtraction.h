/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIClusterSubtraction.h

#ifndef __HIJETREC_HICLUSTERSUBTRACTION_H__
#define __HIJETREC_HICLUSTERSUBTRACTION_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIClusterSubtraction
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Algorithm that applies background subtraction to clusters.
/// The subtraction needs to be handled separately from the making.
/// The unsubtracted clusters are inputs into the jet finding which
/// in turn defines the background (HIEventShapeContainer).
///
////////////////////////////////////////////////////////////////////////////////


#include <AthenaBaseComps/AthAlgorithm.h>
#include <xAODCaloEvent/CaloClusterContainer.h>
#include "AsgTools/ToolHandle.h"
#include <HIJetRec/IHISubtractorTool.h>

class HIClusterSubtraction : public AthAlgorithm
{

public:
  HIClusterSubtraction(const std::string& name, ISvcLocator* pSvcLocator);
  ~HIClusterSubtraction() {};
  virtual StatusCode initialize(){return StatusCode::SUCCESS;};
  virtual StatusCode execute();
  virtual StatusCode finalize(){return StatusCode::SUCCESS;};

private:
  /// \brief Name of input cluster container
  std::string m_cluster_key; 

  /// \brief Name of HIEventShapeContainer defining background
  std::string m_event_shape_key; 

  /// \brief handle to IHISubtractorTool which does calculates subtracted kinematics
  ToolHandle<IHISubtractorTool> m_subtractor_tool; 

};
#endif
