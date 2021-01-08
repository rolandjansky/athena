/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetHaloSelector.h
//   Header file for InDetHaloSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVALTOOLS_HALOSELECTOR_H
#define TRKVALTOOLS_HALOSELECTOR_H

#include "TrkValInterfaces/IGenParticleSelector.h"
#include "HepPDT/ParticleDataTable.hh"
#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {

/** @class InDetHaloSelector
    select truth halo tracks
*/

  class InDetHaloSelector : virtual public IGenParticleSelector, public AthAlgTool {
  public:

    InDetHaloSelector(const std::string& type, const std::string& name,
                        const IInterface* parent);
    ~InDetHaloSelector() {}
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    /** @brief main method performing the genparticle selection; it works on the entire
        collection. It is a factory. */
    virtual std::vector<HepMC::ConstGenParticlePtr>*
      selectGenSignal (const McEventCollection*) const;

  private:
    const HepPDT::ParticleDataTable* m_particleDataTable;

  };

} // end namespace
#endif
