///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SUSYIDWeight.h
// Header file for class SUSYIDWeight
// Author: M.Tripiana <tripiana@cern.ch>                                               
//         based on the McEventWeight class by O.Arnaez
///////////////////////////////////////////////////////////////////
#ifndef DerivationFramework_SUSY_ID_WEIGHT_H
#define DerivationFramework_SUSY_ID_WEIGHT_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "ReweightUtils/WeightToolBase.h"

//namespace DerivationFramework {

  class SUSYIDWeight : public WeightToolBase
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS2( SUSYIDWeight, IWeightTool, IObservableTool )
    
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    public:
    
    /// The default constructor with the instance name of the tool class
    SUSYIDWeight( const std::string& name );
    
    /// Destructor:
    virtual ~SUSYIDWeight(){};
    
    /// returns: the value that was calculated from the xAOD::IParticle (composite built event object for instance)
    virtual double evaluate( const xAOD::IParticle* part ) const override;
    
    /// returns: the value that was calculated from the usual Athena storegate
    virtual double getWeight() const override;
    
    
  private:
    //internal function "computing" the weight
    double computeWeight(const xAOD::EventInfo*) const;
    
    int m_SUSYProcID;
    std::string m_eventInfoName;  
    bool m_useTruthEvents = false;
    
  };
//}
  
#endif //> !DerivationFramework_SUSY_ID_WEIGHT
