 
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkExAlgs/PropResultRootWriterSvc.h"

#include <vector>
#include <deque>
#include <mutex>
#include <thread>

#include "TFile.h"

Trk::PropResultRootWriterSvc::PropResultRootWriterSvc( const std::string& name, ISvcLocator* svc )
: AthService(name, svc),
  m_thistSvc("THistSvc", name),
  m_tree(0) {
}

Trk::PropResultRootWriterSvc::~PropResultRootWriterSvc() {}

StatusCode Trk::PropResultRootWriterSvc::initialize()
{
  ATH_CHECK( AthService::initialize() );

  CHECK(m_thistSvc.retrieve());
  
  const std::string treeName = m_treeName;  
  m_tree = new TTree(treeName.data(), "MyStudies");
  
  std::string fullNtupleName = static_cast<std::string>(m_ntupleDirName) + m_treeName;
  CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));

  if(m_tree == nullptr) {
    ATH_MSG_ERROR("Unable to create TTree");
    return StatusCode::FAILURE;
  }

  m_tree->Branch("event_nr"   , &m_eventNum   );
  
  m_tree->Branch("start_d0"   , &m_start_d0   );
  m_tree->Branch("start_z0"   , &m_start_z0   );
  m_tree->Branch("start_phi"  , &m_start_phi  );
  m_tree->Branch("start_theta", &m_start_theta);
  m_tree->Branch("start_qop"  , &m_start_qop  );
  
  m_tree->Branch("fwd_success"    , &m_fwd_success    );
  m_tree->Branch("fwd_time"       , &m_fwd_time       );
  m_tree->Branch("fwd_final_l0"   , &m_fwd_final_l0   );
  m_tree->Branch("fwd_final_l1"   , &m_fwd_final_l1   );
  m_tree->Branch("fwd_final_x"    , &m_fwd_final_x    );
  m_tree->Branch("fwd_final_y"    , &m_fwd_final_y    );
  m_tree->Branch("fwd_final_z"    , &m_fwd_final_z    );
  m_tree->Branch("fwd_final_phi"  , &m_fwd_final_phi  );
  m_tree->Branch("fwd_final_theta", &m_fwd_final_theta);
  m_tree->Branch("fwd_final_qop"  , &m_fwd_final_qop  );
  m_tree->Branch("fwd_final_sigma_l0"   , &m_fwd_final_sigma_l0   );
  m_tree->Branch("fwd_final_sigma_l1"   , &m_fwd_final_sigma_l1   );
  m_tree->Branch("fwd_final_sigma_phi"  , &m_fwd_final_sigma_phi  );
  m_tree->Branch("fwd_final_sigma_theta", &m_fwd_final_sigma_theta);
  m_tree->Branch("fwd_final_sigma_qop"  , &m_fwd_final_sigma_qop  );
  
  m_tree->Branch("bkw_success"    , &m_bkw_success    );
  m_tree->Branch("bkw_time"       , &m_bkw_time       );
  m_tree->Branch("bkw_final_d0"   , &m_bkw_final_d0   );
  m_tree->Branch("bkw_final_z0"   , &m_bkw_final_z0   );
  m_tree->Branch("bkw_final_phi"  , &m_bkw_final_phi  );
  m_tree->Branch("bkw_final_theta", &m_bkw_final_theta);
  m_tree->Branch("bkw_final_qop"  , &m_bkw_final_qop  );
  m_tree->Branch("bkw_final_sigma_d0"   , &m_bkw_final_sigma_d0   );
  m_tree->Branch("bkw_final_sigma_z0"   , &m_bkw_final_sigma_z0   );
  m_tree->Branch("bkw_final_sigma_phi"  , &m_bkw_final_sigma_phi  );
  m_tree->Branch("bkw_final_sigma_theta", &m_bkw_final_sigma_theta);
  m_tree->Branch("bkw_final_sigma_qop"  , &m_bkw_final_sigma_qop  );
  
  return StatusCode::SUCCESS;
}
  
StatusCode Trk::PropResultRootWriterSvc::finalize()
{
  return StatusCode::SUCCESS;
}

