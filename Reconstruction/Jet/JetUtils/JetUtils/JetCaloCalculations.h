// emacs, this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUTILS_JETCALOHELPER_H
#define JETUTILS_JETCALOHELPER_H

#include "xAODJet/JetConstituentVector.h"
#include "xAODJet/JetAttributes.h"

#include <vector>

#include "xAODJet/Jet.h"

#include "xAODCaloEvent/CaloCluster.h"

namespace CaloConstitHelpers {

  ////////////////////////////////////////////////////////////
  /// \class CaloConstitExtractor
  /// 
  /// \brief Interface to retrieve calo informations from a jet constituent
  ///
  /// This is an interface class which provide function to access calorimeter quantities
  /// stored in a jet constituent.
  /// Implementations of this interface exists for constituents being CaloCluster and PFO objects.
  /// 
  /// (the interface use JetConstituentVector::iterator rather than IParticle in case support for constituent weights is needed one day).
  /// 
  struct CaloConstitExtractor {
    virtual ~CaloConstitExtractor(){}
    virtual bool valid(xAOD::JetConstituentVector::iterator &) {return false;}
    virtual double moment( xAOD::JetConstituentVector::iterator & , xAOD::CaloCluster::MomentType ) {return 0;};
    virtual double time(xAOD::JetConstituentVector::iterator & ){return 0.;}
    virtual double energyHEC(xAOD::JetConstituentVector::iterator & ){return 0.;}
  };  
}

namespace jet {
  ////////////////////////////////////////////////////////////
  /// \class JetCaloCalculator
  /// 
  /// Base class to support cpu-efficient calculation on calorimeter jets
  /// either at CaloCell or constituent level (CaloCluster & PFO objects supported).
  ///
  /// JetCaloCalculator instances use the CaloCluster moments in their calculation.
  /// Similar quantities build from CaloCell directly are calculated by JetCaloCellCalculator
  /// instances. These ones are defined separately in JetCaloCellQualityUtils.h, so they are not 
  /// compiled in RootCore or AthAnalysis releases.
  ///
  /// The reason for using classes rather than simple functions to perform 
  /// cell based calculations is doing  
  ///  
  ///     for each cell { for each calculation {calculate on cell} } (1)
  ///
  /// is more efficient than doing the loop the other way around :
  ///
  ///     for each calculation { for each cell {calculate on cell} } (2)
  ///
  /// Implementing (1) in a modular way requires to use objects which can accumulate
  /// information when processing a cell.
  /// The calculation steps for 1 cell/constituent is to be implemented in processCell(cell)/processConstituent(constit)
  /// functions (whithin these functions the calculator object is expected to accumulate  
  /// relevant information from cell/constit in  private members).
  ///
  /// The function setupJet() is meant to prepare the calculation 
  /// for a given jet (for ex: reset accumulation variables).
  ///
  /// 
  /// JetCalculators are expected to be used within a JetCalculation instance.
  /// They also act as functor and define operator() so one can use them as :
  ///     JetCalcNegativeEnergy calculateNegE;
  ///     double negEinJet =  calculateNegE(jet);
  ///
  class JetCaloCalculator {
  public:
    //typedef typename Jet::navigable_type::paramter_type cellweight_t;
    typedef  double weight_t;

    JetCaloCalculator() : m_name("JetCaloCalculator"){}
    JetCaloCalculator(xAOD::JetAttribute::AttributeID id) ;

    virtual ~JetCaloCalculator(){}

    virtual bool setupEvent() {return true; };
    virtual bool setupJet(const xAOD::Jet* /*jet*/) = 0;


    /// Perform 1 calculation step using 1 constituent. 
    /// Implementation of these functions should use the kinematics from the iterator NOT from the underlying cluster<br>
    /// It is expected that the input is valid, i.e. : CaloConstitExtractor::valid(it) == true.
    virtual bool processConstituent(xAOD::JetConstituentVector::iterator& /*it*/) {return true;}; 


    /// return the result of the calculation
    virtual double jetCalculation() const {return 0;};

    /// convenience function to perform the full calculation on a given jet
    virtual double operator()(const xAOD::Jet* jet, xAOD::JetConstitScale s=xAOD::UncalibratedJetConstituent);

    virtual std::string name() const {return m_name;}
  
    void setName(std::string n){m_name = n;}


    void setExtractor(CaloConstitHelpers::CaloConstitExtractor* ex){m_constitExtractor = ex;}

    /// Clone. Base class should not be cloned.
    virtual JetCaloCalculator* clone() const =0;

  protected:
    std::string m_name;
    xAOD::JetAttribute::AttributeID m_id; // unused for now.
    CaloConstitHelpers::CaloConstitExtractor *m_constitExtractor; // used to retrieve the cluster moment from the constituent (which can be a PFO object or a cluster)
  };


  /// JETCALCFUNCDECL macro : ease the declaration of class inheriting JetCaloCalculator
  ///   cname     : the class name
  ///   id        : A JetAttribute::AttributeID corresponding to what the calculator produces
  ///   others    : a list of instructions executed in constructor (typically in the form 'minPtCut=42;') 
#define JETCALCFUNCDECL( cname, id,  others)                            \
  public:                                                               \
  virtual bool setupJet(const xAOD::Jet* jet);                                  \
  virtual bool processConstituent(xAOD::JetConstituentVector::iterator& iter);  \
  double jetCalculation() const ;                                             \
  cname(xAOD::JetAttribute::AttributeID i=id) : JetCaloCalculator( i ) { others } \
  virtual JetCaloCalculator* clone() const {return new cname(*this); }


  ////////////////////////////////////////////////////////////
  /// \class JetCaloCalculations
  /// 
  /// \brief Holds a list of JetCalculators.
  ///
  /// For a given jet, performs a single, optimized loop over constituents and calls the
  ///  processConstituent function of all its calculators.
  /// 
  /// When processing constituents, it passes JetConstituentVector::iterator at the UncalibratedJetConstituent scale :
  ///  this is a choice (most quantities are build from cluster moments calculated at EM scale).
  ///
  /// See also JetCaloCellCalculations in JetCaloCellQualityUtils.h for a similar object 
  /// using calculators acting on CaloCell directly
  class JetCaloCalculations {
  public:

    JetCaloCalculations(bool ownCalc=true) : m_owncalculators(ownCalc) {}

    virtual ~JetCaloCalculations(){clear();}
    void addCalculator(JetCaloCalculator* c);
  
    bool setupEvent();
  
    /// Perform all tasks correspondings to the associated JetCaloCalculators on Jet jet.
    virtual std::vector<double> process(const xAOD::Jet* jet) const ;

    void clear();


    size_t numCalculators() const {return m_calculators.size();}
    const JetCaloCalculator* at(size_t i) const {return m_calculators[i];}



  protected:
    std::vector<JetCaloCalculator*>  m_calculators;
    bool m_owncalculators;
  };

}
#endif
