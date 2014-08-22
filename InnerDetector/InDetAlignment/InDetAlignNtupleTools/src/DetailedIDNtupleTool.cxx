/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkTrackSummary/TrackSummary.h"

//Perigee
#include "TrkParameters/TrackParameters.h"
//for Amg::error helper function:
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "xAODEventInfo/EventInfo.h"
#include "TrkVertexOnTrack/VertexOnTrack.h"

#include "InDetIdentifier/TRT_ID.h"

#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkAlignEvent/AlignTrack.h"
#include "TrkAlignEvent/AlignPar.h"
#include "TrkAlignEvent/Residual.h"

#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkTruthData/TrackTruth.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "InDetAlignNtupleTools/DetailedIDNtupleTool.h"
#include <memory>

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

#include <iostream>

using namespace std;

namespace InDet {

//________________________________________________________________________
DetailedIDNtupleTool::DetailedIDNtupleTool(const std::string& type, const std::string& name, const IInterface * parent)
	: AthAlgTool(type,name,parent)
	, m_trackSumTool("Trk::TrackSummaryTool/TrackSummaryTool", this)
        , m_alignModuleTool("")
        , m_truthToTrack("Trk::TruthToTrack/InDetTruthToTrack")
        , m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
        , m_idHelper(0)
        , m_storeNormalRefittedOnly(false)
        , m_storeConstrainedOnly(true)
        , m_storeTruth(true)
        , m_matchProbability(0.8)
        , m_file(0)
        , m_tree(0)
        , m_trackCollection("Tracks")
        , m_tracksTruthName("TrackTruthCollection")
        {

	declareInterface<Trk::IFillNtupleTool>(this);

	declareProperty("FileName",                     m_filename="IDAlign.root");
	declareProperty("FilePath",                     m_filepath="./");
        declareProperty("TrackCollection",              m_trackCollection);
        declareProperty("TracksTruthName",              m_tracksTruthName);
	declareProperty("TrackSummaryTool",             m_trackSumTool);
	declareProperty("AlignModuleTool",              m_alignModuleTool);
        declareProperty("MatchProbability",             m_matchProbability);
        declareProperty("StoreTruth"   ,                m_storeTruth);
        declareProperty("MatchProbability",             m_matchProbability);
	declareProperty("StoreNormalRefittedOnly",      m_storeNormalRefittedOnly);
        declareProperty("StoreConstrainedOnly",        m_storeConstrainedOnly);
}

//________________________________________________________________________
DetailedIDNtupleTool::~DetailedIDNtupleTool()
{
}

//________________________________________________________________________
StatusCode DetailedIDNtupleTool::initialize()
{
	ATH_MSG_DEBUG("initialize() of DetailedIDNtupleTool");
	ATH_MSG_DEBUG("creating file with name "<<m_filepath<<m_filename);

	if (!m_tree) initializeNtuple();




        // AG: init truthToTrack
        if (m_truthToTrack.retrieve().isFailure() ) {
	    msg(MSG::FATAL) << "Failed to retrieve tool " << m_truthToTrack << endreq;
	    return StatusCode::FAILURE;
	  }
	  ATH_MSG_INFO("Retrieved TruthToTrack Tool: " << m_truthToTrack);
	
        /* Retrieve extrapolator tool */
	if (m_extrapolator.retrieve().isFailure()) {
            msg(MSG::FATAL) << "Failed to retrieve tool "<<m_extrapolator<<endreq;
            return StatusCode::FAILURE;
         }
         ATH_MSG_INFO("Retrieved Extrapolator Tool " << m_extrapolator);

	// get AlignModuleTool
	if ( m_alignModuleTool.empty() || m_alignModuleTool.retrieve().isFailure() ) {
		msg(MSG::FATAL) << "Failed to retrieve tool " <<m_alignModuleTool<< endreq;
		return StatusCode::FAILURE;
	}
	ATH_MSG_INFO("Retrieved tool " << m_alignModuleTool);

	return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode DetailedIDNtupleTool::fillNtuple()
{
  int success = 1;
  if (m_file && m_file->IsOpen()) {
    m_file->cd();
    if (m_tree) 
      m_tree->Write();
  }
  return success>0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//________________________________________________________________________
StatusCode DetailedIDNtupleTool::finalize()
{
  ATH_MSG_DEBUG("finalize() of DetailedIDNtupleTool");

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
void DetailedIDNtupleTool::dumpTrack(int itrk, const Trk::AlignTrack * alignTrack)
{
	ATH_MSG_DEBUG("In dumpTrack()");
        Trk::AlignTrack::AlignTrackType type = alignTrack->type();	
   
        // if hope to dump all the tracks, should set:
	// m_storeNormalRefittedOnly = fasle, m_storeConstrainedOnly=false
	if( (m_storeNormalRefittedOnly && (type != Trk::AlignTrack::NormalRefitted)) ||
            (m_storeConstrainedOnly   && (type != Trk::AlignTrack::BeamspotConstrained)) ){
             return;
        }

	// get run and event numbers
	ATH_MSG_DEBUG("Retrieving event info.");
	const xAOD::EventInfo * eventInfo;
	if (evtStore()->retrieve(eventInfo).isFailure())
		msg(MSG::ERROR) << "Could not retrieve event info." << endreq;
	else
	{
		m_runNumber = eventInfo->runNumber();
		m_evtNumber = eventInfo->eventNumber();
	}

	// initialize variables
	m_d0 = m_z0 = m_phi0 = m_theta = m_qoverp = m_pt = m_eta = -999.;
        m_chi2 = m_chi2prob = -1e12;
        m_ndof = -999;
        m_xvtx = m_yvtx = m_zvtx = -999.;

        m_err_d0 = m_err_z0 = m_err_phi0 = m_err_theta = m_err_qoverp = -999.;
        m_toRef_d0 = m_toRef_z0 = m_toRef_phi0 = m_toRef_theta = m_toRef_qoverp = -999.;


        m_original_d0 = m_original_z0 = m_original_phi0 = m_original_theta = m_original_qoverp = m_original_pt = m_original_eta = -999.;
        m_original_chi2 = m_original_chi2prob = -1e12;
        m_original_ndof = -999;
        m_original_xvtx = m_original_yvtx = m_original_zvtx = -999.;

        m_original_err_d0 = m_original_err_z0 = m_original_err_phi0 = m_original_err_theta = m_original_err_qoverp = -999.;
        m_original_toRef_d0 = m_original_toRef_z0 = m_original_toRef_phi0 = m_original_toRef_theta = m_original_toRef_qoverp = -999.;

	m_truth_d0 = m_truth_z0 = m_truth_phi0 = m_truth_theta = m_truth_qoverp = m_truth_pt = m_truth_eta= -999.;
        m_truth_prod_x =  m_truth_prod_y = m_truth_prod_z = -999.;



        //const Trk::MeasuredPerigee * aMeasPer = dynamic_cast<const Trk::MeasuredPerigee*>(alignTrack->perigeeParameters());
        //const Trk::MeasuredPerigee* aMeasPer = perigeeParameter(alignTrack);
    const Trk::Perigee * aMeasPer = (alignTrack->perigeeParameters());

	if (aMeasPer==0){
		msg(MSG::ERROR) << "Could not get Trk::Perigee of the alignTrack" << endreq;
                return;
        } 
	else
	{
		m_d0 = aMeasPer->parameters()[Trk::d0];
		m_z0 = aMeasPer->parameters()[Trk::z0];
		m_phi0 = aMeasPer->parameters()[Trk::phi0];
		m_theta = aMeasPer->parameters()[Trk::theta];
		m_qoverp = aMeasPer->parameters()[Trk::qOverP];

/**
		const Trk::ErrorMatrix& locError = aMeasPer->localErrorMatrix();
	    m_err_d0      = locError.error(Trk::d0);
		m_err_z0      = locError.error(Trk::z0);
		m_err_phi0    = locError.error(Trk::phi0);
		m_err_theta   = locError.error(Trk::theta);
	    m_err_qoverp  = locError.error(Trk::qOverP);
**/

        const AmgSymMatrix(5) * locCov = aMeasPer->covariance();
        m_err_d0      = Amg::error(*locCov,Trk::d0);
		m_err_z0      = Amg::error(*locCov,Trk::z0);
		m_err_phi0    = Amg::error(*locCov,Trk::phi0);
		m_err_theta   = Amg::error(*locCov,Trk::theta);
	    m_err_qoverp  = Amg::error(*locCov,Trk::qOverP);
        
		m_pt = sqrt((aMeasPer->momentum().x())*(aMeasPer->momentum().x()) + (aMeasPer->momentum().y())*(aMeasPer->momentum().y()));
		m_eta = aMeasPer->eta();

		m_xvtx = aMeasPer->position().x();
		m_yvtx = aMeasPer->position().y();
		m_zvtx = aMeasPer->position().z();



		// get fit quality and chi2 probability of track
		const Trk::FitQuality * fitQual = alignTrack->fitQuality();
		if (fitQual==0)
			msg(MSG::ERROR) << "No fit quality assigned to the track" << endreq;
		else
		{
			if (fitQual->chiSquared() > 0. && fitQual->numberDoF() > 0)
			{
				m_chi2 = fitQual->chiSquared();
				m_ndof = fitQual->numberDoF();
				m_chi2prob = TMath::Prob(m_chi2,m_ndof);

				ATH_MSG_DEBUG("  - chi2             : " << m_chi2);
				ATH_MSG_DEBUG("  - ndof             : " << m_ndof);
				ATH_MSG_DEBUG("  - chi2/ndof        : " << m_chi2/(double)m_ndof);
				ATH_MSG_DEBUG("  - chi2 propability : " << m_chi2prob);

			}
		}
	}

        // HepGeom::Point3D<double> refPoint = (*alignTrack->trackStateOnSurfaces()->begin())->trackParameters()->associatedSurface()->center();
        // std::string type = (*alignTrack->trackStateOnSurfaces()->begin())->dumpType();

        const Trk::MeasurementBase* measbase = *(alignTrack->measurementsOnTrack()->begin());
        //HepGeom::Point3D<double> refPoint = measbase->associatedSurface().center();
        Amg::Vector3D refPoint = measbase->associatedSurface().center();
        const Trk::VertexOnTrack* vot = dynamic_cast<const Trk::VertexOnTrack*> (measbase);
        msg(MSG::DEBUG) <<" VoT get from the alignTrack:" << *vot << endreq;
        if(!vot) {
            msg(MSG::ERROR) << " Seems the pseudo-measuremnt in the alignTrack not exist!" << endreq;
            msg(MSG::ERROR) << " this pseudo-measurement has been rejected as outlier in the refitting!" << endreq;
            return;
        }

        msg(MSG::DEBUG)<<" the pseudo-measurement position: "<< refPoint << endreq;
        const Trk::Track* originalTrack = alignTrack->originalTrack();
  
        //const Trk::MeasuredPerigee * originalMeasPer = dynamic_cast<const Trk::MeasuredPerigee*>(originalTrack->perigeeParameters());
        const Trk::Perigee * originalMeasPer = originalTrack->perigeeParameters();
        if(!(originalTrack && originalMeasPer)){
	     msg(MSG::ERROR) << "No original track!" << endreq;
        } 
        else{

            m_original_d0     = originalMeasPer->parameters()[Trk::d0];
            m_original_z0     = originalMeasPer->parameters()[Trk::z0];
            m_original_phi0   = originalMeasPer->parameters()[Trk::phi0];
            m_original_theta  = originalMeasPer->parameters()[Trk::theta];
            m_original_qoverp = originalMeasPer->parameters()[Trk::qOverP];

            const AmgSymMatrix(5) * locError = originalMeasPer->covariance();
            m_original_err_d0      = Amg::error(*locError,Trk::d0);
            m_original_err_z0      = Amg::error(*locError,Trk::z0);
            m_original_err_phi0    = Amg::error(*locError,Trk::phi0);
            m_original_err_theta   = Amg::error(*locError,Trk::theta);
            m_original_err_qoverp  = Amg::error(*locError,Trk::qOverP);

            // here it seems the originalMeasPer->momentum() and originalMeasPer->eta() are zero!!!
            //m_original_pt = sqrt((originalMeasPer->momentum().x())*(originalMeasPer->momentum().x()) + (originalMeasPer->momentum().y())*(originalMeasPer->momentum().y()));
            //m_original_eta = originalMeasPer->eta();

	    m_original_pt = (1.0/m_original_qoverp)*sin(m_original_theta);
	    m_original_eta = -log(tan(m_original_theta/2));

            m_original_xvtx = originalMeasPer->position().x();
            m_original_yvtx = originalMeasPer->position().y();
            m_original_zvtx = originalMeasPer->position().z();

            // get fit quality and chi2 probability of original Track
            const Trk::FitQuality * originalFitQual = originalTrack->fitQuality();
            if (originalFitQual==0)
                msg(MSG::ERROR) << "No fit quality assigned to the track" << endreq;
            else
              {
               if (originalFitQual->chiSquared() > 0. && originalFitQual->numberDoF() > 0)
                  {
                   m_original_chi2 = originalFitQual->chiSquared();
                   m_original_ndof = originalFitQual->numberDoF();
                   m_original_chi2prob = TMath::Prob(m_original_chi2,m_original_ndof);

                   ATH_MSG_DEBUG("  - original chi2             : " << m_original_chi2);
                   ATH_MSG_DEBUG("  - original ndof             : " << m_original_ndof);
                   ATH_MSG_DEBUG("  - original chi2/ndof        : " << m_original_chi2/(double)m_original_ndof);
                   ATH_MSG_DEBUG("  - original chi2 propability : " << m_original_chi2prob);
                  }
              }


            const Trk::PerigeeSurface persf(refPoint); 
            const Trk::Perigee* originalPerigeeAtRef = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate(*originalTrack, persf));
	    if (!originalPerigeeAtRef){
             const Trk::Perigee * originalTrackPerigee = originalTrack->perigeeParameters();
	         if ( originalTrackPerigee && ((originalTrackPerigee->associatedSurface())) == persf )
                 {
                  msg(MSG::DEBUG) << "Perigee of Track is already expressed to given vertex, a copy is returned." << endreq;
                  originalPerigeeAtRef = originalTrackPerigee->clone();
                 } else
                     msg(MSG::DEBUG) << "Extrapolation to Perigee failed, NULL pointer is returned." << endreq;         
            }

            if(originalPerigeeAtRef){
              //std::auto_ptr<const Trk::Perigee > originalMeasPerAtRef(dynamic_cast<const Trk::MeasuredPerigee*>(originalPerigeeAtRef) );
              std::auto_ptr<const Trk::Perigee > originalMeasPerAtRef(originalPerigeeAtRef);
              m_original_toRef_d0     = originalMeasPerAtRef->parameters()[Trk::d0];
              m_original_toRef_z0     = originalMeasPerAtRef->parameters()[Trk::z0];
              m_original_toRef_phi0   = originalMeasPerAtRef->parameters()[Trk::phi0];
              m_original_toRef_theta  = originalMeasPerAtRef->parameters()[Trk::theta];
              m_original_toRef_qoverp = originalMeasPerAtRef->parameters()[Trk::qOverP];
            }
            
			//m_extrapolator->extrapolate returns const TrackParameters* or null if fails
			//see: https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkExtrapolation/TrkExInterfaces/trunk/TrkExInterfaces/IExtrapolator.h
			//SAR: Post-eigen, how much of this is still needed?
            const Trk::Perigee* PerigeeAtRef = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate(*alignTrack, persf));
            if (!PerigeeAtRef){
                 const Trk::Perigee* alignTrackPerigee = alignTrack->perigeeParameters();
                 if ( alignTrackPerigee && ((alignTrackPerigee->associatedSurface())) == persf )
                 {
                  msg(MSG::DEBUG) << "Perigee of AlignTrack is already expressed to given vertex, a copy is returned." << endreq;
                  PerigeeAtRef = alignTrackPerigee->clone();
                 } else
                     msg(MSG::DEBUG) << "Extrapolation to Perigee failed, NULL pointer is returned." << endreq;         
            }

			//post-eigen, can simply use the TrackParameters * returned by m_extrapolator->extrapolate?
            if(PerigeeAtRef){
              //std::auto_ptr<const Trk::MeasuredPerigee > MeasPerAtRef(dynamic_cast<const Trk::MeasuredPerigee*>( PerigeeAtRef) );
              std::auto_ptr<const Trk::Perigee > MeasPerAtRef(( PerigeeAtRef) );
              m_toRef_d0     = MeasPerAtRef->parameters()[Trk::d0];
              m_toRef_z0     = MeasPerAtRef->parameters()[Trk::z0];
              m_toRef_phi0   = MeasPerAtRef->parameters()[Trk::phi0];
              m_toRef_theta  = MeasPerAtRef->parameters()[Trk::theta];
              m_toRef_qoverp = MeasPerAtRef->parameters()[Trk::qOverP];
            }
        }

        /// if match truth fail, return directly
        if(m_storeTruth) 
           retrieveTruthInfo(alignTrack);


	m_file->cd();
	m_tree->Fill();
	ATH_MSG_DEBUG("tree filled");

	++itrk;
	return;
}



bool DetailedIDNtupleTool::retrieveTruthInfo(const Trk::AlignTrack * alignTrack)
{
  // although we select tracks using the TrackSelectionTool, we still need to get a complete TrackCollection
  // from StoreGate for use in the track-truth map, otherwise the track-truth matching is screwed up

  const TrackCollection * RecCollection = NULL;
  if (evtStore()->retrieve(RecCollection, m_trackCollection).isFailure()) {
      if (msgLvl(MSG::DEBUG)) msg() <<"Track collection \"" << m_trackCollection << "\" not found." << endreq;
          return false;
      }
  if (RecCollection)
      ATH_MSG_DEBUG("Retrieved "<<m_trackCollection<<" with size "<<RecCollection->size()<<" reconstructed tracks from storegate");
	

  // get TrackTruthCollection
  const TrackTruthCollection  * TruthMap  = NULL;
  if (StatusCode::SUCCESS != evtStore()->retrieve(TruthMap,m_tracksTruthName)) {
    if (msgLvl(MSG::DEBUG)) msg() << "Cannot find " << m_tracksTruthName  << endreq;
    return false;
  }

  ATH_MSG_DEBUG("Track Truth Collection with name "<<m_tracksTruthName<<" with size "<<TruthMap->size()<<" found in StoreGate");

  bool flag = false;
  // get fit quality and chi2 probability of track
  // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
  const Trk::Perigee* startPerigee = alignTrack->perigeeParameters();
  //const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee );
  const Trk::Perigee* measPer = startPerigee;

  if (measPer==0) {
     ATH_MSG_DEBUG("No measured perigee parameters assigned to the track");
     return false;
  }
//unused
  //const AmgVector(5)& perigeeParams = measPer->parameters();

  if (TruthMap) {
  
    ElementLink<TrackCollection> tracklink;
    tracklink.setElement(const_cast<Trk::Track *>(alignTrack->originalTrack()));
    tracklink.setStorableObject(*RecCollection);
    const ElementLink<TrackCollection> tracklink2=tracklink;

    TrackTruthCollection::const_iterator found = TruthMap->find(tracklink2);
    if ((found != TruthMap->end()) && (found->second.probability()>m_matchProbability)){

      TrackTruth trtruth = found->second;
      HepMcParticleLink HMPL = trtruth.particleLink();

      if ( HMPL.isValid()) {
        const HepMC::GenParticle *genparptr = HMPL.cptr();

        if (genparptr) {
          if (genparptr->production_vertex()) {

            if(genparptr->pdg_id() == 0) {
              //msg(MSG::WARNING) <<" Particle with PDG ID = 0! Status "<<genparptr->status()<<" mass "<< genparptr->momentum().m() <<" pt "<<genparptr->momentum().et()<<" eta "<<genparptr->momentum().eta()<<" phi "<<genparptr->momentum().phi()<<" Gen Vertex barcode "<<genparptr->production_vertex()->barcode()<<"Gen Vertex Position x" <<genparptr->production_vertex()->position().x()<< " y "<<genparptr->production_vertex()->position().y()<<" z "<<genparptr->production_vertex()->position().z()<<endreq;
            }
            else {
              const Trk::TrackParameters * generatedTrackPerigee = m_truthToTrack->makePerigeeParameters(genparptr);
              if (!generatedTrackPerigee)
                    msg(MSG::WARNING)<< "Unable to extrapolate genparticle to perigee!"<< endreq;
              else {

                flag = true;
                m_truth_qoverpt = generatedTrackPerigee->parameters()[Trk::qOverP]/sin(generatedTrackPerigee->parameters()[Trk::theta]);
                m_truth_qoverp  = generatedTrackPerigee->parameters()[Trk::qOverP];
                m_truth_phi0    = generatedTrackPerigee->parameters()[Trk::phi0];
                m_truth_d0      = generatedTrackPerigee->parameters()[Trk::d0];
                m_truth_z0      = generatedTrackPerigee->parameters()[Trk::z0];
                m_truth_theta   = generatedTrackPerigee->parameters()[Trk::theta];
                m_truth_eta     = generatedTrackPerigee->eta();
                m_truth_prod_x  = genparptr->production_vertex()->position().x();
                m_truth_prod_y  = genparptr->production_vertex()->position().y();
                m_truth_prod_z  = genparptr->production_vertex()->position().z();

                delete  generatedTrackPerigee;
                m_truth_pt     = 1./fabs(m_truth_qoverpt);
                m_truth_charge = 1;
                if (m_truth_qoverpt<0)
                    m_truth_charge = -1;
                if (m_truth_phi0<0)
                    m_truth_phi0 += 2*3.1415926;
                ATH_MSG_DEBUG("Found matched truth track with phi, PT = " << m_truth_phi0 << ", " << m_truth_pt);
              }
            }
          }
        }
      }
    }
  }

  return flag;
}



//const Trk::MeasuredPerigee* DetailedIDNtupleTool::perigeeParameter(const Trk::AlignTrack* alignTrack)const { 
//I think this is never used!
const Trk::TrackParameters* DetailedIDNtupleTool::perigeeParameter(const Trk::AlignTrack* alignTrack)const { 

    DataVector<const Trk::TrackStateOnSurface>::const_iterator it = alignTrack->trackStateOnSurfaces()->begin();
    for (;it != alignTrack->trackStateOnSurfaces()->end();++it){
          if( ((*it)->type(Trk::TrackStateOnSurface::Perigee)) )
              break;
    }
    if (it == alignTrack->trackStateOnSurfaces()->end()){
        msg(MSG::ERROR) << "Strange there is no perigee of the alignTrack" << endreq;
        return 0;
    }

    //const Trk::MeasuredPerigee * aMeasPer = dynamic_cast<const Trk::MeasuredPerigee*>((*it)->trackParameters());
	    const Trk::TrackParameters* aMeasPer = ((*it)->trackParameters());

    if (aMeasPer==0){
        msg(MSG::ERROR) << "Could not get Trk::MeasuredPerigee of the alignTrack" << endreq;
        return 0;
    }
    return aMeasPer;

}



void DetailedIDNtupleTool::storeHitmap()
{
}
 
//________________________________________________________________________
void DetailedIDNtupleTool::fillHitmap()
{
}
 
void DetailedIDNtupleTool::fillSummary()
{
}

//________________________________________________________________________
void DetailedIDNtupleTool::showStatistics()
{
}

//________________________________________________________________________
void DetailedIDNtupleTool::initializeNtuple()
{
        //m_file = new TFile((m_filepath+m_filename).c_str(), "RECREATE");
        m_file->cd();
	m_tree = new TTree("IDAlign", "Inner Detector Alignment Ntuple");
	
	m_tree->Branch("run",        &m_runNumber, "run/I");
	m_tree->Branch("evt",        &m_evtNumber, "evt/I");
	
	m_tree->Branch("xvtx",       &m_xvtx,      "xvtx/D");
	m_tree->Branch("yvtx",       &m_yvtx,      "yvtx/D");
	m_tree->Branch("zvtx",       &m_zvtx,      "zvtx/D");
	m_tree->Branch("d0",         &m_d0,        "d0/D");
	m_tree->Branch("z0",         &m_z0,        "z0/D");
	m_tree->Branch("phi0",       &m_phi0,      "phi0/D");
	m_tree->Branch("theta",      &m_theta,     "theta/D");
	m_tree->Branch("qoverp",     &m_qoverp,    "qoverp/D");
	m_tree->Branch("pt",         &m_pt,        "pt/D");
	m_tree->Branch("eta",        &m_eta,       "eta/D");
	m_tree->Branch("chi2",       &m_chi2,      "chi2/D");
	m_tree->Branch("ndof",       &m_ndof,      "ndof/I");
	m_tree->Branch("chi2prob",   &m_chi2prob,  "chi2prob/D");
        m_tree->Branch("err_d0",     &m_err_d0,    "err_d0/D");
        m_tree->Branch("err_z0",     &m_err_z0,    "err_z0/D");
        m_tree->Branch("err_phi0",   &m_err_phi0,  "err_phi0/D");
        m_tree->Branch("err_theta",  &m_err_theta, "err_theta/D");
        m_tree->Branch("err_qoverp", &m_err_qoverp,"err_qoverp/D");
	
        if(m_storeTruth){
	  
	        m_tree->Branch("m_truth_prod_x",   &m_truth_prod_x,    "truth_prod_x/D");
		m_tree->Branch("m_truth_prod_y",   &m_truth_prod_y,    "truth_prod_y/D");
		m_tree->Branch("m_truth_prod_z",   &m_truth_prod_z,    "truth_prod_z/D");
		m_tree->Branch("truth_d0",         &m_truth_d0,        "truth_d0/D");
		m_tree->Branch("truth_z0",         &m_truth_z0,        "truth_z0/D");
		m_tree->Branch("truth_phi0",       &m_truth_phi0,      "truth_phi0/D");
		m_tree->Branch("truth_theta",      &m_truth_theta,     "truth_theta/D");
		m_tree->Branch("truth_qoverp",     &m_truth_qoverp,    "truth_qoverp/D");
		m_tree->Branch("truth_pt",         &m_truth_pt,        "truth_pt/D");
		m_tree->Branch("truth_eta",        &m_truth_eta,       "truth_eta/D");
        }
	
        m_tree->Branch("original_xvtx",       &m_original_xvtx,      "original_xvtx/D");
        m_tree->Branch("original_yvtx",       &m_original_yvtx,      "original_yvtx/D");
        m_tree->Branch("original_zvtx",       &m_original_zvtx,      "original_zvtx/D");
        m_tree->Branch("original_d0",         &m_original_d0,        "original_d0/D");
        m_tree->Branch("original_z0",         &m_original_z0,        "original_z0/D");
        m_tree->Branch("original_phi0",       &m_original_phi0,      "original_phi0/D");
        m_tree->Branch("original_theta",      &m_original_theta,     "original_theta/D");
        m_tree->Branch("original_qoverp",     &m_original_qoverp,    "original_qoverp/D");
        m_tree->Branch("original_pt",         &m_original_pt,        "original_pt/D");
        m_tree->Branch("original_eta",        &m_original_eta,       "original_eta/D");
        m_tree->Branch("original_chi2",       &m_original_chi2,      "original_chi2/D");
        m_tree->Branch("original_ndof",       &m_original_ndof,      "original_ndof/I");
        m_tree->Branch("original_chi2prob",   &m_original_chi2prob,  "original_chi2prob/D");

        m_tree->Branch("original_err_d0",     &m_original_err_d0,    "original_err_d0/D");
        m_tree->Branch("original_err_z0",     &m_original_err_z0,    "original_err_z0/D");
        m_tree->Branch("original_err_phi0",   &m_original_err_phi0,  "original_err_phi0/D");
        m_tree->Branch("original_err_theta",  &m_original_err_theta, "original_err_theta/D");
        m_tree->Branch("original_err_qoverp", &m_original_err_qoverp,"original_err_qoverp/D");

        m_tree->Branch("toRef_d0",             &m_toRef_d0,            "toRef_d0/D");
        m_tree->Branch("toRef_z0",             &m_toRef_z0,            "toRef_z0/D");
        m_tree->Branch("toRef_phi0",           &m_toRef_phi0,          "toRef_phi0/D");
        m_tree->Branch("toRef_theta",          &m_toRef_theta,         "toRef_theta/D");
        m_tree->Branch("toRef_qoverp",         &m_toRef_qoverp,        "toRef_qoverp/D");
        m_tree->Branch("original_toRef_d0",    &m_original_toRef_d0,    "original_toRef_d0/D");
        m_tree->Branch("original_toRef_z0",    &m_original_toRef_z0,    "original_toRef_z0/D");
        m_tree->Branch("original_toRef_phi0",  &m_original_toRef_phi0,  "original_toRef_phi0/D");
        m_tree->Branch("original_toRef_theta", &m_original_toRef_theta, "original_toRef_theta/D");
        m_tree->Branch("original_toRef_qoverp",&m_original_toRef_qoverp,"original_toRef_qoverp/D");

	return;
}

} // end namespace
