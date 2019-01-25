/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: LAr Calibration
 * @Package: TrigEgammaHypo
 * @class  : TrigSubDetListWriter
 *
 * @brief : Algorithm to write out a list of LAr ROBs for its input RoI
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * Ignacio Aracena:
 * July 22 2008: taken from TrigROBListWriter and modified to read out the whole detector
 *
 * File and Version Information:
 * $Id: TrigSubDetListWriter.h 707504 2015-11-12 16:33:46Z fwinkl $
 **********************************************************************************/

#ifndef TrigEgammaHypo_TrigSubDetListWriter_H
#define TrigEgammaHypo_TrigSubDetListWriter_H

#include "GaudiKernel/ServiceHandle.h"
#include <vector>
#include <string>

#include "TrigInterfaces/AllTEAlgo.h"
#include "IRegionSelector/RegSelEnums.h"
#include "GaudiKernel/ITHistSvc.h"

#include "eformat/SourceIdentifier.h"

#include "TH1I.h"

class IRegSelSvc;
class PartialEventBuildingInfo;
//class TH1I;
    
namespace HLT {
  class TriggerElement;
}

/**
     @class TrigSubDetListWriter
     Algorithm to write out a list of LAr ROBs for its input RoI
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
*/
class TrigSubDetListWriter : public HLT::AllTEAlgo
{
  public:

  TrigSubDetListWriter(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  /** @brief take one input TriggerElement and process it
        It tries to retrieve any attached TrigRoIDescriptor features and prints them.
	@param input  TriggerElement to be processed
	@param output TriggerElement coming out of the algorithm
  */
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& te_in,
			      unsigned int output);

    HLT::ErrorCode fillPEBInfo(PartialEventBuildingInfo& pebInfo);

    HLT::ErrorCode hltStart();

    TH1I* bookAndRegisterTH1I(const char* name, std::string outpath, int nBins, int minX, int maxX);

    StatusCode bookMonitoringHistograms();

    HLT::ErrorCode hltEndEvent();

    void fillL1CaloRobs(std::vector<uint32_t>& roblist);

  private:
    
    ServiceHandle<IRegSelSvc> m_regionSelector;
    std::string m_pebLabel;

    std::string m_detSpec;
    std::vector<std::string> m_detId;
    std::vector<DETID> m_detectors;
    std::vector<eformat::SubDetector> m_sourceid;
    std::vector<uint32_t> m_extrarobs;
    bool m_addL1Calo;
    
    bool m_useCachedResult;          //!< internal caching: true when the hltExecute will run in cached mode
    int m_maxRoIsPerEvent, m_nRoIs;

    std::map<uint32_t,int> m_robidmap;

    /* Histogram service */
    std::vector<uint32_t> m_pixrange;
    std::vector<uint32_t> m_sctrange;
    std::vector<uint32_t> m_trtrange;
    std::vector<uint32_t> m_ttemrange;
    std::vector<uint32_t> m_tthecrange;
    std::vector<uint32_t> m_tilerange;
    std::vector<uint32_t> m_mdtrange;
    std::vector<uint32_t> m_rpcrange;
    std::vector<uint32_t> m_tgcrange;
    std::vector<uint32_t> m_cscrange;
    std::vector<uint32_t> m_fcalrange;
    std::vector<uint32_t> m_fcalemrange;

    /* Histogram service */
    ITHistSvc *  m_thistSvc;

    std::string m_outputpath;

    bool m_rejectAll;
    /** detector mask */
  typedef SimpleProperty< std::vector<uint32_t> > Uint32ArrayProperty;
  /** @brief list of all enabled ROBs which can be retrieved */
  Uint32ArrayProperty       m_enabledROBs;

  /** @brief list of all enabled Sub Detectors which can be retrieved */
  Uint32ArrayProperty       m_enabledSubDetectors;

  /// check if a ROB is enabled for readout in OKS
  bool isRobEnabled(const uint32_t robid);
//    std::string m_outputpath;

//    std::string m_outputpath;

};

#endif
