// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef RINGERSELECTORTOOLS_IASGRINGERSELECTORTOOL_H
#define RINGERSELECTORTOOLS_IASGRINGERSELECTORTOOL_H


#include "PATCore/IAsgSelectionTool.h"
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"

#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigRinger/TrigRingerRings.h"


namespace Ringer {
  
  class IAsgRingerSelectorTool : virtual public IAsgSelectionTool
  {

    ASG_TOOL_INTERFACE(IAsgRingerSelectorTool)

    public:


      /** Standard destructor */
      virtual ~IAsgRingerSelectorTool() {};

      /**
       * @brief Method to get the plain TAccept for the last particle.
       **/
      virtual const asg::AcceptInfo& getAcceptInfo() const = 0;


      /**
       * @brief Prepare the input model
       **/
      virtual std::vector<std::vector<float>> prepare_inputs( const xAOD::TrigRingerRings*, const xAOD::TrigElectron* ) const = 0;


      /**
       * @briel Calculation of model output for fast calorimetry step
       **/
      virtual float predict(const xAOD::TrigRingerRings *, std::vector<std::vector<float>> &) const = 0;
            


      /**
       * @brief Accept method
       **/
      virtual asg::AcceptData  accept(const xAOD::TrigRingerRings *, float discriminant, float mu ) const=0;



      virtual asg::AcceptData accept( const xAOD::IParticle* part ) const = 0;
  };
}

#endif 
