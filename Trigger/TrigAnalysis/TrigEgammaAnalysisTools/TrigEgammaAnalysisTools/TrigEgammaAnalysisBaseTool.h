/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaAnalysisBaseTool_H
#define TrigEgammaAnalysisBaseTool_H


#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "LumiBlockComps/ILuminosityTool.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
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
  template<class T> const T* getFeature(const HLT::TriggerElement* te);
  template<class T> bool ancestorPassed(const HLT::TriggerElement* te);
  void setParent(IHLTMonTool *parent){ m_parent = parent;};

  // Should be over-written
  //StatusCode childInitialize()=0;
  //StatusCode childExecute()=0;
  //StatusCode childFinalize()=0;

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
  void getHistsFromPath(const std::vector<std::string> &pattern, const std::vector<std::string> &notpattern, std::map<std::string, TH1 *> &ret);
  std::string getPath(const std::string &histName, const std::string &dir = "");
  virtual void print() const;
  
  //void setParent(const IHLTMonTool *);

private:
  std::string m_msg;
  
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

  std::string getProbePid(const std::string);// {return m_offProbeTightness;}
  /*! book common histograms for analysis */
  void bookAnalysisHistos(const std::string);

  /*! fill kinematic histograms, et,eta,phi,lumi and efficiency */
  void fillEfficiency(const std::string,bool,const float,const float,const float,const float,const float avgmu=0.,const float mass=0.);

  void fillL2CaloResolution(const std::string, const xAOD::TrigEMCluster *,const xAOD::Egamma *);
  void fillHLTResolution(const std::string, const xAOD::Egamma *,const xAOD::Egamma *);
  void fillHLTAbsResolution(const std::string, const xAOD::Egamma *,const xAOD::Egamma *);
  void fillShowerShapes(const std::string, const xAOD::Egamma *); // Online and Offline fillers
  void fillTracking(const std::string, const xAOD::Electron *); // Online and Offline fillers
  void fillEFCalo(const std::string,const xAOD::CaloCluster *);
  void fillL2Electron(const std::string,const xAOD::TrigElectron *);
  void fillL2Calo(const std::string,const xAOD::TrigEMCluster *);
  void fillL1Calo(const std::string,const xAOD::EmTauRoI *);
  void fillL1CaloResolution(const std::string, const xAOD::EmTauRoI*, const xAOD::Egamma*);
  void fillL1CaloAbsResolution(const std::string, const xAOD::EmTauRoI*, const xAOD::Egamma*);
  /*! Inefficiency analysis */
  void inefficiency(const std::string,const unsigned int, const unsigned int,const float,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  void fillInefficiency(const std::string,const xAOD::Electron *,const xAOD::Photon *,const xAOD::CaloCluster *,const xAOD::TrackParticle *); 
 
  /*! Resolution methods */
  void resolution(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  void resolutionPhoton(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  void resolutionElectron(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  void resolutionL2Photon(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  void resolutionL2Electron(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  void resolutionEFCalo(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  
  /*! Finalizes efficiency for kinematic histograms */
  void finalizeEfficiency(std::string);
 
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


  //Class Members
  // Athena services
  StoreGateSvc * m_storeGate;
  ITHistSvc* m_histsvc;
  IHLTMonTool *m_parent;

  // ToolHandles
  ToolHandle<Trig::TrigDecisionTool> m_trigdec;
  ToolHandle<Trig::ITrigEgammaMatchingTool> m_matchTool;
  /*! Offline Lumi tool */
  ToolHandle<ILuminosityTool>  m_lumiTool; // This would retrieve the offline <mu>
  /*! Online Lumi tool */
  ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool; // This would retrieve the offline <mu>
  /*! Set Jpsiee */
  bool m_doJpsiee;

  
  // Infra-structure members
  std::string m_file;
  std::set<std::string> m_mongroups; // MonGroups for T0
  std::vector<std::string> m_dir; // maintain directories
  std::map<std::string, TH1 *> m_hist1; // maintain histograms
  std::map<std::string, TH2 *> m_hist2; // maintain histograms
  std::map<std::string, TTree *> m_tree; // maintain trees

  // Book-keeping the current mon group
  std::string m_currentDir;
  /*! String for offline container key */
  std::string m_offElContKey;
  /*! String for offline container key */
  std::string m_offPhContKey;
  /*! Cut Counter Labels for histograms */
  std::vector<std::string> m_cutlabels;
  /*! IsEM Labels for histograms */
  std::vector<std::string> m_labels;
  // Common data members
  /*! creates map of category and triggers for each category */
  //std::map<std::string,std::vector<std::string>> m_mamMap;
  //
  //Include more detailed histograms
  bool m_detailedHists;
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
      GETTER(etcone20)
      GETTER(etcone30)
      GETTER(etcone40)    
      GETTER(topoetcone20)
      GETTER(topoetcone30)
      GETTER(topoetcone40)    
      GETTER(ptcone20)
      GETTER(ptcone30)
      GETTER(ptcone40)    
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
      GETTER(eProbabilityToT)
      GETTER(eProbabilityBrem)
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
TrigEgammaAnalysisBaseTool::getFeature(const HLT::TriggerElement* te){
    if ( te == NULL ) return NULL;
    if ( (m_trigdec->ancestor<T>(te)).te() == NULL )
        return NULL;
    return ( (m_trigdec->ancestor<T>(te)).cptr() );
}

template<class T>
bool
TrigEgammaAnalysisBaseTool::ancestorPassed(const HLT::TriggerElement* te){
    if ( te == NULL ) return false;
    if ( (m_trigdec->ancestor<T>(te)).te() == NULL )
        return false;
    return ( (m_trigdec->ancestor<T>(te)).te()->getActiveState());
}


#endif
