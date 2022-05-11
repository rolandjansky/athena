/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileCalibAlgs_TileOFC2DBAlg_h
#define TileCalibAlgs_TileOFC2DBAlg_h

/***************************************************************************
 TileOFC2DBAlg
 -----------------------------------------
 
 This algorithm writes OFC for specified phase to sqlite file to
 be stored afterwards in COOL DB.
 It uses TileCondToolOfc to calculate OFCs. 

 2008-05-13    Andrei.Artamonov@cern.ch
 ***************************************************************************/

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"

// Tile includes
#include "TileConditions/ITileCondToolOfc.h"
#include "TileConditions/TileCondToolTiming.h"

class IIOVRegistrationSvc;

class TileOFC2DBAlg: public AthAlgorithm {

  public:
    TileOFC2DBAlg(const std::string& name, ISvcLocator* pSvcLocator);

    ~TileOFC2DBAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    StatusCode registerCondObjects();
    StatusCode printCondObjects();

    //===  storegate
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,
      "EventInfo", "EventInfo", "EventInfo key"};

    //===  properties
    UnsignedIntegerProperty m_runIOVSince;
    UnsignedIntegerProperty m_runIOVUntil;
    UnsignedIntegerProperty m_lbnIOVSince;
    UnsignedIntegerProperty m_lbnIOVUntil;
    bool m_of2;
    bool m_fixedPhases;
    std::string m_runType;

    //=== non-property members
    IIOVRegistrationSvc* m_regSvc;
    ToolHandle<TileCondToolTiming> m_tileToolTiming{this,
      "TileCondToolTiming", "TileCondToolTiming", "Tile timing tool"};
    ToolHandle<ITileCondToolOfc> m_tileCondToolOfc{this,
      "TileCondToolOfc", "TileCondToolOfc", "Tile OFC tool"};

    //=== store all folders to be registered
    std::set<std::string> m_folders;

    int m_nFixedPhases;
    double m_phaseStep;
    std::vector<std::string> m_modules;
    std::vector<unsigned int> m_drawerIdxs;
    bool m_creatAllModules;
};

#endif //TileCalibAlgs_TileOFC2DBAlg_h
