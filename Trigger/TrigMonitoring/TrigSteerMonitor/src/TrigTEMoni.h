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
 * $Id: TrigTEMoni.h 771932 2016-09-06 12:53:25Z fwinkl $
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
  const HLT::TrigSteer* m_parentAlg{0}; // should give us pointer to TrigSteer topAlgorithm!!!
  
  TH1I *m_numberOfTEsHist{0};
  TH1I *m_numberOfallTEsHist{0};
  TH1I *m_numberOflvl1TEsHist{0};
  TH1I *m_numberOfalllvl1TEsHist{0};
  TH2I *m_numberOfTEsHist2d{0};
  TH2I *m_numberOfallTEsHist2d{0};
  TH2I *m_numberOflvl1TEsHist2d{0};
  TH2I *m_numberOfalllvl1TEsHist2d{0};

  std::vector< unsigned int > m_configuredTETypes;
  std::vector< unsigned int > m_configuredlvl1TETypes;
  std::map< unsigned int, unsigned int > m_labels; // TriggerElement ID -> bin  
  std::map<unsigned int,unsigned int> m_lvl1labels;//TriggerElement ID -> bin
  std::string m_trigLvl;
};


#endif
