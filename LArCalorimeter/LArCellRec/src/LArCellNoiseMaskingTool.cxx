/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArCellNoiseMaskingTool
PACKAGE:  offline/Calorimeter/LArCellRec

AUTHORS: G.Unal
CREATION:  4 feb 2009

PURPOSE:

********************************************************************/

#include "LArCellNoiseMaskingTool.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "Identifier/Identifier.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArCellNoiseMaskingTool::LArCellNoiseMaskingTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class (type, name, parent),
   m_qualityCut(65536),
   m_maskNoise(true),
   m_maskSporadic(true)
{ 
  declareProperty("qualityCut",m_qualityCut,"Quality cut for sporadic noise channel");
  m_caloNums.clear();
  m_caloNums.push_back( static_cast<int>(CaloCell_ID::LAREM) );
  m_caloNums.push_back( static_cast<int>(CaloCell_ID::LARHEC) );
  m_caloNums.push_back( static_cast<int>(CaloCell_ID::LARFCAL) );

}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArCellNoiseMaskingTool::initialize() {

  m_maskNoise= not m_problemsToMask.empty();
  m_maskSporadic = not m_sporadicProblemsToMask.empty();

  ATH_CHECK(m_bcContKey.initialize(m_maskNoise or m_maskSporadic));

  ATH_CHECK(m_noiseMask.buildBitMask(m_problemsToMask,msg()));
  ATH_CHECK(m_sporadicNoiseMask.buildBitMask(m_sporadicProblemsToMask,msg()));

  if (m_maskNoise) {
    ATH_MSG_INFO (" Cell masking for noise/dead channels activated");
  }
  else {
    ATH_MSG_INFO (" Cell masking for noise/dead channels not activated ");
  }

  if (m_maskSporadic) {
    ATH_MSG_INFO (" Cell masking for sporadic noise activated");
  }
  else {
    ATH_MSG_INFO (" Cell masking for sporadic noise not activated ");
  }

  return StatusCode::SUCCESS;
}

StatusCode
LArCellNoiseMaskingTool::process (CaloCellContainer * theCont,
                                  const EventContext& ctx) const
{
  StatusCode returnSc = StatusCode::SUCCESS ;

  if (!m_maskNoise && !m_maskSporadic) return returnSc;

  SG::ReadCondHandle<LArBadChannelCont> bcContHdl{m_bcContKey,ctx};
  const LArBadChannelCont* bcCont{*bcContHdl};

  for (std::vector<int>::const_iterator itrCalo=m_caloNums.begin();itrCalo!=m_caloNums.end();++itrCalo){
      CaloCell_ID::SUBCALO caloNum=static_cast<CaloCell_ID::SUBCALO>(*itrCalo);
      CaloCellContainer::iterator itrCell=theCont->beginCalo(caloNum);
      CaloCellContainer::iterator itrCellEnd=theCont->endCalo(caloNum);

      for (; itrCell!=itrCellEnd;++itrCell){
          CaloCell * aCell=*itrCell;
          Identifier cellId = aCell->ID();

          bool toMask = false;

// sporadic noise masking using quality cut.   Logic could be improved using neighbour information...
          if (m_maskSporadic) {
              if (aCell->quality() > m_qualityCut) {
		if (m_sporadicNoiseMask.cellShouldBeMasked(bcCont,cellId)) {
		  toMask=true;
		  ATH_MSG_DEBUG (" Mask sporadic noise cell" << cellId << " E,t,chi2 " << aCell->energy() << " " << aCell->time() << " " << aCell->quality());
		}
              }      
          }

// high noise / dead channel masking
          if (m_maskNoise) {
	    if (m_noiseMask.cellShouldBeMasked(bcCont,cellId)) {
	      toMask=true;
	      ATH_MSG_DEBUG (" Mask highNoise/dead  cell" << cellId << " E,t,chi2 " << aCell->energy() << " " << aCell->time() << " " << aCell->quality());
	    }
          }


          if (toMask) {
              aCell->setEnergy(0.);
              aCell->setTime(0.);
              uint16_t qua=0;
              aCell->setQuality(qua);
              uint16_t provenance = (aCell->provenance() | 0x0800);
              aCell->setProvenance(provenance);
          }

      }
  }


  return returnSc ;
}
