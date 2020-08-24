/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/src/LeptonPairFilter.cxx
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

#include "GeneratorFilters/LeptonPairFilter.h"

// Framework Related Headers:-
#include "GaudiKernel/MsgStream.h"

// Other classes used by this class:-
#include <math.h>
#include <vector>
#include <TLorentzVector.h>

//--------------------------------------------------------------------------
LeptonPairFilter::LeptonPairFilter(const std::string& name, 
      ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //--------------------------------------------------------------------------    
  //by default the filter demands that there are exactly 0 SFOS lepton pairs
  //if both Maximum and Minimum are negative, then no requirements
  //are made on that pair type

  //Same-Flavor Opposite-Sign (SFOS)
  declareProperty("NSFOS_Max", m_nSFOS_Max = 0);
  declareProperty("NSFOS_Min", m_nSFOS_Min = 0);

  //Same-Flavor Same-Sign (SFSS)
  declareProperty("NSFSS_Max", m_nSFSS_Max = -1);
  declareProperty("NSFSS_Min", m_nSFSS_Min = -1);

  //Opposite-Flavor Opposite-Sign (OFOS)
  declareProperty("NOFOS_Max", m_nOFOS_Max = -1);
  declareProperty("NOFOS_Min", m_nOFOS_Min = -1);

  //Opposite-Flavor Same-Sign (OFSS)
  declareProperty("NOFSS_Max", m_nOFSS_Max = -1);
  declareProperty("NOFSS_Min", m_nOFSS_Min = -1);

  //Pair Sum (ask for some combination of different 
  //lepton pairs based on configuration)
  declareProperty("NPairSum_Max", m_nPairSum_Max = -1);
  declareProperty("NPairSum_Min", m_nPairSum_Min  = -1);

  //configure which pairs to consider in sum 
  //by default don't consider any 
  declareProperty("UseSFOSInSum",m_bUseSFOSInSum = false);
  declareProperty("UseSFSSInSum",m_bUseSFSSInSum = false);
  declareProperty("UseOFOSInSum",m_bUseOFOSInSum = false);
  declareProperty("UseOFSSInSum",m_bUseOFSSInSum = false);
  
  // only count leptons with a parent with M>20 GeV
  declareProperty("OnlyMassiveParents",m_onlyMassiveParents = false);
  
  //kinematic requirements on leptons to consider
  //this is NOT a veto on the leptons outside the requirements
  declareProperty("Ptcut",m_Ptmin = 10000.);  
  declareProperty("Etacut",m_EtaRange = 10.0); 

  //the filter is only applied if the 
  //if the number of leptons falls within this range
  //otherwise the filter automatically passes
  //if max < 0 it is ignored
  declareProperty("NLeptons_Max",m_nLeptons_Max = -1);  
  declareProperty("NLeptons_Min",m_nLeptons_Min = 0); 
}

//--------------------------------------------------------------------------
 LeptonPairFilter::~LeptonPairFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode LeptonPairFilter::filterInitialize() {
//---------------------------------------------------------------------------
 return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode LeptonPairFilter::filterFinalize() {
//---------------------------------------------------------------------------
 return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LeptonPairFilter::filterEvent() {
//---------------------------------------------------------------------------

  // Loop over all events in McEventCollection
  std::vector<int> vLeptonPDGIDs;
  std::vector<double> vLeptonPt;
  std::vector<double> vLeptonEta;
  std::vector<  std::vector < int > > vLeptonParentPDGIDs;
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for(auto pitr: genEvt->particles()) {
      if( pitr->status()!=1 ) continue;
	// check stable particles only
	// We do not place requirements on their origins (updated: optionally rejecting hadron decays)
	// save pdg ids of found leptons
	// do not consider taus
	  if( std::abs(pitr->pdg_id()) !=  11  && std::abs(pitr->pdg_id()) !=  13) continue;
	  //only consider leptons which satisfy  pt and eta requirements
	  if( (pitr->momentum().perp() < m_Ptmin) || std::abs(pitr->momentum().pseudoRapidity()) > m_EtaRange) continue;
			  if(m_onlyMassiveParents)
			  {
				  auto p = pitr;
				  bool massiveParent = false;
				  while(p)
				  {
					  auto vxp = p->production_vertex();
					  if(!vxp) break;
					  if(vxp->particles_in().size()!=1) break;
					  p = vxp->particles_in().at(0);
					  const int pdg = std::abs(p->pdg_id());
					  if(!((pdg>=11 && pdg<=16) || pdg==22))
					  {
						  massiveParent = (p->generated_mass()>20000);
						  break;
					  }
				  }
				  if(!massiveParent) continue;
			  }
	      		vLeptonPDGIDs.push_back(pitr->pdg_id());
			vLeptonPt.push_back(pitr->momentum().perp());
			vLeptonEta.push_back(pitr->momentum().pseudoRapidity());
			
			std::vector<int> parentPDG_tmp;
			for(auto thisParent: pitr->production_vertex()->particles_in()) parentPDG_tmp.push_back(thisParent->pdg_id());
			vLeptonParentPDGIDs.push_back(parentPDG_tmp);
       }
#else
    for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();
	pitr!=genEvt->particles_end(); ++pitr )
      if( (*pitr)->status()==1 )
	// check stable particles only
	// We do not place requirements on their origins (updated: optionally rejecting hadron decays)
	// save pdg ids of found leptons
	// do not consider taus
	{
	  if( ((*pitr)->pdg_id() ==  11)  || 
	      ((*pitr)->pdg_id() == -11)  ||
	      ((*pitr)->pdg_id() ==  13)  || 
	      ((*pitr)->pdg_id() == -13) ){
	      	//only consider leptons which satisfy  pt and eta requirements
	        if( ((*pitr)->momentum().perp() >= m_Ptmin) && std::abs((*pitr)->momentum().pseudoRapidity()) <=m_EtaRange){
			  if(m_onlyMassiveParents)
			  {
				  auto p = *pitr;
				  bool massiveParent = false;
				  while(p)
				  {
					  auto vxp = p->production_vertex();
					  if(!vxp) break;
					  if(vxp->particles_in_size()!=1) break;
					  p = *vxp->particles_in_const_begin();
					  const int pdg = abs(p->pdg_id());
					  if(!((pdg>=11 && pdg<=16) || pdg==22))
					  {
						  massiveParent = (p->generated_mass()>20000);
						  break;
					  }
				  }
				  if(!massiveParent) continue;
			  }
	      		vLeptonPDGIDs.push_back((*pitr)->pdg_id());
			vLeptonPt.push_back((*pitr)->momentum().perp());
			vLeptonEta.push_back((*pitr)->momentum().pseudoRapidity());
			HepMC::GenVertex::particle_iterator firstParent =
			  (*pitr)->production_vertex()->particles_begin(HepMC::parents);
			HepMC::GenVertex::particle_iterator endParent =
			  (*pitr)->production_vertex()->particles_end(HepMC::parents);
			HepMC::GenVertex::particle_iterator thisParent = firstParent;
			std::vector<int> parentPDG_tmp;
			for(; thisParent != endParent; ++thisParent) parentPDG_tmp.push_back((*thisParent)->pdg_id());

			vLeptonParentPDGIDs.push_back(parentPDG_tmp);
		}
	  }//end if pdg_id
       }//end if status==1
#endif
  }//end loop over collections

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
	if(abs(id1)==abs(id2) && id1*id2 < 0) nSFOS+=1;
	else if(abs(id1)==abs(id2) && id1*id2 > 0) nSFSS+=1;
	else if(abs(id1)!=abs(id2) && id1*id2 < 0) nOFOS+=1;
	else if(abs(id1)!=abs(id2) && id1*id2 > 0) nOFSS+=1;
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
