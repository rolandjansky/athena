/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkBPhys/BPhysPVTools.h"
#include "xAODTracking/VertexContainer.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include <iostream>
#include <cmath>
#include "xAODBPhys/BPhysHelper.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include <limits>
using namespace std;

void DerivationFramework::BPhysPVTools::FillBPhysHelper(xAOD::BPhysHelper &vtx,
        const xAOD::Vertex* PV, const xAOD::VertexContainer* PvContainer,
        xAOD::BPhysHelper::pv_type pvtype, int refitCode) const {

    BPHYS_CHECK( vtx.setPv      ( PV, PvContainer, pvtype ) );

    // set variables claculated from PV
    BPHYS_CHECK( vtx.setLxy    ( m_v0Tools->lxy       (vtx.vtx(), PV), pvtype ) );
    BPHYS_CHECK( vtx.setLxyErr ( m_v0Tools->lxyError  (vtx.vtx(), PV), pvtype ) );
    BPHYS_CHECK( vtx.setA0     ( m_v0Tools->a0        (vtx.vtx(), PV), pvtype ) );
    BPHYS_CHECK( vtx.setA0Err  ( m_v0Tools->a0Error   (vtx.vtx(), PV), pvtype ) );
    BPHYS_CHECK( vtx.setA0xy   ( m_v0Tools->a0xy      (vtx.vtx(), PV), pvtype ) );
    BPHYS_CHECK( vtx.setA0xyErr( m_v0Tools->a0xyError (vtx.vtx(), PV), pvtype ) );
    BPHYS_CHECK( vtx.setZ0     ( m_v0Tools->a0z       (vtx.vtx(), PV), pvtype ) );
    BPHYS_CHECK( vtx.setZ0Err  ( m_v0Tools->a0zError  (vtx.vtx(), PV), pvtype ) );
    BPHYS_CHECK( vtx.setRefitPVStatus  ( refitCode, pvtype ) );

}

void DerivationFramework::BPhysPVTools::FillBPhysHelperNULL(xAOD::BPhysHelper &vtx,
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

size_t DerivationFramework::BPhysPVTools::FindLowZIndex(const xAOD::BPhysHelper &Obj,
       const std::vector<const xAOD::Vertex*> &PVlist) const {
    size_t lowZ  = 0;
    if(PVlist.empty()) {
        lowZ=std::numeric_limits<std::size_t>::max();
        return lowZ;
    }
    size_t size = PVlist.size();
    double lowA0zcalc = fabs(m_v0Tools->a0z       (Obj.vtx(), PVlist[0]));
    for(size_t i =1; i<size; i++) {
        double a0z =  fabs(m_v0Tools->a0z(Obj.vtx(), PVlist[i]));
        if(a0z < lowA0zcalc) {
            lowA0zcalc = a0z;
            lowZ =i;
        }
    }
    return lowZ;
}

void DerivationFramework::BPhysPVTools::DecorateWithDummyVertex(xAOD::VertexContainer* vtxContainer, 
       const xAOD::VertexContainer* pvContainer, const xAOD::Vertex* Dummy,
           int DoVertexType, const bool SetOrignal) const {
    const bool doPt = (DoVertexType & 1) != 0;
    const bool doA0 = (DoVertexType & 2) != 0;
    const bool doZ0 = (DoVertexType & 4) != 0;

    xAOD::VertexContainer::iterator vtxItr = vtxContainer->begin();
    for(; vtxItr!=vtxContainer->end(); ++vtxItr) {
        xAOD::BPhysHelper vtx(*vtxItr);

        // 1) pT error
        double ptErr = m_v0Tools->pTError( vtx.vtx() );
        BPHYS_CHECK( vtx.setPtErr(ptErr) );
        if(doPt) {
             // 2.a) the first PV with the largest sum pT.
             FillBPhysHelper(vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
             if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
        }

        if(doA0) {
             // 2.b) the closest in 3D:
             FillBPhysHelper(vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
             if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0);
        }

        if(doZ0) {
             FillBPhysHelper(vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
             if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0);
        }
    }    
}

void DerivationFramework::BPhysPVTools::DecorateWithNULL(xAOD::VertexContainer* vtxContainer,
        const xAOD::VertexContainer* pvContainer, int DoVertexType) const {
    const bool doPt = (DoVertexType & 1) != 0;
    const bool doA0 = (DoVertexType & 2) != 0;
    const bool doZ0 = (DoVertexType & 4) != 0;
    xAOD::VertexContainer::iterator vtxItr = vtxContainer->begin();
    for(; vtxItr!=vtxContainer->end(); ++vtxItr) {
        xAOD::BPhysHelper vtx(*vtxItr);

        // 1) pT error
        double ptErr = m_v0Tools->pTError( vtx.vtx() );
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
    }
}

StatusCode DerivationFramework::BPhysPVTools::FillCandExistingVertices(xAOD::VertexContainer* vtxContainer,
      const xAOD::VertexContainer* pvContainer, int DoVertexType) {
    //----------------------------------------------------
    // decorate the vertex
    //----------------------------------------------------
    // loop over candidates
    const std::vector<const xAOD::Vertex*> GoodPVs = GetGoodPV(pvContainer);


    if(GoodPVs.empty() == false) {

        const bool doPt = (DoVertexType & 1) != 0;
        const bool doA0 = (DoVertexType & 2) != 0;
        const bool doZ0 = (DoVertexType & 4) != 0;
        xAOD::VertexContainer::iterator vtxItr = vtxContainer->begin();
        for(; vtxItr!=vtxContainer->end(); ++vtxItr) {
            xAOD::BPhysHelper vtx(*vtxItr);

            // 1) pT error
            double ptErr = m_v0Tools->pTError( vtx.vtx() );
            BPHYS_CHECK( vtx.setPtErr(ptErr) );

            // 2) refit the primary vertex and set the related decorations.
            if(doPt) {
                size_t highPtindex = FindHighPtIndex(GoodPVs); //Should be 0 in PV ordering
                // 2.a) the first PV with the largest sum pT.
                FillBPhysHelper(vtx, GoodPVs[highPtindex], pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
            }

            if(doA0) {
                // 2.b) the closest in 3D:
                size_t lowA0 =  FindLowA0Index(vtx, GoodPVs);
                FillBPhysHelper(vtx, GoodPVs[lowA0], pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
            }

            if(doZ0) {
                size_t lowZ  = FindLowZIndex(vtx, GoodPVs);
                FillBPhysHelper(vtx, GoodPVs[lowZ], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
            }

        }// end of loop over vertices
    } //end of check for vertices
    else {
//        cout << "Warning: DerivationFramework::BPhysPVTools::FillCandExistingVertices No Primary Vertices Found trying to decorate wilth dummy \n";
        if(pvContainer->empty()) return StatusCode::FAILURE;
        const xAOD::Vertex* dummy = pvContainer->at(0);  //No good vertices so last vertex must be dummy
        DecorateWithDummyVertex(vtxContainer, pvContainer, dummy, DoVertexType, false);
    }
    return StatusCode::SUCCESS;
}


StatusCode DerivationFramework::BPhysPVTools::FillCandwithRefittedVertices(xAOD::VertexContainer* vtxContainer,
     const xAOD::VertexContainer* pvContainer, xAOD::VertexContainer* refPvContainer,
      Analysis::PrimaryVertexRefitter *pvRefitter, size_t in_PV_max, int DoVertexType) {


    //----------------------------------------------------
    // decorate the vertex
    //----------------------------------------------------
    // loop over candidates

    std::vector<const xAOD::Vertex*> goodPrimaryVertices= GetGoodPV(pvContainer);
    if(goodPrimaryVertices.empty() == false) {

        size_t pVmax =std::min(in_PV_max, goodPrimaryVertices.size());
        std::vector<const xAOD::Vertex*> refPVvertexes;
        std::vector<const xAOD::Vertex*> refPVvertexes_toDelete;
        std::vector<int> exitCode;
        refPVvertexes.reserve(pVmax);
        refPVvertexes_toDelete.reserve(pVmax);
        exitCode.reserve(pVmax);

        bool doPt = (DoVertexType & 1) != 0;
        bool doA0 = (DoVertexType & 2) != 0;
        bool doZ0 = (DoVertexType & 4) != 0;

        xAOD::VertexContainer::iterator vtxItr = vtxContainer->begin();
        for(; vtxItr!=vtxContainer->end(); ++vtxItr) {
            xAOD::BPhysHelper vtx(*vtxItr);

            // 1) pT error
            double ptErr = m_v0Tools->pTError( vtx.vtx() );
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
            size_t lowA0 = doA0 ? FindLowA0Index(vtx, refPVvertexes) : 9999998;
            size_t lowZ  = doZ0 ? FindLowZIndex(vtx, refPVvertexes) : 9999997;

            if(doPt) {
                //Choose old PV container if not refitted
                const xAOD::VertexContainer* ParentContainer =
                    (refPVvertexes_toDelete.at(highPtindex)) ? refPvContainer : pvContainer; 
                if(ParentContainer == refPvContainer) //if refitted add to refitted container
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(highPtindex))); // store the new vertex
              
                FillBPhysHelper(vtx, refPVvertexes[highPtindex],
                     ParentContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, exitCode[highPtindex]);
                vtx.setOrigPv(goodPrimaryVertices[highPtindex], pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
            }

            if(doA0) {
                const xAOD::VertexContainer* ParentContainer = 
                     (refPVvertexes_toDelete.at(lowA0)) ? refPvContainer : pvContainer; 
                if(ParentContainer == refPvContainer && highPtindex!=lowA0)
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(lowA0))); // store the new vertex
                
                FillBPhysHelper(vtx, refPVvertexes[lowA0],
                      ParentContainer, xAOD::BPhysHelper::PV_MIN_A0, exitCode[lowA0]);
                vtx.setOrigPv(goodPrimaryVertices[lowA0], pvContainer, xAOD::BPhysHelper::PV_MIN_A0);
            }


            if(doZ0) {

                const xAOD::VertexContainer* ParentContainer =
                    (refPVvertexes_toDelete.at(lowZ)) ? refPvContainer : pvContainer;
                if(ParentContainer == refPvContainer && highPtindex!=lowZ && lowZ!=lowA0)
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(lowZ))); // store the new vertex
                
                FillBPhysHelper(vtx, refPVvertexes[lowZ], 
                      ParentContainer, xAOD::BPhysHelper::PV_MIN_Z0, exitCode[lowZ]);
                vtx.setOrigPv(goodPrimaryVertices[lowZ], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0);
            }

// 2.c) the closest in Z:
            //nullify ptrs we want to keep so these won't get deleted
            //"delete null" is valid in C++ and does nothing so this is quicker than a lot of if statements
            if(doPt) refPVvertexes_toDelete[highPtindex] = nullptr;
            if(doA0) refPVvertexes_toDelete[lowA0] = nullptr;
            if(lowZ) refPVvertexes_toDelete[lowZ] = nullptr;
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
//        cout << "Warning: DerivationFramework::BPhysPVTools::FillCandwithRefittedVertices No Primary Vertices Found trying to decorate wilth dummy \n";
        if(pvContainer->empty()) return StatusCode::FAILURE;
        const xAOD::Vertex* dummy = pvContainer->at(0);  //No good vertices so last vertex must be dummy
        DecorateWithDummyVertex(vtxContainer, pvContainer, dummy, DoVertexType, true);
    }

    return StatusCode::SUCCESS;
}

size_t DerivationFramework::BPhysPVTools::FindHighPtIndex(const std::vector<const xAOD::Vertex*> &PVlist) {
    // it SHOULD be the first one in the collection but it shouldn't take long to do a quick check
    for(size_t i =0; i<PVlist.size(); i++) {
        if(PVlist[i]->vertexType() == xAOD::VxType::PriVtx) return i;
    }
    cout << "FATAL ERROR High Pt Primary vertex not found - this should not happen\n";
    return std::numeric_limits<std::size_t>::max(); //This should not happen
}

size_t DerivationFramework::BPhysPVTools::FindLowA0Index(const xAOD::BPhysHelper &Obj,
       const std::vector<const xAOD::Vertex*> &PVlist) const {
    size_t lowA0  = 0;
    if(PVlist.empty()) {
        lowA0=std::numeric_limits<std::size_t>::max();
        return lowA0;
    }
    size_t size = PVlist.size();
    double lowA0calc  = m_v0Tools->a0(Obj.vtx(), PVlist[0]);
    for(size_t i =1; i<size; i++) {
        double a0 =  m_v0Tools->a0(Obj.vtx(), PVlist[i]);
        if(a0 < lowA0calc) {
            lowA0calc = a0;
            lowA0 =i;
        }
    }
    return lowA0;
}

vector<const xAOD::Vertex*> DerivationFramework::BPhysPVTools::GetGoodPV(const xAOD::VertexContainer* pvContainer) {
    typedef xAOD::VxType::VertexType VertexType;
    VertexType Pvtx = xAOD::VxType::PriVtx;
    VertexType Pileupvtx = xAOD::VxType::PileUp;
    std::vector<const xAOD::Vertex*> goodPrimaryVertices;
    goodPrimaryVertices.reserve(pvContainer->size());

    for(auto ptr = pvContainer->begin(); ptr!= pvContainer->end(); ++ptr) {
        VertexType thistype = (*ptr)->vertexType();
        if(thistype == Pileupvtx || thistype == Pvtx)
            goodPrimaryVertices.push_back(*ptr);
        else {
//             cout << "vertex type  " << thistype << endl;
        }
    }
    return goodPrimaryVertices;
}

