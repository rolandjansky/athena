// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/src/TileDQstatusTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Construct a TileDQstatus object.
 */


#ifndef TILERECUTILS_TILEDQSTATUSTOOL_H
#define TILERECUTILS_TILEDQSTATUSTOOL_H


#include "TileRecUtils/ITileDQstatusTool.h"
#include "TileConditions/ITileBadChanTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ServiceHandle.h"
class TileHWID;


class TileDQstatusTool
: public extends<AthAlgTool, ITileDQstatusTool>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TileDQstatusTool (const std::string& type,
                    const std::string& name,
                    const IInterface* parent);


  /**
   * @brief Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Make a new TileDQstatus object.
   * @param ctx Event context.
   * @param rawChannelContainer The TileRawChannelContainer (may be null).
   * @param tileDigitsContainer The TileDigitsContainer (may be null).
   * @param tileBeamElemContainer The TileBeamElemContainer (may be null).
   * @param[out] dqstatus TileDQstatus object to fill.
   */
  virtual
  StatusCode
  makeStatus (const EventContext& ctx,
              const TileRawChannelContainer* rawChannelContainer,
              const TileDigitsContainer* tileDigitsContainer,
              const TileBeamElemContainer* tileBeamElemContainer,
              TileDQstatus& dqstatus) const override;

private:
  /**
   * @brief Process BeamElemContainer.
   * @param tileBeamElemContainer The TileBeamElemContainer (may be null).
   * @param dqstatus TileDQstatus object being filled.
   */
  StatusCode doBeamElem (const TileBeamElemContainer* tileBeamElemContainer,
                         TileDQstatus& dqstatus) const;


  Gaudi::Property<bool> m_simulateTrips
  {this, "SimulateTrips", false, "Simulate drawer trips (default=false)"};

  ServiceHandle<IAthRNGSvc> m_athRNGSvc
  {this, "AthRNGSvc", "AthRNGSvc", "Random number service"};

  /// Tool which provides trips probabilities also
  ToolHandle<ITileBadChanTool> m_tileBadChanTool;


  /// Tile ID helper.
  const TileHWID* m_tileHWID = nullptr;
};


#endif // not TILERECUTILS_TILEDQSTATUSTOOL_H
