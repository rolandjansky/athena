/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEINFOLOADER_H
#define TILECONDITIONS_TILEINFOLOADER_H
//****************************************************************************
// Filename : TileInfoLoader.h
// Author   : Ed Frank
// Created  : May 2002
//
// DESCRIPTION
// 
//    TileConditions/TileInfo.h is the Tile interface to the transient
// detector store.  Rather than having every item accessed separately from
// the TDS, people will grab a TileInfo from the TDS and make calls on it.
// Those calls will defer back to the TDS so that time validity is done
// correctly.  We think this may be simpler than getting everything
// separately.  We expect, in the future, to break the TileInfo into pieces,
// but for now, it's all in one.
//
// Until a conditions database exists, we just make a TileInfo from parameters
// in a fplain file.  TileInfoLoader exists just to produce a tileInfo at
// the start of the job until the conditions DB comes along.
//
// Properties (JobOption Parameters):
//
//
// History:
//   07Feb01  efrank  Creation, based upon LArTowerBuilder.h
//  
// BUGS:
//  
//****************************************************************************

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"

#include <string>

class TilePulseShapes;
class TileWienerFilterWeights;
class StoreGateSvc;
class TileCablingSvc;
template<class TYPE> class SvcFactory;
class TileInfo;

static const InterfaceID IID_TileInfoLoader("TileInfoLoader", 1, 0);

class TileInfoLoader: public AthService {

    friend class SvcFactory<TileInfoLoader> ;

  public:

    TileInfoLoader(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TileInfoLoader();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    static const InterfaceID& interfaceID() { return IID_TileInfoLoader; }

private:
    //** Loading stuff from files */
    StatusCode buildDigitsShapesHiLo (TileInfo& info);
    StatusCode buildTTL1Shapes(TileInfo& info,
                               const std::string& ShapeFile,
                               int &NBins, int &Time0Bin, int &BinsPerX
                               , std::vector<double> &FullShape, std::vector<double> &Shape);
    void buildCovMatrix (TileInfo& info);



    //=== services
    ServiceHandle<StoreGateSvc> m_detStore;

    /** Pointer to TilePulseShapes object*/
    TilePulseShapes* m_pulsevar;

    /** Properties to forward to TileInfo. */
    int    m_nSamples;
    int    m_iTrigSample;
    bool   m_tileNoise;
    bool   m_tileCoherNoise;
    bool   m_tileZeroSuppress;
    int m_noiseScaleIndex;
    double m_thresholdRawChannel;
    double m_thresholdDigits;
    double m_ttL1Calib;
    double m_ttL1NoiseSigma;
    double m_ttL1Thresh;
    double m_ttL1Ped;
    double m_ttL1Max;
    double m_MuRcvCalib;
    double m_MuRcvNoiseSigma;
    double m_MuRcvThresh;
    double m_MuRcvPed;
    double m_MuRcvMax;
    double m_muL1Calib;
    double m_muL1NoiseSigma;
    double m_muL1Thresh;
    double m_muL1Ped;
    double m_muL1Max;
    double m_mbtsL1Calib;
    double m_mbtsL1NoiseSigma;
    double m_mbtsL1Thresh;
    double m_mbtsL1Ped;
    double m_mbtsL1Max;
    double m_ttL1CosmicsGain;
    double m_ttL1CosmicsThresh;
    int    m_ADCmax;
    int    m_ADCmaskValue;
    


    /** Pointer to Wiener Filtering weights*/
    TileWienerFilterWeights* m_WFWeights;

    /** Filenames of input files */
    std::string m_digitsShapeFileHi;
    std::string m_digitsShapeFileLo;
    std::string m_TTL1ShapeFile;
    std::string m_MuRcvShapeFile;
    std::string m_MuL1ShapeFile;
    std::string m_DecoCovaFilePrefix;

    bool m_loadWienerFilterWeights;

    // name of TileCalibContainer in detector store
    std::string m_calibCntName;

    std::vector<std::string> m_connectedDrawers; // list of connected drawers
    // we assume that list contains even number of elements - pairs of (begin,end) range,
    // e.g. all drawers connected looks like this:
    // [ "0x100", "0x13F", "0x200", "0x23F", "0x300", "0x33F", "0x400", "0x43F" ]
    // and this is the list for few drawers in LBA connected:
    // [ "0x10A", "0x10C", "0x121", "0x123" ]
    // empty list is equivalent to list with all drawers connected

    TileInfoLoader(const TileInfoLoader&);
    TileInfoLoader& operator=(const TileInfoLoader&);
};

#endif  // TILECONDITIONS_TILEINFOLOADER_H
