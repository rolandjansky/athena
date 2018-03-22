/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkBPhys/BPhysPVCascadeTools.h"
#include "xAODTracking/VertexContainer.h"
#include <iostream>
#include "xAODBPhys/BPhysHelper.h"
#include <limits>
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TVector3.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"
using namespace std;

DerivationFramework::BPhysPVCascadeTools::BPhysPVCascadeTools(CascadeTools *cascadeTools) :
  m_cascadeTools(cascadeTools), m_beamSpotSvc(nullptr), m_PV_minNTracks(0),
  m_BScached(false)
{
}

DerivationFramework::BPhysPVCascadeTools::BPhysPVCascadeTools(CascadeTools *cascadeTools,
                                                const ServiceHandle<IBeamCondSvc>
                                                *beamSpotSvc) :
  m_cascadeTools(cascadeTools), m_beamSpotSvc(beamSpotSvc), m_PV_minNTracks(0),
  m_BScached(false)
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

vector<const xAOD::Vertex*> DerivationFramework::BPhysPVCascadeTools::GetGoodPV(const xAOD::VertexContainer* pvContainer) {
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
Amg::Vector3D DerivationFramework::BPhysPVCascadeTools::GetBeamSpot(bool resetCache)
  const {

  if ( resetCache ) {
    // reset cache
    m_beamspot = Amg::Vector3D(0.,0.,0.);
    m_BScached = false;
  } else {
    if ( !m_BScached ) {
      if ( m_beamSpotSvc != nullptr ) { 
	// obtain beamspot from BeamCondSvc
	m_beamspot = (*m_beamSpotSvc)->beamPos();
      } else {
	m_beamspot = Amg::Vector3D(-10000.,-10000.,-10000.);
      }
    }
  }
  return m_beamspot;
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
//-----------------------------------------------------------------------------
