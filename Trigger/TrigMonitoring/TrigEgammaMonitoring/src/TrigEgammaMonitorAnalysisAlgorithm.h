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


    // Efficiency monitoring
    void fillEfficiencies( std::vector< std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision*> > , TrigInfo ) const;
    void fillEfficiency( const std::string &subgroup, const std::string &level, const std::string &pidword, const TrigInfo info, 
                         std::vector< std::pair< const xAOD::Egamma *, const TrigCompositeUtils::Decision* >> pairObjs) const;


    void fillDistributions( std::vector< std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision*> > , TrigInfo ) const;
    void fillL1Calo( const std::string &trigger, std::vector< const xAOD::EmTauRoI* > l1_vec ) const; 
    void fillL2Calo(const std::string &trigger, std::vector< const xAOD::TrigEMCluster*> emCluster_vec ) const;
    void fillL2Electron(const std::string &trigger, std::vector<const xAOD::TrigElectron*> el_vec) const;
    void fillEFCalo(const std::string &trigger, std::vector<const xAOD::CaloCluster*> clus_vec) const;
    void fillShowerShapes(const std::string &trigger, std::vector<const xAOD::Egamma*> eg_vec, bool online ) const; 
    void fillTracking(const std::string &trigger, std::vector<const xAOD::Electron *> eg_vec, bool online ) const;


};

#endif
