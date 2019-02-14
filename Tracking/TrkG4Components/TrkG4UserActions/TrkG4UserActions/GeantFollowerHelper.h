/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef GeantFollowerHelper_H
#define GeantFollowerHelper_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h" //typedef, can't fwd declare
#include "TrkG4UserActions/IGeantFollowerHelper.h"
#include "G4ThreeVector.hh" //typedef, can't fwd declare

// FIXME: header-global macro for an array size constant!
#ifndef MAXPROBES
#define MAXPROBES 50000
#endif

class TTree;

namespace Trk
{

  class IExtrapolator;

  class GeantFollowerHelper : public extends<AthAlgTool, IGeantFollowerHelper>
  {

    public:
      // constructor, destructor
      GeantFollowerHelper(const std::string&,const std::string&,const IInterface*);
      virtual ~GeantFollowerHelper ();

      // Athena hooks
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      // Follower interface
      // a) begin event - initialize follower process
      void beginEvent() const;
      // b) track the particle
      void trackParticle(const G4ThreeVector& pos, const G4ThreeVector& mom, int pdg, double charge, float t, float X0) const;
      // c) end event - ntuple writing
      void endEvent() const;

    private:

      ToolHandle<IExtrapolator>      m_extrapolator;
      bool                           m_extrapolateDirectly;
      bool                           m_extrapolateIncrementally;

      mutable const TrackParameters* m_parameterCache;
      mutable float                  m_tX0Cache;

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
      };
      std::unique_ptr<TreeData> m_treeData;
  };

}

#endif
