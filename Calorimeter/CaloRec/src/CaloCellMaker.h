/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCELLMAKER_H
#define CALOREC_CALOCELLMAKER_H

/********************************************************************

 NAME:     CaloCellMaker.h
 PACKAGE:  offline/Calorimeter/CaloRec

 AUTHORS:  David Rousseau
 CREATED:  May 11, 2004

 PURPOSE:  Create a CaloCellContainer by calling a set of tools
 sharing interface CaloInterface/ICaloCellMakerTool.h

 ********************************************************************/

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/IChronoStatSvc.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"

// Calo includes
//#include "CaloInterface/ICaloCellMakerTool.h"

class IChronoStatSvc;
class ICaloCellMakerTool;
class CaloCellContainer;

class CaloCellMaker: public AthReentrantAlgorithm {

  public:

    CaloCellMaker(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~CaloCellMaker();

    virtual StatusCode initialize() override;
    virtual StatusCode execute (const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:

//    IChronoStatSvc* m_chrono;
    ServiceHandle<IChronoStatSvc> m_chrono;

    /** whether CellContainer to be created will own (default) its cells or not */
    int m_ownPolicy;

    // Output cell continer to be used 
    //std::string m_caloCellsOutputName;
    SG::WriteHandleKey<CaloCellContainer> m_caloCellsOutputKey;

    // Authorize input to be the same as output (to be done with care)
    //bool m_caloCellHack;

    // list of tools to be used
    //std::vector< std::string > m_caloCellMakerToolNames ;
    //std::vector < ICaloCellMakerTool *> m_caloCellMakerTools ;
    ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerTools;

    //unsigned m_evCounter;

};
#endif
