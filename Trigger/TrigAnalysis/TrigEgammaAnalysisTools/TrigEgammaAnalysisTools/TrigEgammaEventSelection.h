/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMANAVTPNTUPLE_H
#define TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMANAVTPNTUPLE_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavTPBaseTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaPhysValBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODTracking/TrackingPrimitives.h"
#include <map>

class TrigEgammaEventSelection : public TrigEgammaNavTPBaseTool, public TrigEgammaPhysValBase,
                          virtual public ITrigEgammaAnalysisBaseTool
{
  ASG_TOOL_CLASS(TrigEgammaEventSelection, ITrigEgammaAnalysisBaseTool)

  public:
  
    TrigEgammaEventSelection( const std::string& myname );
    ~TrigEgammaEventSelection() {}; 
    StatusCode childInitialize();
    StatusCode childBook();
    StatusCode childExecute();
    StatusCode childFinalize();
 
  private:
    
    /* Dump trigger and offline probe object */
    bool EventSelectionZ();
    bool EventSelectionFakes();

    bool ApplyFireTriggers( const xAOD::Electron *el);
    bool isTruthElectronAny(const xAOD::Egamma *eg );
    
    bool fill(TTree *t, const xAOD::Electron *el );

    /* parse between objects and local variables connected to the 
     * ttree pointer */
    bool fillEvent        ( );
    bool fillMonteCarlo   ( const xAOD::Egamma        *eg         );
    bool fillElectron     ( const xAOD::Electron      *el         );
    bool fillCaloRings    ( const xAOD::Electron      *el         );
    bool fillEmTauRoI     ( const xAOD::EmTauRoI      *emTauRoI   );
    bool fillTrigEMCluster( const xAOD::TrigEMCluster *emCluster  );
    bool fillTrigElectron ( const xAOD::TrigElectron  *trigEl     );
    bool fillCaloCluster  ( const xAOD::CaloCluster   *cluster    );
    bool fillAccept       ( const HLT::TriggerElement *           );
    bool fillTrigCaloRings( const HLT::TriggerElement *feat       );
    bool fillHLTElectron  ( const xAOD::Electron      *el         );
    bool fillTDT          ( const xAOD::Electron      *el      , const HLT::TriggerElement *te   );

    // Helper methods
    double get_el_sigd0(const xAOD::Electron *el);
    double get_el_d0(const xAOD::Electron *el);
    double get_el_eProbabilityHT(const xAOD::Electron *el);
    double get_el_transformed_eProbabilityHT(const xAOD::Electron *el);
    double get_el_d0significance(const xAOD::Electron *el);
    double get_el_deltaPhiRescaled2(const xAOD::Electron *el);
    double get_el_deltaEta1(const xAOD::Electron *el);
    double get_el_DeltaPOverP(const xAOD::Electron *el);


  private:
    
    std::map<std::string, unsigned >m_countMap;

    void count( std::string key ){
      if(m_countMap.find(key) == m_countMap.end()){
        m_countMap[key]= 1;
      }else{
        m_countMap[key]++;
      }
    }



    // [*,*,HLT,EFCalo,L2,L2Calo,L1Calo,isGoodMatch]
    //uint8_t GetByteFromBools(const bool eightBools[8])
    int GetByteFromBools(const bool eightBools[8])
    {
      int ret = 0;
      //uint8_t ret = 0;
      for (int i=0; i<8; i++) if (eightBools[i] == true) ret |= (1<<i);
      return ret;
    }

    //ToolHandleArray<ITrigEgammaEmulationTool>   m_emulationTools;

    ToolHandleArray<IAsgElectronIsEMSelector>   m_EFCaloElectronIsEMSelectors;
    ToolHandleArray<IAsgElectronIsEMSelector>   m_HLTElectronIsEMSelectors;
    ToolHandleArray<IAsgElectronLikelihoodTool> m_EFCaloElectronLHSelectors;
    ToolHandleArray<IAsgElectronLikelihoodTool> m_HLTElectronLHSelectors;
    ToolHandle<IAsgElectronLikelihoodTool>      m_electronLHVLooseTool;

    std::vector<std::string> m_supportingTrigList;
    int  m_detailedDataLevel; 
    bool m_selectionZ;
    bool m_selectionW;
    bool m_selectionMC;
    bool m_selectionJpsi;
    bool m_selectionFakes;
    bool m_doCaloRings;
    bool m_dumpTags;

#define GETTER(_name_)  void get_##_name_(const xAOD::Electron*, float);
  GETTER(trig_EF_calo_isLHTightCaloOnly_rel21_20170217 )
  GETTER(trig_EF_calo_isLHMediumCaloOnly_rel21_20170217)
  GETTER(trig_EF_calo_isLHLooseCaloOnly_rel21_20170217 )
  GETTER(trig_EF_calo_isLHVLooseCaloOnly_rel21_20170217)
  GETTER(trig_EF_el_isLHTight_rel21_20170217 )
  GETTER(trig_EF_el_isLHMedium_rel21_20170217)
  GETTER(trig_EF_el_isLHLoose_rel21_20170217 )
  GETTER(trig_EF_el_isLHVLoose_rel21_20170217)
  GETTER(trig_EF_el_isLHTightNoD0_rel21_20170217 )
  GETTER(trig_EF_el_isLHMediumNoD0_rel21_20170217)
  GETTER(trig_EF_el_isLHLooseNoD0_rel21_20170217 )
  GETTER(trig_EF_el_isLHVLooseNoD0_rel21_20170217)
#undef GETTER

  
};



#endif
