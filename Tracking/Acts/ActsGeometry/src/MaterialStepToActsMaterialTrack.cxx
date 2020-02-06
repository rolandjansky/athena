/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialMapping.cxx, (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////

// Gaudi Units
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ITHistSvc.h"

#include "ActsGeometry//MaterialStepToActsMaterialTrack.h"
#include "TrkGeometry/MaterialStep.h"
#include "TrkGeometry/MaterialStepCollection.h"
#include "TrkGeometry/MaterialProperties.h"
// TrkEvent
#include "TrkNeutralParameters/NeutralParameters.h"
// Amg
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

#include "TTree.h"

MaterialStepToActsMaterialTrack::MaterialStepToActsMaterialTrack(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name,pSvcLocator),
  m_ActsFileName("material-tracks"),
  m_ActsDirName("tracks"),
  m_ActsTreeName("mat-tracks"),
  m_inputMaterialStepCollection("MaterialStepRecords"),
  m_thistSvc(0)
{
  declareProperty("ActsMaterialTrackFileName", m_ActsFileName);

}

MaterialStepToActsMaterialTrack::~MaterialStepToActsMaterialTrack()
{}

StatusCode MaterialStepToActsMaterialTrack::initialize()
{

    ATH_MSG_INFO("initialize()");

    ATH_CHECK( m_inputMaterialStepCollection.initialize() );
    ATH_CHECK( service("THistSvc", m_thistSvc) );

    m_outputTree = new TTree( TString(m_ActsTreeName), "ACTS" );
    std::string fullName =  "/Acts/"+m_ActsTreeName;
    ATH_CHECK( m_thistSvc->regTree(fullName, m_outputTree) );

    /** now add branches and leaves to the tree */
    // event tree : one entry per event
    m_outputTree->Branch("v_x", &m_v_x);
    m_outputTree->Branch("v_y", &m_v_y);
    m_outputTree->Branch("v_z", &m_v_z);
    m_outputTree->Branch("v_px", &m_v_px);
    m_outputTree->Branch("v_py", &m_v_py);
    m_outputTree->Branch("v_pz", &m_v_pz);
    m_outputTree->Branch("v_phi", &m_v_phi);
    m_outputTree->Branch("v_eta", &m_v_eta);
    m_outputTree->Branch("t_X0", &m_tX0);
    m_outputTree->Branch("t_L0", &m_tL0);
    m_outputTree->Branch("mat_x", &m_step_x);
    m_outputTree->Branch("mat_y", &m_step_y);
    m_outputTree->Branch("mat_z", &m_step_z);
    m_outputTree->Branch("mat_step_length", &m_step_length);
    m_outputTree->Branch("mat_X0", &m_step_X0);
    m_outputTree->Branch("mat_L0", &m_step_L0);
    m_outputTree->Branch("mat_A", &m_step_A);
    m_outputTree->Branch("mat_Z", &m_step_Z);
    m_outputTree->Branch("mat_rho", &m_step_rho);

    return StatusCode::SUCCESS;
}


StatusCode MaterialStepToActsMaterialTrack::execute()
{
  ATH_MSG_VERBOSE("MaterialMapping execute() start");

  SG::ReadHandle<Trk::MaterialStepCollection> materialStepCollection(m_inputMaterialStepCollection);

   m_v_x = 0;
   m_v_y = 0;
   m_v_z = 0;

   m_v_px = 0;
   m_v_py = 0;
   m_v_pz = 0;

   m_v_phi = 0;
   m_v_eta = 0;

   m_tX0 = 0;
   m_tL0 = 0;

   m_step_x.clear();
   m_step_y.clear();
   m_step_z.clear();
   m_step_length.clear();
   m_step_X0.clear();
   m_step_L0.clear();
   m_step_A.clear();
   m_step_Z.clear();
   m_step_rho.clear();

  // clearing the recorded layers per event
  if (materialStepCollection.isValid() && materialStepCollection->size()){

    // get the number of material steps
    size_t materialSteps = materialStepCollection->size();
    ATH_MSG_DEBUG("[+] Successfully read  "<<  materialSteps << " geantino steps");

    // create a direction out of the last material step
    m_v_px = (*materialStepCollection)[materialSteps-1]->hitX();
    m_v_py = (*materialStepCollection)[materialSteps-1]->hitY();
    m_v_pz = (*materialStepCollection)[materialSteps-1]->hitZ();

    Amg::Vector3D direction = Amg::Vector3D(m_v_px,m_v_py,m_v_pz).unit();

    m_v_px = direction.x();
    m_v_py = direction.y();
    m_v_pz = direction.z();

    m_v_eta = direction.eta();
    m_v_phi = direction.phi();

    for ( const Trk::MaterialStep* step : *materialStepCollection ) {
      m_step_x.push_back(step->hitX());
      m_step_y.push_back(step->hitY());
      m_step_z.push_back(step->hitZ());
      m_step_length.push_back(step->steplength());
      m_step_X0.push_back(step->x0());
      m_step_L0.push_back(step->l0());
      m_step_A.push_back(step->A());
      m_step_Z.push_back(step->Z());
      m_step_rho.push_back(step->rho());

      m_tX0 +=step->steplength()/step->x0();
      m_tL0 +=step->steplength()/step->l0();
    }
    m_outputTree->Fill();
  }
  return StatusCode::SUCCESS;
}

StatusCode MaterialStepToActsMaterialTrack::finalize()
{
  std::cout << "Fin" << std::endl;
  return StatusCode::SUCCESS;
}
