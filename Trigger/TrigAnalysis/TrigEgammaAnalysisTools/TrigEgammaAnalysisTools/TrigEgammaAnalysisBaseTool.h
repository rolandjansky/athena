/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaAnalysisBaseTool_H
#define TrigEgammaAnalysisBaseTool_H

#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "TrigEgammaAnalysisTools/ITrigEgammaPlotTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationTool.h"
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
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigger/TrigPassBits.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODCaloRings/RingSet.h"                     
#include "xAODCaloRings/RingSetContainer.h"            
#include "xAODCaloRings/CaloRings.h"                   
#include "xAODCaloRings/CaloRingsContainer.h"          
#include "xAODCaloRings/tools/getCaloRingsDecorator.h" 

class TrigEgammaAnalysisBaseTool
: public asg::AsgTool,
  virtual public ITrigEgammaAnalysisBaseTool {
ASG_TOOL_CLASS(TrigEgammaAnalysisBaseTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaAnalysisBaseTool( const std::string& myname);
  ~TrigEgammaAnalysisBaseTool() {};

  StatusCode initialize() ;
  StatusCode book() ;
  StatusCode execute() ;
  StatusCode finalize() ;
  template<class T, class B> std::unique_ptr<xAOD::TrigPassBits> createBits(const T* CONT, const B* BITS);
  template<class T> std::unique_ptr<xAOD::TrigPassBits> getBits(const HLT::TriggerElement* te,const T* CONT);
  template<class T> const T* getFeature(const HLT::TriggerElement* te,const std::string key="");
  template<class T> bool ancestorPassed(const HLT::TriggerElement* te,const std::string key="");
  template <class T1, class T2> const T1* closestObject(const std::pair<const xAOD::Egamma *, const HLT::TriggerElement *>, 
                                                        float &, bool usePassbits=true,const std::string key="");
  // Interface class methods needed to pass information to additional tools or to set common tools
  void setParent(IHLTMonTool *parent)  { m_parent = parent;};
  void setPlotTool(ToolHandle<ITrigEgammaPlotTool> tool)  {m_plot=tool;}
  void setDetail(bool detail)  {m_detailedHists=detail;}
  void setTP(bool tp)  {m_tp=tp;}
  void setEmulation(bool doEmu)  {m_doEmulation=doEmu;}
  void setEmulationTool(ToolHandle<Trig::ITrigEgammaEmulationTool> tool)  {m_emulationTool=tool;}
  void setPVertex(const float onvertex, const float ngoodvertex)  {m_nPVertex = onvertex; m_nGoodVertex = ngoodvertex;}
  void setAvgMu(const float onlmu, const float offmu)  {m_onlmu=onlmu; m_offmu=offmu;} //For tools called by tools
  
  void setSGContainsRnn(bool contains)  {m_sgContainsRnn=contains;}
  void setSGContainsTrigPhoton(bool contains)  {m_sgContainsTrigPhoton=contains;}
  
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
  void updateEmulation(Property& p);
  void updateOutputLevel(Property& p);

  std::string m_msg;
  /*! creates map of trigger name and TrigInfo struct */
  std::map<std::string,TrigInfo> m_trigInfo;
  /*! Include more detailed histograms */
  bool m_detailedHists;
  /*! AcceptInfo to store TrigDecision */
  asg::AcceptInfo m_accept;
  /*! Helper strings for trigger level analysis */
  static const std::vector<std::string> m_trigLevel;
  static const std::map<std::string,std::string> m_trigLvlMap;
  
  /*! Cache pileup info */
  float m_onlmu;
  float m_offmu;
  float m_nGoodVertex; 
  float m_nPVertex;
  
  /*! Hook to check for RNN features in SG */
  bool m_sgContainsRnn;
  bool m_sgContainsTrigPhoton;
  // Properties  
  ToolHandle<Trig::TrigDecisionTool> m_trigdec;
  ToolHandle<Trig::ITrigEgammaMatchingTool> m_matchTool;
  ToolHandle<Trig::ITrigEgammaEmulationTool> m_emulationTool;
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
  void setTrigEmulation(){m_forceTrigEmulation=true;};
  
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

  /*! Setters/Getters for caching Pileup info*/
  float getAvgOnlineMu(){return m_onlmu;}
  void setAvgOnlineMu();
  float getAvgOfflineMu(){return m_offmu;}
  void setAvgOfflineMu();

  /* calculate the number of primary vertex for each event*/
  void calculatePileupPrimaryVertex();
  float getNPVtx(){return m_nPVertex;}
  float getNGoodVertex(){return m_nGoodVertex;}
 
  /* Protection for Holders not in AOD */
  bool getSGContainsRnn() { return m_sgContainsRnn;}
  bool getSGContainsTrigPhoton() { return m_sgContainsTrigPhoton;}

  /* trig rings and offline rings helper method for feature extraction from xaod */ 
  bool getCaloRings( const xAOD::Electron *, std::vector<float> & );
  bool getTrigCaloRings( const xAOD::TrigEMCluster *, std::vector<float> & );
  void parseCaloRingsLayers( unsigned layer, unsigned &minRing, unsigned &maxRing, std::string &caloLayerName);

  /* Helper function to write all trigger lint into a txt file */
  bool write_trigger_list( const std::vector<std::string>  &);
   // Monte carlo answer: Electron, Photon, EnhancedBias, Zee, Unknow
  MonteCarlo::PDGID pdgid(const xAOD::Egamma *, const xAOD::TruthParticleContainer *,const xAOD::TruthParticle *&);

  /*! Retrieve ToolHandles */
  ITrigEgammaPlotTool *plot(){return &*m_plot;}
  Trig::TrigDecisionTool *tdt(){return &*m_trigdec;};
  Trig::ITrigEgammaMatchingTool *match(){return &*m_matchTool;}
  Trig::ITrigEgammaEmulationTool *emulation(){return &*m_emulationTool;}

  // Retrieve Properties
  bool getDetail() const {return m_detailedHists;}
  bool getTP() const {return m_tp;}
  bool getEmulation() const {return m_doEmulation;}

  // AcceptInfo/Data
  const asg::AcceptInfo& getAccept() const {return m_accept;}
  asg::AcceptData setAccept(const HLT::TriggerElement *,const TrigInfo);
  
  //Class Members
  // Athena services
  StoreGateSvc * m_storeGate;
  ITHistSvc* m_histsvc;
  IHLTMonTool *m_parent;

  // ToolHandles
  ToolHandleArray<ITrigEgammaAnalysisBaseTool> m_tools;
  /*! Offline Lumi tool */
  ToolHandle<ILuminosityTool>  m_lumiTool; // This would retrieve the offline <mu>
  /*! Online Lumi tool */
  ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool; // This would retrieve the offline <mu>
  
  /*! Set Jpsiee */
  bool m_doJpsiee;
  /*! TP Trigger Analysis */
  bool m_tp;
  /*! Emulation Analysis */
  bool m_doEmulation;
  /*! force emulation trigger */
  bool m_forceTrigEmulation;
  /* force trigger attachment into the list */
  bool m_forceTrigAttachment;

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

// Attempt to create a unique_ptr from existing xAOD::TrigPassBits
// crashes at container_clid and AuxVectorData line 424(359) 
// when retrieving the cached auxid variable from store
// Assuming my attempt to utilise smart pointers breaks the caching of aux data in ESDtoAOD.
//
// Instead template the conversion and always create transient passBit
template<class CONT, class BITS> 
std::unique_ptr< xAOD::TrigPassBits> TrigEgammaAnalysisBaseTool::createBits(const CONT* container, const BITS* passbits){
    ATH_MSG_DEBUG("Converting "<< ClassID_traits< BITS >::ID() );
    std::unique_ptr<xAOD::TrigPassBits> xbits(new xAOD::TrigPassBits());
    xbits->makePrivateStore();
    xbits->reset(container);
    xbits->setSize(passbits->size());
    if(passbits->size()==0){
        xbits->setPassBits( std::vector< uint32_t >() );
    }
    else {
        xbits->setPassBits( std::vector< uint32_t >( ( ( passbits->size() - 1 ) / 32 ) + 1 ) );
    }
    for ( size_t b = 0, bmax = passbits->size(); b < bmax; ++b ) {
        xbits->markPassing(b, passbits->isPassing(b));
    }
    return xbits;

}

template<class CONT> 
std::unique_ptr< xAOD::TrigPassBits> TrigEgammaAnalysisBaseTool::getBits(const HLT::TriggerElement* te,const CONT* container){
    // For conversion of 2015/2016 data
    const TrigPassBits *bits = getFeature<TrigPassBits>(te);
    if(bits){
        ATH_MSG_DEBUG("Run1 conversion");
        ATH_MSG_DEBUG("create bits AOD to xAOD");
        return createBits<CONT,TrigPassBits>(container,bits);
    }
    // Workaround, see above
    const xAOD::TrigPassBits *xbits=getFeature<xAOD::TrigPassBits>(te);
    if(xbits){
        ATH_MSG_DEBUG("create bits xAOD to xAOD");
        return createBits<CONT,xAOD::TrigPassBits>(container,xbits); 
    }
    else
        return nullptr;
}

template<class T>
const T* TrigEgammaAnalysisBaseTool::getFeature(const HLT::TriggerElement* te,const std::string key){
    if ( te == nullptr ) return nullptr;
    if ( (m_trigdec->ancestor<T>(te,key)).te() == nullptr )
        return nullptr;
    return ( (m_trigdec->ancestor<T>(te,key)).cptr() );
}

template<class T>
bool TrigEgammaAnalysisBaseTool::ancestorPassed(const HLT::TriggerElement* te,const std::string key){
    if ( te == nullptr ) return false;
    if ( (m_trigdec->ancestor<T>(te,key)).te() == nullptr )
        return false;
    return ( (m_trigdec->ancestor<T>(te,key)).te()->getActiveState());
}

template <class T1, class T2>
const T1* TrigEgammaAnalysisBaseTool::closestObject(const std::pair<const xAOD::Egamma *,const HLT::TriggerElement *> pairObj, float &dRmax,bool usePassbits,const std::string key){
    float eta = pairObj.first->eta();
    float phi = pairObj.first->phi();
    // Reset to resonable start value
    if(dRmax < 0.15) dRmax = 0.15;
    const auto *cont=getFeature<T2>(pairObj.second,key);
    if(cont==nullptr) return nullptr;
    std::unique_ptr<xAOD::TrigPassBits> bits = getBits<T2>(pairObj.second,cont);
    
    if(usePassbits){ 
        if(bits==nullptr) return nullptr;
    }
    const T1 *cl = nullptr;
    float dr=0.; 
    for(const auto& obj : *cont){
        if( usePassbits && !bits->isPassing(obj,cont) ) continue; 
        if(obj==nullptr) continue;
        dr=dR(eta,phi,obj->eta(),obj->phi());
        if ( dr<dRmax){
            dRmax=dr;
            cl = obj;
        } // dR
    }
    return cl;
}


#endif
