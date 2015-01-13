/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastHitConvertTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASTCALOSIMHIT_FASTHITCONVERTTOOL_H
#define FASTCALOSIMHIT_FASTHITCONVERTTOOL_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "TileSimEvent/TileHitVector.h"
#include "LArElecCalib/ILArfSampl.h"

class IMassageSvc;
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
//class CaloCellContainer;
class AtlasDetectorID;
class Identifier;

// No NameSpace selected 
//{

/** @class FastHitConvertTool 

  This is for the Doxygen-Documentation.  
  Please delete these lines and fill in information about
  the Algorithm!
  Please precede every member function declaration with a
  short Doxygen comment stating the purpose of this function.

  @author  Bo Liu <boliu@cern.ch>
  */  

class FastHitConvertTool : public AthAlgTool,virtual public ICaloCellMakerTool
{
  public:
    FastHitConvertTool(const std::string& type,const std::string& name,const IInterface* parent);

    /** default destructor */
    virtual ~FastHitConvertTool(){};

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode process(CaloCellContainer *theCellContainer);
    virtual StatusCode finalize();

  private:
    StatusCode initEvent();
    StatusCode finaliseEvent();
    StatusCode hitConstruction(CaloCellContainer *theCellCont);

    //For reading in of fast hit
    LArHitContainer *fastHitContainer;
    TileHitVector *fastTileHits;
    //for writing out of Hit, naming as G4 for default
    LArHitContainer *embHitContainer;
    LArHitContainer *emecHitContainer;
    LArHitContainer *fcalHitContainer;
    LArHitContainer *hecHitContainer;

    TileHitVector* m_tileHits;

    std::string m_caloCellsOutputName;
    //Name for Hit container
    std::string m_embHitContainerName;
    std::string m_emecHitContainerName;
    std::string m_fcalHitContainerName;
    std::string m_hecHitContainerName;
    std::string m_tileHitContainerName;

    AthenaHitsCollectionHelper helper;

    StoreGateSvc *m_storeGate;
    StoreGateSvc *m_detStore;
    ServiceHandle<StoreGateSvc> m_storeGateFastCalo;
    PileUpMergeSvc *m_pMergeSvc;

    const DataHandle<ILArfSampl>   m_dd_fSampl;
    const TileInfo *m_tileInfo;
    const LArEM_ID *m_larEmID;
    const LArFCAL_ID *m_larFcalID;
    const LArHEC_ID *m_larHecID;
    const TileID* m_tileID;

    bool m_pileup;
    /** member variables for algorithm properties: */
    // int/double/bool  m_propertyName;

}; 
#endif          //FASTHITCONVERTTOOL_H 
//} // end of namespace

