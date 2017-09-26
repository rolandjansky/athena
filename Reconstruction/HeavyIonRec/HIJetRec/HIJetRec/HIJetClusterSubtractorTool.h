/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetClusterSubtractorTool.h

#ifndef HIJETREC_HIJETCLUSTERSUBTRACTORTOOL_H
#define HIJETREC_HIJETCLUSTERSUBTRACTORTOOL_H

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
#include <map>
#include "AsgTools/AsgTool.h"


class TH3F;

class HIJetClusterSubtractorTool : public HIJetSubtractorToolBase
{
  ASG_TOOL_CLASS(HIJetClusterSubtractorTool, IHISubtractorTool);
  
  // FIX
  //needs lookup for static geometric factors!
  
public:
  /// \brief Implements method defined in base
  /// First argument is reference to four vector that is updated to reflect
  /// the subtracted kinematics of the IParticle passed in the second arg
  /// Method expects cl_in to be a cluster
  HIJetClusterSubtractorTool(const std::string& myname);
  virtual void Subtract(xAOD::IParticle::FourMom_t&, const xAOD::IParticle*, const xAOD::HIEventShapeContainer*, const HIEventShapeIndex*, const ToolHandle<IHIUEModulatorTool>&, const xAOD::HIEventShape* eshape) const override;
  virtual void SubtractWithMoments(xAOD::CaloCluster*, const xAOD::HIEventShapeContainer*, const HIEventShapeIndex* index, const ToolHandle<IHIUEModulatorTool>&, const xAOD::HIEventShape* eshape ) const override;  
  virtual void UpdateUsingCluster(xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const xAOD::CaloCluster* cl) const override;

  virtual StatusCode initialize() override;
  virtual StatusCode initializeTool();

  private:

  bool m_init;

  float getWeight(float eta, float phi, int sample) const;
  float getWeightEta(float eta, float phi, int sample) const;
  float getWeightPhi(float eta, float phi, int sample) const;
  void updateSlice(xAOD::HIEventShape* slice, float ET, float phi0, float area_cluster) const;

  std::string m_input_file;
  std::string m_config_dir;
  TH3F* m_h3_w;
  TH3F* m_h3_eta;
  TH3F* m_h3_phi;

  bool m_use_samplings;


};

#endif


