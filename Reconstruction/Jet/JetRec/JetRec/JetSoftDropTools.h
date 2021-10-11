/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETSOFDROPTOOLS_H
#define JETREC_JETSOFDROPTOOLS_H
///***********************************************
///
/// \file JetSoftDropTools
///
/// Define jet grooming tools implementing variants of the soft drop algorithm
///
/// These tools implement the IJetProvider interface via the JetGroomer base class.
/// The JetContainer thet returns is build by
/// running the trimming procedure on each jet of the parent JetContainer the tool is acting on.
/// Obviously the parent JetContainer must be present in the evt store, but also the input PseudoJetContainer from which it has been built.

#include "fastjet/PseudoJet.hh"
#include "fastjet/tools/Filter.hh"

#include "xAODJet/JetContainer.h"

#include "JetInterface/IJetProvider.h"
#include "JetRec/JetGroomer.h"
#include "JetRec/PseudoJetContainer.h"

#include "AsgTools/PropertyWrapper.h"


namespace JetGrooming {

  /// \class SoftDrop
  ///
  /// A grooming tool implementing the SoftDrop method by wrapping fastjet::contrib::SoftDrop
  /// Also serves as a base class for other SoftDrop variant.
  class SoftDrop
    : virtual public JetGroomer {
    ASG_TOOL_CLASS(SoftDrop, IJetProvider)

    public:
    using JetGroomer::JetGroomer;
    
    StatusCode initialize() override ;

    virtual void insertGroomedJet(const xAOD::Jet&, const PseudoJetContainer&, xAOD::JetContainer&, PseudoJetVector&) const override ;

  protected:

    void setSoftDropAttributes(xAOD::Jet& groomedjet, int nsdsubjets) const ;
    
    Gaudi::Property<float> m_zcut  {this, "ZCut", 0.1 , "pT fraction for retaining subjets"}; 
    Gaudi::Property<float> m_beta {this, "Beta", 0.0, "How much to consider angular dependence"};
    Gaudi::Property<float> m_R0   {this, "R0", 1.0, "Normalization of angular distance, usually the characteristic jet radius (default R0 = 1)"};

    Gaudi::Property<bool> m_saveSDatt {this, "SaveSoftDropAttributes",false, ""};
  
  };



  /// \class RecSoftDrop
  ///
  /// A specialized SoftDrop tool wrapping fastjet::contrib::RecursiveSoftDrop 
  class RecSoftDrop:   public SoftDrop {
    ASG_TOOL_CLASS0( RecSoftDrop )    
    public:
    using SoftDrop::SoftDrop;//

    StatusCode initialize() override final ;
    
    virtual void insertGroomedJet(const xAOD::Jet&, const PseudoJetContainer&, xAOD::JetContainer&, PseudoJetVector&) const override final ;

  private:
    Gaudi::Property<int> m_N {this, "N", 1 , "Number of layers (-1 <> infinite)"};     
  };





  /// \class BottomUpSoftDrop
  ///
  /// A specialized SoftDrop tool wrapping fastjet::contrib::BottomUpSoftDrop
  class BottomUpSoftDrop: virtual public SoftDrop {
    ASG_TOOL_CLASS0(BottomUpSoftDrop)    
  public:
    using SoftDrop::SoftDrop;
    
    virtual void insertGroomedJet(const xAOD::Jet&, const PseudoJetContainer&, xAOD::JetContainer&, PseudoJetVector&) const override final ;
  };
  

  
}


#endif
