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
   m_maskingTool(this),
   m_maskingSporadicTool(this),
   m_qualityCut(65536),
   m_maskNoise(true),
   m_maskSporadic(true)
{ 
  declareProperty("MaskingTool",m_maskingTool,"Tool handle for noisy/dead channel masking");
  declareProperty("MaskingSporadicTool",m_maskingSporadicTool,"Tool handle for sporadic noisy channel masking");
  declareProperty("qualityCut",m_qualityCut,"Quality cut for sporadic noise channel");
  declareProperty("maskNoise",m_maskNoise,"Flag to mask high Noise / dead channels");
  declareProperty("maskSporadic",m_maskSporadic,"Flag to mask sporadic noise channels");
  m_caloNums.clear();
  m_caloNums.push_back( static_cast<int>(CaloCell_ID::LAREM) );
  m_caloNums.push_back( static_cast<int>(CaloCell_ID::LARHEC) );
  m_caloNums.push_back( static_cast<int>(CaloCell_ID::LARFCAL) );

}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArCellNoiseMaskingTool::initialize()
{
  if (!m_maskingTool.empty()) {
    ATH_CHECK( m_maskingTool.retrieve() );
    ATH_MSG_INFO (" Cell masking for noise/dead channels activated using bad channel masking tool ");
  }
  else {
    ATH_MSG_INFO (" Cell masking for noise/dead channels not activated ");
  }

  if (!m_maskingSporadicTool.empty()) {
    ATH_CHECK( m_maskingSporadicTool.retrieve() );
    ATH_MSG_INFO (" Cell masking for sporadic noise activated using  masking tool ");
  }
  else {
    ATH_MSG_INFO (" Cell masking for sporadic noise not activated ");
  }

  return StatusCode::SUCCESS;
}

StatusCode
LArCellNoiseMaskingTool::process (CaloCellContainer * theCont,
                                  const EventContext& /*ctx*/) const
{
  StatusCode returnSc = StatusCode::SUCCESS ;

  if (!m_maskNoise && !m_maskSporadic) return returnSc;

  bool sporadicMask = !m_maskingSporadicTool.empty() && m_maskSporadic;
  bool noiseMask    = !m_maskingTool.empty() && m_maskNoise;

  for (std::vector<int>::const_iterator itrCalo=m_caloNums.begin();itrCalo!=m_caloNums.end();++itrCalo){
      CaloCell_ID::SUBCALO caloNum=static_cast<CaloCell_ID::SUBCALO>(*itrCalo);
      CaloCellContainer::iterator itrCell=theCont->beginCalo(caloNum);
      CaloCellContainer::iterator itrCellEnd=theCont->endCalo(caloNum);

      for (; itrCell!=itrCellEnd;++itrCell){
          CaloCell * aCell=*itrCell;
          Identifier cellId = aCell->ID();

          bool toMask = false;

// sporadic noise masking using quality cut.   Logic could be improved using neighbour information...
          if (sporadicMask) {
              if (aCell->quality() > m_qualityCut) {
                  if (m_maskingSporadicTool->cellShouldBeMasked(cellId)) {
                      toMask=true;
                      ATH_MSG_DEBUG (" Mask sporadic noise cell" << cellId << " E,t,chi2 " << aCell->energy() << " " << aCell->time() << " " << aCell->quality());
                  }
              }      
          }

// high noise / dead channel masking
          if (noiseMask) {
             if (m_maskingTool->cellShouldBeMasked(cellId)) {
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
