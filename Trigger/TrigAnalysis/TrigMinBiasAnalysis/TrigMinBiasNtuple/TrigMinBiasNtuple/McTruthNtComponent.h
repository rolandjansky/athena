/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_MCTRUTHNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_MCTRUTHNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include "HepPDT/ParticleDataTable.hh"
#include "HepMC/GenParticle.h"
#include <vector>

namespace NtComponent {
  class McTruthNtComponent: public NtupleComponent {
  public:
    McTruthNtComponent(NtupleAlgorithm *mainAlg,
		       NtComponentParameters parameters);
    virtual ~McTruthNtComponent();

    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:

    StatusCode findVertexIndex(std::map<HepMC::GenVertex *, long > *vertexIndexMap, HepMC::GenVertex *vtxPtr, long *index);

    /** The name of the truth collection container */
    std::string m_truthCollectionKey;

    /** Atlas particle ID table */
    const HepPDT::ParticleDataTable* m_particleDataTable;

    std::vector<double>* m_truth_evt_weights;

    int m_truth_npart; // Number of particles
    std::vector<float>* m_truth_part_px;
    std::vector<float>* m_truth_part_py;
    std::vector<float>* m_truth_part_pz;
    std::vector<float>* m_truth_part_e;
    std::vector<long>* m_truth_part_status;
    std::vector<long>* m_truth_part_pdgId;
    std::vector<long>* m_truth_part_barcode;
    std::vector<float>* m_truth_part_charge;
    std::vector<long>* m_truth_part_prod_vtx; // Index of the production vertex
    std::vector<long>* m_truth_part_end_vtx; // Index of the end vertex

    int m_truth_nvtx; // Number of vertices
    std::vector<float>* m_truth_vtx_x;
    std::vector<float>* m_truth_vtx_y;
    std::vector<float>* m_truth_vtx_z;
    std::vector<float>* m_truth_vtx_t;
    std::vector<long>* m_truth_vtx_id;
    std::vector<long>* m_truth_vtx_barcode;
  };
}

#endif
