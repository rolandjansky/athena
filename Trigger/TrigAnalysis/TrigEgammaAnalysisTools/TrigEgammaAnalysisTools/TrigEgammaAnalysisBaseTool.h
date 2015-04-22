/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaAnalysisBaseTool_H
#define TrigEgammaAnalysisBaseTool_H


#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaAnalysisTools/ITrigEgammaMatchingTool.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
class TrigEgammaAnalysisBaseTool
: public asg::AsgTool,
  virtual public ITrigEgammaAnalysisBaseTool {
ASG_TOOL_CLASS(TrigEgammaAnalysisBaseTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaAnalysisBaseTool( const std::string& myname);
  ~TrigEgammaAnalysisBaseTool() {};

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

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

  void getHistsFromPath(const std::vector<std::string> &pattern, const std::vector<std::string> &notpattern, std::map<std::string, TH1 *> &ret);
  std::string getPath(const std::string &histName, const std::string &dir = "");
  virtual void print() const;

private:
  std::string m_msg;
protected:

  //IToolSvc* m_toolSvc;
  StoreGateSvc * m_storeGate;
  std::string m_file;

  // Histograms
  ITHistSvc* m_histsvc;

  // ToolHandles
  ToolHandle<Trig::TrigDecisionTool> m_trigdec;
  ToolHandle<ITrigEgammaMatchingTool> m_matchTool;
  // Infra-structure members
  std::vector<std::string> m_dir; // maintain directories
  std::map<std::string, TH1 *> m_hist1; // maintain histograms
  std::map<std::string, TH2 *> m_hist2; // maintain histograms
  std::map<std::string, TTree *> m_tree; // maintain trees

  // Book-keeping the current mon group
  std::string m_currentDir;
  /*! Simple setter to pick up correct probe PID for given trigger */
  void parseTriggerName(const std::string,const std::string, std::string &,float &, float &, std::string &,std::string &, bool&, bool&);
  /*! Creates static map to return L1 item from trigger name */
  std::string getL1Item(std::string trigger);
  
  std::string getProbePid(const std::string);// {return m_offProbeTightness;}
  /*! String for offline container key */
  std::string m_offElContKey;

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
  /*! Macros for plotting */  

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

#endif
