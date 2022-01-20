/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// ****************************************************************************
// ----------------------------------------------------------------------------
// ReVertex
//
// Konstantin Beloborodov <Konstantin.Beloborodov@cern.ch>
//
// ----------------------------------------------------------------------------
// ****************************************************************************
#include "DerivationFrameworkBPhys/ReVertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "JpsiUpsilonTools/JpsiUpsilonCommon.h"

#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "xAODBPhys/BPhysHypoHelper.h"

using namespace DerivationFramework;

ReVertex::ReVertex(const std::string& t,
                   const std::string& n,
                   const IInterface* p) :
    AthAlgTool(t,n,p), m_vertexEstimator("InDet::VertexPointEstimator"), m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_massConst(0.),
    m_totalMassConst(0.),
    m_v0Tools("Trk::V0Tools"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_beamSpotSvc("BeamCondSvc",n),
    m_doMassConst(false),
    m_vertexFittingWithPV(false),
    m_chi2cut(-1.0),
    m_trkDeltaZ(-1.0),
    m_useAdditionalTrack(false)
{

    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TrackIndices", m_TrackIndices);
    declareProperty("TrkVertexFitterTool", m_iVertexFitter);
    declareProperty("VertexPointEstimator",m_vertexEstimator);

    declareProperty("OutputVtxContainerName", m_OutputContainerName);
    declareProperty("InputVtxContainerName", m_inputContainerName);
    declareProperty("TrackContainerName", m_trackContainer = "InDetTrackParticles");
    declareProperty("UseVertexFittingWithPV", m_vertexFittingWithPV);

    declareProperty("HypothesisNames",m_hypoNames);
   
    declareProperty("V0Tools"               , m_v0Tools);
    declareProperty("PVRefitter"            , m_pvRefitter);
    declareProperty("PVContainerName"       , m_pvContainerName        = "PrimaryVertices");
    declareProperty("RefPVContainerName"    , m_refPVContainerName     = "RefittedPrimaryVertices");

    declareProperty("UseMassConstraint", m_doMassConst);
    declareProperty("VertexMass", m_totalMassConst);
    declareProperty("SubVertexMass", m_massConst);
    declareProperty("MassInputParticles", m_trkMasses);
    declareProperty("SubVertexTrackIndices", m_indices);
   
    declareProperty("UseAdditionalTrack", m_useAdditionalTrack);

    declareProperty("RefitPV"               , m_refitPV                = false);
    //This parameter will allow us to optimize the number of PVs under consideration as the probability
    //of a useful primary vertex drops significantly the higher you go
    declareProperty("MaxPVrefit"            , m_PV_max                = 1000);
    declareProperty("DoVertexType"          , m_DoVertexType           = 7);
    // minimum number of tracks for PV to be considered for PV association
    declareProperty("MinNTracksInPV"        , m_PV_minNTracks          = 0);
    declareProperty("Do3d"        , m_do3d          = false);
    declareProperty("AddPVData"        , m_AddPVData          = true);
    declareProperty("StartingPoint0"        , m_startingpoint0     = false);
   declareProperty("BMassUpper",m_BMassUpper = std::numeric_limits<double>::max() );
   declareProperty("BMassLower",m_BMassLower = std::numeric_limits<double>::min() );
   declareProperty("Chi2Cut",m_chi2cut = std::numeric_limits<double>::max() );
    declareProperty("TrkDeltaZ",m_trkDeltaZ);
    
    
}

StatusCode ReVertex::initialize() {
    ATH_MSG_DEBUG("in initialize()");
    if(m_TrackIndices.empty()) {
        ATH_MSG_FATAL("No Indices provided");
        return StatusCode::FAILURE;
    }
    ATH_CHECK(m_iVertexFitter.retrieve());
    ATH_CHECK(m_v0Tools.retrieve());
    ATH_CHECK(m_pvRefitter.retrieve());
    ATH_CHECK(m_vertexEstimator.retrieve());
    ATH_CHECK(m_beamSpotSvc.retrieve());
    m_VKVFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iVertexFitter));
    return StatusCode::SUCCESS;
}


StatusCode ReVertex::addBranches() const {

    xAOD::VertexContainer    *vtxContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer	 *vtxAuxContainer = new xAOD::VertexAuxContainer();
    vtxContainer->setStore(vtxAuxContainer);

    ATH_CHECK(evtStore()->record(vtxContainer, m_OutputContainerName));
    ATH_CHECK(evtStore()->record(vtxAuxContainer, m_OutputContainerName+"Aux."));

    const size_t Ntracks = m_TrackIndices.size();

    const xAOD::VertexContainer    *InVtxContainer = nullptr;
    ATH_CHECK(evtStore()->retrieve(InVtxContainer, m_inputContainerName ));

    const xAOD::TrackParticleContainer* importedTrackCollection = nullptr;
    ATH_CHECK(evtStore()->retrieve(importedTrackCollection, m_trackContainer ));

    //----------------------------------------------------
    // retrieve primary vertices
    //----------------------------------------------------
    const xAOD::VertexContainer* pvContainer = nullptr;
    ATH_CHECK(evtStore()->retrieve(pvContainer, m_pvContainerName));

    std::vector<const xAOD::TrackParticle*> fitpair(Ntracks + m_useAdditionalTrack);
    for(const xAOD::Vertex* v : *InVtxContainer)
    {

      bool passed = false;
      for(size_t i=0;i<m_hypoNames.size();i++) {
	 xAOD::BPhysHypoHelper onia(m_hypoNames.at(i), v);
	 passed |= onia.pass();
      }
      if (!passed && m_hypoNames.size()) continue;
       
        for(size_t i =0; i<Ntracks; i++)
        {
            size_t trackN = m_TrackIndices[i];
            if(trackN >= v->nTrackParticles())
            {
                ATH_MSG_FATAL("Indices exceeds limit in particle");
                return StatusCode::FAILURE;
            }
            fitpair[i] = v->trackParticle(trackN);
        }

       if (m_useAdditionalTrack) 
       {
	  // Loop over ID tracks, call vertexing
	  for (auto trkItr=importedTrackCollection->cbegin(); trkItr!=importedTrackCollection->cend(); ++trkItr) {
	     const xAOD::TrackParticle* tp (*trkItr);
	     fitpair.back() = nullptr;
	     if (Analysis::JpsiUpsilonCommon::isContainedIn(tp,fitpair)) continue; // remove tracks which were used to build J/psi+2Tracks
	     fitpair.back() = tp;

	      // Daniel Scheirich: remove track too far from the Jpsi+2Tracks vertex (DeltaZ cut)
	      if(m_trkDeltaZ>0 &&
		 std::abs((tp)->z0() + (tp)->vz() - v->z()) > m_trkDeltaZ )
	       continue;
	     
	     fitAndStore(vtxContainer,v,InVtxContainer,fitpair,importedTrackCollection,pvContainer);
	  }
       }
       else 
       {
	  fitAndStore(vtxContainer,v,InVtxContainer,fitpair,importedTrackCollection,pvContainer);
       }
    }

    if(m_AddPVData){
     // Give the helper class the ptr to v0tools and beamSpotsSvc to use
     BPhysPVTools helper(&(*m_v0Tools), &m_beamSpotSvc);
     helper.SetMinNTracksInPV(m_PV_minNTracks);
     helper.SetSave3d(m_do3d);

    if(m_refitPV) {
        //----------------------------------------------------
        // Try to retrieve refitted primary vertices
        //----------------------------------------------------
        xAOD::VertexContainer*    refPvContainer    = nullptr;
        xAOD::VertexAuxContainer* refPvAuxContainer = nullptr;
        if(evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
          // refitted PV container exists. Get it from the store gate
          ATH_CHECK(evtStore()->retrieve(refPvContainer   , m_refPVContainerName       ));
          ATH_CHECK(evtStore()->retrieve(refPvAuxContainer, m_refPVContainerName + "Aux."));
        } else {
          // refitted PV container does not exist. Create a new one.
          refPvContainer = new xAOD::VertexContainer;
          refPvAuxContainer = new xAOD::VertexAuxContainer;
          refPvContainer->setStore(refPvAuxContainer);
          ATH_CHECK(evtStore()->record(refPvContainer   , m_refPVContainerName));
          ATH_CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
        }

        if(vtxContainer->size() >0){
          ATH_CHECK(helper.FillCandwithRefittedVertices(vtxContainer, pvContainer, refPvContainer, &(*m_pvRefitter) ,  m_PV_max, m_DoVertexType));
        }
     }else{
         if(vtxContainer->size() >0) ATH_CHECK(helper.FillCandExistingVertices(vtxContainer, pvContainer, m_DoVertexType));
     }
    }

    return StatusCode::SUCCESS;
}

void ReVertex::fitAndStore(xAOD::VertexContainer* vtxContainer,
				    const xAOD::Vertex* v,
				    const xAOD::VertexContainer    *InVtxContainer,
				    const std::vector<const xAOD::TrackParticle*> &inputTracks,
				    const xAOD::TrackParticleContainer* importedTrackCollection,
				    const xAOD::VertexContainer* pvContainer) const
{
   std::unique_ptr<xAOD::Vertex> ptr(fit(inputTracks, importedTrackCollection, nullptr));
   if(!ptr)return;

   double chi2DOF = ptr->chiSquared()/ptr->numberDoF();
   ATH_MSG_DEBUG("Candidate chi2/DOF is " << chi2DOF);
   bool chi2CutPassed = (m_chi2cut <= 0.0 || chi2DOF < m_chi2cut);
   if(!chi2CutPassed) { ATH_MSG_DEBUG("Chi Cut failed!");  return; }
   xAOD::BPhysHelper bHelper(ptr.get());//"get" does not "release" still automatically deleted
   bHelper.setRefTrks();
   if (m_trkMasses.size()==inputTracks.size()) {
      TLorentzVector bMomentum = bHelper.totalP(m_trkMasses);
      double bMass = bMomentum.M();
      bool passesCuts = (m_BMassUpper > bMass && bMass > m_BMassLower);
      if(!passesCuts)return;
   }
	  
   DerivationFramework::BPhysPVTools::PrepareVertexLinks( ptr.get(), importedTrackCollection );
   std::vector<const xAOD::Vertex*> thePreceding;
   thePreceding.push_back(v);
   if(m_vertexFittingWithPV){
      //
      Analysis::CleanUpVertex closestRefPV = Analysis::JpsiUpsilonCommon::ClosestRefPV(bHelper, pvContainer, &(*m_pvRefitter));
      if (!closestRefPV.get()) return;
      std::unique_ptr<xAOD::Vertex> ptrPV(fit(inputTracks, importedTrackCollection, closestRefPV.get()));
      if(!ptrPV) return;

      double chi2DOFPV = ptrPV->chiSquared()/ptrPV->numberDoF();
      ATH_MSG_DEBUG("CandidatePV chi2/DOF is " << chi2DOFPV);
      bool chi2CutPassed = (m_chi2cut <= 0.0 || chi2DOFPV < m_chi2cut);
      if(!chi2CutPassed) { ATH_MSG_DEBUG("Chi Cut failed!");  return; }
      xAOD::BPhysHelper bHelperPV(ptrPV.get());//"get" does not "release" still automatically deleted
      bHelperPV.setRefTrks();
      if (m_trkMasses.size()==inputTracks.size()) {
	 TLorentzVector bMomentumPV = bHelperPV.totalP(m_trkMasses);
	 double bMass = bMomentumPV.M();
	 bool passesCuts = (m_BMassUpper > bMass && bMass > m_BMassLower);
	 if(!passesCuts)return;
      }
      
      bHelperPV.setPrecedingVertices(thePreceding, InVtxContainer);
      vtxContainer->push_back(ptrPV.release());
      return; //Don't store other vertex
   }
   bHelper.setPrecedingVertices(thePreceding, InVtxContainer);
   vtxContainer->push_back(ptr.release());
}

    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // fit - does the fit
    // ---------------------------------------------------------------------------------
    
xAOD::Vertex* ReVertex::fit(const std::vector<const xAOD::TrackParticle*> &inputTracks,
			    const xAOD::TrackParticleContainer* importedTrackCollection,
			    const xAOD::Vertex* pv) const
{
   m_VKVFitter->setDefault();

   if (m_doMassConst && (m_trkMasses.size()==inputTracks.size())) {
      m_VKVFitter->setMassInputParticles(m_trkMasses);
      if (m_totalMassConst) m_VKVFitter->setMassForConstraint(m_totalMassConst);
      if (m_massConst) m_VKVFitter->setMassForConstraint(m_massConst, m_indices);
   }
   if (pv) {
      m_VKVFitter->setCnstType(8);
      m_VKVFitter->setVertexForConstraint(pv->position().x(),
					  pv->position().y(),
					  pv->position().z());
      m_VKVFitter->setCovVrtForConstraint(pv->covariancePosition()(Trk::x,Trk::x),
					  pv->covariancePosition()(Trk::y,Trk::x),
					  pv->covariancePosition()(Trk::y,Trk::y),
					  pv->covariancePosition()(Trk::z,Trk::x),
					  pv->covariancePosition()(Trk::z,Trk::y),
					  pv->covariancePosition()(Trk::z,Trk::z) );
   }

   // Do the fit itself.......
   // Starting point (use the J/psi position)
   const Trk::Perigee& aPerigee1 = inputTracks[0]->perigeeParameters();
   const Trk::Perigee& aPerigee2 = inputTracks[1]->perigeeParameters();
   int sflag = 0;
   int errorcode = 0;
   Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1,&aPerigee2,sflag,errorcode);
   if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
   xAOD::Vertex* theResult = m_VKVFitter->fit(inputTracks, startingPoint);

   // Added by ASC
   if(theResult != 0){
      std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
      for(unsigned int i=0; i< theResult->trackParticleLinks().size(); i++)
	{
	   ElementLink<DataVector<xAOD::TrackParticle> > mylink=theResult->trackParticleLinks()[i]; //makes a copy (non-const)
	   mylink.setStorableObject( *importedTrackCollection, true);
	   newLinkVector.push_back( mylink );
	}
      theResult->clearTracks();
      theResult->setTrackParticleLinks( newLinkVector );
   }
   
   return theResult;
}
