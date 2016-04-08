///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Get the mcEventWeight with index #McEventWeightIndex from
// EventInfo
//
// Author: Olivier Arnaez <olivier.arnaez@cern.ch>
///////////////////////////////////////////////////////////////////

#ifndef MCEVENTWEIGHT_H
#define MCEVENTWEIGHT_H 1

// Include the base class
#include "ReweightUtils/WeightToolBase.h"


class McEventWeight : public WeightToolBase
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS2( McEventWeight, IWeightTool, IObservableTool )

   public:
    /// The default constructor with the instance name of the tool class
    McEventWeight( const std::string& name );

    /// Default destructor
    virtual ~McEventWeight() {};

    /// returns: the value that was calculated from the xAOD::IParticle (composite built event object for instance)
    virtual double evaluate( const xAOD::IParticle* part ) const override;

    /// returns: the value that was calculated from the usual Athena storegate
    virtual double getWeight() const override;


   private:
    //internal function "computing" the weight
    double computeWeight(const xAOD::EventInfo*) const;

    int m_McEventWeightIndex;

    std::string m_eventInfoName;

};

#endif //> !MCEVENTWEIGHT_H
