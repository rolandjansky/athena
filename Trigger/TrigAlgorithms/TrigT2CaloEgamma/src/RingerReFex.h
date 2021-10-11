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
        RingSet( unsigned int maxRings , double deta, double dphi, 
                 const std::vector<int> &detectors,
                 const std::vector<int> &samplings,
                 const std::vector<int> &samples,
                 bool doQuarter, bool doEtaAxesDivision, bool doPhiAxesDivision );
        
        ~RingSet()=default;

        void buildRings( const double eta_hot, const double phi_hot);

        const std::vector<double>& rings() const;
        const std::vector<std::pair<int,int>>  detectors() const;
        bool isValid( const CaloCell * ) const;
        void push_back( const CaloCell * );
        void clear();
        void fill_cells_info(std::vector<float> &cells_eta, std::vector<float> &cells_phi, std::vector<float> &cells_et,  std::vector<int> &cells_sampling, std::vector<int> &cells_size, std::vector < double > &rings_sum);
        
      private:

        double m_deltaEta, m_deltaPhi;
        std::vector<int> m_detectors, m_samplings, m_samples;
        bool m_doQuarter, m_doEtaAxesDivision, m_doPhiAxesDivision;

        std::vector<const CaloCell*> m_cells;
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

    
    
    ToolHandle< GenericMonitoringTool >                m_monTool { this, "MonTool", "", "Monitoring tool"};
    SG::WriteHandleKey<xAOD::TrigRingerRingsContainer> m_ringerContainerKey  {this, "RingerKey", "HLT_FastCaloRinger", "TrigRingerRings container key"};
    SG::ReadHandleKey<xAOD::TrigEMClusterContainer>    m_clusterContainerKey {this, "ClustersName", "HLT_FastCaloEMClusters", "TrigEMCluster container key"};


    Gaudi::Property<double>                        m_etaSearchWindowSize  {this, "EtaSearchWindowSize", 0.1,  ""};
    Gaudi::Property<double>                        m_phiSearchWindowSize  {this, "PhiSearchWindowSize", 0.1,  ""};
    Gaudi::Property<std::vector<float>>            m_deltaEta    {this, "DeltaEta"    , {}   ,  "Eta step for each layer"        };
    Gaudi::Property<std::vector<float>>            m_deltaPhi    {this, "DeltaPhi"    , {}   ,  "Phi step for each layer"        };
    Gaudi::Property<std::vector<unsigned int>>     m_nRings      {this, "NRings"      , {}   ,  "Number of rings for each layer" };
    Gaudi::Property<std::vector<float>>            m_etaBins     {this, "EtaBins"    , {}   , "Eta bins range cover by the reconstruction."};
    Gaudi::Property<std::vector<std::vector<int>>> m_detectors   {this, "Detectors"  , {}   , "" };
    Gaudi::Property<std::vector<std::vector<int>>> m_samplings   {this, "Samplings"  , {}   , "" };
    Gaudi::Property<std::vector<std::vector<int>>> m_samples     {this, "Samples"    , {}   , "samples per layer" };
    
    Gaudi::Property<bool>                          m_useTile     {this, "UseTile"     , true ,  "Use tile cells"                 };
    Gaudi::Property<bool>                          m_decorateWithCells     {this, "DumpCells"     , false ,  "Dump Ringer Cells Information"                 };
    Gaudi::Property<bool>                          m_globalCenter{this, "GlobalCenter", false,  "Use cluster position as center" };
    Gaudi::Property<std::vector<bool>>             m_doQuarter     {this, "DoQuarter", {} ,  "Do Quarter Rings" };
    Gaudi::Property<std::vector<bool>>             m_doEtaAxesDivision{this, "DoEtaAxesDivision", {} ,  "Do Eta axes division" };
    Gaudi::Property<std::vector<bool>>             m_doPhiAxesDivision{this, "DoPhiAxesDivision",{},  "Do Phi axes division" };

 

};
#endif
