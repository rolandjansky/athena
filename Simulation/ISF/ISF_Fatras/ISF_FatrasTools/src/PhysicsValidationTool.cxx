/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PhysicsValidationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/PhysicsValidationTool.h"

// Tracking
#include "TrkParameters/TrackParameters.h"
// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ParticleUserInformation.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
//
// ROOT includes
#include "TTree.h"
 
#include <iostream>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::PhysicsValidationTool::PhysicsValidationTool( const std::string& t,
                                      const std::string& n,
                                      const IInterface*  p )
 : base_class(t,n,p),
   m_thistSvc("THistSvc",n),
   m_validationStream("ISFFatras")
{
  // validation output section
  declareProperty("ValidationStreamName",
                  m_validationStream = "ISFFatras",
                  "Name of the output stream" );
  declareProperty("THistService",
                  m_thistSvc,
                  "The THistSvc" );
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::PhysicsValidationTool::~PhysicsValidationTool()
{
  m_interactions = 0;
  delete(m_interactions);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::PhysicsValidationTool::initialize()
{
  
  ATH_MSG_INFO( "initialize()" );
  
  // retrieve the histogram service
  if ( m_thistSvc.retrieve().isSuccess() ) {
    // Create the prefix of histogram names for the THistSvc
    const char *treeName="particles";
    const std::string prefix = "/" + m_validationStream + "/"+ treeName;
    m_particles = new TTree( treeName, treeName );
    m_particles->Branch("pdg"       , &m_pdg    , "pdg/I"         );
    m_particles->Branch("scIn"      , &m_scIn   , "scIn/I"         );
    m_particles->Branch("scOut"     , &m_scEnd  , "scOut/I"         );
    m_particles->Branch("gen"       , &m_gen    , "gen/I"         );
    m_particles->Branch("geoID"     , &m_geoID  , "geoID/I"         );
    m_particles->Branch("pth"       , &m_pth     , "pth/F"            );
    m_particles->Branch("pph"       , &m_pph     , "pph/F"            );
    m_particles->Branch("p"         , &m_p      , "p/F"            );
    m_particles->Branch("eloss"     , &m_eloss  , "eloss/F"            );
    m_particles->Branch("ionloss"     , &m_ionloss  , "ionloss/F"            );
    m_particles->Branch("radloss"     , &m_radloss  , "radloss/F"            );
    m_particles->Branch("zOaTr"     , &m_zOaTr  , "zOaTr/F"            );
    m_particles->Branch("wZ"         , &m_wZ  ,   "wZ/F"            );
    m_particles->Branch("X0"        , &m_X0     , "X0/F"            );
    m_particles->Branch("dt"        , &m_dt     , "dt/F"            );
    m_particles->Branch("thIn"      , &m_thIn   , "thIn/F"            );
    m_particles->Branch("phIn"      , &m_phIn   , "phIn/F"            );
    m_particles->Branch("dIn"       , &m_dIn    , "dIn/F"            );
    m_particles->Branch("thEnd"     , &m_thEnd  , "thEnd/F"            );
    m_particles->Branch("phEnd"     , &m_phEnd  , "phEnd/F"            );
    m_particles->Branch("dEnd"      , &m_dEnd   , "dEnd/F"            );
    
    // register the Tree to the THistSvc and return it's StatusCode
    ATH_CHECK(m_thistSvc->regTree( prefix, m_particles) );


    const char *treeNameInt="interactions";
    const std::string prefixInt = "/" + m_validationStream + "/"+ treeNameInt;
    m_interactions = new TTree( treeNameInt, treeNameInt );
    m_interactions->Branch("process"     , &m_process  , "process/I"         );
    m_interactions->Branch("pdg_mother"  , &m_pdg_mother  , "pdg_mother/I"         );
    m_interactions->Branch("gen_mother"  , &m_gen_mother  , "gen_mother/I"         );
    m_interactions->Branch("nChild"      , &m_nChild      , "nch/I"         );
    m_interactions->Branch("vtx_dist"    , &m_vtx_dist    , "vtx_dist/F"         );
    m_interactions->Branch("vtx_theta"   , &m_vtx_theta   , "vtx_theta/F"         );
    m_interactions->Branch("vtx_phi"     , &m_vtx_phi     , "vtx_phi/F"         );
    m_interactions->Branch("vtx_e_diff"  , &m_vtx_e_diff  , "vtx_e_diff/F"         );
    m_interactions->Branch("vtx_p_diff"  , &m_vtx_p_diff  , "vtx_p_diff/F"         );
    m_interactions->Branch("vtx_plong_diff"  , &m_vtx_plong_diff  , "vtx_plong_diff/F"         );
    m_interactions->Branch("vtx_pperp_diff"  , &m_vtx_pperp_diff  , "vtx_pperp_diff/F"         );
    m_interactions->Branch("p_mother"    , &m_p_mother    , "p_mother/F"         );
    m_interactions->Branch("pdg_child"   , m_pdg_child    , "pdg_child[nch]/I"         );
    m_interactions->Branch("fp_child"    , m_fp_child     , "fp_child[nch]/F"         );
    m_interactions->Branch("oa_child"    , m_oa_child     , "oa_child[nch]/F"         );
    // register the Tree to the THistSvc and return it's StatusCode
    ATH_CHECK(m_thistSvc->regTree( prefixInt, m_interactions) );
    
  }

  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode iFatras::PhysicsValidationTool::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

/** new transport tool */
void iFatras::PhysicsValidationTool::saveISFParticleInfo(const ISF::ISFParticle& isp, 
							 const Trk::ExtrapolationCell<Trk::TrackParameters>& ec,
							 Trk::ExtrapolationCode ecode) const {

  m_p     = isp.momentum().mag();
  m_ionloss = ec.eLoss ? ec.eLoss->meanIoni() : 0.;
  m_radloss = ec.eLoss ? ec.eLoss->meanRad() : 0.;           // average estimate
  m_zOaTr   = ec.zOaTrX;
  m_wZ   = ec.zX;

  if (ecode == Trk::ExtrapolationCode::SuccessBoundaryReached) {   // surviving particle
    m_scEnd = 0;
    m_eloss = ec.endParameters->momentum().mag()-m_p;
    //m_dt = time - isp.timeStamp();
    m_dt = ec.pathLength/CLHEP::c_light;   // TODO get timing info from the cell 

    m_thEnd = ec.endParameters->position().theta();
    m_phEnd = ec.endParameters->position().phi();
    m_dEnd  = ec.endParameters->position().mag();

  } else {

    m_scEnd = -1;   // undefined
    if (ecode == Trk::ExtrapolationCode::SuccessPathLimit) m_scEnd = 201;   // decay
    else if (ecode == Trk::ExtrapolationCode::SuccessMaterialLimit ) m_scEnd = ec.materialProcess;   // material interaction

    m_eloss = -m_p;
    m_thEnd = -10.;     // TODO : retrieve position of stopped particle
    m_phEnd = -10.;     // TODO : retrieve position of stopped particle 
    m_dEnd = -1.;       // TODO : retrieve position of stopped particle
  }
  m_X0 = ec.materialX0;

  saveInfo(isp);

}

/** new transport tool */
void iFatras::PhysicsValidationTool::saveISFParticleInfo(const ISF::ISFParticle& isp, 
							 const Trk::ExtrapolationCell<Trk::NeutralParameters>& ec,
							 Trk::ExtrapolationCode ecode) const {

  m_p     = isp.momentum().mag();
  m_ionloss = 0.;
  m_radloss = 0.;
  m_zOaTr   = ec.zOaTrX;
  m_wZ   = ec.zX;

  if (ecode == Trk::ExtrapolationCode::SuccessBoundaryReached) {   // surviving particle
    m_scEnd = 0;
    m_eloss = ec.endParameters->momentum().mag()-m_p;
    //m_dt = time - isp.timeStamp();
    m_dt = ec.pathLength/CLHEP::c_light;   // TODO get timing info from the cell 

    m_thEnd = ec.endParameters->position().theta();
    m_phEnd = ec.endParameters->position().phi();
    m_dEnd  = ec.endParameters->position().mag();

  } else {

    m_scEnd = -1;   // undefined
    if (ecode == Trk::ExtrapolationCode::SuccessPathLimit) m_scEnd = 201;   // decay
    else if (ecode == Trk::ExtrapolationCode::SuccessMaterialLimit ) m_scEnd = ec.materialProcess;   // material interaction

    m_eloss = -m_p;
    m_thEnd = -10.;     // TODO : retrieve position of stopped particle
    m_phEnd = -10.;     // TODO : retrieve position of stopped particle 
    m_dEnd = -1.;       // TODO : retrieve position of stopped particle
  }
  m_X0 = ec.materialX0;

  saveInfo(isp);

}

/** old transport tool/prompt decay */
void iFatras::PhysicsValidationTool::saveISFParticleInfo(const ISF::ISFParticle& isp, 
							 int endProcess,
							 const Trk::TrackParameters* ePar,
							 double time, double dX0 ) const {

  m_pdg = isp.pdgCode();
  m_scIn = isp.getUserInformation()? isp.getUserInformation()->process() : 0;      // assume primary track
  m_gen = isp.getUserInformation()? isp.getUserInformation()->generation() : 0;    // assume primary track 
  m_pth=isp.momentum().theta();
  m_pph=isp.momentum().phi();
  m_geoID = isp.nextGeoID();
  m_dIn  = isp.position().mag();
  m_thIn = isp.position().theta();
  m_phIn = isp.position().phi();
  m_p     = isp.momentum().mag();
  m_scEnd = endProcess;
  m_eloss = ePar? ePar->momentum().mag()-m_p : -m_p;
  m_dt = time - isp.timeStamp();

  m_ionloss = 0.;    // n.a.
  m_radloss = 0.;    // n.a.
  m_zOaTr   = 0.;    // n.a.

  if (ePar) { 

    //error propagation
    //const Trk::MeasuredTrackParameters* mPar = dynamic_cast<const Trk::MeasuredTrackParameters*> (ePar);
    //if (mPar) std::cout << "error propagation :"<< mPar->localErrorMatrix().error(Trk::qOverP) << std::endl;

    m_thEnd = ePar->position().theta();
    m_phEnd = ePar->position().phi();
    m_dEnd  = ePar->position().mag();
  } else {
    m_thEnd = m_thIn;
    m_phEnd = m_phIn;
    m_dEnd = -1.;       // particle stopped and position unknown
  }
  m_X0 = dX0;

  m_particles->Fill();

}

void iFatras::PhysicsValidationTool::saveInfo(const ISF::ISFParticle& isp) const { 

  // ISF particle info ( common )

  m_pdg = isp.pdgCode();
  m_scIn = isp.getUserInformation()? isp.getUserInformation()->process() : 0;      // assume primary track
  m_gen = isp.getUserInformation()? isp.getUserInformation()->generation() : 0;    // assume primary track 
  m_pth=isp.momentum().theta();
  m_pph=isp.momentum().phi();
  m_geoID = isp.nextGeoID();
  m_dIn  = isp.position().mag();
  m_thIn = isp.position().theta();
  m_phIn = isp.position().phi();

  m_particles->Fill();

}

void iFatras::PhysicsValidationTool::saveISFVertexInfo(int process,Amg::Vector3D vertex,const ISF::ISFParticle& parent,Amg::Vector3D primIn,
						       Amg::Vector3D* primOut, const ISF::ISFParticleVector children) const {

  m_process = process;
  unsigned int nSec = children.size();
  
  int iPrimSurv = primOut ? 1 : 0;
  
  m_pdg_mother = parent.pdgCode();
  m_gen_mother = parent.getUserInformation()? parent.getUserInformation()->generation() : 0;  // assume primary track
  
  m_p_mother   = primIn.mag();
  
  m_vtx_dist    = vertex.mag(); 
  m_vtx_theta   = vertex.theta(); 
  m_vtx_phi     = vertex.phi(); 
  
  m_nChild     = nSec+iPrimSurv;
  
  Amg::Vector3D pbal(primIn);
  
  if (iPrimSurv>0) {
    m_pdg_child[0] = m_pdg_mother;
    m_fp_child[0] = primOut->mag()/m_p_mother;
    m_oa_child[0] = primOut->unit().dot( primIn.unit() );                              
    pbal -= *primOut;
  }

  unsigned int nSecMax = nSec+iPrimSurv> MAXCHILDREN ? MAXCHILDREN-iPrimSurv : nSec; 
  for (unsigned int isec=0; isec< nSec; isec++) {
    if (isec<nSecMax) {
      m_pdg_child[isec+iPrimSurv] = children[isec]->pdgCode();
      m_fp_child[isec+iPrimSurv]  = children[isec]->momentum().mag()/m_p_mother;
      m_oa_child[isec+iPrimSurv]  = children[isec]->momentum().unit().dot( primIn.unit() );
    }
    pbal -= children[isec]->momentum();
  }

  m_vtx_p_diff = pbal.mag();
  m_vtx_plong_diff = pbal.dot(primIn)/m_p_mother;
  m_vtx_pperp_diff = sqrt(m_vtx_p_diff*m_vtx_p_diff-m_vtx_plong_diff*m_vtx_plong_diff);

  m_interactions->Fill(); 
}
