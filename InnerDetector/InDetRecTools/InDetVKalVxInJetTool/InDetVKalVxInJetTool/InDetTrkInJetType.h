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
   The corresponding weights are returned as vector<float> with track type ordering.
   Multiclass TMVA is used for classification, then wgt[0]+wgt[1]+wgt[2]=1 always.
    
   The tool works (calibrated) for 35GeV<JetPt<3.5TeV.
   Jets above 3.5TeV and below 35GeV are considered as having 3.5TeV and 35GeV correspondingly.
   The tool is trained using ttbar+Z'(2.5,5TeV)+JZ4,6,8 + Gbb7000 samples
   The tool uses trkPt vs JetAxis (no any dR cone cut!) therefore the tool can be used for any jet with "reasonable" dR size.

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
namespace MVAUtils { class BDT; }
namespace Trk {  class TrkVKalVrtFitter; }

namespace InDet {

//------------------------------------------------------------------------
  class IInDetTrkInJetType : virtual public IAlgTool {
    public:
      DeclareInterfaceID( IInDetTrkInJetType, 1, 0 );
//---------------------------------------------------------------------------
//Interface itself

      virtual std::vector<float> trkTypeWgts( const xAOD::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &) const =0;

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

//------------------------------------------------------------------------------------------------------------------
// Private data and functions
//

   private:

    std::unique_ptr<MVAUtils::BDT> m_trkClassBDT;
    IChronoStatSvc* m_timingProfile{}; 
   
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
 };




}  //end namespace
#endif
