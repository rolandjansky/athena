/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  m_tileID=nullptr; m_tree=nullptr;
}  //end MuonCalibExtraTreeTriggerAlg::MuonCalibExtraTreeTriggerAlg
  
MuonCalibExtraTreeTriggerAlg::~MuonCalibExtraTreeTriggerAlg() {
}

StatusCode MuonCalibExtraTreeTriggerAlg::initialize() {
  
  MsgStream log(msgSvc(), name());
  ATH_MSG_INFO("Initialisation started     " );

  if( m_doMuCTPI ){
    ATH_CHECK( m_rpcRoiTool.retrieve());
    ATH_CHECK( m_tgcRoiTool.retrieve());
  }

  if( m_doMBTS ){
    m_tileID = nullptr;
    ATH_CHECK(detStore()->retrieve(m_tileID));
  }

 ATH_MSG_INFO( "Initialization ended     ");
  return StatusCode::SUCCESS;
}  //end MuonCalibExtraTreeTriggerAlg::initialize

// Execute
StatusCode MuonCalibExtraTreeTriggerAlg::execute(){
 
  if(!m_init) {

    TDirectory* dir = RootFileManager::getInstance()->getDirectory( m_ntupleName.c_str() );
    dir->cd();
      
    ATH_MSG_DEBUG( "Try the GetObject call" );
    dir->GetObject("Segments",m_tree);
      
    ATH_MSG_DEBUG( "retrieved tree " << m_tree );
    
    ATH_MSG_DEBUG( "created tree" );
    if(m_doMuCTPI) m_muCTPIBranch.createBranch(m_tree);
    if(m_doMBTS) m_mbtsBranch.createBranch(m_tree);
    if(m_doLVL1Calo) m_caloBranch.createBranch(m_tree);
    if(m_doCTP)  m_ctpBranch.createBranch(m_tree);

    m_init = true;
  }

  ATH_MSG_DEBUG( " execute()     " );

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
 
  ATH_MSG_DEBUG( " adding CTP information " );
  if( !evtStore()->contains< CTP_RDO >( m_ctpLocation ) ) {
    ATH_MSG_DEBUG( " CTP information no available in StoreGate at " << m_ctpLocation );
    return;
  }

  const CTP_RDO* theCTP_RDO = nullptr;
  if(evtStore()->retrieve(theCTP_RDO, m_ctpLocation).isFailure()) {
    ATH_MSG_WARNING ( "Could not find \"CTP_RDO\" in StoreGate at " << m_ctpLocation );
    return;
  }
  if( !theCTP_RDO ) {
    ATH_MSG_WARNING ( "Could not find \"CTP_RDO\" in StoreGate at " << m_ctpLocation );
    return;
  }
    
  CTP_Decoder ctp;
  ctp.setRDO(theCTP_RDO);
  
  m_ctpBranch.fillBranch(theCTP_RDO);
}
  
void MuonCalibExtraTreeTriggerAlg::addMuCTPI() {
 

  m_muCTPIBranch.reset();

  // Get the MuCTPI RDO from StoreGate:
  if( !evtStore()->contains< MuCTPI_RDO >(m_muCPTILocation) ) {
    ATH_MSG_DEBUG( " MuCTPI information no available in StoreGate at " << m_muCPTILocation );
    return;
  }

  const MuCTPI_RDO* muCTPI_RDO = nullptr;
  if ( evtStore()->retrieve( muCTPI_RDO, m_muCPTILocation ).isFailure() ) {
    ATH_MSG_DEBUG( "Could not find MUCTPI_RDO at " << m_muCPTILocation );
    return;
  }
  ATH_MSG_DEBUG( "Retrieved MUCTPI_RDO at " << m_muCPTILocation );

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
    LVL1::RecMuonRoI rec_roi( data, &( *m_rpcRoiTool ), &( *m_tgcRoiTool ), &dummy_thresholds );
    ATH_MSG_DEBUG( " MuCTPI entry: sys  " << rec_roi.sysID() << " subsys " << rec_roi.subsysID() 
	<< " sectorId " << rec_roi.sectorID()
	<< " roi " << rec_roi.getRoINumber() 
	<< " overlap " << rec_roi.getOverlap()
	<< " eta " << rec_roi.eta() 
	<< " phi " << rec_roi.phi() );
    
    MuCTPI_DataWord_Decoder daqWordDecod = MuCTPI_DataWord_Decoder( daqWord );
    uint16_t   roiBCIDWord = daqWordDecod.getBCID();
    unsigned int roiBCID   = ( roiBCIDWord & 0x7 );
      
    m_muCTPIBranch.fillBranch(rec_roi,roiBCID);
  }
}  //end MuonCalibExtraTreeTriggerAlg::addMuCTPI

void MuonCalibExtraTreeTriggerAlg::addCalo() {
 

  m_caloBranch.reset();

  const CaloCellContainer* theCalocontainer = nullptr;
  if( !evtStore()->contains<CaloCellContainer>(m_caloLocation) ) {
    ATH_MSG_DEBUG( " CaloCellContainer no available in StoreGate at " << m_caloLocation );
    return;
  }

  if(evtStore()->retrieve(theCalocontainer,m_caloLocation).isFailure()) {
    ATH_MSG_WARNING(" Cannot find CaloCell Container in TDS at " << m_caloLocation );
    return;
  }  
  ATH_MSG_VERBOSE ( "Retrieval of CaloCell container succeeded at " << m_caloLocation );

  if( m_tileID ) {    
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
      if (tilecell!=nullptr) {
	Identifier id  = cell->ID();
	float e  = cell->energy() ; 
	if (e < cellEnergyThreshold ) continue ;
	if (e < eCellcut) continue ;
	int sample = m_tileID->sample(id);
	if (sample > 2) continue;

	int k = int(32.0*(cell->phi()/M_PI));
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
	  ATH_MSG_DEBUG( " Store tower energies sample " << sample << " index1 " <<   index1 << " index2 " <<  index2 << " E " << e );
	  ATH_MSG_DEBUG( " k " << k << " m1 " << m1 << " l1 " << l1 << " E " << et1 );
	  ATH_MSG_DEBUG( " k " << k << " m2 " << m2 << " l2 " << l2 << " E " << et2 );
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
	  ATH_MSG_DEBUG( " Store tower energies sample " << sample << " index " <<   index << " E " << e );
	  ATH_MSG_DEBUG( " k " << k << " m " << m << " l " << l <<  " E " << et );
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
      ATH_MSG_DEBUG( " Energy " << std::abs(ite->first) << " index " << ite->second ); 
      std::vector <const CaloCell* > cellVector = towerIndexToCells[ite->second];
      if (nstore > nmaxstore) continue;
      nstore++;
      ATH_MSG_DEBUG( " cells in tower " << cellVector.size() );
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
	ATH_MSG_DEBUG( " x " << (*itc)->x() << " y " <<  (*itc)->y()<< " z " <<  (*itc)->z() << " time " <<  (*itc)->time() << " eta " << (*itc)->eta() << " phi " << (*itc)->phi() << " energy " <<  (*itc)->energy() );
      }
      if (cellVector.size() >  0) {  
	time = time/etot; 
	Amg::Vector3D gpos(xpos/etot,ypos/etot,zpos/etot); 
	ATH_MSG_DEBUG( " Store Calo info " << cellVector.size() << " positions " << gpos << " time " << time << " Etot " << etot );
	// We could also store 
	//int index = ite->second;
	MuonCalibCaloHit hit(cellVector.size(),gpos,time,etot); 
	m_caloBranch.fillBranch(hit);
      }
    }
         
    if (indMax > 0) {
      ATH_MSG_DEBUG( " Cross check Highest Energy " << Emax <<  " index " << indMax );
      std::vector <const CaloCell* > cellVector = towerIndexToCells[indMax];
      ATH_MSG_DEBUG( " cells " << cellVector.size() );
      std::vector<const CaloCell*>::const_iterator itc = cellVector.begin();
      std::vector<const CaloCell*>::const_iterator itc_end = cellVector.end();
      for (;itc!=itc_end;++itc) {
	ATH_MSG_DEBUG( " x " << (*itc)->x() << " y " <<  (*itc)->y()<< " z " <<  (*itc)->z() << " time " <<  (*itc)->time() << " eta " << (*itc)->eta() << " phi " << (*itc)->phi() << " energy " <<  (*itc)->energy() );
      }    
    } 
    if (indsMax > 0) {
      ATH_MSG_DEBUG( " Cross check Second Highest Energy " << Esmax <<  " index " << indsMax );
      std::vector <const CaloCell* > cellVector = towerIndexToCells[indsMax];
      ATH_MSG_DEBUG( " cells " << cellVector.size() );
      std::vector<const CaloCell*>::const_iterator itc = cellVector.begin();
      std::vector<const CaloCell*>::const_iterator itc_end = cellVector.end();
      for (;itc!=itc_end;++itc) {
	ATH_MSG_DEBUG( " x " << (*itc)->x() << " y " <<  (*itc)->y()<< " z " <<  (*itc)->z() << " time " <<  (*itc)->time() << " eta " << (*itc)->eta() << " phi " << (*itc)->phi() << " energy " <<  (*itc)->energy() );
      }    
    } 
  } // endif TileID helper && detStore successful
  
}  //end MuonCalibExtraTreeTriggerAlg::addCalo

void MuonCalibExtraTreeTriggerAlg::addMBTS() { 

  m_mbtsBranch.reset();
    
  if( !evtStore()->contains<TileCellContainer>(m_mbtsLocation) ) {
    ATH_MSG_DEBUG( " MBTS TileCellContainer no available in StoreGate at " << m_mbtsLocation );
    return;
  }

  const TileCellContainer* theMBTScontainer;
  if(evtStore()->retrieve(theMBTScontainer,m_mbtsLocation).isFailure()) {
    ATH_MSG_WARNING(" Cannot find MBTS Container in TDS at " << m_mbtsLocation );
    return;
  }  
  ATH_MSG_VERBOSE ( "Retrieval of MBTS container succeeded at " << m_mbtsLocation );
         
  if( m_tileID ) { 
   
    double energy[32],time[32];
    int module[32];
         
    TileCellContainer::const_iterator iCell = theMBTScontainer->begin();
    TileCellContainer::const_iterator lastCell  = theMBTScontainer->end();
      
    int ic = 0; 
    for( ; iCell != lastCell; ++iCell) {
      Identifier id;
      const TileCell* mbtsCell = *iCell;   // pointer to cell object
      if (mbtsCell != nullptr)  {
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
	    ATH_MSG_VERBOSE ( "Counter: " << counter );
	    ATH_MSG_VERBOSE ( "Energy= " << energy[ic] << " pCb" );
	    ATH_MSG_VERBOSE ( "Time= " << time[ic] ); ;
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
      double phi0 = (module[i]%8)*2*M_PI/8.;
      Amg::Vector3D gpos(radius*std::cos(phi0),radius*std::sin(phi0),ze); 
      MuonCalibCaloHit hit(module[i],gpos,time[i],energy[i]); 
      m_mbtsBranch.fillBranch(hit);
    } // end loop over MBTS modules 
  } // endif TileID helper && detStore successful
} // end MuonCalibExtraTreeTriggerAlg::addMBTS

void MuonCalibExtraTreeTriggerAlg::finishEvent(){
  MsgStream log(msgSvc(), name());
  ATH_MSG_DEBUG( "in finishEvent, filling tree " << m_tree );
  m_tree->Fill();
  ATH_MSG_DEBUG( "tree Filled " );
  if(m_doCTP)  m_ctpBranch.reset();
  if(m_doMBTS) m_mbtsBranch.reset();
  if(m_doMuCTPI) m_muCTPIBranch.reset();
  if(m_doLVL1Calo) m_caloBranch.reset();
  ATH_MSG_DEBUG( "finish event ready... " );
}  //end MuonCalibExtraTreeTriggerAlg::finishEvent
  
}//end namespace MuonCalib
