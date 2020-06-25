/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
//

class TLorentzVector;
class IChronoStatSvc;
namespace Rec{ class TrackParticle; }
namespace MVAUtils { class BDT; }
namespace Trk {  class TrkVKalVrtFitter; }
namespace TMVA { class Reader; }

namespace InDet {

//------------------------------------------------------------------------
  class IInDetTrkInJetType : virtual public IAlgTool {
    public:
      DeclareInterfaceID( IInDetTrkInJetType, 1, 0 );
//---------------------------------------------------------------------------
//Interface itself

      virtual std::vector<float> trkTypeWgts( const xAOD::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &) const =0;
      virtual std::vector<float> trkTypeWgts( const Rec::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &) const =0;

  };




  class InDetTrkInJetType : public extends<AthAlgTool, IInDetTrkInJetType>
  {

   public:
       /* Constructor */
      InDetTrkInJetType(const std::string& type, const std::string& name, const IInterface* parent);
       /* Destructor */
      virtual ~InDetTrkInJetType();


      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;

      virtual std::vector<float> trkTypeWgts(const xAOD::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &) const override;
      virtual std::vector<float> trkTypeWgts(const Rec::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &) const override;

//------------------------------------------------------------------------------------------------------------------
// Private data and functions
//

   private:

    TMVA::Reader* m_tmvaReader{};
    MVAUtils::BDT* m_localBDT{};
    IChronoStatSvc * m_timingProfile{}; 
   
    int m_trkSctHitsCut{};
    int m_trkPixelHitsCut{};
    float m_trkChi2Cut{};
    float m_trkMinPtCut{};
    float m_jetMaxPtCut{};
    float m_jetMinPtCut{};
    float m_d0_limLow{};
    float m_d0_limUpp{};
    float m_Z0_limLow{};
    float m_Z0_limUpp{};
    std::string m_calibFileName;
    ToolHandle < Trk::IVertexFitter >  m_fitterSvc
       {this, "VertexFitterTool", "Trk::TrkVKalVrtFitter/VertexFitterTool",""};
    Trk::TrkVKalVrtFitter*   m_fitSvc{};

    int m_initialised{};

    float m_Sig3D{};
    float m_prbP{};
    float m_d0{};
    float m_pTvsJet{};
    float m_SigZ{};
    float m_SigR{};
    float m_ptjet{};
    float m_etatrk{};
    float m_ibl{};
    float m_bl{};
 };




}  //end namespace
#endif
