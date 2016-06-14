/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaAnalysisBaseTool_H
#define TrigEgammaAnalysisBaseTool_H


#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "PATCore/TAccept.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "TrigEgammaAnalysisTools/ITrigEgammaPlotTool.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
//#include "xAODCaloRings/RingSetContainer.h" 
//#include "xAODCaloRings/CaloRingsContainer.h"
//#include "xAODCaloRings/tools/getCaloRingsDecorator.h"
#include "xAODEgamma/PhotonAuxContainer.h"
//class MonGroup;


class TrigEgammaAnalysisBaseTool
: public asg::AsgTool,
  virtual public ITrigEgammaAnalysisBaseTool {
ASG_TOOL_CLASS(TrigEgammaAnalysisBaseTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaAnalysisBaseTool( const std::string& myname);
  ~TrigEgammaAnalysisBaseTool() {};

  StatusCode initialize();
  StatusCode book();
  StatusCode execute();
  StatusCode finalize();
  template<class T> const T* getFeature(const HLT::TriggerElement* te,const std::string key="");
  template<class T> bool ancestorPassed(const HLT::TriggerElement* te,const std::string key="");
  template <class T1, class T2> const T1* closestObject(const std::pair<const xAOD::Egamma *, const HLT::TriggerElement *>, float &, bool usePassbits=true,const std::string key="");
  void setParent(IHLTMonTool *parent){ m_parent = parent;};
  void setPlotTool(ToolHandle<ITrigEgammaPlotTool> tool){m_plot=tool;}
  void setDetail(bool detail){m_detailedHists=detail;}
  void setTP(bool tp){m_tp=tp;}

  // Set current MonGroup
  void cd(const std::string &dir);

  // Accessor members
  void addDirectory(const std::string &s);
  void addHistogram(TH1 *h, const std::string &dir = "");
  void addHistogram(TH2 *h, const std::string &dir = "");
  void addTree(TTree *t, const std::string &dir = "");

  TH1 *hist1(const std::string &histName, const std::string &dir = "");
  TH2 *hist2(const std::string &histName, const std::string &dir = "");
  TTree *tree(const std::string &treeName, const std::string &dir = "");
  void setLabels(TH1* histo, const std::vector<std::string>& labels);
  
  virtual void print() const;
  
private:
  
  /*! Property update handlers */
  void updateDetail(Property& p);
  void updateAltBinning(Property& p);
  void updateTP(Property& p);
  void updateOutputLevel(Property& p);

  std::string m_msg;
  /*! creates map of trigger name and TrigInfo struct */
  std::map<std::string,TrigInfo> m_trigInfo;
  /*! Include more detailed histograms */
  bool m_detailedHists;
  /*! TAccept to store TrigDecision */
  Root::TAccept m_accept;
  /*! Helper strings for trigger level analysis */
  static const std::vector<std::string> m_trigLevel;
  static const std::map<std::string,std::string> m_trigLvlMap;
  // Properties
  
  ToolHandle<Trig::TrigDecisionTool> m_trigdec;
  ToolHandle<Trig::ITrigEgammaMatchingTool> m_matchTool;
  ToolHandle<ITrigEgammaPlotTool> m_plot;

protected:
  // Methods
  /*! Simple setter to pick up correct probe PID for given trigger */
  void parseTriggerName(const std::string,const std::string, bool&, std::string &,float &, float &, std::string &,std::string &, bool&, bool&);
  /*! Split double object trigger in two simple object trigger */
  bool splitTriggerName(const std::string, std::string &, std::string &);
  /*! Creates static map to return L1 item from trigger name */
  std::string getL1Item(std::string trigger);

  /*! Check if electron fulfils isolation criteria */
  bool isIsolated(const xAOD::Electron*, const std::string);
  bool isPrescaled(const std::string); 

  std::string getProbePid(const std::string);// {return m_offProbeTightness;}
  /*! book common histograms for analysis */
  void bookAnalysisHistos(const std::string);

  void setTrigInfo(const std::string);
  TrigInfo getTrigInfo(const std::string);
  std::map<std::string,TrigInfo> getTrigInfoMap() { return m_trigInfo; }
 
  float dR(const float, const float, const float, const float);

  /*! Helper functions now part of base class */
  float getEta2(const xAOD::Egamma* eg);
  float getEt(const xAOD::Electron* eg);
  float getEtCluster37(const xAOD::Egamma* eg);
  float getDEmaxs1(const xAOD::Egamma *eg);
  float rTRT  (const xAOD::Electron* eg);
  float getSigmaD0(const xAOD::Electron *eg);
  float getD0sig(const xAOD::Electron *eg);
  float getEnergyBE0(const xAOD::Egamma *eg);
  float getEnergyBE1(const xAOD::Egamma *eg);
  float getEnergyBE2(const xAOD::Egamma *eg);
  float getEnergyBE3(const xAOD::Egamma *eg);
  float getEaccordion(const xAOD::Egamma *eg);
  float getE0Eaccordion(const xAOD::Egamma *eg);
  float getAvgMu();
  
  /* trig rings and offline rings helper method for feature extraction from xaod */
  bool getCaloRings( const xAOD::Electron *el, std::vector<float> &ringsE );
  bool getTrigCaloRings( const xAOD::TrigEMCluster *emCluster, std::vector<float> &ringsE );
  void parseCaloRingsLayers( unsigned layer, unsigned &minRing, unsigned &maxRing, std::string &caloLayerName);
  const xAOD::TruthParticle* matchTruth(const xAOD::TruthParticleContainer *truthCont, const xAOD::Egamma *eg,
                                        bool &Zfound,bool &Wfound);

  /* calculate the number of primary vertex for each event*/
  void calculatePileupPrimaryVertex();
  int m_nGoodVtx; 
  int m_nPileupPrimaryVtx;

  /*! Retrieve ToolHandles */
  ITrigEgammaPlotTool *plot(){return &*m_plot;}
  Trig::TrigDecisionTool *tdt(){return &*m_trigdec;};
  Trig::ITrigEgammaMatchingTool *match(){return &*m_matchTool;}

  // Retrieve Properties
  bool getDetail(){return m_detailedHists;}
  bool getTP(){return m_tp;}

  // TAccept
  Root::TAccept getAccept(){return m_accept;}
  void setAccept(Root::TAccept accept){m_accept=accept;}
  void setAccept(const HLT::TriggerElement *,const TrigInfo);
  //Class Members
  // Athena services
  StoreGateSvc * m_storeGate;
  ITHistSvc* m_histsvc;
  IHLTMonTool *m_parent;

  // ToolHandles
  ToolHandleArray<ITrigEgammaAnalysisBaseTool> m_tools;

  /*! enable luminosity tool. If is false, we will use primary vertex calc */
  bool m_useLumiTool;
  /*! Offline Lumi tool */
  ToolHandle<ILuminosityTool>  m_lumiTool; // This would retrieve the offline <mu>
  /*! Online Lumi tool */
  ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool; // This would retrieve the offline <mu>
  
  /*! Set Jpsiee */
  bool m_doJpsiee;
  /*! TP Trigger Analysis */
  bool m_tp;

  
  // Infra-structure members
  std::string m_file;
  
  /*! String for offline container key */
  std::string m_offElContKey;
  /*! String for offline container key */
  std::string m_offPhContKey;
  /*! Cut Counter Labels for histograms */
  std::vector<std::string> m_cutlabels;
  /*! IsEM Labels for histograms */
  std::vector<std::string> m_labels;
  /*! isEMResultNames for offline */
  std::vector<std::string> m_isemname;
  /*! LHResultNames for offline */
  std::vector<std::string> m_lhname;

  // Common data members
  //
  /*! default probe pid for trigitems that don't have pid in their name */
  std::string m_defaultProbePid;

  /*! C Macros for plotting */  
#define GETTER(_name_) float getShowerShape_##_name_(const xAOD::Egamma* eg);
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
#define GETTER(_name_) float getIsolation_##_name_(const xAOD::Electron* eg);
      GETTER(ptcone20)
      GETTER(ptcone30)
      GETTER(ptcone40)    
      GETTER(ptvarcone20)
      GETTER(ptvarcone30)
      GETTER(ptvarcone40)    
#undef GETTER    
#define GETTER(_name_) float getIsolation_##_name_(const xAOD::Egamma* eg);
      GETTER(etcone20)
      GETTER(etcone30)
      GETTER(etcone40)    
      GETTER(topoetcone20)
      GETTER(topoetcone30)
      GETTER(topoetcone40)   
#undef GETTER    
      // GETTERs for CaloCluster monitoring   
#define GETTER(_name_) float getCluster_##_name_(const xAOD::Egamma* eg);
      GETTER(et)
      GETTER(phi)
      GETTER(eta)   
#undef GETTER

      // GETTERs for Track monitoring   
#define GETTER(_name_) float getTrack_##_name_(const xAOD::Electron* eg);
      GETTER(pt)
      GETTER(phi)
      GETTER(eta)   
      GETTER(d0)  
      GETTER(z0)    
#undef GETTER


      // GETTERs for Track details monitoring    
#define GETTER(_name_) float getTrackSummary_##_name_(const xAOD::Electron* eg);
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

#define GETTER(_name_) float getTrackSummaryFloat_##_name_(const xAOD::Electron* eg);
      GETTER(eProbabilityComb)
      GETTER(eProbabilityHT)
      GETTER(pixeldEdx)    
#undef GETTER

      // GETTERs for Calo-Track monitoring
#define GETTER(_name_) float getCaloTrackMatch_##_name_(const xAOD::Electron* eg);
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
/** Main feature of the code 
 * templates the TE to get a feature
 * or find the passing TE
 * **********************/
template<class T>
const T*
TrigEgammaAnalysisBaseTool::getFeature(const HLT::TriggerElement* te,const std::string key){
    if ( te == NULL ) return NULL;
    if ( (m_trigdec->ancestor<T>(te,key)).te() == NULL )
        return NULL;
    return ( (m_trigdec->ancestor<T>(te)).cptr() );
}

template<class T>
bool
TrigEgammaAnalysisBaseTool::ancestorPassed(const HLT::TriggerElement* te,const std::string key){
    if ( te == NULL ) return false;
    if ( (m_trigdec->ancestor<T>(te,key)).te() == NULL )
        return false;
    return ( (m_trigdec->ancestor<T>(te)).te()->getActiveState());
}

template <class T1, class T2>
const T1*
TrigEgammaAnalysisBaseTool::closestObject(const std::pair<const xAOD::Egamma *,const HLT::TriggerElement *> pairObj, float &dRmax,bool usePassbits,const std::string key){
    float eta = pairObj.first->eta();
    float phi = pairObj.first->phi();
    // Reset to resonable start value
    if(dRmax < 0.15) dRmax = 0.15;
    const auto *cont=getFeature<T2>(pairObj.second,key);
    if(cont==NULL) return NULL;
    const TrigPassBits *bits = NULL;
    if(usePassbits){ 
        bits=getFeature<TrigPassBits>(pairObj.second);
        if(bits==NULL) return NULL;
    }
    const T1 *cl = NULL;
    float dr=0.; 
    for(const auto& obj : *cont){
        if( usePassbits && !HLT::isPassing(bits,obj,cont) ) continue; 
        if(obj==NULL) continue;
        dr=dR(eta,phi,obj->eta(),obj->phi());
        if ( dr<dRmax){
            dRmax=dr;
            cl = obj;
        } // dR
    }
    return cl;
}

#endif
