///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDPHYSALGS_CALOCELLCALCENERGYCORR_H
#define CALOCONDPHYSALGS_CALOCELLCALCENERGYCORR_H 1
// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "GaudiKernel/ToolHandle.h"
class LArHVMapTool;



class CaloCellCalcEnergyCorr: public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  CaloCellCalcEnergyCorr( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~CaloCellCalcEnergyCorr(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  virtual StatusCode  stop();

private:
  // Properties
  std::string m_folder;
  std::vector<int> m_calosample;  // which CaloSamples to be filled (numbers from CaloSample enum in CaloCell_Base_ID
  std::vector<float> m_value;     // value to fill for every sample
  std::vector<int> m_hvlines;        // which hvlines should be changed in addition
  std::vector<float> m_hvvalue;    // value to fill for every hvline

  // Private members
  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  const LArEM_ID* m_larem_id;
  const LArHEC_ID* m_larhec_id;
  const LArFCAL_ID* m_larfcal_id;

  ToolHandle<LArHVMapTool> m_hvtool;
  //std::vector<int> GetHVLines(const Identifier& id) ;
}; 

#endif //> !CALOCONDPHYSALGS_CALOCELLCALCENERGYCORR_H
