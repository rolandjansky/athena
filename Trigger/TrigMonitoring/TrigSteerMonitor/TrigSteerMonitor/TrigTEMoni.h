/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * @Project: ATLAS HLT Steering Monitoring
 * @Package: Trigger/TrigMonitoring/TrigSteerMonitor
 * @Class  : TrigTEMoni
 * @Created: Nov, 2006
 *
 * @brief monitore number of active trigger elements
 *
 * @author Gordon Fischer        Gordon.Fischer@cern.ch
 *
 * File and Version Information:
 * $Id: TrigTEMoni.h,v 1.11 2008-10-16 20:21:15 tbold Exp $
 **********************************************************************************/


#ifndef TRIGTEMONI_H
#define TRIGTEMONI_H
#include <vector>
#include <string>

#include "TrigMonitorBase/TrigMonitorToolBase.h"

#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

namespace HLT {
   class TrigSteer;
}


class TH1I;
class TH2I;
class StoreGateSvc;

class TrigTEMoni: public TrigMonitorToolBase {

 public:
  TrigTEMoni(const std::string & type, const std::string & name,
	     const IInterface* parent);

  virtual ~TrigTEMoni() {};

  virtual StatusCode initialize();

  virtual StatusCode bookHists();
  virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  virtual StatusCode fillHists();

 private:
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_lvl1ConfigSvc; //!< handle for the LVL1 configuration service
  const HLT::TrigSteer* m_parentAlg; // should give us pointer to TrigSteer topAlgorithm!!!
  
  //std::vector<TH1I*>  m_numberOfTEsHist;
  TH1I  *m_numberOfTEsHist;
  //  TH1I *m_numberOfTEsHist_runsummary;
  TH1I  *m_numberOfallTEsHist;
  //  TH1I *m_numberOfallTEsHist_runsummary;
  
  TH1I  *m_numberOflvl1TEsHist;
  //  TH1I *m_numberOflvl1TEsHist_runsummary;
  TH1I * m_numberOfalllvl1TEsHist;
  //  TH1I *m_numberOfalllvl1TEsHist_runsummary;

  TH2I *m_numberOfTEsHist2d;
  //  TH2I *m_numberOfTEsHist2d_runsummary;
  TH2I *m_numberOfallTEsHist2d;
  //  TH2I *m_numberOfallTEsHist2d_runsummary;


  TH2I *m_numberOflvl1TEsHist2d;
  //  TH2I *m_numberOflvl1TEsHist2d_runsummary;
  TH2I *m_numberOfalllvl1TEsHist2d;
  //  TH2I *m_numberOfalllvl1TEsHist2d_runsummary;


  //  unsigned m_reserveLumiHistos;
  //  unsigned int m_bookedLumiHistos;
  //  std::string m_histoPathexpert;
  std::vector< unsigned int > m_configuredTETypes;
  std::vector< unsigned int > m_configuredlvl1TETypes;
  //std::vector< unsigned int > m_configuredlvl1TETypes_from_lvl1config;
  std::map< unsigned int, unsigned int > m_labels; // TriggerElement ID -> bin  
  std::map<unsigned int,unsigned int> m_lvl1labels;//TriggerElement ID -> bin
  //std::map<unsigned int,unsigned int> m_lvl1cclabels;//TriggerElement ID -> bin
  
  //  std::map<int,int > m_lumiBlockIndex;
  std::string m_trigLvl;

};


#endif
