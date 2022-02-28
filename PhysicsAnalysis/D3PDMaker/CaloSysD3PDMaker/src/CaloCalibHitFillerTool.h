/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSYSD3PDMAKER_CALOCALIBHITFILLERTOOL_H
#define CALOSYSD3PDMAKER_CALOCALIBHITFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloDmDescrManager;
class Identifier;
class LArEM_ID; 
class LArHEC_ID; 
class LArFCAL_ID; 
class TileID; 
class LArOnlineID;
class CaloCell_ID;
class CaloDM_ID;


namespace D3PD {

/** @brief define a special getter 
 */

class CaloCalibrationHitContainerGetterTool
  : public D3PD::SGCollectionGetterTool<CaloCalibrationHitContainer>
{
public:
  CaloCalibrationHitContainerGetterTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent)
    : SGCollectionGetterTool<CaloCalibrationHitContainer>
         (type, name, parent) {}
};





/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class CaloCalibHitFillerTool
  : public BlockFillerTool<CaloCalibrationHit>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CaloCalibHitFillerTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize() override;


  /// Book variables for this block.
  virtual StatusCode book() override;

  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const CaloCalibrationHit& p) override;

private:

  /// Variable: time, quality and id
  float* m_eta;
  float* m_phi;
  float* m_etot;
  float* m_evis;
  float* m_e_em;
  float* m_e_nonem;
  float* m_e_inv;
  float* m_e_esc;
  unsigned int* m_particleID;
  unsigned int*   m_id ;
  unsigned int*   m_det ;
  unsigned int*   m_sam ;
  unsigned int*   m_side ;
  unsigned int*   m_reg ;
  unsigned int*   m_etatow ;
  unsigned int*   m_phimod ;

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

  // idHelper 
  const CaloDmDescrManager* m_dm_ddm;
  const LArEM_ID* m_emid;
  const LArFCAL_ID* m_fcalid;
  const LArHEC_ID* m_hecid;
  const TileID* m_tileid;
  const LArOnlineID* m_onlineid;
  const CaloCell_ID* m_cellid;
  const CaloDM_ID* m_dmid;
  unsigned int CaloHit_GetDetectorInfo(Identifier &HitID);

  // properties
  bool m_isDM;
  bool m_isbasic;
  bool m_savepartID;
  bool m_savefullID;
  bool m_savetoten;
  bool m_savevisen;
  bool m_saveallen;
};


} // namespace D3PD


#endif // not CALOSYSD3PDMAKER_CALOCALIBHITFILLERTOOL_H
