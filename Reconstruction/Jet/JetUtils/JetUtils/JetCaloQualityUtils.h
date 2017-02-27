// emacs, this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  
     @file JetCaloQualityUtils 
     Collection of utilities to compute calorimeter based variables for jet quality 
     @author Nikola Makovec
     @author P-A Delsart
     @date (first implementation) December , 2009
     @date (re-write, run2) February-November, 2014
*/

#ifndef JETUTIL_JETCALOQUALITYUTILS_H
#define JETUTIL_JETCALOQUALITYUTILS_H

#include "JetUtils/JetCaloCalculations.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetAttributes.h"


namespace jet {

  using xAOD::Jet;
  class JetCaloQualityUtils {
  public:
    friend class JetCalcnLeadingCells;

    /* @brief Compute em fraction for the vector of e per sampling (jet->getAttribute(EnergyPerSampling))*/
    static double emFraction(const std::vector<float>& ePerSampling);

    /* @brief Compute the maximum of E_layer/Ejet*/
    static double fracSamplingMax(const Jet* jet, int& SamplingMax);

    // /* @brief Compute Out-Of-Time energy fraction from cells*/
    // static double jetOutOfTimeEnergyFraction(const Jet* jet, const double time=25, const bool doOnlyPos=false);

    // /* @brief DEPRECATED still here to avoid a compilation failure in JetD3PDMaker/ */
    static double jetOutOfTimeEnergyFractionClusters(const Jet* , const double , const bool ){return 0;}


    /* @brief Compute the jet energy fraction in HEC*/
    static double hecF(const Jet* jet);

    /* @brief Compute the jet energy fraction in presampler*/
    static double presamplerFraction(const Jet* jet);

    /* @brief Compute the jet energy fraction in TILEGAP3*/
    static double tileGap3F(const Jet* jet);

    /* @brief total negative energy in a jet*/
    static double jetNegativeEnergy(const Jet* jet);

    //  isGoodTight , isGoodMedium, isGoodMedium and 
    // isBadTight, isBadMedium, isBad
    // These functions are removed because DEPRECATED.
    // Instead use IJetSelector and/or  JetCleaningTool from PhysicsAnalysis/JetMissingEtID/JetSelectorTools

    /* @brief Check if it is a ugly jet. Still needed in run2 ? */
    static bool isUgly(const Jet* jet,const bool recalculateQuantities=false);


  private:
    /* @brief Compute the minimal number of objects containing at least threshold% of the jet energy  */
    static int compute_nLeading(std::vector<double> &cell_energies,  const float& e, const float& frac);




  };


  //// /////////////////////////////////////////////////////
  // Implement some of the task in JetCaloQualityUtils as JetCaloCalculator

  // JETCALCFUNCDECL is a declaration shortcut defined in JetCaloHelper.h
  
  using xAOD::JetAttribute;

  /* @brief Compute the minimal number of cells containing at least threshold% of the jet energy*/
  class JetCalcnLeadingCells: virtual public JetCaloCalculator {
  protected:
    std::vector<double> m_cell_energies;
    double m_sumE_cells;
    double m_threshold;

    JETCALCFUNCDECL( JetCalcnLeadingCells, JetAttribute::N90Constituents, m_threshold = 0.9;);
  };


  /* @brief Compute Out-Of-Time energy fraction */
  class JetCalcOutOfTimeEnergyFraction: virtual public JetCaloCalculator {
  protected:
    double m_sumE,m_sumE_OOT;
  public:
    double timecut;
    bool onlyPosEnergy;
    JETCALCFUNCDECL( JetCalcOutOfTimeEnergyFraction, JetAttribute::OotFracCells10, timecut = 10; onlyPosEnergy=false;m_sumE=0;);
  };

  /* @brief Compute the jet timing information from calorimeter*/

  class JetCalcTimeCells: virtual public JetCaloCalculator {
  protected:
    double m_time,m_norm;

    JETCALCFUNCDECL( JetCalcTimeCells, JetAttribute::Timing, m_norm=0; );

  };


  /* @brief Compute the jet average quality factor */
  class JetCalcAverageLArQualityF: virtual public JetCaloCalculator {
  protected:  
    double m_qf,m_norm;
    bool m_useTile;
    JETCALCFUNCDECL( JetCalcAverageLArQualityF, JetAttribute::AverageLArQF, m_useTile=false; m_norm=0;);

  };

  /* @brief Compute the jet quality factor information*/
  class JetCalcQuality: virtual public JetCaloCalculator {
  protected:
    double m_totE, m_badE;

  public:
    int LArQualityCut;
    int TileQualityCut; // =254
    bool includeTile, includeLAr;

    JETCALCFUNCDECL( JetCalcQuality, JetAttribute::LArQuality, includeTile = false;includeLAr=true; LArQualityCut=4000 ; TileQualityCut=254; m_totE=0; m_badE=0;);
  };


  /* @brief Compute the jet quality factor information for HEC only*/
  class JetCalcQualityHEC: virtual public JetCalcQuality {
  public:
  
    //ABC this class does not include JETCALCFUNCDECL ?
    bool processConstituent(xAOD::JetConstituentVector::iterator& iter);
    //bool processCell(const CaloCell *theCell, weight_t);
    JetCaloCalculator* clone() const {return new JetCalcQualityHEC(*this); }
  
    //bool processCell(const CaloCell *cell, weight_t w);
    JetCalcQualityHEC() :  JetCaloCalculator(JetAttribute::HECQuality), JetCalcQuality(JetAttribute::HECQuality) {  }

  };


  /* @brief total negative energy in a jet*/
  class JetCalcNegativeEnergy: virtual public JetCaloCalculator {
  protected:
    double m_totE;

    JETCALCFUNCDECL( JetCalcNegativeEnergy, JetAttribute::NegativeE, m_totE=0; );
  };

  /* @brief total negative energy in a jet*/
  class JetCalcCentroid: virtual public JetCaloCalculator {
  protected:
    double m_totE, m_centroid_x, m_centroid_y, m_centroid_z;


    JETCALCFUNCDECL( JetCalcCentroid, JetAttribute::CentroidR, m_totE=0; );
  };

  /// Fraction of Bad energy in jet. From cluster moment ENG_BAD_CELLS
  class JetCalcBadCellsFrac : virtual public JetCaloCalculator {
  protected:
    double m_totE;
    double m_badE;
    JETCALCFUNCDECL( JetCalcBadCellsFrac, JetAttribute::BchCorrCell, setName("BchCorrCell"););
    
  };

}
#endif
