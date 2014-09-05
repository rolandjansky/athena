/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 TileExpertDump
 -----------------------------------------
 Test algorithm to dump out calibration constants from the TileInfoSvc


 2007-05-14      nils.gollub@cern.ch
 2008-06-06      lukas.pribyl@cern.ch
 ***************************************************************************/

#ifndef TILECONDITIONS_TILEEXPERTDUMP_H
#define TILECONDITIONS_TILEEXPERTDUMP_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "TileIdentifier/TileEmscaleCalibOptions.h"
#include "TileConditions/TileExpertToolEmscale.h"

class TileHWID;
class EventInfo;

/**
 ** Algorithn to dump out calibration constants from the TileInfoSvc
 **/

class TileExpertDump: public AthAlgorithm {
  public:
    TileExpertDump(const std::string& name, ISvcLocator* pSvcLocator);
    ~TileExpertDump();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    StatusCode setEmExpertOptions();

  private:

    TileEmscaleCalibOptions m_emExpertOptions;
    bool m_OflCisFitLin;
    bool m_OflCisFitNln;
    bool m_OflLasLin;
    bool m_OflLasNln;
    bool m_OflLasPart;
    bool m_OflCes;
    bool m_OflEms;
    bool m_OflLasLinPisa;
    bool m_OflLasNlnPisa;
    bool m_OflMinbias;
    bool m_OnlCisLin;
    bool m_OnlLasLin;
    bool m_OnlCes;
    bool m_OnlEms;
    void printExpertEmscale();
    void printExpertEmscaleOnl();
    bool m_printExpertEmscale;
    bool m_printExpertEmscaleOnl;

    //=== non-property members
    ToolHandle<TileExpertToolEmscale> m_tileExpertToolEmscale;

    const TileHWID* m_tileHWID;
    const EventInfo* m_evt;
};

#endif 
