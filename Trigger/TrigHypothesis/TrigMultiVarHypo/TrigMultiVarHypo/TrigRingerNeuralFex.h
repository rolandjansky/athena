/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigRingerNeuralFex.h
 * @author Danilo Enoque Ferreira de Lima 
 * $Author: dferreir $
 * $Date: 2009-01-12 17:51:36 $
 *
 * @brief Describes a ring neural processor
 */

#ifndef TRIGRINGERNEURALFEX_H
#define TRIGRINGERNEURALFEX_H

#include <string>

#include "TrigInterfaces/FexAlgo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "CaloEvent/CaloSampling.h"
#include "TrigMultiVarHypo/Neural.h"
#include "TrigTimeAlgs/TrigTimer.h"


typedef double Feature;
typedef std::vector<Feature> Pattern;

class TrigRingerNeuralFex: public HLT::FexAlgo {
  
 public:

  TrigRingerNeuralFex(const std::string & name, ISvcLocator* pSvcLocator);
  virtual ~TrigRingerNeuralFex(){};
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  const static double ENERGY_THRESHOLD; // Defines the energy threshold for divisions, in MeV's

 protected:
  TrigTimer* m_storeTimer;
  TrigTimer *m_decisionTimer;   ///< For timing
  TrigTimer *m_normalizationTimer;


  // Helper class used to build ringsets
  class RingSet {
    
  public: //interface
    
    enum Normalisation {
      EVENT = 0, ///< normalises the ring energies with the total event energy
      SECTION = 1, ///< normalises the ring energies with the section energy
      SET = 2, ///< normalises the ring energies with the set energy
      SEQUENTIAL = 3, ///< normalises the ring energies with a special scheme
      NONE = 4 ///< no normalisation is applied
    };
    
    typedef enum Normalisation Normalisation;

    enum Section {
      EM = 0, ///< electromagnetic section of the ATLAS calorimetry
      HADRONIC = 1 ///< hadronic section of the ATLAS calorimetry
    };

    /**
     * Builds a new RingSet, starting from a ring definition. The RingSet
     * starts empty.
     *
     */
    RingSet (unsigned int max, const Normalisation &norm, const Section &section);
    
    /**
     * Virtualises the destructor
     */
    virtual ~RingSet();
    
    inline size_t max(void) const { return m_max; }
    inline Normalisation normalisation(void) const { return m_norm; }
    inline Section section(void) const { return m_section; }
    
    /**
     * Returns the (current) ring values.
     */
    inline Pattern& pattern (void) { return m_val; }
    
    /**
     * Resets all current values
     */
    inline void reset (void) { m_val.clear(); }

    /**
     * Appends ring to ringset
     */
    inline void add ( double ring) { m_val.push_back(ring); }

    
  private: //representation
    
    size_t m_max;
    Normalisation m_norm; ///< the normalisation scheme for this ring set
    Section m_section; ///< the section this ring set belongs to
    
    Pattern m_val; ///< my current values
  };

 private:

  std::string m_hlt_feature;
  std::string m_feature;
  std::string m_key;
  Neural *m_network;
  unsigned int m_maxRingsAccumulated;


  /**
   * This function retrieves the associated TrigEMCluster object for this TE.
   *
   * @param ote Output trigger element that initiated the execution.
   * @return The TrigEMCluster retrieved from Store Gate for the specified TriggerElement.
   */
  const TrigEMCluster* get_cluster (const HLT::TriggerElement* ote);

  // Properties:
  std::vector<unsigned> m_nodes;
  std::vector<float> m_bias;
  std::vector<float> m_weight;

  // RingSet properties:
  std::vector<unsigned int> m_nRings;
  std::vector<unsigned int> m_normRings;
  std::vector<unsigned int> m_sectionRings;

  /**
   * Calculates and applies the sequential normalisation vector that results
   * from the following arithmetic:
   *
   * @f[
   * N0 = E
   * N1 = E - E0
   * N2 = E - E0 - E1 = N1 - E1
   * N3 = E - E0 - E1 - E2 = N2 - E2
   * ...
   * NX = N(X-1) - E(X-1)
   * @f]
   *
   * Where Ni are the normalisation factors, E is the total layer energy and Ei
   * are the energy values for each ring. 
   *
   * @param rings The calculated ring values one wishes to normalise
   * @param stop_energy The threshold to judge when to stop this normalisation
   * strategy and start using the total layer energy instead, in MeV.
   */
  void sequential (Pattern& rings, const Feature& stop_energy = 100.0);

  /**
   * Apply normalization based on the ring set configuration
   *
   * @param rset The ring set configuration to use for creating the rings
   */
  void normalize_rings(std::vector<RingSet>&  rset);

  std::vector<RingSet>  m_ringsSet;           // It will containg the raw rings generated and their configuration.
  RingerRings m_rings;

};

#endif /* TRIGRINGERNEURALFEX_H */
