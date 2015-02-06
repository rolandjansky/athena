/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetCellSubtractorTool.h

#ifndef __HIJETREC_HIJETCELLSUBTRACTORTOOL_H__
#define __HIJETREC_HIJETCELLSUBTRACTORTOOL_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetCellSubtractorTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Concrete implementation of HIJetSubtractorToolBase.
/// Class used by HIClusterSubtraction and HIJetConstituentSubtractionTool
/// to provide kinematics of a cluster after subtraction given an event shape.
/// This version of the tool navigates back to the cells.
/// 
////////////////////////////////////////////////////////////////////////////////


#include "HIJetRec/HIJetSubtractorToolBase.h"
#include "AsgTools/AsgTool.h"

class HIJetCellSubtractorTool : public HIJetSubtractorToolBase

{
  ASG_TOOL_CLASS(HIJetCellSubtractorTool, IHISubtractorTool)
  HIJetCellSubtractorTool(const std::string& myname);

  public:

  /// \brief Implements method defined in base
  /// Navigates back to cells to do subtraction
  /// First argument is reference to four vector that is updated to reflect
  /// the subtracted kinematics of the IParticle passed in the second arg
  /// Method expects cl_in to be a cluster
  virtual void Subtract(xAOD::IParticle::FourMom_t& subtr_mom, const xAOD::IParticle* cl_in, const xAOD::HIEventShapeContainer* shape);

};

#endif
