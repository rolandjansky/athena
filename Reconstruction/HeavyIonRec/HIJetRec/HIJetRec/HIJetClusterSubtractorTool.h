/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetClusterSubtractorTool.h

#ifndef __HIJETREC_HIJETCLUSTERSUBTRACTORTOOL_H__
#define __HIJETREC_HIJETCLUSTERSUBTRACTORTOOL_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetClusterSubtractorTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Concrete implementation of HIJetSubtractorToolBase.
/// Class used by HIClusterSubtraction and HIJetConstituentSubtractionTool
/// to provide kinematics of a cluster after subtraction given an event shape.
/// This version of the tool does not use the cell information and thus
/// should be faster than HIJetCellSubtractorTool.
///
////////////////////////////////////////////////////////////////////////////////

#include "HIJetRec/HIJetSubtractorToolBase.h"
#include "AsgTools/AsgTool.h"

class HIJetClusterSubtractorTool : public HIJetSubtractorToolBase
{
  ASG_TOOL_CLASS(HIJetClusterSubtractorTool, IHISubtractorTool)

  public:
  /// \brief Implements method defined in base
  /// First argument is reference to four vector that is updated to reflect
  /// the subtracted kinematics of the IParticle passed in the second arg
  /// Method expects cl_in to be a cluster
  virtual StatusCode Subtract(xAOD::IParticle::FourMom_t& subtr_mom, xAOD::CaloCluster* cl_in, HIEventShapeContainer* shape) = 0;

  // FIX
  //needs lookup for static geometric factors!


};

#endif
