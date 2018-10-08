/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastHitConv.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASTCALOSIMHIT_FASTHITCONV_H
#define FASTCALOSIMHIT_FASTHITCONV_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "LArElecCalib/ILArfSampl.h"
#include "LArSimEvent/LArHitContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandle.h"
#include "TileSimEvent/TileHitVector.h"

#include "GaudiKernel/ServiceHandle.h"

class LArHitFloat;
class LArHitFloatContainer;
class LArHit;
class LArHitContainer;
class PileUpMergeSvc;
class TileInfo;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class TileID;

class FastHitConv: public AthAlgorithm {

public:

  // usual ATHENA constructor of an algorithm
  FastHitConv(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  virtual ~FastHitConv(){}

  StatusCode initialize() override final;
  StatusCode execute() override final;


private:

  StatusCode initEvent();
  StatusCode finaliseEvent();

  StatusCode hitConstruction();

  // For reading in of Fast Hits
  LArHitContainer* m_fastHitCont;
  TileHitVector* m_fastTileHits;
  //for writing out of Hit, naming as G4 for default
  SG::WriteHandle<LArHitContainer> m_embHitContainer;
  SG::WriteHandle<LArHitContainer> m_emecHitContainer;
  SG::WriteHandle<LArHitContainer> m_fcalHitContainer;
  SG::WriteHandle<LArHitContainer> m_hecHitContainer;
  SG::WriteHandle<TileHitVector>   m_tileHitVector;

  std::string m_caloCellsOutputName;

  ServiceHandle<StoreGateSvc> m_storeGateFastCalo;
  PileUpMergeSvc* m_pMergeSvc;

  // Data description objects

  const DataHandle<ILArfSampl>    m_dd_fSampl; // DataHandle for LAr sampling fraction
  const TileInfo* m_tileInfo;                  // Pointer to TileInfo class

  const LArEM_ID*     m_larEmID;
  const LArFCAL_ID*   m_larFcalID;
  const LArHEC_ID*    m_larHecID;
  const TileID*       m_tileID;

  bool m_pileup;                // pile up or not?

};
#endif    //FASTCALOSIMHIT_FASTHITCONV_H
