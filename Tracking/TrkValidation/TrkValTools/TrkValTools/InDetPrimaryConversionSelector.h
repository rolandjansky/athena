/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetPrimaryConversionSelector.h
//   Header file for InDetPrimaryConversionSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVALTOOLS_PrimaryConversionSelector_H
#define TRKVALTOOLS_PrimaryConversionSelector_H

#include "TrkValInterfaces/IGenParticleSelector.h"
#include "HepPDT/ParticleDataTable.hh"
#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {

/** @class InDetPrimaryConversionSelector
    @brief Modular concept to select truth particles for the truth tree filling
        and efficiency calculation, this one selects detectable tracks in the ID.

     This implementation of Trk::IGenParticleSelector selects generated
     particles which produce a detectable trace in the Inner Detector.
     It works by testing if production and end vertices are within Inner Detector
     acceptance.

    @author Till.Eifert -at- cern.ch
*/

  class InDetPrimaryConversionSelector : virtual public IGenParticleSelector, public AthAlgTool {
  public:

    InDetPrimaryConversionSelector(const std::string& type, const std::string& name,
                        const IInterface* parent);
    ~InDetPrimaryConversionSelector() {}
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    /** @brief main method performing the genparticle selection; it works on the entire
        collection. It is a factory. */
    virtual std::vector<HepMC::ConstGenParticlePtr>*
      selectGenSignal (const McEventCollection*) const;

  private:
    const HepPDT::ParticleDataTable* m_particleDataTable;
    float             m_minPt;
    float             m_maxEta;
    float             m_maxRStartAll; //!< Max R of start vertex for primaries and secondaries.
    float             m_maxZStartAll; //!< Max z of start vertex for primaries + sec.
  };

} // end namespace
#endif
