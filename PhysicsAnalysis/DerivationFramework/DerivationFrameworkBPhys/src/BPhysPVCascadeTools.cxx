/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkBPhys/BPhysPVCascadeTools.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBPhys/BPhysHelper.h"
#include "TVector3.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"
#include "TrkVKalVrtFitter/VxCascadeInfo.h"
#include "DerivationFrameworkBPhys/LocalVector.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "HepPDT/ParticleDataTable.hh"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include <limits>
#include <iostream>

DerivationFramework::BPhysPVCascadeTools::BPhysPVCascadeTools(const CascadeTools *cascadeTools) :
  m_cascadeTools(cascadeTools), m_beamSpotData(nullptr), m_PV_minNTracks(0),
  m_copyAllVertices(true)
{
}

DerivationFramework::BPhysPVCascadeTools::BPhysPVCascadeTools(const CascadeTools *cascadeTools,
                                                const InDet::BeamSpotData* beamSpotSvc) :
  m_cascadeTools(cascadeTools), m_beamSpotData(beamSpotSvc), m_PV_minNTracks(0),
  m_copyAllVertices(true)
{
}

void DerivationFramework::BPhysPVCascadeTools::FillBPhysHelper(const std::vector<TLorentzVector> &mom, Amg::MatrixX cov, xAOD::BPhysHelper &vtx,
							const xAOD::Vertex* PV, const xAOD::VertexContainer* PvContainer,
							xAOD::BPhysHelper::pv_type pvtype, int refitCode) const {

  BPHYS_CHECK( vtx.setPv      ( PV, PvContainer, pvtype ) );

  // cout << "BPhysPVCascadeTools::FillBPhysHelper for pvtype = " << pvtype << endl;
  // cout << "lxy " << m_cascadeTools->lxy(mom, vtx.vtx(), PV) << " error " << m_cascadeTools->lxyError(mom, cov, vtx.vtx(), PV) << endl;
 
  // set variables calculated from PV
  BPHYS_CHECK( vtx.setLxy    ( m_cascadeTools->lxy       (mom, vtx.vtx(), PV), pvtype ) );
  BPHYS_CHECK( vtx.setLxyErr ( m_cascadeTools->lxyError  (mom, cov, vtx.vtx(), PV), pvtype ) );
  BPHYS_CHECK( vtx.setA0     ( m_cascadeTools->a0        (mom, vtx.vtx(), PV), pvtype ) );
  BPHYS_CHECK( vtx.setA0Err  ( m_cascadeTools->a0Error   (mom, cov, vtx.vtx(), PV), pvtype ) );
  BPHYS_CHECK( vtx.setA0xy   ( m_cascadeTools->a0xy      (mom, vtx.vtx(), PV), pvtype ) );
  BPHYS_CHECK( vtx.setA0xyErr( m_cascadeTools->a0xyError (mom, cov, vtx.vtx(), PV), pvtype ) );
  BPHYS_CHECK( vtx.setZ0     ( m_cascadeTools->a0z       (mom, vtx.vtx(), PV), pvtype ) );
  BPHYS_CHECK( vtx.setZ0Err  ( m_cascadeTools->a0zError  (mom, cov, vtx.vtx(), PV), pvtype ) );
  BPHYS_CHECK( vtx.setRefitPVStatus  ( refitCode, pvtype ) );

}

void DerivationFramework::BPhysPVCascadeTools::ProcessVertex(const std::vector<TLorentzVector> &mom, Amg::MatrixX cov, xAOD::BPhysHypoHelper &vtx,
							xAOD::BPhysHelper::pv_type pvtype, double mass) const {

  const xAOD::Vertex* pv = vtx.pv(pvtype);
  if (pv) {
    // decorate the vertex.
    vtx.setTau( m_cascadeTools->tau(mom, vtx.vtx(), pv), pvtype, xAOD::BPhysHypoHelper::TAU_INV_MASS );
    vtx.setTauErr( m_cascadeTools->tauError(mom, cov, vtx.vtx(), pv), pvtype, xAOD::BPhysHypoHelper::TAU_INV_MASS );
    // Proper decay time assuming constant mass hypothesis
    vtx.setTau( m_cascadeTools->tau(mom, vtx.vtx(), pv, mass), pvtype, xAOD::BPhysHypoHelper::TAU_CONST_MASS );
    vtx.setTauErr( m_cascadeTools->tauError(mom, cov, vtx.vtx(), pv, mass), pvtype, xAOD::BPhysHypoHelper::TAU_CONST_MASS );
    //enum pv_type {PV_MAX_SUM_PT2, PV_MIN_A0, PV_MIN_Z0, PV_MIN_Z0_BA};
  } else {
    const float errConst = -9999999.;
    BPHYS_CHECK( vtx.setTau( errConst, pvtype, xAOD::BPhysHypoHelper::TAU_INV_MASS) );
    BPHYS_CHECK( vtx.setTauErr( errConst, pvtype, xAOD::BPhysHypoHelper::TAU_INV_MASS) );
    BPHYS_CHECK( vtx.setTau(errConst, pvtype, xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
    BPHYS_CHECK( vtx.setTauErr( errConst, pvtype, xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
  }

}

void DerivationFramework::BPhysPVCascadeTools::FillBPhysHelperNULL(xAOD::BPhysHelper &vtx,
							    const xAOD::VertexContainer* PvContainer,
							    xAOD::BPhysHelper::pv_type pvtype) {
   DerivationFramework::BPhysPVTools::FillBPhysHelperNULL(vtx, PvContainer, pvtype);
}

size_t DerivationFramework::BPhysPVCascadeTools::FindLowZIndex(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper &Obj,
							const std::vector<const xAOD::Vertex*> &PVlist,
							const size_t PV_minNTracks) const {
  size_t lowZ  = 0;
  if(PVlist.empty()) {
    lowZ=std::numeric_limits<std::size_t>::max();
    return lowZ;
  }
  size_t size = PVlist.size();
  double lowA0zcalc = fabs(m_cascadeTools->a0z       (mom, Obj.vtx(), PVlist[0]));
  for(size_t i =1; i<size; i++) {
    if ( PVlist[i]->nTrackParticles() >= PV_minNTracks ) {
      double a0z =  fabs(m_cascadeTools->a0z(mom, Obj.vtx(), PVlist[i]));
      if(a0z < lowA0zcalc) {
	lowA0zcalc = a0z;
	lowZ =i;
      }
    }
  }
  return lowZ;
}

size_t DerivationFramework::BPhysPVCascadeTools::FindHighPtIndex(const std::vector<const xAOD::Vertex*> &PVlist) {
    return DerivationFramework::BPhysPVTools::FindHighPtIndex(PVlist);
}

size_t DerivationFramework::BPhysPVCascadeTools::FindLowA0Index(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper &Obj,
							 const std::vector<const xAOD::Vertex*> &PVlist,
							 const size_t PV_minNTracks) const {
    size_t lowA0  = 0;
    if(PVlist.empty()) {
        lowA0=std::numeric_limits<std::size_t>::max();
        return lowA0;
    }
    size_t size = PVlist.size();
    double lowA0calc  = m_cascadeTools->a0(mom, Obj.vtx(), PVlist[0]);
    for(size_t i =1; i<size; i++) {
      if ( PVlist[i]->nTrackParticles() >= PV_minNTracks ) {
	double a0 =  m_cascadeTools->a0(mom, Obj.vtx(), PVlist[i]);
	if(a0 < lowA0calc) {
	  lowA0calc = a0;
	  lowA0 =i;
	}
      }
    }
    return lowA0;
}

std::vector<const xAOD::Vertex*> DerivationFramework::BPhysPVCascadeTools::GetGoodPV(const xAOD::VertexContainer* pvContainer) {
    typedef xAOD::VxType::VertexType VertexType;
    VertexType Pvtx = xAOD::VxType::PriVtx;
    VertexType Pileupvtx = xAOD::VxType::PileUp;
    std::vector<const xAOD::Vertex*> goodPrimaryVertices;
    goodPrimaryVertices.reserve(pvContainer->size());

    for (auto ptr = pvContainer->begin(); ptr!= pvContainer->end(); ++ptr) {
        VertexType thistype = (*ptr)->vertexType();
        if ( thistype == Pileupvtx || thistype == Pvtx ) {
	  goodPrimaryVertices.push_back(*ptr);
        } else {
//             cout << "vertex type  " << thistype << endl;
        }
    }
    return goodPrimaryVertices;
}
//-----------------------------------------------------------------------------
//
void DerivationFramework::BPhysPVCascadeTools::SetMinNTracksInPV(size_t PV_minNTracks)
{

  m_PV_minNTracks = PV_minNTracks;
}
//-----------------------------------------------------------------------------
//
const Amg::Vector3D& DerivationFramework::BPhysPVCascadeTools::GetBeamSpot() const  noexcept {
  if(m_beamSpotData) return m_beamSpotData->beamPos();
  else {
    static const Amg::Vector3D defaultBS(-10000.,-10000.,-10000.);
    return defaultBS;
  } 
}
//-----------------------------------------------------------------------------
//
size_t DerivationFramework::BPhysPVCascadeTools::FindLowZ0BAIndex(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper &obj,
							   const std::vector<const xAOD::Vertex*> &PVlist,
							   const size_t PV_minNTracks) const {
  
  size_t ilowZ0BA   = std::numeric_limits<std::size_t>::max();
  double lowZ0BAcalc = std::numeric_limits<double>::max(); 
  for (size_t i = 0; i<PVlist.size(); ++i) {
    if ( PVlist[i]->nTrackParticles() >= PV_minNTracks ) {
      double z0BA =  m_cascadeTools->a0(mom, obj.vtx(), PVlist[i]);
      if (z0BA < lowZ0BAcalc) {
	lowZ0BAcalc = z0BA;
	ilowZ0BA = i;
      }
    }
  }
  return ilowZ0BA;
}
//-----------------------------------------------------------------------------
//
double DerivationFramework::BPhysPVCascadeTools::DistInZtoDOCA(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper &obj, const xAOD::Vertex* vertex) const {

  Amg::Vector3D pv    = vertex->position();
  Amg::Vector3D xDOCA = DocaExtrapToBeamSpot(mom, obj);
  Amg::Vector3D vec   = pv - xDOCA;
  return vec.z();
}
//-----------------------------------------------------------------------------
//
Amg::Vector3D DerivationFramework::BPhysPVCascadeTools::DocaExtrapToBeamSpot(const std::vector<TLorentzVector> &mom, const xAOD::BPhysHelper& obj) const {

  Amg::Vector3D xDOCA(-99999., -99999., -99999.);
  TLorentzVector totalMom;
  unsigned int NTrk = mom.size();
  for( unsigned int it=0; it<NTrk; it++) totalMom += mom[it];
  TVector3      totP = totalMom.Vect();
  Amg::Vector3D pSV(totP.X(), totP.Y(), totP.Z());
  Amg::Vector3D pT(pSV.x(), pSV.y(), 0.);
  if ( pT.mag2() > 0 ) {
    Amg::Vector3D xBS = GetBeamSpot();
    Amg::Vector3D xSV = obj.vtx()->position();
    Amg::Vector3D xT(xSV.x()-xBS.x(), xSV.y()-xBS.y(), 0.);
    xDOCA = xSV - pSV*pT.dot(xT)/pT.mag2();
  } else {
    std::cout << "BPhysPVCascadeTools::DocaExtrapToBeamSpot: WARNING pT == 0."
	      << std::endl;
  }
  return xDOCA;
}

void DerivationFramework::BPhysPVCascadeTools::PrepareVertexLinks(Trk::VxCascadeInfo *result,  const xAOD::TrackParticleContainer* importedTrackCollection)
{
    auto &collection = result->vertices();
    for(auto v : collection)
    {
       std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
       for(unsigned int i=0; i< v->trackParticleLinks().size(); i++)
       {
         ElementLink<DataVector<xAOD::TrackParticle> > mylink=v->trackParticleLinks()[i]; // makes a copy (non-const) 
         mylink.setStorableObject(*importedTrackCollection, true);
         newLinkVector.push_back( mylink ); 
       }
       v->clearTracks();
       v->setTrackParticleLinks( newLinkVector );
    }
}

std::vector<const xAOD::TrackParticle*> DerivationFramework::BPhysPVCascadeTools::CollectAllChargedTracks(const std::vector<xAOD::Vertex*> &cascadeVertices)
{
  std::vector<const xAOD::TrackParticle*> exclTrk;
  for( size_t jt=0; jt<cascadeVertices.size(); jt++) {
    for( size_t it=0; it<cascadeVertices[jt]->vxTrackAtVertex().size(); it++) {
     if(cascadeVertices[jt]->trackParticle(it)->charge() != 0) exclTrk.push_back(cascadeVertices[jt]->trackParticle(it));
    }
  }
  return exclTrk;
}

StatusCode DerivationFramework::BPhysPVCascadeTools::FillCandwithRefittedVertices( bool refitPV,
									   const xAOD::VertexContainer* pvContainer, xAOD::VertexContainer* refPvContainer,
									   const Analysis::PrimaryVertexRefitter *pvRefitter, size_t in_PV_max, int DoVertexType,
                                                                           Trk::VxCascadeInfo* casc, int index,
                                                                           double mass, xAOD::BPhysHypoHelper &vtx)
{
    const std::vector<TLorentzVector> &mom = casc->getParticleMoms()[index];
    const Amg::MatrixX &cov = casc->getCovariance()[index];
    const std::vector<xAOD::Vertex*> &cascadeVertices = casc->vertices();
    const bool doPt   = (DoVertexType & 1) != 0;
    const bool doA0   = (DoVertexType & 2) != 0;
    const bool doZ0   = (DoVertexType & 4) != 0;
    const bool doZ0BA = (DoVertexType & 8) != 0;

    // Collect the tracks that should be excluded from the PV
    std::vector<const xAOD::TrackParticle*> exclTrk = CollectAllChargedTracks(cascadeVertices);


    const std::vector<const xAOD::Vertex*> GoodPVs = GetGoodPV(pvContainer);
    // 2) PV dependent variables
    if (GoodPVs.empty() == false) {
       if (refitPV) {
         size_t pVmax =std::min((size_t)in_PV_max, GoodPVs.size());
         std::vector<const xAOD::Vertex*> refPVvertexes;
         std::vector<const xAOD::Vertex*> refPVvertexes_toDelete;
         std::vector<int> exitCode;
         refPVvertexes.reserve(pVmax);
         refPVvertexes_toDelete.reserve(pVmax);
         exitCode.reserve(pVmax);

         // Refit the primary vertex and set the related decorations.

         for (size_t i =0; i < pVmax ; i++) {
           const xAOD::Vertex* oldPV = GoodPVs.at(i);
           // when set to false this will return null when a new vertex is not required
//           ATH_MSG_DEBUG("old PV x " << oldPV->x() << " y " << oldPV->y() << " z " << oldPV->z());
           int exitcode = 0;
           const xAOD::Vertex* refPV = pvRefitter->refitVertex(oldPV, exclTrk, m_copyAllVertices, &exitcode);
//           if (refPV) ATH_MSG_DEBUG("ref PV x " << refPV->x() << " y " << refPV->y() << " z " << refPV->z());
           exitCode.push_back(exitcode);
           // we want positioning to match the goodPrimaryVertices
           if (refPV == nullptr) {
              refPVvertexes.push_back(oldPV);
              refPVvertexes_toDelete.push_back(nullptr);
           } else {
              refPVvertexes.push_back(refPV);
              refPVvertexes_toDelete.push_back(refPV);
           }
        }
         LocalVector<size_t, 4> indexesUsed;
         LocalVector<std::pair<size_t, xAOD::BPhysHelper::pv_type>, 4> indexestoProcess;

         if(doPt){
            indexestoProcess.push_back(std::make_pair
                (FindHighPtIndex(refPVvertexes), xAOD::BPhysHelper::PV_MAX_SUM_PT2));
         }
         if(doA0) {
            indexestoProcess.push_back(std::make_pair( FindLowA0Index(mom, vtx, refPVvertexes, m_PV_minNTracks),  
              xAOD::BPhysHelper::PV_MIN_A0));
         }
         if(doZ0) {
            indexestoProcess.push_back(std::make_pair(FindLowZIndex(mom, vtx, refPVvertexes, m_PV_minNTracks),
                    xAOD::BPhysHelper::PV_MIN_Z0));
         }
         if(doZ0BA) {
            size_t lowZBA = FindLowZ0BAIndex(mom, vtx, refPVvertexes, m_PV_minNTracks);
            if( lowZBA < pVmax ) { 
               indexestoProcess.push_back(std::make_pair(lowZBA, xAOD::BPhysHelper::PV_MIN_Z0_BA));
            }
            else FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
         }

         for(size_t i =0 ; i<indexestoProcess.size(); i++){
             //if refitted add to refitted container
             auto index  = indexestoProcess[i].first;
             auto pvtype = indexestoProcess[i].second;
             const xAOD::VertexContainer* ParentContainer =
                 (refPVvertexes_toDelete.at(index)) ? refPvContainer : pvContainer;
             if(ParentContainer == refPvContainer && !indexesUsed.contains(index)) {
                 // store the new vertex
                 refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(index))); 
                 indexesUsed.push_back(index);
             }
             FillBPhysHelper(mom, cov, vtx, refPVvertexes[index],
                  ParentContainer, pvtype, exitCode[index]);
             vtx.setOrigPv(GoodPVs[index], pvContainer, pvtype);               
         }
          //nullify ptrs we want to keep so these won't get deleted
         //"delete null" is valid in C++ and does nothing so this is quicker than a lot of if statements
         for(size_t x : indexesUsed) refPVvertexes_toDelete[x] = nullptr;
          //Loop over toDELETE container, anything that is used or was not refitted is null
         //This cleans up all extra vertices that were created and not used
         for(const xAOD::Vertex* ptr : refPVvertexes_toDelete) delete ptr;
         refPVvertexes.clear(); // Clear lists of now dangling ptrs
         refPVvertexes_toDelete.clear();
         exitCode.clear();

       } else {
            // 2.a) the first PV with the largest sum pT.
         if(doPt) {
           size_t highPtindex = FindHighPtIndex(GoodPVs); // Should be 0 in PV ordering
           FillBPhysHelper(mom, cov, vtx, GoodPVs[highPtindex], pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
         }
         // 2.b) the closest in 3D:
         if(doA0) {
           size_t lowA0 =  FindLowA0Index(mom, vtx, GoodPVs, m_PV_minNTracks);
           FillBPhysHelper(mom, cov, vtx, GoodPVs[lowA0], pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
         }
         // 2.c) the closest in Z:
         if(doZ0) {
           size_t lowZ  = FindLowZIndex(mom, vtx, GoodPVs, m_PV_minNTracks);
           FillBPhysHelper(mom, cov, vtx, GoodPVs[lowZ], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
         }
         // 2.d) the closest in Z (DOCA w.r.t. beam axis):
         if(doZ0BA) {
           size_t lowZBA = FindLowZ0BAIndex(mom, vtx, GoodPVs, m_PV_minNTracks);
           if ( lowZBA < GoodPVs.size() ) { // safety against vector index out-of-bounds
             FillBPhysHelper(mom, cov, vtx, GoodPVs[lowZBA], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
           } else {
             // nothing found -- fill NULL
             FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
           }
         }
       } // refitPV
     } else {

      if(pvContainer->empty()) return StatusCode::FAILURE;
      const xAOD::Vertex* Dummy = pvContainer->at(0);

          // 2.a) the first PV with the largest sum pT.
      if(doPt) {
        FillBPhysHelper(mom, cov, vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
        if(refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
      }
      // 2.b) the closest in 3D:
      if(doA0) {
        FillBPhysHelper(mom, cov, vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
        if(refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0);
      }
      // 2.c) the closest in Z:
      if(doZ0) {
        FillBPhysHelper(mom, cov, vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
        if(refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0);
      }
      // 2.d) the closest in Z (DOCA w.r.t. beam axis):
      if(doZ0BA) {
        FillBPhysHelper(mom, cov, vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
        if(refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
      }
   } // GoodPVs.empty()

   // 3) proper decay time and error:
   // retrieve the refitted PV (or the original one, if the PV refitting was turned off)
   if(doPt)   ProcessVertex(mom, cov, vtx, xAOD::BPhysHelper::PV_MAX_SUM_PT2, mass);
   if(doA0)   ProcessVertex(mom, cov, vtx, xAOD::BPhysHelper::PV_MIN_A0, mass);
   if(doZ0)   ProcessVertex(mom, cov, vtx, xAOD::BPhysHelper::PV_MIN_Z0, mass);
   if(doZ0BA) ProcessVertex(mom, cov, vtx, xAOD::BPhysHelper::PV_MIN_Z0_BA, mass);

   return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------

void DerivationFramework::BPhysPVCascadeTools::SetVectorInfo(xAOD::BPhysHelper &vtx, const Trk::VxCascadeInfo* casc){

    const std::vector< std::vector<TLorentzVector> > &moms = casc->getParticleMoms();
    const std::vector<xAOD::Vertex*> &cascadeVertices = casc->vertices();
    // Get refitted track momenta from all vertices, charged tracks only
    std::vector<float> px;
    std::vector<float> py;
    std::vector<float> pz;
    for( size_t jt=0; jt<moms.size(); jt++) {
       for( size_t it=0; it<cascadeVertices[jt]->vxTrackAtVertex().size(); it++) {
        px.push_back( moms[jt][it].Px() );
        py.push_back( moms[jt][it].Py() );
        pz.push_back( moms[jt][it].Pz() );
      }
    }
    vtx.setRefTrks(std::move(px),std::move(py),std::move(pz));
    
}

bool DerivationFramework::BPhysPVCascadeTools::uniqueCollection(const std::vector<const xAOD::TrackParticle*>&col){
    for(auto p : col){
        if(std::count(col.begin(), col.end(), p) > 1) return false;
    }
    return true;
}

bool DerivationFramework::BPhysPVCascadeTools::uniqueCollection(const std::vector<const xAOD::TrackParticle*>&col1, const std::vector<const xAOD::TrackParticle*>&col2){
    for(auto p : col1){
        if((std::count(col1.begin(), col1.end(), p) + std::count(col2.begin(), col2.end(), p)) > 1) return false;
    }
    for(auto p : col2){
        if((std::count(col1.begin(), col1.end(), p) + std::count(col2.begin(), col2.end(), p)) > 1) return false;
    }
    return true;
}

bool DerivationFramework::BPhysPVCascadeTools::LinkVertices(SG::AuxElement::Decorator<VertexLinkVector> &decor, const std::vector<const xAOD::Vertex*>& vertices,
                                                 const xAOD::VertexContainer* vertexContainer, const xAOD::Vertex* vert){
  // create tmp vector of preceding vertex links
  VertexLinkVector precedingVertexLinks;

  // loop over input precedingVertices  
  auto precedingVerticesItr = vertices.begin();
  for(; precedingVerticesItr!=vertices.end(); ++precedingVerticesItr) {
       // sanity check 1: protect against null pointers
       if( !(*precedingVerticesItr) )
         return false;
    
    // create element link
    VertexLink vertexLink;
    vertexLink.setElement(*precedingVerticesItr);
    vertexLink.setStorableObject(*vertexContainer);
    
       // sanity check 2: is the link valid?
    if( !vertexLink.isValid() )
       return false;
    
    // link is OK, store it in the tmp vector
    precedingVertexLinks.push_back( vertexLink );
 
  } // end of loop over preceding vertices
  
    // all OK: store preceding vertex links in the aux store
   decor(*vert) = precedingVertexLinks;
   return true;
}

double DerivationFramework::BPhysPVCascadeTools::getParticleMass(const HepPDT::ParticleDataTable* pdt, int pdgcode){
    auto ptr = pdt->particle( pdgcode );
    return ptr ? ptr->mass() : 0.;
}

