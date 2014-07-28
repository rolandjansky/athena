/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDiMuon/Candidate.h"

Candidate::Candidate() :
m_pt                          (0.0),
m_eta                         (0.0),
m_phi                         (0.0),
m_charge                      (0),
m_eta_ext_inner_barrel        (0.0),
m_phi_ext_inner_barrel        (0.0),
m_eta_ext_middle_barrel       (0.0),
m_phi_ext_middle_barrel       (0.0),
m_eta_ext_inner_endcap        (0.0),
m_phi_ext_inner_endcap        (0.0),
m_eta_ext_middle_endcap       (0.0),
m_phi_ext_middle_endcap       (0.0),
m_n_mdt_hits                  (0),
m_n_tgc_eta_hits              (0),
m_n_tgc_phi_hits              (0),
m_n_rpc_eta_hits              (0),
m_n_rpc_phi_hits              (0),
m_do_rpc                      (false),
m_do_tgc                      (false)
{
}
