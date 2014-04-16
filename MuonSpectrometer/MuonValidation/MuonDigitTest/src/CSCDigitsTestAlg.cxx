/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// CSCDigitsTestAlg
// --------
// based on Ketevi A. Assamagan's CSCDigitValidation
////////////////////////////////////////////////////////////////////////////////

#include "MuonDigitTest/CSCDigitsTestAlg.h"
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
#include "MuonReadoutGeometry/CscReadoutElement.h"

#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonSimEvent/CscHitIdHelper.h" 
 
#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonDigitContainer/CscDigitCollection.h"
#include "MuonDigitContainer/CscDigit.h"
   
#include "GeneratorObjects/McEventCollection.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <TH2D.h>
#include "TTree.h"
#include <string>
 

#include "CLHEP/Vector/ThreeVector.h"

#include "HepMC/GenEvent.h"

#include "TrackRecord/TrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"
 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

 
#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include <algorithm>
#include <cmath>
#include <fstream>

const int max_ValDigitNumber =100 ;
 
const int maxDigits = 200;

const int maxFired = 8192;

using namespace MuonGM;
 

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

CSCDigitsTestAlg::CSCDigitsTestAlg(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator),
  TestAlgInterface(),
  m_pMuonMgr           (0),
  m_pCscIdHelper       (0),
  log( msgSvc(), name )
{
  declareProperty("WriteNtuple",          m_writeNtuple = true);
  declareProperty("McEventKey",           m_key="TruthEvent");
  declareProperty("NtupleFileName",       m_ntupleFileName = "MuonDigits");
  declareProperty("NtupleDirectoryName",  m_ntupleDirName  = "CSCDigitsValidation");
  declareProperty("NtupleTreeName",       m_ntupleTreeName = "CSCTestDigits");	
  declareProperty("DoCSCDigits",          m_DoCSCDigits = true);
  declareProperty("Threshold",            m_threshold = 500);
  declareProperty("DumpTrackRecord",      m_dumpTrackRecord = true);
  m_event = 0;

  log << MSG::INFO << "++++++++++++  CSCDigitsTestAlg created  ++++++++++++"
      << endreq;

}

CSCDigitsTestAlg::~CSCDigitsTestAlg()
{
  log << MSG::INFO << " deleting CSCDigitsTestAlg " << endreq;
}
   
////////////////////////////////////////////////////////////////////////////////
// member function implementation
////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------*/
StatusCode CSCDigitsTestAlg::initialize()
{
  log << MSG::INFO << " initializing CSCDigitsTestAlg " << endreq;
 
  // Locate the StoreGateSvc and initialize our local ptr
  // intitialize transient event store

  StatusCode sc = service("StoreGateSvc", m_sgSvc);

  if (!sc.isSuccess() || 0 == m_sgSvc) {
    log << MSG::ERROR << "CSCDigitsTestAlg: Could not find StoreGateSvc" << endreq;
    return StatusCode::FAILURE;
  }
  else { 
    log << MSG::DEBUG << "Retrieved StoreGateSvc" << endreq;
  } 
      

  // retrieve the StoreGate Active store Service
  
  sc = service("ActiveStoreSvc",m_activeStore);
  if (sc.isFailure()) {
    log<<MSG::ERROR << "Could not retrieve StoreGate ActiveStoreSvc!" <<endreq;
  } else {
    log<<MSG::DEBUG<< "Retrieved StoreGate ActiveStoreSvc." << endreq;
  }



  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore );
  if ( sc.isSuccess() ) 
    {
      sc = detStore->retrieve( m_pMuonMgr );
      if ( sc.isFailure() ) 
	{
	  log << MSG::ERROR 
	      << " CSCDigitsTestAlg::initialize() :  Could not find MuonDetDescrMgr " 
	      << endreq;
	} 
      else 
	{ 
	  m_pCscIdHelper = m_pMuonMgr->cscIdHelper();
	}
    } 
  else 
    {
      log << MSG::ERROR 
	  << " CSCDigitsTestAlg::initialize() : MuonDetDescrMgr not found in DetectorStore " 
	  << endreq;
    }    
      

  if (sc.isFailure()){
    log <<MSG::ERROR << "Could not get DetectorStore"<<endreq;
    return( StatusCode::FAILURE );
  }
	
	
  /// ROOT AANtuple Initialization---------------------------------------

  if (m_writeNtuple) {


    std::string cscStreamAndPath = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
    std::string csctreeName = m_ntupleTreeName;

    std::string csctreePath = cscStreamAndPath+"/"+csctreeName;

    csctree = new TTree(TString(csctreeName), "Muon CSC Digits output");

    StatusCode status;
    status=ToolRootHistSvc()->regTree(csctreePath, csctree);

    csctree->Branch("m_Validation_CSC_NumberOfDigits",&m_Val_CSC_NumberOfDigits,"m_Validation_CSC_NumberOfDigits/I");
    csctree->Branch("m_Validation_CSC_RunNumber",&m_Val_CSC_RunNumber,"m_Validation_CSC_RunNumber/I");
    csctree->Branch("m_Validation_CSC_EventNumber",&m_Val_CSC_EventNumber,"m_Validation_CSC_EventNumber/I");
    csctree->Branch("m_Validation_CSC_StationName",&m_Validation_CSC_StationName,"m_Validation_CSC_StationName/I");
    csctree->Branch("m_Validation_CSC_StationEta",&m_Validation_CSC_StationEta,"m_Validation_CSC_StationEta/I");
    csctree->Branch("m_Validation_CSC_StationPhi",&m_Validation_CSC_StationPhi,"m_Validation_CSC_Statio/I");
    csctree->Branch("m_Validation_CSC_ChamberLayer",&m_Validation_CSC_ChamberLayer,"m_Validation_CSC_ChamberLayer/I");
    csctree->Branch("m_Validation_CSC_WireLayer",&m_Validation_CSC_WireLayer,"m_Validation_CSC_WireLayer/I");
    csctree->Branch("m_Validation_CSC_MeasuresPhi",&m_Validation_CSC_MeasuresPhi,"m_Validation_CSC_MeasuresPhi/I");
    csctree->Branch("m_Validation_CSC_Strip",&m_Validation_CSC_Strip,"m_Validation_CSC_Strip/I");
    csctree->Branch("m_Validation_CSC_PosX",&m_Validation_CSC_PosX,"m_Validation_CSC_PosX/D");
    csctree->Branch("m_Validation_CSC_PosY",&m_Validation_CSC_PosY,"m_Validation_CSC_PosY/D");
    csctree->Branch("m_Validation_CSC_PosZ",&m_Validation_CSC_PosZ,"m_Validation_CSC_PosZ/D");
    csctree->Branch("m_Validation_CSC_Charge",&m_Validation_CSC_Charge,"m_Validation_CSC_Charge/D");

    if(status.isFailure()) {
      log << MSG::DEBUG << "CSCDigitsTestAlg:: Unable to register TTreeTuple : " << csctreePath << endreq;
      return status;
    }

  }
 
  log << MSG::INFO << "CSCDigitsTestAlg:: Initialisation ended  " << endreq;
  return StatusCode::SUCCESS;

}
 

/// Execute - on event by event

/*---------------------------------------------------------*/
StatusCode CSCDigitsTestAlg::execute()
{

  StatusCode sc;
  log << MSG::INFO << " CSCDigitsTestAlg:: execute " << endreq;

  // ---------------------------------------
  // fill event data
  // get EventInfo for run and event number
  const EventInfo* pevt;


  if (storeGateSvc()->retrieve(pevt).isFailure()) {
    log << MSG::FATAL << "CSCDigitsTestAlg::Could not find event" << endreq;
    return(StatusCode::FAILURE);
  }
  else {
    log << MSG::INFO << "CSCDigitsTestAlg::Found EventInfo in SG" << endreq;
  }

  if(isVerbose())
    {
      log << MSG::VERBOSE <<"CSCDigitsTestAlg::Processing EventInfo event #"<<pevt->event_ID()->event_number() << " run: " << pevt->event_ID()->run_number() << endreq;
    }
  
  clear_variables();

  m_Val_CSC_EventNumber = pevt->event_ID()->event_number();
  m_Val_CSC_RunNumber = pevt->event_ID()->run_number();

  if(m_DoCSCDigits) sc = CSCvalidateDigits();


  return StatusCode::SUCCESS;

}
/*---------------------------------------------------------*/
StatusCode CSCDigitsTestAlg::CSCvalidateDigits()
{
  StatusCode sc;
  static  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute(): validateDigits" << endreq;

  std::string key = "CSC_DIGITS";
  const DataHandle <CscDigitContainer> container;
  sc = m_sgSvc->retrieve(container,key);
  if (sc.isFailure())
    {
      log << MSG::ERROR << " Cannot retrieve CSC Container " << endreq;
      return StatusCode::FAILURE;
    }

  // Using the Identifier of a tube or a strip as the key, you can also retrieve
  // the truth information for that particular digit. This is shown in the loop below.

  m_Val_CSC_NumberOfDigits=0;
  for (CscDigitContainer::const_iterator it1_coll=container->begin();	it1_coll!=container->end(); ++it1_coll)
    { // loop over container
      const  CscDigitCollection* cscCollection = *it1_coll;

      Identifier moduleId = cscCollection->identify();
      if (m_pCscIdHelper->validElement(moduleId))
	{
	  for(CscDigitCollection::const_iterator it1_digit=cscCollection->begin(); it1_digit!=cscCollection->end();++it1_digit)
	    {// loop over collection

	      const CscDigit* cscDigit = *it1_digit;
	      if (cscDigit->is_valid(m_pCscIdHelper) )
		{
		  Identifier channelId = cscDigit->identify();

		  const MuonGM::CscReadoutElement * descriptor = m_pMuonMgr->getCscReadoutElement(channelId);
		  Amg::Vector3D pos = descriptor->stripPos(channelId);

		  cscDigit->charge();
		  int stationName = m_pCscIdHelper->stationName(channelId);
		  int stationEta  = m_pCscIdHelper->stationEta(channelId);
		  int stationPhi  = m_pCscIdHelper->stationPhi(channelId);
		  int clayer      = m_pCscIdHelper->chamberLayer(channelId);
		  int wlayer      = m_pCscIdHelper->wireLayer(channelId);
		  int measuresPhi = m_pCscIdHelper->measuresPhi(channelId);
		  int strip 	= m_pCscIdHelper->strip(channelId);

		  m_Validation_CSC_StationName=stationName;
		  m_Validation_CSC_StationEta=stationEta;
		  m_Validation_CSC_StationPhi=stationPhi;
		  m_Validation_CSC_ChamberLayer=clayer;
		  m_Validation_CSC_WireLayer=wlayer;
		  m_Validation_CSC_MeasuresPhi=measuresPhi;
		  m_Validation_CSC_Strip=strip;
		  m_Validation_CSC_PosX=pos.x();
		  m_Validation_CSC_PosY=pos.y();
		  m_Validation_CSC_PosZ=pos.z();
		  m_Validation_CSC_Charge=cscDigit->charge();
		  m_Val_CSC_NumberOfDigits++;

		  csctree->Fill();
		}
	    }
	}
    }
  log << MSG::DEBUG <<"  Number of CSC Collections  Accessed " <<endreq;

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
HepGeom::Point3D<double>  CSCDigitsTestAlg::truth_info(bool& is_muon) const 
{   
  StatusCode status = StatusCode::SUCCESS;
  MsgStream log(msgSvc(), name());
 
  HepGeom::Point3D<double> pvert = HepGeom::Point3D<double>(-10000.0,-10000.0,-10000.0);

  const DataHandle <TrackRecordCollection> collTR;

  std::string location = "MuonEntryLayer";
  status = m_sgSvc->retrieve(collTR,location);

  if (status.isFailure() ) 
    {
      log << MSG::ERROR << "Could not find TrackRecordCollection at " << location << " trying MuonEntryRecord" << endreq;
      
      location = "MuonEntryRecord";
      status = m_sgSvc->retrieve(collTR,location);
      if (status.isFailure() ) 
	{
	  log << MSG::ERROR << "Could not find TrackRecordCollection at " << location <<endreq;
	  is_muon = false;
	} 
    }
  else  if (status.isSuccess())
    {
      log<<MSG::DEBUG<<"TrackRecordCollection retrieved: size = "<<collTR->size()<<" TrackRecord hits!" <<endreq;              

      TrackRecordCollection::const_iterator iterTR;
      // loop over all tracks- cbnt_ttr
      for (iterTR=collTR->begin();iterTR!=collTR->end();++iterTR)
	{
	  if (m_dumpTrackRecord) {
	    log << MSG::INFO << "Track found with pdg id= " << (*iterTR)->GetPDGCode() 
		<< " with energy "<< (*iterTR)->GetEnergy() << "Muon EntryRecord: Pt = " 
		<< ((*iterTR)->GetMomentum()).perp() << endreq;
	  }
	  int pdgCode = (*iterTR)->GetPDGCode();
	  if ( pdgCode == 13 || pdgCode == -13 || pdgCode == 0) { // muons or geantinos 
	    double  x     = ((*iterTR)->GetPosition()).x();
	    double  y     = ((*iterTR)->GetPosition()).y();
	    double  z     = ((*iterTR)->GetPosition()).z();
	    double  rho   = sqrt(x*x+y*y);
            double  phi   = ((*iterTR)->GetMomentum()).phi();
	    double  theta = ((*iterTR)->GetMomentum()).theta();
	    double  eta   = ((*iterTR)->GetMomentum()).eta();
	    double  pt    = ((*iterTR)->GetMomentum()).perp();
	    if (m_dumpTrackRecord) {
	      log << MSG::DEBUG << "EntryRecord: The momentum is " << pt << " MeV" << endreq;
	      log << MSG::DEBUG << "EntryRecord: The eta is " << eta << endreq;
	      log << MSG::DEBUG << "EntryRecord: The vertex is (mm)" << x << " " << y << " "
		  << z << " PDG code = " << pdgCode << endreq;
	    }
	    pvert = HepGeom::Point3D<double> (rho,theta,phi);
	    is_muon = true;
	  }
	}
    }  
  return pvert;
}
/*---------------------------------------------------------*/
double CSCDigitsTestAlg::getDistance(HepGeom::Point3D<double> maxPos, HepGeom::Point3D<double> vertex, 
				     int measuresPhi, Identifier id) {
 
  MsgStream log(messageService(), name());
 
  double dist = -1000.0;
  double thetaStrip = maxPos.theta();
  double phiStrip   = maxPos.phi();
  double thetaMC    = vertex.y(); // x and y components contains theta and phi
  double phiMC      = vertex.z(); // see method truth_info for details

  //double alpha = 11.59 * 3.141592654 / 180; // inclination of the chambre with respect to the vertical 
  double rhoS = sqrt( maxPos.x()*maxPos.x()+maxPos.y()*maxPos.y() );
 
  const MuonGM::CscReadoutElement * descriptor = m_pMuonMgr->getCscReadoutElement(id); 
  Amg::Vector3D theNorm = descriptor->normal();
  if (measuresPhi == 0) theNorm = descriptor->normal(id);
  HepGeom::Point3D<double> normDir = HepGeom::Point3D<double>(theNorm.x(),theNorm.y(),theNorm.z());
  HepGeom::Point3D<double> thePoint = findThePoint(vertex,maxPos,normDir);
 
  double rhoMC = thePoint.perp(); 
  //double rhoMC = maxPos.z() * tan (thetaMC);

  Amg::Vector3D maxPosforlocCtr(maxPos.x(),maxPos.y(),maxPos.z());
  Amg::Vector3D thePointforlocPnt(thePoint.x(),thePoint.y(),thePoint.z());
  Amg::Vector3D localCenter = descriptor->localPos(maxPosforlocCtr);
  Amg::Vector3D localPoint = descriptor->localPos(thePointforlocPnt);
 
  if (measuresPhi==0) {
    dist = (localPoint.z()-localCenter.z());   
    //dist = (rhoMC*cos(phiMC-phiStrip) - rhoS) / cos (alpha); 
  } else {
    dist = (localPoint.y()-localCenter.y());
  } 
 
  if (m_writeNtuple) {
    log << MSG::INFO <<" Comparing MC Truth to GeoModel Position "
	<< " rhoMC = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << rhoMC
	<< " thetaMC = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << thetaMC
	<< " phiMC   = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << phiMC
	<< " rhoStrip = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << rhoS
	<< " thetaStrip = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << thetaStrip
	<< " phiStrip = " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << phiStrip 
	<< " distance from track (mm) = " << setiosflags(std::ios::fixed) << std::setprecision(3) << std::setw(7) << dist
	<< endreq;
  }
  return dist;
}
/*---------------------------------------------------------*/
HepGeom::Point3D<double> CSCDigitsTestAlg::findThePoint(HepGeom::Point3D<double> vertex, HepGeom::Point3D<double> maxPos, HepGeom::Point3D<double> norm) {
  // extrapolate the vertex direction to the plane of the strip or cluster and 
  // find the point on that plane
  // assume a line extrapolation from (0,0,0) which is the case here
  double B = 1.0e+15;  /// some arbitrary constant
  double thetaMC    = vertex.y(); // x and y components contains theta and phi
  double phiMC      = vertex.z(); // see method truth_info for details
   
  HepGeom::Point3D<double> b = HepGeom::Point3D<double>(B*sin(thetaMC)*cos(phiMC), B*sin(thetaMC)*sin(phiMC), B*cos(thetaMC));
  HepGeom::Point3D<double> thePoint = ( maxPos.dot(norm) / b.dot(norm) ) * b;  /// the point on the plane of the strip
  return thePoint;
}
/*---------------------------------------------------------*/
StatusCode CSCDigitsTestAlg::finalize()
{
  log << MSG::ERROR << " CSCDigitsTestAlg :: !!!! in finalize()" << endreq;

  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
ITHistSvc* CSCDigitsTestAlg::ToolRootHistSvc()
  /*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() )
    {
      log << MSG::WARNING << ">>> Unable to locate the CSCDigitsTestAlg Histogram service" << endreq;
    }
  return m_rootsvc;
}


void	CSCDigitsTestAlg::clear_variables()
{
  m_Val_CSC_RunNumber=-1;
  m_Val_CSC_EventNumber=-1;
  m_Val_CSC_NumberOfDigits=-1;
  m_Validation_CSC_StationName=-1;
  m_Validation_CSC_StationEta=-1;
  m_Validation_CSC_StationPhi=-1;
  m_Validation_CSC_ChamberLayer=-1;
  m_Validation_CSC_WireLayer=-1;
  m_Validation_CSC_MeasuresPhi=-1;
  m_Validation_CSC_Strip=-1;
  m_Validation_CSC_PosX=-1;
  m_Validation_CSC_PosY=-1;
  m_Validation_CSC_PosZ=-1;
  m_Validation_CSC_Charge=-1;
}


