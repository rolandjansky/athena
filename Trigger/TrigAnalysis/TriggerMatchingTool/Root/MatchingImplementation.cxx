/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/MatchingImplementation.h"
#include "TriggerMatchingTool/MatchingTool.h"
#include "MinimalSumAssociation.h"

namespace Trig {

MatchingImplementation::MatchingImplementation(MatchingTool& mt) : asg::AsgMessaging("MatchingImplementation"), m_tool(mt) {
  m_strategies[Trig::MatchingStrategy::MinimalSum] = std::unique_ptr<IAssociationStrategy>(new MinimalSumAssociation());
}

Trig::TrigDecisionTool* MatchingImplementation::tdt(){
  return m_tool.m_trigDecTool.operator->();
}

  bool MatchingImplementation::assocIsMatched(IAssociationStrategy::index_assignment_t association, const std::vector<std::vector<double> >& matrix, double threshold){
    int ndim =  matrix.size();
    if(!ndim) return false;
    
    bool result = true;
    for(auto reco_trig : association){
      double distance = matrix[reco_trig.first][reco_trig.second];
      bool single_assoc =  distance < threshold;
      result = result && single_assoc;
      ATH_MSG_DEBUG("reco: " << reco_trig.first << " associated to trig: " << reco_trig.second <<
		    " with distance: " << distance << " ok: " << single_assoc << " overall: " << result);
    }
    return result;
  }
  
  std::pair<bool,IAssociationStrategy::index_assignment_t> MatchingImplementation::matchDistanceMatrix(const std::vector<std::vector<double> >& matrix, double threshold, const Trig::MatchingStrategy::Strategy strategy){
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
	ATH_MSG_DEBUG("row of matrix does not contain any distance below threshold. Return false");
	return std::make_pair(false,IAssociationStrategy::index_assignment_t());
      }
    }
    
    auto association_map = m_strategies[strategy]->associate(matrix);
    return std::make_pair(assocIsMatched(association_map,matrix,threshold),association_map);
  }
}
