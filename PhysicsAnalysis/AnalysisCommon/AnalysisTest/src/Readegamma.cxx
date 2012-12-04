/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTest/Readegamma.h"
#include "Particle/TrackParticle.h"

StatusCode Readegamma::userInit()
{
  m_ntuplePtr -> addItem (m_prefix+"/e", m_nCon,  m_e);
  return StatusCode::SUCCESS;
}

StatusCode Readegamma::userExec(const egamma *val)
{
  MsgStream log(msgSvc(), name());

  m_e [m_nCon] = val->e();

  // get LAr cluster
  const CaloCluster *cls = val->cluster();
  if (cls == 0)
    {
      log << MSG::INFO << "Null LArCluster" << endreq;
    }
  else
    {
      log << MSG::INFO << "LArCluster eta0: " << cls->eta0() << endreq;
    }

  // get TrackParticle
  const Rec::TrackParticle *tp;
  tp = val->trackParticle();
  if (tp == 0)
    {
      log << MSG::INFO << "Null TrackParticle" << endreq;
    }
  else
    {
      log << MSG::INFO << "TrackParticle charge: " << tp->charge() << endreq;
    }

  return StatusCode::SUCCESS;
}
