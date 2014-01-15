/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTVERTEXTOOL_H
#define TRIG_TRIGNTVERTEXTOOL_H

/**********************************************************************************
 * @Package: TrigCostAthena
 * @Class  : TrigNtVertexTool
 * @Author : Stephen Sekula, Rustem Ospanov
 *
 * @Brief  : Tool for recording reconstructed vertex informartion
 *
 **********************************************************************************/

// Framework
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"

// Detectors
#include "TileIdentifier/TileTBID.h"

// Trigger
#include "TrigCostMonitor/ITrigNtTool.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "VxVertex/VxContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"


// Local
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostData/Vertex.h"

class TTree;
class ITHistSvc;

namespace Trig
{
  static const InterfaceID IID_TrigNtVertexTool("Trig::TrigNtVertexTool", 1, 0); 
  
  class TrigNtVertexTool : virtual public Trig::ITrigNtTool, virtual public AlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtVertexTool; }
    
    TrigNtVertexTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtVertexTool() {}
    
    StatusCode initialize();
    StatusCode finalize();
    
    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);
    
  private:

    MsgStream& log() const { return *m_log; }

  private:
       
    // Properties
    std::string                          m_vertexContainerName;
    std::string                          m_offvtxContainerName;
    std::string                          m_hltvtxContainerName;
    bool                                 m_debug;
    bool                                 m_fillVertexVector;
    float                                m_vertexThreshold;

    // Athena tool and service handles
    MsgStream                           *m_log;
    ServiceHandle<StoreGateSvc>          m_storeGate;
    ServiceHandle<ITHistSvc>             m_histSvc;
    TrigVertexCollection*                m_vertexColl;
    VxContainer*                         m_offvtxColl;
    VxContainer*                         m_hltvtxColl;

    ToolHandle<Trig::TrigDecisionTool>   m_trigDec;


    // Anp interface objects:
    Anp::Branch< std::vector<Trig::Vertex> > m_vertexVector;
    Anp::Branch< std::vector<Trig::Vertex> > m_offvtxVector;
    Anp::Branch< std::vector<Trig::Vertex> > m_hltvtxVector;
  };
}

#endif
