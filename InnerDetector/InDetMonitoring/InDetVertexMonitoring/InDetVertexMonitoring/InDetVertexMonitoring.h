/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetVertexMonitoring_H
#define InDetVertexMonitoring_H

#include <string>
#include <map>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/StatusCode.h"

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

class TH1F_LW;
class TF2F_LW;
 
class InDetVertexMonitoring : public ManagedMonitorToolBase  {

 public:
   InDetVertexMonitoring( const std::string & type, const std::string & name, const IInterface* parent );
   ~InDetVertexMonitoring();

   virtual StatusCode initialize();
   virtual StatusCode bookHistograms();
   virtual StatusCode fillHistograms();

 protected:
 
  TH1F_LW* m_h_vxdist;
  TH1F_LW* m_h_Ntracks;
  TH1F_LW* m_h_sumpt;
  TH1F_LW* m_h_pvChiSq;
  TH1F_LW* m_h_pvDof;
  TH1F_LW* m_h_pvChiSqovDof;
  TH1F_LW* m_h_pvChiSqProb;
  TH1F_LW* m_h_cumulVx;
  TH2F_LW* m_h_deltaZpairs;

  private:
   bool m_doControlPlots;
   
   StoreGateSvc* m_storeGate;
   std::string m_VxContainerName;
   std::string m_histFolder;

};

#endif // InDetVertexMonitoring_H

