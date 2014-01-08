/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STEALTHFILTERALG_H
#define STEALTHFILTERALG_H

/** 
 *  @brief This file contains the class definition for the StealthFilterAlg class  (originated from MuonFilterAlg). 
 *  @author Heather Russell <hrussell(at)cern.ch>
 **/

#include <string>
#include <vector>
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"


class StealthFilterAlg : public AthFilterAlgorithm {
 public: // Constructor and Destructor
   /// Standard Service Constructor
   StealthFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~StealthFilterAlg();

 public:
   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

 private:

   int m_nProcessed;
   int m_nEventsPassAll;

   double              m_cutMinTrackSd0;
   int                 m_cutMinNTracks;
   float               m_cutMinTrackpT;
   float               m_cutMinTrackEta;
   bool                m_passAll;

   std::string         m_trackParticleContainerName;
   
};

#endif
