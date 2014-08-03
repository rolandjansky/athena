/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEONESD_H
#define ZEEONESD_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : ZeeOnESD.h
/// Package : offline/PhysicsAnalysis/AnalysisCommon/AnalysisExamples
/// Author  : Ketevi A. Assamagan
/// Created : February 2005
///
/// DESCRIPTION:
///
/// Example of Z->ee reconstruction on ESD
/// Ketevi A. Assamagan on February 20, 2005
///
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/Algorithm.h"
//#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "egammaEvent/ElectronContainer.h"

#include <string>
#include "TH1.h"

/// type definition
typedef std::vector<const ElectronContainer::base_value_type*> egammaPair;

class ZeeOnESD : public AthAlgorithm {

 public:

   ZeeOnESD(const std::string& name, ISvcLocator* pSvcLocator);
   ~ZeeOnESD();

   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();

 private:

   /// the selection function for electrons
   friend bool selectEgamma(ZeeOnESD *self, const egammaPair &ll);

   /// the z -> ee reconstruction method
   StatusCode zee_on_esd();

 private:

   /// a handle on Store Gate - don't need this with AthAlgorithm
   //   StoreGateSvc* m_storeGate;

   /** a handle on the Hist/TTree registration service */
   ITHistSvc * m_thistSvc;

   /// name of the AOD electron container to retrieve from StoreGate
   std::string m_egammaContainerName;
   std::string m_trkMatchContainerName;
 
   /// use selection cuts - for electrons 
   /// to be modified thru job options
   double m_etEgammaCut;
   double m_etaEgammaCut;

   /// create the histograms
   /// for electrons
   /// reconstructed quantities
   TH1F* m_esd_egamma_pt;
   TH1F* m_esd_egamma_eta;
   TH1F* m_esd_egamma_overp;
   TH1F* m_esd_egamma_isEM;
   TH1F* m_esd_zee_mass_hist;
   TH1F* m_histEgammaTrackP;
   TH1F* m_histEgammaClusterE;

};

#endif // ZEEONESD_H

