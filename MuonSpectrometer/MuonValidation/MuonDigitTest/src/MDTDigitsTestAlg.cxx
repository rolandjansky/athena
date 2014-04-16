/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// MDTDigitsTestAlg
// --------
// based on Ketevi A. Assamagan's MDTDigitValidation
////////////////////////////////////////////////////////////////////////////////

#include "MuonDigitTest/MDTDigitsTestAlg.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//#include "EventInfo/AtlasMcWeight.h"

///for GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ITHistSvc.h"

///for StoreGate
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"

#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h" 
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
 
#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigit.h"

#include "MuonSimValUtils/MuonChambersRange.h"
#include "MuonSimValUtils/MuonChamberNameConverter.h"

#include "MdtCalibSvc/MdtCalibrationSvc.h"

#include "MuonCalibEvent/MdtCalibHit.h"
#include "GeneratorObjects/McEventCollection.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "CLHEP/Vector/ThreeVector.h"

#include "HepMC/GenEvent.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "AnalysisTools/AANTupleParams.h"

#include <algorithm>
//#include <cmath>
#include <TMath.h>
#include <iostream>
#include <fstream>
#include <TH2D.h>
#include "TTree.h"
#include <string>

using namespace MuonGM;
using namespace std;

//Function for histogram, AANtuple booking
void bookMDTHistograms(std::string);
void bookMDTChamberStats();
////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

MDTDigitsTestAlg::MDTDigitsTestAlg(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator),
  TestAlgInterface(),
  m_pMuonMgr           (0),
  m_pMdtIdHelper       (0),
  log( msgSvc(), name )
{
  declareProperty("WriteNtuple",          m_writeNtuple = true);
  declareProperty("McEventKey",           m_key = "TruthEvent");
  declareProperty("NtupleFileName",       m_ntupleFileName = "MuonDigits");
  declareProperty("NtupleDirectoryName",  m_ntupleDirName  = "MDTDigitsValidation");
  declareProperty("NtupleTreeName",       m_ntupleTreeName = "MDTTestDigits");
  declareProperty("DoMDTTest",            m_DoMDTTest = true);
  declareProperty("BarrelEtaCut",         m_BarrelEtaCut=1);
  declareProperty("ChamberName",	  m_chamberName="XXX");	
  declareProperty("StationSize",          m_StationSize="XXX");
  declareProperty("StationEta",           m_StationEta=-100);
  declareProperty("StationPhi",           m_StationPhi=-100);  
  declareProperty("WriteChamberGraphs",   m_writeChambers = true);
  declareProperty("LastEvent",            m_LastEvent=1);    
  
  log << MSG::INFO << "++++++++++++  MDTDigitsTestAlg created  ++++++++++++" << endreq;
}

MDTDigitsTestAlg::~MDTDigitsTestAlg()
{
  log << MSG::INFO << " deleting MDTDigitsTestAlg " << endreq;
}

////////////////////////////////////////////////////////////////////////////
// member function implementation
////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------*/
StatusCode MDTDigitsTestAlg::initialize()
{
  log << MSG::DEBUG << " initializing MDTDigitsTestAlg " << endreq;

  // Locate the StoreGateSvc and initialize our local ptr
  // intitialize transient event store
  StatusCode sc = service("StoreGateSvc", m_sgSvc);
  if (!sc.isSuccess() || 0 == m_sgSvc)
    {
      log << MSG::ERROR << "MDTDigitsTestAlg: Could not find StoreGateSvc" << endreq;
      return sc;
    }
  log << MSG::DEBUG << "Retrieved StoreGateSvc" << endreq;
 
  sc = serviceLocator()->service("MdtCalibrationSvc", m_calibSvc);
  if (sc.isFailure())
    {
      log << MSG::ERROR << " Cannot retrieve MdtCalibrationSvc " << endreq;
      return sc;
    } 
  log << MSG::DEBUG << " Retrieved MdtCalibrationSvc " << endreq;
 
  // retrieve the StoreGate Active store Service

  sc = service("ActiveStoreSvc",m_activeStore);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Could not retrieve StoreGate ActiveStoreSvc!" <<endreq;
    }
  log << MSG::DEBUG << "Retrieved StoreGate ActiveStoreSvc." << endreq;
 
  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore );
  if ( sc.isSuccess() )
    {
      sc = detStore->retrieve( m_pMuonMgr );
      if ( sc.isFailure() )
	{
	  log << MSG::ERROR << " MDTDigitsTestAlg::initialize() :  Could not find MuonDetDescrMgr " << endreq;
	}
      else  m_pMdtIdHelper = m_pMuonMgr->mdtIdHelper();
    }
  else
    {
      log << MSG::ERROR << " MDTDigitsTestAlg::initialize() : MuonDetDescrMgr not found in DetectorStore " << endreq;
    }


  if (sc.isFailure())
    {
      log << MSG::ERROR << "Could not get DetectorStore" << endreq;
      return sc;
    }


  /// ROOT AANtuple Initialization---------------------------------------

  if (m_writeNtuple)
    {
      std::string mdtStreamAndPath = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
      std::string mdttreeName = m_ntupleTreeName;

      std::string mdttreePath = mdtStreamAndPath+"/"+mdttreeName;
      std::string tmp;

      mdttree = new TTree(TString(mdttreeName), "Muon MDT Digits output");

      StatusCode status;
      status=ToolRootHistSvc()->regTree(mdttreePath, mdttree);

      mdttree->Branch("m_Validation_MDT_NumberOfDigits",&m_Val_MDT_NumberOfDigits,"m_Validation_MDT_NumberOfDigits/I");
      mdttree->Branch("m_Validation_MDT_RunNumber",&m_Val_MDT_RunNumber,"m_Validation_MDT_RunNumber/I");
      mdttree->Branch("m_Validation_MDT_EventNumber",&m_Val_MDT_EventNumber,"m_Validation_MDT_EventNumber/I");
      mdttree->Branch("m_Validation_MDT_StationName",&m_Validation_MDT_StationName,"m_Validation_MDT_StationName/I");
      mdttree->Branch("m_Validation_MDT_StationEta",&m_Validation_MDT_StationEta,"m_Validation_MDT_StationEta/I");
      mdttree->Branch("m_Validation_MDT_StationPhi",&m_Validation_MDT_StationPhi,"m_Validation_MDT_StationPhi/I");
      mdttree->Branch("m_Validation_MDT_WireX",&m_Validation_MDT_WireX,"m_Validation_MDT_WireX/D");
      mdttree->Branch("m_Validation_MDT_WireY",&m_Validation_MDT_WireY,"m_Validation_MDT_WireY/D");
      mdttree->Branch("m_Validation_MDT_WireZ",&m_Validation_MDT_WireZ,"m_Validation_MDT_WireZ/D");
      mdttree->Branch("m_Validation_MDT_Driftradius",&m_Validation_MDT_Driftradius,"m_Validation_MDT_Driftradius/D");
      mdttree->Branch("m_Validation_MDT_DriftradiusHit",&m_Validation_MDT_DriftradiusHit,"m_Validation_MDT_DriftradiusHit/D");
      mdttree->Branch("m_Validation_MDT_Residuals",&m_Validation_MDT_Residuals,"m_Validation_MDT_Residuals/D");
      mdttree->Branch("m_Validation_MDT_IDTube",&m_Validation_MDT_IDTube,"m_Validation_MDT_IDTube/I");
      mdttree->Branch("m_Validation_MDT_IDMultilayer",&m_Validation_MDT_IDMultilayer,"m_Validation_MDT_IDMultilayer/I");
      mdttree->Branch("m_Validation_MDT_IDLayer",&m_Validation_MDT_IDLayer,"m_Validation_MDT_IDLayer/I");

      if(status.isFailure())
	{
	  log << MSG::DEBUG << "MDTDigitsTestAlg:: Unable to register TTreeTuple : " <<
	    mdttreePath << endreq;
	  return status;
	}
    }

  std::vector<std::string> hardware_name_list;
  hardware_name_list.push_back("XXX");
  event_counter=0;  
  bookMDTChamberStats();
  log << MSG::DEBUG << "MDTDigitsTestAlg:: Initialisation ended  " << endreq;
  return sc;

}



/// Execute - on event by event

/*---------------------------------------------------------*/
StatusCode MDTDigitsTestAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;
  log << MSG::DEBUG << " MDTDigitsTestAlg:: execute " << endreq;

  clear_variables();

  // ---------------------------------------
  // fill event data
  // get EventInfo for run and event number
  const EventInfo* pevt;

  if (storeGateSvc()->retrieve(pevt).isFailure())
    {
      log << MSG::FATAL << "Could not find event" << endreq;
      return sc;
    }
  log << MSG::DEBUG << "Found EventInfo in SG" << endreq;
 
  if(isVerbose())
    {
      log << 	MSG::VERBOSE <<"Processing EventInfo event #"<<pevt->event_ID()->event_number() << " run: " <<
	pevt->event_ID()->run_number() << endreq;
    }

  m_Val_MDT_EventNumber = pevt->event_ID()->event_number();
  m_Val_MDT_RunNumber = pevt->event_ID()->run_number();

  if(m_DoMDTTest)
    {
      findMuons();
      if( m_theMuons.size() ){ if( do_AANtuple_conversion().isFailure()) return StatusCode::FAILURE; }
      else log << MSG::WARNING << "could not find any muon, skipping MDTDigitsTestAlgvalidation" << endreq;
    }
  
  return sc;
}

/*---------------------------------------------------------*/
StatusCode MDTDigitsTestAlg::finalize()
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void MDTDigitsTestAlg::findMsel()
{}
/*---------------------------------------------------------*/
void MDTDigitsTestAlg::findMuons()
{
    
  m_theMuons.clear();
  HepGeom::Point3D<double> pvert = HepGeom::Point3D<double>(0,0,0);

  const DataHandle<McEventCollection> mcEvent;

  if(storeGateSvc()->retrieve(mcEvent,m_key).isFailure())
    {
      log << MSG::FATAL << "MDTDigitsTestAlg: Could not find MCevent" << endreq;
      return;
    }
  log << MSG::DEBUG << "MDTDigitsTestAlg: Found MCEvent" << endreq;
 
  m_nPar = 0;
  mu_phi = 0.;
  mu_eta = 0.;

  for (McEventCollection::const_iterator itr = mcEvent->begin(); itr!=mcEvent->end(); ++itr)
    {
      const HepMC::GenEvent* myEvent = (*itr);

      HepMC::GenEvent::particle_const_iterator Part;
      for (Part = myEvent->particles_begin(); Part!=myEvent->particles_end(); ++Part )
	{
	  if ( (*Part)->pdg_id() == -13 || (*Part)->pdg_id() == 13 || (*Part)->pdg_id() == 999)
	    {
	      HepMC::GenVertex* Vert = (*Part)->production_vertex();
	      if (Vert!=0)
		{
		  pvert.setX((*Part)->momentum().px());
		  pvert.setY((*Part)->momentum().py());
		  pvert.setZ((*Part)->momentum().pz());

		  mu_phi = (*Part)->momentum().phi();
		  mu_eta = (*Part)->momentum().eta();
		  m_theMuons.insert(std::map<int,HepGeom::Point3D<double> >::value_type((*Part)->barcode(),
									 pvert));
		  mu_p     =       (*Part)->momentum().e();
		  mu_pT    =       (*Part)->momentum().perp();
		  //double	x=       Vert->position().x();
		  //double	y=       Vert->position().y();
		  //double	z=	Vert->position().z();
		}
	    }
	  ++m_nPar;
	}
    }
}

/*---------------------------------------------------------*/
StatusCode MDTDigitsTestAlg::do_AANtuple_conversion()
{

  /// ROOT histograms ---------------------------------------    
  
  TH2 *mdtdet;
  TH2 *mdtlongview; 
  TH1 *mdtmultil;    
  TH1 *mdtlayer;    
  TH1 *mdttube;  
  TH1 *mdteta;
  TH1 *mdtphi;
  TH1 *mdtx;  
  TH1 *mdty;
  TH1 *mdtz;
  TH1 *mdtchamberstat;  
  TH1 *mdtchamberwithhit;
  TH1 *mdthitsperevent;
  
  
  //retrieve all mdt digitizized information
  StatusCode sc;
  log << MSG::DEBUG << "Do validation " << endreq;

  typedef std::map<int, HepGeom::Point3D<double> >::const_iterator  mu_iterator;
  typedef MdtDigitContainer::const_iterator collection_iterator;
  typedef MdtDigitCollection::const_iterator digit_iterator;

  std::string key = "MDT_DIGITS";
  const DataHandle <MdtDigitContainer> container;
  sc = (*m_activeStore)->retrieve(container,key);
  if (sc.isFailure())
    {
      log << MSG::ERROR << " Cannot retrieve MDT digit Container " << endreq;
      return sc;
    }

  std::string  sdoKey= "MDT_SDO";
  const MuonSimDataCollection* sdoContainer;
  sc = m_sgSvc->retrieve(sdoContainer,sdoKey);
  if (!sc.isSuccess())
    {
      log << MSG::ERROR << "No Sdo Container found" << endreq;
      return false;
    }

  collection_iterator it1_coll= container->begin();
  collection_iterator it2_coll= container->end();

  mu_iterator mu_start=m_theMuons.begin();
  mu_iterator mu_stop=m_theMuons.end();

  m_Val_MDT_NumberOfDigits=0;
  std::list<std::string> ChamberList;
  //int mucount=0;

  // loop over muons
  for(;mu_start!=mu_stop;++mu_start)
    {
      HepGeom::Point3D<double> pos_mu(0.,0.,0.);
      HepGeom::Vector3D<double> dir_mu = (*mu_start).second.unit();

      //double dr = dir_mu.perp();
      //double dz = dir_mu.z();
      // loop over container
      for (  ; it1_coll!=it2_coll; ++it1_coll)
	{
	  const MdtDigitCollection* thecoll=(*it1_coll);

	  digit_iterator it1_dig=thecoll->begin();
	  digit_iterator it2_dig=thecoll->end();
	  // loop over digit collection
	  for (  ; it1_dig!=it2_dig; ++it1_dig)
	    {
	      const MdtDigit* theDigit = *it1_dig;
	      Identifier channelId = theDigit->identify();

	      //std::cout << " MDTDigitVal::Digit: " << thecoll->size() << " mLayer "
	      // << m_pMdtIdHelper->multilayer(channelId)
	      //<< " layer " <<  m_pMdtIdHelper->tubeLayer(channelId) << " tube "
	      //<<  m_pMdtIdHelper->tube(channelId) << std::endl;

	      if (m_pMdtIdHelper->valid(channelId))
		{
		  const MdtReadoutElement *descriptor=m_pMuonMgr->getMdtReadoutElement(channelId);
		  Amg::Vector3D digPos    = descriptor->tubePos(channelId);
		  Amg::Vector3D ROPos     = descriptor->ROPos(channelId);

		  const MuonSimData* sim = getMuonSimData(channelId,*sdoContainer);
		  double position_along_wire(0.);
		  double radius(0.);

		  if(!sim)
		    {
		      log << MSG::ERROR << "No MuonSimData found for " <<
			m_pMdtIdHelper->show_to_string(channelId) << endreq;
		    }
		  else
		    {
		      const std::vector< MuonSimData::Deposit >& deposits = sim->getdeposits();
		      radius = deposits[0].second.firstEntry();
		      position_along_wire = deposits[0].second.secondEntry();
		    }


		  Amg::Vector3D tube_dir = (digPos - ROPos).normalized();

		  double distRO =	descriptor->tubeFrame_localROPos(channelId).z();

		  // prop delay calculated with respect to the center of the tube
		  double __attribute__((__unused__)) sign(-1.);
		  if(distRO < 0.) sign = 1.;
		  //double propagation_delay = sign*position_along_wire;

		  ////// nieuw
		  Amg::Vector3D localHitOnWire(0.,0.,position_along_wire);
		  Amg::Vector3D globalWire =  descriptor->localToGlobalCoords( localHitOnWire, channelId );
		  HepGeom::Point3D<double> digPosforDist(digPos.x(),digPos.y(),digPos.z());
		  HepGeom::Point3D<double> tube_dirforDist(tube_dir.x(),tube_dir.y(),tube_dir.z());
		  DistSegment dist = getDistance(pos_mu,dir_mu,digPosforDist,tube_dirforDist);

		  int mdt_stname=m_pMdtIdHelper->stationName(channelId);
		  int mdt_steta=m_pMdtIdHelper->stationEta(channelId);
		  int mdt_stphi=m_pMdtIdHelper->stationPhi(channelId);
		  int mLayer=m_pMdtIdHelper->multilayer(channelId);
		  int layer=m_pMdtIdHelper->tubeLayer(channelId);
		  int tube=m_pMdtIdHelper->tube(channelId);

	    	  double rad=sqrt(globalWire.x()*globalWire.x()+globalWire.y()*globalWire.y());
	    
		  //	    	  double mdt_phi = TMath::ATan(globalWire.y()/globalWire.x());
		  //	    	  if (mdt_phi<0.) {mdt_phi += 2.*M_PI;} 
	    	  double mdt_phi = globalWire.phi();
		  if (mdt_phi<0.) {mdt_phi += 2.*M_PI;} 

		  double mdt_theta = TMath::ATan(rad/globalWire.z());
	    	  if (mdt_theta<0) {mdt_theta += M_PI;}
 	    	  double mdt_eta = -(TMath::Log(TMath::Tan((mdt_theta/2.))));

                  if (m_writeNtuple) {

		  // convert time in radius using calibrationSvc
		  Amg::Vector3D mooPos(globalWire);
		  double driftRadius = 0.0;
            double residual    = 0.0;
		  //double sigma;
		  MdtCalibHit calibHit( theDigit->identify(), theDigit->tdc(),
					theDigit->adc(), mooPos, 0 );
            bool ok = m_calibSvc->driftRadiusFromTime( calibHit, globalWire.mag(),0 );
            if(ok) 
             {
              driftRadius = calibHit.driftRadius();
              residual = fabs(radius) - driftRadius;
             }
		 
		  //double digeta=digPos.eta();
		  //double digphi=digPos.phi();

		  m_Validation_MDT_StationName	= mdt_stname;
		  m_Validation_MDT_StationEta	= mdt_steta;
		  m_Validation_MDT_StationPhi	= mdt_stphi;
		  m_Validation_MDT_WireX	= globalWire.x();
		  m_Validation_MDT_WireY	= globalWire.y();
		  m_Validation_MDT_WireZ	= globalWire.z();
		  m_Validation_MDT_Driftradius	= driftRadius;
            m_Validation_MDT_DriftradiusHit= radius;
            m_Validation_MDT_Residuals    = residual;
		  m_Validation_MDT_IDMultilayer	= mLayer;
		  m_Validation_MDT_IDLayer	= layer;
		  m_Validation_MDT_IDTube	= tube;
		  m_Val_MDT_NumberOfDigits++;

		  mdttree->Fill();
		
		  } //end ntuple writing if statement 
		
		  /**From function that convert MDT and RPC identifiers to harware names.Located in MuonSimValUtils*/
		  std::string hardware_name=convertChamberName(mdt_stname,mdt_steta,mdt_stphi,"MDT") ;		
		  ChamberList.push_back(hardware_name);

		  //log << MSG::INFO <<  "hardwarename : " << hardware_name << " mdt_phi :" << mdt_phi << endreq;	
		
		  /**bool function, selects chamber to be plotted in histo file according to the user's 
		     selection in jobOpt.Located in MuonSimValUtils
		  */
		  if (selectChambersRange(hardware_name, m_chamberName, 
					  mdt_steta, m_StationEta,
					  mdt_stphi, m_StationPhi, m_StationSize)) {

                   if(m_writeChambers){

		    std::string m_generic_path_mdtdetcxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_det";
		    m_generic_path_mdtdetcxx += hardware_name;
       	    
		    std::string m_generic_path_mdtlongviewcxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_longview";
		    m_generic_path_mdtlongviewcxx += hardware_name;

		    std::string m_generic_path_mdtmultilcxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_multil";
		    m_generic_path_mdtmultilcxx += hardware_name;

		    std::string m_generic_path_mdtlayercxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_layer";
		    m_generic_path_mdtlayercxx += hardware_name;

		    std::string m_generic_path_mdttubecxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_tube";
		    m_generic_path_mdttubecxx += hardware_name;

		    std::string m_generic_path_mdtetacxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_eta";
		    m_generic_path_mdtetacxx += hardware_name;

		    std::string m_generic_path_mdtphicxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_phi";
		    m_generic_path_mdtphicxx += hardware_name;

		    std::string m_generic_path_mdtxcxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_x";
		    m_generic_path_mdtxcxx += hardware_name;

		    std::string m_generic_path_mdtycxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_y";
		    m_generic_path_mdtycxx += hardware_name;

		    std::string m_generic_path_mdtzcxx = "/muondigits/MDT/"+hardware_name+"/mdtdigit_z";
		    m_generic_path_mdtzcxx += hardware_name;		

		    histo_flag=true;
		    for (std::vector<std::string>::const_iterator iter=hardware_name_list.begin(); iter!=hardware_name_list.end(); iter++){
		      if (hardware_name==*iter){histo_flag=false;}
		    }
		    hardware_name_list.push_back(hardware_name);
		    if (histo_flag){ bookMDTHistograms(hardware_name); 
		    }
	      
		    if(tHistSvc()->getHist(m_generic_path_mdtdetcxx,mdtdet).isFailure()) return StatusCode::FAILURE; 
		    if(tHistSvc()->getHist(m_generic_path_mdtlongviewcxx,mdtlongview).isFailure()) return StatusCode::FAILURE; 
		    if(tHistSvc()->getHist(m_generic_path_mdtmultilcxx,mdtmultil).isFailure()) return StatusCode::FAILURE; 
		    if(tHistSvc()->getHist(m_generic_path_mdtlayercxx,mdtlayer).isFailure()) return StatusCode::FAILURE; 
		    if(tHistSvc()->getHist(m_generic_path_mdttubecxx,mdttube).isFailure()) return StatusCode::FAILURE; 
		    if(tHistSvc()->getHist(m_generic_path_mdtetacxx,mdteta).isFailure()) return StatusCode::FAILURE; 
		    if(tHistSvc()->getHist(m_generic_path_mdtphicxx,mdtphi).isFailure()) return StatusCode::FAILURE; 
		    if(tHistSvc()->getHist(m_generic_path_mdtxcxx,mdtx).isFailure()) return StatusCode::FAILURE; 
		    if(tHistSvc()->getHist(m_generic_path_mdtycxx,mdty).isFailure()) return StatusCode::FAILURE; 
		    if(tHistSvc()->getHist(m_generic_path_mdtzcxx,mdtz).isFailure()) return StatusCode::FAILURE; 
	      
		    ///for MDTs that have barrel + endcap section, take only the barrel when plotting the xy-2d plot of the detector
		    if(fabs(mdt_eta)<m_BarrelEtaCut){
		      mdtdet->Fill(globalWire.x(),globalWire.y());
		    }		
		    mdtlongview->Fill(globalWire.z(),rad);
		    mdtmultil->Fill(mLayer); 
		    mdtlayer->Fill(layer);  
		    mdttube->Fill(tube);  
		    mdteta->Fill(mdt_eta);
		    mdtphi->Fill(mdt_phi);
		    mdtx->Fill(globalWire.x());  
		    mdty->Fill(globalWire.y());
		    mdtz->Fill(globalWire.z());
	           }//end chamber writing if statement
	     
		    map<string,int>::iterator iter_digitsperchamber = digitsperchamber_map.find(hardware_name);
		    if ( iter_digitsperchamber  == digitsperchamber_map.end() ){ 
		      digitsperchamber_map.insert( make_pair( hardware_name,1 ) );  
		    } else {iter_digitsperchamber->second+=1;}	  
			
		  }		
		
		}
	    }
	}
      std::string m_generic_path_mdtchamberwithhitcxx = "/muondigits/MDT/mdtdigit_chamberwithhit";
      if(tHistSvc()->getHist(m_generic_path_mdtchamberwithhitcxx,mdtchamberwithhit).isFailure()) return StatusCode::FAILURE; 

      std::string m_generic_path_mdthitspereventcxx = "/muondigits/MDT/mdtdigit_hitsperevent";
      if(tHistSvc()->getHist(m_generic_path_mdthitspereventcxx,mdthitsperevent).isFailure()) return StatusCode::FAILURE; 

      ChamberList.unique();
      mdtchamberwithhit->Fill(ChamberList.size());
      mdthitsperevent->Fill(m_Val_MDT_NumberOfDigits);

    }
  ++event_counter;
  if (event_counter==m_LastEvent) {
  	
    std::string m_generic_path_mdtchamberstatcxx = "/muondigits/MDT/mdtdigit_chamberstat";
		
    /**Writes digits per harware chamber in a txt file*/
    ofstream myfile;
    myfile.open ("mdtdigits.txt");
    myfile << "-------- Digit per Chamber Statistics--------\n";
	
    map<string,int>::iterator iter;
    //bookMDTChamberStats();
    for( iter = digitsperchamber_map.begin(); iter != digitsperchamber_map.end(); iter++ ) {
      //bookMDTChamberStats();	
      if(tHistSvc()->getHist(m_generic_path_mdtchamberstatcxx,mdtchamberstat).isFailure()) return StatusCode::FAILURE; 
	
      const char* chambername_char = iter->first.c_str ();
      /**Fills digits per harware chamber plot*/
      mdtchamberstat->Fill(chambername_char,iter->second);
		
      myfile << iter->first << "            |            " << iter->second << "\n";
    }
    myfile.close();
  }


  return StatusCode::SUCCESS;
}

void MDTDigitsTestAlg::clear_variables()
{
  m_Val_MDT_RunNumber=-1;
  m_Val_MDT_EventNumber=-1;
  m_Val_MDT_NumberOfDigits=-1;
  m_Validation_MDT_StationName=-1;
  m_Validation_MDT_StationEta=-1;
  m_Validation_MDT_StationPhi=-1;
  m_Validation_MDT_WireX=-1;
  m_Validation_MDT_WireY=-1;
  m_Validation_MDT_WireZ=-1;
  m_Validation_MDT_Driftradius=-1;
  m_Validation_MDT_DriftradiusHit=-1;
  m_Validation_MDT_Residuals=-1;
  m_Validation_MDT_IDLayer=-1;
  m_Validation_MDT_IDMultilayer=-1;
  m_Validation_MDT_IDTube=-1;
}


/*---------------------------------------------------------*/
DistSegment MDTDigitsTestAlg::getDistance(const HepGeom::Point3D<double>& lpos,const HepGeom::Vector3D<double>& ldir,
					  const HepGeom::Point3D<double>& tpos,const HepGeom::Vector3D<double>& tdir)
{
  double R0 = (lpos - tpos)*tdir - ( ldir*tdir ) * ( ldir*(lpos - tpos));
  double R1 = 1 - ( ldir*tdir ) * ( ldir*tdir );
  double x = R0/R1;                                // position on this line
  double t = -ldir*(lpos - tpos-tdir*x);           // position on other line
  HepGeom::Point3D<double> pos_tube = tpos + x * tdir;
  HepGeom::Point3D<double> pos_tr   = lpos + t * ldir;
  return DistSegment(pos_tube,pos_tr);
}

/*---------------------------------------------------------*/
const MuonSimData* MDTDigitsTestAlg::getMuonSimData(const Identifier& id, const MuonSimDataCollection& sdoContainer)
{
  MuonSimDataCollection::const_iterator it = sdoContainer.find(id);
  if( it == sdoContainer.end() ) return 0;
  else { return &it->second; }
}

ITHistSvc* MDTDigitsTestAlg::ToolRootHistSvc()
{
  MsgStream log(msgSvc(), name());
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() ) log << MSG::WARNING << ">>> Unable to locate the MDTDigitsTestAlg Histogram service" << endreq;
  return m_rootsvc;
}
