/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////
//
//
//       ALFA_GloRec class implementation
//
//
/////////////////////////////////////////////////////////
//
//
//	PURPOSE:
//	- reconstruction of global tracks in ALFA
//
//
//	INPUT JOB OPTIONS:
//	- data type: DataType
//	- patterns of tracks through detectors: TrackPathPatterns
//	- analysis root file: AnalysisFile
//	- name of input collection: InputLocRecCollectionName
// 	-	name of output collection: InputGloRecCollectionName
//	-	name of truth info collection: TruthEventCollectionName
//
//	OUTPUT:
//	- GloRecEvCollection of ALFA_GloRecEvent
//  - some analysis histograms
//
//	OTHER INFO: 
//	Uses aux. classes
//	1) AlfaLocalHit
//			- stores one local hit
//			x, y, z, pot name, pot number, pointer to ALFA_LocRecCorrEvent
//
//	2) AlfaLocalHits
//		- stores stl vector of AlfaLocalHit
//		- may contain stl vector of itself to define path of the track
//			... the path is defined as pattern string
//			... string = names of detectors separated by _ or X for any possibility
//			... there's one pattern at a time for the object, it can be reset
//		- if the pattern is applied m_paths (of AlfaLocalHits) is filled
//			... there may be more paths for one pattern
//				(if there were more local hit combinations)
//		- in job options the TrackPathPatterns may contain multiple patterns
//			separated by comas (without spaces), it is then looped over all 
//			patterns 
//		- detector names are ALFA standard names:
//			B7L1U A7L1U  B7L1L A7L1L B7R1U A7R1U B7R1L A7R1L
//
//	3) AlfaTrackCand
//		- is declared with ONE path from AlfaLocalHits object 
//		- contains x and y halfway between the pots and slope x' and y' of the gloabl track
//		- contains two pointers of ALFA_LocRecEvent the track is constructed from
//
//	There many methods of the above aux. classes not described at the moment.
//	Things are under development.
//
//
//////////////////////////////////////////////////////////


#include "ALFA_GloRec/ALFA_GloRec.h"
#include <algorithm>
#include <functional>


/////////////////////////////////////////////////////////
//
//       ALFA_GloRec constructor
//
//////////////////////////////////////////////////////////
ALFA_GloRec::ALFA_GloRec(const std::string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator)
//,m_digitsStore("StoreGateSvc", name)
{

  MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GloRec::ALFA_GloRec");
	LogStream << MSG::DEBUG << "begin ALFA_GloRec::ALFA_GloRec" << endmsg;

	////////////////////////////////
	// ... steerable job options 
	////////////////////////////////
  declareProperty("DataType", m_iDataType);
  declareProperty("TrackPathPatterns", m_strTrackPathPatterns);
  declareProperty("AnalysisFile", m_strGloRecAnalysisFile);
  
	declareProperty("OutputGloRecCollectionName",m_strGloRecCollectionName);
	declareProperty("InputLocRecCollectionName",m_strLocRecCorrCollectionName);
	declareProperty("TruthEventCollectionName",m_TruthCollectionName);   

	m_TObjArrTrackPathPatterns = 0;
	m_pGloRecEvCollection=NULL;
	
	m_px_g_pos = 0.0;
	m_py_g_pos = 0.0;
	m_pz_g_pos = 0.0;
	m_x_g_pos  = 0.0;
	m_y_g_pos  = 0.0;
	m_z_g_pos  = 0.0;
	m_px_g_neg = 0.0;
	m_py_g_neg = 0.0;
	m_pz_g_neg = 0.0;
	m_x_g_neg  = 0.0;
	m_y_g_neg  = 0.0;
	m_z_g_neg  = 0.0;

	m_th1_x_g                      = 0;
	m_th1_y_g                      = 0;
	m_th1_xslope_g                 = 0;
	m_th1_yslope_g                 = 0;
	m_th1_xnearuppotresiduals      = 0;
	m_th1_ynearuppotresiduals      = 0;
	m_th1_xfaruppotresiduals       = 0;
	m_th1_yfaruppotresiduals       = 0;
	m_th1_xnearlwpotresiduals      = 0;
	m_th1_ynearlwpotresiduals      = 0;
	m_th1_xfarlwpotresiduals       = 0;
	m_th1_yfarlwpotresiduals       = 0;
	m_th2_truexvsrecx              = 0;
	m_th2_trueyvsrecy              = 0;
	m_th2_truexslopevsrecxslope    = 0;
	m_th2_trueyslopevsrecyslope    = 0;
	m_th1_recxovertruex            = 0;
	m_th1_recyovertruey            = 0;
	m_th1_recxslopeovertruexslope  = 0;
	m_th1_recyslopeovertrueyslope  = 0;
	m_th1_recxminustruex           = 0;
	m_th1_recyminustruey           = 0;
	m_th1_recxslopeminustruexslope = 0;
	m_th1_recyslopeminustrueyslope = 0;
	m_th2_extrapxvsrecxnearpot     = 0;
	m_th2_extrapyvsrecynearpot     = 0;
	m_th1_recxoverextrapxnearpot   = 0;
	m_th1_recyoverextrapynearpot   = 0;
	m_th1_recxminusextrapxnearpot  = 0;
	m_th1_recyminusextrapynearpot  = 0;
	m_th2_extrapxvsrecxfarpot      = 0;
	m_th2_extrapyvsrecyfarpot      = 0;
	m_th1_recxoverextrapxfarpot    = 0;
	m_th1_recyoverextrapyfarpot    = 0;
	m_th1_recxminusextrapxfarpot   = 0;
	m_th1_recyminusextrapyfarpot   = 0;

	m_storeGate = 0;
	m_nEvent = 0;

}

/////////////////////////////////////////////////////////
//
//       ALFA_GloRec destructor
//
//////////////////////////////////////////////////////////
ALFA_GloRec::~ALFA_GloRec() {
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GloRec::~ALFA_GloRec");
	LogStream << MSG::DEBUG << "begin ALFA_GloRec::~ALFA_GloRec" << endmsg;
	LogStream << MSG::DEBUG << "end ALFA_GloRec::~ALFA_GloRec" << endmsg;
}

/////////////////////////////////////////////////////////
//
//      initialize method
//
//////////////////////////////////////////////////////////
StatusCode ALFA_GloRec::initialize() {
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GloRec::initialize()");
	LogStream << MSG::DEBUG << "begin ALFA_GloRec::initialize()" << endmsg;

	msg(MSG::DEBUG) << "======================================" << endmsg;
	msg(MSG::DEBUG) << "ALFA_GloRec::initialize  - data type  " << endmsg;
	msg(MSG::DEBUG) << "======================================" << endmsg;
	msg(MSG::DEBUG) << "DataType = " << m_iDataType << endl;

	TString l_strtmp(m_strTrackPathPatterns);
  	m_TObjArrTrackPathPatterns.Clear();
	msg(MSG::DEBUG) << "==========================================================" << endmsg;
	msg(MSG::DEBUG) << "ALFA_GloRec::initialize  - Path patterns from job options " << endmsg;
	msg(MSG::DEBUG) << "==========================================================" << endmsg;
  	m_TObjArrTrackPathPatterns = *(l_strtmp.Tokenize(","));
  	for(int i = 0; i < m_TObjArrTrackPathPatterns.GetEntries(); i++){
			msg(MSG::DEBUG) << "pattern[" << i << "] = " << ((TObjString*) m_TObjArrTrackPathPatterns.At(i))->GetString().Data() << endmsg;
	}

  
  ////////////////////////////////////////////////////
  // ... declare histograms for some analysis
  ////////////////////////////////////////////////////
  InitHistos();
  
  
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////
//
//      execute method
//
//////////////////////////////////////////////////////////
StatusCode ALFA_GloRec::execute() {

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GloRec::execute()");
	LogStream << MSG::DEBUG << "begin ALFA_GloRec::execute()" << endmsg;
	StatusCode sc = StatusCode::SUCCESS;

 	//////////////////////////
	// ... get event info 
	//////////////////////////
	const EventInfo* eventInfo;
	sc = evtStore()->retrieve(eventInfo);
	if (sc.isFailure())	{
		msg(MSG::ERROR) << "ALFA_GloRec cannot get event info." << endmsg;
		return sc;
	}
	m_nEvent = eventInfo->event_ID()->event_number();
	//m_iRunNum = eventInfo->event_ID()->run_number();
	//m_iLumiBlock = eventInfo->event_ID()->lumi_block();
	



  //////////////////////////////////////////
  //  get truth information
  //////////////////////////////////////////
  if(m_iDataType == 0){
	  LogStream << MSG::DEBUG << "truth info()" << endmsg;        
	  sc = Truth_info();  
	  if  (sc.isFailure()) {
		LogStream << MSG::WARNING << "ALFA digi, Truth_info failed" << endmsg;  
	    return StatusCode::SUCCESS; 
	  }
	}

	/////////////////////////////////////
	// ... record output collection 
	/////////////////////////////////////
	m_pGloRecEvCollection = new ALFA_GloRecEvCollection();
	sc = evtStore()->record(m_pGloRecEvCollection, "ALFA_GloRecEvCollection");
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Could not create the empty LocRecEv collection in StoreGate" << endmsg;
	}


	/////////////////////////////////////////////////////////////
	// ... get LocRecCorr data
	/////////////////////////////////////////////////////////////
	const ALFA_LocRecCorrEvCollection* pLocRecCorrCol = NULL;
	sc = evtStore()->retrieve(pLocRecCorrCol, m_strLocRecCorrCollectionName);
	if(sc.isFailure() || !pLocRecCorrCol)
	{
		msg(MSG::ERROR) << "Container "<< m_strLocRecCorrCollectionName <<" NOT FOUND !!!!!!!" << endmsg;
		return sc;
	}


	//////////////////////////////////////////////////////////////////////////////////
	// ... collect the local hits and contain them in an auxiliary hit collection
	//////////////////////////////////////////////////////////////////////////////////
	ALFA_LocRecCorrEvCollection::const_iterator mcColBeg = pLocRecCorrCol->begin();
	ALFA_LocRecCorrEvCollection::const_iterator mcColEnd = pLocRecCorrCol->end();
	AlfaLocalHits hits;
	ALFA_LocRecCorrEvent * pLocRecCorr_tmp = 0;
	for(; mcColBeg!=mcColEnd; ++mcColBeg) {
	        HepGeom::Point3D<double> PointInAtlasCS;
		pLocRecCorr_tmp = static_cast<ALFA_LocRecCorrEvent*> (*mcColBeg);
		AlfaLocalHit l_hit(pLocRecCorr_tmp);
		hits.AddHit(l_hit);
		//PointInAtlasCS = m_pGeometryReader->GetDetPointInAtlas((eRPotName)(iRPot+1), Point3D<double>(fRecPosX, fRecPosY, 10.0));
	}
  //hits.Dump();
   
   

  /////////////////////////////////
  // ... apply path pattern
  /////////////////////////////////
	for(int i = 0; i < m_TObjArrTrackPathPatterns.GetEntries(); i++){
		TString l_pathpattern(((TObjString*) m_TObjArrTrackPathPatterns.At(i))->GetString());
		hits.ApplyPathPattern(l_pathpattern.Data());
		//hits.DumpPaths();
		for(int i = 0 ; i < hits.GetNpaths() ; i++){
			AlfaTrackCand trkcand(hits.GetPathHits(i));
			if(trkcand.IsValidTrack()){
				m_pGloRecEvCollection->push_back(new ALFA_GloRecEvent(trkcand.GetArmNum(),trkcand.GetX(),trkcand.GetY(),trkcand.GetXslope(),trkcand.GetXslope(),trkcand.GetNearLocRecCorr(),trkcand.GetFarLocRecCorr()));	
				FillTrackCandHistos(&trkcand);
			}
	  }
	  hits.ResetPaths();
  }




	return sc;
}




/////////////////////////////////////////////////////////
//
//      finalize method
//
//////////////////////////////////////////////////////////
StatusCode ALFA_GloRec::finalize() {

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GloRec::finalize()");
	LogStream << MSG::DEBUG << "begin ALFA_GloRec::finalize()" << endmsg;

	WriteHistos();

	return StatusCode::SUCCESS;
}




///////////////////////////////////////////////////////////
//
//  Returns truth info
// 
//  - this may be specific to the way the sample was
//    generated, i.e. part. gun, full MC etc.
//  - the user needs to consider how to retrieve
//		the information he/she wants
//
///////////////////////////////////////////////////////////
StatusCode ALFA_GloRec::Truth_info(){

  const McEventCollection* mcTru = 0;      
  StatusCode sc = evtStore()->retrieve(mcTru,m_TruthCollectionName);
  if(sc.isFailure() || !mcTru){
	msg(MSG::DEBUG) << "Container "<< m_TruthCollectionName <<" NOT FOUND !!!!!!!" << endmsg;
    return StatusCode::FAILURE;
  }

  
  //int VertexFoundInFrontOfALFA = 0;


  McEventCollection::const_iterator mcTruBeg = mcTru->begin();
  McEventCollection::const_iterator mcTruEnd = mcTru->end();  

  //loop over collection (container) with hits (i.e. over 1 event)
  for(;mcTruBeg!=mcTruEnd;++mcTruBeg){

    HepMC::GenEvent::particle_const_iterator begGenItr = (**mcTruBeg).particles_begin();
    HepMC::GenEvent::particle_const_iterator endGenItr = (**mcTruBeg).particles_end();    
   
    //loop over one event
    m_z_g_pos = -1.e10;
    m_z_g_neg =  1.e10;
    for(int i = 0;begGenItr!=endGenItr;++begGenItr){
 	  //if(((*begGenItr)->status()==1)&&((abs((*begGenItr)->pdg_id())==2212)||(abs((*begGenItr)->pdg_id())==211))){
	 		if(((abs((*begGenItr)->pdg_id())==2212)||(abs((*begGenItr)->pdg_id())==211))){
				double px, py, pz;
				double x, y, z;
				px = (**begGenItr).momentum().px();	  
			  py = (**begGenItr).momentum().py();
				pz = (**begGenItr).momentum().pz();
				HepMC::GenVertex * l_prodvert = (**begGenItr).production_vertex();
			  //HepMC::GenVertex * l_endvert =  (**begGenItr).end_vertex();
				x = l_prodvert->position().x();
				y = l_prodvert->position().y();
				z = l_prodvert->position().z();
				if(z > 236000.){
					m_px_g_pos = px;
					m_py_g_pos = py;
					m_pz_g_pos = pz;
					m_x_g_pos = x;
					m_y_g_pos = y;
					m_z_g_pos = z;										
					continue;
				}else if(z < -236000.){
					m_px_g_neg = px;
					m_py_g_neg = py;
					m_pz_g_neg = pz;
					m_x_g_neg = x;
					m_y_g_neg = y;
					m_z_g_neg = z;			
					continue;							
				}
		
			  i++;
			  // ... break the loop if both suitable vertices were found
			  if (m_z_g_pos > 0. && m_z_g_neg < 0.){
				msg(MSG::DEBUG) << "gen px, py, pz = " << m_px_g_pos <<  "  " << m_py_g_pos << "  " << m_pz_g_pos << endmsg;
					msg(MSG::DEBUG) << "prod x, y, z = " << m_x_g_pos <<  "  " << m_y_g_pos << "  " << m_z_g_pos << endmsg;
					msg(MSG::DEBUG) << "gen px, py, pz = " << m_px_g_neg <<  "  " << m_py_g_neg << "  " << m_pz_g_neg << endmsg;
					msg(MSG::DEBUG) << "prod x, y, z = " << m_x_g_neg <<  "  " << m_y_g_neg << "  " << m_z_g_neg << endmsg;
					break;
				}
      }
    }
  }  
  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////
//
//   Declare histograms for user analysis
//
////////////////////////////////////////////////////
void ALFA_GloRec::InitHistos(){
	m_th1_x_g = new TH1F("x_g","x^{vertex}_{gen}", 100, -5,5);
	m_th1_y_g = new TH1F("y_g","y^{vertex}_{gen}", 100, -5,5);
	m_th1_xslope_g = new TH1F("xslope_g","x'^{vertex}_{gen}", 100, -1.e-3,1.e3);
	m_th1_yslope_g = new TH1F("yslope_g","y'^{vertex}_{gen}", 100, -1.e-3,1.e3);

  m_th1_xnearuppotresiduals = new TH1F("xnearuppotresiduals","trk x extrap. minus near IP U loc hit",100, -10., 10.);
  m_th1_ynearuppotresiduals = new TH1F("ynearuppotresiduals","trk y extrap. minus near IP U loc hit",100, -10., 10.);
  m_th1_xfaruppotresiduals  = new TH1F("xfaruppotresiduals","trk x extrap. minus far IP U loc hit",100, -10., 10.);
  m_th1_yfaruppotresiduals  = new TH1F("yfaruppotresiduals","trk y extrap. minus far IP U loc hit",100, -10., 10.);
  m_th1_xnearlwpotresiduals = new TH1F("xnearlwpotresiduals","trk x extrap. minus near IP L loc hit",100, -10., 10.);
  m_th1_ynearlwpotresiduals = new TH1F("ynearlwpotresiduals","trk y extrap. minus near IP L loc hit",100, -10., 10.);
  m_th1_xfarlwpotresiduals  = new TH1F("xfarlwpotresiduals","trk x extrap. minus far IP L loc hit",100, -10., 10.);
  m_th1_yfarlwpotresiduals  = new TH1F("yfarlwpotresiduals","trk y extrap. minus far IP L loc hit",100, -10., 10.);
      
  m_th2_truexvsrecx = new TH2F("truexvsrecx","gen x vs rec x", 100, -50, 50, 100, -50, 50);
  m_th2_trueyvsrecy = new TH2F("trueyvsrecy","gen y vs rec y", 100, -50, 50, 100, -50, 50);
  m_th2_truexslopevsrecxslope = new TH2F("truexslopevsrecxslope","gen xslope vs rec xslope", 100,-0.002,0.002,100,-0.002,0.002);
  m_th2_trueyslopevsrecyslope = new TH2F("trueyslopevsrecyslope","gen yslope vs rec yslope", 100,-0.002,0.002,100,-0.002,0.002);
  m_th1_recxovertruex = new TH1F("recxovertruex","rec x over true x", 100, -2, 2);
  m_th1_recyovertruey = new TH1F("recyovertruey","rec y over true y", 100, -2, 2);
  m_th1_recxslopeovertruexslope = new TH1F("recxslopeovertruexslope","rec xslope over true xslope", 100,-2,2);
  m_th1_recyslopeovertrueyslope = new TH1F("recyslopeovertrueyslope","rec yslope over true yslope", 100,-2,2);
  m_th1_recxminustruex = new TH1F("recxminustruex","rec x minus true x", 100, -10, 10);
  m_th1_recyminustruey = new TH1F("recyminustruey","rec y minus true y", 100, -10, 10);
  m_th1_recxslopeminustruexslope = new TH1F("recxslopeminustruexslope","rec xslope minus true xslope", 100,-0.001,0.001);
  m_th1_recyslopeminustrueyslope = new TH1F("recyslopeminustrueyslope","rec yslope minus true yslope", 100,-0.001,0.001);

  m_th2_extrapxvsrecxnearpot = new TH2F("extrapxvsrecxnearpot","extrapolated gen x vs rec x nearpot", 100, -50, 50, 100, -50, 50);
  m_th2_extrapyvsrecynearpot = new TH2F("extrapyvsrecynearpot","extrapolated gen y vs rec y nearpot", 100, -50, 50, 100, -50, 50);
  m_th1_recxoverextrapxnearpot = new TH1F("recxoverextrapxnearpot","rec x over extrap x nearpot", 100, 1.-0.2, 1.+0.2);
  m_th1_recyoverextrapynearpot = new TH1F("recyoverextrapynearpot","rec y over extrap y nearpot", 100, 1.-0.2, 1.+0.2);
  m_th1_recxminusextrapxnearpot = new TH1F("recxminusextrapxnearpot","rec x minus extrap x nearpot", 100, -0.5, 0.5);
  m_th1_recyminusextrapynearpot = new TH1F("recyminusextrapynearpot","rec y minus extrap y nearpot", 100, -0.5, 0.5);

  m_th2_extrapxvsrecxfarpot = new TH2F("extrapxvsrecxfarpot","extrapolated gen x vs rec x farpot", 100, -50, 50, 100, -50, 50);
  m_th2_extrapyvsrecyfarpot = new TH2F("extrapyvsrecyfarpot","extrapolated gen y vs rec y farpot", 100, -50, 50, 100, -50, 50);
  m_th1_recxoverextrapxfarpot = new TH1F("recxoverextrapxfarpot","rec x over extrap x farpot", 100, 1.-0.2, 1.+0.2);
  m_th1_recyoverextrapyfarpot = new TH1F("recyoverextrapyfarpot","rec y over extrap y farpot", 100, 1.-0.2, 1.+0.2);
  m_th1_recxminusextrapxfarpot = new TH1F("recxminusextrapxfarpot","rec x minus extrap x farpot", 100, -0.5, 0.5);
  m_th1_recyminusextrapyfarpot = new TH1F("recyminusextrapyfarpot","rec y minus extrap y farpot", 100, -0.5, 0.5);
}


////////////////////////////////////////////////////
//
//   Fill histograms in user analysis
//
////////////////////////////////////////////////////
void ALFA_GloRec::FillTrackCandHistos(AlfaTrackCand * trkcand){
	
	if(m_iDataType==0){
		double x_g, y_g, z_g, px_g, py_g, pz_g;
		if(trkcand->IsLeftTrack()){ // left is positive
			x_g = m_x_g_pos;
			y_g = m_y_g_pos;
			z_g = m_z_g_pos;
			px_g = m_px_g_pos;
			py_g = m_py_g_pos;
			pz_g = m_pz_g_pos;
		}else if(trkcand->IsRightTrack()){ // right is negative
			x_g = m_x_g_neg;
			y_g = m_y_g_neg;
			z_g = m_z_g_neg;
			px_g = m_px_g_neg;
			py_g = m_py_g_neg;
			pz_g = m_pz_g_neg;
		}else{
			msg(MSG::DEBUG) << "ALFA_GloRec::FillTrackCandHistos : Track candidate neither left nor right !!!" << endmsg;
			exit(0);
		}
		m_th1_xnearuppotresiduals -> Fill(trkcand->GetNearImpactPoint()->x()-trkcand->GetNearLocalHit()->GetX(),1.);
		m_th1_ynearuppotresiduals -> Fill(trkcand->GetNearImpactPoint()->y()-trkcand->GetNearLocalHit()->GetY(),1.);
		m_th1_xfaruppotresiduals -> Fill(trkcand->GetFarImpactPoint()->x()-trkcand->GetFarLocalHit()->GetX(),1.);
		m_th1_yfaruppotresiduals -> Fill(trkcand->GetFarImpactPoint()->y()-trkcand->GetFarLocalHit()->GetY(),1.);
		m_th2_truexvsrecx -> Fill(x_g, trkcand->GetX(), 1.);
		m_th2_trueyvsrecy -> Fill(y_g, trkcand->GetY(), 1.);
		m_th1_recxovertruex -> Fill(trkcand->GetX()/x_g, 1.);
		m_th1_recyovertruey -> Fill(trkcand->GetY()/y_g, 1.);
		//double p_gen = TMath::Sqrt(px_g*px_g+px_g*px_g+py_g*pz_g);
		double px_norm = px_g / pz_g;//p_gen;
		double py_norm = py_g / pz_g;//p_gen;
		m_th2_truexslopevsrecxslope -> Fill(px_norm, trkcand->GetXslope(), 1.);
		m_th2_trueyslopevsrecyslope -> Fill(py_norm, trkcand->GetYslope(), 1.);
		m_th1_recxovertruex -> Fill(trkcand->GetX()/x_g, 1.);
		m_th1_recyovertruey -> Fill(trkcand->GetY()/y_g, 1.);
		m_th1_recxslopeovertruexslope -> Fill(trkcand->GetXslope()/px_norm, 1.);
		m_th1_recyslopeovertrueyslope -> Fill(trkcand->GetYslope()/py_norm, 1.);
		m_th1_recxminustruex -> Fill(trkcand->GetX()-x_g, 1.);
		m_th1_recyminustruey -> Fill(trkcand->GetY()-y_g, 1.);
		m_th1_recxslopeminustruexslope -> Fill(trkcand->GetXslope()-px_norm, 1.);
		m_th1_recyslopeminustrueyslope -> Fill(trkcand->GetYslope()-py_norm, 1.);
		// ... extrapolate x any of gen track to near pot
		double nearhitx = trkcand->GetNearLocalHit()->GetX();
		double nearhity = trkcand->GetNearLocalHit()->GetY();
		double nearhitz = trkcand->GetNearLocalHit()->GetZ();
		double genxextrtonearpot = x_g + px_norm * (nearhitz - z_g);
		double genyextrtonearpot = y_g + py_norm * (nearhitz - z_g);
		//double genzextrtonearpot = nearhitz;
		m_th2_extrapxvsrecxnearpot -> Fill(genxextrtonearpot, nearhitx,1.);
		m_th2_extrapyvsrecynearpot -> Fill(genyextrtonearpot, nearhity,1.);
		m_th1_recxoverextrapxnearpot -> Fill(nearhitx / genxextrtonearpot, 1.);
		m_th1_recyoverextrapynearpot -> Fill(nearhity / genyextrtonearpot, 1.);
		m_th1_recxminusextrapxnearpot -> Fill(nearhitx - genxextrtonearpot, 1.);
		m_th1_recyminusextrapynearpot -> Fill(nearhity - genyextrtonearpot, 1.);
		// ... extrapolate x any of gen track to far pot
		double farhitx = trkcand->GetFarLocalHit()->GetX();
		double farhity = trkcand->GetFarLocalHit()->GetY();
		double farhitz = trkcand->GetFarLocalHit()->GetZ();
		double genxextrtofarpot = x_g + px_norm * (farhitz - z_g);
		double genyextrtofarpot = y_g + py_norm * (farhitz - z_g);
		//double genzextrtofarpot = farhitz;
		m_th2_extrapxvsrecxfarpot -> Fill(genxextrtofarpot, farhitx,1.);
		m_th2_extrapyvsrecyfarpot -> Fill(genyextrtofarpot, farhity,1.);
		m_th1_recxoverextrapxfarpot -> Fill(farhitx / genxextrtofarpot, 1.);
		m_th1_recyoverextrapyfarpot -> Fill(farhity / genyextrtofarpot, 1.);
		m_th1_recxminusextrapxfarpot -> Fill(farhitx - genxextrtofarpot, 1.);
		m_th1_recyminusextrapyfarpot -> Fill(farhity - genyextrtofarpot, 1.);
	}
}




////////////////////////////////////////////////////
//
//   Write histograms into file
//
////////////////////////////////////////////////////
void ALFA_GloRec::WriteHistos(){
  /////////////////////////////////////////////////////////
  // ... write the histograms into a root file
  /////////////////////////////////////////////////////////
	TFile fw_glorecanalysis(m_strGloRecAnalysisFile.data(), "RECREATE", m_strGloRecAnalysisFile.data(), 0);
  m_th1_xnearuppotresiduals -> Write();
  m_th1_ynearuppotresiduals -> Write();
  m_th1_xfaruppotresiduals -> Write();
  m_th1_yfaruppotresiduals -> Write();
  m_th1_xnearlwpotresiduals -> Write();
  m_th1_ynearlwpotresiduals -> Write();
  m_th1_xfarlwpotresiduals -> Write();
  m_th1_yfarlwpotresiduals -> Write();
  m_th2_truexvsrecx -> Write();    
	m_th2_trueyvsrecy -> Write();    
	m_th2_truexslopevsrecxslope -> Write();    
	m_th2_trueyslopevsrecyslope -> Write();    
	m_th1_recxovertruex -> Write();    
	m_th1_recyovertruey -> Write();    
	m_th1_recxslopeovertruexslope -> Write();    
	m_th1_recyslopeovertrueyslope -> Write();    
	m_th1_recxminustruex -> Write();    
	m_th1_recyminustruey -> Write();    
	m_th1_recxslopeminustruexslope -> Write();    
	m_th1_recyslopeminustrueyslope -> Write();    
  m_th2_extrapxvsrecxnearpot -> Write();   
  m_th2_extrapyvsrecynearpot -> Write();
  m_th1_recxoverextrapxnearpot -> Write();
  m_th1_recyoverextrapynearpot -> Write();
  m_th1_recxminusextrapxnearpot -> Write();
  m_th1_recyminusextrapynearpot -> Write();
  m_th2_extrapxvsrecxfarpot -> Write();   
  m_th2_extrapyvsrecyfarpot -> Write();
  m_th1_recxoverextrapxfarpot -> Write();
  m_th1_recyoverextrapyfarpot -> Write();
  m_th1_recxminusextrapxfarpot -> Write();
  m_th1_recyminusextrapyfarpot -> Write();

  fw_glorecanalysis.Write();
  fw_glorecanalysis.Close();
}


