/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RINGERREFEX_H
#define RINGERREFEX_H


#include <string>
#include "xAODTrigRinger/TrigRingerRings.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "TrigT2CaloCommon/IReAlgToolCalo.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "PhiComps.h"

/*
 * This is the Ringer's tool. It creates the rings which are put into
 * RingerRings and then looked by a multi-variable hypothesis making
 * algorithm or regular LVL2 decision is taken by TrigL2CaloHypo.
 * The number, sampling and widths of the rings is fully
 * configurable.
 */
class RingerReFex : public IReAlgToolCalo
{

  public:

    // This class is very usefull since this select each ring the energy cell will be added give
    // the center (hottest position) and the current cell position
    // Helper class used to build ringsets
    class RingSet {
      public:

        RingSet(unsigned int max, unsigned int maxCells, double etasz, double phisz, const std::vector<CaloSampling::CaloSample> &samples);
        virtual ~RingSet();

        void push_back(const std::vector<const CaloCell *>& c, const double eta_center, const double phi_center);

        void clear(void){
          for(unsigned int i = 0; i < m_rings.size(); ++i)
            m_rings[i] = 0;
        }

        inline const double& eta_size(void) const { return m_etasz; }
        inline const double& phi_size(void) const { return m_phisz; }
        inline size_t        maxCells(void) const { return m_maxCells; }
        inline size_t        size(void) const { return m_rings.size(); }

        const std::vector<double>& pattern(void) const { return m_rings; };
        const std::vector<CaloSampling::CaloSample>& samples(void) const { return m_samples; }
        bool belongs( const CaloCell *c ) const;

      private:

        double m_etasz; // the width of rings, in eta
        double m_phisz; // the width of rings, in phi
        size_t m_maxCells; // the amount of cells to gather
        // The vector of rings that will be used to hold the sum energy
        std::vector<double>  m_rings; // my current values
        std::vector<CaloSampling::CaloSample> m_samples; ///< I'm good for those
        float m_cachedOverEtasize; ///< cached value of 1/m_config.eta_size() for optimizations
        float m_cachedOverPhisize; ///< cached value of 1/m_config.phi_size() for optimizations


    };


  public:

    RingerReFex (const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~RingerReFex() { }

    using IReAlgToolCalo::execute;

    virtual StatusCode initialize() override;

    virtual StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
                               const IRoiDescriptor& roi,
                               const CaloDetDescrElement*& /*caloDDE*/,
                               const EventContext& context) const override;



  private:

    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool"};
    SG::WriteHandleKey<xAOD::TrigRingerRingsContainer> m_ringerContainerKey {this, "RingerKey", "HLT_FastCaloRinger", "TrigRingerRings container key"};
    SG::ReadHandleKey<xAOD::TrigEMClusterContainer>    m_clusterContainerKey {this, "ClustersName", "HLT_FastCaloEMClusters", "TrigEMCluster container key"};
    Gaudi::Property<std::vector<float>>  m_etaBins  {this, "EtaBins", {}, "Eta bins range cover by the ringer reconstruction."};
    Gaudi::Property<bool>  m_global_center  {this, "GlobalCenter", false,  ""};
    Gaudi::Property<bool>  m_useHad  {this, "UseHad", true,  ""};
    Gaudi::Property<double>  m_etaSearchWindowSize  {this, "EtaSearchWindowSize", 0.1,  ""};
    Gaudi::Property<double>  m_phiSearchWindowSize  {this, "PhiSearchWindowSize", 0.1,  ""};
    Gaudi::Property<std::vector<float>>  m_detaRings  {this, "DEtaRings", {},  ""};
    Gaudi::Property<std::vector<float>>  m_dphiRings  {this, "DPhiRings", {},  ""};
    Gaudi::Property<std::vector<unsigned int>>  m_nRings  {this, "NRings", {},  ""};
    Gaudi::Property<std::vector<unsigned int>>  m_layersRings  {this, "LayersRings", {},  ""};
    Gaudi::Property<std::vector<unsigned int>>  m_nlayersRings  {this, "NLayersRings", {},  ""};
    Gaudi::Property<std::vector<unsigned int>>  m_maxCells  {this, "NMaxCells", {},  ""};

    unsigned int              m_maxRingsAccumulated;

    bool configurationInvalid();


    // Calculates the maximum energy cell in a CaloCell collection.
    inline void maxCell ( const std::vector<const CaloCell*>& vcell,
    			                double& eta,
                          double& phi,
                          const double eta_ref,
    			                const double phi_ref,
                          const double eta_window,
    			                const double phi_window) const ;


    void printRings( std::vector<RingSet> &, xAOD::TrigRingerRings * ) const;

};
#endif
