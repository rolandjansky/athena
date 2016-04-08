// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETEtaRingsFillerTool.h
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Eta Ring objects.
 */


#ifndef MISSINGETD3PPDMAKER_MISSINGETETARINGSFILLERTOOL_H
#define MISSINGETD3PPDMAKER_MISSINGETETARINGSFILLERTOOL_H


#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "MissingETPerformance/MissingETData.h"

#include "D3PDMakerUtils/BlockFillerTool.h"


class MissingET;


namespace D3PD {


/**
 * @brief Block filler tool for Missing ET objects.
 */
class MissingETEtaRingsFillerTool
  : public BlockFillerTool<CaloCellContainer>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MissingETEtaRingsFillerTool (const std::string& type,
			       const std::string& name,
			       const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();
 
  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const CaloCellContainer& p);
  std::vector<double> etaRingVector(int index, std::string axis, bool doCut);

protected:
  MsgStream mLog;

private:

  ToolHandle<MissingETData>                       _data;
 
  bool m_doNoiseCut;

  std::vector<float>* m_MET_EtaRings;

  //x-compenents
  std::vector<double>* m_MET_AllEtaRings_etx;
  std::vector<double>* m_MET_PreSamplerBEtaRings_etx;
  std::vector<double>* m_MET_EMB1EtaRings_etx;
  std::vector<double>* m_MET_EMB2EtaRings_etx;
  std::vector<double>* m_MET_EMB3EtaRings_etx;
  std::vector<double>* m_MET_PreSamperEEtaRings_etx;
  std::vector<double>* m_MET_EME1EtaRings_etx;
  std::vector<double>* m_MET_EME2EtaRings_etx;
  std::vector<double>* m_MET_EME3EtaRings_etx;
  std::vector<double>* m_MET_HEC0EtaRings_etx;
  std::vector<double>* m_MET_HEC1EtaRings_etx;
  std::vector<double>* m_MET_HEC2EtaRings_etx;
  std::vector<double>* m_MET_HEC3EtaRings_etx;
  std::vector<double>* m_MET_TileBar0EtaRings_etx;
  std::vector<double>* m_MET_TileBar1EtaRings_etx;
  std::vector<double>* m_MET_TileBar2EtaRings_etx;
  std::vector<double>* m_MET_TileGap1EtaRings_etx;
  std::vector<double>* m_MET_TileGap2EtaRings_etx;
  std::vector<double>* m_MET_TileGap3EtaRings_etx;
  std::vector<double>* m_MET_TileExt0EtaRings_etx;
  std::vector<double>* m_MET_TileExt1EtaRings_etx;
  std::vector<double>* m_MET_TileExt2EtaRings_etx;
  std::vector<double>* m_MET_FCAL0EtaRings_etx;
  std::vector<double>* m_MET_FCAL1EtaRings_etx;
  std::vector<double>* m_MET_FCAL2EtaRings_etx;
  
  //y-components
  std::vector<double>* m_MET_AllEtaRings_ety;
  std::vector<double>* m_MET_PreSamplerBEtaRings_ety;
  std::vector<double>* m_MET_EMB1EtaRings_ety;
  std::vector<double>* m_MET_EMB2EtaRings_ety;
  std::vector<double>* m_MET_EMB3EtaRings_ety;
  std::vector<double>* m_MET_PreSamperEEtaRings_ety;
  std::vector<double>* m_MET_EME1EtaRings_ety;
  std::vector<double>* m_MET_EME2EtaRings_ety;
  std::vector<double>* m_MET_EME3EtaRings_ety;
  std::vector<double>* m_MET_HEC0EtaRings_ety;
  std::vector<double>* m_MET_HEC1EtaRings_ety;
  std::vector<double>* m_MET_HEC2EtaRings_ety;
  std::vector<double>* m_MET_HEC3EtaRings_ety;
  std::vector<double>* m_MET_TileBar0EtaRings_ety;
  std::vector<double>* m_MET_TileBar1EtaRings_ety;
  std::vector<double>* m_MET_TileBar2EtaRings_ety;
  std::vector<double>* m_MET_TileGap1EtaRings_ety;
  std::vector<double>* m_MET_TileGap2EtaRings_ety;
  std::vector<double>* m_MET_TileGap3EtaRings_ety;
  std::vector<double>* m_MET_TileExt0EtaRings_ety;
  std::vector<double>* m_MET_TileExt1EtaRings_ety;
  std::vector<double>* m_MET_TileExt2EtaRings_ety;
  std::vector<double>* m_MET_FCAL0EtaRings_ety;
  std::vector<double>* m_MET_FCAL1EtaRings_ety;
  std::vector<double>* m_MET_FCAL2EtaRings_ety;

  //variable sumet-components
  std::vector<double>* m_MET_AllEtaRings_sumet;
  std::vector<double>* m_MET_PreSamplerBEtaRings_sumet;
  std::vector<double>* m_MET_EMB1EtaRings_sumet;
  std::vector<double>* m_MET_EMB2EtaRings_sumet;
  std::vector<double>* m_MET_EMB3EtaRings_sumet;
  std::vector<double>* m_MET_PreSamperEEtaRings_sumet;
  std::vector<double>* m_MET_EME1EtaRings_sumet;
  std::vector<double>* m_MET_EME2EtaRings_sumet;
  std::vector<double>* m_MET_EME3EtaRings_sumet;
  std::vector<double>* m_MET_HEC0EtaRings_sumet;
  std::vector<double>* m_MET_HEC1EtaRings_sumet;
  std::vector<double>* m_MET_HEC2EtaRings_sumet;
  std::vector<double>* m_MET_HEC3EtaRings_sumet;
  std::vector<double>* m_MET_TileBar0EtaRings_sumet;
  std::vector<double>* m_MET_TileBar1EtaRings_sumet;
  std::vector<double>* m_MET_TileBar2EtaRings_sumet;
  std::vector<double>* m_MET_TileGap1EtaRings_sumet;
  std::vector<double>* m_MET_TileGap2EtaRings_sumet;
  std::vector<double>* m_MET_TileGap3EtaRings_sumet;
  std::vector<double>* m_MET_TileExt0EtaRings_sumet;
  std::vector<double>* m_MET_TileExt1EtaRings_sumet;
  std::vector<double>* m_MET_TileExt2EtaRings_sumet;
  std::vector<double>* m_MET_FCAL0EtaRings_sumet;
  std::vector<double>* m_MET_FCAL1EtaRings_sumet;
  std::vector<double>* m_MET_FCAL2EtaRings_sumet;
};

} // namespace D3PD


#endif // not MISSINGETD3PPDMAKER_MISSINGETETARINGSFILLERTOOL_H
