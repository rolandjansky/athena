/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/ReadCondHandle.h"
#include "TileSimEvent/TileHitVector.h"
#include "TileConditions/TileSamplingFraction.h"
#include "TileConditions/TileCablingSvc.h"

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
class TileHWID;

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

  //for writing out of Hit, naming as G4 for default
  SG::WriteHandle<LArHitContainer> m_embHitContainer;
  SG::WriteHandle<LArHitContainer> m_emecHitContainer;
  SG::WriteHandle<LArHitContainer> m_fcalHitContainer;
  SG::WriteHandle<LArHitContainer> m_hecHitContainer;
  SG::WriteHandle<TileHitVector>   m_tileHitVector;

  std::string m_caloCellsOutputName;

  ServiceHandle<StoreGateSvc> m_storeGateFastCalo;
  PileUpMergeSvc* m_pMergeSvc{nullptr};

  // Data description objects
  SG::ReadCondHandleKey<ILArfSampl> m_fSamplKey{this,"fSamplKey","LArfSamplSym","SG Key of LArfSampl object"};

  /**
   * @brief Name of TileSamplingFraction in condition store
   */
  SG::ReadCondHandleKey<TileSamplingFraction> m_tileSamplingFractionKey{this,
      "TileSamplingFraction", "TileSamplingFraction", "Input Tile sampling fraction"};

  /**
   * @brief Name of Tile cabling service
   */
  ServiceHandle<TileCablingSvc> m_tileCablingSvc{ this,
     "TileCablingSvc", "TileCablingSvc", "Tile cabling service"};

  const LArEM_ID*     m_larEmID{nullptr};
  const LArFCAL_ID*   m_larFcalID{nullptr};
  const LArHEC_ID*    m_larHecID{nullptr};
  const TileID*       m_tileID{nullptr};
  const TileHWID*     m_tileHWID{nullptr};
  const TileCablingService* m_tileCabling{nullptr};

  bool m_pileup{false};                // pile up or not?

};
#endif    //FASTCALOSIMHIT_FASTHITCONV_H
