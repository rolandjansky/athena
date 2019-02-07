// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IBTaggingSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CPIBTAGGINGSELECTIONTOOL_H
#define CPIBTAGGINGSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODJet/Jet.h"
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"
#include <string>

class IBTaggingSelectionTool : virtual public asg::IAsgTool {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( IBTagSelectionTool )

    public:

    virtual const asg::AcceptInfo& getAcceptInfo() const = 0;
    /// Get the decision using a generic IParticle pointer
    virtual asg::AcceptData accept( const xAOD::IParticle* p ) const = 0;
    virtual asg::AcceptData accept( const xAOD::Jet& j ) const = 0;
    /// Get the decision using thet jet's pt and mv2c20 weight values
    virtual asg::AcceptData accept(double /* jet pt */, double /* jet eta */, double /* mv2c20 weight */ ) const = 0;
    virtual asg::AcceptData  accept(double /* jet pt */, double /* jet eta */, double /* mv2c20 weight */, double /* mv2c20 weight */ ) const = 0;
    /// Decide in which quantile of the MV2c20 weight distribution the jet belongs
    /// The return value represents the bin index of the quantile distribution
    virtual int getQuantile( const xAOD::IParticle* ) const = 0;
    virtual int getQuantile( const xAOD::Jet& ) const = 0;
    virtual int getQuantile( double, double, double ) const = 0;

    virtual double getCutValue() const = 0;

  };
#endif // CPIBTAGGINGSELECTIONTOOL_H
