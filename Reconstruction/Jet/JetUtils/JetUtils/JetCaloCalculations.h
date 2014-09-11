// emacs, this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUTILS_JETCALOHELPER_H
#define JETUTILS_JETCALOHELPER_H


//#include "CaloIdentifier/CaloCell_ID.h"

//#include "CaloEvent/CaloSampling.h"

#include "xAODJet/JetConstituentVector.h"
#include "xAODJet/JetAttributes.h"

#include <vector>

#include "xAODJet/Jet.h"

class CaloCell;

namespace jet {
  ////////////////////////////////////////////////////////////
  /// \class JetCaloCalculator
  /// 
  /// Base class to support cpu-efficient calculation on calorimeter jets
  /// either at CaloCell or constituent level (currently only CaloCluster supported).
  /// The idea is doing 
  ///     for each cell { for each calculation {calculate on cell} }
  /// is more efficient than doing the loop the other way around :
  ///     for each calculation { for each cell {calculate on cell} }
  ///
  /// The actual calculation on a cell/constituent is to be performed in processCell/processConstituent
  /// function (The calculator is supposed to accumulate the jet-level necessary information as a private member)
  /// The function setupJet is meant to prepare (for ex: reset accumulation variables) the calculation 
  /// for a given jet.
  /// the bool member m_cellLevelCalculation is true when no cluster-level calculation should be attempted before
  ///  cell-level calculation.
  /// 
  /// JetCalculators are expected to be used within a JetCalculation instance
  class JetCaloCalculator {
  public:
    //typedef typename Jet::navigable_type::paramter_type cellweight_t;
    typedef  double weight_t;

    //JetCaloCalculator() : m_name("JetCaloCalculator"), m_cellLevelCalculation(true){}
    JetCaloCalculator(xAOD::JetAttribute::AttributeID id=xAOD::JetAttribute::UnkownAttribute) ;

    virtual ~JetCaloCalculator(){}

    virtual bool setupEvent() {return true; };
    virtual bool setupJet(const xAOD::Jet* /*jet*/) {return true ;};

    /// perform calculation for a single cell 
    virtual bool processCell(const CaloCell* /*cell*/, weight_t /*w*/) {return true;};

    ///virtual bool processConstituent(JetConstituentIterator& /*it*/, weight_t /*w*/) {return true;}; // not used for now
    virtual bool processConstituent(xAOD::JetConstituentVector::iterator& /*it*/, weight_t /*w*/) {return true;}; // ABC: is now used

    //virtual bool processCluster(const xAOD::CaloCluster* /*clust*/, weight_t /*w*/) {return true;};

    /// return the result of the calculation
    virtual double jetCalculation() {return 0;};

    /// convenience function to perform the full calculation on a given jet
    double operator()(const xAOD::Jet* jet);

    virtual std::string name(){return m_name;}
    virtual bool cellLevelCalculation(){return m_cellLevelCalculation;}
  
    void setName(std::string n){m_name = n;}

    /// ABC choose between cells and clusters for calculations: true is the default and  means cells 
    void setCellLevelCalculation(bool level){m_cellLevelCalculation=level;};

  protected:
    std::string m_name;
    xAOD::JetAttribute::AttributeID m_id;
    bool m_cellLevelCalculation;
  };

  /// JETCALCFUNCDECL macro : ease the declaration of class inheriting JetCaloCalculator
  ///   cname     : the class name
  ///   id        : A JetAttribute::AttributeID corresponding to what the calculator produces
  ///   others    : a list of instructions executed in constructor (typically in the form 'minPtCut=42;') 
#define JETCALCFUNCDECL( cname, id,  others)                            \
  public:                                                               \
  bool setupJet(const xAOD::Jet* jet);                                  \
  bool processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t w); \
  double jetCalculation() ;                                             \
  cname(xAOD::JetAttribute::AttributeID i=id) : JetCaloCalculator( i ) {  others } \
  bool processCell(const CaloCell *cell, weight_t w)


  ////////////////////////////////////////////////////////////
  /// \class JetCaloCalculations
  /// 
  /// Holds a list of JetCalculators.
  /// For a given jet, it performs a single, optimized loop over cells and calls the
  /// processCell function of all its calculators.
  class JetCaloCalculations {
  public:

    ~JetCaloCalculations(){clear();}
    void addCalculator(JetCaloCalculator* c);
  
    bool setupEvent();
  
    /// Perform all tasks correspondings to the associated JetCaloCalculators on Jet jet.
    bool process(const xAOD::Jet* jet);

    void clear();
    /// return the results of all JetCaloCalculators (this vector is parrallel to the vector returned by calculators() )
    std::vector<double> calculations();

    /// the vector of JetCaloCalculators 
    std::vector<JetCaloCalculator*>& calculators(){return m_calculators;}
  protected:
    //std::vector<JetCaloCalculator> calculatorvec_t;
    std::vector<JetCaloCalculator*>  m_calculators;

  };

}
#endif
