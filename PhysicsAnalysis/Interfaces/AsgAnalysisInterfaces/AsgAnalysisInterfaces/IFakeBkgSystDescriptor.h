/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFAKE_BKG_SYST_DESCRIPTOR_H
#define IFAKE_BKG_SYST_DESCRIPTOR_H

#include "PATInterfaces/SystematicVariation.h"

#include <string>

namespace CP
{

/// \brief Auxiliary interface equiping the tools derived from IFakeBkgTool with helper methods to identify what the different SystematicVariations correspond to
/// All the tools inherit from this interface
class IFakeBkgSystDescriptor
{
  public:
  
    virtual ~IFakeBkgSystDescriptor() = default;
    
    /// \brief retrieves the systematic variation(s) associated to a particular systematic uncertainty defined in the ROOT/XML file(s) used to configure the tool
    virtual CP::SystematicSet affectingSystematicsFor(const std::string& nuisanceParameter) const = 0;

    /// \brief returns a human-readable description of the source of systematic uncertainty specified as argument
    virtual std::string getUncertaintyDescription(const CP::SystematicVariation& systematic) const = 0;
    
    /// \brief prints a human-readable description of the source of systematic uncertainty specified as argument
    virtual void printUncertaintyDescription(const CP::SystematicVariation& systematic) const = 0;
    
    /// \brief checks whether the specified source of uncertainty originates from a systematic uncertainty in the efficiencies (as opposed to statistical)
    virtual bool isSystematicUncertainty(const CP::SystematicVariation& systematic) const = 0;
    
    /// \brief checks whether the specified source of uncertainty originates from a statistical uncertainty in the efficiencies (as opposed to systematic)
    virtual bool isStatisticalUncertainty(const CP::SystematicVariation& systematic) const = 0;
    
    /// \brief checks whether the specified source of uncertainty affects electrons
    virtual bool affectsElectrons(const CP::SystematicVariation& systematic) const = 0;
    
    /// \brief checks whether the specified source of uncertainty affects muons
    virtual bool affectsMuons(const CP::SystematicVariation& systematic) const = 0;

    /// \brief checks whether the specified source of uncertainty affects taus
    virtual bool affectsTaus(const CP::SystematicVariation& systematic) const = 0;
    
    /// \brief checks whether the specified source of uncertainty affects real efficiencies
    virtual bool affectsRealEfficiencies(const CP::SystematicVariation& systematic) const = 0;
    
    /// \brief checks whether the specified source of uncertainty affects fake efficiencies
    virtual bool affectsFakeEfficiencies(const CP::SystematicVariation& systematic) const = 0;
    
    /// \brief checks whether the specified source of uncertainty affects fake factors
    virtual bool affectsFakeFactors(const CP::SystematicVariation& systematic) const = 0;
};

}

#endif
