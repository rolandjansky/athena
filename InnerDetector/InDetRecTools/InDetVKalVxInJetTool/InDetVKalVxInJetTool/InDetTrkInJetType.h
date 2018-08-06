/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
// InDetTrkInJetType.h - Description
//
/*
   Tool to classify track origins in a jet.
   Track types:
    0 -  Heavy Flavour         (Signal)
    1 -  Fragmentation tracks  (Fragment)
    2 -  Garbage    (Interactions+V0s+Pileup)
    
   The tool works for JetPt<2.5TeV.
   Tool is trained using ttbar+Z'(1.5,3,5TeV)+JZ4,5,6 samples

    Author: Vadim Kostyukhin
    e-mail: vadim.kostyukhin@cern.ch
*/
#ifndef InDet_InDetTrkInJetType_H
#define InDet_InDetTrkInJetType_H

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TLorentzVector.h"
#include "xAODTracking/TrackParticleContainer.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include  "Particle/TrackParticle.h"
//

namespace TMVA { class Reader; }

namespace InDet {

//------------------------------------------------------------------------
  static const InterfaceID IID_IInDetTrkInJetType("IInDetTrkInJetType", 1, 0);

  class IInDetTrkInJetType : virtual public IAlgTool {
    public:
      static const InterfaceID& interfaceID() { return IID_IInDetTrkInJetType;}
//---------------------------------------------------------------------------
//Interface itself

      virtual std::vector<float> trkTypeWgts( const xAOD::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &) =0;
      virtual std::vector<float> trkTypeWgts( const Rec::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &) =0;

  };




  class InDetTrkInJetType : public AthAlgTool, virtual public IInDetTrkInJetType
  {

   public:
       /* Constructor */
      InDetTrkInJetType(const std::string& type, const std::string& name, const IInterface* parent);
       /* Destructor */
      virtual ~InDetTrkInJetType();


      StatusCode initialize();
      StatusCode finalize();

      std::vector<float> trkTypeWgts(const xAOD::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &);
      std::vector<float> trkTypeWgts(const Rec::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &);

//------------------------------------------------------------------------------------------------------------------
// Private data and functions
//

   private:

    TMVA::Reader* m_tmvaReader;
    double m_trkMinPtCut;
    float m_d0_limLow;
    float m_d0_limUpp;
    float m_Z0_limLow;
    float m_Z0_limUpp;
    std::string m_calibFileName;
    ToolHandle < Trk::IVertexFitter >  m_fitterSvc;
    Trk::TrkVKalVrtFitter*   m_fitSvc;

    int m_initialised;

    float m_prbS;
    float m_Sig3D;
    float m_prbP;
    float m_d0;
    float m_vChi2;
    float m_pTvsJet;
    float m_prodTJ;
    float m_SigZ;
    float m_SigR;
    float m_ptjet;
    float m_etajet;
    float   m_ibl;
    float   m_bl;
 };




}  //end namespace
#endif
