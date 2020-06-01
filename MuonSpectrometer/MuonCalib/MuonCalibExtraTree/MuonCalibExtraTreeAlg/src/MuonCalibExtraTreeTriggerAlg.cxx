/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibExtraTreeTriggerAlg.h"
#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTriggerInfo.h"

// gaudi
#include "GaudiKernel/MsgStream.h"
#include "TrkEventPrimitives/ParamDefs.h"

//CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

// MBTS trigger
#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileContainer.h"
#include "CaloIdentifier/TileID.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"

#include "MuonCalibNtuple/RootFileManager.h"
#include "MuonCalibStl/DeleteObject.h"

// trigger includes:
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_RIO.h"
#include "TrigT1Result/CTP_Decoder.h"

// TrigT1 include(s):
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include <iostream>
#include <map>

#include <TTree.h>
#include <TDirectory.h>

namespace MuonCalib { 

MuonCalibExtraTreeTriggerAlg::MuonCalibExtraTreeTriggerAlg(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_rpcRoiService( "LVL1RPC::RPCRecRoiSvc", name ),
  m_tgcRoiService( "LVL1TGC::TGCRecRoiSvc", name ),
  m_caloBranch("calo_"),
  m_init(false) {
  declareProperty("doCTP", m_doCTP = true );
  declareProperty("doMuCTPI", m_doMuCTPI = true );
  declareProperty("doMBTS", m_doMBTS = true );
  declareProperty("doLVL1Calo", m_doLVL1Calo = true );
  declareProperty("CTPLocation",m_ctpLocation = "CTP_RDO");
  declareProperty("MuCTPILocation",m_muCPTILocation = "MUCTPI_RDO");
  declareProperty("LVL1CaloContainerLocation",m_caloLocation = "AllCalo");
  declareProperty("MBTSContainerLocation",m_mbtsLocation = "MBTSContainer");
  declareProperty("NtupleName",m_ntupleName = "PatternNtupleMaker");
  m_eventNumber=0;
  m_tileID=NULL; m_tree=NULL;
}  //end MuonCalibExtraTreeTriggerAlg::MuonCalibExtraTreeTriggerAlg
  
MuonCalibExtraTreeTriggerAlg::~MuonCalibExtraTreeTriggerAlg() {
}

StatusCode MuonCalibExtraTreeTriggerAlg::initialize() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialisation started     " << endmsg;

  if( m_doMuCTPI ){
    StatusCode sc = m_rpcRoiService.retrieve();
    if ( sc.isFailure() ) {
      log << MSG::FATAL << "Unable to access RPCRecRoiSvc" << endmsg;
      return sc;
    }
    sc = m_tgcRoiService.retrieve();
    if ( sc.isFailure() ) {
      log << MSG::FATAL << "Unable to access TGCRecRoiSvc" << endmsg;
      return sc;
    }
  }

  if( m_doMBTS ){
    m_tileID = 0;
    StatusCode sc = detStore()->retrieve(m_tileID);
    if ( sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve TileID helper from DetectorStore" << endmsg;
    }
  }

  log << MSG::INFO << "Initialization ended     " << endmsg;
  return StatusCode::SUCCESS;
}  //end MuonCalibExtraTreeTriggerAlg::initialize

// Execute
StatusCode MuonCalibExtraTreeTriggerAlg::execute(){
  MsgStream log(msgSvc(), name());

  if(!m_init) {

    TDirectory* dir = RootFileManager::getInstance()->getDirectory( m_ntupleName.c_str() );
    dir->cd();
      
    log << MSG::DEBUG << "Try the GetObject call" << endmsg;
    dir->GetObject("Segments",m_tree);
      
    log << MSG::DEBUG << "retrieved tree " << m_tree << endmsg;
    
    log << MSG::DEBUG << "created tree" << endmsg;
    if(m_doMuCTPI) m_muCTPIBranch.createBranch(m_tree);
    if(m_doMBTS) m_mbtsBranch.createBranch(m_tree);
    if(m_doLVL1Calo) m_caloBranch.createBranch(m_tree);
    if(m_doCTP)  m_ctpBranch.createBranch(m_tree);

    m_init = true;
  }

  log << MSG::DEBUG << " execute()     " << endmsg;

  if( m_doMuCTPI ) addMuCTPI();
  if( m_doCTP )    addCTP();
  if( m_doMBTS )   addMBTS();
  if( m_doLVL1Calo )   addCalo();
  
  finishEvent();

  return  StatusCode::SUCCESS; 
}  //end MuonCalibExtraTreeTriggerAlg::execute

StatusCode MuonCalibExtraTreeTriggerAlg::finalize() {
  m_mbtsBranch.reset(); 
  m_ctpBranch.reset(); 
  m_caloBranch.reset(); 
  m_muCTPIBranch.reset(); 
  return StatusCode::SUCCESS;
}

void MuonCalibExtraTreeTriggerAlg::addCTP() {
  m_ctpBranch.reset();

  //=======================================================================
  // CTP Information
  //=======================================================================
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " adding CTP information " << endmsg;
  if( !evtStore()->contains< CTP_RDO >( m_ctpLocation ) ) {
    log << MSG::DEBUG << " CTP information no available in StoreGate at " << m_ctpLocation << endmsg;
    return;
  }

  const CTP_RDO* theCTP_RDO = 0;
  if(evtStore()->retrieve(theCTP_RDO, m_ctpLocation).isFailure()) {
    log << MSG::WARNING << "Could not find \"CTP_RDO\" in StoreGate at " << m_ctpLocation << endmsg;
    return;
  }
  if( !theCTP_RDO ) {
    log << MSG::WARNING << "Could not find \"CTP_RDO\" in StoreGate at " << m_ctpLocation << endmsg;
    return;
  }
    
  CTP_Decoder ctp;
  ctp.setRDO(theCTP_RDO);
  
  m_ctpBranch.fillBranch(theCTP_RDO);
}
  
void MuonCalibExtraTreeTriggerAlg::addMuCTPI() {
  MsgStream log(msgSvc(), name());    

  m_muCTPIBranch.reset();

  // Get the MuCTPI RDO from StoreGate:
  if( !evtStore()->contains< MuCTPI_RDO >(m_muCPTILocation) ) {
    log << MSG::DEBUG << " MuCTPI information no available in StoreGate at " << m_muCPTILocation << endmsg;
    return;
  }

  const MuCTPI_RDO* muCTPI_RDO;
  if ( evtStore()->retrieve( muCTPI_RDO, m_muCPTILocation ).isFailure() ) {
    log << MSG::DEBUG << "Could not find MUCTPI_RDO at " << m_muCPTILocation << endmsg;
    return;
  }
  log << MSG::DEBUG << "Retrieved MUCTPI_RDO at " << m_muCPTILocation << endmsg;

  std::vector< uint32_t > dataWords = muCTPI_RDO->dataWord();

  // LVL1::RecMuonRoI needs a vector of TrigConf::TriggerThreshold in it's constructor,
  // bus as I'm not using any functionality of the class that would need it, I just
  // pass it a dummy one:
  std::vector< TrigConf::TriggerThreshold* > dummy_thresholds;

  // Decode the RoI information for each candidate:
  for ( std::vector< uint32_t >::const_iterator it = dataWords.begin(); it != dataWords.end(); ++it ) {
    uint32_t     daqWord = *it;
    unsigned int data = ( daqWord & 0x3fff ) | ( ( daqWord >> 3 ) & 0x7fc000 ) |
                     ( daqWord & 0x10000000 );

 // copied from CBNTAA_MuctpiRoI
    LVL1::RecMuonRoI rec_roi( data, &( *m_rpcRoiService ), &( *m_tgcRoiService ), &dummy_thresholds );
    log << MSG::DEBUG << " MuCTPI entry: sys  " << rec_roi.sysID() << " subsys " << rec_roi.subsysID() 
	<< " sectorId " << rec_roi.sectorID()
	<< " roi " << rec_roi.getRoINumber() 
	<< " overlap " << rec_roi.getOverlap()
	<< " eta " << rec_roi.eta() 
	<< " phi " << rec_roi.phi() << endmsg;
    
    MuCTPI_DataWord_Decoder daqWordDecod = MuCTPI_DataWord_Decoder( daqWord );
    uint16_t   roiBCIDWord = daqWordDecod.getBCID();
    unsigned int roiBCID   = ( roiBCIDWord & 0x7 );
      
    m_muCTPIBranch.fillBranch(rec_roi,roiBCID);
  }
}  //end MuonCalibExtraTreeTriggerAlg::addMuCTPI

void MuonCalibExtraTreeTriggerAlg::addCalo() {
  MsgStream log(msgSvc(), name());    

  m_caloBranch.reset();

  const CaloCellContainer* theCalocontainer;
  if( !evtStore()->contains<CaloCellContainer>(m_caloLocation) ) {
    log << MSG::DEBUG << " CaloCellContainer no available in StoreGate at " << m_caloLocation << endmsg;
    return;
  }

  if(evtStore()->retrieve(theCalocontainer,m_caloLocation).isFailure()) {
    log<<MSG::WARNING<<" Cannot find CaloCell Container in TDS at " << m_caloLocation <<endmsg;
    return;
  }  
  log << MSG::VERBOSE << "Retrieval of CaloCell container succeeded at " << m_caloLocation << endmsg;

  if( m_tileID ) {
    double pi = 4*std::atan(1.);  
    double cellEnergyThreshold = 100. ;
    double eCellcut = 200.;
    double etowercut = 500.;
    int nmodules = 4*64;
    int kNEtaCells = 22;
    CLHEP::HepMatrix towereta(kNEtaCells,1,0);
    CLHEP::HepMatrix etower0(nmodules,kNEtaCells,0);
    CLHEP::HepMatrix etower1(nmodules,kNEtaCells,0);
    std::map <int, std::vector < const CaloCell* > > towerIndexToCells;
    std::map <int, double> indexToEnergy;   
    CaloCellContainer::const_iterator iCell = theCalocontainer->begin();
    CaloCellContainer::const_iterator lastCell  = theCalocontainer->end();
    
    double Emax = 0.; 
    double Esmax = 0.; 
    int indMax = -1;
    int indsMax = -1;
    int ntowers = 0;  
    for( ; iCell != lastCell; ++iCell) {
      const CaloCell* cell = (*iCell) ;
      if(! m_tileID->is_tile(cell->ID()) ) continue;
      const TileCell* tilecell = dynamic_cast<const TileCell*> (cell);
      if (tilecell!=0) {
	Identifier id  = cell->ID();
	float e  = cell->energy() ; 
	if (e < cellEnergyThreshold ) continue ;
	if (e < eCellcut) continue ;
	int sample = m_tileID->sample(id);
	if (sample > 2) continue;

	int k = int(32.0*(cell->phi()/pi));
	if( k < 0) k += 64;
	if(sample == 2){
	  int m1 = int(10.0*(cell->eta() - 0.05));
	  int m2 = int(10.0*(cell->eta() + 0.05));
	  int l1 = 1;
	  int l2 = 1;
	  if (m1>0) l1 = 0; 
	  if (m2>0) l2 = 0;
	  m1 = std::abs(m1);
	  m2 = std::abs(m2);
	  if(l1==0) etower0[k][m1] += e/2.;
	  if(l1==1) etower1[k][m1] += e/2.;
	  if(l2==0) etower0[k][m2] += e/2.;
	  if(l2==1) etower1[k][m2] += e/2.;
	  ntowers += 2;
	  int index1 = k + 1000*l1+10000*m1; 
	  int index2 = k + 1000*l2+10000*m2;

	  // Store Highest energies
	  double et1 = etower0[k][m1];
	  if (l1==1) et1 = etower1[k][m1];
	  double et2 = etower0[k][m2];
	  if (l2==1) et2 = etower1[k][m2];
	  //	  {
	  log << MSG::DEBUG<< " Store tower energies sample " << sample << " index1 " <<   index1 << " index2 " <<  index2 << " E " << e << endmsg;
	  log << MSG::DEBUG<< " k " << k << " m1 " << m1 << " l1 " << l1 << " E " << et1 << endmsg;
	  log << MSG::DEBUG<< " k " << k << " m2 " << m2 << " l2 " << l2 << " E " << et2 << endmsg;
	    //	  }
	  // Keep Maximum energy and second Maximum: Cross check               
	  if (et1 > et2) {
	    if (et1 > Emax) {
	      Esmax = Emax;
	      indsMax = indMax;  
	      Emax = et1;
	      indMax = index1;  
	    } else if (et1 > Esmax) {
	      Esmax = et1;
	      indsMax = index1;  
	    }
	  } else { // et2 > et1
	    if (et2 > Emax) {
	      Esmax = Emax;
	      indsMax = indMax;  
	      Emax = et2;
	      indMax = index2; 
	    } else if (et2 > Esmax) {
	      Esmax = et2;
	      indsMax = index2;
	    } 
	  } 
	  // Store Energies
    
	  indexToEnergy[index1] += e/2;
	  indexToEnergy[index2] += e/2;
	  
	  // Store Cells 
	  if ( towerIndexToCells.count(index1) == 0) {
	    std::vector <const CaloCell* > cellVector; 
	    cellVector.push_back(cell); 
	    towerIndexToCells[index1] = cellVector;
	  } else {
	    std::vector <const CaloCell* > cellVector = towerIndexToCells[index1];
	    cellVector.push_back(cell);
	    towerIndexToCells[index1] = cellVector; 
	  }
	  if ( towerIndexToCells.count(index2) == 0) {
	    std::vector <const CaloCell* > cellVector; 
	    cellVector.push_back(cell); 
	    towerIndexToCells[index2] = cellVector;
	  } else {
	    std::vector <const CaloCell* > cellVector = towerIndexToCells[index2];
	    cellVector.push_back(cell);
	    towerIndexToCells[index2] = cellVector; 
	  }
	} else {  // sample  !=2 
	  int l = 1; 
	  if (cell->eta() > 0) l = 0; 
	  int m = std::abs(int(10.0*cell->eta()));
	  if (m >= kNEtaCells) m = kNEtaCells-1;
	  if (l==0) etower0[k][m] += e;
	  if (l==1) etower1[k][m] += e;
	  towereta[m][0] = std::abs(cell->eta());
	  ntowers++;
	  int index = k + 1000*l+10000*m;
	  double et = etower0[k][m];
	  if (l==1) et = etower1[k][m];
	  //	  {
	  log << MSG::DEBUG<< " Store tower energies sample " << sample << " index " <<   index << " E " << e << endmsg;
	  log << MSG::DEBUG<< " k " << k << " m " << m << " l " << l <<  " E " << et << endmsg;
	  //	  }
	  // Store Energies
	  indexToEnergy[index] += e;
	  // Store Cells
	  if ( towerIndexToCells.count(index) == 0) {
	    std::vector <const CaloCell* > cellVector; 
	    cellVector.push_back(cell); 
	    towerIndexToCells[index] = cellVector;
	  } else {
	    std::vector <const CaloCell* > cellVector = towerIndexToCells[index];
	    cellVector.push_back(cell);
	    towerIndexToCells[index] = cellVector; 
	  }
	  // Keep Maximum energy and second Maximum: Cross check               
	  if (et > Emax) {
	    Esmax = Emax;
	    indsMax = indMax;  
	    Emax = et;
	    indMax = index;  
	  } else if (et > Esmax) {
	    Esmax = et;
	    indsMax = index;  
	  }
	}    //end else sample  !=2 
      }	     //end if tilecell!=0
    }        //end loop over Cells
    //    Order according to -energies
    std::map <double,int> energyToIndex;
    std::map <int,double>::iterator iti = indexToEnergy.begin();
    std::map <int,double>::iterator iti_end = indexToEnergy.end();
    for (;iti!=iti_end;++iti) {
      if (iti->second < etowercut) continue;
      energyToIndex[-(iti->second)] = iti->first;  
    }
    //    Access towers Highest first with highest energy etc
    //    Store information 
    int nmaxstore = 5;
    int nstore = 0;
    std::map <double,int>::iterator ite = energyToIndex.begin();
    std::map <double,int>::iterator ite_end = energyToIndex.end();
    for (;ite!=ite_end;++ite) {
      log << MSG::DEBUG<< " Energy " << std::abs(ite->first) << " index " << ite->second << endmsg; 
      std::vector <const CaloCell* > cellVector = towerIndexToCells[ite->second];
      if (nstore > nmaxstore) continue;
      nstore++;
      log << MSG::DEBUG<< " cells in tower " << cellVector.size() << endmsg;
      std::vector<const CaloCell*>::const_iterator itc = cellVector.begin();
      std::vector<const CaloCell*>::const_iterator itc_end = cellVector.end();
      double xpos = 0;   
      double ypos = 0;   
      double zpos = 0; 
      double time = 0.;  
      double etot = 0.;
      for (;itc!=itc_end;++itc) {
	double ei = (*itc)->energy();
	xpos += (*itc)->x()*ei;
	ypos += (*itc)->y()*ei;
	zpos += (*itc)->z()*ei;
	time += (*itc)->time()*ei;
	etot += ei;
	log << MSG::DEBUG<< " x " << (*itc)->x() << " y " <<  (*itc)->y()<< " z " <<  (*itc)->z() << " time " <<  (*itc)->time() << " eta " << (*itc)->eta() << " phi " << (*itc)->phi() << " energy " <<  (*itc)->energy() << endmsg;
      }
      if (cellVector.size() >  0) {  
	time = time/etot; 
	Amg::Vector3D gpos(xpos/etot,ypos/etot,zpos/etot); 
	log << MSG::DEBUG << " Store Calo info " << cellVector.size() << " positions " << gpos << " time " << time << " Etot " << etot << endmsg;
	// We could also store 
	//int index = ite->second;
	MuonCalibCaloHit hit(cellVector.size(),gpos,time,etot); 
	m_caloBranch.fillBranch(hit);
      }
    }
         
    if (indMax > 0) {
      log << MSG::DEBUG<< " Cross check Highest Energy " << Emax <<  " index " << indMax << endmsg;
      std::vector <const CaloCell* > cellVector = towerIndexToCells[indMax];
      log << MSG::DEBUG<< " cells " << cellVector.size() << endmsg;
      std::vector<const CaloCell*>::const_iterator itc = cellVector.begin();
      std::vector<const CaloCell*>::const_iterator itc_end = cellVector.end();
      for (;itc!=itc_end;++itc) {
	log << MSG::DEBUG << " x " << (*itc)->x() << " y " <<  (*itc)->y()<< " z " <<  (*itc)->z() << " time " <<  (*itc)->time() << " eta " << (*itc)->eta() << " phi " << (*itc)->phi() << " energy " <<  (*itc)->energy() << endmsg;
      }    
    } 
    if (indsMax > 0) {
      log << MSG::DEBUG << " Cross check Second Highest Energy " << Esmax <<  " index " << indsMax << endmsg;
      std::vector <const CaloCell* > cellVector = towerIndexToCells[indsMax];
      log << MSG::DEBUG<< " cells " << cellVector.size() << endmsg;
      std::vector<const CaloCell*>::const_iterator itc = cellVector.begin();
      std::vector<const CaloCell*>::const_iterator itc_end = cellVector.end();
      for (;itc!=itc_end;++itc) {
	log << MSG::DEBUG << " x " << (*itc)->x() << " y " <<  (*itc)->y()<< " z " <<  (*itc)->z() << " time " <<  (*itc)->time() << " eta " << (*itc)->eta() << " phi " << (*itc)->phi() << " energy " <<  (*itc)->energy() << endmsg;
      }    
    } 
  } // endif TileID helper && detStore successful
  
}  //end MuonCalibExtraTreeTriggerAlg::addCalo

void MuonCalibExtraTreeTriggerAlg::addMBTS() {
  MsgStream log(msgSvc(), name());    

  m_mbtsBranch.reset();
    
  if( !evtStore()->contains<TileCellContainer>(m_mbtsLocation) ) {
    log << MSG::DEBUG << " MBTS TileCellContainer no available in StoreGate at " << m_mbtsLocation << endmsg;
    return;
  }

  const TileCellContainer* theMBTScontainer;
  if(evtStore()->retrieve(theMBTScontainer,m_mbtsLocation).isFailure()) {
    log<<MSG::WARNING<<" Cannot find MBTS Container in TDS at " << m_mbtsLocation << endmsg;
    return;
  }  
  log << MSG::VERBOSE << "Retrieval of MBTS container succeeded at " << m_mbtsLocation << endmsg;
         
  if( m_tileID ) {  
    double pi = 4*std::atan(1.); 
    double energy[32],time[32];
    int module[32];
         
    TileCellContainer::const_iterator iCell = theMBTScontainer->begin();
    TileCellContainer::const_iterator lastCell  = theMBTScontainer->end();
      
    int ic = 0; 
    for( ; iCell != lastCell; ++iCell) {
      Identifier id;
      const TileCell* mbtsCell = *iCell;   // pointer to cell object
      if (mbtsCell != 0)  {
	id = mbtsCell->ID();
 
	// Calculate MBTS counter from "side", "tower" and "module"
	// Counter goes 0-31.
	// EBA-inner:0-7,EBA-outer:8-15,EBC-inner:16-23,EBC-outer:24-31
	// tower is 1 for outer counter (lower eta) and 0 for inner counter (higher eta)
	// module counts from 0-7 in increasing phi
	// side is -1 for EBC 1 for EBA
	int counter = (m_tileID->module(id))+8*(m_tileID->tower(id));
	if(m_tileID->side(id)<0) // EBC side
	  counter += 16;
	  
	if ( mbtsCell->energy() > 0.1 && ic < 32) { 
	    
	  // hits above threshold
	  energy[ic] = mbtsCell->energy();
	  time[ic] = mbtsCell->time();
	  module[ic] = counter;
	  if (msgSvc()->outputLevel( name() )< MSG::DEBUG ) {
	    log << MSG::VERBOSE << "Counter: " << counter << endmsg;
	    log << MSG::VERBOSE << "Energy= " << energy[ic] << " pCb" << endmsg;
	    log << MSG::VERBOSE << "Time= " << time[ic] << endmsg ;
	  }
	  //                         double quality = mbtsCell->quality();
	  ic++;
	}
 
      } // end if cell not empty
    } // end Cell loop

    // Store MBTS information as a MuonCalibCaloHit
    for (int i=0; i<ic; i++) {
      double ze = 3560.;
      if (module[i]>15) ze = -ze;
      double radius = 153+(426-153)/2.; 
      if (module[i]%16 > 7) radius = 426 +(890-426)/2.; 
      double phi0 = (module[i]%8)*2*pi/8.;
      Amg::Vector3D gpos(radius*std::cos(phi0),radius*std::sin(phi0),ze); 
      MuonCalibCaloHit hit(module[i],gpos,time[i],energy[i]); 
      m_mbtsBranch.fillBranch(hit);
    } // end loop over MBTS modules 
  } // endif TileID helper && detStore successful
} // end MuonCalibExtraTreeTriggerAlg::addMBTS

void MuonCalibExtraTreeTriggerAlg::finishEvent(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finishEvent, filling tree " << m_tree << endmsg;
  m_tree->Fill();
  log << MSG::DEBUG << "tree Filled " << endmsg;
  if(m_doCTP)  m_ctpBranch.reset();
  if(m_doMBTS) m_mbtsBranch.reset();
  if(m_doMuCTPI) m_muCTPIBranch.reset();
  if(m_doLVL1Calo) m_caloBranch.reset();
  log << MSG::DEBUG << "finish event ready... " << endmsg;
}  //end MuonCalibExtraTreeTriggerAlg::finishEvent
  
}//end namespace MuonCalib
