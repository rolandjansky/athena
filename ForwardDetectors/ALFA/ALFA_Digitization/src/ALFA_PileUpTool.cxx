/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_Digitization/ALFA_PileUpTool.h"

#include "ALFA_SimEv/ALFA_HitCollection.h"
#include "ALFA_SimEv/ALFA_Hit.h"
#include "ALFA_SimEv/ALFA_ODHitCollection.h"

#include "ALFA_RawEv/ALFA_DigitCollection.h"
#include "ALFA_SimEv/ALFA_ODHit.h"
#include "ALFA_RawEv/ALFA_ODDigitCollection.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandPoissonQ.h"

#include <algorithm>

#include <map>

#include "PileUpTools/PileUpMergeSvc.h"
#include "Identifier/Identifier.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// from now old part

#include <math.h>
#include <functional>

//for truth particles
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "HepMC/SimpleVector.h"

#include "TTree.h"

#include "GaudiKernel/ITHistSvc.h"

#include "PathResolver/PathResolver.h"

// *********************************************************************************

ALFA_PileUpTool::ALFA_PileUpTool(const std::string& type, 
								const std::string& name,
								const IInterface* parent) :
  PileUpToolBase(type, name, parent),  
  m_mergeSvc    	("PileUpMergeSvc", name),
  m_atRndmGenSvc    ("AtRndmGenSvc"  , name),
  m_rndEngine		(0), 
  m_sigma0          (0.1),
  m_sigma1          (1.0),
  m_meanE_dep       (0.0863), //MeV 
  m_meanN_photo     (4.11), 
  m_mean            (0.0), 
  m_stdDev          (1.0),
  m_AmplitudeCut    (0.24)  
//  m_fillRootTree    (false) 
{ 
  
  m_SimHitCollectionName = "ALFA_HitCollection";
  m_SimODHitCollectionName = "ALFA_ODHitCollection";
  
  m_digitCollection = 0;	// initializing to null pointer
  m_ODdigitCollection = 0;	// initializing to null pointer
  
  m_mergedALFA_HitList = 0;
  m_mergedALFA_ODHitList = 0;
  
  m_key_DigitCollection  = "ALFA_DigitCollection";
  m_key_ODDigitCollection  = "ALFA_ODDigitCollection";  

  // Properties  
  declareProperty("SimHitCollection", m_SimHitCollectionName, "Name of the input ALFA Sim Hit Collection of simulated hits");
  declareProperty("SimODHitCollection", m_SimODHitCollectionName, "Name of the input ALFA Sim OD Hit Collection of simulated hits");  
 
  declareProperty("ALFA_DigitCollection", m_key_DigitCollection, "Name of the Collection to hold the output from the ALFA main detector digitization"); 
  declareProperty("ALFA_ODDigitCollection", m_key_ODDigitCollection, "Name of the Collection to hold the output from the ALFA OD digitization");
  
  declareProperty("RndmSvc", m_atRndmGenSvc, "Random Number Service used in ALFA digitization");
  declareProperty("mergeSvc", m_mergeSvc, "Store to hold the pile-ups");

//  declareProperty("fillRootTree", m_fillRootTree);  
   
  
  declareProperty("sigma0", m_sigma0); 
  declareProperty("sigma1", m_sigma1);
  declareProperty("meanE_dep", m_meanE_dep);
  declareProperty("meanN_photo", m_meanN_photo);
  declareProperty("mean", m_mean);
  declareProperty("stdDev", m_stdDev);
  declareProperty("AmplitudeCut", m_AmplitudeCut);  
//  declareProperty("fillRootTree", m_fillRootTree);
}




StatusCode ALFA_PileUpTool::initialize(){
  
  ATH_MSG_DEBUG ("ALFA_digiti initialize() called"); 
  
  ATH_MSG_DEBUG (" Pedestal: " << m_AmplitudeCut);
  
  if (m_atRndmGenSvc.retrieve().isFailure()) { 
    ATH_MSG_FATAL ( "Could not retrieve RandomNumber Service!" ); 
    return StatusCode::FAILURE; 
  }
  else { 
    ATH_MSG_DEBUG ( "Retrieved RandomNumber Service" ); 
  }
  
  m_mergedALFA_HitList = new ALFA_HitCollection();
  m_mergedALFA_ODHitList = new ALFA_ODHitCollection();
  
  return StatusCode::SUCCESS;
}




// *********************************************************************************

StatusCode ALFA_PileUpTool::processAllSubEvents() {

  ATH_MSG_DEBUG ("ALFA_PileUpTool::processAllSubEvents()");

  //retrieve the PileUpMergeSvc if necessary
  if(!m_mergeSvc) {
    if(!m_mergeSvc.retrieve().isSuccess() || !m_mergeSvc) {
      ATH_MSG_FATAL("digitize: Could not find PileUpMergeSvc");
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("digitize: retrieved PileUpMergeSvc");
  }
  else ATH_MSG_DEBUG("digitize: PileUpMergeSvc already available"); 

  typedef PileUpMergeSvc::TimedList<ALFA_HitCollection>::type TimedALFAHitCollList;
  typedef PileUpMergeSvc::TimedList<ALFA_ODHitCollection>::type TimedALFAODHitCollList;
  
  TimedALFAHitCollList tHitCollList;
  TimedALFAODHitCollList tODHitCollList;
  
  
  if (m_mergeSvc->retrieveSubEvtsData(m_SimHitCollectionName, tHitCollList).isFailure()) { 

    ATH_MSG_FATAL ( "Could not fill TimedALFAHitCollList" ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( "Retrieved TimedALFAHitCollList" ); }
  
  ATH_MSG_DEBUG ( "PileUp: Merge " << tHitCollList.size() << " ALFA_HitCollection with key" << m_SimHitCollectionName );
  
  
  if (m_mergeSvc->retrieveSubEvtsData(m_SimODHitCollectionName, tODHitCollList).isFailure()) { 

    ATH_MSG_FATAL ( "Could not fill TimedALFAODHitCollList" ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( "Retrieved TimedALFAODHitCollList" ); }
  
  ATH_MSG_DEBUG ( "PileUp: Merge " << tODHitCollList.size() << " ALFA_ODHitCollection with key " << m_SimODHitCollectionName );
  
  
  TimedHitCollection<ALFA_Hit> tALFAhit;

  TimedALFAHitCollList::iterator iHitColl (tHitCollList.begin());
  TimedALFAHitCollList::iterator eHitColl (tHitCollList.end());
  
  while (iHitColl != eHitColl) {
    
    const ALFA_HitCollection* tmpColl(iHitColl->second);
    
    tALFAhit.insert(iHitColl->first, tmpColl);
    
    ATH_MSG_DEBUG ( " ALFA_HitCollection found with " << tmpColl->size() << " hits " << iHitColl->first );
    
    ++iHitColl;
  }
  
  TimedHitCollection<ALFA_ODHit> tALFAODhit;

  TimedALFAODHitCollList::iterator iODHitColl  (tODHitCollList.begin());
  TimedALFAODHitCollList::iterator eODHitColl  (tODHitCollList.end());
  
  while (iODHitColl != eODHitColl) {
    
    const ALFA_ODHitCollection* tmpColl(iODHitColl->second);
    
    tALFAODhit.insert(iODHitColl->first, tmpColl);
    
    ATH_MSG_DEBUG ( " ALFA_ODHitCollection found with " << tmpColl->size() << " hits " << iODHitColl->first );
    
    ++iODHitColl;
  }

  // preparing containers 
  
  m_rndEngine = m_atRndmGenSvc->GetEngine("ALFARndEng");

 
  if (recordCollection(this->evtStore(), m_key_DigitCollection).isFailure()) { 
    
    ATH_MSG_FATAL ( " ALFA_PileUpTool::processAllSubEvents(): Could not record the empty ALFA digit container in StoreGate " ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( " ALFA_PileUpTool::processAllSubEvents(): ALFA Digit container is recorded in StoreGate " ); }
  
  
  if (recordODCollection(this->evtStore(), m_key_ODDigitCollection).isFailure()) { 
    
    ATH_MSG_FATAL ( " ALFA_PileUpTool::processAllSubEvents(): Could not record the empty ALFA OD digit container in StoreGate " ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( " ALFA_PileUpTool::processAllSubEvents(): ALFA OD Digit container is recorded in StoreGate " ); }  
 
 
  // filling containers 
  
  ALFA_MD_info(tALFAhit);
  
  StatusCode sc = fill_MD_DigitCollection(m_rndEngine);
  if  (sc.isFailure()) {
    ATH_MSG_WARNING ("ALFA_PileUpTool::fill_MD_DigitCollection() failed");  
    return StatusCode::SUCCESS;     
  }
  
    
  ALFA_OD_info(tALFAODhit);
  
  sc = fill_OD_DigitCollection(m_rndEngine);
  if  (sc.isFailure()) {
    ATH_MSG_WARNING ("ALFA_PileUpTool::fill_OD_DigitCollection() failed");  
    return StatusCode::SUCCESS;     
  }  
  
  return StatusCode::SUCCESS;
}




StatusCode ALFA_PileUpTool::prepareEvent(const unsigned int nInputEvents){

  ATH_MSG_DEBUG ( "ALFA_PileUpTool::prepareEvent() called for " << nInputEvents << " input events" );
 
  StatusCode sc = recordCollection(this->evtStore(), m_key_DigitCollection);
 
  if (sc.isFailure()) { ATH_MSG_FATAL ( " ALFA_PileUpTool::prepareEvent(): Could not record the empty digit container in StoreGate " ); return sc; }
  else                { ATH_MSG_DEBUG ( " ALFA_PileUpTool::prepareEvent(): Digit container is recorded in StoreGate " ); }
  
  sc = recordODCollection(this->evtStore(), m_key_ODDigitCollection);
 
  if (sc.isFailure()) { ATH_MSG_FATAL ( " ALFA_PileUpTool::prepareEvent(): Could not record the empty digit OD container in StoreGate " ); return sc; }
  else                { ATH_MSG_DEBUG ( " ALFA_PileUpTool::prepareEvent(): Digit OD container is recorded in StoreGate " ); }
  
  m_mergedALFA_HitList->clear();
  m_mergedALFA_ODHitList->clear();
  
  return StatusCode::SUCCESS;
}


StatusCode ALFA_PileUpTool::processBunchXing(int bunchXing,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents) {
  ATH_MSG_DEBUG ( "ALFA_PileUpTool::processBunchXing() " << bunchXing );
  SubEventIterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; iEvt++) {
    StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
    //PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index()); not used 
    ATH_MSG_VERBOSE("SubEvt StoreGate " << seStore.name() << " :"
                    << " bunch crossing : " << bunchXing
                    << " time offset : " << iEvt->time()
                    << " event number : " << iEvt->ptr()->eventNumber()
                    << " run number : " << iEvt->ptr()->runNumber()
                    );
  
    
    const ALFA_HitCollection* tmpHitColl = 0;
   
    if (!seStore.retrieve(tmpHitColl, m_SimHitCollectionName).isSuccess()) {
      
      ATH_MSG_ERROR ("SubEvent ALFA_HitCollection not found in StoreGate " << seStore.name() );
      
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG ("SubEvent, ALFA_HitCollection found with " << tmpHitColl->size() << " hits");
    
    ALFA_HitCollection::const_iterator iHitColl = tmpHitColl->begin();
    ALFA_HitCollection::const_iterator eHitColl = tmpHitColl->end();
   
    for (; iHitColl!=eHitColl; ++iHitColl) m_mergedALFA_HitList->push_back((*iHitColl));
    
    const ALFA_ODHitCollection* tmpODHitColl = 0;
   
    if (!seStore.retrieve(tmpODHitColl, m_SimODHitCollectionName).isSuccess()) {
      
      ATH_MSG_ERROR ( "SubEvent ALFA_ODHitCollection not found in StoreGate " << seStore.name() );
      
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG ( "ALFA_ODHitCollection found with " << tmpODHitColl->size() << " hits" );
    
    ALFA_ODHitCollection::const_iterator iODHitColl = tmpODHitColl->begin();
    ALFA_ODHitCollection::const_iterator eODHitColl = tmpODHitColl->end();
   
    for (; iODHitColl!=eODHitColl; ++iODHitColl) m_mergedALFA_ODHitList->push_back((*iODHitColl));
    
    m_rndEngine = m_atRndmGenSvc->GetEngine("ALFARndEng");
  }
  
  return StatusCode::SUCCESS;
}




StatusCode ALFA_PileUpTool::mergeEvent(){


  ALFA_MD_info(m_mergedALFA_HitList);
  
  StatusCode sc = fill_MD_DigitCollection(m_rndEngine);
  if  (sc.isFailure()) {
    ATH_MSG_WARNING ("ALFA_PileUpTool::fill_MD_DigitCollection() failed");  
    return StatusCode::SUCCESS;     
  }
    
  
  ALFA_OD_info(m_mergedALFA_ODHitList);
  
  sc = fill_OD_DigitCollection(m_rndEngine);
  if  (sc.isFailure()) {
    ATH_MSG_WARNING ("ALFA_PileUpTool::fill_OD_DigitCollection() failed");  
    return StatusCode::SUCCESS;     
  }   
  
  return StatusCode::SUCCESS;
}

StatusCode ALFA_PileUpTool::finalize() { return StatusCode::SUCCESS; }



StatusCode ALFA_PileUpTool::recordCollection(ServiceHandle<StoreGateSvc>& evtStore, std::string key_digitCnt) 
{
  ATH_MSG_DEBUG ("ALFA_Digitization::recordCollection"); 
    
  m_digitCollection = new ALFA_DigitCollection();
  
  StatusCode sc = evtStore->record(m_digitCollection, key_digitCnt);

  return sc;
}


StatusCode ALFA_PileUpTool::recordODCollection(ServiceHandle<StoreGateSvc>& evtStore, std::string key_ODdigitCnt) {

  ATH_MSG_DEBUG ("ALFA_Digitization::recordODCollection");  
    
  m_ODdigitCollection = new ALFA_ODDigitCollection();
  
  StatusCode sc = evtStore->record(m_ODdigitCollection, key_ODdigitCnt);
  
  return sc;
}




void ALFA_PileUpTool::ALFA_MD_info(TimedHitCollection<ALFA_Hit>& tHitALFA) 
{ 

// cleaning
  
  for ( int l = 0; l < 8; l++ )
  {
    for ( int i = 0; i < 20; i++ )
    {
      for ( int j = 0; j < 64;  j++ )
	{
	
        m_E_fib[l][i][j] = 0.;
		m_fibres[l][i][j] = 0;
	
	} 
    }
  }  
         	   
// cleaning end 

  int fiber, plate, sign, station;

  TimedHitCollection<ALFA_Hit>  thpc = tHitALFA;
  TimedHitCollection<ALFA_Hit>::const_iterator i, e, it;
 
  while (thpc.nextDetectorElement(i, e)) for (it = i; it != e; it++) {
       
    station = (*it)->GetStationNumber();
    plate   = (*it)->GetPlateNumber();
    fiber   = (*it)->GetFiberNumber();
    sign    = (*it)->GetSignFiber();
    
    ATH_MSG_DEBUG ("station=" << station << ", plate= "<< plate << ", fiber=" << fiber << ", sign=" << sign << ", dep energy=" << ((*it)->GetEnergyDeposit()));    

    m_E_fib[station-1][2*(plate-1)+(1-sign)/2][fiber-1] += ((*it)->GetEnergyDeposit());
      
  }  
  
}




void ALFA_PileUpTool::ALFA_MD_info(const ALFA_HitCollection* ALFA_HitCollection) 
{

// cleaning
  
  for ( int l = 0; l < 8; l++ )
  {
    for ( int i = 0; i < 20; i++ )
    {
      for ( int j = 0; j < 64;  j++ )
	{
	
        m_E_fib[l][i][j] = 0.;
		m_fibres[l][i][j] = 0;
			
	
	} 
    }
  }  
         	   
// cleaning end 

  int fiber, plate, sign, station;

  ALFA_HitConstIter it    = ALFA_HitCollection->begin();
  ALFA_HitConstIter itend = ALFA_HitCollection->end();
  
  for (; it != itend; it++) {

    station = it->GetStationNumber();
    plate   = it->GetPlateNumber();
    fiber   = it->GetFiberNumber();
    sign    = it->GetSignFiber();
    
    ATH_MSG_DEBUG ("station=" << station << ", plate= "<< plate << ", fiber=" << fiber << ", sign=" << sign << ", dep energy=" << it->GetEnergyDeposit());    

    m_E_fib[station-1][2*(plate-1)+(1-sign)/2][fiber-1] += it->GetEnergyDeposit();
  

  }
}




void ALFA_PileUpTool::ALFA_OD_info(TimedHitCollection<ALFA_ODHit>& tODHitALFA) 
{

// cleaning
  
  for ( int l = 0; l < 8; l++ ){
    for ( int i = 0; i < 2; i++ ){
      for ( int j = 0; j < 3; j++ ){
        for ( int k = 0; k < 30; k++ ){
	  
            m_E_ODfib[l][i][j][k] = 0.; 
	  
	  }
      }
    }
  }         	  
  
// cleaning end

  int fiber, plate, sign, side, station; 


  TimedHitCollection<ALFA_ODHit>  thpc = tODHitALFA;
  TimedHitCollection<ALFA_ODHit>::const_iterator i, e, it;
  
  while (thpc.nextDetectorElement(i, e)) for (it = i; it != e; it++) {
     
    station = (*it)->GetStationNumber();
    side    = (*it)->GetODSide();    
    plate   = (*it)->GetPlateNumber();
    fiber   = (*it)->GetFiberNumber();
    sign    = (*it)->GetSignFiber();

       
    ATH_MSG_DEBUG ("station=" << station << ", side=" << side << ", plate= "<< plate << ", fiber=" << fiber << ", sign=" << sign << ", dep energy=" << ((*it)->GetEnergyDeposit()));

    if (sign==0) m_E_ODfib[station-1][side-1][plate-1][fiber+15] += ((*it)->GetEnergyDeposit());
    else m_E_ODfib[station-1][side-1][plate-1][fiber] += ((*it)->GetEnergyDeposit());
    
  }  

}




void ALFA_PileUpTool::ALFA_OD_info(const ALFA_ODHitCollection* ALFA_ODHitCollection) 
{
  
  // cleaning
  
  for ( int l = 0; l < 8; l++ ){
    for ( int i = 0; i < 2; i++ ){
      for ( int j = 0; j < 3; j++ ){
        for ( int k = 0; k < 30; k++ ){
	  
            m_E_ODfib[l][i][j][k] = 0.; 
	  
	  }
      }
    }
  }         	  
  
// cleaning end

  int fiber, plate, sign, side, station; 
  
  
  ALFA_ODHitConstIter it    = ALFA_ODHitCollection->begin();
  ALFA_ODHitConstIter itend = ALFA_ODHitCollection->end();

  for (; it != itend; it++) {

     
    station = it->GetStationNumber();
    side    = it->GetODSide();    
    plate   = it->GetPlateNumber();
    fiber   = it->GetFiberNumber();
    sign    = it->GetSignFiber();

       
    ATH_MSG_DEBUG ("station=" << station << ", side=" << side << ", plate= "<< plate << ", fiber=" << fiber << ", sign=" << sign << ", dep energy=" << it->GetEnergyDeposit());
    
    if (sign==0) m_E_ODfib[station-1][side-1][plate-1][fiber+15] += it->GetEnergyDeposit();
    else m_E_ODfib[station-1][side-1][plate-1][fiber] += it->GetEnergyDeposit();
    
  }
}




StatusCode ALFA_PileUpTool::fill_MD_DigitCollection(CLHEP::HepRandomEngine* rndEngine) {
    
  ATH_MSG_DEBUG(" ALFA_PileUpTool::fill_MD_DigitCollection()");
  
  double N_photo = 0.;
  //double N_CTphoto = 0.;
  //double sigma   = 0.;
  double amplitude = 0.; 
  //double amplitude_CT = 0.; 

  
  for ( int l = 0; l < 8; l++ ) 
  {
    for ( int i = 0; i < 20; i++ ) 
    {
      for ( int j = 0; j < 64;  j++ ) 
	  { 
	  
	    N_photo   = CLHEP::RandPoisson::shoot(rndEngine,m_E_fib[l][i][j]*m_meanN_photo/m_meanE_dep);
	    
		//sigma     = m_sigma1 * sqrt(N_photo);
        //double noise_1 = sigma * CLHEP::RandGaussZiggurat::shoot (rndEngine, m_mean, m_stdDev);
        //double noise_2 = m_sigma0 * CLHEP::RandGaussZiggurat::shoot (rndEngine, m_mean, m_stdDev);
	   
        //amplitude =  N_photo 
	    //             + noise_1 
		//             + noise_2;
			
		amplitude = CLHEP::RandGaussZiggurat::shoot(m_rndEngine, N_photo, sqrt(pow(m_sigma0,2)+N_photo*pow(m_sigma1,2)));

		//N_photo_CT   = CLHEP::RandPoisson::shoot(m_rndEngine,0.08*m_E_fib[l][i][j]*m_meanN_photo/m_meanE_dep); 		
		//amplitude_CT = CLHEP::RandGaussQ::shoot (m_rndEngine, N_photo_CT, sqrt(pow(m_sigma0,2)+N_photo_CT*pow(m_sigma1,2)));
				 
				  
	    if (amplitude >= m_AmplitudeCut )
		{
	      ATH_MSG_DEBUG(" ALFA_Digitization::fillDigitCollection, amplitude " << amplitude);
	      ATH_MSG_DEBUG(" station = " << l << ", plate= " << i << ", fiber=" << j );
	      m_digitCollection->push_back(new ALFA_Digit(l,i,j));
		m_fibres[l][i][j] = m_fibres[l][i][j] + 1;
	    }
      
	  }
    }
  }
  
  double rand_fib;
  
  	for ( int l = 0; l < 8; l++ ) 
	{
		for ( int i = 0; i < 20; i++ ) 
		{
			for ( int j = 0; j < 64;  j++ ) 
			{	
				if ( m_fibres[l][i][j] > 0 )
				{
					for (int f = j+1; f < 64; f++)
					{	
						rand_fib  = CLHEP::RandFlat::shoot(m_rndEngine,0.,1.);
						
						if ( m_fibres[l][i][f] == 0)
						{
							if (rand_fib <= m_cross_talk[l][63+f-j]) 
							{	
								m_digitCollection->push_back(new ALFA_Digit(l,i,f));
							}
						}
					}	
									
					for (int f = j-1; f > -1; f--)
					{	
						rand_fib = CLHEP::RandFlat::shoot(m_rndEngine,0.,1.);	
							
						if ( m_fibres[l][i][f] == 0)
						{
							if (rand_fib <= m_cross_talk[l][63-(j-f)]) 
							{	
								m_digitCollection->push_back(new ALFA_Digit(l,i,f));
							}
						}
					}
				}
			}
		}
    }
  return StatusCode::SUCCESS;
   
}  



     
StatusCode ALFA_PileUpTool::fill_OD_DigitCollection(CLHEP::HepRandomEngine* rndEngine) {
    
  ATH_MSG_DEBUG(" ALFA_PileUpTool::fill_OD_DigitCollection ");
  
  double N_photo = 0.;
  double sigma   = 0.;
  double amplitude = 0.;
  
  for ( int l = 0; l < 8; l++ ) 
  {
    for ( int i = 0; i < 2; i++ ) 
    {
      for ( int j = 0; j < 3; j++ ) 
	{
        for (int k = 0; k < 30; k++) 
	  { 
	  
	    N_photo   = CLHEP::RandPoisson::shoot(rndEngine,m_E_ODfib[l][i][j][k]*m_meanN_photo/m_meanE_dep);
	    sigma     = m_sigma1 * sqrt(N_photo);
		 
          double noise_1 = sigma * CLHEP::RandGaussZiggurat::shoot (rndEngine, m_mean, m_stdDev);
          double noise_2 = m_sigma0 * CLHEP::RandGaussZiggurat::shoot (rndEngine, m_mean, m_stdDev);
	     
		 
          amplitude =  N_photo 
	               + noise_1 
		         + noise_2;
	  
			
	    if (amplitude >= m_AmplitudeCut )
	    {
	      ATH_MSG_DEBUG(" ALFA_Digitization::fill_OD_DigitCollection, amplitude " << amplitude);
	      ATH_MSG_DEBUG(" station = " << l << ", side = " << i << ", plate = " << j << ", fiber = " << k);

	      m_ODdigitCollection->push_back(new ALFA_ODDigit(l,i,j,k));
	    }

      
        }
      }
    }
  }
    
  return StatusCode::SUCCESS;
}


  StatusCode ALFA_PileUpTool::XTalk() 
  {	
	for (unsigned int j=0; j<8; j++){
		
		m_s.str("");
			
		m_filename = "Xtalk_station";
		m_s << j+1;
		m_filename += m_s.str();
		m_filename += ".txt";
		
		ATH_MSG_DEBUG("file name " << m_filename.c_str() );
		
		std::string filePath = PathResolver::find_file(m_filename.c_str(),"DATAPATH", PathResolver::RecursiveSearch);
		
		if(filePath.length() == 0)
		{
			ATH_MSG_FATAL(" XTalk file " <<  m_filename.c_str() << " not found in Datapath");
			throw std::runtime_error("FATAL: mapping MD maroc-mapmt not found in Datapath.");
		}
		
		else
		{
			ATH_MSG_DEBUG("the XTALK file \"" <<  m_filename.c_str() << "\" found in Datapath");
			ATH_MSG_DEBUG("filePath =  " << filePath.c_str() );
		}

		m_fXTalk.open(filePath.c_str());
		
		if (m_fXTalk.is_open())
		{
			for (unsigned int i=0;i<127;i++)
			{				
				m_fXTalk >> m_cross_talk[j][i];

				ATH_MSG_DEBUG("cross_talk[" << j << "][" << i << "] = " << m_cross_talk[j][i]);

			}
			m_fXTalk.close();
		}
		else
		{
			ATH_MSG_ERROR("the file " << m_filename.c_str() << " was not open");
		      return StatusCode::FAILURE;
		}	
	}		
	return StatusCode::SUCCESS;
  }
