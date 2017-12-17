/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigRNNOutput.h"

#include "TrigCaloEvent/TrigEMClusterContainer.h"

#include "TrigCaloEventTPCnv/TrigRNNOutput_p2.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_p2.h"

#include "AthLinks/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

void TrigRNNOutputCnv_p2::transToPers(const TrigRNNOutput *trans, 
                                    TrigRNNOutput_p2 *pers,
				    MsgStream &log) {

  log << MSG::DEBUG << "TrigRNNOutputCnv_p2::tranToPers" << endmsg;
  
  if ( (!trans) || (!pers) )
    return;

  pers->m_output.clear();
  for (unsigned int i = 0; i < trans->size(); ++i) { 
    pers->m_output.push_back(trans->at(i));
  }

  ELinkTrigEMClusterCnv.transToPers(&trans->clusterLink(), &pers->m_cluster, log);
}

void TrigRNNOutputCnv_p2::persToTrans(const TrigRNNOutput_p2 *pers, 
                                    TrigRNNOutput *trans,
				    MsgStream &log) {

  log << MSG::DEBUG << "TrigRNNOutputCnv_p2::persToTrans" << endmsg;

  if ( (!pers) || (!trans) )
    return;

  trans->output().clear();
  for (unsigned int i = 0; i < pers->m_output.size(); ++i){   
    trans->output().push_back(pers->m_output[i]);
  }

  ElementLink<TrigEMClusterContainer> cluster;
  ELinkTrigEMClusterCnv.persToTrans(&pers->m_cluster, &cluster, log);
  trans->setCluster (cluster);
}

