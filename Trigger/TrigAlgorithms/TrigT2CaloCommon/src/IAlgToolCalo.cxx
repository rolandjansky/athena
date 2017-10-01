/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     IAlgToolCalo.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
//
// AUTHOR:   Denis Oliveira Damazio
//
// REFERENCES: This will provide infrastructure for the Tools
//		to perform their algorithms
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArRecEvent/LArCell.h"

#include "CaloIdentifier/LArEM_ID.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCellContainer.h"

#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigInterfaces/FexAlgo.h"


// Initialize method for all tools
// Retrieval of all Tools to be used during run
StatusCode IAlgToolCalo::initialize()
{
        ATH_MSG_DEBUG("in initialize() by IAlgToolCalo");

        ATH_CHECK( detStore()->retrieve(m_larMgr) );
        ATH_CHECK( m_data.retrieve() );
        ATH_CHECK( m_geometryTool.retrieve() );

        // Initialize timing service in order to perform some measures
        // of performance
        if( !m_timersvc.empty() && m_timersvc.retrieve().isFailure() ) {
          ATH_MSG_WARNING( name() <<
                           ": Unable to locate TrigTimer Service" );
	  // Does not need to fail the Algorithm if no timing service is found
        } // End of if timing service
        // Initialize four timers for RegionSelector, ByteStreamCnv,
        // Algorithm time, Saving EMShowerMinimal
        if (!m_timersvc.empty()) {
		if ( name().find("Fex",0) == std::string::npos) 
                  ATH_MSG_WARNING( " Name of Alg not found" );
		else {
                std::string basename(name().substr(6,1)+name().substr(name().find("Fex",0)-5,5));
                //basename+=(name().substr(6,1)+name().substr(name().find("Fex",0)-5,5));
                ATH_MSG_INFO( "BaseName is : " << basename );
		HLT::FexAlgo *p = dynamic_cast<HLT::FexAlgo*>(const_cast<IInterface*>(parent()));
		/*
                m_timer[0] = m_timersvc->addItem(basename+"Total");
                m_timer[1] = m_timersvc->addItem(basename+"RegSel");
                m_timer[2] = m_timersvc->addItem(basename+"BSCnv");
                m_timer[3] = m_timersvc->addItem(basename+"Algor");
                m_timer[4] = m_timersvc->addItem(basename+"SaveEM");
		*/
		std::string Total((basename+"Total"));
                m_timer[0] = p->addTimer(Total);
		std::string RegSel((basename+"RegSel"));
                m_timer[1] = p->addTimer(RegSel);
		std::string BSCnv((basename+"BSCnv"));
                m_timer[2] = p->addTimer(BSCnv);
		std::string Algor((basename+"Algor"));
                m_timer[3] = p->addTimer(Algor);
		std::string SaveEM((basename+"SaveEM"));
                m_timer[4] = p->addTimer(SaveEM);
		} // End of check for Fex
        } // End of if timing service

	// Calibration object initialization
	if ( m_limit.size() != 0 ){
		m_calib->initialize(m_limit,m_dimension,m_correction);
	}
        m_lardecoded=true;
        m_tiledecoded=true;

	return StatusCode::SUCCESS;
} // End of initialize

// Finalize method for all tools
// nothing realy important done here
StatusCode IAlgToolCalo::finalize(){
  ATH_MSG_DEBUG( "in finalize() by IAlgToolCalo" );
  return StatusCode::SUCCESS;
} // End of finalize

#ifndef NDEBUG
//#define EXTRADEBUG
// Method designed by Tomek Kott. This method 
// Loops over cells and try to find out about their energy
// Then it prints out a cluster view for debugging purposes.
#ifdef EXTRADEBUG
# define ARG(x) x
#else
# define ARG(x)
#endif

void IAlgToolCalo::PrintCluster(const double ARG(energyFromAlg),
                                const int ARG(nCaloType),
                                const int ARG(nCaloSamp),
                                CaloSampling::CaloSample ARG(SAMP1), 
                                CaloSampling::CaloSample ARG(SAMP2) )
#undef ARG
{
#ifdef EXTRADEBUG

  int CaloTech = -1 ; // 0 for LAr | 1 for Tile;
  std::string Samp1_str;
  std::string Samp2_str;


  std::list<double> phiInfo;
  std::list<double> etaInfo;

  std::list<double>::const_iterator phi_it;
  std::list<double>::const_iterator eta_it;

  if ( ( SAMP1 == CaloSampling::PreSamplerB || SAMP1 == CaloSampling::EMB1 ||
       SAMP1 == CaloSampling::EMB2 || SAMP1 == CaloSampling::EMB3 ||
       SAMP1 == CaloSampling::PreSamplerE || SAMP1 == CaloSampling::EME1 ||
       SAMP1 == CaloSampling::EME2 || SAMP1 == CaloSampling::EME3 ||
       SAMP1 == CaloSampling::HEC0 || SAMP1 == CaloSampling::HEC1 ||
       SAMP1 == CaloSampling::HEC2 ) &&
       ( SAMP2 == CaloSampling::PreSamplerB || SAMP2 == CaloSampling::EMB1 ||
       SAMP2 == CaloSampling::EMB2 || SAMP2 == CaloSampling::EMB3 ||
       SAMP2 == CaloSampling::PreSamplerE || SAMP2 == CaloSampling::EME1 ||
       SAMP2 == CaloSampling::EME2 || SAMP2 == CaloSampling::EME3 ||
       SAMP2 == CaloSampling::HEC0 || SAMP2 == CaloSampling::HEC1 ||
       SAMP2 == CaloSampling::HEC2 ) ) {

         CaloTech = 0;
         if ( SAMP1 == CaloSampling::PreSamplerB )
           Samp1_str="PreSamplerB";
         else if ( SAMP1 == CaloSampling::EMB1 )
           Samp1_str="EMB1";
         else if ( SAMP1 == CaloSampling::EMB2 )
           Samp1_str="EMB2";
         else if ( SAMP1 == CaloSampling::EMB3 )
           Samp1_str="EMB3";
         else if ( SAMP1 == CaloSampling::EME1 )
           Samp1_str="EME1";
         else if ( SAMP1 == CaloSampling::EME2 )
           Samp1_str="EME2";
         else if ( SAMP1 == CaloSampling::EME3 )
           Samp1_str="EME3";
         else if ( SAMP1 == CaloSampling::HEC0 )
           Samp1_str="HEC0";
         else if ( SAMP1 == CaloSampling::HEC1 )
           Samp1_str="HEC1";
         else if ( SAMP1 == CaloSampling::HEC2 )
           Samp1_str="HEC2";

         if ( SAMP2 == CaloSampling::PreSamplerB )
           Samp2_str="PreSamplerB";
         else if ( SAMP2 == CaloSampling::EMB1 )
           Samp2_str="EMB1";
         else if ( SAMP2 == CaloSampling::EMB2 )
           Samp2_str="EMB2";
         else if ( SAMP2 == CaloSampling::EMB3 )
           Samp2_str="EMB3";
         else if ( SAMP2 == CaloSampling::EME1 )
           Samp2_str="EME1";
         else if ( SAMP2 == CaloSampling::EME2 )
           Samp2_str="EME2";
         else if ( SAMP2 == CaloSampling::EME3 )
           Samp2_str="EME3";
         else if ( SAMP2 == CaloSampling::HEC0 )
           Samp2_str="HEC0";
         else if ( SAMP2 == CaloSampling::HEC1 )
           Samp2_str="HEC1";
         else if ( SAMP2 == CaloSampling::HEC2 )
           Samp2_str="HEC2";

  }
  else if ((SAMP1 == CaloSampling::TileBar0 || SAMP1 == CaloSampling::TileExt0||
        SAMP1 == CaloSampling::TileBar1 || SAMP1 == CaloSampling::TileExt1 ||
        SAMP1 == CaloSampling::TileBar2 || SAMP1 == CaloSampling::TileExt2 ) &&
       ( SAMP2 == CaloSampling::TileBar0 ||SAMP2 == CaloSampling::TileExt0 ||
         SAMP2 == CaloSampling::TileBar1 ||SAMP2 == CaloSampling::TileExt1 ||
         SAMP2 == CaloSampling::TileBar2 ||SAMP2 == CaloSampling::TileExt2 ) ) {

         CaloTech = 1;
         if ( SAMP1 == CaloSampling::TileBar0 )
           Samp1_str="TileBar0";
         else if ( SAMP1 == CaloSampling::TileBar1 )
           Samp1_str="TileBar1";
         else if ( SAMP1 == CaloSampling::TileBar2 )
           Samp1_str="TileBar2";
         else if ( SAMP1 == CaloSampling::TileExt0 )
           Samp1_str="TileExt0";
         else if ( SAMP1 == CaloSampling::TileExt1 )
           Samp1_str="TileExt1";
         else if ( SAMP1 == CaloSampling::TileExt2 )
           Samp1_str="TileExt2";

         if ( SAMP2 == CaloSampling::TileBar0 )
           Samp2_str="TileBar0";
         else if ( SAMP2 == CaloSampling::TileBar1 )
           Samp2_str="TileBar1";
         else if ( SAMP2 == CaloSampling::TileBar2 )
           Samp2_str="TileBar2";
         else if ( SAMP2 == CaloSampling::TileExt0 )
           Samp2_str="TileExt0";
         else if ( SAMP2 == CaloSampling::TileExt1 )
           Samp2_str="TileExt1";
         else if ( SAMP2 == CaloSampling::TileExt2 )
           Samp2_str="TileExt2";
  }

  if ( CaloTech == 0 )
  {  
    for ( m_it = m_iBegin; m_it != m_iEnd; ++m_it) {
      if ( 
          (CaloSampling::getSampling(*(*m_it)) == SAMP1||
           CaloSampling::getSampling(*(*m_it)) == SAMP2) ){//If for Det. Description

	double etaCell = (*m_it)->eta();
        double phiCell = (*m_it)->phi();
        if( m_geometryTool->CellInWidCluster(nCaloType,
               nCaloSamp,etaCell,phiCell) ) {
            bool phiDouble=false, etaDouble=false;
            for(phi_it = phiInfo.begin(); phi_it != phiInfo.end(); phi_it++) {
              if( floorf(1e4*(*phi_it)) == floorf(1e4*phiCell) ) {
                 phiDouble = true;
                 break;
              }
            }
            if(phiDouble == false) {
               phiInfo.push_back( phiCell );
            }
            for(eta_it = etaInfo.begin(); eta_it != etaInfo.end(); eta_it++) {
               if( floorf(1e4*(*eta_it)) == floorf(1e4*etaCell )) {
                   etaDouble = true;
                   break;
               }
            }
            if(etaDouble == false) {
                etaInfo.push_back( etaCell );
            }
        } // end of if phi/eta within bounds
      } // End of if Det. Description
    }// end for iterators
  } // end if EM samp
  else if ( CaloTech == 1 )
  {

    for (unsigned int iR=0;iR< m_data->TileContSize();iR++) {

        // For the first sample you will create the containers
        // For the others no
      if ( m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure() ) {
       std::cout << "ERROR loading tile collections." << std::endl;
        return;
      }
      for ( m_itt = m_itBegin; m_itt != m_itEnd; ++m_itt) {

        if ( CaloSampling::getSampling(*(*m_itt)) == SAMP1||
             CaloSampling::getSampling(*(*m_itt)) == SAMP2 ){//If for Det. Description
          double etaCell = (*m_itt)->eta();
          double phiCell = (*m_itt)->phi();
          if( m_geometryTool->CellInWidCluster(nCaloType,nCaloSamp,etaCell,phiCell) ) {
              bool phiDouble=false, etaDouble=false;
              for(phi_it = phiInfo.begin(); phi_it != phiInfo.end(); phi_it++) {
                if( floorf(1e4*(*phi_it)) == floorf(1e4*phiCell) ) {
                   phiDouble = true;
                   break;
                }
              }
              if(phiDouble == false) {
                 phiInfo.push_back( phiCell );
              }
              for(eta_it = etaInfo.begin(); eta_it != etaInfo.end(); eta_it++) {
                 if( floorf(1e4*(*eta_it)) == floorf(1e4*etaCell )) {
                     etaDouble = true;
                     break;
                 }
              }
              if(etaDouble == false) {
                etaInfo.push_back( etaCell );
              }
          } // end of if phi/eta within bounds
        } // End of if Det. Description
      }// end for iterators
    }// end loop tile colletions
  } // end if HAD samp
  else
  {
    std::cout << "Incompatible choice of SAMP1= " << SAMP1 << " and SAMP2 = "<< SAMP2 << std::endl;
    return;
  }

  phiInfo.sort();
  etaInfo.sort();

  std::cout << "\n" << name() << " for " << Samp1_str << " and " << Samp2_str 
            << std::endl;
  std::cout << "\nphiInfo size: " << phiInfo.size()
            << ", etaInfo size: " << etaInfo.size()
            << " Energy total value : " << energyFromAlg
            << "\nphiInfo and etaInfo are populated by: "
            << "(Energy values expressed in MeV)";

  std::cout << "\n" << std::setw(9) << "phi/eta |";
  for (eta_it = etaInfo.begin(); eta_it != etaInfo.end(); eta_it++)
       std::cout << std::setw(7) << *eta_it << "|";
  std::cout << "\n";
  for (phi_it = phiInfo.begin(); phi_it != phiInfo.end(); phi_it++) {
     std::cout << std::setw(7) << *phi_it << " |";
     for (eta_it = etaInfo.begin(); eta_it != etaInfo.end(); eta_it++) {
        double Energy=0.;
       if ( CaloTech == 0 ) {
         for ( m_it = m_iBegin; m_it != m_iEnd; ++m_it) {
           if ( (CaloSampling::getSampling(*(*m_it)) == SAMP1||
              CaloSampling::getSampling(*(*m_it)) == SAMP2) ) {//If for Det. Description

              if ( floorf(1e4*(*phi_it)) == floorf(1e4* ((*m_it)->phi()) ) &&
                  floorf(1e4*(*eta_it)) == floorf(1e4* ((*m_it)->eta()) )) {
                 Energy += (*m_it)->energy();
              }
           } // End of if Det. Description
         }// end for iterators
       }
       else if (CaloTech == 1 ) {
         for (unsigned int iR=0;iR< m_data->TileContSize();iR++) {

        // For the first sample you will create the containers
        // For the others no
           if ( m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure() ) {
             std::cout << "ERROR loading tile collections." << std::endl;
             return;
           }

           for ( m_itt = m_itBegin; m_itt != m_itEnd; ++m_itt) {
             if ( (CaloSampling::getSampling(*(*m_itt)) == SAMP1||
                CaloSampling::getSampling(*(*m_itt)) == SAMP2) ) {//If for Det. Description

                if ( floorf(1e4*(*phi_it)) == floorf(1e4* ((*m_itt)->phi()) ) &&
                  floorf(1e4*(*eta_it)) == floorf(1e4* ((*m_itt)->eta()) )) {
                  Energy += (*m_itt)->energy();
                }
             } // End of if Det. Description
           }// end for iterators
         }//end loop tile collections
       }
        std::cout << std::setw(7) << (int)floorf(Energy) << "|";
     } // end of for etaInfo
     std::cout << "\n";
  } // end of for phiInfo
#endif // End of if EXTRADEBUG
return;
} // End of PrintCluster

#endif

// very loose implementation. No worry about time performance
void IAlgToolCalo::storeCells( void ) {

        CaloCellContainer * ContainerLAr;
        CaloCellContainer * ContainerTile;
	if ( evtStore()->contains<CaloCellContainer>("RoILArCells") ) {
	        ATH_MSG_DEBUG("Found Container, will retrieve it");
	if ( evtStore()->retrieve(ContainerLAr,"RoILArCells").isFailure() ){
	        ATH_MSG_DEBUG("Could not retrieve container : RoILarCells");
	}
	} else {
	     ATH_MSG_DEBUG("Creating Container RoILArCells");
          ContainerLAr = new CaloCellContainer();
	
             if ( evtStore()->record(ContainerLAr,"RoILArCells").isFailure() ){
	        ATH_MSG_ERROR("Error! Could not store Container");
             }
        } // End of if contains
	if ( evtStore()->contains<CaloCellContainer>("RoITileCells") ) {
	        ATH_MSG_DEBUG("Found Container, will retrieve it");
	if ( evtStore()->retrieve(ContainerTile,"RoITileCells").isFailure() ){
	        ATH_MSG_DEBUG("Could not retrieve container : RoITileCells");
	}
	} else {
	     ATH_MSG_DEBUG("Creating Container RoITileCells");
             ContainerTile = new CaloCellContainer();
             if ( evtStore()->record(ContainerTile,"RoITileCells").isFailure() ){
	        ATH_MSG_ERROR("Could not store Container");
             }
        } // End of if retrieve
        // Now I have a LAr container
        if ( m_lardecoded )
        for ( m_it = m_iBegin; m_it != m_iEnd; ++m_it) {
                CaloCell* larcell = (CaloCell*)(*m_it)->clone();
                ContainerLAr->push_back(larcell);
        }
        ATH_MSG_DEBUG( "LAr Container size : " << ContainerLAr->size() );
        // Now I have a Tile container
        if ( m_tiledecoded )
	  for ( m_itt = m_itBegin; m_itt != m_itEnd; ++m_itt) {
	    CaloCell* tilecell = (CaloCell*)(*m_itt)->clone();
	    ContainerTile->push_back(tilecell);
	  }
        ATH_MSG_DEBUG( "Tile Container size : "<< ContainerTile->size() );
}
