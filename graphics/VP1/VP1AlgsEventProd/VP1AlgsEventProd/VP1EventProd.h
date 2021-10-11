/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1Alg                           //
//                                                         //
//  update: Riccardo-Maria BIANCHI <rbianchi@cern.ch>      //
//          23 May 2014                                    //
//                                                         //
//  This is the Athena algorithm starting the production   //
//  of event files for VP1 Live, the online 3D event       //
//  display at P1.                                         //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1ALGS_VP1EVENTPROD
#define VP1ALGS_VP1EVENTPROD

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include <string>

class VP1EventProd: public AthAlgorithm,
		    public IIncidentListener
{
 public:
  VP1EventProd(const std::string& name, ISvcLocator* pSvcLocator);
  ~VP1EventProd();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void handle(const Incident& inc);

 private:
  // run/event number to be used in the vp1 event file name
  unsigned long m_runNumber;
  uint64_t m_eventNumber;
  unsigned int m_timeStamp;
  std::string m_humanTimestamp;
  std::string m_outputFileType;
  bool m_removeTempInputFiles;

  // properties
  std::string m_inputPoolFile;
  std::string m_destinationDir;
  bool m_createDestinationDir;
  int m_maxProducedFiles;
  int m_nEvent; // Internal counter for the number of processed events
};

#endif
