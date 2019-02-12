/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSYSD3PDMAKER_LARDIGITFILLERTOOL_H
#define CALOSYSD3PDMAKER_LARDIGITFILLERTOOL_H
/**
 * @file CaloSysD3PDMaker/src/LArDigitFillerTool.h
 * @author Hong Ma <hma@bnl.gov>
 * @date March 8, 2010
 * @brief Block filler tool for CaloCell, implementing similar features of CBNTAA_CaloCell
 */

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArOnlineID;


namespace D3PD {


/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class LArDigitFillerTool
  : public BlockFillerTool<LArDigit>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  LArDigitFillerTool (const std::string& type,
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
  virtual StatusCode fill (const LArDigit& p);

private: 

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  // flags
  bool m_savesca;
  bool m_savedigit;
  //int m_maxChannels;
  std::string m_CaloType; // EM, HEC, FCAL
  bool m_storeId;

  bool m_em;
  bool m_hec;
  bool m_fcal;

  //unsigned short  m_dump_cut;
  std::string m_dump_gain;
  bool m_gains[CaloGain::LARNGAIN];
  bool m_dumpIter;
  //bool m_dumpDisc;


  // id helper
  const LArOnlineID* m_onlineHelper;
  const LArEM_ID* m_emId;
  const LArHEC_ID* m_hecId;
  const LArFCAL_ID* m_fcalId;
  
  // variables in ntuple 
  long m_cellIndex;
  //long m_evt;

  int* m_layer;
  int* m_eta;
  int* m_phi;
  int* m_gain;
  int* m_calo;
  int* m_barrel_ec;
  int* m_pos_neg;
  int* m_FT;
  int* m_slot;
  int* m_channel;
  // std::vector< short >* m_samples; 
  // std::vector< unsigned short >*    m_sca;

  std::vector< int >* m_samples; 
  std::vector< unsigned int >*    m_sca;

  unsigned int* m_offId;
  //For OF Iter results:
  bool*    m_Itervalid;      //actually bool
  bool*    m_Iterconverged;  //actually bool
  short*   m_IterNIter;     
  float*   m_IterAmpl;      
  float*   m_IterTau;
  short*   m_IterPeakSample; 
  float*   m_IterDelay;     
  float*   m_IterQual;       
  short*   m_IterIdx;      

};

}

#endif
