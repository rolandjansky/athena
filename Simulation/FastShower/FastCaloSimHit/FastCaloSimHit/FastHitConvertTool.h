/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastHitConvertTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASTCALOSIMHIT_FASTHITCONVERTTOOL_H
#define FASTCALOSIMHIT_FASTHITCONVERTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "LArElecCalib/ILArfSampl.h"
#include "LArSimEvent/LArHitContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TileSimEvent/TileHitVector.h"
#include "TileConditions/TileSamplingFraction.h"
#include "TileConditions/TileCablingSvc.h"
#include "EventInfo/PileUpEventInfo.h"

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

/** @class FastHitConvertTool

    This is for the Doxygen-Documentation.
    Please delete these lines and fill in information about
    the Algorithm!
    Please precede every member function declaration with a
    short Doxygen comment stating the purpose of this function.

    @author  Bo Liu <boliu@cern.ch>
*/

class FastHitConvertTool : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:
  FastHitConvertTool(const std::string& type,const std::string& name,const IInterface* parent);

  /** default destructor */
  virtual ~FastHitConvertTool(){};

  /** standard Athena-Algorithm method */
  virtual StatusCode initialize() override final;
  /** standard Athena-Algorithm method */
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

private:

  //for writing out of Hit, naming as G4 for default
  SG::WriteHandleKey<LArHitContainer> m_embHitContainerKey{this,"embHitContainername","LArHitEMB","Name of output FastSim LAr EM Barrel Hit Container"};
  SG::WriteHandleKey<LArHitContainer> m_emecHitContainerKey{this,"emecHitContainername","LArHitEMEC","Name of output FastSim LAr EM Endcap Hit Container"};
  SG::WriteHandleKey<LArHitContainer> m_fcalHitContainerKey{this,"fcalHitContainername","LArHitFCAL","Name of output FastSim LAr FCAL Hit Container"};
  SG::WriteHandleKey<LArHitContainer> m_hecHitContainerKey{this,"hecHitContainername","LArHitHEC","Name of output FastSim LAr HEC Hit Container"};
  SG::WriteHandleKey<TileHitVector>   m_tileHitVectorKey{this,"tileHitContainername","TileHitVec","Name of output FastSim Tile Hit Container"};

  ServiceHandle<StoreGateSvc> m_storeGateFastCalo;
  PileUpMergeSvc *m_pMergeSvc{nullptr};

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

  const LArEM_ID *m_larEmID{nullptr};
  const LArFCAL_ID *m_larFcalID{nullptr};
  const LArHEC_ID *m_larHecID{nullptr};
  const TileID* m_tileID{nullptr};
  const TileHWID*     m_tileHWID{nullptr};
  const TileCablingService* m_tileCabling{nullptr};

  Gaudi::Property<bool> m_pileup{this,"doPileup",false,"Pileup mode (default=false)"};
  SG::ReadHandleKey<EventInfo> m_pileup_evt{this,"pileupEventInfo","MyEvent",""};
  SG::ReadHandleKey<PileUpEventInfo> m_pileup_pOverEvent{this,"pileupOverlayEvent","OverlayEvent",""};
};
#endif          //FASTCALOSIMHIT_FASTHITCONVERTTOOL_H
