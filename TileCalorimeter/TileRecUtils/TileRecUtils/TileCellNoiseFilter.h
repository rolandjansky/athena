/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef TILECELLNOISEFILTER_H
#define TILECELLNOISEFILTER_H

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Calo includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

// Tile includes
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseSample.h"

// forward declarations
class TileID;
class TileHWID;
class TileCell;
class CaloCellContainer;

/**
 @class TileCellNoiseFilter
 @brief This tool subtracts common-mode noise from all TileCells
 */
class TileCellNoiseFilter: public AthAlgTool, virtual public ICaloCellMakerTool {
  public:

    /** AlgTool like constructor */
    TileCellNoiseFilter(const std::string& type, const std::string& name,
        const IInterface* parent);

    /** Virtual destructor */
    virtual ~TileCellNoiseFilter() {
    }
    ;

    /** AlgTool InterfaceID */
    static const InterfaceID& interfaceID();

    /** AlgTool initialize method.*/
    StatusCode initialize();
    /** AlgTool finalize method */
    StatusCode finalize();

    /** proceed the coherent noise subtruction algorithm and correct Tile cell energies */
    virtual StatusCode process(CaloCellContainer *cellcoll);

    /** Callback to handle Data-driven GeoModel initialisation */
    virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

  private:

    // set common-mode subtructed energy
    void setCMSEnergy(TileCell *cell);

    // calculate common-mode for all the motherboards
    int calcCM(const CaloCellContainer *cellcoll);

    // derive a value of common-mode shift
    float getCMShift(int partition, int drawer, int channel) {
      return m_commonmode[partition][drawer][channel / maxChannel];
    }

    const TileID* m_tileID;   //!< Pointer to TileID
    const TileHWID* m_tileHWID; //!< Pointer to TileHWID

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample; //!< tool which provided noise values
    ToolHandle<ICalorimeterNoiseTool> m_noiseTool;       //!< Calo Noise tool

    // properties
    float m_truncationThresholdOnAbsEinSigma;
    float m_minimumNumberOfTruncatedChannels;
    bool m_useTwoGaussNoise;
    bool m_useTileNoiseDB;

    static const CaloCell_ID::SUBCALO m_caloIndex = CaloCell_ID::TILE;

    static const int maxPartition = 4; // LBA,LBC,EBA,EBC
    static const int maxDrawer = 64;   // # of drawers per partition
    static const int maxMOB = 4;       // # of motherboards per drawer
    static const int maxChannel = 12;  // # of channels per motherboard

    float m_commonmode[maxPartition][maxDrawer][maxMOB];
    int m_nemptychan[maxPartition][maxDrawer][maxMOB];
    int m_ngoodchan[maxPartition][maxDrawer][maxMOB];
};

#endif // TILECELLNOISEFILTER_H
