/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETEtaRingsFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Eta Ring objects.
 */


#include "MissingETD3PDMaker/MissingETEtaRingsFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MissingETEtaRingsFillerTool::MissingETEtaRingsFillerTool (const std::string& type,
							    const std::string& name,
							    const IInterface* parent) 
    :BlockFillerTool<CaloCellContainer> (type, name, parent)
    ,   mLog(msgSvc(), name )
    ,   _data ( "MissingETData" )
  {
    declareProperty("DoNoiseCut", m_doNoiseCut = false);

    book().ignore(); // Avoid coverity warning.
  }

  /**
   * @brief Book variables for this block.
   */
  StatusCode MissingETEtaRingsFillerTool::book()
{
   CHECK( addVariable ("eta",   m_MET_EtaRings) );
   CHECK( addVariable ("etx",   m_MET_AllEtaRings_etx) );
   CHECK( addVariable ("PreSamplerB_etx",   m_MET_PreSamplerBEtaRings_etx) );
   CHECK( addVariable ("EMB1_etx",   m_MET_EMB1EtaRings_etx) );
   CHECK( addVariable ("EMB2_etx",   m_MET_EMB2EtaRings_etx) );
   CHECK( addVariable ("EMB3_etx",   m_MET_EMB3EtaRings_etx) );
   CHECK( addVariable ("PreSamperE_etx",   m_MET_PreSamperEEtaRings_etx) );
   CHECK( addVariable ("EME1_etx",   m_MET_EME1EtaRings_etx) );
   CHECK( addVariable ("EME2_etx",   m_MET_EME2EtaRings_etx) );
  CHECK( addVariable ("EME3_etx",   m_MET_EME3EtaRings_etx) );
  CHECK( addVariable ("HEC0_etx",   m_MET_HEC0EtaRings_etx) );
  CHECK( addVariable ("HEC1_etx",   m_MET_HEC1EtaRings_etx) );
  CHECK( addVariable ("HEC2_etx",   m_MET_HEC2EtaRings_etx) );
  CHECK( addVariable ("HEC3_etx",   m_MET_HEC3EtaRings_etx) );
  CHECK( addVariable ("TileBar0_etx",   m_MET_TileBar0EtaRings_etx) );
  CHECK( addVariable ("TileBar1_etx",   m_MET_TileBar1EtaRings_etx) );
  CHECK( addVariable ("TileBar2_etx",   m_MET_TileBar2EtaRings_etx) );
  CHECK( addVariable ("TileGap1_etx",   m_MET_TileGap1EtaRings_etx) );
  CHECK( addVariable ("TileGap2_etx",   m_MET_TileGap2EtaRings_etx) );
  CHECK( addVariable ("TileGap3_etx",   m_MET_TileGap3EtaRings_etx) );
  CHECK( addVariable ("TileExt0_etx",   m_MET_TileExt0EtaRings_etx) );
  CHECK( addVariable ("TileExt1_etx",   m_MET_TileExt1EtaRings_etx) );
  CHECK( addVariable ("TileExt2_etx",   m_MET_TileExt2EtaRings_etx) );
  CHECK( addVariable ("FCAL0_etx",   m_MET_FCAL0EtaRings_etx) );
  CHECK( addVariable ("FCAL1_etx",   m_MET_FCAL1EtaRings_etx) );
  CHECK( addVariable ("FCAL2_etx",   m_MET_FCAL2EtaRings_etx) );

  CHECK( addVariable ("ety",   m_MET_AllEtaRings_ety) );
  CHECK( addVariable ("PreSamplerB_ety",   m_MET_PreSamplerBEtaRings_ety) );
  CHECK( addVariable ("EMB1_ety",   m_MET_EMB1EtaRings_ety) );
  CHECK( addVariable ("EMB2_ety",   m_MET_EMB2EtaRings_ety) );
  CHECK( addVariable ("EMB3_ety",   m_MET_EMB3EtaRings_ety) );
  CHECK( addVariable ("PreSamperE_ety",   m_MET_PreSamperEEtaRings_ety) );
  CHECK( addVariable ("EME1_ety",   m_MET_EME1EtaRings_ety) );
  CHECK( addVariable ("EME2_ety",   m_MET_EME2EtaRings_ety) );
  CHECK( addVariable ("EME3_ety",   m_MET_EME3EtaRings_ety) );
  CHECK( addVariable ("HEC0_ety",   m_MET_HEC0EtaRings_ety) );
  CHECK( addVariable ("HEC1_ety",   m_MET_HEC1EtaRings_ety) );
  CHECK( addVariable ("HEC2_ety",   m_MET_HEC2EtaRings_ety) );
  CHECK( addVariable ("HEC3_ety",   m_MET_HEC3EtaRings_ety) );
  CHECK( addVariable ("TileBar0_ety",   m_MET_TileBar0EtaRings_ety) );
  CHECK( addVariable ("TileBar1_ety",   m_MET_TileBar1EtaRings_ety) );
  CHECK( addVariable ("TileBar2_ety",   m_MET_TileBar2EtaRings_ety) );
  CHECK( addVariable ("TileGap1_ety",   m_MET_TileGap1EtaRings_ety) );
  CHECK( addVariable ("TileGap2_ety",   m_MET_TileGap2EtaRings_ety) );
  CHECK( addVariable ("TileGap3_ety",   m_MET_TileGap3EtaRings_ety) );
  CHECK( addVariable ("TileExt0_ety",   m_MET_TileExt0EtaRings_ety) );
  CHECK( addVariable ("TileExt1_ety",   m_MET_TileExt1EtaRings_ety) );
  CHECK( addVariable ("TileExt2_ety",   m_MET_TileExt2EtaRings_ety) );
  CHECK( addVariable ("FCAL0_ety",   m_MET_FCAL0EtaRings_ety) );
  CHECK( addVariable ("FCAL1_ety",   m_MET_FCAL1EtaRings_ety) );
  CHECK( addVariable ("FCAL2_ety",   m_MET_FCAL2EtaRings_ety) );


  CHECK( addVariable ("sumet",   m_MET_AllEtaRings_sumet) );
  CHECK( addVariable ("PreSamplerB_sumet",   m_MET_PreSamplerBEtaRings_sumet) );
  CHECK( addVariable ("EMB1_sumet",   m_MET_EMB1EtaRings_sumet) );
  CHECK( addVariable ("EMB2_sumet",   m_MET_EMB2EtaRings_sumet) );
  CHECK( addVariable ("EMB3_sumet",   m_MET_EMB3EtaRings_sumet) );
  CHECK( addVariable ("PreSamperE_sumet",   m_MET_PreSamperEEtaRings_sumet) );
  CHECK( addVariable ("EME1_sumet",   m_MET_EME1EtaRings_sumet) );
  CHECK( addVariable ("EME2_sumet",   m_MET_EME2EtaRings_sumet) );
  CHECK( addVariable ("EME3_sumet",   m_MET_EME3EtaRings_sumet) );
  CHECK( addVariable ("HEC0_sumet",   m_MET_HEC0EtaRings_sumet) );
  CHECK( addVariable ("HEC1_sumet",   m_MET_HEC1EtaRings_sumet) );
  CHECK( addVariable ("HEC2_sumet",   m_MET_HEC2EtaRings_sumet) );
  CHECK( addVariable ("HEC3_sumet",   m_MET_HEC3EtaRings_sumet) );
  CHECK( addVariable ("TileBar0_sumet",   m_MET_TileBar0EtaRings_sumet) );
  CHECK( addVariable ("TileBar1_sumet",   m_MET_TileBar1EtaRings_sumet) );
  CHECK( addVariable ("TileBar2_sumet",   m_MET_TileBar2EtaRings_sumet) );
  CHECK( addVariable ("TileGap1_sumet",   m_MET_TileGap1EtaRings_sumet) );
  CHECK( addVariable ("TileGap2_sumet",   m_MET_TileGap2EtaRings_sumet) );
  CHECK( addVariable ("TileGap3_sumet",   m_MET_TileGap3EtaRings_sumet) );
  CHECK( addVariable ("TileExt0_sumet",   m_MET_TileExt0EtaRings_sumet) );
  CHECK( addVariable ("TileExt1_sumet",   m_MET_TileExt1EtaRings_sumet) );
  CHECK( addVariable ("TileExt2_sumet",   m_MET_TileExt2EtaRings_sumet) );
  CHECK( addVariable ("FCAL0_sumet",   m_MET_FCAL0EtaRings_sumet) );
  CHECK( addVariable ("FCAL1_sumet",   m_MET_FCAL1EtaRings_sumet) );
  CHECK( addVariable ("FCAL2_sumet",   m_MET_FCAL2EtaRings_sumet) );

  return StatusCode::SUCCESS;
}


StatusCode MissingETEtaRingsFillerTool::fill (const CaloCellContainer& p)
{
  _data->setAllCaloCells(&p);
  _data->setCaloCellsExist(true);
  _data->fillEtaRings();
  mLog << MSG::DEBUG << "In MissingETD3PDMaker::MissingETEtaRingsFillerTool the toolMissingETPerformance::MissingETData::fillEtaRings has been executed" << endreq;
  m_MET_EtaRings->reserve(100);
 
  for(int i = 0; i < 100; ++i){
    m_MET_EtaRings->push_back(((float)i)/10.0 - 4.95);
  }
  
  *m_MET_AllEtaRings_etx = etaRingVector(25, "x", m_doNoiseCut);
  *m_MET_PreSamplerBEtaRings_etx =  etaRingVector( CaloCell_ID::PreSamplerB, "x", m_doNoiseCut);       
  *m_MET_EMB1EtaRings_etx = etaRingVector(CaloCell_ID::EMB1, "x", m_doNoiseCut);
  *m_MET_EMB1EtaRings_etx = etaRingVector(CaloCell_ID::EMB1, "x", m_doNoiseCut);
  *m_MET_EMB2EtaRings_etx = etaRingVector(CaloCell_ID::EMB2, "x", m_doNoiseCut);
  *m_MET_EMB3EtaRings_etx = etaRingVector(CaloCell_ID::EMB3, "x", m_doNoiseCut);
  *m_MET_PreSamperEEtaRings_etx = etaRingVector(CaloCell_ID::PreSamplerE, "x", m_doNoiseCut);
  *m_MET_EME1EtaRings_etx = etaRingVector(CaloCell_ID::EME1, "x", m_doNoiseCut);
  *m_MET_EME2EtaRings_etx = etaRingVector(CaloCell_ID::EME2, "x", m_doNoiseCut);
  *m_MET_EME3EtaRings_etx = etaRingVector(CaloCell_ID::EME3, "x", m_doNoiseCut);
  *m_MET_HEC0EtaRings_etx = etaRingVector(CaloCell_ID::HEC0, "x", m_doNoiseCut);
  *m_MET_HEC1EtaRings_etx = etaRingVector(CaloCell_ID::HEC1, "x", m_doNoiseCut);
  *m_MET_HEC2EtaRings_etx = etaRingVector(CaloCell_ID::HEC2, "x", m_doNoiseCut);
  *m_MET_HEC3EtaRings_etx = etaRingVector(CaloCell_ID::HEC3, "x", m_doNoiseCut);
  *m_MET_TileBar0EtaRings_etx = etaRingVector(CaloCell_ID::TileBar0, "x", m_doNoiseCut);
  *m_MET_TileBar1EtaRings_etx = etaRingVector(CaloCell_ID::TileBar1, "x", m_doNoiseCut);
  *m_MET_TileBar2EtaRings_etx = etaRingVector(CaloCell_ID::TileBar2, "x", m_doNoiseCut);
  *m_MET_TileGap1EtaRings_etx = etaRingVector(CaloCell_ID::TileGap1, "x", m_doNoiseCut);
  *m_MET_TileGap2EtaRings_etx = etaRingVector(CaloCell_ID::TileGap2, "x", m_doNoiseCut);
  *m_MET_TileGap3EtaRings_etx = etaRingVector(CaloCell_ID::TileGap3, "x", m_doNoiseCut);
  *m_MET_TileExt0EtaRings_etx = etaRingVector(CaloCell_ID::TileExt0, "x", m_doNoiseCut);
  *m_MET_TileExt1EtaRings_etx = etaRingVector(CaloCell_ID::TileExt1, "x", m_doNoiseCut);
  *m_MET_TileExt2EtaRings_etx = etaRingVector(CaloCell_ID::TileExt2, "x", m_doNoiseCut);
  *m_MET_FCAL0EtaRings_etx = etaRingVector(CaloCell_ID::FCAL0, "x", m_doNoiseCut);
  *m_MET_FCAL1EtaRings_etx = etaRingVector(CaloCell_ID::FCAL1, "x", m_doNoiseCut);
  *m_MET_FCAL2EtaRings_etx = etaRingVector(CaloCell_ID::FCAL2, "x", m_doNoiseCut);

  *m_MET_AllEtaRings_ety= etaRingVector(25, "y", m_doNoiseCut);
  *m_MET_PreSamplerBEtaRings_ety =  etaRingVector( CaloCell_ID::PreSamplerB, "y", m_doNoiseCut);       
  *m_MET_EMB1EtaRings_ety = etaRingVector(CaloCell_ID::EMB1, "y", m_doNoiseCut);
  *m_MET_EMB1EtaRings_ety = etaRingVector(CaloCell_ID::EMB1, "y", m_doNoiseCut);
  *m_MET_EMB2EtaRings_ety = etaRingVector(CaloCell_ID::EMB2, "y", m_doNoiseCut);
  *m_MET_EMB3EtaRings_ety = etaRingVector(CaloCell_ID::EMB3, "y", m_doNoiseCut);
  *m_MET_PreSamperEEtaRings_ety = etaRingVector(CaloCell_ID::PreSamplerE, "y", m_doNoiseCut);
  *m_MET_EME1EtaRings_ety = etaRingVector(CaloCell_ID::EME1, "y", m_doNoiseCut);
  *m_MET_EME2EtaRings_ety = etaRingVector(CaloCell_ID::EME2, "y", m_doNoiseCut);
  *m_MET_EME3EtaRings_ety = etaRingVector(CaloCell_ID::EME3, "y", m_doNoiseCut);
  *m_MET_HEC0EtaRings_ety = etaRingVector(CaloCell_ID::HEC0, "y", m_doNoiseCut);
  *m_MET_HEC1EtaRings_ety = etaRingVector(CaloCell_ID::HEC1, "y", m_doNoiseCut);
  *m_MET_HEC2EtaRings_ety = etaRingVector(CaloCell_ID::HEC2, "y", m_doNoiseCut);
  *m_MET_HEC3EtaRings_ety = etaRingVector(CaloCell_ID::HEC3, "y", m_doNoiseCut);
  *m_MET_TileBar0EtaRings_ety = etaRingVector(CaloCell_ID::TileBar0, "y", m_doNoiseCut);
  *m_MET_TileBar1EtaRings_ety = etaRingVector(CaloCell_ID::TileBar1, "y", m_doNoiseCut);
  *m_MET_TileBar2EtaRings_ety = etaRingVector(CaloCell_ID::TileBar2, "y", m_doNoiseCut);
  *m_MET_TileGap1EtaRings_ety = etaRingVector(CaloCell_ID::TileGap1, "y", m_doNoiseCut);
  *m_MET_TileGap2EtaRings_ety = etaRingVector(CaloCell_ID::TileGap2, "y", m_doNoiseCut);
  *m_MET_TileGap3EtaRings_ety = etaRingVector(CaloCell_ID::TileGap3, "y", m_doNoiseCut);
  *m_MET_TileExt0EtaRings_ety = etaRingVector(CaloCell_ID::TileExt0, "y", m_doNoiseCut);
  *m_MET_TileExt1EtaRings_ety = etaRingVector(CaloCell_ID::TileExt1, "y", m_doNoiseCut);
  *m_MET_TileExt2EtaRings_ety = etaRingVector(CaloCell_ID::TileExt2, "y", m_doNoiseCut);
  *m_MET_FCAL0EtaRings_ety = etaRingVector(CaloCell_ID::FCAL0, "y", m_doNoiseCut);
  *m_MET_FCAL1EtaRings_ety = etaRingVector(CaloCell_ID::FCAL1, "y", m_doNoiseCut);
  *m_MET_FCAL2EtaRings_ety = etaRingVector(CaloCell_ID::FCAL2, "y", m_doNoiseCut);

  *m_MET_AllEtaRings_sumet= etaRingVector(25, "set", m_doNoiseCut);
  *m_MET_PreSamplerBEtaRings_sumet =  etaRingVector( CaloCell_ID::PreSamplerB, "set", m_doNoiseCut);       
  *m_MET_EMB1EtaRings_sumet = etaRingVector(CaloCell_ID::EMB1, "set", m_doNoiseCut);
  *m_MET_EMB1EtaRings_sumet = etaRingVector(CaloCell_ID::EMB1, "set", m_doNoiseCut);
  *m_MET_EMB2EtaRings_sumet = etaRingVector(CaloCell_ID::EMB2, "set", m_doNoiseCut);
  *m_MET_EMB3EtaRings_sumet = etaRingVector(CaloCell_ID::EMB3, "set", m_doNoiseCut);
  *m_MET_PreSamperEEtaRings_sumet = etaRingVector(CaloCell_ID::PreSamplerE, "set", m_doNoiseCut);
  *m_MET_EME1EtaRings_sumet = etaRingVector(CaloCell_ID::EME1, "set", m_doNoiseCut);
  *m_MET_EME2EtaRings_sumet = etaRingVector(CaloCell_ID::EME2, "set", m_doNoiseCut);
  *m_MET_EME3EtaRings_sumet = etaRingVector(CaloCell_ID::EME3, "set", m_doNoiseCut);
  *m_MET_HEC0EtaRings_sumet = etaRingVector(CaloCell_ID::HEC0, "set", m_doNoiseCut);
  *m_MET_HEC1EtaRings_sumet = etaRingVector(CaloCell_ID::HEC1, "set", m_doNoiseCut);
  *m_MET_HEC2EtaRings_sumet = etaRingVector(CaloCell_ID::HEC2, "set", m_doNoiseCut);
  *m_MET_HEC3EtaRings_sumet = etaRingVector(CaloCell_ID::HEC3, "set", m_doNoiseCut);
  *m_MET_TileBar0EtaRings_sumet = etaRingVector(CaloCell_ID::TileBar0, "set", m_doNoiseCut);
  *m_MET_TileBar1EtaRings_sumet = etaRingVector(CaloCell_ID::TileBar1, "set", m_doNoiseCut);
  *m_MET_TileBar2EtaRings_sumet = etaRingVector(CaloCell_ID::TileBar2, "set", m_doNoiseCut);
  *m_MET_TileGap1EtaRings_sumet = etaRingVector(CaloCell_ID::TileGap1, "set", m_doNoiseCut);
  *m_MET_TileGap2EtaRings_sumet = etaRingVector(CaloCell_ID::TileGap2, "set", m_doNoiseCut);
  *m_MET_TileGap3EtaRings_sumet = etaRingVector(CaloCell_ID::TileGap3, "set", m_doNoiseCut);
  *m_MET_TileExt0EtaRings_sumet = etaRingVector(CaloCell_ID::TileExt0, "set", m_doNoiseCut);
  *m_MET_TileExt1EtaRings_sumet = etaRingVector(CaloCell_ID::TileExt1, "set", m_doNoiseCut);
  *m_MET_TileExt2EtaRings_sumet = etaRingVector(CaloCell_ID::TileExt2, "set", m_doNoiseCut);
  *m_MET_FCAL0EtaRings_sumet = etaRingVector(CaloCell_ID::FCAL0, "set", m_doNoiseCut);
  *m_MET_FCAL1EtaRings_sumet = etaRingVector(CaloCell_ID::FCAL1, "set", m_doNoiseCut);
  *m_MET_FCAL2EtaRings_sumet = etaRingVector(CaloCell_ID::FCAL2, "set", m_doNoiseCut);

  mLog << MSG::DEBUG << "MissingETD3PDMaker::MissingETEtaRingsFillerTool -- Eta Rings filled" << endreq;

  return StatusCode::SUCCESS;
}


  std::vector<double> MissingETEtaRingsFillerTool::etaRingVector(int index, std::string axis, bool doCut)
  {
    std::vector<double> temp;
    for(int i = 0; i < 100; ++i){
      if(!doCut){
	if(index == 25 && axis == "x") temp.push_back(_data->eta_rings_all_samples(i)->etx());
	else if (index == 25 && axis == "y")  temp.push_back(_data->eta_rings_all_samples(i)->ety());
	else if (index == 25 && axis == "set")  temp.push_back(_data->eta_rings_all_samples(i)->sumet());
	else{
	  if(axis == "x") temp.push_back(_data->eta_rings_by_sample(index,i)->etx());
	  else if(axis == "y") temp.push_back(_data->eta_rings_by_sample(index,i)->ety());
	  else if(axis == "set") temp.push_back(_data->eta_rings_by_sample(index,i)->sumet());
	}//end of no cut
      }//end of not do cut
      else {
	if(index == 25 && axis == "x") temp.push_back(_data->eta_rings_all_samples_nsigma(i)->etx());
	else if (index == 25 && axis == "y")  temp.push_back(_data->eta_rings_all_samples_nsigma(i)->ety());
	else if (index == 25 && axis == "set")  temp.push_back(_data->eta_rings_all_samples_nsigma(i)->sumet());
	else{
	  if(axis == "x") temp.push_back(_data->eta_rings_by_sample_nsigma(index,i)->etx());
	  else if(axis == "y") temp.push_back(_data->eta_rings_by_sample_nsigma(index,i)->ety());
	  else if(axis == "set") temp.push_back(_data->eta_rings_by_sample_nsigma(index,i)->sumet());
	}//end of do cut
    
      }//end of else
    }
    return temp;
  }//end of etaRingVector


} // namespace D3PD
