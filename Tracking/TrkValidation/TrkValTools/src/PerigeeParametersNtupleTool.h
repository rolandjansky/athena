/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// PerigeeParametersNtupleTool.h
//   Header file for PerigeeParametersNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////
#ifndef TRK_PERIGEEPARAMETERSNTUPLETOOL_H
#define TRK_PERIGEEPARAMETERSNTUPLETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkValInterfaces/ITrackValidationNtupleTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

  class IExtrapolator;

/** @class PerigeeParametersNtupleTool
    @brief Explain...

**/

class PerigeeParametersNtupleTool : virtual public Trk::ITrackValidationNtupleTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    PerigeeParametersNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~PerigeeParametersNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    /** @brief add branches to the tree
      Should be called once (per track collection and tree) dunring the initialisation phase by the
      calling algorithm (usually Trk::TrackValidationNtupleWriter) */
    virtual StatusCode addNtupleItems ( TTree* tree ) const;

    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackData (
        const Trk::Track&,
        const int iterationIndex,
        const Trk::FitterStatusCode fitStatCode ) const;

    /** fill ntuple data of a given TrackParticle without writing the record.
        - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackParticleData ( const Trk::TrackParticleBase& ) const;

    //! fill data about the truth match (score, parameter-pulls etc)
    virtual StatusCode fillTrackTruthData ( const TrackParameters*&,
                                            const TrackTruth&,
                                            const int) const;

    /** fill ntuple data of a given proto-trajectory (function used for fitter validation) */
    virtual StatusCode fillProtoTrajectoryData (
        const Trk::ProtoTrajectory&,
        const int iterationIndex,
        const Trk::Perigee*,
        const Trk::FitterStatusCode fitStatCode ) const;

    /** reset the variables after writing the record to disk ntuple */
    virtual void resetVariables( ) const;

private:

    // perigee filling common to Track and TrackParticle input
    StatusCode fillTrackPerigee(const Trk::Perigee* perigee) const;

    ToolHandle<Trk::IExtrapolator> m_extrapolator;  //!< extrapolator, in case tracks do not have perigee
    bool                m_doTruth;     //!< switch to turn truth on/off
    mutable float       m_Rec_d0;      //!< reconstructed track params: local d0 at perigee    UNIT:mm
    mutable float       m_Rec_z0;      //!< reconstructed track params: local z0 at perigee    UNIT:mm
    mutable float       m_Rec_phi0;    //!< reconstructed track params: angle phi at perigee   UNIT:rad
    mutable float       m_Rec_theta;   //!< reconstructed track params: angle theta at perigee UNIT:rad
    mutable float       m_Rec_eta;     //!< reconstructed track params: pseudorapidity         UNIT:1
    mutable float       m_Rec_qOverP;  //!< reconstructed track params: curvature q/p          UNIT:1/MeV

    mutable float       m_errord0;     //!< error on local d0 at perigee    UNIT:mm
    mutable float       m_errorz0;     //!< error on local z0 at perigee    UNIT:mm
    mutable float       m_errorphi0;   //!< error on angle phi at perigee   UNIT:rad
    mutable float       m_errortheta0; //!< error on angle theta at perigee UNIT:rad
    mutable float       m_errorqoverp; //!< error on curvature q/p          UNIT:1/MeV

    // Truth information
    mutable float       m_mc_d0;      //!< matched MC-truth track params: local d<sub>0</sub> at perigee UNIT:mm
    mutable float       m_mc_z0;      //!< matched MC-truth track params: local z<sub>0</sub> at perigee UNIT:mm
    mutable float       m_mc_phi0;    //!< matched MC-truth track params: angle phi at perigee UNIT:rad
    mutable float       m_mc_theta;   //!< matched MC-truth track params: angle theta at perigee UNIT:rad
    mutable float       m_mc_qOverP;  //!< matched MC-truth track params: curvature q/p at perigee UNIT:1/MeV
    mutable float       m_mc_qOverPt; //!< matched MC-truth track params: projected curvature q/pT at perigee UNIT:1/MeV
    mutable float       m_mc_eta;     //!< matched MC-truth track params: pseudorapidity

    mutable float       m_mc_diff_d0; //!< difference reconstructed minus true parameter: d<sub>0</sub> UNIT:mm
    mutable float       m_mc_diff_z0; //!< difference reconstructed minus true parameter: z<sub>0</sub> UNIT:mm
    mutable float       m_mc_diff_phi0; //!< difference reconstructed minus true parameter: angle phi UNIT:rad
    mutable float       m_mc_diff_theta; //!< difference reconstructed minus true parameter: angle theta UNIT:rad
    mutable float       m_mc_diff_qOverP; //!< difference reconstructed minus true parameter: q/p UNIT:1/MeV
    
    mutable float       m_mc_pull_d0;    //!< parameter-pull for d<sub>0</sub> UNIT:1
    mutable float       m_mc_pull_z0;    //!< parameter-pull for z<sub>0</sub> UNIT:1
    mutable float       m_mc_pull_phi0;  //!< parameter-pull for phi           UNIT:1
    mutable float       m_mc_pull_theta; //!< parameter-pull for theta         UNIT:1
    mutable float       m_mc_pull_qOverP;//!< parameter-pull for q/p           UNIT:1

    mutable int         m_mc_particleID; //!< PDG-ID for matched truth track
    mutable int         m_mc_barcode;    //!< barcode for matched truth track
    mutable int         m_mc_truthTreeIndex; //!< entry index linking to 'Truth' tree in ntuple
    mutable float       m_mc_energy;     //!< energy of the truth particle     UNIT:MeV

    mutable float       m_mc_prob;       //!< match probability for the truth particle (calculated by the job's configured TrackTruthSelector)

};


} // end of namespace


#endif // TRK_PERIGEEPARAMETERSNTUPLETOOL_H
