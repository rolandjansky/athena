// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BTaggingSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
/**
  @class BTaggingSelectionTool
  Tool to apply flavour-tagging requirements on jets
  @author C. Lüdtke, M. Ughetto
  @contact cluedtke@cern.ch, mughetto@cern.ch

  Note for getTaggerWeight and getCutValue: the LAST defaulted argument
  is now 'useCTag', but before AB 21.2.221 it was 'useVetoWP', which has the 
  **opposite** meaning when set to true. 

*/

#ifndef CPBTAGGINGSELECTIONTOOL_H
#define CPBTAGGINGSELECTIONTOOL_H

#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "xAODBTagging/BTagging.h"

#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"

#include "TFile.h"
#include "TSpline.h"
#include "TVector.h"
#include "TMatrixD.h"
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
  StatusCode initialize();
  /// Get an object describing the "selection steps" of the tool
  virtual const Root::TAccept& getTAccept() const;

  /// Get the decision using a generic IParticle pointer
  virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const;
  virtual const Root::TAccept& accept( const xAOD::Jet& jet ) const;

  /// Get the decision using thet jet's pt and mv2c20 weight values
  virtual const Root::TAccept& accept(double /* jet pt */, double /* jet eta */, double /* tag_weight */ ) const;
  virtual const Root::TAccept& accept(double /* jet pt */, double /* jet eta*/, double /* taggerWeight_b */, double /* taggerWeight_c */) const;
  virtual const Root::TAccept& accept(double /* jet pt */, double /* jet eta */, double /* dl1pb */, double /* dl1pc  */ , double /* dl1pu  */) const;

  /// Decide in which quantile of the MV2c20 weight distribution the jet belongs (continuous tagging)
  /// The return value represents the bin index of the quantile distribution
  virtual int getQuantile( const xAOD::IParticle* ) const;
  virtual int getQuantile( const xAOD::Jet& ) const;
  virtual int getQuantile( double /* jet pt */, double /* jet eta */, double /* tag weight */  ) const;
  virtual int getQuantile(double /*pT*/, double /*eta*/, double /*tag_weight_b*/, double /*tag_weight_c*/ ) const;

  virtual CP::CorrectionCode getCutValue(double /* jet pt */, double & cutval) const;
  virtual CP::CorrectionCode getTaggerWeight( const xAOD::Jet& jet, double & weight ,bool useCTag = false) const;
  virtual CP::CorrectionCode getTaggerWeight( double /* dl1pb */, double /* dl1pc  */ , double /* dl1pu  */ , double & weight,bool useCTag = false) const;

private:
  /// Helper function that decides whether a jet belongs to the correct jet selection for b-tagging
  virtual bool checkRange( double /* jet pt */, double /* jet eta */ ) const;
  //fill the spline or vector that store the cut values for a particular working point
  void InitializeTaggerVariables(std::string taggerName,std::string OP, TSpline3 *spline, TVector *constcut, double &fraction);

  bool m_initialised;

  bool m_ErrorOnTagWeightFailure;
  bool m_StoreNConstituents = false;
  bool m_continuous   = false; //Continuous1D
  bool m_continuous2D = false; //Continuous2D
   /// Object used to store the last decision
  mutable Root::TAccept m_accept;

  double m_maxEta;
  double m_minPt;
  double m_maxRangePt;

  std::string m_CutFileName;
  std::string m_taggerName;
  std::string m_OP;
  std::string m_jetAuthor;
  std::string m_ContinuousBenchmarks;

  TFile *m_inf;
  double m_continuouscuts[6];

  struct taggerproperties{
    std::string name;
    double fraction_b;
    double fraction_c;
    TSpline3* spline;
    TVector*  constcut; 
    TMatrixD*  cuts2D; //useful only in Continuous2D
    std::vector<int> benchmarks; //useful only in Continuous WP. list of bins that are considered as tagged. 
  };

  taggerproperties m_tagger;

  enum Tagger{UNKNOWN, DL1, DL1r, MV2c10};
  Tagger m_taggerEnum;

  Tagger SetTaggerEnum(std::string taggerName){
    if(taggerName == "DL1r") return Tagger::DL1r;
    else if(taggerName == "DL1") return Tagger::DL1;
    else if(taggerName == "MV2c10") return Tagger::MV2c10;
    else 
      ATH_MSG_ERROR("Tagger Name NOT supported.");
    return Tagger::UNKNOWN;
  };
  //get from the CDI file the taggers cut object(that holds the definition of cut values)
  //and flaovur fraction (for DL1 tagger) and store them in the right taggerproperties struct
  void ExtractTaggerProperties(taggerproperties& tagger, std::string taggerName, std::string OP);

  std::vector<std::string> split (const std::string &input, const char &delimiter);

};

#endif // CPBTAGGINGSELECTIONTOOL_H
