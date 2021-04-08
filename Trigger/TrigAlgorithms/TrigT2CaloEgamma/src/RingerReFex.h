/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT2CALOEGAMMA_RINGERREFEX_H
#define TRIGT2CALOEGAMMA_RINGERREFEX_H


#include <string>
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "TrigT2CaloCommon/IReAlgToolCalo.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "helpers/PhiComps.h"
#include "helpers/VectorVectorIntParser.h"



class RingerReFex : public IReAlgToolCalo
{

  public:


    class RingSet {
     
      public:
        RingSet( unsigned int maxRings , 
                 double deta, 
                 double dphi, 
                 const std::vector<int> &detectors,
                 const std::vector<int> &samplings,
                 const std::vector<int> &samples );
        
        ~RingSet()=default;

        void push_back( const std::vector<const CaloCell *>& cells, const double eta_hot, const double phi_hot);

        const std::vector<double>& rings(void) const { return m_rings; };
        
        const std::vector<std::pair<int,int>>  detectors() const;
      
        bool isValid( const CaloCell *c ) const;

      private:

        double m_deltaEta, m_deltaPhi;
        std::vector<int> m_detectors, m_samplings, m_samples;
        std::vector<double>  m_rings; 

    };


  public:

    RingerReFex(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~RingerReFex () { };

    using IReAlgToolCalo::execute;

    virtual StatusCode initialize() override;

    virtual StatusCode execute(xAOD::TrigEMCluster &emCluster,
                               const IRoiDescriptor& roi,
                               const CaloDetDescrElement*& /*caloDDE*/,
                               const EventContext& context) const override;



  private:


    bool configurationInvalid();

    inline bool maxCell ( const CaloCell* cell, double &energy, const double eta_ref, const double phi_ref ) const;
    
    void printRings( std::vector<RingSet> &, const xAOD::TrigEMCluster & ) const;


    unsigned m_maxRingsAccumulated;
    
    
    ToolHandle< GenericMonitoringTool >                m_monTool { this, "MonTool", "", "Monitoring tool"};
    SG::WriteHandleKey<xAOD::TrigRingerRingsContainer> m_ringerContainerKey  {this, "RingerKey", "HLT_FastCaloRinger", "TrigRingerRings container key"};
    SG::ReadHandleKey<xAOD::TrigEMClusterContainer>    m_clusterContainerKey {this, "ClustersName", "HLT_FastCaloEMClusters", "TrigEMCluster container key"};


    Gaudi::Property<double>                        m_etaSearchWindowSize  {this, "EtaSearchWindowSize", 0.1,  ""};
    Gaudi::Property<double>                        m_phiSearchWindowSize  {this, "PhiSearchWindowSize", 0.1,  ""};
    Gaudi::Property<bool>                          m_globalCenter{this, "GlobalCenter", false,  "Use cluster position as center" };
    Gaudi::Property<bool>                          m_useTile     {this, "UseTile"     , true ,  "Use tile cells"                 };
    Gaudi::Property<std::vector<float>>            m_deltaEta    {this, "DeltaEta"    , {}   ,  "Eta step for each layer"        };
    Gaudi::Property<std::vector<float>>            m_deltaPhi    {this, "DeltaPhi"    , {}   ,  "Phi step for each layer"        };
    Gaudi::Property<std::vector<unsigned int>>     m_nRings      {this, "NRings"      , {}   ,  "Number of rings for each layer" };
    Gaudi::Property<std::vector<float>>            m_etaBins     { this, "EtaBins"    , {}   , "Eta bins range cover by the reconstruction."};
    Gaudi::Property<std::vector<std::vector<int>>> m_detectors   { this, "Detectors"  , {}   , "" };
    Gaudi::Property<std::vector<std::vector<int>>> m_samplings   { this, "Samplings"  , {}   , "" };
    Gaudi::Property<std::vector<std::vector<int>>> m_samples     { this, "Samples"    , {}   , "samples per layer" };

};
#endif
