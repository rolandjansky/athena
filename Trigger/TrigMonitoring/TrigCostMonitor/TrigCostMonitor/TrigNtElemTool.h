/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTELEMTOOL_H
#define TRIG_TRIGNTELEMTOOL_H

// $Id: $$
//
// Tool that collects trigger elements

// Framework
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Local
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigCostMonitor/IElemNtTool.h"
#include "TrigCostMonitor/ITrigNtTool.h"

class IClassIDSvc;
class TrigRoiDescriptor;

namespace LVL1
{
  class RecJetEtRoI;
  class RecEnergyRoI;
  class RecJetRoI;
  class RecMuonRoI;
  class RecEmTauRoI;
}

namespace HLT
{
  class TriggerElement;
}

namespace Trig
{
  static const InterfaceID IID_TrigNtElemTool("Trig::TrigNtElemTool", 1, 0); 
  
  class TrigNtElemTool : virtual public Trig::ITrigNtTool, public AthAlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtElemTool; }
    
    TrigNtElemTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtElemTool() {}
    
    StatusCode initialize();
    StatusCode finalize();

    void SetSteer(const HLT::TrigSteer *ptr);
    
    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);

  private:
    
    struct RoiData {
      RoiData() 
  : roiEm(0), roiEnergy(0), roiJet(0), roiJetEt(0), roiMuon(0), 
     roiHlt(0), roiValid(false) {}

      const LVL1::RecEmTauRoI      *roiEm;
      const LVL1::RecEnergyRoI     *roiEnergy;
      const LVL1::RecJetRoI        *roiJet;
      const LVL1::RecJetEtRoI      *roiJetEt;
      const LVL1::RecMuonRoI       *roiMuon;
      const TrigRoiDescriptor      *roiHlt;
      
      bool       roiValid;
      TrigMonRoi roiMon;
    };

    typedef std::map<const HLT::TriggerElement *, TrigMonTE> TEMap;
    typedef std::map<const HLT::TriggerElement *, RoiData> RoiMap;

  private:

    void CollectTEs(TrigMonTE &elem_parent,
        const std::vector<HLT::TriggerElement *> &te_vec);

    void FillAllRoI(const HLT::TriggerElement* _initialNode);
    
    TrigMonTE& MakeElem(const HLT::TriggerElement *hlt_te);

    bool IsChainOutputTE(const HLT::TriggerElement *hlt_te, int level) const;

    bool PassFilter(TrigMonTE &elem) const;

    void ReadRoiId(TrigMonTE &elem, const HLT::TriggerElement *hlt_te);

  private:
       
    // Tool properties
    bool           m_saveNavigation;    // Save full navigation
    bool           m_collectRoIData;    // Collect additional RoI data
    bool           m_collectClid;       // Collect TE features
    bool           m_filterTE;          // Filter saved trigger elements
    bool           m_printClid;         // Print feature clids (very noisy)
    bool           m_printDebug;        // Print debugging messages

    // Athena tool and service handles
    const HLT::TrigSteer               *m_parentAlg;
    HLT::Navigation                    *m_navig;
    IClassIDSvc                        *m_clidSvc;
    ToolHandleArray<Trig::IElemNtTool>  m_elemTools;   
        
    // Tool variables
    TrigMonConfig    *m_Config;            // Pointer to trigger configuration
    TEMap             m_Map;               // TriggerElement to TrigMonTE map
    RoiMap            m_Roi;               // RoIWord to RoiData map
  };
}

#endif
