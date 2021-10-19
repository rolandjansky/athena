// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef RINGERSELECTORTOOLS_ASGRINGERSELECTORTOOL_H
#define RINGERSELECTORTOOLS_ASGRINGERSELECTORTOOL_H

// STL includes:
#include <string>

// Athena includes
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"
#include "AsgTools/AsgTool.h"

// xAOD includes:
#include "RingerSelectorTools/IAsgRingerSelectorTool.h"
#include "RingerSelectorTools/tools/onnx/RingerSelector.h"
#include "AthOnnxruntimeService/IONNXRuntimeSvc.h"



namespace Ringer {
  
  class AsgRingerSelectorTool : public asg::AsgTool,
                                  virtual public IAsgRingerSelectorTool
  {

  public:

    // Use ASG_TOOL macro for 2 interface base classes.
    ASG_TOOL_CLASS2(AsgRingerSelectorTool,
                    IAsgRingerSelectorTool,
                    IAsgSelectionTool)


    /** Standard constructor */
    AsgRingerSelectorTool(std::string name);

    /** Standard destructor */
    virtual ~AsgRingerSelectorTool();

    /** Gaudi Service Interface method implementations */
    virtual StatusCode initialize() override;

    /** Gaudi Service Interface method implementations */
    virtual StatusCode finalize() override;


    /**
     * @brief Method to get the plain TAccept for the last particle.
     **/
    virtual const asg::AcceptInfo& getAcceptInfo() const override{ return m_accept; };


    /**
     * @briel Calculation of model output for fast calorimetry or fast photon step (HLT usage only)
     **/
    virtual float predict(const xAOD::TrigRingerRings * ) const override;

    /**
     * @briel Calculation of model output for fast electron step (HLT usage only)
     **/
    virtual float predict(const xAOD::TrigRingerRings *, const xAOD::TrigElectron *) const override;


    /**
     * @brief Accept method
     **/
    virtual asg::AcceptData  accept(const xAOD::TrigRingerRings *, float discr, float mu ) const override;
    
    
    virtual asg::AcceptData  accept(const xAOD::IParticle *) const override;


  private:

    asg::AcceptInfo m_accept;
    onnx::RingerSelector m_selector;

    Gaudi::Property<std::vector<std::string>> m_configFiles{this, "ConfigFiles", {}, "Ringer Run3 Calib Path"};  
    ServiceHandle< AthONNX::IONNXRuntimeSvc > m_onnxSvc{ this, "ONNXRuntimeSvc", "AthONNX::ONNXRuntimeSvc", "Name of the service to use" };

};

}
#endif 
