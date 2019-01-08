/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PREPROC_TRIGRINGERPREPROC_H
#define PREPROC_TRIGRINGERPREPROC_H

///std librarys
#include <vector>
///Local includes
#include "TrigMultiVarHypo/tools/TrigRingerHelper.h"



class TrigRingerPreprocessor{
  
  private:

    // RingSet properties:
    std::vector<unsigned int> m_nRings;
    std::vector<unsigned int> m_normRings;
    std::vector<unsigned int> m_sectionRings;
    double                    m_energyThreshold;///Defines the energy threshlod for divisions, in MeV's
    /// RingSet object
    std::vector<TrigCaloRingsHelper::RingSet*>     m_ringsSet;
    unsigned int              m_maxRingsAccumulated;
  
  public:

    TrigRingerPreprocessor(std::vector<unsigned int> &nRings, 
                           std::vector<unsigned int> &normRings,
                           std::vector<unsigned int> &sectionRings);
  
    ~TrigRingerPreprocessor();

    bool ppExecute( Pattern &rings );

    void setEnergyThreshold( double e ){ m_energyThreshold = e;};
    
    double getEnergyThreshold(){ return m_energyThreshold;};

    float normalize_eta( float, float etamin, float etamax );

    float normalize_mu( float, float mumax );

    /*
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
    void normalize_rings(std::vector<TrigCaloRingsHelper::RingSet*>&  rset);

};
#endif /* TrigRingerPreproc */

