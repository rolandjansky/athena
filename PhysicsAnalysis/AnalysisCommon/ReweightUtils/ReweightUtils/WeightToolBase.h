///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class for a per-event weight computation
// The getWeight() function computes and returns the weight
// while the evaluate() function (from IObservableTool) is dedicated
// to be called mainly when the information already exists
// The addDecoration function can be called to add the computed
// weight to the EventInfo
//
// Author: Olivier Arnaez <olivier.arnaez@cern.ch>
///////////////////////////////////////////////////////////////////

#ifndef WEIGHTOOLBASE_H
#define WEIGHTOOLBASE_H 1

// Include the base class
#include "PATInterfaces/IObservableTool.h"
#include "AsgTools/AsgTool.h"

#include "PATInterfaces/IWeightTool.h"
#include "PATInterfaces/IObservableTool.h"
#include "xAODEventInfo/EventInfo.h"


class WeightToolBase : public asg::AsgTool, virtual public IWeightTool, virtual public IObservableTool
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS2( WeightToolBase, IWeightTool, IObservableTool )

   public:
    /// The default constructor with the instance name of the tool class
    WeightToolBase( const std::string& name );

    /// Default destructor
    virtual ~WeightToolBase() {};

    /// Usual initialize method of the framework
    virtual StatusCode  initialize() override;

    /// Usual finalize method of the framework
#ifndef XAOD_STANDALONE
    virtual StatusCode  finalize() override;
#else
    virtual StatusCode finalize();
#endif

    /*/// returns: the value that was calculated from the xAOD::IParticle (composite built event object for instance)
      virtual double evaluate( const xAOD::IParticle* part ) const=0;*/

    /// returns: the value that was calculated from the usual Athena storegate
    virtual double getWeight() const override = 0;


   protected:
    //add weight to EventInfo as decoration
    bool addDecoration(const xAOD::EventInfo* evtInfo, float weight) const;

    std::string m_prefixWeightDecoration;
    std::string m_weightName;

  };

#endif //> !WEIGHTOOLBASE_H
