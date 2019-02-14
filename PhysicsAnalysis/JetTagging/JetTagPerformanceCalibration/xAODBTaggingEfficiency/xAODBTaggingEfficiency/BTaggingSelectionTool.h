// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BTaggingSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
/**
  @class BTaggingSelectionTool
  Tool to apply flavour-tagging requirements on jets
  @author C. Lüdtke, M. Ughetto
  @contact cluedtke@cern.ch, mughetto@cern.ch
*/

#ifndef CPBTAGGINGSELECTIONTOOL_H
#define CPBTAGGINGSELECTIONTOOL_H

#include "xAODBTaggingEfficiency/IBTaggingSelectionTool.h"
#include "xAODBTagging/BTagging.h"

#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"

#include "TFile.h"
#include "TSpline.h"
#include "TVector.h"
#include <string>
#include <set>
#include <vector>
#include <map>

class BTaggingSelectionTool: public asg::AsgTool, 
			     public virtual IBTaggingSelectionTool,
			     public virtual IAsgSelectionTool  {
  typedef double (xAOD::BTagging::* tagWeight_member_t)() const;

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( BTaggingSelectionTool , IAsgSelectionTool, IBTaggingSelectionTool )
  
  public:  
  /// Create a constructor for standalone usage
  BTaggingSelectionTool( const std::string& name );
  virtual StatusCode initialize() override;
  /// Get an object describing the "selection steps" of the tool
  virtual const asg::AcceptInfo& getAcceptInfo() const override;

  /// Get the decision using a generic IParticle pointer
  virtual asg::AcceptData accept( const xAOD::IParticle* p ) const override;
  virtual asg::AcceptData accept( const xAOD::Jet& jet ) const override;

  /// Get the decision using thet jet's pt and mv2c20 weight values
  virtual asg::AcceptData accept(double /* jet pt */, double /* jet eta */, double /* mv2c20 weight */ ) const override;
  virtual asg::AcceptData accept(double /* jet pt */, double /* jet eta */, double /* mv2c00 weight */, double /* mv2c100 weight */ ) const override;

  /// Decide in which quantile of the MV2c20 weight distribution the jet belongs (continuous tagging)
  /// The return value represents the bin index of the quantile distribution
  virtual int getQuantile( const xAOD::IParticle* ) const override;
  virtual int getQuantile( const xAOD::Jet& ) const override;
  virtual int getQuantile( double /* jet pt */, double /* jet eta */, double /* mv2c20 weight */  ) const override;

  virtual double getCutValue() const override; // Only for 1D flat cut

private:
  /// Helper function that decides whether a jet belongs to the correct jet selection for b-tagging
  virtual bool checkRange( double pT, double eta,
                           asg::AcceptData& acceptData) const;

  bool m_initialised;

  /// Object used to store selection information.
  asg::AcceptInfo m_accept;
 
  double m_maxEta;
  double m_minPt;
  double m_maxRangePt;
  std::string m_CutFileName;
  std::string m_taggerName;
  std::string m_OP;
  std::string m_jetAuthor;

  TFile *m_inf;
  TSpline3 *m_spline;
  TVector *m_constcut;
  double m_continuouscuts[6];

};

#endif // CPBTAGGINGSELECTIONTOOL_H

