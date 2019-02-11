/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBMONITORING_TBPHASEMONTOOL_H
#define TBMONITORING_TBPHASEMONTOOL_H

/*! \brief monitors trigger-TTCclock phase and related TDCs
 *
 * The \c TBPhaseMonTool produces trigger-TTCclock related TDC distributions.
 *
 * \author Michel Lefebvre <lefebvre@uvic.ca>
 * \date August 07, 2004
 * 
 * June 30, 2004 - first implementation
 */  

#include "AthenaMonitoring/MonitorToolBase.h"

#include "CaloGeoHelpers/CaloSampling.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "Identifier/HWIdentifier.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

class StoreGateSvc;
class IToolSvc;
class LArOnlineID;
class LArCablingLegacyService;

#include <map>
#include <string>
#include <vector>

class TBPhaseMonTool: public MonitorToolBase
{
  
 public:
  
  /*! \brief \c AlgTool constructor */
  TBPhaseMonTool(const std::string& type,
		 const std::string& name,
		 const IInterface* parent);

  virtual ~TBPhaseMonTool();

  virtual StatusCode initialize();
    
  virtual StatusCode bookHists();
  virtual StatusCode fillHists();
  virtual StatusCode finalHists();

  virtual StatusCode bookMyHists();

 protected:

  ServiceHandle<LArCablingLegacyService> m_cablingService;
  const LArOnlineID* m_onlineHelper;

  /*! \brief properties */
  // in base class !!!  std::string m_path;
  std::string m_tdcContainerName;
  std::string m_caloCellName;
  std::string m_TBPhaseName;

  std::vector<std::string> m_tdcNames ;
  std::vector<std::string> m_samplingNames;

  double m_energyCut;
  int    m_tdcBins;
  double m_tdcLow, m_tdcHigh;
  double m_tdcRightLow, m_tdcRightHigh;
  double m_tdcLeftLow, m_tdcLeftHigh;
  int    m_timeBins;
  double m_timeLow, m_timeHigh;
  int    m_phaseBins;
  double m_phaseLow, m_phaseHigh;
  double m_tUnit;
  int    m_dTtoWACBins;
  double m_dTtoWACLow, m_dTtoWACHigh;
  
  std::string m_recoName;
  
  /*! \brief stores */
  //  const CaloCell_ID* m_caloCellHelper;
  std::vector<CaloSampling::CaloSample> m_samplingIndices;
  std::map< CaloCell_ID::SUBCALO, std::vector<CaloSampling::CaloSample> > m_calosAndSamplings;
  std::vector<HWIdentifier> m_febIDs;

  /*! \brief internal helpers */
  StatusCode setupLookupTables();
  std::map<int, std::string>                               m_slotToFebNameLookup;
  std::map<CaloCell_ID::SUBCALO, std::string>              m_caloToNameLookup;
  std::map<std::string, CaloCell_ID::SUBCALO>              m_caloFromNameLookup;
  std::map<CaloSampling::CaloSample, std::string>          m_samplingToNameLookup;
  std::map<std::string, CaloSampling::CaloSample>          m_samplingFromNameLookup;
  std::map<CaloSampling::CaloSample, CaloCell_ID::SUBCALO> m_caloLookup;

 private:

  /*! \brief tdc distribution plots */
  std::map<std::string, AIDA::IHistogram1D*> m_tdcHMap;  // tdcName keyed

  /*! \brief in the case of more than 1 tdc */
  AIDA::IHistogram1D* m_tdcDelta;       // tdc1 - tdc0
  AIDA::IHistogram1D* m_tdcDeltaRight;  // right peak of tdc1 - tdc0
  AIDA::IHistogram1D* m_tdcDeltaLeft;   // left  peak of tdc1 - tdc0
  AIDA::IHistogram2D* m_tdcCorrelation; // tdc1 vs tdc0

  /*! \brief cubic energy weighted peaking time per sampling */
  std::map<CaloSampling::CaloSample, AIDA::IHistogram1D*> m_cubicTimePerSampling;       // sampling id keyed
  std::map<CaloSampling::CaloSample, AIDA::IHistogram1D*> m_cubicPhaseTimePerSampling;  // phase corrected
  std::map<CaloSampling::CaloSample, AIDA::IHistogram2D*> m_cubicTimeVsPhasePerSampling;
  /*! \brief cubic energy weighted peaking time per sampling vs tdc*/
  std::map<CaloSampling::CaloSample, std::map<std::string, AIDA::IHistogram2D*> > m_cubicTimeVsTDCPerSampling; // sampling id , tdc name keyed

  /*! \brief cubic energy weighted peaking time per FEB */
  std::map<HWIdentifier, AIDA::IHistogram1D*> m_cubicTimePerFeb;       // febID keyed
  std::map<HWIdentifier, AIDA::IHistogram1D*> m_cubicPhaseTimePerFeb;  // phase corrected
  std::map<HWIdentifier, AIDA::IHistogram2D*> m_cubicTimeVsPhasePerFeb;
  /*! \brief cubic energy weighted peaking time per FEB vs tdc*/
  std::map<HWIdentifier, std::map<std::string, AIDA::IHistogram2D*> > m_cubicTimeVsTDCPerFeb; // febID , tdc name keyed

  /*! \brief phase distribution plot */
  AIDA::IHistogram1D* m_phase;          // phase from TBPhaseRec
  AIDA::IHistogram1D* m_dTtoWAC;        // phase from TBPhaseRec

};
#endif
