/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
//   McEventNtupleTool.cxx
//   Source file for class McEventNtupleTool
//////////////////////////////////////////////////////
// (c) ATLAS Detector software
//////////////////////////////////////////////////////
// Tatjana.Lenz@cern.ch
//////////////////////////////////////////////////////

//Gaudi
#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

// Trk
#include "TrkVertexFitterValidationTools/McEventNtupleTool.h"

//CLHEP
#include "CLHEP/Vector/LorentzVector.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

// constructor
Trk::McEventNtupleTool::McEventNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_ntupleFileName("/NTUPLES"),
        m_ntupleDirName("VtxFitterValidation"),
        m_ntupleMcTreeName("MC_Vertex"),
        m_ptCut(500.),
        m_etaCut(2.5),
        m_radiusCut(50.5),
        m_zPosCut(150.),
        m_radiusRes(0.044),
        m_zPosRes(0.221),
        m_vtx_tree(nullptr),
        m_quark_tree(nullptr),
        m_numTrueVertices{},
        m_true_pri_x{},
        m_true_pri_y{},
        m_true_pri_z{},
        m_true_sec_x(nullptr),
        m_true_sec_y(nullptr),
        m_true_sec_z(nullptr)
{

    declareInterface<IMcEventNtupleTool>(this);
    // Declare the properties
    declareProperty("NtupleFileName",		      m_ntupleFileName,	       "Ntuple file handle");
    declareProperty("NtupleDirectoryName",	      m_ntupleDirName,	       "Directory name for ntuple tree");
    declareProperty("NtupleTrueTreeName",	      m_ntupleMcTreeName,      "Name of the MC ntuple tree");
    declareProperty("ptCut_McCollection",	      m_ptCut,		       "Pt cut on true MC vertices");
    declareProperty("etaCut_McCollection",	      m_etaCut,		       "Eta cut on true MC vertices");
    declareProperty("radiusCut_McCollection",	      m_radiusCut,	       "Radius cut on true MC vertices");
    declareProperty("zPosCut_McCollection",	      m_zPosCut,	       "Z position cut on true MC vertices");
    declareProperty("radResolution_McCollection",     m_radiusRes,	       "MC vertex resolution criterion (radius)");
    declareProperty("zResolution_McCollection",      m_zPosRes,	       "MC vertex resolution criterion (z pos)");
}

// destructor
Trk::McEventNtupleTool::~McEventNtupleTool() {}

//////////////////
/// initialize
////////////////

StatusCode Trk::McEventNtupleTool::initialize() {

    // Retrieve the Histo Service
    ITHistSvc* hist_svc;
    StatusCode status;

    status=service("THistSvc",hist_svc);
    if(status.isFailure())
    {
	    msg(MSG::ERROR) << "Could not find HistService" << endmsg;
	    return status;
    }

    //registering the mc event tree
    m_vtx_tree = new TTree(TString(m_ntupleMcTreeName), "MC output");
    std::string fullMcNtupleName = m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleMcTreeName;
    status = hist_svc->regTree(fullMcNtupleName, m_vtx_tree);
    if (status.isFailure()) {
	   msg(MSG::ERROR) << "Unable to register TTree : " << fullMcNtupleName << endmsg;
	   return status;
    }

    //mc vtx tree
    m_vtx_tree->Branch ("vertex_true_pri_x", &m_true_pri_x, "vtx_true_pri_x/F");
    m_vtx_tree->Branch ("vertex_true_pri_y", &m_true_pri_y, "vtx_true_pri_y/F");
    m_vtx_tree->Branch ("vertex_true_pri_z", &m_true_pri_z, "vtx_true_pri_z/F");

    m_true_sec_x = new std::vector<float>();
    m_true_sec_y = new std::vector<float>();
    m_true_sec_z = new std::vector<float>();

    m_vtx_tree->Branch ("vertex_true_sec_x", &m_true_sec_x);
    m_vtx_tree->Branch ("vertex_true_sec_y", &m_true_sec_y);
    m_vtx_tree->Branch ("vertex_true_sec_z", &m_true_sec_z);

    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::McEventNtupleTool::finalize() {

    msg(MSG::DEBUG) << "start finalize() in " << name() << endmsg;
    return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////
// fill Mc vertices data
////////////////////////////////////////////////

StatusCode Trk::McEventNtupleTool::fillMcEventData(const HepMC::GenEvent& myEvent) const {

    HepMC::GenEvent::vertex_const_iterator Vert = myEvent.vertices_begin();
    HepMC::GenEvent::vertex_const_iterator Vert_end = myEvent.vertices_end();
     //store primary vertex
     CLHEP::HepLorentzVector pv_pos((*Vert)->position().x(),
			     (*Vert)->position().y(),
			     (*Vert)->position().z(),
			     (*Vert)->position().t());
     double pv_r = pv_pos.perp();
     double pv_z = pv_pos.z();
     std::map<int,HepMC::GenVertex *> pv_vtx_ids;
     std::vector<std::map<int,HepMC::GenVertex *> > sec_vtx_ids_vec;
     HepMC::GenEvent::particle_const_iterator  Part = myEvent.particles_begin();
     for (;Part!=myEvent.particles_end(); ++Part){
          //information about incomming and outgoing particles
          CLHEP::HepLorentzVector par_mom((*Part)->momentum().px(),
				   (*Part)->momentum().py(),
				   (*Part)->momentum().pz(),
				   (*Part)->momentum().e());
          HepMC::GenVertex* par_vert = (*Part)->production_vertex();
	  if (par_vert)
          {
           CLHEP::HepLorentzVector lv_pos(par_vert->position().x(),
				   par_vert->position().y(),
				   par_vert->position().z(),
				   par_vert->position().t());

           if (par_mom.perp() > m_ptCut && fabs(par_mom.eta()) <= m_etaCut && fabs(lv_pos.perp()) <= m_radiusCut &&  fabs(lv_pos.z()) <= m_zPosCut){

             if(fabs(lv_pos.perp() - pv_r)<m_radiusRes  && fabs(lv_pos.z() - pv_z)<m_zPosRes)
             {
               pv_vtx_ids.insert(std::make_pair(par_vert->barcode(),par_vert));
             } else {
                bool new_sec_vtx = true;
                //loop over all entries in sec_vtx_ids_vec = vector of sec_vtx_map's
                for (unsigned int sec_vec_itr = 0; sec_vec_itr < sec_vtx_ids_vec.size(); ++sec_vec_itr)
                {
                  std::map<int,HepMC::GenVertex *> sec_vtx_map = sec_vtx_ids_vec[sec_vec_itr];
                  std::map<int,HepMC::GenVertex *>::iterator map_itr = sec_vtx_map.begin();
                  for (; map_itr!= sec_vtx_map.end(); ++map_itr)
                  {
                    CLHEP::HepLorentzVector sec_pos((*map_itr).second->position().x(),
					     (*map_itr).second->position().y(),
					     (*map_itr).second->position().z(),
					     (*map_itr).second->position().t());
                    double sec_r = sec_pos.perp();
                    double sec_z = sec_pos.z();
		    // if vertex position are nearly the same, store this vertex in the map
                    if(fabs(lv_pos.perp() - sec_r)< m_radiusRes && fabs(lv_pos.z() - sec_z)<m_zPosRes)
                    {
                       sec_vtx_map.insert(std::make_pair(par_vert->barcode(),par_vert));
                       new_sec_vtx = false;
                       break;
                    }
                  }
                }
                if(new_sec_vtx) //store new entry in sec_vtx_ids_vec
                {
                  std::map<int,HepMC::GenVertex *> new_map;
                  new_map.insert(std::make_pair(par_vert->barcode(),par_vert));
                  sec_vtx_ids_vec.push_back(new_map);
                }
             }//if not primary
           }//eta && mom cut
          }//if production vertex
      }//end loop over particles
      std::map<int,HepMC::GenVertex *>::iterator pv_map_itr = pv_vtx_ids.begin();
      m_true_pri_x = (*pv_map_itr).second->position().x();
      m_true_pri_y = (*pv_map_itr).second->position().y();
      m_true_pri_z = (*pv_map_itr).second->position().z();
      //std::cout<<"number of secondary vertices = "<<sec_vtx_ids_vec.size()<<std::endl;
      for (unsigned int sec_vtx_itr = 0; sec_vtx_itr < sec_vtx_ids_vec.size(); ++sec_vtx_itr)
      {
           std::map<int,HepMC::GenVertex *> sec_map =  sec_vtx_ids_vec[sec_vtx_itr];
           std::map<int,HepMC::GenVertex *>::iterator map_itr = sec_map.begin();
           CLHEP::HepLorentzVector sec_vtx_pos;
           for (; map_itr != sec_map.end(); ++map_itr){
                sec_vtx_pos.setX(sec_vtx_pos.x() + (*map_itr).second->position().x());
                sec_vtx_pos.setY(sec_vtx_pos.y() + (*map_itr).second->position().y());
                sec_vtx_pos.setZ(sec_vtx_pos.z() + (*map_itr).second->position().z());
                sec_vtx_pos.setT(sec_vtx_pos.t() + (*map_itr).second->position().t());
          }
           if (sec_map.size() > 0){
              sec_vtx_pos = sec_vtx_pos/sec_map.size();
              m_true_sec_x->push_back(sec_vtx_pos.x());
              m_true_sec_y->push_back(sec_vtx_pos.y());
              m_true_sec_z->push_back(sec_vtx_pos.z());
           }
      }
      m_numTrueVertices = 1 + sec_vtx_ids_vec.size();

      m_vtx_tree->Fill();

     //clear
     m_true_sec_x->clear();
     m_true_sec_y->clear();
     m_true_sec_z->clear();
     return StatusCode::SUCCESS;
}
