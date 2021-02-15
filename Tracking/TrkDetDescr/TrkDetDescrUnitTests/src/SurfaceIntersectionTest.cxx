/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IntersectionTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg includes
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
// Trk includes
#include "TrkDetDescrUnitTests/SurfaceIntersectionTest.h"
#include "TrkValidationUtils/SurfaceNtupleBranch.h"
#include "TrkValidationUtils/VectorNtupleBranch.h"
#include "TrkSurfaces/Surface.h"
//Root includes
#include "TTree.h"
#include "TString.h"
// Gaudi
#include "GaudiKernel/ITHistSvc.h"

Trk::SurfaceIntersectionTest::SurfaceIntersectionTest(const std::string& name, ISvcLocator* pSvcLocator) :
 Trk::TrkDetDescrUnitTestBase(name, pSvcLocator),
 m_surfaceBuilder(),
 m_testsPerSurface(100),
 m_eventCounter(0),
 m_surfaces(6,0),
 m_surfaceAttempts(6,0),
 m_surfaceSuccessful(6,0),
 m_surfaceNames(6,""),
 m_surfaceNamesSet(6,false),
 m_writeTTree(true),
 m_treeFolder("/val/")
{
    declareProperty("SurfaceBuilder",           m_surfaceBuilder);
    declareProperty("NumberOfTestsPerSurface",  m_testsPerSurface);
    declareProperty("WriteTTree",               m_writeTTree);
    
}

StatusCode Trk::SurfaceIntersectionTest::finalize() {

    ATH_MSG_INFO("Test statistics output ---------------------- (start) -------------");     
    for (size_t it = 0; it < m_surfaces.size(); ++it){
        if (m_surfaceNamesSet[it]){
            ATH_MSG_INFO("Tested " << m_surfaces[it] << " created "<< m_surfaceNames[it] << " objects.");
            ATH_MSG_INFO("  -> Result [ attempts / successful (percentage)] : " 
                << m_surfaceAttempts[it] << " / " << m_surfaceSuccessful[it] 
                << " ( " << double(m_surfaceSuccessful[it])/m_surfaceAttempts[it] << " )" );
        }
    }
    ATH_MSG_INFO("Test statistics output ----------------------- (end) --------------");     

    return Trk::TrkDetDescrUnitTestBase::finalize();   
}

StatusCode Trk::SurfaceIntersectionTest::initializeTest() 
{
    if ( m_surfaceBuilder.retrieve().isFailure() ){
        ATH_MSG_FATAL("Could not retrieve surface builder. Aborting.");
        return StatusCode::FAILURE;
    }
    // success return
    return StatusCode::SUCCESS;    
}




StatusCode Trk::SurfaceIntersectionTest::runTest()
{
    ATH_MSG_VERBOSE("Running the SurfaceIntersectionTest Test");
    
    // ----------------- creation of the surfaces & track parameters -------------
    for (size_t it = 0; it < TrkDetDescrUnitTestBase::m_numTests; ++it ){
        
        // get a surface from the surface builder
        const Trk::Surface* sf = m_surfaceBuilder->surface();
        if (!sf) continue;
        
        // get the type
        Trk::Surface::SurfaceType sType = sf->type();
        ++m_surfaces[sType];
        
        ATH_MSG_VERBOSE("Surface of type " << sf->name() << " at postion " << Amg::toString(sf->center()));
        if (!m_surfaceNamesSet[sType]) {
            m_surfaceNames[sType] = sf->name();
            m_surfaceNamesSet[sType]   = true;
        } 
        // create the Tree for this TrackingVolume
        TString treeName = "IntersectionTest_Event";
                treeName += m_eventCounter;        
        TTree* currentTree = new TTree(treeName.Data(),"Intersection");
                treeName += "_Info";        
        TTree* currentInfoTree = new TTree(treeName.Data(),"Surface Information");
        
        ITHistSvc* tHistSvc = nullptr;
        if (m_writeTTree && service("THistSvc",tHistSvc).isFailure()) {
            ATH_MSG_FATAL( "initialize() Could not find Hist Service! Aborting." );
            return StatusCode::FAILURE;
        }
        // create the name under which the tree is registered
        TString fullTreeName  = m_treeFolder+"Intersection_Event";
                fullTreeName += m_eventCounter;
                fullTreeName += "_Number";
                fullTreeName += it;
                fullTreeName += "_"+sf->name();
                
        if (m_writeTTree && tHistSvc->regTree(fullTreeName.Data(), currentTree).isFailure()) {
            ATH_MSG_ERROR( "initialize() Could not register the validation Tree! Aborting." );
            delete currentTree;
            return StatusCode::FAILURE;
        }
        
        TString fullInfoTreeName  = m_treeFolder+"SurfaceInfo_Event";
                fullInfoTreeName += m_eventCounter;
                fullInfoTreeName += "_Number";
                fullInfoTreeName += it;
                fullInfoTreeName += "_"+sf->name();
       if (m_writeTTree && tHistSvc->regTree(fullInfoTreeName.Data(), currentInfoTree).isFailure()) {
           ATH_MSG_ERROR( "initialize() Could not register the validation Tree! Aborting." );
           delete currentInfoTree;
           return StatusCode::FAILURE;
       }                
         
        // the ntuple branches
        Trk::SurfaceNtupleBranch            surfaceBranch;
        surfaceBranch.initForWrite(*currentInfoTree);
        surfaceBranch.fill(*sf);
        
        Trk::VectorNtupleBranch             intersectionBranch;
        intersectionBranch.initForWrite(*currentTree,"Intersection_",3);

        if (sf){
            for (size_t ist = 0; ist < m_testsPerSurface; ++ist){
            // starting around the perigee
            Amg::Vector3D startPosition;
            startPosition.x() = TrkDetDescrUnitTestBase::m_gaussDist->shoot()* 50.;
            startPosition.y() = TrkDetDescrUnitTestBase::m_gaussDist->shoot()* 50.;
            startPosition.z() = TrkDetDescrUnitTestBase::m_gaussDist->shoot()* 150.;
            // aiming towards the surface center
            const Amg::Vector3D& sfCenter  = sf->center();
            Amg::Vector3D direction = sf->center();
            direction.x() = sfCenter.x()*(1.+0.25*TrkDetDescrUnitTestBase::m_gaussDist->shoot());
            direction.y() = sfCenter.y()*(1.+0.25*TrkDetDescrUnitTestBase::m_gaussDist->shoot());
            direction.z() = sfCenter.z()*(1.+0.25*TrkDetDescrUnitTestBase::m_gaussDist->shoot());
            direction = direction.normalized();
            ATH_MSG_VERBOSE(" -> Line: " << Amg::toString(startPosition) << " + t * " << Amg::toString(direction));
            
            // now doing the distance estimation
            Trk::Intersection intersection = sf->straightLineIntersection(startPosition,direction,false);
            ++m_surfaceAttempts[sType];
            // now check if the intesection is valid or not
            if (intersection.valid){
                ++m_surfaceSuccessful[sType];
                Amg::Vector3D consistency = (intersection.position-intersection.pathLength*direction) - startPosition;
                if (m_writeTTree){
                    intersectionBranch.fill(intersection.position);
                    currentTree->Fill();
                }    
                ATH_MSG_VERBOSE( "  -o Intersection  at " << Amg::toString(intersection.position) );
                ATH_MSG_VERBOSE( "  -x Cross-checked at " << Amg::toString(consistency) );
              }
            }
            // write the tree
            if (m_writeTTree) currentInfoTree->Fill();
            delete sf;
        }
    }
    ++m_eventCounter;
    return StatusCode::SUCCESS;
}


