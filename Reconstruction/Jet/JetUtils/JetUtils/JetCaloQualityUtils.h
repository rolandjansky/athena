// emacs, this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  
     @class JetCaloQualityTool
     Calculates calorimeter based variables for jet quality 
     @author Nikola Makovec
     @author P-A Delsart
     @date (first implementation) December , 2009
     @date (re-write, run2) February , 2014
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

    /* @brief Compute Out-Of-Time energy fraction from cells*/
    static double jetOutOfTimeEnergyFraction(const Jet* jet, const double time=25, const bool doOnlyPos=false);

    /* @brief Compute Out-Of-Time energy fraction from clusters*/
    static double jetOutOfTimeEnergyFractionClusters(const Jet* jet=NULL, const double timecut=25, const bool doOnlyPos=false);

    /* @brief Compute the jet timing information using clusters*/
    static double jetTimeClusters(const Jet* jet);

    /* @brief Compute the jet timing information using cells*/
    static double jetTimeCells(const Jet* jet);

    /* @brief Compute the jet timing information - default to cells for compatibility*/
    static double jetTime(const Jet* jet);

    /* @brief Compute the jet average quality factor */
    static double jetAverageLArQualityF(const Jet* jet);

    /* @brief Compute the jet quality factor information*/
    static double jetQuality(const Jet* jet, int LArQualityCut=4000, int TileQualityCut=254);

    /* @brief Compute the jet quality factor information for LAr only*/
    static double jetQualityLAr(const Jet* jet, int LArQualityCut=4000);

    /* @brief Compute the jet quality factor information for Tile only*/
    static double jetQualityTile(const Jet* jet, int TileQualityCut=254);

    /* @brief Compute the jet quality factor information for HEC only*/
    static double jetQualityHEC(const Jet* jet, int LArQualityCut=4000, int TileQualityCut=254);

    /* @brief Compute the jet energy fraction in HEC*/
    static double hecF(const Jet* jet);

    /* @brief Compute the jet energy fraction in presampler*/
    static double presamplerFraction(const Jet* jet);

    /* @brief Compute the jet energy fraction in TILEGAP3*/
    static double tileGap3F(const Jet* jet);

    /* @brief total negative energy in a jet*/
    static double jetNegativeEnergy(const Jet* jet);

    /* @brief Check if it is a good jet with tight quality criteria (includes non-ugly)*/
    static bool isGoodTight(const Jet* jet,const bool recalculateQuantities=false);

    /* @brief Check if it is a bad jet with tight quality criteria*/
    static bool isBadTight(const Jet* jet,const bool recalculateQuantities=false);

    /* @brief Check if it is a good jet with tight quality criteria (includes non-ugly)*/
    static bool isGoodMedium(const Jet* jet,const bool recalculateQuantities=false);

    /* @brief Check if it is a bad jet with tight quality criteria*/
    /* @brief Check if it is a bad jet with medium quality criteria*/
    static bool isBadMedium(const Jet* jet,const bool recalculateQuantities=false);

    /* @brief Check if it is a good jet (includes non-ugly)*/
    static bool isGood(const Jet* jet,const bool recalculateQuantities=false);

    /* @brief Check if it is a bad jet*/
    static bool isBad(const Jet* jet,const bool recalculateQuantities=false);

    /* @brief Check if it is a ugly jet*/
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
  class JetCalcnLeadingCells: public JetCaloCalculator {

    std::vector<double> cell_energies;
    double sumE_cells;
    double threshold;

    JETCALCFUNCDECL( JetCalcnLeadingCells, JetAttribute::N90Constituents, threshold = 0.9;);
  };


  /* @brief Compute Out-Of-Time energy fraction */
  class JetCalcOutOfTimeEnergyFraction: public JetCaloCalculator {
    double sumE,sumE_OOT;
  public:
    double timecut;
    bool onlyPosEnergy;
    JETCALCFUNCDECL( JetCalcOutOfTimeEnergyFraction, JetAttribute::OotFracCells10, timecut = 10; onlyPosEnergy=false;sumE=0;);
  };

  /* @brief Compute the jet timing information using cells*/

  class JetCalcTimeCells: public JetCaloCalculator {
    double time,norm;

    JETCALCFUNCDECL( JetCalcTimeCells, JetAttribute::Timing, norm=0; );

  };


  /* @brief Compute the jet average quality factor */
  class JetCalcAverageLArQualityF: public JetCaloCalculator {
  
    double qf,norm;
    bool useTile;
    JETCALCFUNCDECL( JetCalcAverageLArQualityF, JetAttribute::AverageLArQF, useTile=false; norm=0;);

  };

  /* @brief Compute the jet quality factor information*/
  class JetCalcQuality: public JetCaloCalculator {
  protected:
    double totE, badE;

  public:
    int LArQualityCut;
    int TileQualityCut; // =254
    bool includeTile, includeLAr;

    JETCALCFUNCDECL( JetCalcQuality, JetAttribute::LArQuality, includeTile = false;includeLAr=true; LArQualityCut=4000 ; TileQualityCut=254; totE=0; badE=0;);
  };


  /* @brief Compute the jet quality factor information for HEC only*/
  class JetCalcQualityHEC: public JetCalcQuality {
  public:
  
    //ABC this class does not include JETCALCFUNCDECL ?
    bool processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t w);
    bool processCell(const CaloCell *theCell, weight_t);
  
    //bool processCell(const CaloCell *cell, weight_t w);
    JetCalcQualityHEC() : JetCalcQuality(JetAttribute::HECQuality) {  }

  };


  /* @brief total negative energy in a jet*/
  class JetCalcNegativeEnergy: public JetCaloCalculator {

    double totE;
    double totSig ; // cluster level calculation

    JETCALCFUNCDECL( JetCalcNegativeEnergy, JetAttribute::NegativeE, totE=0; totSig=0; );
  };

  /* @brief total negative energy in a jet*/
  class JetCalcCentroid: public JetCaloCalculator {

    double totE, centroid_x, centroid_y, centroid_z;


    JETCALCFUNCDECL( JetCalcCentroid, JetAttribute::CentroidR, totE=0; );
  };

}
#endif
