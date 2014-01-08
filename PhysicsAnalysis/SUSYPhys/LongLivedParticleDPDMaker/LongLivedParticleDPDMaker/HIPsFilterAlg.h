/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPSFILTERALG_H
#define HIPSFILTERALG_H

/** 
 *  @brief This file contains the class definition for the HIPsFilterAlg class  (originated from MuonFilterAlg). 
 *  @author Sergey Burdin <burdin(at)cern.ch>
 **/

#include <string>
#include <vector>
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "InDetIdentifier/TRT_ID.h"

class HIPsFilterAlg : public AthFilterAlgorithm {
 public: // Constructor and Destructor
   /// Standard Service Constructor
   HIPsFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~HIPsFilterAlg();

 public:
   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

 private:
   const TRT_ID *m_trtID;
   // double deltaPhi(double phi1, double phi2);  
   int m_nProcessed;
   int m_nEventsPassE;
   int m_nEventsPassHTTRTnumber;
   int m_nEventsPassHTTRTfraction;
   int m_nEventsPassAll;
   
   double              m_cutMinE;
   double              m_cutEtaMax;
   double              m_cutMaxDeltaPhi;
   int                 m_cutMinTRTHTHits;
   double              m_cutMinTRTHTFraction;
   std::string         m_caloCluster;
   bool                m_passAll;

};

#endif
