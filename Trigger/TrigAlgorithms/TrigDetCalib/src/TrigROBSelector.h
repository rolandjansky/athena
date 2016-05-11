/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: LAr Calibration
 * @Package: TrigDetCalib
 * @Class  : TrigROBSelector
 *
 * @brief : Tool to select a list of ROBs for a given (eta,phi) region and subdetector set
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: TrigROBSelector.h 707504 2015-11-12 16:33:46Z fwinkl $
 **********************************************************************************/

#ifndef TrigEgammaHypo_TrigROBSelector_H
#define TrigEgammaHypo_TrigROBSelector_H

#include "TrigDetCalib/ITrigROBSelector.h"

#include <vector>
#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "IRegionSelector/RegSelEnums.h"

class IRegSelSvc;
class MsgStream;
class PartialEventBuildingInfo;
class TH1I;
    
namespace HLT {
  class TriggerElement;
}

/**
     @class TrigROBSelector
     Tool to select a list of ROBs for a given (eta,phi) region and subdetector set
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
 */


class TrigROBSelector : public AthAlgTool, public virtual ITrigROBSelector
{
  public:

    TrigROBSelector(const std::string& type, const std::string& name,
                    const IInterface* parent = 0);
    
    StatusCode initialize();
    StatusCode finalize();

    HLT::ErrorCode fillPEBInfo(PartialEventBuildingInfo& pebInfo,
                               const IRoiDescriptor& roi, 
                               std::vector<DETID>* dets = 0, std::vector<int>* nRobs = 0);

    HLT::ErrorCode setupMonitoring();
    HLT::ErrorCode bookMonitoringHistograms();
    TH1I* bookAndRegisterTH1I(std::string name, std::string outpath, int nBins, int minX, int maxX);

    bool reset();
    
    StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ); //!< Gaudi interface method
    static const InterfaceID& interfaceID() { return ITrigROBSelector::interfaceID(); }  //!< Gaudi interface method
     
  private:
    
    ServiceHandle<IRegSelSvc> m_regionSelector;
    ServiceHandle<ITHistSvc> m_tHistSvc;

    double m_etaMaxMon, m_etaMinMon, m_phiMinMon, m_phiMaxMon;
    
    std::string m_detSpec;
    std::vector<DETID> m_detectors;
    
    // Monitoring
    std::string m_outputPath;
    std::map<uint32_t, int> m_robIdMap;
    std::map < DETID, std::vector<uint32_t> > m_ranges;
    std::map < DETID, std::string > m_detNames;
    std::map < DETID, TH1I * > m_robHists;
};

#endif
