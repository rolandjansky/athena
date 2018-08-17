/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGL2CALORINGERTOOL_H
#define TRIGL2CALORINGERTOOL_H 1

#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "xAODTrigRinger/TrigRNNOutput.h"





class TrigL2CaloRingerHypoToolMT : virtual public ::AthAlgTool
{ 
 public: 
  TrigL2CaloRingerHypoToolMT( const std::string& type, 
                            const std::string& name, 
                            const IInterface* parent );

  virtual ~TrigL2CaloRingerHypoToolMT();
  virtual StatusCode initialize() override;



  struct RNNOutInfo {
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrigRNNOutput* rnnOut;
  };

  StatusCode decide( std::vector<RNNOutInfo>& decisions )  const;
  bool decideOnSingleObject( const xAOD::TrigRNNOutput* rnnOut ) const;

  
      ///Helper class
  class CutDefsHelper{
      private:
        double m_etamin;
        double m_etamax;
        double m_etmin;
        double m_etmax;
        double m_threshold;

      public:
        CutDefsHelper(double th, double etamin, double etamax,
                      double etmin, double etmax):m_etamin(etamin),
                      m_etamax(etamax),m_etmin(etmin),m_etmax(etmax),
                      m_threshold(th)
        {;}

        ~CutDefsHelper()
        {;}
        double threshold(){return m_threshold;};
        double etamin(){return m_etamin;};
        double etamax(){return m_etamax;};
        double etmin(){return m_etmin;};
        double etmax(){return m_etmax;};

    };///end of configuration

    std::vector<TrigL2CaloRingerHypoToolMT::CutDefsHelper*>  m_cutDefs; 



 private:
  HLT::Identifier m_decisionId;
  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
  Gaudi::Property<std::vector<double>>                                m_thresholds;
  Gaudi::Property<std::vector<std::vector<double>>>                   m_etaBins;
  Gaudi::Property<std::vector<std::vector<double>>>                   m_etBins;
  Gaudi::Property<double>                                             m_nThresholds;
  Gaudi::Property<double>                                             m_emEtCut;

  
  size_t m_multiplicity = 1;

  //ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

//:DECLARE_TOOL_FACTORY( TrigL2CaloRingerHypoToolMT )
#endif //> ! TRIGL2CALORINGERTOOL_H
