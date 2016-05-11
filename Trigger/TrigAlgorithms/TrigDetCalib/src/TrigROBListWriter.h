/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: LAr Calibration
 * @Package: TrigEgammaHypo
 * @Class  : TrigROBListWriter
 *
 * @brief : Algorithm to write out a list of LAr ROBs for its input RoI
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: TrigROBListWriter.h 707504 2015-11-12 16:33:46Z fwinkl $
 **********************************************************************************/

#ifndef TrigEgammaHypo_TrigROBListWriter_H
#define TrigEgammaHypo_TrigROBListWriter_H

#include "TrigInterfaces/FexAlgo.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <vector>
#include <string>

#include "IRegionSelector/RegSelEnums.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigDetCalib/ITrigROBSelector.h"

#include "eformat/SourceIdentifier.h"

class IRegSelSvc;
class MsgStream;
class PartialEventBuildingInfo;
class TH1I;
    
namespace HLT {
  class TriggerElement;
}

/**
     @class TrigROBListWriter
     Algorithm to write out a list of LAr ROBs for its input RoI
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
*/
class TrigROBListWriter : public HLT::FexAlgo
{
  public:

  TrigROBListWriter(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  /** @brief take one input TriggerElement and process it
        It tries to retrieve any attached TrigRoIDescriptor features and prints them.
	@param input  TriggerElement to be processed
	@param output TriggerElement coming out of the algorithm
  */
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
			      HLT::TriggerElement* output);

    HLT::ErrorCode hltBeginRun();

    HLT::ErrorCode hltEndEvent();


    
  private:
    
    ToolHandle<ITrigROBSelector> m_robSelector;
    bool m_addCTPResult, m_addL2Result, m_addEFResult;

    double m_etaEdge, m_etaWidth, m_phiWidth;
    
    std::string m_pebLabel;
    std::vector<std::string> m_roILabels;

    int m_maxRoIsPerEvent, m_nRoIs;

    // Hardcoded extra ROBs that should be shipped in all cases
    std::vector<uint32_t> m_extraROBs;    

    // Monitored variables
    std::vector<DETID> m_dets;
    std::vector<eformat::SubDetector> m_trigResults;
    std::vector<int> m_nROBs;
    double m_roIEta, m_roIPhi;

    bool m_rejectAll;
};

#endif
