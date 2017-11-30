/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IHISubtractorTool.h

#ifndef HIJETREC_IHISUBTRACTORTOOL_H
#define HIJETREC_IHISUBTRACTORTOOL_H

#include "AsgTools/IAsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODBase/IParticle.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloCluster.h"


////////////////////////////////////////////////////////////////////////////////
///
/// \class IHISubtractorTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Abstract interface for tools that implement constituent based
/// subtraction.
///
////////////////////////////////////////////////////////////////////////////////

class HIEventShapeIndex;
class IHIUEModulatorTool;

class IHISubtractorTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IHISubtractorTool)

  public:

  virtual ~IHISubtractorTool() { };

  /// \brief Abstract method where particle itself is not modified
  /// IParticle::FourMom_t containing kinematics after subtraction is passed by reference

  virtual void Subtract(xAOD::IParticle::FourMom_t&, const xAOD::IParticle*, const xAOD::HIEventShapeContainer*, const HIEventShapeIndex*, const ToolHandle<IHIUEModulatorTool>&, const xAOD::HIEventShape* shape ) const = 0;
  virtual void SubtractWithMoments(xAOD::CaloCluster*, const xAOD::HIEventShapeContainer*, const HIEventShapeIndex* index, const ToolHandle<IHIUEModulatorTool>&, const xAOD::HIEventShape* shape ) const = 0;  

  /// \brief Method to update the shape based on a given cluster
  /// two sets of indices are passed by reference and updated by the method
  /// sets are queried to see if the cluster has already been used, e.g. by another jet seed
  /// checking/updating sets prevents double counting
  /// eta-bin set used to determine eta-averaged flow
  virtual void UpdateUsingCluster(xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const xAOD::CaloCluster* cl) const = 0;
  virtual float MinEnergyForMoments() const = 0;
  virtual bool usesCells() const = 0;

  virtual const xAOD::HIEventShapeContainer* getShape() const = 0;
  virtual const HIEventShapeIndex* getIndex() const = 0;
  virtual const IHIUEModulatorTool* getModulator() const = 0;
  virtual void setShape(const xAOD::HIEventShapeContainer*) = 0;
  virtual void setIndex(const HIEventShapeIndex*) = 0;
  virtual void setModulator(const IHIUEModulatorTool*) = 0;

};

#endif
