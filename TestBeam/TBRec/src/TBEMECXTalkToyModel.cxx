/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TBEMECXTalkToyModel
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  Johannes Erdmann
CREATED:  October 14,2008

PURPOSE:  A simple toy model to simulate longitudinal cross-talk
          between EMEC2 and EMEC3 in the reco step.
          Code mainly copied from CaloCellContainerCorrectorTool

********************************************************************/

#include "TBEMECXTalkToyModel.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

TBEMECXTalkToyModel::TBEMECXTalkToyModel(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class (type, name, parent),
   m_caloSelection(false),
   m_calo_dd_man(nullptr),
   m_calo_id(nullptr)
{
  declareProperty("CaloNums",m_caloNums);
  declareProperty("XTalkScaleLong",    m_xtalkScaleLong);
  declareProperty("XTalkScaleEta",     m_xtalkScaleEta);
  declareProperty("XTalkScaleEMEC2Eta",m_xtalkScaleEMEC2Eta);
  m_caloNums.clear();
  //default: process all calo
  m_caloNums.push_back( static_cast<int>(CaloCell_ID::NSUBCALO) );
}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode TBEMECXTalkToyModel::initialize()
{
  unsigned int nSubCalo=static_cast<unsigned int>(CaloCell_ID::NSUBCALO) ;

  //check calo number specified
  m_caloSelection = true ;
  if (m_caloNums.size()==0) {
    ATH_MSG_WARNING (" no calo specified for correction. Will do nothing. ");
    return StatusCode::SUCCESS;
  } else if  (m_caloNums.size()>nSubCalo ) {
    ATH_MSG_ERROR (" More than " 
                   << nSubCalo << " calo specified. Must be wrong. Stop.");
    return StatusCode::FAILURE;
  }  else if  (m_caloNums.size()==1 && m_caloNums[0]==static_cast<int>(nSubCalo)) {
    m_caloSelection = false ;
    ATH_MSG_INFO (" Correction will be applied on all calo.");
  } else {
    for (unsigned int index=0; index < m_caloNums.size() ; ++index) {
      if (m_caloNums[index]<0 || m_caloNums[index]>=static_cast<int>(nSubCalo) ) {
        ATH_MSG_ERROR ("Invalid calo specification:" 
                       << m_caloNums[index] << "Stop.");
	return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO (" Correction will be applied on calo:" 
                      << static_cast<int>(m_caloNums[index]));
      }
    }
  }

  m_calo_dd_man = CaloDetDescrManager::instance(); 
  m_calo_id = m_calo_dd_man->getCaloCell_ID();	

  ATH_MSG_VERBOSE ("XTalkScaleLong     = " << m_xtalkScaleLong);
  ATH_MSG_VERBOSE ("XTalkScaleEta      = " << m_xtalkScaleEta);
  ATH_MSG_VERBOSE ("XTalkScaleEMEC2Eta = " << m_xtalkScaleEMEC2Eta);
  ATH_MSG_VERBOSE ("Initialization completed successfully" );

  return StatusCode::SUCCESS;

}

StatusCode TBEMECXTalkToyModel::process (CaloCellContainer* theCont,
                                         const EventContext& /*ctx*/) const
{
  StatusCode sc;

  if (!m_caloSelection) {
    // no selection mode (faster)
    CaloCellContainer::iterator itrCellBeg=theCont->begin();
    CaloCellContainer::iterator itrCellEnd=theCont->end();
    
    sc = processOnCellIterators(itrCellBeg, itrCellEnd );
    if (sc.isFailure()) 
      ATH_MSG_WARNING ("Failure from processOnCellIterators");
  }else {
    // selection mode 

    for (std::vector<int>::const_iterator itrCalo=m_caloNums.begin();itrCalo!=m_caloNums.end();++itrCalo){


      CaloCell_ID::SUBCALO caloNum=static_cast<CaloCell_ID::SUBCALO>(*itrCalo);

      CaloCellContainer::iterator itrCellBeg=theCont->beginCalo(caloNum);
      CaloCellContainer::iterator itrCellEnd=theCont->endCalo(caloNum);

      if (!theCont->hasCalo(caloNum))
      {
        ATH_MSG_WARNING (" Attempt to apply correction but CaloCellContainer has not been filled for this calo : " 
                         << *itrCalo);
      } else 
      {
	sc=processOnCellIterators(itrCellBeg, itrCellEnd );
	if (sc.isFailure()) 
          ATH_MSG_WARNING ("Failure from processOnCellIterators for calo "
                           << static_cast<int> (caloNum));
      }
    } 
  }

  return StatusCode::SUCCESS ;
}

StatusCode TBEMECXTalkToyModel::processOnCellIterators(const CaloCellContainer::iterator &  itrCellBeg,
                                                       const CaloCellContainer::iterator & itrCellEnd ) const
{
  unsigned int myCellHashOffset[CaloCell_ID::NSUBCALO];
  std::set<int> m_validCalos;
  m_validCalos.insert(CaloCell_ID::LAREM);
  std::set<int>::const_iterator vCaloIter = m_validCalos.begin(); 
  std::set<int>::const_iterator vCaloIterEnd = m_validCalos.end(); 
  for(; vCaloIter!=vCaloIterEnd; vCaloIter++) {
    IdentifierHash myHashMin,myHashMax;
    m_calo_id->calo_cell_hash_range ((*vCaloIter),myHashMin,myHashMax);
    myCellHashOffset[(*vCaloIter)] = myHashMin;
  }

  CaloCellContainer::iterator itrCell;
  std::map<Identifier,CaloCell*> cellMap;
  std::map<Identifier,float> energyMap;
  for (itrCell=itrCellBeg;itrCell!=itrCellEnd;++itrCell) {
    CaloCell * theCell = *itrCell;
    cellMap.insert(std::pair<Identifier,CaloCell*>(theCell->ID(),theCell));
    energyMap.insert(std::pair<Identifier,float>(theCell->ID(),theCell->energy()));
  }

  for (itrCell=itrCellBeg;itrCell!=itrCellEnd;++itrCell) {
    CaloCell * theCell = *itrCell;
    const CaloDetDescrElement* element = theCell->caloDDE();

    if (element->is_lar_em_endcap_inner()) {
      CaloCell_ID::CaloSample sample = element->getSampling();

      if (sample==6) { // this is the EMEC2
	std::map<Identifier,float>::iterator cellItEng = energyMap.find(theCell->ID());
	if (cellItEng==energyMap.end()) {
          ATH_MSG_ERROR ("Identifier not found in energyMap");
	  return StatusCode::FAILURE;
	}
	double e = (*cellItEng).second;

        const CaloCell_ID::SUBCALO mySubDet = element->getSubCalo();
	std::vector<IdentifierHash> theNeighbors;

	int otherSubDet;
	Identifier myId = theCell->ID();
	IdentifierHash myHashId = m_calo_id->subcalo_cell_hash(myId,otherSubDet);

	// longitudinal neighbor (N1)
	m_calo_id->get_neighbours(myHashId+myCellHashOffset[mySubDet],LArNeighbours::nextInSamp,theNeighbors);
        ATH_MSG_DEBUG ("N1 theNeighbors.size() = " << theNeighbors.size());
	IdentifierHash nId = theNeighbors.at(0)-myCellHashOffset[mySubDet];
	myId = m_calo_id->cell_id(mySubDet,nId);
	std::map<Identifier,CaloCell*>::iterator cellIt = cellMap.find(myId);
	if (cellIt==cellMap.end()) {
	  ATH_MSG_ERROR ("neighbor CaloCell object not found in cellMap");
	  return StatusCode::FAILURE;
	}
	CaloCell * theCellN1 = (*cellIt).second;
        if (!theCellN1) {
	  ATH_MSG_ERROR ("neighbor CaloCell object not found in cellMap");
	  return StatusCode::FAILURE;
        }
	cellItEng = energyMap.find(theCellN1->ID());
	if (cellItEng==energyMap.end()) {
	  ATH_MSG_ERROR ( "Identifier not found in energyMap" );
	  return StatusCode::FAILURE;
	}
	double eN1 = (*cellItEng).second;

	// neighbors in eta of EMEC3 cell (N2,N3) -- only if they exist
	int EMEC3neighbors = 0;
	myHashId = m_calo_id->subcalo_cell_hash(theCellN1->ID(),otherSubDet);
	//
	m_calo_id->get_neighbours(myHashId+myCellHashOffset[mySubDet],LArNeighbours::nextInEta,theNeighbors);
	ATH_MSG_DEBUG ( "N2 theNeighbors.size() = " << theNeighbors.size() );
	CaloCell * theCellN2 = 0x0;
	double eN2 = 0.;
	if (theNeighbors.size()>0) {
	  nId = theNeighbors.at(0)-myCellHashOffset[mySubDet];
	  myId = m_calo_id->cell_id(mySubDet,nId);
	  cellIt = cellMap.find(myId);
	  theCellN2 = (*cellIt).second;
	  if (cellIt!=cellMap.end()) {
	    cellItEng = energyMap.find(theCellN2->ID());
	    if (cellItEng==energyMap.end()) {
	      ATH_MSG_ERROR ( "Identifier not found in energyMap" );
	      return StatusCode::FAILURE;
	    }
	    eN2 = (*cellItEng).second;
	    EMEC3neighbors++;
	  }
	}
	//
	m_calo_id->get_neighbours(myHashId+myCellHashOffset[mySubDet],LArNeighbours::prevInEta,theNeighbors);
	ATH_MSG_DEBUG ( "N3 theNeighbors.size() = " << theNeighbors.size() );
	CaloCell * theCellN3 = 0x0;
	double eN3 = 0.;
	if (theNeighbors.size()>0) {
	  nId = theNeighbors.at(0)-myCellHashOffset[mySubDet];
	  myId = m_calo_id->cell_id(mySubDet,nId);
	  cellIt = cellMap.find(myId);
	  theCellN3 = (*cellIt).second;
	  if (cellIt!=cellMap.end()) {
	    cellItEng = energyMap.find(theCellN3->ID());
	    if (cellItEng==energyMap.end()) {
	      ATH_MSG_ERROR ( "Identifier not found in energyMap" );
	      return StatusCode::FAILURE;
	    }
	    eN3 = (*cellItEng).second;
	    EMEC3neighbors++;
	  }
	}

	// neighbors in eta of EMEC2 cell (N4,N5) -- only if they exist
	int EMEC2neighbors = 0;
	myHashId = m_calo_id->subcalo_cell_hash(theCell->ID(),otherSubDet);
	//
	m_calo_id->get_neighbours(myHashId+myCellHashOffset[mySubDet],LArNeighbours::nextInEta,theNeighbors);
	ATH_MSG_DEBUG ( "N4 theNeighbors.size() = " << theNeighbors.size() );
	CaloCell * theCellN4 = 0x0;
	double eN4 = 0.;
	if (theNeighbors.size()>0) {
	  nId = theNeighbors.at(0)-myCellHashOffset[mySubDet];
	  myId = m_calo_id->cell_id(mySubDet,nId);
	  cellIt = cellMap.find(myId);
	  theCellN4 = (*cellIt).second;
	  if (cellIt!=cellMap.end()) {
	    cellItEng = energyMap.find(theCellN4->ID());
	    if (cellItEng==energyMap.end()) {
	      ATH_MSG_ERROR ( "Identifier not found in energyMap" );
	      return StatusCode::FAILURE;
	    }
	    eN4 = (*cellItEng).second;
	    EMEC2neighbors++;
	  }
	}
	//
	m_calo_id->get_neighbours(myHashId+myCellHashOffset[mySubDet],LArNeighbours::prevInEta,theNeighbors);
	ATH_MSG_DEBUG ( "N5 theNeighbors.size() = " << theNeighbors.size() );
	CaloCell * theCellN5 = 0x0;
	double eN5 = 0.;
	if (theNeighbors.size()>0) {
	  nId = theNeighbors.at(0)-myCellHashOffset[mySubDet];
	  myId = m_calo_id->cell_id(mySubDet,nId);
	  cellIt = cellMap.find(myId);
	  theCellN5 = (*cellIt).second;
	  if (cellIt!=cellMap.end()) {
	    cellItEng = energyMap.find(theCellN5->ID());
	    if (cellItEng==energyMap.end()) {
	      ATH_MSG_ERROR ( "Identifier not found in energyMap" );
	      return StatusCode::FAILURE;
	    }
	    eN5 = (*cellItEng).second;
	    EMEC2neighbors++;
	  }
	}

	double rescaled_e   = (1.-m_xtalkScaleLong-EMEC3neighbors*m_xtalkScaleEta-EMEC2neighbors*m_xtalkScaleEMEC2Eta)*e
	  + m_xtalkScaleLong*eN1 + m_xtalkScaleEta*(eN2+eN3) + m_xtalkScaleEMEC2Eta*(eN4+eN5)
	  + (theCell->energy()-e);
	double rescaled_eN1 = 0;
        if (theCellN1) rescaled_eN1 = (1.-m_xtalkScaleLong)*eN1 + m_xtalkScaleLong*e + (theCellN1->energy()-eN1);
	double rescaled_eN2 = 0.;
	if (theCellN2) rescaled_eN2 = (1.-m_xtalkScaleEta)*eN2 + m_xtalkScaleEta*e + (theCellN2->energy()-eN2);
	double rescaled_eN3 = 0.;
	if (theCellN3) rescaled_eN3 = (1.-m_xtalkScaleEta)*eN3 + m_xtalkScaleEta*e + (theCellN3->energy()-eN3);
	/*
	double rescaled_eN4 = 0.;
	if (theCellN4) rescaled_eN4 = (1.-m_xtalkScaleEMEC2Eta)*eN4 + m_xtalkScaleEMEC2Eta*e + (theCellN4->energy()-eN4);
	double rescaled_eN5 = 0.;
	if (theCellN5) rescaled_eN5 = (1.-m_xtalkScaleEMEC2Eta)*eN5 + m_xtalkScaleEMEC2Eta*e + (theCellN5->energy()-eN5);
	*/

	ATH_MSG_DEBUG ("");
	ATH_MSG_DEBUG ( "neighbors of cell [" << m_calo_id->show_to_string(theCell  ->ID(),0,'/') << "] : " );
        if (theCellN1)
          ATH_MSG_DEBUG ( "             N1 = [" << m_calo_id->show_to_string(theCellN1->ID(),0,'/') << "]" );
        if (theCellN2)
          ATH_MSG_DEBUG ( "             N2 = [" << m_calo_id->show_to_string(theCellN2->ID(),0,'/') << "]" );
        if (theCellN3)
          ATH_MSG_DEBUG ( "             N3 = [" << m_calo_id->show_to_string(theCellN3->ID(),0,'/') << "]" );
        if (theCellN4)
          ATH_MSG_DEBUG ( "             N4 = [" << m_calo_id->show_to_string(theCellN4->ID(),0,'/') << "]" );
        if (theCellN5)
          ATH_MSG_DEBUG ( "             N5 = [" << m_calo_id->show_to_string(theCellN5->ID(),0,'/') << "]" );
	
	ATH_MSG_DEBUG ( "EMEC2 cell     : energy before = " << e   << " | energy after rescaling = " << rescaled_e   );
	ATH_MSG_DEBUG ( "                                 "
                        << (1.-m_xtalkScaleLong-EMEC3neighbors*m_xtalkScaleEta-EMEC2neighbors*m_xtalkScaleEMEC2Eta)*e
                        << " | " << m_xtalkScaleLong*eN1 
                        << " | " << m_xtalkScaleEta*(eN2+eN3)
                        << " | " << m_xtalkScaleEMEC2Eta*(eN4+eN5)
                        << " | " << (theCell->energy()-e) );
	ATH_MSG_DEBUG ( "EMEC3 cell (N1): energy before = " << eN1 << " | energy after rescaling = " << rescaled_eN1 );
	ATH_MSG_DEBUG ( "                                 " << (1.-m_xtalkScaleLong)*eN1 + m_xtalkScaleLong*e
                        << " | " << (theCellN1->energy()-eN1) );
	if (theCellN2) {
	  ATH_MSG_DEBUG ( "EMEC3 cell (N2): energy before = " << eN2 << " | energy after rescaling = " << rescaled_eN2 );
	  ATH_MSG_DEBUG ( "                                 " << (1.-m_xtalkScaleEta)*eN2 + m_xtalkScaleEta*e
                          << " | " << (theCellN2->energy()-eN2) );
	}
	if (theCellN3) {
	  ATH_MSG_DEBUG ( "EMEC3 cell (N3): energy before = " << eN3 << " | energy after rescaling = " << rescaled_eN3 );
	  ATH_MSG_DEBUG ( "                                 " << (1.-m_xtalkScaleEta)*eN3 + m_xtalkScaleEta*e
                          << " | " << (theCellN3->energy()-eN3) );
	}
	/*
	if (theCellN4) {
	  log << MSG::DEBUG << "EMEC2 cell (N4): energy before = " << eN4 << " | energy after rescaling = " << rescaled_eN4 << endmsg;
	  log << MSG::DEBUG << "                                 " << (1.-m_xtalkScaleEMEC2Eta)*eN4 + m_xtalkScaleEMEC2Eta*e
	      << " | " << (theCellN4->energy()-eN4) << endmsg;
	}
	if (theCellN5) {
	  log << MSG::DEBUG << "EMEC2 cell (N5): energy before = " << eN5 << " | energy after rescaling = " << rescaled_eN5 << endmsg;
	  log << MSG::DEBUG << "                                 " << (1.-m_xtalkScaleEMEC2Eta)*eN5 + m_xtalkScaleEMEC2Eta*e
	      << " | " << (theCellN5->energy()-eN5) << endmsg;
	}
	*/
	theCell  ->setEnergy(rescaled_e);
	if (theCellN1) theCellN1->setEnergy(rescaled_eN1);
	if (theCellN2) theCellN2->setEnergy(rescaled_eN2);
	if (theCellN3) theCellN3->setEnergy(rescaled_eN3);
	//if (theCellN4) theCellN4->setEnergy(rescaled_eN4);
	//if (theCellN5) theCellN5->setEnergy(rescaled_eN5);
      }
    }
  }
  return StatusCode::SUCCESS;
}
