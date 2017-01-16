/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
// TrueTracksNtupleTool.cxx
//   Source file for class TrueTracksNtupleTool
//////////////////////////////////////////////////////
// (c) ATLAS Detector software
//////////////////////////////////////////////////////
// Tatjana.Lenz@cern.ch
//////////////////////////////////////////////////////

//Gaudi
#include "GaudiKernel/ITHistSvc.h" 
#include "TTree.h"

// Trk
#include "TrkVertexFitterValidationTools/TrueTracksNtupleTool.h" 
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTruthData/TrackTruthCollection.h"
//CLHEP
#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

// constructor
Trk::TrueTracksNtupleTool::TrueTracksNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_ntupleFileName("/NTUPLES"),
        m_ntupleDirName("VtxFitterValidation"),
        m_ntupleTreeName("TrueTracks"),
        tree(nullptr),
        m_prod_x(nullptr),
        m_prod_y(nullptr),
        m_prod_z(nullptr),
        m_particle_id(nullptr),
        m_parent_id(nullptr),
        m_num_trks{}
{
    declareInterface<ITrueTracksValidationNtupleTool>(this);
    // Declare the properties
    declareProperty("NtupleFileName",			 m_ntupleFileName,	"Ntuple file handle");
    declareProperty("NtupleDirectoryName", m_ntupleDirName,	"directory name for ntuple tree");
    declareProperty("NtupleTreeName",			 m_ntupleTreeName,	"Name of the track ntuple tree");
}

// destructor
Trk::TrueTracksNtupleTool::~TrueTracksNtupleTool() {}

//////////////////
/// initialize
////////////////

StatusCode Trk::TrueTracksNtupleTool::initialize() {

    // Retrieve the Histo Service
    ITHistSvc* hist_svc;

    StatusCode status;

    status=service("THistSvc",hist_svc);
    if(status.isFailure())
    {
	     msg(MSG::ERROR) << "Could not find HistService" << endmsg;
	     return status;
    }

    //registering the true tracks tree
    tree = new TTree(TString(m_ntupleTreeName), "True tracks info output");
    std::string fullNtupleName = m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
    status = hist_svc->regTree(fullNtupleName, tree);
    if (status.isFailure()) {
	      msg (MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endmsg;
	      return status;
    }

    m_prod_x = new std::vector<float>();
    m_prod_y= new std::vector<float>();
    m_prod_z = new std::vector<float>();

    m_particle_id = new std::vector<int>();
    m_parent_id = new std::vector<int>();

    tree->Branch ("track_ prod_x", &m_prod_x);
    tree->Branch ("track_prod_y", &m_prod_y);
    tree->Branch ("track_prod_z", &m_prod_z);

    tree->Branch ("track_particle_id", &m_particle_id);
    tree->Branch ("track_parent_id", &m_parent_id);

    tree->Branch ("track_num_tracks", &m_num_trks, "numTrks/I");


    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::TrueTracksNtupleTool::finalize() {

    msg(MSG::DEBUG) << "start finalize() in " << name() << endmsg;
    return StatusCode::SUCCESS;
}

/////////////////////////////////////
/// fill VxCandidateData
////////////////////////////////////

StatusCode Trk::TrueTracksNtupleTool::fillTrueTracksInfo(const TrackCollection& trk_coll, const TrackTruthCollection& trk_true_coll) const {

    if (msgLvl(MSG::DEBUG)) msg (MSG::DEBUG) << "in fillTrueTracksInfo(vxTrackAtVertex)"  << endmsg;

    StatusCode status;
   //fill track info
   m_num_trks = trk_coll.size();
   if (m_num_trks != 0) {
      DataVector<Trk::Track>::const_iterator itr =  trk_coll.begin();
      for (; itr != trk_coll.end(); ++itr)
      {
         ElementLink<TrackCollection> tracklink;
         tracklink.setElement(const_cast<Trk::Track*>(*itr));
         tracklink.setStorableObject(trk_coll);
         const ElementLink<TrackCollection> tracklink2=tracklink;
         TrackTruthCollection::const_iterator found = trk_true_coll.find(tracklink2);
         if (found == trk_true_coll.end()) {
            if (msgLvl(MSG::DEBUG)) msg (MSG::DEBUG) << " truth is missing" << endmsg;
         } else {  
             TrackTruth trk_truth=found->second;
             const HepMC::GenParticle * particle;
             particle = trk_truth.particleLink();
             HepMC:: GenVertex* prod_vtx = particle->production_vertex();
             //fill prod vertex
             m_prod_x->push_back(prod_vtx->position().x());
             m_prod_y->push_back(prod_vtx->position().y());
             m_prod_z->push_back(prod_vtx->position().z());

             HepMC::GenVertex::particle_iterator   parent_iter = prod_vtx->particles_begin(HepMC::parents);
             m_parent_id->push_back((*parent_iter)->pdg_id());
             m_particle_id->push_back(particle->pdg_id());
            }

       }//end loop over tracks
   } else {
     if (msgLvl(MSG::DEBUG)) msg (MSG::DEBUG) << "No tracks in the Track Collection!!" << endmsg;
   }
   
   tree->Fill();

   m_prod_x->clear();
   m_prod_y->clear();
   m_prod_z->clear();

   m_particle_id->clear();
   m_parent_id->clear();

   return StatusCode::SUCCESS;
}
