/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloSysD3PDMaker/src/CaloInfoFillerTool.cxx
 * @author Hong Ma
 * @date  March, 2010
 * @brief Block filler tool for CaloCellContainer Info
 */

#include "CaloInfoFillerTool.h"
#include "CaloEvent/CaloCell.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "LArIdentifier/LArOnlineID.h"

#include <sstream>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CaloInfoFillerTool::CaloInfoFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<CaloCellContainer> (type, name, parent),m_posneg(0)
{

  declareProperty("PosNeg",m_posneg = 0);
  declareProperty("DoEt",m_doEt = 0);

  CaloInfoFillerTool::book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode CaloInfoFillerTool::book()
{

  CHECK( addVariable("Nh_Calo",m_Nh_Calo));
  CHECK( addVariable("Eh_Calo",m_Eh_Calo));
  CHECK( addVariable("Nh_EM",m_Nh_EM));
  CHECK( addVariable("Eh_EM",m_Eh_EM));
  CHECK( addVariable("Nh_HAD",m_Nh_HAD));
  CHECK( addVariable("Eh_HAD",m_Eh_HAD));
  CHECK( addVariable("Nh_PresB",m_Nh_PresB));
  CHECK( addVariable("Eh_PresB",m_Eh_PresB));
  CHECK( addVariable("Nh_EMB",m_Nh_EMB));
  CHECK( addVariable("Eh_EMB",m_Eh_EMB));
  CHECK( addVariable("Nh_EMEC",m_Nh_EMEC));
  CHECK( addVariable("Eh_EMEC",m_Eh_EMEC));
  CHECK( addVariable("Nh_Tile",m_Nh_Tile));
  CHECK( addVariable("Eh_Tile",m_Eh_Tile));
  CHECK( addVariable("Nh_TileGap",m_Nh_TileGap));
  CHECK( addVariable("Eh_TileGap",m_Eh_TileGap));
  CHECK( addVariable("Nh_HEC",m_Nh_HEC));
  CHECK( addVariable("Eh_HEC",m_Eh_HEC));
  CHECK( addVariable("Nh_FCal",m_Nh_FCal));
  CHECK( addVariable("Eh_FCal",m_Eh_FCal));
  CHECK( addVariable("Nh_PresE",m_Nh_PresE));
  CHECK( addVariable("Eh_PresE",m_Eh_PresE));
  CHECK( addVariable("Nh_Scint",m_Nh_Scint));
  CHECK( addVariable("Eh_Scint",m_Eh_Scint));
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode CaloInfoFillerTool::fill (const CaloCellContainer& cellCont)
{
  *m_Nh_EM=0 ;
  *m_Nh_HAD=0 ;
  *m_Nh_PresB=0 ;
  *m_Nh_PresE=0 ;
  *m_Nh_EMB=0 ;
  *m_Nh_EMEC=0 ;
  *m_Nh_Tile=0 ;
  *m_Nh_TileGap=0 ;
  *m_Nh_Scint=0 ;
  *m_Nh_HEC=0 ;
  *m_Nh_FCal=0 ;
  *m_Eh_Calo=0. ;
  *m_Eh_EM=0. ;
  *m_Eh_HAD=0. ;

  m_Eh_PresB->clear();  
  m_Eh_PresB->resize(1,0.);
  
  m_Eh_EMB->clear() ;
  m_Eh_EMB->resize(3,0.);
 

  m_Eh_EMEC->clear() ;
  m_Eh_EMEC->resize(3,0.);
 
  m_Eh_Tile->clear() ;
  m_Eh_Tile->resize(4,0.);
 
  m_Eh_TileGap->clear() ;
  m_Eh_TileGap->resize(1,0.);
 
  m_Eh_HEC->clear() ;
  m_Eh_HEC->resize(4,0.);
 
  m_Eh_FCal->clear() ;
  m_Eh_FCal->resize(3,0.);
 
  m_Eh_PresE->clear() ;
  m_Eh_PresE->resize(1,0.);
 
  m_Eh_Scint->clear() ;
  m_Eh_Scint->resize(1,0.);
 
  *m_Nh_EM = cellCont.nCellsCalo(CaloCell_ID::LAREM);
  *m_Nh_Calo = cellCont.nCellsCalo(CaloCell_ID::LAREM);


  *m_Nh_HAD += cellCont.nCellsCalo(CaloCell_ID::LARHEC);
  *m_Nh_Calo += cellCont.nCellsCalo(CaloCell_ID::LARHEC);

  *m_Nh_HAD += cellCont.nCellsCalo(CaloCell_ID::LARFCAL);
  *m_Nh_Calo += cellCont.nCellsCalo(CaloCell_ID::LARFCAL);

  *m_Nh_HAD += cellCont.nCellsCalo(CaloCell_ID::TILE);
  *m_Nh_Calo += cellCont.nCellsCalo(CaloCell_ID::TILE);

  for (const CaloCell* cell : cellCont)
    {
      double theCellE=cell->energy();
      if (m_doEt) theCellE *= cell->sinTh(); // calculate a basic Et

      int inum = CaloSamplingHelper::getSampling( *cell );

      const CaloDetDescrElement * caloDDE=cell->caloDDE();

      // check if only one side is requested.
      if ( ( m_posneg==-1 && caloDDE->eta_raw()>0 ) ||  ( m_posneg==1 && caloDDE->eta_raw()<0 ) )
	continue ;
	  
      *m_Eh_Calo  += theCellE ;

      CaloCell_ID::SUBCALO theSUBCALO = caloDDE->getSubCalo() ;

      // count EM energy separately
      if (theSUBCALO==CaloCell_ID::LAREM) {
	*m_Eh_EM  += theCellE;
      }
      else {
	*m_Eh_HAD  += theCellE;
      }
      
      //now layer per layer energy
      if (CaloSampling::PreSamplerB == inum) {
	*m_Nh_PresB += 1;
	(*m_Eh_PresB)[0] += theCellE;
      }
      else if (CaloSampling::PreSamplerE == inum) {
	*m_Nh_PresE += 1;
	(*m_Eh_PresE)[0] += theCellE;
      }
      else if (CaloSampling::EMB1 == inum || CaloSampling::EMB2 == inum 
	       || CaloSampling::EMB3 == inum) {
	*m_Nh_EMB += 1;
	if (CaloSampling::EMB1 == inum) (*m_Eh_EMB)[0] += theCellE;
	if (CaloSampling::EMB2 == inum) (*m_Eh_EMB)[1] += theCellE;
	if (CaloSampling::EMB3 == inum) (*m_Eh_EMB)[2] += theCellE;
      }
      else if (CaloSampling::EME1 == inum || CaloSampling::EME2 == inum 
	       || CaloSampling::EME3 == inum) {
	*m_Nh_EMEC += 1;
	if (CaloSampling::EME1 == inum) (*m_Eh_EMEC)[0] += theCellE;
	if (CaloSampling::EME2 == inum) (*m_Eh_EMEC)[1] += theCellE;
	if (CaloSampling::EME3 == inum) (*m_Eh_EMEC)[2] += theCellE;
      }
      else if (CaloSampling::TileBar0 == inum  || 
	       CaloSampling::TileBar1 == inum  || 
	       CaloSampling::TileBar2 == inum  || 
	       CaloSampling::TileExt0 == inum  || 
	       CaloSampling::TileExt1 == inum  || 
	       CaloSampling::TileExt2 == inum ) {
	*m_Nh_Tile += 1;
	if (CaloSampling::TileBar0 == inum ||
	    CaloSampling::TileExt0 == inum )
	  (*m_Eh_Tile)[0] += theCellE;
	if (CaloSampling::TileBar1 == inum ||
	    CaloSampling::TileExt1 == inum )
	  (*m_Eh_Tile)[1] += theCellE;
	if (CaloSampling::TileBar2 == inum ||
	    CaloSampling::TileExt2 == inum )
	  (*m_Eh_Tile)[2] += theCellE;
      }
      else if (CaloSampling::TileGap1 == inum  || 
	       CaloSampling::TileGap2 == inum  || 
	       CaloSampling::TileGap3 == inum ) {
	*m_Nh_TileGap += 1;
	(*m_Eh_TileGap)[0] += theCellE;
	if (CaloSampling::TileGap3 == inum  ) {
	  *m_Nh_Scint += 1;
	  (*m_Eh_Scint)[0] += theCellE;
	}
      }
      else if (CaloSampling::HEC0 == inum  || 
	       CaloSampling::HEC1 == inum  || 
	       CaloSampling::HEC2 == inum  || 
	       CaloSampling::HEC3 == inum  ) {
	*m_Nh_HEC += 1;
	if (CaloSampling::HEC0 == inum ) (*m_Eh_HEC)[0] += theCellE;
	if (CaloSampling::HEC1 == inum ) (*m_Eh_HEC)[1] += theCellE;
	if (CaloSampling::HEC2 == inum ) (*m_Eh_HEC)[2] += theCellE;
	if (CaloSampling::HEC3 == inum ) (*m_Eh_HEC)[3] += theCellE;
      }
      else if (CaloSampling::FCAL0 == inum  || 
	       CaloSampling::FCAL1 == inum  || 
	       CaloSampling::FCAL2 == inum  ) {
	*m_Nh_FCal += 1;
	if (CaloSampling::FCAL0 == inum ) (*m_Eh_FCal)[0] += theCellE;
	if (CaloSampling::FCAL1 == inum ) (*m_Eh_FCal)[1] += theCellE;
	if (CaloSampling::FCAL2 == inum ) (*m_Eh_FCal)[2] += theCellE;
      }
    }

  ATH_MSG_DEBUG(" EM Cell hits: "<< *m_Nh_EM  );
  ATH_MSG_DEBUG(" total energy in EM: "<< *m_Eh_EM  );
  ATH_MSG_DEBUG(" total energy in EM barrel PS: "<< (*m_Eh_PresB)[0]  );
  ATH_MSG_DEBUG(" total energy in EM endcap PS "<< (*m_Eh_PresE)[0]  );

  ATH_MSG_DEBUG(" HAD Cell hits: "<< *m_Nh_HAD  );
  ATH_MSG_DEBUG(" total energy in HAD: "<< *m_Eh_HAD  );
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD
