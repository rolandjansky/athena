/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECALGS_TILEINFOLOADER_H
#define TILERECALGS_TILEINFOLOADER_H
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
#include "GaudiKernel/IIncidentListener.h"

// Athena includes
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"

#include <string>

class TilePulseShapes;
class TileOptFilterWeights;
class StoreGateSvc;
class TileCablingSvc;
template<class TYPE> class SvcFactory;
class TileInfo;

static const InterfaceID IID_TileInfoLoader("TileInfoLoader", 1, 0);

class TileInfoLoader: public AthService, virtual public IIncidentListener {

    friend class SvcFactory<TileInfoLoader> ;

  public:

    TileInfoLoader(const std::string& name, ISvcLocator* pSvcLocator);
    ~TileInfoLoader();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    static const InterfaceID& interfaceID() { return IID_TileInfoLoader; }

    //** Loading stuff from files */
    StatusCode buildDigitsShapesHiLo();
    StatusCode buildTTL1Shapes(std::string ShapeFile, int &NBins, int &Time0Bin, int &BinsPerX
                               , std::vector<double> &FullShape, std::vector<double> &Shape);
    void buildCovMatrix();

    virtual void handle(const Incident&);   //!< Callback for Incident service

  private:

    //=== services
    ServiceHandle<StoreGateSvc> m_detStore;

    /** Pointer to instance in detector store */
    TileInfo* m_info;

    /** Pointer to TilePulseShapes object*/
    TilePulseShapes* m_pulsevar;

    /** Pointer to Optimal Filtering weights*/
    TileOptFilterWeights* m_OFWeights;

    /** Filenames of input files */
    std::string m_digitsShapeFileHi;
    std::string m_digitsShapeFileLo;
    std::string m_TTL1ShapeFile;
    std::string m_MuRcvShapeFile;
    std::string m_MuL1ShapeFile;
    std::string m_DecoCovaFilePrefix;

    bool m_loadOptFilterWeights;
    bool m_loadOptFilterCorrelation;
    bool m_eorCalled;

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

#endif  // TileInfoLoader.h
