/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkBPhys/BPhysPVCascadeTools.h"
#include "xAODTracking/VertexContainer.h"
//#include "DerivationFrameworkBPhys/CascadeTools.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include <iostream>
#include <cmath>
#include "xAODBPhys/BPhysHelper.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include <limits>
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TVector3.h"
using namespace std;

DerivationFramework::BPhysPVCascadeTools::BPhysPVCascadeTools(CascadeTools *cascadeTools) :
  m_cascadeTools(cascadeTools), m_beamSpotSvc(NULL), m_PV_minNTracks(0),
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

void DerivationFramework::BPhysPVCascadeTools::FillBPhysHelper(std::vector<TLorentzVector> mom, Amg::MatrixX cov, xAOD::BPhysHelper &vtx,
							const xAOD::Vertex* PV, const xAOD::VertexContainer* PvContainer,
							xAOD::BPhysHelper::pv_type pvtype, int refitCode) const {

  BPHYS_CHECK( vtx.setPv      ( PV, PvContainer, pvtype ) );

  // cout << "BPhysPVCascadeTools::FillBPhysHelper for pvtype = " << pvtype << endl;
        cout << "lxy " << m_cascadeTools->lxy(mom, vtx.vtx(), PV) << " error " << m_cascadeTools->lxyError(mom, cov, vtx.vtx(), PV) << endl;

  
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

void DerivationFramework::BPhysPVCascadeTools::FillBPhysHelperNULL(xAOD::BPhysHelper &vtx,
							    const xAOD::VertexContainer* PvContainer,
							    xAOD::BPhysHelper::pv_type pvtype) {
  const xAOD::Vertex* PV = NULL;
  BPHYS_CHECK( vtx.setPv      ( PV, PvContainer, pvtype ) );
  const float errConst = std::numeric_limits<float>::lowest();
  // set variables claculated from PV
  BPHYS_CHECK( vtx.setLxy    ( errConst, pvtype ) );
  BPHYS_CHECK( vtx.setLxyErr ( errConst, pvtype ) );
  BPHYS_CHECK( vtx.setA0     ( errConst, pvtype ) );
  BPHYS_CHECK( vtx.setA0Err  ( errConst, pvtype ) );
  BPHYS_CHECK( vtx.setA0xy   ( errConst, pvtype ) );
  BPHYS_CHECK( vtx.setA0xyErr( errConst, pvtype ) );
  BPHYS_CHECK( vtx.setZ0     ( errConst, pvtype ) );
  BPHYS_CHECK( vtx.setZ0Err  ( errConst, pvtype ) );
  BPHYS_CHECK( vtx.setRefitPVStatus  ( 0, pvtype ) );
}

size_t DerivationFramework::BPhysPVCascadeTools::FindLowZIndex(std::vector<TLorentzVector> mom, const xAOD::BPhysHelper &Obj,
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

/*
void DerivationFramework::BPhysPVCascadeTools::DecorateWithDummyVertex(std::vector<TLorentzVector> mom, Amg::MatrixX cov, xAOD::VertexContainer* vtxContainer, 
								const xAOD::VertexContainer* pvContainer, const xAOD::Vertex* Dummy,
								int DoVertexType, const bool SetOrignal) const {
  const bool doPt   = (DoVertexType & 1) != 0;
  const bool doA0   = (DoVertexType & 2) != 0;
  const bool doZ0   = (DoVertexType & 4) != 0;
  const bool doZ0BA = (DoVertexType & 8) != 0;

  xAOD::VertexContainer::iterator vtxItr = vtxContainer->begin();
  for(; vtxItr!=vtxContainer->end(); ++vtxItr) {
    xAOD::BPhysHelper vtx(*vtxItr);

    // 1) pT error
    double ptErr = m_cascadeTools->pTError( mom, cov );
    BPHYS_CHECK( vtx.setPtErr(ptErr) );
    if(doPt) {
      // 2.a) the first PV with the largest sum pT.
      FillBPhysHelper(mom, cov, vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
      if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
    }

    if(doA0) {
      // 2.b) the closest in 3D:
      FillBPhysHelper(mom, cov, vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
      if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0);
    }

    if(doZ0) {
      FillBPhysHelper(mom, cov, vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
      if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0);
    }

    if(doZ0BA) {
      FillBPhysHelper(mom, cov, vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
      if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
    }
  }    
}
*/

/*
void DerivationFramework::BPhysPVCascadeTools::DecorateWithNULL(std::vector<TLorentzVector> mom, Amg::MatrixX cov, xAOD::VertexContainer* vtxContainer,
							 const xAOD::VertexContainer* pvContainer, int DoVertexType) const {
  const bool doPt   = (DoVertexType & 1) != 0;
  const bool doA0   = (DoVertexType & 2) != 0;
  const bool doZ0   = (DoVertexType & 4) != 0;
  const bool doZ0BA = (DoVertexType & 8) != 0;
  xAOD::VertexContainer::iterator vtxItr = vtxContainer->begin();
  for(; vtxItr!=vtxContainer->end(); ++vtxItr) {
    xAOD::BPhysHelper vtx(*vtxItr);

    // 1) pT error
    double ptErr = m_cascadeTools->pTError( mom, cov );
    BPHYS_CHECK( vtx.setPtErr(ptErr) );
    if(doPt) {
      // 2.a) the first PV with the largest sum pT.
      FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
    }

    if(doA0) {
      // 2.b) the closest in 3D:
      FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_A0);
    }

    if(doZ0) {
      FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0);
    }
    if(doZ0BA) {
      FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
    }
  }
}
*/

/*
StatusCode DerivationFramework::BPhysPVCascadeTools::FillCandExistingVertices(std::vector<TLorentzVector> mom, Amg::MatrixX cov, xAOD::VertexContainer* vtxContainer,
								       const xAOD::VertexContainer* pvContainer, int DoVertexType) {

  // reset beamspot cache
  GetBeamSpot(true);

  //----------------------------------------------------
  // decorate the vertex
  //----------------------------------------------------
  // loop over candidates -- Don't apply PV_minNTracks requirement here
  // because it may result in exclusion of the high-pt PV.
  const std::vector<const xAOD::Vertex*> GoodPVs = GetGoodPV(pvContainer);


  if(GoodPVs.empty() == false) {

    const bool doPt   = (DoVertexType & 1) != 0;
    const bool doA0   = (DoVertexType & 2) != 0;
    const bool doZ0   = (DoVertexType & 4) != 0;
    const bool doZ0BA = (DoVertexType & 8) != 0;

  xAOD::VertexContainer::iterator vtxItr = vtxContainer->begin();
    for(; vtxItr!=vtxContainer->end(); ++vtxItr) {
      xAOD::BPhysHelper vtx(*vtxItr);

      // 1) pT error
      double ptErr = m_cascadeTools->pTError( mom, cov );
      BPHYS_CHECK( vtx.setPtErr(ptErr) );

      // 2) refit the primary vertex and set the related decorations.
      if(doPt) {
	size_t highPtindex = FindHighPtIndex(GoodPVs); //Should be 0 in PV ordering
	// 2.a) the first PV with the largest sum pT.
	FillBPhysHelper(mom, cov, vtx, GoodPVs[highPtindex], pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
      }

      if(doA0) {
	// 2.b) the closest in 3D:
	size_t lowA0 =  FindLowA0Index(mom, vtx, GoodPVs, m_PV_minNTracks);
	FillBPhysHelper(mom, cov, vtx, GoodPVs[lowA0], pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
      }

      if(doZ0) {
	size_t lowZ  = FindLowZIndex(mom, vtx, GoodPVs, m_PV_minNTracks);
	FillBPhysHelper(mom, cov, vtx, GoodPVs[lowZ], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
      }

      if(doZ0BA) {
	size_t lowZBA = FindLowZ0BAIndex(mom, vtx, GoodPVs, m_PV_minNTracks);
	if ( lowZBA < GoodPVs.size() ) { // safety against vector index out-of-bounds
	  FillBPhysHelper(mom, cov, vtx, GoodPVs[lowZBA], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
	} else {
	  // nothing found -- fill NULL
	  FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
	}
      }

    }// end of loop over vertices
  } //end of check for vertices
  else {
    //        cout << "Warning: DerivationFramework::BPhysPVCascadeTools::FillCandExistingVertices No Primary Vertices Found trying to decorate wilth dummy \n";
    if(pvContainer->empty()) return StatusCode::FAILURE;
    const xAOD::Vertex* dummy = pvContainer->at(0);  //No good vertices so last vertex must be dummy
    //DecorateWithDummyVertex(mom, cov, vtxContainer, pvContainer, dummy, DoVertexType, false);
  }
  return StatusCode::SUCCESS;
}
*/

/*
StatusCode DerivationFramework::BPhysPVCascadeTools::FillCandwithRefittedVertices(std::vector<TLorentzVector> mom, Amg::MatrixX cov, xAOD::VertexContainer* vtxContainer,
									   const xAOD::VertexContainer* pvContainer, xAOD::VertexContainer* refPvContainer,
									   Analysis::PrimaryVertexRefitter *pvRefitter, size_t in_PV_max, int DoVertexType) {

  // reset beamspot cache
  GetBeamSpot(true);

  //----------------------------------------------------
  // decorate the vertex
  //----------------------------------------------------
  // loop over candidates -- Don't apply PV_minNTracks requirement here
  // because it may result in exclusion of the high-pt PV.
  std::vector<const xAOD::Vertex*> goodPrimaryVertices = GetGoodPV(pvContainer);

//  cout << "BPhysPVCascadeTools::FillCandwithRefittedVertices: #PVs: all: "
//       << pvContainer->size() << " ref: " << refPvContainer->size()
//       << " good: " << goodPrimaryVertices.size()
//       << " PV_minNTracks: " << m_PV_minNTracks << endl; 

  if(goodPrimaryVertices.empty() == false) {

        size_t pVmax =std::min(in_PV_max, goodPrimaryVertices.size());
        std::vector<const xAOD::Vertex*> refPVvertexes;
        std::vector<const xAOD::Vertex*> refPVvertexes_toDelete;
        std::vector<int> exitCode;
        refPVvertexes.reserve(pVmax);
        refPVvertexes_toDelete.reserve(pVmax);
        exitCode.reserve(pVmax);

        bool doPt   = (DoVertexType & 1) != 0;
        bool doA0   = (DoVertexType & 2) != 0;
        bool doZ0   = (DoVertexType & 4) != 0;
	bool doZ0BA = (DoVertexType & 8) != 0;

        xAOD::VertexContainer::iterator vtxItr = vtxContainer->begin();
        for(; vtxItr!=vtxContainer->end(); ++vtxItr) {
            xAOD::BPhysHelper vtx(*vtxItr);

            // 1) pT error
            double ptErr = m_cascadeTools->pTError( mom, cov );
            BPHYS_CHECK( vtx.setPtErr(ptErr) );

            for(size_t i =0; i < pVmax ; i++) {
                const xAOD::Vertex* oldPV = goodPrimaryVertices.at(i);
                //when set to false this will return null when a new vertex is not required
                const xAOD::Vertex* refPV = pvRefitter->refitVertex(oldPV, vtx.vtx(), false);
                exitCode.push_back(pvRefitter->getLastExitCode());
                //I want positioning to match the goodPrimaryVertices
                if(refPV == nullptr){
                   refPVvertexes.push_back(oldPV);
                   refPVvertexes_toDelete.push_back(nullptr);
                }else{
                   refPVvertexes.push_back(refPV);
                   refPVvertexes_toDelete.push_back(refPV);
                }
            }

            // 2) refit the primary vertex and set the related decorations.

            size_t highPtindex = doPt ? FindHighPtIndex(refPVvertexes) : 9999999; //Should be 0 in PV ordering
            size_t lowA0 = doA0 ?
	      FindLowA0Index(mom, vtx, refPVvertexes, m_PV_minNTracks) : 9999998;
            size_t lowZ  = doZ0 ?
	      FindLowZIndex(mom, vtx, refPVvertexes, m_PV_minNTracks) : 9999997;
            size_t lowZBA = doZ0BA ?
	      FindLowZ0BAIndex(mom, vtx, refPVvertexes, m_PV_minNTracks) : 9999996;

	    //cout << "BPhysPVCascadeTools::FillCandwithRefittedVertices: in_PV_max/pVMax = "
	    //     << in_PV_max << ", " << pVmax << endl;
	    //cout << "BPhysPVCascadeTools::FillCandwithRefittedVertices: m_PV_minNTracks = "
	    //     << m_PV_minNTracks << endl;
	    //cout << "BPhysPVCascadeTools::FillCandwithRefittedVertices: hPt,lowA0/Z/ZBA = "
	    //     << highPtindex << ", "
	    //     << lowA0 << ", " << lowZ << ", " << lowZBA << " "
	    //     << (lowA0 != lowZ   ? "1!" : "  ")
	    //     << (lowA0 != lowZBA ? "2!" : "  ")
	    //     << (lowZ  != lowZBA ? "3!" : "  ")
	    //     << (highPtindex != lowA0  ? "4!" : "  ")
	    //     << (highPtindex != lowZ   ? "5!" : "  ")
	    //     << (highPtindex != lowZBA ? "6!" : "  ")
	    //     << endl; 
 
            if(doPt) {
                //Choose old PV container if not refitted
                const xAOD::VertexContainer* ParentContainer =
                    (refPVvertexes_toDelete.at(highPtindex)) ? refPvContainer : pvContainer; 
                if(ParentContainer == refPvContainer) //if refitted add to refitted container
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(highPtindex))); // store the new vertex
              
                FillBPhysHelper(mom, cov, vtx, refPVvertexes[highPtindex],
                     ParentContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, exitCode[highPtindex]);
                vtx.setOrigPv(goodPrimaryVertices[highPtindex], pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
            }

            if(doA0) {
                const xAOD::VertexContainer* ParentContainer = 
                     (refPVvertexes_toDelete.at(lowA0)) ? refPvContainer : pvContainer; 
                if(ParentContainer == refPvContainer && highPtindex!=lowA0)
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(lowA0))); // store the new vertex
                
                FillBPhysHelper(mom, cov, vtx, refPVvertexes[lowA0],
                      ParentContainer, xAOD::BPhysHelper::PV_MIN_A0, exitCode[lowA0]);
                vtx.setOrigPv(goodPrimaryVertices[lowA0], pvContainer, xAOD::BPhysHelper::PV_MIN_A0);
            }


	    // 2.c) the closest in Z:
            if(doZ0) {

                const xAOD::VertexContainer* ParentContainer =
                    (refPVvertexes_toDelete.at(lowZ)) ? refPvContainer : pvContainer;
                if(ParentContainer == refPvContainer && highPtindex!=lowZ && lowZ!=lowA0)
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(lowZ))); // store the new vertex
                
                FillBPhysHelper(mom, cov, vtx, refPVvertexes[lowZ], 
                      ParentContainer, xAOD::BPhysHelper::PV_MIN_Z0, exitCode[lowZ]);
                vtx.setOrigPv(goodPrimaryVertices[lowZ], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0);
            }

	    // 2.d) the closest in Z (DOCA w.r.t. beam axis):
            if (doZ0BA) {
	      if ( lowZBA < pVmax ) { // safety for vector indices
		const xAOD::VertexContainer* ParentContainer =
		  (refPVvertexes_toDelete.at(lowZBA)) ?
		  refPvContainer : pvContainer;
		if (ParentContainer == refPvContainer && highPtindex!=lowZBA
		    && lowZBA!=lowA0 && lowZBA != lowZ) {
		  // store the new vertex
		  refPvContainer->push_back(const_cast<xAOD::Vertex*>
					    (refPVvertexes.at(lowZBA)));
		}
		FillBPhysHelper(mom, cov, vtx, refPVvertexes[lowZBA],
				ParentContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA,
				exitCode[lowZBA]);
		vtx.setOrigPv(goodPrimaryVertices[lowZBA], pvContainer,
			      xAOD::BPhysHelper::PV_MIN_Z0_BA);
	      } else {
		// nothing found -- fill NULL
		FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
		// nothing found -- fill dummy vertex (type-0 vertex)
		// if(pvContainer->empty()) return StatusCode::FAILURE;
 	        // const xAOD::Vertex* dummy = pvContainer->at(pvContainer->size()-1);  //No good vertices so last vertex must be dummy
		// FillBPhysHelper(mom, cov, vtx, dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
		// vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
	      }
	    }

            //nullify ptrs we want to keep so these won't get deleted
            //"delete null" is valid in C++ and does nothing so this is quicker than a lot of if statements
            if(doPt)                     refPVvertexes_toDelete[highPtindex] = nullptr;
            if(doA0)                     refPVvertexes_toDelete[lowA0]       = nullptr;
            if(doZ0)                     refPVvertexes_toDelete[lowZ]        = nullptr;
            if(doZ0BA && lowZBA < pVmax) refPVvertexes_toDelete[lowZBA]      = nullptr;
            //Loop over toDELETE container, anything that is used or was not refitted is null
            //This cleans up all extra vertices that were created and not used
            for(const xAOD::Vertex* ptr : refPVvertexes_toDelete) {
                delete ptr;
            }
            refPVvertexes.clear();// Clear lists of now dangling ptrs
            refPVvertexes_toDelete.clear();
            exitCode.clear();
        }  // end of loop over vertices

    } else {
//        cout << "Warning: DerivationFramework::BPhysPVCascadeTools::FillCandwithRefittedVertices No Primary Vertices Found trying to decorate wilth dummy \n";
        if(pvContainer->empty()) return StatusCode::FAILURE;
        const xAOD::Vertex* dummy = pvContainer->at(0);  //No good vertices so last vertex must be dummy
        DecorateWithDummyVertex(mom, cov, vtxContainer, pvContainer, dummy, DoVertexType, true);
    }

    return StatusCode::SUCCESS;
}
*/

size_t DerivationFramework::BPhysPVCascadeTools::FindHighPtIndex(const std::vector<const xAOD::Vertex*> &PVlist) {
    // it SHOULD be the first one in the collection but it shouldn't take long to do a quick check
    for(size_t i =0; i<PVlist.size(); i++) {
        if(PVlist[i]->vertexType() == xAOD::VxType::PriVtx) return i;
    }
    cout << "FATAL ERROR High Pt Primary vertex not found - this should not happen\n";
    return std::numeric_limits<std::size_t>::max(); //This should not happen
}

size_t DerivationFramework::BPhysPVCascadeTools::FindLowA0Index(std::vector<TLorentzVector> mom, const xAOD::BPhysHelper &Obj,
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
// added by WW:
//
void DerivationFramework::BPhysPVCascadeTools::SetMinNTracksInPV(size_t PV_minNTracks)
{

  m_PV_minNTracks = PV_minNTracks;
}
//-----------------------------------------------------------------------------
// added by WW:
//
Amg::Vector3D DerivationFramework::BPhysPVCascadeTools::GetBeamSpot(bool resetCache)
  const {

  if ( resetCache ) {
    // reset cache
    m_beamspot = Amg::Vector3D(0.,0.,0.);
    m_BScached = false;
  } else {
    if ( !m_BScached ) {
      if ( m_beamSpotSvc != NULL ) { 
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
// added by WW:
//
size_t DerivationFramework::BPhysPVCascadeTools::FindLowZ0BAIndex(std::vector<TLorentzVector> mom, const xAOD::BPhysHelper &obj,
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
// added by WW:
//
double DerivationFramework::BPhysPVCascadeTools::DistInZtoDOCA(std::vector<TLorentzVector> mom, const xAOD::BPhysHelper &obj, const xAOD::Vertex* vertex) const {

  Amg::Vector3D pv    = vertex->position();
  Amg::Vector3D xDOCA = DocaExtrapToBeamSpot(mom, obj);
  Amg::Vector3D vec   = pv - xDOCA;
  return vec.z();
}
//-----------------------------------------------------------------------------
// added by WW:
//
Amg::Vector3D DerivationFramework::BPhysPVCascadeTools::DocaExtrapToBeamSpot(std::vector<TLorentzVector> mom, const xAOD::BPhysHelper& obj) const {

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
