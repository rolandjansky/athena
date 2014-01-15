/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTSTOREVERTICES_H
#define TRIG_TRIGNTSTOREVERTICES_H

/**********************************************************************************
 * @Package: TrigCostAthena
 * @Class  : TrigNtStoreVertices
 * @Author : Aidan Randle-Conde, Stephen Sekula, Rustem Ospanov
 *
 * @Brief  : Tool for recording reconstructed vertex informartion and trigger
 *           decision information
 *
 **********************************************************************************/

// Framework
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger
#include "TrigCostMonitor/ITrigNtTool.h"

class TTree;
class TFile;
class StoreGateSvc;

namespace Trig
{
  static const InterfaceID IID_TrigNtStoreVertices("Trig::TrigNtStoreVertices", 1, 0); 
  
  class TrigNtStoreVertices : virtual public Trig::ITrigNtTool, virtual public AlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtStoreVertices; }
    
    TrigNtStoreVertices(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtStoreVertices() {}
    
    StatusCode initialize();
    StatusCode finalize();
    
    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);
    
  private:

    MsgStream& log() const { return *m_log; }

  private:
       
    // Properties
    std::string m_offvtxContainerName;
    std::string m_outputNtuple ;    
    bool        m_debug;

    // Athena tool and service handles
    MsgStream                   *m_log;
    ServiceHandle<StoreGateSvc>  m_storeGate;

    TrigMonConfig* m_config ;

    TFile* m_file ;
    TTree* m_treeEvent;
    TTree* m_treeConfig;

    //
    // Event tree branch objects
    //
    Int_t m_runNumber    ;
    Int_t m_lumiBlock    ;
    Int_t m_eventNumber  ;
    Int_t m_bunchId      ;
    Int_t m_timestamp    ;
    Int_t m_timestamp_ns ;
    Int_t m_nVtx         ;

    std::vector<Float_t> *m_vtx_x;
    std::vector<Float_t> *m_vtx_y;
    std::vector<Float_t> *m_vtx_z;
    std::vector<Float_t> *m_vtx_chi2;
    std::vector<Int_t>   *m_vtx_ndof;

    std::vector<uint16_t>* m_event_lv1_ptr ;
    std::vector<uint32_t>* m_event_hlt_ptr ;

    //
    // Config tree branch objects
    //
    Int_t m_HLTPrescaleKey ;
    Int_t m_LV1PrescaleKey ;
    Int_t m_masterKey      ;

    std::vector<std::string> *m_chainNames;
    std::vector<uint32_t>    *m_chainIds;
  };
}

#endif
