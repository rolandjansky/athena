/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaUtils/egammaDuplicateRemoval.h"
#include "FourMomUtils/P4Helpers.h"
#include "AnalysisUtils/AnalysisMisc.h"

namespace egammaDuplicateRemoval {

  std::vector<const xAOD::CaloCluster *> getClusterDuplicateRemoval(std::vector<const xAOD::CaloCluster* > input_phList, double DETA /*=0.05 */, double DPHI /*=0.1*/) {

    //sort w.r.t. cluster Et
    AnalysisUtils::Sort::pT (&input_phList);
    
    std::vector<bool> keepCand(input_phList.size(), true);
    std::vector<const xAOD::CaloCluster*> output_phList;

    // we need more than 2 clusters to check overlap 
    if ( input_phList.size() >  1 )  {
        for (unsigned int iph1=0; iph1 < input_phList.size(); iph1++) {
	  if (keepCand[iph1]) {
	    for (unsigned int iph2=iph1+1; iph2 < input_phList.size(); iph2++) {
	      if (keepCand[iph2]) {
		double dPhi = P4Helpers::deltaPhi( input_phList[iph1]->phi(), input_phList[iph2]->phi() );
		double dEta = input_phList[iph1]->eta()- input_phList[iph2]->eta();
		// find out if there are overlaps
		if ( (fabs(dPhi) <= DPHI) && (fabs(dEta) <= DETA) ) {
		   keepCand[iph2]=false;
		}
	      } 
	    }  // second loop on clusters
	  }      
	} // first loop on clusters
    } // check input size

    // fill the new vector with non-overlapped candidates
    for (unsigned int i=0; i < input_phList.size(); i++) {
       if ( keepCand[i]) output_phList.push_back(input_phList[i]);
    }

    return output_phList;
}
     
} // end of namespace
