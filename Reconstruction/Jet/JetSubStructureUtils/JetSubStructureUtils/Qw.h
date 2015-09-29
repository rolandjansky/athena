// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_qw_header
#define jetsubstructureutils_qw_header
/////////////////////////////////////////////
/// \class Qw
///
/// 
/// Calculate the Qw substructure variable : minimum invariant mass of pairs from 3 subjets.
/// This implementation supports 3 different ways of finding the 3 subjets.
/// These 3 variants are chosen by the 1st argument to the constructor.
///    - Normal : exactly 3 subjets are requested by calling ClusterSequence::exclusive_jets(3) on the constituents.
///      (in this case the 2nd argument of ctor is ignored).
///   The 2 other variants are (very) experimental :
///    - MassCut : same as above, then the subjets are splitted into their parents if their mass > massCut 
///      (this cut is set by the 2nd argument of ctor). This splitting is done recursively as long as mass > massCut.
///       Finally we choose the 3 highest pt subjets. 
///    - SmallSubjets : Consider the 3 highest pt subjets obtained from ClusterSequence::inclusive_jets() on the constituents.
///      In this case the 2nd argument of ctor is the subjet alg radius.
///
///  In the MassCut and SmallSubjets mode the Qw is rescaled by a factor (Total Pt)/(Sum 3 subjets Pt).
///
/// \author P-A Delsart
/// \date April 2015
///////////////////////////////////////////////////////
#include "JetSubStructureUtils/SubstructureCalculator.h"
#include "fastjet/JetDefinition.hh"

namespace JetSubStructureUtils {
  class Qw : public SubstructureCalculator<double> { 
    public:

    /// describes the differnt calculation variants
    enum QwVariant {
      Normal,
      MassCut,
      SmallSubjets,
    };
    
    Qw( QwVariant mode=Normal, double p=-1, fastjet::JetAlgorithm jetalg=fastjet::kt_algorithm) ;
    
    using SubstructureCalculator::result;
    /// SubstructureCalculator interface
    virtual double result(const fastjet::PseudoJet &jet) const;

    /// Implementation of the calculation.     
    virtual double result(std::vector<fastjet::PseudoJet> &constituents) const;
    
    
  private:
    QwVariant m_vMode;
    fastjet::JetDefinition m_jetdef;
    
    double m_massCut;
  };
}

#endif
