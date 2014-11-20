// This file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUTILS_JETCALOCELLQUALHELPER_H
#define JETUTILS_JETCALOCELLQUALHELPER_H

#include "JetUtils/JetCaloQualityUtils.h"



class CaloCell;

namespace jet {

  ////////////////////////////////////////////////////////////
  /// \class JetCaloCellCalculator
  /// \brief Base class (inheriting JetCaloCalculator) for calculations accessing CaloCell directly.
  ///
  /// A base class which virtually inherit JetCaloCalculator and provide the interface 
  /// to perform calo calculation from jets, using CaloCell acces directly.
  /// 
  /// This class is available *only* in Athena.
  ///
  /// The virutal inheritance allow concrete implementations to also inherit the
  /// corresponding implementations of JetCaloCalculator. For example :
  ///      JetCalcNegativeEnergy_fromCells inherits JetCalcNegativeEnergy
  ///      JetCalcNegativeEnergy_fromCells only needs to implement processCell() and clone()
  ///
  /// *IMPORTANT* : cell based calculation are not necessarily identical to their correspondant
  /// cluster based calculation.
  /// Several cell calculation actually differ or can depend on parameters different than those used for the
  /// cluster based quantity.
  ///
  class JetCaloCellCalculator : virtual public JetCaloCalculator {
  public:
    /// perform calculation for a single cell 
    virtual bool processCell(const CaloCell* /*cell*/, weight_t /*w*/)  =0;
    virtual double operator()(const xAOD::Jet* jet, xAOD::JetConstitScale s=xAOD::UncalibratedJetConstituent);

    virtual JetCaloCellCalculator* clone()const = 0;

  };


  /// DECLARECELLCALC : a macro to ease the declaration of concrete implementations of JetCaloCellCalculator
  ///   \param baseclass    : the class to be inherited.
  ///   \param derivedclass : the JetCaloCellCalculator implentation to declare
  ///   \param inits        : code to be added to derivedclass constructor  
  ///
#define DECLARECELLCALC( baseclass, derivedclass, inits ) \
  class derivedclass : public baseclass ,  public JetCaloCellCalculator  {                            \
  public:                                                               \
    derivedclass() :  baseclass() {inits;}              \
    virtual bool processCell(const CaloCell* , weight_t )  ;             \
    JetCaloCellCalculator* clone()const {return new derivedclass(*this);}    \
  }
    

  // below we declare cell based calo calculators inheriting the cluster based calculators.
  DECLARECELLCALC(JetCalcnLeadingCells, JetCalcnLeadingCells_fromCells, m_id = JetAttribute::N90Cells; );
  DECLARECELLCALC(JetCalcOutOfTimeEnergyFraction, JetCalcOutOfTimeEnergyFraction_fromCells, );
  DECLARECELLCALC(JetCalcTimeCells, JetCalcTimeCells_fromCells, );
  DECLARECELLCALC(JetCalcAverageLArQualityF, JetCalcAverageLArQualityF_fromCells, );
  DECLARECELLCALC(JetCalcQualityHEC, JetCalcQualityHEC_fromCells, );
  DECLARECELLCALC(JetCalcQuality, JetCalcQuality_fromCells, );
  DECLARECELLCALC(JetCalcNegativeEnergy, JetCalcNegativeEnergy_fromCells, );
  DECLARECELLCALC(JetCalcCentroid, JetCalcCentroid_fromCells, );
  //  DECLARECELLCALC(JetCalcBadCellsFrac, JetCalcBadCellsFrac_fromCells, );


  ////////////////////////////////////////////////////////////
  /// \class JetCaloCellCalculations
  /// \brief collection of JetCaloCellCalculator
  ///
  /// This class is similar and inherit JetCaloCalculations
  /// It's specialized for JetCaloCellCalculator . 
  class JetCaloCellCalculations : public JetCaloCalculations  {
  public:
    
    JetCaloCellCalculations(bool ownCalc=true) : JetCaloCalculations(ownCalc) {}

    /// Perform all tasks correspondings to the associated JetCaloCalculators on Jet jet.
    virtual std::vector<double> process(const xAOD::Jet* jet) const ;    

    void addCellCalculator(JetCaloCellCalculator* c);

  protected:
    std::vector<JetCaloCellCalculator*> m_cellcalculators;

  };



}

#endif
