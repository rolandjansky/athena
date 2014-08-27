/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/JetFilterWithTruthPhoton.cxx
// Description:
//  Applies the generator level filter for jet candidates
// AuthorList:
//         
// Junichi Tanaka Aug 2012
//

// Header for this module:-

#include "GeneratorFilters/JetFilterWithTruthPhoton.h"

// Framework Related Headers:-
#include "GaudiKernel/PhysicalConstants.h"

// Other classes used by this class:-
#include <math.h>
#include <vector>
#include <functional>
#include <algorithm>
using HepMC::GenVertex;
using HepMC::GenParticle;
//
//--------------------------------------------------------------------------
JetFilterWithTruthPhoton::JetFilterWithTruthPhoton(const std::string& name, 
						 ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //----------------------------    
   declareProperty("EtaRange",m_UserEta=2.7);
   declareProperty("JetThreshold",m_UserThresh=17000.); // note MeV units    
   declareProperty("JetLeadingThreshold",m_UserLeadingThresh=17000.); // note MeV units    
   declareProperty("SeedThreshold",m_Stop=1000.);    //note MeV units
   declareProperty("ConeSize",m_Cone=0.4);
   declareProperty("GridSizeEta",m_Gride=2);    
   declareProperty("GridSizePhi",m_Gridp=2);    
   declareProperty("JetNumber",m_UserNumber=1); 
   declareProperty("JetType",m_Type=true); // true if its a cone               

   declareProperty("PhotonEta",m_photonEta=2.7); 
   declareProperty("PhotonPt",m_photonPt=500.); // 500 MeV
   declareProperty("PhotonSumPt",m_photonSumPt=-999.); // -999 MeV
   declareProperty("DeltaRPhotonJet",m_dRphotonjet=0.25); 
   declareProperty("MassDijet",m_massDijet=-999.); // -999 MeV

   m_EtaRange = 0.;
   m_emaxeta = 0.;
   m_edeta = 0.;
   m_edphi = 0.;
   m_twopi = 0.;
   m_nphicell = 0;
   m_netacell = 0;
   m_nphicell2 = 0;
   m_netacell2 = 0;
}
//--------------------------------------------------------------------------
 JetFilterWithTruthPhoton::~JetFilterWithTruthPhoton(){
//--------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
StatusCode JetFilterWithTruthPhoton::filterInitialize() {
//---------------------------------------------------------------------------
  msg( MSG:: INFO) << " JetFilterWithTruthPhoton INITIALISING.  \n"  << endreq;
// set up some defaults
   m_emaxeta=6.0;
   m_twopi=4*asin(1.);
   m_edphi=m_twopi/m_grphi; // cell size
   m_edeta=2.*m_emaxeta/m_greta; // cell size
  // How many cells in the jet cluster 
   if(!m_Type){ // its the rectangular grid
     m_nphicell=m_Gridp/2;
     m_netacell=m_Gride/2; 
     m_nphicell2=m_Gridp;
     m_netacell2=m_Gride;
   }
  else{
    m_nphicell=(int) (m_Cone/m_edphi); // number of cells inside cone
    m_netacell=(int) (m_Cone/m_edeta);  //number of cells inside come
    m_nphicell2=2*m_nphicell+1; // guaranteed to be odd so that the highest cell is in middle
    m_netacell2=2*m_netacell+1;
  }
   
   if (m_UserNumber <= 1 || m_UserLeadingThresh < m_UserThresh) {
     m_UserLeadingThresh = m_UserThresh;
   }

   msg( MSG:: INFO) << "  JetFilterWithTruthPhoton filtering .  \n"  
      << " Parameters are \n " << endreq;  
  if(m_Type){
    msg( MSG:: INFO) << "  Cone algorithm.  \n"  
       << " Pt cut  = " << m_UserThresh  << ", Number= " <<  m_UserNumber  << 
     ", Cone size=" << m_Cone << ", Rapidity range " << m_UserEta << "\n "  << endreq;
   if (m_UserNumber >= 2) {
     msg( MSG:: INFO) << "  Leading Pt cut  = " << m_UserLeadingThresh << "\n" << endreq;
   }
  }
  else {
    msg( MSG:: INFO) << "  GridAlgorithm.  \n"  
       << " Pt cut  = " << m_UserThresh << ", Number= " <<  m_UserNumber << 
     ", eta size (units of 0.06) =" << m_Gride <<", phi  size (units of 0.06) =" 
       << m_Gridp <<  ", Rapidity range " << m_UserEta << " \n "<< endreq;    
   if (m_UserNumber >= 2) {
     msg( MSG:: INFO) << "  Leading Pt cut  = " << m_UserLeadingThresh << "\n" << endreq;
   }
  }

  msg( MSG::INFO) << "Truth photon condition pt = " << m_photonEta << " eta = " << m_photonEta << " dR(photon,jet) = " << m_dRphotonjet << endreq;
  msg( MSG::INFO) << "Truth photon sum  pt = " << m_photonSumPt << " (if negative, photon info is not used in the filter)" << endreq;
  msg(MSG::INFO) << "Dijet mass = " << m_massDijet << " (if negative, mjj info is not used in the filter)" << endreq;

  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------------------
StatusCode JetFilterWithTruthPhoton::filterFinalize() {
//---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------------------
StatusCode JetFilterWithTruthPhoton::filterEvent() {
  //---------------------------------------------------------------------------
  msg( MSG:: INFO) << "  JetFilterWithTruthPhoton filtering .  \n"  << endreq;  
  // Loop over all events in McEventCollection: find particles and fill grid
  double m_etgrid[m_grphi][m_greta]; // clean it out before we start
  bool m_etgridused[m_grphi][m_greta]; //will use this to mark off cells after they are added to jets
  for(int ie=0; ie < m_greta; ++ie) { //initialise everything to be safe
    for(int ip=0; ip < m_grphi; ++ip) {
      m_etgrid[ip][ie]=0.;
      m_etgridused[ip][ie]=false;
    }
  }

  std::vector<HepMC::GenParticle*> MCTruthPhotonList;
  MCTruthPhotonList.clear();

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    // Loop over all particles in the event and build up the grid
    const HepMC::GenEvent* genEvt = (*itr);
    for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();
	pitr!=genEvt->particles_end(); ++pitr ){
      if( (*pitr)->status() == 1 ){// stables only
	if( ((*pitr)->pdg_id() != 13 ) &&  ((*pitr)->pdg_id() != -13 ) 
	    &&((*pitr)->pdg_id() != 12 ) && ((*pitr)->pdg_id() != -12 ) 
	    &&((*pitr)->pdg_id() != 14 ) && ((*pitr)->pdg_id() != -14 ) 
	    &&((*pitr)->pdg_id() != 16 ) && ((*pitr)->pdg_id() != -16 ) 
	    && (fabs((*pitr)->momentum().pseudoRapidity()) <= m_emaxeta) 
	     ){	// no neutrinos or muons and particles must be in active range
	  int ip,ie;
	  //	  std::cout << (*pitr)->momentum().phi() << "eta" << (*pitr)->momentum().pseudoRapidity() << std::endl;
	  ip=(int) ((m_twopi/2.+ (*pitr)->momentum().phi())/m_edphi); //phi is in range -pi to pi
	  ie=(int) (((*pitr)->momentum().pseudoRapidity()+m_emaxeta)/m_edeta);
	  //	  	  std::cout << ip << "   "<< ie <<std::endl;
	  //		  std::cout << " true rap " << (*pitr)->momentum().pseudoRapidity() << "false rap " << (ie+0.5)*m_edeta-m_emaxeta << " True phi " <<  (*pitr)->momentum().phi() << "  false phi  "  << -m_twopi/2.+(ip+0.5)*m_edphi << std::endl;
	  if( (ie<0) || (ie>=  m_greta)){ // its outside the ends so we should not be here
	    msg( MSG::FATAL) << "  Jet too close to end"  << endreq;
	    return StatusCode::FAILURE;
	  }
	  while(ip<0)
	    ip+=m_grphi; //fix phi wrapping note that this is done after rr is calculated
	  while(ip>m_grphi-1)
	    ip-=m_grphi; //fix phi wrapping note that this is done after rr is calculated
	  

	  m_etgrid[ip][ie]=m_etgrid[ip][ie]+(*pitr)->momentum().perp(); // fortran had pt here
	  if ((*pitr)->pdg_id() == 22 && 
	      (*pitr)->momentum().perp() > m_photonPt && 
	      fabs((*pitr)->momentum().pseudoRapidity()) < m_photonEta) {
	    MCTruthPhotonList.push_back((*pitr));
	  }
#if 0
	  if ((*pitr)->pdg_id() == 22) {
	    msg(MSG::DEBUG) << "Truth photon pt = " << (*pitr)->momentum().perp() << " eta = " << (*pitr)->momentum().pseudoRapidity() << endreq;
	  }
#endif
	}
      }
    }	
  }
  //
  // find the highest cell
  // we loop here until we cannot find more jets
  double ethigh=2.*m_Stop; // et of highest cell
  //  int ltest=0;
  int hasLeadingJet = 0;
  while (ethigh>m_Stop){// stop looping when there are no cells left above threshold; 
    ethigh=0.;
    int    etahigh=0;
    int    phihigh=0;
    for (int ie0=m_netacell; ie0< m_greta-m_netacell; ++ie0){// only look away from the edges
      for (int ip0=0; ip0<m_grphi; ++ip0){
	if(m_etgrid[ip0][ie0]>ethigh && !m_etgridused[ip0][ie0]){
	  ethigh=m_etgrid[ip0][ie0];
	  etahigh=ie0;
	  phihigh=ip0;
	}
      }
    }
    //std::cout << "Highest cell eta phi  " << etahigh << " , " << phihigh << "energy " << ethigh << std::endl;
    //std::cout << "interesting cells  83 and 129  "<< m_etgrid[83][129] << "   " <<  m_etgrid[84][129] << std::endl;
    /*
    if(ltest<1 ){
      for (int ie0=1+m_netacell; ie0< m_greta-m_netacell-1; ++ie0){// only look away from the edges
	for (int ip0=0; ip0<m_grphi; ++ip0){
	  std::cout << "ip  " <<ip0 << "ie " <<ie0 << "energy  "<<m_etgrid[ip0][ie0]/GeV << std::endl;
	}
      } 
      std::cout << "Highest cell "<< ethigh/Gaudi::Units::GeV  << "eta " << etahigh*m_edeta-m_emaxeta << "phi  " << -m_twopi/2.+m_edphi*phihigh << std::endl;
      ltest++;
    }
    */
    if(ethigh>m_Stop){ // this passes only if there is tower above threshold
      // new jet
      CLHEP::HepLorentzVector FoundJet;
      double jetpx=0.;
      double jetpy=0.;
      double jetpz=0.;
      double jete=0.;
      //      std::cout <<" cell dimensions"  << m_netacell<< "   " << m_nphicell<< "   " <<m_netacell2<< "   " <<m_nphicell2 << std::endl ;    //  std::cout <<" high cell"  << etahigh<< "   " << phihigh<<  std::endl ;

      if(!m_Type){//grid handle differantly if there are an even number of cells
	//std::cout << "etacall, phicell" << m_netacell2 << m_netacell2%2 << m_nphicell2 << m_nphicell2%2 << std::endl;
	if(m_netacell2%2 == 0 && m_nphicell2%2 == 1){ //eta even
	  double sum=0.;
	  double sum1=0.;
	  for(int ip0=0; ip0<m_nphicell2 ; ip0++){
	    int ip1=ip0-m_nphicell+phihigh;
	    sum=sum+m_etgrid[ip1][etahigh-1];
	    sum1=sum1+m_etgrid[ip1][etahigh+1];
	  }
	  if(sum < sum1) {
	    etahigh=etahigh+1; //shift over by one
	  }
	}
	if(m_netacell2%2 == 1 && m_nphicell2%2 == 0){ // phi even
	  double sum=0.;
	  double sum1=0.;
	  for(int ie0=0; ie0<m_netacell2 ; ie0++){
	    int ie1=ie0-m_netacell+etahigh;
	    sum=sum+m_etgrid[(phihigh-1)%m_grphi][ie1];
	    sum1=sum1+m_etgrid[(phihigh+1)%m_grphi][ie1];
	  }
	  if(sum < sum1) {
	    phihigh=(phihigh+1)%m_grphi; //shift over by one
	  }
	}
	if(m_netacell2%2 == 0 && m_nphicell2%2 == 0){ // both even
	  double sum=0.;
	  double sum1=0.;
	  double sum2=0.;
	  double sum3=0.;
	  //std::cout << "insdie mover" << m_netacell2 <<  m_nphicell2 << std::endl;
	  for(int ie0=0; ie0<m_netacell2; ie0++){
	    //std::cout << " ie0 " << ie0 << std::endl;
	    for(int ip0=0; ip0<m_nphicell2 ; ip0++){
	      int ip1=ip0-m_nphicell+phihigh;
	      int ie1=ie0-m_netacell+etahigh;
	      //std::cout << "checking loop " << ip1 << "   "<< ie1 << std::endl;
	      if(!m_etgridused[ip1][ie1]) sum=sum+m_etgrid[ip1][ie1];
	      if(!m_etgridused[ip1][ie1+1]) sum1=sum1+m_etgrid[ip1][ie1+1];
	      if(!m_etgridused[ip1+1][ie1]) sum2=sum2+m_etgrid[(ip1+1)%m_grphi][ie1];
	      if(!m_etgridused[ip1+1][ie1+1]) sum3=sum3+m_etgrid[(ip1+1)%m_grphi][ie1+1];
	    }
	  }
	  if(sum < sum1 && sum2 < sum1 && sum3 < sum1) etahigh=etahigh+1;
	  if(sum < sum2 && sum1 <= sum2 && sum3 < sum2) phihigh=(phihigh+1)%m_grphi;
	  if(sum < sum3 && sum2 <= sum3 && sum1 <= sum3) {
	    etahigh=etahigh+1;
	    phihigh=(phihigh+1)%m_grphi;
	  }
	  //std::cout << "shifting phihigh " << phihigh << "  etahigh " <<etahigh<< "list of cells" << sum << sum1 <<sum2 <<sum3 << std::endl;
	}
      }
      //add up stuff around high cell 
      for (int ie0=0; ie0< m_netacell2; ++ie0){
	int ie1=ie0-m_netacell+etahigh;
	if( (ie1<0) || (ie1>=  m_greta)){ // its outside the ends so we should not be here
	  msg(MSG:: FATAL) << "  Jet too close to end"  << endreq;
	  return StatusCode::FAILURE;
	}
	for (int ip0=0; ip0<m_nphicell2; ++ip0){
	  int ip1=ip0-m_nphicell+phihigh;
     // are we using a cone, if so check that its inside
	  double rr=(ie1-etahigh)*(ie1-etahigh)*m_edeta*m_edeta+(ip1-phihigh)*(ip1-phihigh)*m_edphi*m_edphi;
	  while(ip1<0)
	    ip1+=m_grphi; //fix phi wrapping note that this is done after rr is calculated
	  while(ip1>m_grphi-1)
	    ip1-=m_grphi; //fix phi wrapping note that this is done after rr is calculated

	  if(rr<m_Cone*m_Cone || !m_Type){ // make sure that its inside
	    //check that the cell can be used and add energy to jet and mark the cell as used
	    if(!m_etgridused[ip1][ie1]){
	      m_etgridused[ip1][ie1]=true;
	      jetpx=jetpx+m_etgrid[ip1][ie1]*cos(-m_twopi/2.+(ip1+0.5)*m_edphi);
	      jetpy=jetpy+m_etgrid[ip1][ie1]*sin(-m_twopi/2.+(ip1+0.5)*m_edphi);
	      jetpz=jetpz+m_etgrid[ip1][ie1]*sinh((ie1+0.5)*m_edeta-m_emaxeta);
	      jete=jete+m_etgrid[ip1][ie1]*cosh((ie1+0.5)*m_edeta-m_emaxeta);
	    }
	  }
	}
      }
      FoundJet.setPx(jetpx);  
      FoundJet.setPy(jetpy);  
      //std::cout << "long p" << jetpz << std::endl;
      FoundJet.setPz(jetpz);  
      FoundJet.setE(jete); 
      if(fabs(FoundJet.pseudoRapidity())< m_UserEta && 
	 FoundJet.vect().perp() > m_UserThresh){
	int foundPhotons = 0;
	//std::cout << "# of photons = " << MCTruthPhotonList.size() << std::endl;
	//std::cout << "Jet eta and phi = " << FoundJet.pseudoRapidity() << " " << FoundJet.phi() << std::endl;
	if (m_photonSumPt >= 0. && MCTruthPhotonList.size() > 0) {
	  double sumPt = 0.;
	  for (unsigned int ig=0;ig<MCTruthPhotonList.size();++ig) {
	    double deta = fabs(MCTruthPhotonList[ig]->momentum().pseudoRapidity()-FoundJet.pseudoRapidity());
	    double dphi = fabs(MCTruthPhotonList[ig]->momentum().phi()-FoundJet.phi());
	    if (dphi > M_PI) dphi = 2.*M_PI - dphi;
	    double dR = sqrt(deta*deta+dphi*dphi);
	    if (dR < m_dRphotonjet) {
	      sumPt += MCTruthPhotonList[ig]->momentum().perp();
	    }
	  }
	  if (sumPt > m_photonSumPt) foundPhotons = 1;
	}
	if (m_photonSumPt < 0.) foundPhotons = 1; // always OK 
	if (foundPhotons) {
	  m_Jets.push_back(FoundJet);   //OK we found one. add it to the list  if its inside the eta region
	  if (FoundJet.vect().perp() > m_UserLeadingThresh) {
	    hasLeadingJet = 1;
	  }
	}
      }
    }
  }
  sort(m_Jets.begin(),m_Jets.end(),std::greater<JetFilter::McObj>());
  msg(MSG::DEBUG)<< "  Summary.  "  
      << " Number of jets found   = " <<  m_Jets.size() <<  " \n "<< endreq;   
  if (m_Jets.size()>0) {
    msg(MSG::DEBUG)<< " Highest pt (in GeV)  " <<  (m_Jets[0].P().perp()/Gaudi::Units::GeV)  <<   "   Rapidity " <<m_Jets[0].P().pseudoRapidity()<<  "   Phi "<< m_Jets[0].P().phi() << "\n "<< endreq; 
    if (m_Jets.size()>1) msg(MSG::DEBUG)<< " Second Highest pt (in GeV)  " <<  (m_Jets[1].P().perp()/Gaudi::Units::GeV)  <<   "   Rapidity " <<m_Jets[1].P().pseudoRapidity()<<  "   Phi "<< m_Jets[1].P().phi() << "\n "<< endreq; 
    msg(MSG::DEBUG) << " Lowest pt (in GeV)  " <<  (m_Jets[m_Jets.size()-1].P().perp()/Gaudi::Units::GeV)  <<  "   Rapidity " <<m_Jets[m_Jets.size()-1].P().pseudoRapidity() <<  "   Phi " << m_Jets[m_Jets.size()-1].P().phi() << "\n "<< endreq; 
  }
  int isOK = 1;
  if (m_UserNumber >= 2 && m_UserLeadingThresh > m_UserThresh && !hasLeadingJet) isOK = 0;
  //std::cout << "# of jets = " << m_Jets.size() << std::endl;
  if (m_UserNumber >= 2 && m_Jets.size()>=2 && m_massDijet >= 0.) {
    isOK = 0;
    if (hasLeadingJet) {
      for (unsigned i=0;i<m_Jets.size()-1;++i) {
	for (unsigned j=i+1;j<m_Jets.size();++j) {
	  CLHEP::HepLorentzVector pjj = m_Jets[i].P()+m_Jets[j].P();
	  double mjj = pjj.m();
	  //std::cout << i << " " << j << " mjj = " << mjj << std::endl;
	  if (mjj > m_massDijet && (m_Jets[i].P().perp() > m_UserLeadingThresh || m_Jets[j].P().perp() > m_UserLeadingThresh)) {
	    isOK = 1;
	    break;
	  }
	}
	if (isOK == 1) break;
      }
    }
  }
  //std::cout << "isOK = " << isOK << std::endl;

#if 0
  if (0) {
    msg(MSG::DEBUG) << " Summary  isOK " << isOK << " Number of jets found   = " << m_Jets.size() << " \n "<< endreq;
    if (m_Jets.size()>0) {
      msg(MSG::DEBUG) << " Highest pt (in GeV)  " <<  (m_Jets[0].P().perp()/Gaudi::Units::GeV)  <<   "   Rapidity " <<m_Jets[0].P().pseudoRapidity()<<  "   Phi "<< m_Jets[0].P().phi() << endreq;
      if (m_Jets.size()>1) msg(MSG::DEBUG) << " Second Highest pt (in GeV)  " <<  (m_Jets[1].P().perp()/Gaudi::Units::GeV)  <<   "   Rapidity " <<m_Jets[1].P().pseudoRapidity()<<  "   Phi "<< m_Jets[1].P().phi() << endreq; 
      msg(MSG::DEBUG) << " Lowest pt (in GeV)  " <<  (m_Jets[m_Jets.size()-1].P().perp()/Gaudi::Units::GeV)  <<  "   Rapidity " <<m_Jets[m_Jets.size()-1].P().pseudoRapidity() <<  "   Phi " << m_Jets[m_Jets.size()-1].P().phi() << endreq; 
    }
  }
#endif

  // now look at the jets and check the filter
  if(m_Jets.size() >= unsigned(m_UserNumber)){
    //if(m_Jets[m_UserNumber-1].P().perp()>m_UserThresh){ 
    if (isOK) {
      //it passed
      m_Jets.clear(); //clean it out
      return StatusCode::SUCCESS; 
    }
  }
  //std::cout << "FAIL" << std::endl;
  setFilterPassed(false); // it failed to find any useful jets
  m_Jets.clear(); //clean out the found jets
  return StatusCode::SUCCESS;
}
