/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerMSHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef GeantFollowerMSHelper_H
#define GeantFollowerMSHelper_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkG4UserActions/IGeantFollowerMSHelper.h"
#include "G4ThreeVector.hh"

#ifndef MAXPROBES
#define MAXPROBES 50000
#endif

class TTree;

namespace Trk {

  class IExtrapolator;
  class IEnergyLossUpdator;

  class Track;
  class MeasurementBase;
  class MaterialEffectsOnTrack;
  class TrackStateOnSurface;
  
  class GeantFollowerMSHelper : public AthAlgTool, virtual public IGeantFollowerMSHelper {

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
      // modify TSOS vector 
      const std::vector<const Trk::TrackStateOnSurface*> modifyTSOSvector(const std::vector<const Trk::TrackStateOnSurface*> matvec, double scaleX0, double scaleEloss, bool reposition, bool aggregate, bool updateEloss, double caloEnergy, double caloEnergyError, double pCaloEntry, double momentumError, double & Eloss_tot) const;

      // c) end event - ntuple writing      
      void endEvent() const;

   private:
       
        ToolHandle<IExtrapolator>      m_extrapolator;
        ToolHandle<IEnergyLossUpdator> m_elossupdator;

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
        mutable float                  m_t_x;
        mutable float                  m_t_y;
        mutable float                  m_t_z;
        mutable float                  m_t_theta;
        mutable float                  m_t_eta;
        mutable float                  m_t_phi;
        mutable float                  m_t_p;
        mutable float                  m_t_charge;
        mutable int                    m_t_pdg;
        mutable float                  m_m_x;
        mutable float                  m_m_y;
        mutable float                  m_m_z;
        mutable float                  m_m_theta;
        mutable float                  m_m_eta;
        mutable float                  m_m_phi;
        mutable float                  m_m_p;
        mutable float                  m_b_x;
        mutable float                  m_b_y;
        mutable float                  m_b_z;
        mutable float                  m_b_theta;
        mutable float                  m_b_eta;
        mutable float                  m_b_phi;
        mutable float                  m_b_p;
        mutable float                  m_b_X0;
        mutable float                  m_b_Eloss;
        /** Ntuple variables : g4 step parameters */
        mutable int                    m_g4_steps;
	mutable float                  m_g4_p[MAXPROBES];
        mutable float                  m_g4_eta[MAXPROBES];
        mutable float                  m_g4_theta[MAXPROBES];
        mutable float                  m_g4_phi[MAXPROBES];
        mutable float                  m_g4_x[MAXPROBES];
        mutable float                  m_g4_y[MAXPROBES];
        mutable float                  m_g4_z[MAXPROBES];
        mutable float                  m_g4_tX0[MAXPROBES];
        mutable float                  m_g4_t[MAXPROBES];
        mutable float                  m_g4_X0[MAXPROBES];
        /** Ntuple variables : trk follow up parameters */
        mutable int                    m_trk_status[MAXPROBES];
        mutable float                  m_trk_p[MAXPROBES];
        mutable float                  m_trk_eta[MAXPROBES];
        mutable float                  m_trk_theta[MAXPROBES];
        mutable float                  m_trk_phi[MAXPROBES];
        mutable float                  m_trk_x[MAXPROBES];
        mutable float                  m_trk_y[MAXPROBES];
        mutable float                  m_trk_z[MAXPROBES];   
        mutable float                  m_trk_lx[MAXPROBES];
        mutable float                  m_trk_ly[MAXPROBES];
        mutable float                  m_trk_eloss[MAXPROBES];
        mutable float                  m_trk_eloss1[MAXPROBES];
        mutable float                  m_trk_eloss0[MAXPROBES];
        mutable float                  m_trk_eloss5[MAXPROBES];
        mutable float                  m_trk_eloss10[MAXPROBES];
        mutable float                  m_trk_scaleeloss[MAXPROBES];
        mutable float                  m_trk_scalex0[MAXPROBES];
        mutable float                  m_trk_x0[MAXPROBES];
        mutable float                  m_trk_erd0[MAXPROBES];
        mutable float                  m_trk_erz0[MAXPROBES];
        mutable float                  m_trk_erphi[MAXPROBES];
        mutable float                  m_trk_ertheta[MAXPROBES];
        mutable float                  m_trk_erqoverp[MAXPROBES];
        /** Scattering centra from Trk */
        mutable int                    m_trk_scats;
        mutable int                    m_trk_sstatus[500];
        mutable float                  m_trk_sx[500];
        mutable float                  m_trk_sy[500];
        mutable float                  m_trk_sz[500];
        mutable float                  m_trk_sx0[500];
        mutable float                  m_trk_seloss[500];
        mutable float                  m_trk_smeanIoni[500];
        mutable float                  m_trk_ssigIoni[500];
        mutable float                  m_trk_smeanRad[500];
        mutable float                  m_trk_ssigRad[500];
        mutable float                  m_trk_ssigTheta[500];
        mutable float                  m_trk_ssigPhi[500];
        mutable int                    m_g4_stepsMS;

 
  };
    
}

#endif
