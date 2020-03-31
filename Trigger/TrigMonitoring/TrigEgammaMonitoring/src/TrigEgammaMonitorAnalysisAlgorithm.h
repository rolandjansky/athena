/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaMonitorAnalysisAlgorithm_H
#define TrigEgammaMonitorAnalysisAlgorithm_H

#include "TrigEgammaMonitorBaseAlgorithm.h"


class TrigEgammaMonitorAnalysisAlgorithm: public TrigEgammaMonitorBaseAlgorithm 
{

  public:

    TrigEgammaMonitorAnalysisAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TrigEgammaMonitorAnalysisAlgorithm() override;

    virtual StatusCode initialize() override;

  protected:

    void fillLabel( const ToolHandle<GenericMonitoringTool>& groupHandle, const std::string &histname, const std::string &label ) const;
    
    void fillL2Calo(const std::string &trigger, const xAOD::TrigEMCluster *emCluster) const;
    void fillL2Electron(const std::string &trigger, const xAOD::TrigElectron *el) const;
    void fillEFCalo(const std::string &trigger, const xAOD::CaloCluster *clus) const;
    void fillShowerShapes(const std::string &trigger,const xAOD::Egamma *eg, bool online ) const; 
    void fillTracking(const std::string &trigger, const xAOD::Electron *eg, bool online ) const;
    void fillL1CaloResolution(const std::string &trigger,const xAOD::EmTauRoI *l1, const xAOD::Egamma *off) const ;
    void fillL1CaloAbsResolution(const std::string &trigger,const xAOD::EmTauRoI *l1, const xAOD::Egamma *off) const;
    void fillL2CaloResolution(const std::string &trigger,const xAOD::TrigEMCluster *onl, const xAOD::Egamma *off) const;
    void fillHLTAbsResolution(const std::string &trigger,const xAOD::Egamma *onl, const xAOD::Egamma *off) const ;
    void fillHLTResolution(const std::string &trigger,const xAOD::Egamma *onl, const xAOD::Egamma *off) const ;
    void fillIsolationResolution(const std::string &trigger, const xAOD::Egamma *onl, const xAOD::Egamma *off) const;


    // Efficiency monitoring
    void fillEfficiencies( std::vector< std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision*> > , TrigInfo ) const;
    void fillEfficiency( const std::string &level, const TrigInfo info, std::vector< std::pair< const xAOD::Egamma *, const TrigCompositeUtils::Decision* >> pairObjs) const;

};

#endif
