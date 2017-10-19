// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IBTaggingSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CPIBTAGGINGSELECTIONTOOL_H
#define CPIBTAGGINGSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODJet/Jet.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATCore/TAccept.h"
#include <string>

class IBTaggingSelectionTool : virtual public asg::IAsgTool {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( IBTagSelectionTool )

    public:

    virtual const Root::TAccept& getTAccept() const = 0;
    /// Get the decision using a generic IParticle pointer
    virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const = 0;
    virtual const Root::TAccept& accept( const xAOD::Jet& j ) const = 0;
    /// Get the decision using thet jet's pt and weight values (number of weight values depends on which tagger is used)
    virtual const Root::TAccept& accept(double /* jet pt */, double /* jet eta */, double /* tag_weight */ ) const = 0;
    virtual const Root::TAccept& accept(double /* jet pt */, double /* jet eta */, double /* mv2cl100 weight */, double /* mv2c100 weight */ ) const = 0;
    virtual const Root::TAccept& accept(double /* jet pt */, double /* jet eta */, double /* dl1pb */, double /* dl1pc  */ , double /* dl1pu  */) const = 0;

    /// Decide in which quantile of the tagger weight distribution the jet belongs
    /// The return value represents the bin index of the quantile distribution
    virtual int getQuantile( const xAOD::IParticle* ) const = 0;
    virtual int getQuantile( const xAOD::Jet& ) const = 0;
    virtual int getQuantile( double, double, double ) const = 0;

    virtual CP::CorrectionCode getCutValue(double /* jet pt */, double & cutval) const = 0;
    virtual CP::CorrectionCode getTaggerWeight( const xAOD::Jet& jet, double & weight ) const = 0;
    virtual CP::CorrectionCode getTaggerWeight( double /* dl1pb */, double /* dl1pc  */ , double /* dl1pu  */ , double & weight) const = 0;

  };
#endif // CPIBTAGGINGSELECTIONTOOL_H
