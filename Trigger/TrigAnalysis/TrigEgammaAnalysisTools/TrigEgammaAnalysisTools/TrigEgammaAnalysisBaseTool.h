/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaAnalysisBaseTool_H
#define TrigEgammaAnalysisBaseTool_H

#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/TrigEgammaMatchingTool.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTrigger/TrigPassBits.h"
#include "xAODCaloRings/CaloRings.h"
#include "xAODCaloRings/CaloRingsContainer.h"
#include "xAODCaloRings/tools/getCaloRingsDecorator.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


using namespace Trig;




class TrigEgammaAnalysisBaseTool: public asg::AsgTool, virtual public ITrigEgammaAnalysisBaseTool 
{
  ASG_TOOL_CLASS(TrigEgammaAnalysisBaseTool, ITrigEgammaAnalysisBaseTool)

  public:
  
    TrigEgammaAnalysisBaseTool( const std::string& myname);
    virtual ~TrigEgammaAnalysisBaseTool() {};
  
    virtual StatusCode initialize() override;
    virtual StatusCode execute( const EventContext& ctx ) const  override;
    virtual StatusCode finalize() override;
  
    
    virtual void setGMTools( ToolHandleArray<GenericMonitoringTool> &tools ) override {m_GMTools=tools;}
    virtual void setTP(bool tp) override  {m_tp=tp;}
    virtual void setDetail(bool detail) override  {m_detailedHists=detail;}
    virtual void setAvgmu(float value) override  {m_lbAverageInteractionsPerCrossing=value;};
  
    
    virtual StatusCode toolExecute(const EventContext&, std::string, TrigInfo, 
        std::vector<std::pair<const xAOD::Egamma_v1*, const TrigEgammaMatchingUtils::Element> >) const override{return StatusCode::SUCCESS;}; 
    

 
  private:
   
    /*! Propagate m_tp to other tools */
    void updateTP(Property& p);
    /*! Propagate m_detailedHists to other tools */
    void updateDetail(Property& p);


    /*! Trigger decision tool */
    ToolHandle<Trig::TrigDecisionTool> m_trigdec;
    /* Trigger e/g matching tool */
    ToolHandle<Trig::TrigEgammaMatchingTool> m_matchTool;
    /*! lbAverageInteractionsPerCrossing */  
    float m_lbAverageInteractionsPerCrossing;
    /*! creates map of trigger name and TrigInfo struct */
    std::map<std::string,TrigInfo> m_trigInfo;
    /*! AcceptInfo to store TrigDecision */
    asg::AcceptInfo m_accept;
    /*! Helper strings for trigger level analysis */
    static const std::vector<std::string> m_trigLevel;
    /*! Helper strings for trigger level analysis */
    static const std::map<std::string,std::string> m_trigLvlMap;
  
  
  
  protected:
    
    /*! Tool analysis handles */
    ToolHandleArray<ITrigEgammaAnalysisBaseTool> m_tools {this, "Tools", {}};
    /*! Generic monitoring tools */
    ToolHandleArray<GenericMonitoringTool> m_GMTools {this,"GMTools",{}}; ///< Array of Generic Monitoring Tools 
    /*! Offline isEM Selectors */
    ToolHandleArray<IAsgElectronIsEMSelector> m_electronIsEMTool{this,"ElectronIsEMSelector",{}};
    /*! Offline LH Selectors */
    ToolHandleArray<IAsgElectronLikelihoodTool> m_electronLHTool{this,"ElectronLikelihoodTool",{}};
    /*! Offline LH Very loose selector */
    ToolHandle<IAsgElectronLikelihoodTool> m_electronLHVLooseTool{this,"ElectronLHVLooseTool", ""};
    
    
    /*! TP Trigger Analysis */
    bool m_tp;
    /*! default probe pid for trigitems that don't have pid in their name */
    std::string m_defaultProbePid;
    /*! isem names */
    std::vector<std::string> m_isemname;
    /*! lh names */
    std::vector<std::string> m_lhname;
    /*! Include more detailed histograms */
    bool m_detailedHists;
  
  
    /** Helper methods **/
 


    float avgmu() const {return m_lbAverageInteractionsPerCrossing;};
    /*! Get the TDT  */
    const ToolHandle<Trig::TrigDecisionTool>& tdt() const {return m_trigdec;};
    /*! Get the e/g match tool */
    const ToolHandle<Trig::TrigEgammaMatchingTool>& match() const {return m_matchTool;}
    /*! Get the accept object for all trigger levels */
    const asg::AcceptInfo& getAccept() const {return m_accept;}
    /*! Set the accept object for all trigger levels */
    asg::AcceptData setAccept(const TrigEgammaMatchingUtils::Element, const TrigInfo) const;
    /*! Get the trigger info parsed from the chain name */
    TrigInfo getTrigInfo(const std::string) const;
    /*! Get delta R */
    float dR(const float, const float, const float, const float) const;
    /*! Simple setter to pick up correct probe PID for given trigger */
    void parseTriggerName(const std::string,const std::string, bool&, std::string &,float &, float &, std::string &,std::string &, bool&, bool&) const;
    /*! Split double object trigger in two simple object trigger */
    bool splitTriggerName(const std::string, std::string &, std::string &) const;
    /*! Creates static map to return L1 item from trigger name */
    std::string getL1Item(std::string trigger) const;
    /*! Check if electron fulfils isolation criteria */
    bool isIsolated(const xAOD::Electron*, const std::string) const;
    /*! Check if the event is prescaled */
    bool isPrescaled(const std::string) const;
    /*! Get the pid name */
    std::string getProbePid(const std::string) const;
    /*! Set the trigger info parsed from the chain name */
    void setTrigInfo(const std::string);
    
    
  
    /** Features helper **/
   
  
  
    /*! Helper functions now part of base class */
    float getEta2(const xAOD::Egamma* eg) const;
    float getEt(const xAOD::Electron* eg) const ;
    float getEtCluster37(const xAOD::Egamma* eg) const;
    float getDEmaxs1(const xAOD::Egamma *eg) const;
    float rTRT  (const xAOD::Electron* eg) const;
    float getSigmaD0(const xAOD::Electron *eg) const;
    float getD0sig(const xAOD::Electron *eg) const;
    float getEnergyBE0(const xAOD::Egamma *eg) const;
    float getEnergyBE1(const xAOD::Egamma *eg) const;
    float getEnergyBE2(const xAOD::Egamma *eg) const;
    float getEnergyBE3(const xAOD::Egamma *eg) const;
    float getEaccordion(const xAOD::Egamma *eg) const;
    float getE0Eaccordion(const xAOD::Egamma *eg) const;



  /*! C Macros for plotting */
#define GETTER(_name_) float getShowerShape_##_name_(const xAOD::Egamma* eg) const;
  GETTER(e011)
      GETTER(e132)
      GETTER(e237)
      GETTER(e277)
      GETTER(ethad)
      GETTER(ethad1)
      GETTER(weta1)
      GETTER(weta2)
      GETTER(f1)
      GETTER(e2tsts1)
      GETTER(emins1)
      GETTER(emaxs1)
      GETTER(wtots1)
      GETTER(fracs1)
      GETTER(Reta)
      GETTER(Rphi)
      GETTER(f3)
      GETTER(f3core)
      GETTER(Eratio)
      GETTER(Rhad)
      GETTER(Rhad1)
      GETTER(DeltaE)
#undef GETTER


      // GETTER for Isolation monitoring
#define GETTER(_name_) float getIsolation_##_name_(const xAOD::Electron* eg) const;
      GETTER(ptcone20)
      GETTER(ptcone30)
      GETTER(ptcone40)
      GETTER(ptvarcone20)
      GETTER(ptvarcone30)
      GETTER(ptvarcone40)
#undef GETTER
#define GETTER(_name_) float getIsolation_##_name_(const xAOD::Egamma* eg) const;
      GETTER(etcone20)
      GETTER(etcone30)
      GETTER(etcone40)
      GETTER(topoetcone20)
      GETTER(topoetcone30)
      GETTER(topoetcone40)
#undef GETTER
      // GETTERs for CaloCluster monitoring
#define GETTER(_name_) float getCluster_##_name_(const xAOD::Egamma* eg) const;
      GETTER(et)
      GETTER(phi)
      GETTER(eta)
#undef GETTER

      // GETTERs for Track monitoring
#define GETTER(_name_) float getTrack_##_name_(const xAOD::Electron* eg) const;
      GETTER(pt)
      GETTER(phi)
      GETTER(eta)
      GETTER(d0)
      GETTER(z0)
#undef GETTER


      // GETTERs for Track details monitoring
#define GETTER(_name_) float getTrackSummary_##_name_(const xAOD::Electron* eg) const;
      GETTER(numberOfInnermostPixelLayerHits)
      GETTER(numberOfInnermostPixelLayerOutliers)
      GETTER(numberOfPixelHits)
      GETTER(numberOfPixelOutliers)
      GETTER(numberOfSCTHits)
      GETTER(numberOfSCTOutliers)
      GETTER(numberOfTRTHits)
      GETTER(numberOfTRTHighThresholdHits)
      GETTER(numberOfTRTHighThresholdOutliers)
      GETTER(numberOfTRTOutliers)
      GETTER(expectInnermostPixelLayerHit)
      GETTER(numberOfPixelDeadSensors)
      GETTER(numberOfSCTDeadSensors)
      GETTER(numberOfTRTXenonHits)
#undef GETTER

#define GETTER(_name_) float getTrackSummaryFloat_##_name_(const xAOD::Electron* eg) const;
      GETTER(eProbabilityComb)
      GETTER(eProbabilityHT)
      GETTER(pixeldEdx)
#undef GETTER

      // GETTERs for Calo-Track monitoring
#define GETTER(_name_) float getCaloTrackMatch_##_name_(const xAOD::Electron* eg) const;
      GETTER(deltaEta0)
      GETTER(deltaPhi0)
      GETTER(deltaPhiRescaled0)
      GETTER(deltaEta1)
      GETTER(deltaPhi1)
      GETTER(deltaPhiRescaled1)
      GETTER(deltaEta2)
      GETTER(deltaPhi2)
      GETTER(deltaPhiRescaled2)
      GETTER(deltaEta3)
      GETTER(deltaPhi3)
      GETTER(deltaPhiRescaled3)
#undef GETTER
};

#endif
