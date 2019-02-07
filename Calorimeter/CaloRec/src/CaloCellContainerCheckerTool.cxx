/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellContainerCheckerTool
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  David Rousseau
CREATED:  May 12,2004

PURPOSE:  check integrity of CaloCellContainer find and iterators

********************************************************************/

#include "CaloCellContainerCheckerTool.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "CaloGeoHelpers/CaloPhiRange.h"
#include "FourMom/P4PxPyPzE.h"

using CLHEP::MeV;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellContainerCheckerTool::CaloCellContainerCheckerTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class(type, name, parent)
{ 
  declareProperty ("EventsToCheck",m_eventsToCheck = 5);
}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode CaloCellContainerCheckerTool::initialize() {
  return StatusCode::SUCCESS;
}


StatusCode
CaloCellContainerCheckerTool::process (CaloCellContainer* theCont,
                                       const EventContext& ctx) const
{
  return doProcess (theCont, ctx);
}


StatusCode
CaloCellContainerCheckerTool::process (CaloConstCellContainer* theCont,
                                       const EventContext& ctx) const
{
  return doProcess (theCont->asDataVector(), ctx);
}


StatusCode
CaloCellContainerCheckerTool::doProcess (const CaloCellContainer* theCont,
                                         const EventContext& ctx) const
{
  if (ctx.evt() >= m_eventsToCheck) {
    return StatusCode::SUCCESS;
  }

  StatusCode returnSc = StatusCode::SUCCESS ;

  double eSum = 0;
  unsigned int index = 0;
  std::vector<double> eSumCalos;
  std::vector<unsigned int> nCellCalos;
  for (int iCalo=CaloCell_ID::LAREM; iCalo<CaloCell_ID::NSUBCALO ; ++iCalo) {
    eSumCalos.push_back(0.);
    nCellCalos.push_back(0);
  }

  CaloPhiRange aCaloPhiRange;
  

  for (const CaloCell* aCell : *theCont) {
    const CaloDetDescrElement * theDDE=aCell->caloDDE();
    int iCalo = static_cast<int>(theDDE->getSubCalo());
    eSum+=aCell->e();
    eSumCalos[iCalo]+=aCell->e();
    ++(nCellCalos[iCalo]);
    ++index ;
    P4PxPyPzE aP(aCell);
    const double aPeta=aP.e() != 0 ? aP.eta() : 0;
    const double aPphi=aP.phi() ;

    // not needed anymore : now implemented in FOurMom
    //    if (aCell->e()<0) { 
    //   aPeta=-aPeta;
    //  aPphi=aCaloPhiRange.fix(aPphi+aCaloPhiRange.twopi()/2.);
    //}
    

    if (aCell->e()!=0 && std::abs(aCell->eta()-aPeta)>0.0001) {
      msg(MSG::WARNING) << "Cell " << index << " eta inconsistency : " << aCell->eta() 
	  << " vs recalculated " << aPeta << endmsg ;
    }
    if (aCell->e()!=0 && std::abs(aCaloPhiRange.diff(aCell->phi(),aPphi))>0.0001) {
      msg(MSG::WARNING) << "Cell " << index << " phi inconsistency : " << aCell->phi() 
	  << " vs recalculated " << aPphi << endmsg ;
    }
	    

  }
  //std::cout << "step 4" << std::endl;
  index =0;
  if (msgLvl(MSG::VERBOSE)) {
    for (const CaloCell* aCell : *theCont) {
      const CaloDetDescrElement * theDDE=aCell->caloDDE();
      msg(MSG::VERBOSE) << index << " " << " " << aCell
			<< " hash id " << theDDE->calo_hash()
			<< " eta " << theDDE->eta()
			<< " phi " << theDDE->phi()
			<< " e " << aCell->e() << endmsg ;
      ++index;
    }
  }

  //check hasCalo() flags
  for (int iCalo=CaloCell_ID::LAREM; iCalo<CaloCell_ID::NSUBCALO ; ++iCalo) {
    if (nCellCalos[iCalo]>0 && !theCont->hasCalo(static_cast<CaloCell_ID::SUBCALO>(iCalo) ) ) 
    {
      msg(MSG::WARNING) << " There are cells for calo " 
			<< iCalo << " but hasCalo is not set. " << endmsg ;
    }
  }
    
  ATH_MSG_DEBUG(" eSum " << eSum);
  
  //check now the sub calo  iterators 
  for (int iCalo=CaloCell_ID::LAREM; iCalo<CaloCell_ID::NSUBCALO ; ++iCalo) {
    CaloCell_ID::SUBCALO enumCalo=static_cast<CaloCell_ID::SUBCALO>(iCalo);
    double theESumCalo=0;
    unsigned int theNCellCalo=0;

    CaloCellContainer::const_iterator itrCell=theCont->beginConstCalo(enumCalo);
    CaloCellContainer::const_iterator endCell=theCont->endConstCalo(enumCalo);
    for (;itrCell!=endCell;++itrCell){
      theESumCalo+=(*itrCell)->e();
      ++theNCellCalo;
    }

  
    ATH_MSG_DEBUG(" theNCellCalo="<<theNCellCalo
		  <<" nCellCalos[iCalo]=" <<nCellCalos[iCalo] 
		  <<" theCont->nCellsCalo(enumCalo)=" << theCont->nCellsCalo(enumCalo) );

    if (theNCellCalo!=nCellCalos[iCalo] ||
        static_cast<int>(theNCellCalo)!=theCont->nCellsCalo(enumCalo)  ) {
      msg(MSG::ERROR) <<  " Iterators: ncell do not match"
		      << " theNCellCalo="<<theNCellCalo
		      <<" nCellCalos[iCalo]=" <<nCellCalos[iCalo] 
		      <<" theCont->nCellsCalo(enumCalo)=" << theCont->nCellsCalo(enumCalo) 
		      << endmsg;
      returnSc = StatusCode::FAILURE;
    }
      

      
    ATH_MSG_DEBUG("theESumCalo eSumCalos[iCalo] "<< theESumCalo << " " << eSumCalos[iCalo]);
    const double epsilon=1e-5;
    if (fabs(theESumCalo-eSumCalos[iCalo])>epsilon) {
      msg(MSG::ERROR) << " Non const iterators: E sum do not match"
		      << " subcalo " <<  iCalo
		      << " should be " << eSumCalos[iCalo]
		      << " is " << theESumCalo <<" (" << theESumCalo-eSumCalos[iCalo] << ")" << endmsg ;
      returnSc = StatusCode::FAILURE;      
    }

      
  }

  // check find methods

  // count number of holes
  const CaloDetDescrManager * theCaloDDM = CaloDetDescrManager::instance() ;
  const CaloCell_ID * theCaloCCIDM   = theCaloDDM->getCaloCell_ID() ;
  unsigned int hashMax=theCaloCCIDM->calo_cell_hash_max();
  unsigned int nHoles =0;
  
  ATH_MSG_DEBUG("Now check all hashes give meaningful answer");
  for (unsigned int theHash=0;theHash<hashMax;++theHash){
    const CaloCell * theCell = theCont->findCell(theHash) ;
    if (theCell==0) {
      ++nHoles;
      if (msgLvl(MSG::VERBOSE)) {
	//const  CaloDetDescrElement* theCaloDDM->dde=get_element(theHash);
	const Identifier id=theCaloCCIDM->cell_id(theHash);
	const int subcalo=theCaloCCIDM->sub_calo(theHash);
	const int sampling=theCaloCCIDM->calo_sample(theHash);
	const int pos_neg=theCaloCCIDM->pos_neg(id);
	const int region=theCaloCCIDM->region(id);
	const int sample=theCaloCCIDM->sample(id);
	msg(MSG::VERBOSE) << "Cell not found: id=0x" << std::hex << id << std::dec 
			  << ", SubCalo=" <<subcalo <<", sampling="<< sampling << ", pos_neg=" 
			  << pos_neg << ", region=" << region << ", sample=" << sample << endmsg;
      }//end if verbose
    }//end if cell==0
  }//end loop over hashes
  ATH_MSG_DEBUG("Number of hash with no cells: " << nHoles << " out of " << hashMax);
  
  std::vector<IdentifierHash> hashes ;
  hashes.clear();
  double eSumFound=0;

  index =0;
  
  for (  CaloCellContainer::const_iterator itrCell=theCont->begin();itrCell!=theCont->end();++itrCell){
    IdentifierHash theHash = (*itrCell)->caloDDE()->calo_hash();
    if ((*itrCell)->et()>2000 * MeV ) {
      hashes.push_back(theHash);
      eSumFound+=(*itrCell)->energy();
    }
    const CaloCell * reCell=theCont->findCell(theHash);
    if (reCell!=*itrCell){

      const CaloDetDescrElement * reCaloDDE=0 ;
      unsigned int reHash=0 ;
      

      if (reCell!=0) reCaloDDE=reCell->caloDDE();
      if (reCaloDDE!=0) reHash = reCaloDDE->calo_hash();
      
      
      msg(MSG::ERROR) << index << "Cell " << theHash << " " << *itrCell
		      << " not found back " << reCell 
		      << " reCaloDDE " << reCaloDDE
		      << " rehash " << reHash << endmsg ;
      returnSc = StatusCode::FAILURE;
    }
    ++index;
  }

  ATH_MSG_DEBUG("number of cells to be refound " << hashes.size() << " total energy " << eSumFound);
    
  CaloCellContainer::CellVector foundCells;
  foundCells.clear();
  
  theCont->findCellVector(hashes,foundCells);
  
  if (hashes.size()!=foundCells.size()){
    msg(MSG::ERROR) << "number of cells to be found " << hashes.size() << "number found: " << foundCells.size() << endmsg ; 
    returnSc = StatusCode::FAILURE;
  }

  
  double reSumFound=0;
  
  for (CaloCellContainer::CellVector::const_iterator itrCell=foundCells.begin();itrCell!=foundCells.end(); ++itrCell) 
    if (*itrCell!=0) reSumFound+=(*itrCell)->e();
  
 
  if (std::abs(reSumFound-eSumFound)>10 * MeV ){ 
    msg(MSG::ERROR) << "Found cells wrong E sum " << reSumFound << " instead of " << eSumFound << endmsg ;
    returnSc = StatusCode::FAILURE;
  }


  

  return returnSc ;
}








