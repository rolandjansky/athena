// emacs: this is -*- c++ -*-
/** @file AnalysisConfig.h */

#ifndef TrigInDetAnalysisUtils_T_AnalysisConfig_H
#define TrigInDetAnalysisUtils_T_AnalysisConfig_H

#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
// #include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/TrackAnalysis.h"


#include "TrigInDetAnalysisUtils/RoI.h"
// #include "TrigInDetAnalysisUtils/Converter.h"
#include "TrigInDetAnalysisUtils/TrigTrackSelector.h"

#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"




class MsgSvc;
class StoreGateSvc;

class TrackAssociator;
// class Converter;

  // class RoI;
  // class TrackFilter;

template<class Provider>
class T_AnalysisConfig {
  
public:
  
  // Full constructor: test/reference/selection
  // - analysisInstanceName: the name of the analysis chain being created
  // - xxxChainName: the name of the chain to be used as test/reference/selection; must be "StoreGate" in case of direct access to SG containers
  // - xxxType: the type of tracks to be retrieved from the test/reference/selection chain or container
  // - xxxKey:  the key for tracks to be retrieved from the test/reference/selection chain or container
  // - roiInfo: in case the test chain is a real chain, this is used to specify RoI widths; in case the test chain is a fake chain, this is used for RoI position too
  // - all standard operations are performed in loops over 0=test 1=reference 2=selection
  T_AnalysisConfig(const std::string& analysisInstanceName,
		   const std::string& testChainName,      const std::string& testType,      const std::string& testKey,
		   const std::string& referenceChainName, const std::string& referenceType, const std::string& referenceKey,
		   const std::string& selectionChainName, const std::string& selectionType, const std::string& selectionKey,
		   TIDARoiDescriptor* roiInfo,
		   TrackFilter* testFilter, TrackFilter* referenceFilter, TrackFilter* selectionFilter,
		   TrackAssociator* associator,
		   TrackAnalysis* analysis) :
    m_provider(0),
    m_analysisInstanceName(analysisInstanceName),
    m_refChainName(referenceChainName),
    m_refChainKey(referenceKey),
    m_testChainName(testChainName),
    m_testChainKey(testKey),
    m_roiInfo(roiInfo),
    m_selectorRef(0),
    m_selectorTest(0),
    m_selectorSel(0),
    m_associator(associator),
    m_analysis(analysis),
    m_mcTruth(false),
    m_genericFlag(true),
    m_releaseData("")
  {
      // Rearrange objects in vectors: chain names
      std::vector<std::string> testChainNames; testChainNames.push_back(testChainName);
      std::vector<std::string> referenceChainNames; referenceChainNames.push_back(referenceChainName);
      std::vector<std::string> selectionChainNames; selectionChainNames.push_back(selectionChainName);
      m_chainNames.push_back(testChainNames); m_chainNames.push_back(referenceChainNames); m_chainNames.push_back(selectionChainNames);
      // Types
      std::vector<std::string> testTypes; testTypes.push_back(testType);
      std::vector<std::string> referenceTypes; referenceTypes.push_back(referenceType);
      std::vector<std::string> selectionTypes; selectionTypes.push_back(selectionType);
      m_types.push_back(testTypes); m_types.push_back(referenceTypes); m_types.push_back(selectionTypes);
      // Keys
      std::vector<std::string> testKeys; testKeys.push_back(testKey);
      std::vector<std::string> referenceKeys; referenceKeys.push_back(referenceKey);
      std::vector<std::string> selectionKeys; selectionKeys.push_back(selectionKey);
      m_keys.push_back(testKeys); m_keys.push_back(referenceKeys); m_keys.push_back(selectionKeys);
      // Filters
      std::vector<TrackFilter*> testFilters; testFilters.push_back(testFilter);
      std::vector<TrackFilter*> referenceFilters; referenceFilters.push_back(referenceFilter);
      std::vector<TrackFilter*> selectionFilters; selectionFilters.push_back(selectionFilter);
      m_filters.push_back(testFilters); m_filters.push_back(referenceFilters); m_filters.push_back(selectionFilters);
  }


  // Partial constructor: test/reference
  // - analysisInstanceName: the name of the analysis chain being created
  // - xxxChainName: the name of the chain to be used as test/reference; must be "StoreGate" in case of direct access to SG containers
  // - xxxType: the type of tracks to be retrieved from the test/reference chain or container
  // - xxxKey:  the key for tracks to be retrieved from the test/reference chain or container
  // - roiInfo: in case the test chain is a real chain, this is used to specify RoI widths; in case the test chain is a fake chain, this is used for RoI position too
  // - all standard operations are performed in loops over 0=test 1=reference 2=selection
  T_AnalysisConfig(const std::string& analysisInstanceName,
		   const std::string& testChainName,      const std::string& testType,      const std::string& testKey,
		   const std::string& referenceChainName, const std::string& referenceType, const std::string& referenceKey,
		   TIDARoiDescriptor* roiInfo,
		   TrackFilter* testFilter, TrackFilter* referenceFilter,
		   TrackAssociator* associator,
		   TrackAnalysis* analysis) :
    m_provider(0),
    m_analysisInstanceName(analysisInstanceName),
    m_refChainName(referenceChainName),
    m_refChainKey(referenceKey),
    m_testChainName(testChainName),
    m_testChainKey(testKey),
    m_roiInfo(roiInfo),
    m_selectorRef(0),
    m_selectorTest(0),
    m_selectorSel(0),
    m_associator(associator),
    m_analysis(analysis),
    m_mcTruth(false),
    m_beamX(0),
    m_beamY(0),
    m_beamZ(0),
    m_genericFlag(true),
    m_releaseData("")
  {
      // Rearrange objects in vectors: chain names
      std::vector<std::string> testChainNames; testChainNames.push_back(testChainName);
      std::vector<std::string> referenceChainNames; referenceChainNames.push_back(referenceChainName);
      std::vector<std::string> selectionChainNames; selectionChainNames.push_back("NONE");
      m_chainNames.push_back(testChainNames); m_chainNames.push_back(referenceChainNames); m_chainNames.push_back(selectionChainNames);
      // Types
      std::vector<std::string> testTypes; testTypes.push_back(testType);
      std::vector<std::string> referenceTypes; referenceTypes.push_back(referenceType);
      std::vector<std::string> selectionTypes; selectionTypes.push_back("");
      m_types.push_back(testTypes); m_types.push_back(referenceTypes); m_types.push_back(selectionTypes);
      // Keys
      std::vector<std::string> testKeys; testKeys.push_back(testKey);
      std::vector<std::string> referenceKeys; referenceKeys.push_back(referenceKey);
      std::vector<std::string> selectionKeys; selectionKeys.push_back("");
      m_keys.push_back(testKeys); m_keys.push_back(referenceKeys); m_keys.push_back(selectionKeys);
      // Filters
      std::vector<TrackFilter*> testFilters; testFilters.push_back(testFilter);
      std::vector<TrackFilter*> referenceFilters; referenceFilters.push_back(referenceFilter);
      std::vector<TrackFilter*> selectionFilters;
      m_filters.push_back(testFilters); m_filters.push_back(referenceFilters); m_filters.push_back(selectionFilters);

      m_selectorRef  = new TrigTrackSelector( referenceFilter );
      m_selectorTest = new TrigTrackSelector( testFilter );
  }





  // Destructor
  virtual ~T_AnalysisConfig() {
      if ( m_selectorRef  ) delete m_selectorRef;
      if ( m_selectorTest ) delete m_selectorTest;
      if ( m_selectorSel  ) delete m_selectorSel;
      if ( m_analysis     ) delete m_analysis;
  }



  // Configuration methods
  void addTestChain(const std::string& chainName, const std::string& type, const std::string& key) {
      m_chainNames[0].push_back(chainName); m_types[0].push_back(type); m_keys[0].push_back(key);
  }

  void addReferenceChain(const std::string& chainName, const std::string& type, const std::string& key) {
      m_chainNames[1].push_back(chainName); m_types[1].push_back(type); m_keys[1].push_back(key);
  }

  void addSelectionChain(const std::string& chainName, const std::string& type, const std::string& key) {
      m_chainNames[2].push_back(chainName); m_types[2].push_back(type); m_keys[2].push_back(key);
  }
    
  void addTestFilter(TrackFilter* filter)      { m_filters[0].push_back(filter); }
  void addReferenceFilter(TrackFilter* filter) { m_filters[1].push_back(filter); }
  void addSelectionFilter(TrackFilter* filter) { m_filters[2].push_back(filter); }

  // Initialize, execute and finalize generic methods
  //    void initialize(MsgStream* msg, StoreGateSvc* sg, ToolHandle<Trig::TrigDecisionTool>* tdt) {
  virtual void initialize(Provider* p, ToolHandle<Trig::TrigDecisionTool>* tdt) {
      m_provider = p;
      m_tdt=tdt;
      if ( m_tdt==0 ) m_analysis->initialise();
  }
  
    

  virtual void execute() { 
      if ( !m_provider ) std::cerr << "ERROR T_AnalysisConfig::execute() called without initialising" << std::endl;
      loop();
  }
  
  virtual void finalize() { m_analysis->finalise(); }
  
  virtual void book() { }

  // Accessor methods
  const std::string name() const {return m_analysisInstanceName;}

  const std::string testChains() const {
      std::string name;
      for(unsigned int i=0; i<m_chainNames[0].size(); i++)
	name += (m_chainNames[0][i]+"::"+m_types[0][i]+"::"+m_keys[0][i]+" ");
      return name;
  }
 
  const std::string referenceChains() const {
      std::string name;
      for(unsigned int i=0; i<m_chainNames[1].size(); i++)
	name += (m_chainNames[1][i]+"::"+m_types[1][i]+"::"+m_keys[1][i]+" ");
      return name;
  }
  
  const std::string selectionChains() const {
      std::string name;
      for(unsigned int i=0; i<m_chainNames[2].size(); i++)
	name += (m_chainNames[2][i]+"::"+m_types[2][i]+"::"+m_keys[2][i]+" ");
      return name;
  }

  const TrackAnalysis* analysis() const {return m_analysis;}

  void setMCTruth(bool b=true) { m_mcTruth=b; }
  void mcTruth() const         { return m_mcTruth; }
  
  void   setBeamX(double d) { m_beamX=d; }
  void   setBeamY(double d) { m_beamY=d; }
  void   setBeamZ(double d) { m_beamZ=d; }
  double beamX() const      { return m_beamX; }
  double beamY() const      { return m_beamY; }
  double beamZ() const      { return m_beamZ; }
  
  bool genericFlag() const    { return m_genericFlag; }
  void setGenericFlag(bool b) { m_genericFlag=b; }  

  void releaseData() const                { return m_releaseData; } 
  void releaseData(const std::string& s)  { m_releaseData = s; } 

protected:

  virtual void loop() = 0;

  void filterTracks(std::vector< std::vector<TrigInDetAnalysis::Track*> >& tracks, RoI* roi) { 

    // Filter tracks
    // - creates a temporary filtered track vector
    // - loop is performed over tracks to be filtered, each is filtered with all filters and copied if passes all of them
    std::vector< std::vector<TrigInDetAnalysis::Track*> > filteredTracks(3);
    for(unsigned int i=0; i<3; i++) {
      
      // Loop over tracks to be filtered
      std::vector<TrigInDetAnalysis::Track*>::iterator track(tracks[i].begin());
      std::vector<TrigInDetAnalysis::Track*>::iterator trackEnd(tracks[i].end());
      while ( track!=trackEnd ) { 
	
	// Loop over filters and apply them to current track
	bool passes=true;
	for(unsigned int j=0; j<(m_filters[i]).size(); j++) passes = passes && (((m_filters[i])[j])->select(*track, roi->descriptor()));
	
	// Check if track passes all filters
	if(passes) filteredTracks[i].push_back(*track);

	track++;
      }
      
      // Associate filtered tracks to the RoI
      roi->tracks(filteredTracks[i], i);
    } 
  }

  
  //   template<class Collection> 
  //   void readStoreGateTracks(const std::string& key) {
  //     const Collection* collection=0;
  //     StatusCode sc = m_provider->evtStore()->retrieve(collection, key);
  //     if(sc.isSuccess() && collection) m_converter->selectTracks(collection);
  //     else m_provider->msg(MSG::ERROR) << "unavailable track SG key " << key << endreq;
  //   }
  
  
    
  // template<class Collection> 
  //   bool readFeatureTracks(Trig::FeatureContainer::combination_const_iterator c,  const std::string& key="") {   
  //     std::vector<Trig::Feature<Collection> > trackcollections=c->get<Collection>(key, TrigDefs::alsoDeactivateTEs);
  //     if(!trackcollections.empty()) { 
  //       // NB!! a combination should never have more than one entry for a track collection from a single algorithm chain 
  //       for(unsigned ifeat=0; ifeat<trackcollections.size(); ifeat++) {
  // 	std::cout << "readFeatureTracks() ifeat=" << ifeat << "\t(Carlos I beleieve)" << std::endl;    
  // 	Trig::Feature<Collection> trackfeature = trackcollections.at(ifeat);
  // 	if(!trackfeature.empty()) { 
  // 	  // Select the tracks
  // 	  const Collection* trigtracks = trackfeature.cptr();
  // 	  m_converter->selectTracks(trigtracks);
  // 	}
  //       }
  //       return true;
  //     }
  //     else {
  //       m_provider->msg(MSG::DEBUG) << "TDT TrackFeature collection is empty " << key << endreq;
  //       return false;
  //     }
  //   }


  

  template<class Collection> 
  bool selectTracks( TrigTrackSelector* selector, Trig::FeatureContainer::combination_const_iterator citr,  const std::string& key="" ) {   
    std::vector< Trig::Feature<Collection> >  trackcollections = citr->get<Collection>( key, TrigDefs::alsoDeactivateTEs );
    if ( !trackcollections.empty() ) { 
      // NB!! a combination should never have more than one entry for a track collection from a single algorithm, 
      //   if ( trackcollections.size()>1 ) std::cerr << "SUTT OH NO!!!!!!!!" << endreq;
      for ( unsigned ifeat=0 ; ifeat<trackcollections.size() ; ifeat++ ) { 
	//	std::cout << "selectTracks() ifeat=" << ifeat << "\tkey " << key << std::endl;    
	Trig::Feature<Collection> trackfeature = trackcollections.at(ifeat);
	if ( !trackfeature.empty() ) { 
	  //	  m_provider->msg(MSG::DEBUG) << "TDT TrackFeature->size() " << trackfeature.cptr()->size() << " (" << key << ")" << endreq;
	  // actually select the tracks from this roi at last!!
	  const Collection* trigtracks = trackfeature.cptr();
	  selector->selectTracks( trigtracks );
	}
      }
      return true;
    }
    else {
      m_provider->msg(MSG::DEBUG) << "TDT TrackFeature collection is empty " << endreq;
      return false;
    }     
  }


  /// NB: because we want to use this for the L2Star chains, we have to use this method, *not* the above 
  ///     method without the TruthMap argument - if we don't want/have the TruthMap, just set the pointer 
  ///     to 0 - there should be abetter way to do this, but shan't worry about it now.  
  template<class Collection> 
  bool selectTracks( TrigTrackSelector* selector, Trig::FeatureContainer::combination_const_iterator citr, const TrigInDetTrackTruthMap* truthmap, const std::string& key="", unsigned index=0 ) {   
    std::vector< Trig::Feature<Collection> >  trackcollections = citr->get<Collection>( key, TrigDefs::alsoDeactivateTEs );
    if ( !trackcollections.empty() ) { 

      Trig::ExpertMethods* em = (*m_tdt)->ExperimentalAndExpertMethods();
      em->enable();
      const HLT::NavigationCore* nc = em->getNavigation();
      
      // NB!! a combination should never have more than one entry for a track collection from a single algorithm, 
      //   if ( trackcollections.size()>1 ) std::cerr << "SUTT OH NO!!!!!!!!" << endreq;
      //   maybe a bit dodgy, if we really do have multiple objects returned, but that should only be for 
      //   multiple object triggers - then probably none of this would work anyhow 
      for ( unsigned ifeat=0 ; ifeat<trackcollections.size() ; ifeat++ ) { 
	//	std::cout << "selectTracks() ifeat=" << ifeat << "\tkey " << key << "\t(truthmap)" << std::endl;    
	Trig::Feature<Collection> trackfeature = trackcollections.at(ifeat);

	/// get the correspondiong TE
	const HLT::TriggerElement* te = trackfeature.te();

	/// get all the named collections attached to the TE
	std::vector< const Collection* > collectionVector;
	if ( !const_cast<HLT::NavigationCore *>(nc)->getFeatures( te, collectionVector, key ) ) return false;

	//	std::cout << "foundok " << foundok << "\tsize " << collectionVector.size() << std::endl;

	/// now got the collectionVector, can navigate through this

	for ( unsigned iv=0 ; iv<collectionVector.size() ; iv++ ) {
	  /// NB: this is safer than using index as an indev, ie collectionVector[index] 
	  ///     since it will do nothing if index is out of range
	  if ( index!=iv ) continue; 
	  //	if ( !trackfeature.empty() ) { 
	  // m_provider->msg(MSG::DEBUG) << "TDT TrackFeature->size() " << trackfeature.cptr()->size() << " (" << key << ")" << endreq;
	  // actually select the tracks from this roi at last!!
	  //	  const Collection* trigtracks = trackfeature.cptr();
	  //	  selector->selectTracks( trigtracks, truthmap );
	  // m_provider->msg(MSG::DEBUG) << "TDT TrackFeature->size() " << collectionVector[iv]->size() << " (" << key << ")" << endreq;
	  selector->selectTracks( collectionVector[iv], truthmap );
	}
      }
      return true;
    }
    else {
      m_provider->msg(MSG::DEBUG) << "TDT TrackFeature collection is empty " << endreq;
      return false;
    }     
  }



  
  template<class Collection> 
  bool selectTracks( TrigTrackSelector* selector, const std::string& key ) {   
    const Collection* collection = 0;
    if ( key!="" ) { 
      StatusCode sc = m_provider->evtStore()->retrieve( collection, key );
      m_provider->msg(MSG::DEBUG) << "SG Collection->size() " << collection->size() << " (" << key << ")" << endreq;
      if( sc.isSuccess() && collection ) selector->selectTracks( collection );
    }
    return true; 
  }
  

protected:
  
  // Athena tools
  
  Provider* m_provider;
  
  //    MsgStream* m_msg;
  //    StoreGateSvc* m_sg;
  ToolHandle<Trig::TrigDecisionTool>* m_tdt;
  
  // TrigInDetAnalysis tools
  //  Converter* m_converter;
  
  // Analysis instance name
  std::string m_analysisInstanceName;
  
  // Chain info
  std::vector< std::vector<std::string> > m_chainNames;
  std::vector< std::vector<std::string> > m_types;
  std::vector< std::vector<std::string> > m_keys;
  
  std::string m_refChainName;
  std::string m_refChainKey;

  std::string m_testChainName;
  std::string m_testChainKey;
  
  
  // RoI information
  TIDARoiDescriptor* m_roiInfo;
  
  // Analysis tools
  std::vector< std::vector<TrackFilter*> > m_filters;
  
  TrigTrackSelector*     m_selectorRef;
  TrigTrackSelector*     m_selectorTest;
  TrigTrackSelector*     m_selectorSel;
  
  TrackAssociator*       m_associator;
  TrackAnalysis*         m_analysis;
  
  bool                   m_mcTruth;
  
  double                 m_beamX, m_beamY, m_beamZ;
  
  int                    m_genericFlag;
  std::string            m_releaseData;

};

//}  

  
#endif  // TrigInDetAnalysisUtils_T_AnalysisConfig_H

