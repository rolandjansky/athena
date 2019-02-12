/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef TILERECUTILS_TILECELLNOISEFILTER_H
#define TILERECUTILS_TILECELLNOISEFILTER_H

// Tile includes
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseSample.h"

// Calo includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"


// forward declarations
class TileID;
class TileHWID;
class TileCell;
class CaloCellContainer;

/**
 @class TileCellNoiseFilter
 @brief This tool subtracts common-mode noise from all TileCells
 */
class TileCellNoiseFilter: public extends<AthAlgTool, ICaloCellMakerTool> {

  public:

    /** AlgTool like constructor */
    TileCellNoiseFilter(const std::string& type, const std::string& name,
        const IInterface* parent);

    /** Virtual destructor */
    virtual ~TileCellNoiseFilter() {};

    /** AlgTool InterfaceID */
    static const InterfaceID& interfaceID();

    /** AlgTool initialize method.*/
    StatusCode initialize() override;
    /** AlgTool finalize method */
    StatusCode finalize() override;

    /** proceed the coherent noise subtraction algorithm and correct Tile cell energies */
    virtual StatusCode process (CaloCellContainer* cellcoll,
                                const EventContext& ctx) const override;

  private:

    static const int s_maxPartition = 4; // LBA,LBC,EBA,EBC
    static const int s_maxDrawer = 64;   // # of drawers per partition
    static const int s_maxMOB = 4;       // # of motherboards per drawer
    static const int s_maxChannel = 12;  // # of channels per motherboard

    typedef float cmdata_t[s_maxPartition][s_maxDrawer][s_maxMOB];

    // set common-mode subtructed energy
    void setCMSEnergy(const cmdata_t& commonMode, TileCell *cell) const;

    // calculate common-mode for all the motherboards
    int calcCM(const CaloCellContainer *cellcoll, cmdata_t& commonMode) const;

    // derive a value of common-mode shift
    float getCMShift(const cmdata_t& commonMode,
                     int partition, int drawer, int channel) const
    {
      return commonMode[partition][drawer][channel / s_maxChannel];
    }

    const TileID* m_tileID;   //!< Pointer to TileID
    const TileHWID* m_tileHWID; //!< Pointer to TileHWID

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale{this,
        "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale calibration tool"};

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile noise sample tool"};

    ToolHandle<ICalorimeterNoiseTool> m_noiseTool;       //!< Calo Noise tool

    ToolHandle<ITileBadChanTool> m_tileBadChanTool{this,
        "TileBadChanTool", "TileBadChanTool", "Tile bad channel tool"};

    // properties
    float m_truncationThresholdOnAbsEinSigma;
    float m_minimumNumberOfTruncatedChannels;
    bool m_useTwoGaussNoise;
    bool m_useTileNoiseDB;

    static const CaloCell_ID::SUBCALO s_caloIndex = CaloCell_ID::TILE;

    float m_maxNoiseSigma;

};

#endif // TILERECUTILS_TILECELLNOISEFILTER_H
