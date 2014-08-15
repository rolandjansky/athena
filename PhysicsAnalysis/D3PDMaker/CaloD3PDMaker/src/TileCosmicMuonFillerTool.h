/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCosmicMuonFillerTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on November 2, 2011, 1:26 PM
 */

#ifndef TILECOSMICMUONFILLERTOOL_H
#define	TILECOSMICMUONFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileEvent/TileCosmicMuonContainer.h"
#include "TileEvent/TileCosmicMuon.h"

namespace D3PD{

class TileCosmicMuonFillerTool : public BlockFillerTool<TileCosmicMuon> {
public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TileCosmicMuonFillerTool(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    
    virtual ~TileCosmicMuonFillerTool();
    
    //Book variables for this block.
    virtual StatusCode book();

    /**
     * @brief Fill one block --- type-safe version.
     * @param p The input object.
     * 
     * This is called once per object.  The caller
     * is responsible for arranging that all the pointers for booked variables
     * are set appropriately upon entry.
     */
    virtual StatusCode fill (const TileCosmicMuon& p);
    
private:

    std::string m_cellContainerKey;  //!< name of the CellContainer object in StoreGate
    std::string m_tileCosmicMuonKey;  //!< name of the TileCosmicMuon object in StoreGate
    std::string m_prefix;  //!< Prefix of the TileMuonFitter variables in the ntuple
    std::string m_beamType;  //!< Prefix of the TileMuonFitter variables in the ntuple

//////    int m_nTracks;
//////    int m_trackOfMaxE;

    float* m_positionX;
    float* m_positionY;
    float* m_positionZ;
    float* m_directionPhi;
    float* m_directionTheta;
    float* m_time;
    float* m_fitQuality;
    int* m_fitNCells;
    int* m_trackNCells;
    float* m_fullPath;
    float* m_fullEnergy; 
    float* m_pathTopA;
    float* m_pathTopBC;
    float* m_pathTopD;
    float* m_pathBottomA;
    float* m_pathBottomBC;
    float* m_pathBottomD;
    float* m_energyTopA;
    float* m_energyTopBC;
    float* m_energyTopD;
    float* m_energyBottomA;
    float* m_energyBottomBC;
    float* m_energyBottomD;

//    int m_nSegments;
    std::vector<float>* m_segmentPath;
    std::vector<int>* m_segmentPartition;
    std::vector<int>* m_segmentModule;
    std::vector<int>* m_segmentSampling;
//    std::vector<int>* m_segmentTrack;

};

}


#endif	/* TILECOSMICMUONFILLERTOOL_H */

