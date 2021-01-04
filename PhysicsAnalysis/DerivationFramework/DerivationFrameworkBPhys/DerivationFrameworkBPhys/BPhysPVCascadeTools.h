/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_PVCASCADETOOLS_H
#define DERIVATIONFRAMEWORK_PVCASCADETOOLS_H

#include "GaudiKernel/ToolHandle.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include "DerivationFrameworkBPhys/CascadeTools.h"
#include "EventKernel/PdtPdg.h"

#include <vector>
// Authors: Adam Barton <abarton@SPAMMENOTTtttcern.ch>
//          Eva Bouhova <bouhova@SPAMMENOTTtttcern.ch>


//class CascadeTools;
namespace Trk {
  class V0Tools;
  class VxCascadeInfo;
}

namespace Analysis{
  class PrimaryVertexRefitter;
}
namespace InDet{
class BeamSpotData;
}

namespace HepPDT{
  class ParticleDataTable;
}

namespace DerivationFramework {
  
  class BPhysPVCascadeTools {
  typedef ElementLink<xAOD::VertexContainer> VertexLink;
  typedef std::vector<VertexLink> VertexLinkVector;
  private:
       const CascadeTools *m_cascadeTools;
       const InDet::BeamSpotData *m_beamSpotData;

       /// minimum number of tracks required in PVs considered
       size_t m_PV_minNTracks;
       
  public:
       bool m_copyAllVertices;
       BPhysPVCascadeTools(const CascadeTools *cascadeTools);
       BPhysPVCascadeTools(const CascadeTools *cascadeTools,
                           const InDet::BeamSpotData*);
         
       
       void ProcessVertex(const std::vector<TLorentzVector> &mom, Amg::MatrixX cov, xAOD::BPhysHypoHelper &vtx, xAOD::BPhysHelper::pv_type pvtype, double mass) const;
    
       static void FillBPhysHelperNULL(xAOD::BPhysHelper &vtx, const xAOD::VertexContainer* PvContainer,
           xAOD::BPhysHelper::pv_type pvtype);
       
       ///Fills the BPhysHelper object with the standard parameters
       void FillBPhysHelper(const std::vector<TLorentzVector> &mom, Amg::MatrixX cov, xAOD::BPhysHelper &vtx, const xAOD::Vertex* refPV,const xAOD::VertexContainer* refPvContainer,
                    xAOD::BPhysHelper::pv_type pvtype, int) const;
    
       ///Returns the index integer of the vertex with the lowest Z in relation to the given vertex
       size_t FindLowZIndex(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper &Obj,
			    const std::vector<const xAOD::Vertex*> &PVlist,
			    const size_t PV_minNTracks=0) const;
       ///Returns the index integer of the vertex with the lowest A0 in relation to the given vertex
       size_t FindLowA0Index(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper &Obj,
			     const std::vector<const xAOD::Vertex*> &PVlist,
			     const size_t PV_minNTracks=0) const;
       
       static size_t FindHighPtIndex(const std::vector<const xAOD::Vertex*> &PVlist);
       
       template< size_t NTracks> //NTracks = number of tracks in this type of vertex, if this is not known do not use this method
       static bool VerticesMatchTracks(const xAOD::Vertex* v1, const xAOD::Vertex* v2); 

       template< size_t NTracks>
       static const xAOD::Vertex* FindVertex(const xAOD::VertexContainer* c, const xAOD::Vertex* v); 

       /// Static method call with
       /// DerivationFramework::BPhysDerHelpers::GetGoodPV
       /// Returns a std::vector containing only PVs of type 1 and 3 - HighPt
       /// and Pileup, which have at least PV_minNTracks tracks.
       static std::vector<const xAOD::Vertex*> GetGoodPV(const xAOD::VertexContainer* pvContainer);
       
       /// Set the minimum number of tracks required for primary vertices to be
       /// considered for primary vertex association to a secondary vertex.
       /// Note that this requirement will not be applied for finding
       /// the vertex with the highest pT sum (FindHighPtIndex()) since
       /// it would possibly exclude this vertex which has been marked
       /// earlier in the tool chain.
       void SetMinNTracksInPV(size_t PV_minNTracks);

       /// Get the current beamspot position either from cache or from
       /// BeamCondSvc.
       /// Before processing a new event, make sure to call
       /// GetBeamSpot();
       [[nodiscard]] const Amg::Vector3D& GetBeamSpot() const noexcept;

       /// Find the index for the PV with the lowest distance in z of
       /// the SV's DOCA point w.r.t. the beamline and the PV.
       size_t FindLowZ0BAIndex(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper &obj,
			       const std::vector<const xAOD::Vertex*> &PVlist,
			       const size_t PV_minNTracks=0) const;
       /// Calculate the distance along z axis between the PV and
       ///  SV's DOCA point w.r.t. the beamline.
       double DistInZtoDOCA(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper &obj,
			    const xAOD::Vertex* vertex) const;
       /// Point of DOCA w.r.t. the beamline backward extrapolated
       /// along the B candidate's momentum direction. 
       Amg::Vector3D DocaExtrapToBeamSpot(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper &obj) const;

       static void PrepareVertexLinks(Trk::VxCascadeInfo *result,  const xAOD::TrackParticleContainer* importedTrackCollection);

       StatusCode FillCandwithRefittedVertices( bool refitPV,
					      const xAOD::VertexContainer* pvContainer, xAOD::VertexContainer* refPvContainer,
					      const Analysis::PrimaryVertexRefitter *pvRefitter, size_t in_PV_max, int DoVertexType,
                                              Trk::VxCascadeInfo* casc, int index,
                                              double mass, xAOD::BPhysHypoHelper &vtx);

       static std::vector<const xAOD::TrackParticle*> CollectAllChargedTracks(const std::vector<xAOD::Vertex*> &cascadeVertices);
       
       static void SetVectorInfo(xAOD::BPhysHelper &, const Trk::VxCascadeInfo*);
       static bool uniqueCollection(const std::vector<const xAOD::TrackParticle*>&);
       static bool uniqueCollection(const std::vector<const xAOD::TrackParticle*>&, const std::vector<const xAOD::TrackParticle*>&);
       static bool LinkVertices(SG::AuxElement::Decorator<VertexLinkVector> &decor, const std::vector<const xAOD::Vertex*>& vertices,
                                                 const xAOD::VertexContainer* vertexContainer, const xAOD::Vertex* vert);
       static double getParticleMass(const HepPDT::ParticleDataTable* pdt, int pdg);
  }; // class BPhysPVCascadeTools

} // namespace DerivationFramework


//added by ab
template< size_t NTracks>
bool DerivationFramework::BPhysPVCascadeTools::VerticesMatchTracks(const xAOD::Vertex* v1, const xAOD::Vertex* v2)
{
    if(v1->nTrackParticles() != v2->nTrackParticles()) return false;
    assert(v1->nTrackParticles() == NTracks);
    std::array<const xAOD::TrackParticle*, NTracks> a1;
    std::array<const xAOD::TrackParticle*, NTracks> a2;
    for(size_t i=0;i<NTracks;i++){
       a1[i] = v1->trackParticle(i);
       a2[i] = v2->trackParticle(i);
    }
    std::sort(a1.begin(), a1.end());
    std::sort(a2.begin(), a2.end());
    return a1 == a2;
}

template< size_t NTracks>
const xAOD::Vertex* DerivationFramework::BPhysPVCascadeTools::FindVertex(const xAOD::VertexContainer* c, const xAOD::Vertex* v){
   for (const xAOD::Vertex* a : *c){
      if(VerticesMatchTracks<NTracks>(a,v)) return a;
   }
   return nullptr;
}
#endif // DERIVATIONFRAMEWORK_PVCASCADETOOLS_H
