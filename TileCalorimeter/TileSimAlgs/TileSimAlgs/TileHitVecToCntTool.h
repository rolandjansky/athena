/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
// Filename : TileHitVecToCntTool.h
// Author   : Vishnu Zutshi
// Created  : Dec. 2009
//
// DESCRIPTION
// 
//    TileHitVecToCnt copies all TileHits from TileHitVector (AthenaHitsVector)
//    to TileHitContainer (identifiable container) without any corrections.
//    If pileup option is set, all hits are merged. The pileup is done bunchXing
//    by bunchXing (main difference from TileHitVecToCnt). If several hits have
//    the same ID, energy and time from all of them is stored in vectors
//    inside single TileHit. 
//
// Properties (JobOption Parameters):
//
//    TileHitVectors            string   Name of TileHitVector to read
//    TileHitContainer          string   Name of TileHitContainer to write
//    TileInfoName              string   Name of object in TDS with all parameters
//    PileUp                    bool     Do or not pileup
//    HitTimeFlag               int      Working with hit times
//**************************************************************************

#ifndef TILESIMALGS_TILEHITVECTOCNTTOOL_H
#define TILESIMALGS_TILEHITVECTOCNTTOOL_H

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "PileUpTools/PileUpToolBase.h"

// Tile includes
#include "TileIdentifier/TileFragHash.h"
#include "TileSimEvent/TileHitVector.h"


// Avoid pushing dependencies into clients- just fwd declare the following:

class TileID;
class TileTBID;
class TileHit;
class TileInfo;
class TileHitContainer;
class TileDetDescrManager;
class PileUpMergeSvc;
class ITriggerTime;
class IAtRndmGenSvc;
class Identifier;
class TileCablingSvc;
class TileCablingService;
class TileHitCollection;


namespace CLHEP {
  class HepRandomEngine;
}

// C++ STL includes
#include <string>
#include <vector>

/** 
 @class TileHitVecToCntTool
 @brief This AlgTool merges TileHits from different AthenaHitVectors and stores the result in a TileHitContainer

 TileHitVecToCntTool copies all TileHits from TileHitVector (AthenaHitsVector)
 to TileHitContainer (identifiable container) without any corrections
 If pileup option is set, all hits are merged, if several hits have
 the same ID, energy and time from all of them is stored in vectors
 inside single TileHit
 */

class TileHitVecToCntTool: public PileUpToolBase {

  public:
    TileHitVecToCntTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
    StatusCode initialize();
    virtual StatusCode prepareEvent(unsigned int /*nInputEvents*/);
    virtual StatusCode mergeEvent();
    virtual StatusCode processBunchXing(int bunchXing
                                        , PileUpEventInfo::SubEvent::const_iterator bSubEvents
                                        , PileUpEventInfo::SubEvent::const_iterator eSubEvents);
    virtual StatusCode processAllSubEvents();
    StatusCode finalize();

  private:
    StatusCode createContainers();
    void processHitVectorForOverlay(const TileHitVector* inputHits, int& nHit, double& eHitTot);
    void processHitVectorForPileUp(const TileHitVector* inputHits, double SubEvtTimOffset, int& nHit, double& eHitTot);
    void processHitVectorWithoutPileUp(const TileHitVector* inputHits, int& nHit, double& eHitTot);
    double ApplyPhotoStat(double energy, Identifier pmt_id);    //!< Method to apply photostatistics effect
    void findAndMergeE1(const TileHitCollection* const_coll, int frag_id);
    void findAndMergeMBTS(const TileHitCollection* const_coll, int frag_id);

    std::vector<std::string> m_hitVectorNames;  //!< vector with the names of TileHitVectors to use
    std::string m_hitContainer;                 //!< name of the TileHitCointainer
    std::string m_infoName;                     //!< name of TileInfo object in TES
    bool m_pileUp;                              //!< if true => pileup mode is activated
    double m_deltaT;                            //!< minimal time granularity for TileHit
    int m_timeFlag;                             //!< specail options to deal with times of hits for cosmics and TB
    double m_triggerTime;                       //!< fixed trigger time value (default=0)
    double m_maxHitTime;                        //!< all sub-hits with time above m_maxHitTime will be ignored
    double m_photoStatWindow;                   //!< sum up energy in [-m_photoStatWindow,+m_photoStatWindow] and use it for photostatistics
    int m_PhElStat;                             //!< photoelectron statistics type: 0 - Poisson, 1 - "new" Poisson + Gauss, 2 - Poisson->Gauss
    bool m_skipNoHit;                           //!< if true => skip events with no Tile hits 
    bool m_rndmEvtOverlay;                      //!< If true => overlay with random event (zero-luminosity pile-up)
    bool m_useTriggerTime;                      //!< if true => take trigger time from external tool or from m_triggerTime
    ToolHandle<ITriggerTime> m_triggerTimeTool; //!< tool to take the time from

    PileUpMergeSvc* m_mergeSvc;                 //!< Pointer to PileUpMergeService

    const TileID* m_tileID;                     //!< Pointer to TileID helper
    const TileTBID* m_tileTBID;                 //!< Pointer to TileID helper
    const TileInfo* m_tileInfo;                 //!< Pointer to TileInfo
    const TileDetDescrManager* m_tileMgr;       //!< Pointer to TileDetDescrManager
    float numPhElec[6];                         //!< number of photo electrons per GeV in samplings

    CLHEP::HepRandomEngine* m_pHRengine;       //!< Random number service to use
    ServiceHandle<IAtRndmGenSvc> m_rndmSvc;     //!< Random number generator engine to use

    std::vector<TileHit*> m_allHits;           //!< vector for all TileHits
    TileHitContainer* m_hits;                   //!< pointer to hits container

    bool do_checks;                             //!< initial value of do_checks flag in TileID helper
    bool do_checks_tb;                          //!< initial value of do_checks flag in TileTBID helper

    int m_mbtsOffset;                           //<! index of first MBTS hit in m_allHits vector
    static const int nSide = 2;
    static const int nPhi = 8;
    static const int nEta = 2;
    static const int nCellMBTS = nSide * nPhi * nEta;
    inline int mbts_index(int side, int phi, int eta) const {
      return (side * nPhi + phi) * nEta + eta + m_mbtsOffset;
    }

    ServiceHandle<TileCablingSvc> m_cablingSvc;
    const TileCablingService* m_cabling; 
    TileFragHash m_fragHashFunc;
    std::vector<bool> m_E1merged;
    std::vector<bool> m_MBTSmerged;
    enum CELL_TOWER{E1_TOWER = 10};
};

#endif // TILESIMALGS_TILEHITVECTOCNTTOOL_H
