// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloSysD3PDMaker/src/CellFillerTool.h
 * @author Francesco Lanni
 * @date 2012
 * @brief Block filler tool for making super cells and fill ntuple. 
 */

#ifndef CALOSYSD3PDMAKER_SCFILLERTOOL_H
#define CALOSYSD3PDMAKER_SCFILLERTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "LArCabling/LArCablingLegacyService.h"

#include <vector>
#include <list>
#include <map>

class CaloCell;
class CaloCellContainer;

// Forward declaration(s):
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class TileID;
class ICaloNoiseTool;
class LArOnlineID;
class Identifier;

namespace D3PD {


/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class SCFillerTool
  : public BlockFillerTool<CaloCellContainer>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SCFillerTool (const std::string& type,
		const std::string& name,
		const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();
  virtual StatusCode initialize();
  virtual StatusCode fill (const CaloCellContainer& p);
  virtual void fillHashTables(const CaloCellContainer& p);
  virtual void dumpHashTables(const CaloCellContainer& p);
  virtual void resetEnergies();

private:
 
  const LArEM_ID   *m_emid;
  const LArFCAL_ID *m_fcalid;
  const LArHEC_ID  *m_hecid;
  const TileID     *m_tileid;    
  const LArOnlineID* m_onlineHelper;
  ToolHandle<LArCablingLegacyService> m_larCablingSvc;
  //  const CaloDetDescrManager*  m_dd_man; 


  /// parameters
  int *m_nSC;
  std::vector<double> *m_E;
  std::vector<double> *m_eta;
  std::vector<double> *m_phi;
  std::vector<double> *m_Et;
  std::vector<double> *m_time;
  std::vector<double> *m_quality;

  std::vector<int> *m_calo;
  std::vector<int> *m_region;
  std::vector<int> *m_sampling;
  std::vector<int> *m_ieta;
  std::vector<int> *m_jphi;
  std::vector<int> *m_hashid;

  bool m_tableFilled;
  std::map<const Identifier, int>   m_sc_hashTable;
  std::vector<int> m_sc_list;
  std::map<int, double> m_sc_energy;
  std::map<int, double> m_sc_eta;
  std::map<int, double> m_sc_phi;
  std::map<int, std::vector<const CaloCell*> > m_sc_compId;

  std::vector<float> m_etaCut;
  std::vector<float> m_phiCut;
  std::vector< unsigned int > m_caloNums; 
  std::vector< unsigned int >  m_caloLayers;

  bool m_caloEtaSelection;
  bool m_caloPhiSelection;
  bool m_caloLayerSelection;
  bool m_caloSelection;

  bool m_tileDLayerOption;

};


} // namespace D3PD


#endif // not CALOSYSD3PDMAKER_SCFILLERTOOL_H
