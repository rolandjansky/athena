/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEFMISSINGET_EFMISSINGETFROMCELLSMT_H
#define TRIGEFMISSINGET_EFMISSINGETFROMCELLSMT_H 1

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

// TrigEFMissingET includes
//#include "TrigEFMissingET/IMissingETTool.h"



/**
 * @class Tool for calculations of MET from bare cells
 * @brief 
 **/

class EFMissingETFromCellsMT: public extends<AthAlgTool, IMissingETTool> { 

 public: 

  EFMissingETFromCellsMT( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  virtual ~EFMissingETFromCellsMT(){}

  virtual StatusCode  initialize() override;

  virtual StatusCode update( xAOD::TrigMissingET *met,
			     TrigEFMissingEtHelper *metHelper,
           const EventContext& ctx  ) const override;

 private: 
  EFMissingETFromCellsMT();
  
  Gaudi::Property<bool>m_doCellNoiseSupp { this, "DoCellNoiseSuppression", false, "switch on/off cell noise suppression" };
  Gaudi::Property<bool>m_useFullColl     { this, "useFullCollection",      true,  "switch on/off fast access via LoadFullCollection" };
  Gaudi::Property<bool>m_makeRobustness  { this, "makeRobustness",         true , "switch on/off robustness checks" };
  Gaudi::Property<float>m_rmsOneSided    { this, "CaloNoiseOneSidedCut",   3.0,   "1-sided noise suppression (unit = rms }" };
  Gaudi::Property<float>m_rmsTwoSided    { this, "CaloNoiseTwoSidedCut",   2.0,   "2-sided noise suppression (unit = rms }" };
  float m_maxThreshold;		//<! maximum between the previous ones
  Gaudi::Property<int>m_MaxLArQ          { this, "MaxLArQ",                4000,  "Max Q LAr Cell value" };
  Gaudi::Property<int>m_MaxTileQ         { this, "MaxTileQ",               254,   "Max Q Tile Cell value" };
  Gaudi::Property<std::vector<float> >m_MinCellSNratio { this, "MinCellSNratio", std::vector<float>(24, 3.0), "(vector) min cell |energy|/rms for robustness checks" };

  SG::ReadHandleKey<CaloCellContainer> m_cellsKey      { this, "CellsCollection", "CaloCells", "Collection containg all cels" };
  
  //PublicToolHandle<ICalorimeterNoiseTool>  m_noiseTool { this, "CaloNoiseTool", "CaloNoiseTool/CaloNoiseToolDefault", "Tool Handle for noise tool" };

}; 


#endif //> !TRIGEFMISSINGET_EFMISSINGETFROMCELLSMT_H
