/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author : Matthew Beckingham 
// date of creation : July 2, 2008


#ifndef FASTHITCONV_H
#define FASTHITCONV_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "StoreGate/StoreGateSvc.h"

#include "TileSimEvent/TileHitVector.h"

#include "LArElecCalib/ILArfSampl.h"

class IMessageSvc;
class StoreGateSvc;

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

class FastHitConv: public Algorithm {

public:

  // usual ATHENA constructor of an algorithm
    FastHitConv(const std::string & name, ISvcLocator * pSvcLocator);

    // Destructor
    virtual ~FastHitConv(){}

    StatusCode initialize();
    StatusCode execute();


private:

    StatusCode initEvent();
    StatusCode finaliseEvent();

    StatusCode hitConstruction();

    // For reading in of Fast Hits
    LArHitContainer* fastHitCont;
    TileHitVector* fastTileHits;

    // For writing out of "G4" hits
    LArHitContainer* embHitCont;
    LArHitContainer* emecHitCont;
    LArHitContainer* fcalHitCont;
    LArHitContainer* hecHitCont;

    TileHitVector* m_tileHits;


    std::string m_caloCellsOutputName;
    
    std::string m_embHitContName;
    std::string m_emecHitContName;
    std::string m_fcalHitContName;
    std::string m_hecHitContName;
    std::string m_tileHitContName;
    

    AthenaHitsCollectionHelper helper;

    IMessageSvc* m_msgSvc;

    StoreGateSvc* m_storeGate;
    StoreGateSvc* m_detStore;

    ServiceHandle<StoreGateSvc> m_storeGateFastCalo;

    PileUpMergeSvc* m_pMergeSvc;

    // Data description objects

    const DataHandle<ILArfSampl>    m_dd_fSampl; // DataHandle for LAr sampling fraction
    const TileInfo* m_tileInfo;                  // Pointer to TileInfo class

    const LArEM_ID*     m_larEmID;
    const LArFCAL_ID*   m_larFcalID;
    const LArHEC_ID*    m_larHecID;
    const TileID*       m_tileID;

    bool m_pileup;                // pile up or not?

};
#endif    //FASTHITCONV_H


