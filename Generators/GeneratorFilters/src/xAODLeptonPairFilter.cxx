/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/src/xAODLeptonPairFilter.cxx
// Description:
// Classify Pairs of leptons according to their flavor and sign combinations
// and filter on these classifications
// Same-Flavor Opposite-Sign (SFOS)
// Same-Flavor Same-Sign (SFSS)
// Opposite-Flavor Opposite-Sign (OFOS)
// Opposite-Flavor Same-Sign(OFSS)
//
// AuthorList:
//         A Long:  May 2014
//
// Header for this module:-

#include "GeneratorFilters/xAODLeptonPairFilter.h"

// Framework Related Headers:-
#include "GaudiKernel/MsgStream.h"

// Other classes used by this class:-
#include <math.h>
#include <vector>
#include <TLorentzVector.h>

//--------------------------------------------------------------------------
xAODLeptonPairFilter::xAODLeptonPairFilter(const std::string& name, 
      ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
}

//--------------------------------------------------------------------------
 xAODLeptonPairFilter::~xAODLeptonPairFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode xAODLeptonPairFilter::filterInitialize() {
//---------------------------------------------------------------------------
 return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode xAODLeptonPairFilter::filterFinalize() {
//---------------------------------------------------------------------------
 return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode xAODLeptonPairFilter::filterEvent() {
//---------------------------------------------------------------------------

  // Loop over all events in McEventCollection
  std::vector<int> vLeptonPDGIDs;
  std::vector<double> vLeptonPt;
  std::vector<double> vLeptonEta;
  std::vector<  std::vector < size_t > > vLeptonParentPDGIDs;

  // Retrieve full TruthEvent container
  const xAOD::TruthEventContainer *xTruthEventContainer;
  if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
  {
    ATH_MSG_ERROR("No TruthParticle collection with name " << "TruthEvents" << " found in StoreGate!");
    return StatusCode::FAILURE;
  }
  for (xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {
    // Loop over all particles in the event
    const xAOD::TruthEvent* genEvt = (*itr);

    unsigned int nPart = genEvt->nTruthParticles();
    for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
        const xAOD::TruthParticle* pitr =  genEvt->truthParticle(iPart);
        if( pitr->status()!=1 ) continue;
        // check stable particles only
        // We do not place requirements on their origins (updated: optionally rejecting hadron decays)
        // save pdg ids of found leptons
        // do not consider taus
        if( std::abs(pitr->pdgId()) !=  11  && std::abs(pitr->pdgId()) !=  13) continue;
        //only consider leptons which satisfy  pt and eta requirements
        if( (pitr->pt() < m_Ptmin) || std::abs(pitr->eta()) > m_EtaRange) continue;
			  if(m_onlyMassiveParents)
			  {
				  auto p = pitr;
				  bool massiveParent = false;
				  while(p)
				  {
					  auto vxp = p->prodVtx();
					  if(!vxp) break;
					  if(vxp->nIncomingParticles()!=1) break;
					  p = vxp->incomingParticle(0);
					  const int pdg = std::abs(p->pdgId());
					  if(!((pdg>=11 && pdg<=16) || pdg==22))
					  {
						  massiveParent = (p->m()>20000);
						  break;
					  }
				  }
				  if(!massiveParent) continue;
			  }

        vLeptonPDGIDs.push_back(pitr->pdgId());
        vLeptonPt.push_back(pitr->pt());
        vLeptonEta.push_back(pitr->eta());

        std::vector<size_t> parentPDG_tmp;
        for(size_t thisParent_id=0; pitr->prodVtx()->nIncomingParticles()<thisParent_id;thisParent_id++) 
          parentPDG_tmp.push_back(pitr->prodVtx()->incomingParticle(thisParent_id)->pdgId());
        vLeptonParentPDGIDs.push_back(parentPDG_tmp);
    }

  }

  int nLeptons = vLeptonPDGIDs.size();

  //Filter automatically passes if number of leptons is different than expected
  if (!(  (nLeptons >= m_nLeptons_Min) && 
  	  (m_nLeptons_Max < 0 || nLeptons <= m_nLeptons_Max) )) {
    ATH_MSG_INFO("# Lep = "<<nLeptons << " Pass"  );
    return StatusCode::SUCCESS;
  }

  int nSFOS = 0;
  int nSFSS = 0;
  int nOFOS = 0;
  int nOFSS = 0;

  int id1 = 0;
  int id2 = 0;
  //loop over all possible pair combinations of the leptons
  //which were found.  
  for (unsigned int i = 0 ; i < vLeptonPDGIDs.size() ; i++){
    //don't double count 
    for (unsigned int j = i ; j < vLeptonPDGIDs.size() ; j++){
        //Do not compare the lepton with itself
    	if(i==j) continue;
	id1 = vLeptonPDGIDs[i];
	id2 = vLeptonPDGIDs[j];
	//classify the pair and count it
	if(std::abs(id1)==std::abs(id2) && id1*id2 < 0) nSFOS+=1;
	else if(std::abs(id1)==std::abs(id2) && id1*id2 > 0) nSFSS+=1;
	else if(std::abs(id1)!=std::abs(id2) && id1*id2 < 0) nOFOS+=1;
	else if(std::abs(id1)!=std::abs(id2) && id1*id2 > 0) nOFSS+=1;
	else ATH_MSG_ERROR( "Couldn't classify lepton pair"  );
    }
  }

  //based on configuration
  //create sum of multiple pair combinations
  //which can alternatively be cut on instead
  //of the individual ones
  int nPairSum = 0;
  if(m_bUseSFOSInSum) nPairSum+=nSFOS;
  if(m_bUseSFSSInSum) nPairSum+=nSFSS;
  if(m_bUseOFOSInSum) nPairSum+=nOFOS;
  if(m_bUseOFSSInSum) nPairSum+=nOFSS;


  bool passSFOS = false;
  bool passSFSS = false;
  bool passOFOS = false;
  bool passOFSS = false;
  bool passPairSum = false;
  //Test if number of lepton pairs satisfies requirements
  //The maximum requirement is ignored if it is negative
  //No requirement is made if both min and max values are negative
  ATH_MSG_INFO("# Lep " << vLeptonPDGIDs.size() << ", "<< nSFOS << " SFOS, "<<nSFSS<< " SFSS, " << nOFOS << " OFOS, " << nOFSS << " OFSS pairs ," << nPairSum << "summed pairs"  );

  if(nSFOS >= m_nSFOS_Min && (m_nSFOS_Max<0 || nSFOS <= m_nSFOS_Max)) passSFOS=true;
  if(nSFSS >= m_nSFSS_Min && (m_nSFSS_Max<0 || nSFSS <= m_nSFSS_Max)) passSFSS=true;
  if(nOFOS >= m_nOFOS_Min && (m_nOFOS_Max<0 || nOFOS <= m_nOFOS_Max)) passOFOS=true;
  if(nOFSS >= m_nOFSS_Min && (m_nOFSS_Max<0 || nOFSS <= m_nOFSS_Max)) passOFSS=true;
  if(nPairSum >= m_nPairSum_Min && (m_nPairSum_Max<0 || nPairSum <= m_nPairSum_Max)) passPairSum=true;

  if(passSFOS && passSFSS && passOFOS && passOFSS && passPairSum){
    ATH_MSG_INFO("Pass"  );
    for (unsigned int i = 0;i<vLeptonPDGIDs.size();i++){
       int pdg = vLeptonPDGIDs[i];
       double pt = vLeptonPt[i];
       double eta = vLeptonEta[i];
       msg(MSG::DEBUG) << pdg << ": Pt = "<<pt<<", Eta = "<<eta << ", Parent PDG: ";
       for (unsigned int j=0;j<vLeptonParentPDGIDs[i].size();j++) msg(MSG::DEBUG) << vLeptonParentPDGIDs[i][j];
       msg(MSG::DEBUG) << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  // if we get here we have failed
  setFilterPassed(false);
  ATH_MSG_INFO("Fail"  );
  return StatusCode::SUCCESS;
}
