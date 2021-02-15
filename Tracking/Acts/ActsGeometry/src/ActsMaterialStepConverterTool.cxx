/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsMaterialStepConverterTool.h"

// ATHENA
#include "GaudiKernel/EventContext.h"

// PACKAGE
#include "ActsGeometryInterfaces/IActsTrackingGeometrySvc.h"

// Tracking
#include "TrkGeometry/MaterialStep.h"

// ACTS
#include "Acts/Material/MaterialSlab.hpp"

// STL
#include <iostream>
#include <memory>

ActsMaterialStepConverterTool::ActsMaterialStepConverterTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent)
{
}

StatusCode
ActsMaterialStepConverterTool::initialize()
{
  ATH_MSG_INFO(name() << " initializing");

  return StatusCode::SUCCESS;
}


const Acts::RecordedMaterialTrack
ActsMaterialStepConverterTool::convertToMaterialTrack(const Trk::MaterialStepCollection &colStep) const
{
  Acts::RecordedMaterialTrack mTrack;
  std::vector<Acts::MaterialInteraction> nStep;
  Acts::RecordedMaterial recorded;
  double sum_X0 = 0;
  double sum_L0 = 0;

  double x_lengh = colStep.back()->hitX() - colStep.front()->hitX();
  double y_lengh = colStep.back()->hitY() - colStep.front()->hitY();
  double z_lengh = colStep.back()->hitZ() - colStep.front()->hitZ();
  double r_lengh = colStep.back()->hitR() - colStep.front()->hitR();

  double norm = 1/(std::sqrt(x_lengh*x_lengh +
                                 y_lengh*y_lengh +
                                 z_lengh*z_lengh));


  Acts::Vector3D v_pos{0, 0, colStep.front()->hitZ() - (z_lengh/r_lengh)*colStep.front()->hitR() };
  Acts::Vector3D v_imp{x_lengh*norm, y_lengh*norm, z_lengh*norm};
  Acts::Vector3D prev_pos = v_pos;

  for(auto const step: colStep) {

    Acts::MaterialInteraction interaction;

    Acts::Vector3D pos{step->hitX(), step->hitY(), step->hitZ()};
    Acts::MaterialSlab matProp(Acts::Material::fromMassDensity(step->x0(), step->l0(), step->A(), step->Z(), step->rho()),step->steplength());
    interaction.position = pos;

    double x_dir = pos.x() - prev_pos.x();
    double y_dir = pos.y() - prev_pos.y();
    double z_dir = pos.z() - prev_pos.z();
    double norm_dir = 1/(std::sqrt(x_dir*x_dir +
                                 y_dir*y_dir +
                                 z_dir*z_dir));

    Acts::Vector3D dir{x_dir * norm_dir, y_dir * norm_dir, z_dir * norm_dir};
    interaction.direction = dir;
    prev_pos = pos;

    interaction.materialSlab = matProp;
    sum_X0 += step->steplengthInX0();
    sum_L0 += step->steplengthInL0();
    nStep.push_back(interaction);
  }

  recorded.materialInX0 = sum_X0;
  recorded.materialInL0 = sum_L0;
  recorded.materialInteractions = nStep;

  mTrack = std::make_pair(std::make_pair(v_pos, v_imp), recorded);

return mTrack;

}
