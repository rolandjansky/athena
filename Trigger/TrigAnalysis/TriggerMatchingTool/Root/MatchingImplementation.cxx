/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/MatchingImplementation.h"
#include "TriggerMatchingTool/MatchingTool.h"
#include "MinimalSumAssociation.h"

namespace Trig {

MatchingImplementation::MatchingImplementation(MatchingTool& mt, double threshold) : asg::AsgMessaging("MatchingImplementation"), m_tool(mt), m_threshold(threshold) {
  m_strategies[Trig::MatchingStrategy::MinimalSum] = std::unique_ptr<IAssociationStrategy>(new MinimalSumAssociation());
}

Trig::TrigDecisionTool* MatchingImplementation::tdt(){
  return m_tool.m_trigDecTool.operator->();
}

  bool MatchingImplementation::assocIsMatched(IAssociationStrategy::index_assignment_t association, const std::vector<std::vector<double> >& matrix, double threshold) const {
  int ndim =  matrix.size();
  if(!ndim) return false;

  bool result = true;
  for(auto trig_reco : association){
    double distance = matrix[trig_reco.first][trig_reco.second];
    bool single_assoc =  distance < threshold;
    result = result && single_assoc;
    ATH_MSG_DEBUG("reco: " << trig_reco.first << " associated to trig: " << trig_reco.second <<
		  " with distance: " << distance << " ok: " << single_assoc << " overall: " << result);
  }
  return result;
}

  bool MatchingImplementation::matchDistanceMatrix(const std::vector<std::vector<double> >& matrix, const Trig::MatchingStrategy::Strategy strategy, double threshold) const {
  int nrows =  matrix.size();
  int ncols = matrix.at(0).size();

  ATH_MSG_DEBUG("matching a " << nrows << "x" << ncols << "matrix now");


  auto MSG_MATRIX = MSG::DEBUG;
  if(msgLvl(MSG_MATRIX)){
    msg() << MSG_MATRIX << "===========" << endmsg;
    for(auto& row : matrix){
      msg() << MSG_MATRIX << "|";
      for(auto distance : row){ 
	msg() << MSG_MATRIX << distance << " , ";
      }
      msg() << MSG_MATRIX << "|" << endmsg;
    }
    msg() << MSG_MATRIX << "===========" << endmsg;
  }
  
  
  //require that every row contains an entry that is smaller than threshold, otherwise matching will obviously fail 
  for(auto& row : matrix) {
    bool rowOk=false;
    for(auto distance : row) {
      if( distance < threshold ) { rowOk = true; break; }
    }
    if(!rowOk) {
      ATH_MSG_DEBUG("row of matrix does not contain any distance below threshold. Return false"); return false;
    }
  }

  auto association_map = m_strategies.at(strategy)->associate(matrix);
  return assocIsMatched(association_map,matrix,threshold);
}

}
