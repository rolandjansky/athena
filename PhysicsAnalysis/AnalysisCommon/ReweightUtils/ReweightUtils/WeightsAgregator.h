///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Computes the product (multiplication) of the weights provided by
// different IWeightTools
//
// Author: Olivier Arnaez <olivier.arnaez@cern.ch>
///////////////////////////////////////////////////////////////////

#ifndef WEIGHTSAGREGATOR_H
#define WEIGHTSAGREGATOR_H 1

// Include the base class
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "ReweightUtils/WeightToolBase.h"
#include "PATInterfaces/IWeightTool.h"


class WeightsAgregator : public WeightToolBase
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS2( WeightsAgregator, IWeightTool, IObservableTool )

   public:
    /// The default constructor with the instance name of the tool class
    WeightsAgregator( const std::string& name );

    /// Default destructor
    virtual ~WeightsAgregator() {};

    /// Usual initialize method of the framework
    virtual StatusCode  initialize() override;

    /// returns: the value that was calculated from the xAOD::IParticle (composite built event object for instance)
      virtual double evaluate( const xAOD::IParticle* part ) const override;

    /// returns: the value that was calculated from the usual Athena storegate
    virtual double getWeight() const override;


   private:
    /// Name of the xAOD::EventInfo object
    std::string m_eventInfoName;

    /// Array of ingredient weight tools
    mutable ToolHandleArray< IWeightTool > m_toolArray;

  };

#endif //> !WEIGHTSAGREGATOR_H
