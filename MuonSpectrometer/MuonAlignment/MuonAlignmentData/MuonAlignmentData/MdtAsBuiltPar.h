/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_MDTASBUILTPAR_H
#define MUONALIGNMENTDATA_MDTASBUILTPAR_H

#include <string>

#include "MuonAlignmentData/MuonAlignmentPar.h"

/**
 * Container classfor the MDT as-built parameters
 * See parameter description in
 * http://atlas-muon-align.web.cern.ch/atlas-muon-align/endplug/asbuilt.pdf
 */
class MdtAsBuiltPar : public MuonAlignmentPar {
public:
    // Default constructor
    MdtAsBuiltPar();

    // destructor
    virtual ~MdtAsBuiltPar() override = default;

    /**
     * MDT multi-layer index
     */
    enum multilayer_t { ML1 = 0, ML2, NMLTYPES };

    /**
     * MDT tube side
     */
    enum tubeSide_t {
        POS = 0,  // s>0 side
        NEG,      // s<0 side
        NTUBESIDETYPES
    };

    /**
     * Set the alignment parameters for a ML and a tube side
     */
    void setAlignmentParameters(multilayer_t iML, tubeSide_t iTubeSide, float y0, float z0, float alpha, float ypitch, float zpitch,
                                int stagg);

    /**
     * Initialize from a line of an ASCII CLOB, return true on success
     */
    bool setFromAscii(const std::string& line);

    // Getters
    double y0(multilayer_t iML, tubeSide_t iTubeSide) const { return meas(iML, iTubeSide).y0; }
    double z0(multilayer_t iML, tubeSide_t iTubeSide) const { return meas(iML, iTubeSide).z0; }
    double alpha(multilayer_t iML, tubeSide_t iTubeSide) const { return meas(iML, iTubeSide).alpha; }
    double ypitch(multilayer_t iML, tubeSide_t iTubeSide) const { return meas(iML, iTubeSide).ypitch; }
    double zpitch(multilayer_t iML, tubeSide_t iTubeSide) const { return meas(iML, iTubeSide).zpitch; }
    int stagg(multilayer_t iML, tubeSide_t iTubeSide) const { return meas(iML, iTubeSide).stagg; }

private:
    // Alignment parameters
    struct AlignmentParameters {
        AlignmentParameters();
        multilayer_t iML;      // ML index
        tubeSide_t iTubeSide;  // tube side
        float y0;              // y position of first wire w.r.t. reference point
        float z0;              // z position of first wire w.r.t. reference point
        float alpha;           // rotation around tube axis
        float ypitch;          // y pitch
        float zpitch;          // z pitch
        int stagg;             // is tube staggering ATLAS-standard or reversed?
    };
    static const int NMEAS = NMLTYPES * NTUBESIDETYPES;
    AlignmentParameters m_meas[NMEAS];  // in this order: ML1_HV, ML1_RO, ML2_HV, ML2_RO

    AlignmentParameters& meas(multilayer_t iML, tubeSide_t iTubeSide) { return m_meas[NTUBESIDETYPES * iML + iTubeSide]; }
    const AlignmentParameters& meas(multilayer_t iML, tubeSide_t iTubeSide) const { return m_meas[NTUBESIDETYPES * iML + iTubeSide]; }
};
#endif  // MUONALIGNMENTDATA_MDTASBUILTPAR_H
