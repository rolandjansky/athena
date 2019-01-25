/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerMSHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef GeantFollowerMSHelper_H
#define GeantFollowerMSHelper_H

#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkG4UserActions/IGeantFollowerMSHelper.h"
#include "G4ThreeVector.hh"

// FIXME: header-global macro for an array size constant!
#ifndef MAXPROBES
#define MAXPROBES 50000
#endif

class TTree;

namespace Trk
{

  class IExtrapolator;

  class Track;
  class MeasurementBase;
  class MaterialEffectsOnTrack;
  class TrackStateOnSurface;

  class GeantFollowerMSHelper : public extends<AthAlgTool, IGeantFollowerMSHelper>
  {

    public:
      // constructor, destructor
      GeantFollowerMSHelper(const std::string&,const std::string&,const IInterface*);
      virtual ~GeantFollowerMSHelper ();

      // Athena hooks
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      // Follower interface
      // a) begin event - initialize follower process
      void beginEvent() const;
      // b) track the particle
      void trackParticle(const G4ThreeVector& pos, const G4ThreeVector& mom, int pdg, double charge, float t, float X0) const;

      /// modify TSOS vector
      /// FIXME: const by-value vector argument!
      const std::vector<const Trk::TrackStateOnSurface*>
      modifyTSOSvector(const std::vector<const Trk::TrackStateOnSurface*> matvec,
                       double scaleX0, double scaleEloss, bool reposition,
                       bool aggregate, bool updateEloss, double caloEnergy,
                       double caloEnergyError, double pCaloEntry,
                       double momentumError, double & Eloss_tot) const;

      // c) end event - ntuple writing
      void endEvent() const;

    private:

      ToolHandle<IExtrapolator>      m_extrapolator;
        PublicToolHandle<IEnergyLossUpdator> m_elossupdator
           {this,"EnergyLossUpdator","Trk::EnergyLossUpdator/AtlasEnergyLossUpdator",""};

      bool                           m_extrapolateDirectly;
      bool                           m_extrapolateIncrementally;
      bool                           m_speedup;

      mutable const TrackParameters* m_parameterCache;
      mutable const TrackParameters* m_parameterCacheCov;
      mutable const TrackParameters* m_parameterCacheMS;
      mutable const TrackParameters* m_parameterCacheMSCov;
      mutable float                  m_tX0Cache;

      mutable bool                   m_crossedMuonEntry;
      mutable bool                   m_exitLayer;
      mutable const PlaneSurface*    m_destinationSurface;

      // put some validation code is
      std::string                    m_validationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string                    m_validationTreeDescription; //!< validation tree description - second argument in TTree
      std::string                    m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out

      TTree*                         m_validationTree;            //!< Root Validation Tree
      /** Ntuple variables : initial parameters
          Split this out into a separate, dynamically-allocated block.
          Otherwise, the CaloCellNoiseAlg is so large that it violates
          the ubsan sanity checks. **/
      struct TreeData {
          mutable float                  m_t_x {0};
          mutable float                  m_t_y {0};
          mutable float                  m_t_z {0};
          mutable float                  m_t_theta {0};
          mutable float                  m_t_eta {0};
          mutable float                  m_t_phi {0};
          mutable float                  m_t_p {0};
          mutable float                  m_t_charge {0};
          mutable int                    m_t_pdg {0};
          mutable float                  m_m_x {0};
          mutable float                  m_m_y {0};
          mutable float                  m_m_z {0};
          mutable float                  m_m_theta {0};
          mutable float                  m_m_eta {0};
          mutable float                  m_m_phi {0};
          mutable float                  m_m_p {0};
          mutable float                  m_b_x {0};
          mutable float                  m_b_y {0};
          mutable float                  m_b_z {0};
          mutable float                  m_b_theta {0};
          mutable float                  m_b_eta {0};
          mutable float                  m_b_phi {0};
          mutable float                  m_b_p {0};
          mutable float                  m_b_X0 {0};
          mutable float                  m_b_Eloss {0};
          /** Ntuple variables : g4 step parameters */
          mutable int                    m_g4_steps {0};
          mutable float                  m_g4_p[MAXPROBES] {0};
          mutable float                  m_g4_eta[MAXPROBES] {0};
          mutable float                  m_g4_theta[MAXPROBES] {0};
          mutable float                  m_g4_phi[MAXPROBES] {0};
          mutable float                  m_g4_x[MAXPROBES] {0};
          mutable float                  m_g4_y[MAXPROBES] {0};
          mutable float                  m_g4_z[MAXPROBES] {0};
          mutable float                  m_g4_tX0[MAXPROBES] {0};
          mutable float                  m_g4_t[MAXPROBES] {0};
          mutable float                  m_g4_X0[MAXPROBES] {0};
          /** Ntuple variables : trk follow up parameters */
          mutable int                    m_trk_status[MAXPROBES] {0};
          mutable float                  m_trk_p[MAXPROBES] {0};
          mutable float                  m_trk_eta[MAXPROBES] {0};
          mutable float                  m_trk_theta[MAXPROBES] {0};
          mutable float                  m_trk_phi[MAXPROBES] {0};
          mutable float                  m_trk_x[MAXPROBES] {0};
          mutable float                  m_trk_y[MAXPROBES] {0};
          mutable float                  m_trk_z[MAXPROBES] {0};
          mutable float                  m_trk_lx[MAXPROBES] {0};
          mutable float                  m_trk_ly[MAXPROBES] {0};
          mutable float                  m_trk_eloss[MAXPROBES] {0};
          mutable float                  m_trk_eloss1[MAXPROBES] {0};
          mutable float                  m_trk_eloss0[MAXPROBES] {0};
          mutable float                  m_trk_eloss5[MAXPROBES] {0};
          mutable float                  m_trk_eloss10[MAXPROBES] {0};
          mutable float                  m_trk_scaleeloss[MAXPROBES] {0};
          mutable float                  m_trk_scalex0[MAXPROBES] {0};
          mutable float                  m_trk_x0[MAXPROBES] {0};
          mutable float                  m_trk_erd0[MAXPROBES] {0};
          mutable float                  m_trk_erz0[MAXPROBES] {0};
          mutable float                  m_trk_erphi[MAXPROBES] {0};
          mutable float                  m_trk_ertheta[MAXPROBES] {0};
          mutable float                  m_trk_erqoverp[MAXPROBES] {0};
          /** Scattering centra from Trk */
          mutable int                    m_trk_scats {0};
          mutable int                    m_trk_sstatus[500] {0};
          mutable float                  m_trk_sx[500] {0};
          mutable float                  m_trk_sy[500] {0};
          mutable float                  m_trk_sz[500] {0};
          mutable float                  m_trk_sx0[500] {0};
          mutable float                  m_trk_seloss[500] {0};
          mutable float                  m_trk_smeanIoni[500] {0};
          mutable float                  m_trk_ssigIoni[500] {0};
          mutable float                  m_trk_smeanRad[500] {0};
          mutable float                  m_trk_ssigRad[500] {0};
          mutable float                  m_trk_ssigTheta[500] {0};
          mutable float                  m_trk_ssigPhi[500] {0};
          mutable int                    m_g4_stepsMS {0};
      };
      std::unique_ptr<TreeData> m_treeData;
  };

}

#endif
