/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: LAr Calibration
 * @Package: TrigEgammaHypo
 * @Class  : TrigCheckForMuons
 *
 * @brief : Algorithm to write out a list of ROBs around muons
 *
 * @author Olya Igonkina, Amy Tee
 *
 * File and Version Information:
 * $Id: TrigCheckForMuons.h 707504 2015-11-12 16:33:46Z fwinkl $
 **********************************************************************************/

#ifndef TrigDetCalib_TrigCheckForMuons_H
#define TrigDetCalib_TrigCheckForMuons_H

#include "TrigInterfaces/AllTEAlgo.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <vector>
#include <string>

#include "IRegionSelector/RegSelEnums.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigDetCalib/ITrigROBSelector.h"

#include "eformat/SourceIdentifier.h"
#include "TrigNavStructure/Types.h"

class IRegSelSvc;
class MsgStream;
class PartialEventBuildingInfo;
class TH1I;
    
namespace HLT {
  class TriggerElement;
}

/**
     @class TrigCheckForMuons
     Algorithm to write out a list of ROBs around all muons.
     In principle, we could also run it only once per event.
     However, if there is another chain that also wants all muons and is not yet finished, then we risk to miss some muons.
     It is very quick, and every ROB will be written only once, so it should not be a problem to repeat it..
     
     @author  Olya Igonkina
*/
class TrigCheckForMuons : public HLT::AllTEAlgo  
{
  public:

  TrigCheckForMuons(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  /** @brief take one input TriggerElement and process it
        It tries to retrieve any attached TrigRoIDescriptor features and prints them.
	@param input  TriggerElement to be processed
	@param output TriggerElement coming out of the algorithm
  */

  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output);

    HLT::ErrorCode hltBeginRun();

    HLT::ErrorCode hltEndEvent();

    
  private:

  ToolHandle<ITrigROBSelector> m_robSelector;
  bool m_addCTPResult, m_addHLTResult;
  bool m_addOppositePhiRoI;
  bool m_addOppositeEtaPhiRoI;

  double m_etaEdge, m_etaWidth, m_phiWidth;
    
  std::string m_pebLabel;
  std::vector<std::string> m_muonLabels;

    int m_maxRoIsPerEvent, m_nRoIs;

    // Hardcoded extra ROBs that should be shipped in all cases
    std::vector<uint32_t> m_extraROBs;    

    // Monitored variables
    std::vector<DETID> m_dets;
    std::vector<eformat::SubDetector> m_trigResults;
    std::vector<int> m_nROBs;
    double m_roIEta, m_roIPhi, m_roIPhi_2;
    float m_mindRSqr;

    bool m_rejectAll;
};

#endif
