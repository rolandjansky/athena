/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECALGS_TILECELLCORRECTION_H
#define TILERECALGS_TILECELLCORRECTION_H

/********************************************************************
 *
 * NAME:     TileCellCorrection
 * PACKAGE:  offline/TileCalorimeter/TileRecAlgs
 *
 * AUTHOR :  A. Solodkov
 * CREATED:  July 15, 2009
 *
 * PURPOSE:  Update cell time or cell energy for all cells 
 *           which are read from ESD 
 *
 *  Input: CaloCellContainer
 *  Output: The same container with updated TileCell
 *  JobOption Parameters:
 *    CaloCellContainer - Name of input container
 *    CorrectTime - correct or not cell time using new timing constants from DB
 *    CorrectEnergy - correct or not cell energy new calib constants from DB
 *    MaskBadChannles - apply or not bad channel masking 
 *    PartitionScaleFactor - 4 numbers, scale factors for LBA,LBC,EBA,EBC
 *                           they are applied only if they are not zeros 
 *                           and CorrectEnergy is set to false 
 *   
 ********************************************************************/

#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/StoreGateSvc.h"

// Atlas includes
#include "AthenaBaseComps/AthService.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICaloCellMakerTool.h"

template<class TYPE> class SvcFactory;
class TileHWID;
class TileID;
class TileCondToolEmscale;
class TileCondToolTiming;
class TileCellNoiseFilter;
class ITileBadChanTool;
class TileCellMaskingTool;
class CaloCellContainer;

#include <string>
#include <vector>

static const InterfaceID IID_TileCellCorrection("TileCellCorrection", 1, 0);

/**
 @class TileCellCorrection
 @brief This class correct time and/or energy of all TileCells
 */
class TileCellCorrection: public AthService, virtual public IIncidentListener {

    friend class SvcFactory<TileCellCorrection> ;

  public:
    TileCellCorrection(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~TileCellCorrection(); //!< Destructor

    virtual StatusCode initialize();  //!< intialize method
    virtual StatusCode finalize();    //!< finalize method

    virtual void handle(const Incident&);   //!< Callback for Incident service

    static const InterfaceID& interfaceID() {
      return IID_TileCellCorrection;
    }

  private:

    std::string m_caloCellContainerID;    //!< Name of the CaloCellContainer

    bool m_correctTime;    //!< if true - correct time
    bool m_correctEnergy; //!< if true - correct energy using from "online" to "offline"
    bool m_maskBadChannels;  //!< if true - mask bad channels
    bool m_doTileCellNoiseFilter; //!< if true - apply noise filter to all the cells
    bool m_doTileCellMasking; //!< if true - call maksing tool to mask some cells
    bool m_doTileCellNeighbors; //!< if true - call CaloCellNeighborsAverageCorr to recover bad cells

    std::vector<float> m_partScaleFactors; //!< additional scale factors for LBA,LBC,EBA,EBC
    float m_zeroEnergy; //!< energy to store in every PMT if both PMTs are bad

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
    ToolHandle<TileCondToolTiming> m_tileToolTiming; //!< Tile Timing Calibration tool
    ToolHandle<TileCellNoiseFilter> m_tileCellNoiseFilter; //!< Tile Noise Filter tool
    ToolHandle<ICaloCellMakerTool> m_tileCellMaskingTool; //!< Tile Cell Masking tool
    ToolHandle<ICaloCellMakerTool> m_tileCellNeighborsTool; //!< CaloCellNeighborsAverageCorr

    const TileID* m_tileID;
    const TileHWID* m_tileHWID;

    static CaloCell_ID::SUBCALO m_caloIndex;

    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (event store by default)
    mutable StoreGateSvc_t m_evtStore;

    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;

    double totalEnergy(const CaloCellContainer * caloCellContainer);

};

#endif
