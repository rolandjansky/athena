// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOSYSD3PDMAKER_LARHITFILLERTOOL_H
#define CALOSYSD3PDMAKER_LARHITFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "LArSimEvent/LArHitContainer.h"

class CaloDetDescrManager ; 
class Identifier;
class LArEM_ID; 
class LArHEC_ID; 
class LArFCAL_ID; 
class TileID; 
class LArOnlineID;

#include "LArSimEvent/LArHit.h"


namespace D3PD {

/** @brief define a special getter 
 */

class LArHitContainerGetterTool
  : public D3PD::SGCollectionGetterTool<LArHitContainer>
{
public:
  LArHitContainerGetterTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent)
    : SGCollectionGetterTool<LArHitContainer>
         (type, name, parent) {}
};





/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class LArHitFillerTool
  : public BlockFillerTool<LArHit>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  LArHitFillerTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


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
  virtual StatusCode fill (const LArHit& p);

private:

  /// Variable: time, quality and id
  float* m_eta;
  float* m_phi;
  float* m_e;
  float* m_time;
  unsigned int*   m_id ;

  // idHelper 
  const LArEM_ID* m_emid;
  const LArFCAL_ID* m_fcalid;
  const LArHEC_ID* m_hecid;
  const TileID* m_tileid;
  const LArOnlineID* m_onlineid;
  unsigned int CaloCell_GetDetectorInfo(Identifier &cellID);
  const CaloDetDescrManager*  m_dd_man; 
};


} // namespace D3PD


#endif // not CALOSYSD3PDMAKER_LARHITFILLERTOOL_H
