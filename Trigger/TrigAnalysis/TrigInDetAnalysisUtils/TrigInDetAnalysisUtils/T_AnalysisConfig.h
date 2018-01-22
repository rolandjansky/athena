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


// #include "TrigInDetAnalysisUtils/RoI.h"
// #include "TrigInDetAnalysisUtils/Converter.h"
#include "TrigInDetAnalysisUtils/TrigTrackSelector.h"
#include "TrigInDetAnalysisUtils/OfflineObjectSelection.h"

#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"

#ifdef XAODTRACKING_TRACKPARTICLE_H
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODTau/TauJetContainer.h"
#else
#include "muonEvent/MuonContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "tauEvent/TauJetContainer.h"
#endif

class MsgSvc;
class StoreGateSvc;

// class TrackAssociator;
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
    m_beamX(0),
    m_beamY(0),
    m_beamZ(0),
    m_genericFlag(true),
    m_releaseData(""),
    m_keepAllEvents(false),
    m_useHighestPT(false),
    m_vtxIndex(-1),
    m_filterOnRoi(true),
    m_requireDecision(false)
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
    m_releaseData(""),
    m_keepAllEvents(false),
    m_useHighestPT(false),
    m_vtxIndex(-1),
    m_filterOnRoi(true),
    m_requireDecision(false)
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

      // m_selectorRef  = new TrigTrackSelector( referenceFilter );
      // m_selectorTest = new TrigTrackSelector( testFilter );
  }





  // Destructor
  virtual ~T_AnalysisConfig() {
      // if ( m_selectorRef  ) delete m_selectorRef;
      // if ( m_selectorTest ) delete m_selectorTest;
      // if ( m_selectorSel  ) delete m_selectorSel;
      // if ( m_analysis     ) delete m_analysis;
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
  virtual void initialize(Provider* p, ToolHandle<Trig::TrigDecisionTool>* tdt ) { 
      m_provider = p;
      m_tdt      = tdt;
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
  bool mcTruth() const         { return m_mcTruth; }

  void   setBeamX(double d) { m_beamX=d; }
  void   setBeamY(double d) { m_beamY=d; }
  void   setBeamZ(double d) { m_beamZ=d; }
  double beamX() const      { return m_beamX; }
  double beamY() const      { return m_beamY; }
  double beamZ() const      { return m_beamZ; }

  bool genericFlag() const    { return m_genericFlag; }
  void setGenericFlag(bool b) { m_genericFlag=b; }

  std::string releaseData() const                { return m_releaseData; }
  void releaseData(const std::string& s)  { m_releaseData = s; }

  void keepAllEvents( bool b ) { m_keepAllEvents = b; }

  void setUseHighestPT( bool b )    { m_useHighestPT=b; } 
  bool getUseHighestPT()      const { return m_useHighestPT; } 

  void setVtxIndex( int i )    { m_vtxIndex=i; } 
  int  getVtxIndex()     const { return m_vtxIndex; } 

  bool filterOnRoi()          const { return m_filterOnRoi; }
  bool setFilterOnRoi(bool b)       { return m_filterOnRoi=b; }

  void setRequireDecision(bool b) { m_requireDecision=b; } 
  bool requireDecision() const    { return m_requireDecision; } 
  
protected:

  virtual void loop() = 0;


  template<class Collection>
  bool selectTracks( TrigTrackSelector* selector, Trig::FeatureContainer::combination_const_iterator citr,  const std::string& key="" ) {

    //    std::cout << "try " << key << "\t" << m_provider->evtStore()->template transientContains<Collection>(key) << std::endl;

    std::vector< Trig::Feature<Collection> >  trackcollections = citr->get<Collection>( key, TrigDefs::alsoDeactivateTEs );
    if ( !trackcollections.empty() ) {
      // NB!! a combination should never have more than one entry for a track collection from a single algorithm, for single object triggers
      for ( unsigned ifeat=0 ; ifeat<trackcollections.size() ; ifeat++ ) {
	Trig::Feature<Collection> trackfeature = trackcollections.at(ifeat);
	if ( !trackfeature.empty() ) {
	  // actually select the tracks from this roi at last!!
	  const Collection* trigtracks = trackfeature.cptr();
	  selector->selectTracks( trigtracks );
	}
      }
      return true;
    }
    else {
      m_provider->msg(MSG::DEBUG) << "TDT TrackFeature collection (" << key << ") is empty " << endmsg;
      return false;
    }
  }


  /// NB: because we want to use this for the L2Star chains, we have to use this method, *not* the above
  ///     method without the TruthMap argument - if we don't want/have the TruthMap, just set the pointer
  ///     to 0 - there should be abetter way to do this, but shan't worry about it now.
  template<class Collection>
  bool selectTracks( TrigTrackSelector* selector, Trig::FeatureContainer::combination_const_iterator citr, const TrigInDetTrackTruthMap* truthmap, const std::string& key="", unsigned index=0 ) {

    //    std::cout << "try " << key << "\t" << m_provider->evtStore()->template transientContains<Collection>(key) << std::endl;

    std::vector< Trig::Feature<Collection> >  trackcollections = citr->get<Collection>( key, TrigDefs::alsoDeactivateTEs );
    if ( !trackcollections.empty() ) {

      Trig::ExpertMethods* em = (*m_tdt)->ExperimentalAndExpertMethods();
      em->enable();
      const HLT::NavigationCore* nc = em->getNavigation();

      // NB!! a combination should never have more than one entry for a track collection from a single algorithm,
      //   if ( trackcollections.size()>1 ) std::cerr << "SUTT OH NO!!!!!!!!" << endmsg;
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
	  // m_provider->msg(MSG::DEBUG) << "TDT TrackFeature->size() " << trackfeature.cptr()->size() << " (" << key << ")" << endmsg;
	  // actually select the tracks from this roi at last!!
	  //	  const Collection* trigtracks = trackfeature.cptr();
	  //	  selector->selectTracks( trigtracks, truthmap );
	  // m_provider->msg(MSG::DEBUG) << "TDT TrackFeature->size() " << collectionVector[iv]->size() << " (" << key << ")" << endmsg;
	  selector->selectTracks( collectionVector[iv], truthmap );
	}
      }
      return true;
    }
    else {
      m_provider->msg(MSG::DEBUG) << "TDT TrackFeature collection (" << key << ") is empty" << endmsg;
      return false;
    }
  }




  template<class Collection>
  bool selectTracks( TrigTrackSelector* selector, const std::string& key ) {
    const Collection* collection = 0;
    if ( key!="" ) {
      if ( m_provider->evtStore()->template contains<Collection>( key ) ) {
	StatusCode sc = m_provider->evtStore()->retrieve( collection, key );
	m_provider->msg(MSG::DEBUG) << "SG Collection->size() " << collection->size() << " (" << key << ")" << endmsg;
	if( sc.isSuccess() && collection ) {
	  selector->selectTracks( collection );
	  return true;
	}
      }
    }
    return false;
  }








  template<class Collection>
  std::vector<double>  getBeamspot( const std::string& key ) {
    const Collection* collection = 0;
    std::vector<double> v;

    if ( key!="" ) {
      if ( m_provider->evtStore()->template contains<Collection>( key ) ) {
	StatusCode sc = m_provider->evtStore()->retrieve( collection, key );
	m_provider->msg(MSG::DEBUG) << "SG Collection->size() " << collection->size() << " (" << key << ")" << endmsg;
	if( sc.isSuccess() && collection ) {

	  typename Collection::const_iterator  trackitr = collection->begin();
	  typename Collection::const_iterator  trackend = collection->end();
	  if ( trackitr!=trackend ) {
	    v.resize(3);
	    v[0] = (*trackitr)->vx();
	    v[1] = (*trackitr)->vy();
	    v[2] = (*trackitr)->vz();
	    return v;
	  } // only need to look at the first track
	}
      }
    }
    return v;
  }




  template<class Collection>
  std::vector<double> getBeamspot( Trig::FeatureContainer::combination_const_iterator citr,  const std::string& key="" ) {
    std::vector< Trig::Feature<Collection> >  trackcollections = citr->get<Collection>( key, TrigDefs::alsoDeactivateTEs );
    std::vector<double> v;
    if ( !trackcollections.empty() ) {
      // NB!! a combination should never have more than one entry for a track collection from a single algorithm,                                                                                                     
      //   if ( trackcollections.size()>1 ) std::cerr << "SUTT OH NO!!!!!!!!" << endmsg;                                                                                                                              
      for ( unsigned ifeat=0 ; ifeat<trackcollections.size() ; ifeat++ ) {
        //      std::cout << "selectTracks() ifeat=" << ifeat << "\tkey " << key << std::endl;                                                                                                                        
	Trig::Feature<Collection> trackfeature = trackcollections.at(ifeat);
        if ( !trackfeature.empty() ) {
          //      m_provider->msg(MSG::DEBUG) << "TDT TrackFeature->size() " << trackfeature.cptr()->size() << " (" << key << ")" << endmsg;                                                                          
          // actually select the tracks from this roi at last!!                                                                                                                                                       
          const Collection* trigtracks = trackfeature.cptr();

	  typename Collection::const_iterator  trackitr = trigtracks->begin();
	  typename Collection::const_iterator  trackend = trigtracks->end();
          if ( trackitr!=trackend ) {
            v.resize(3);
            v[0] = (*trackitr)->vx();
            v[1] = (*trackitr)->vy();
            v[2] = (*trackitr)->vz();
            return v;
	  } // only need to look at the first track
        }
      }
      return v;
    }
    else {
      m_provider->msg(MSG::DEBUG) << "TDT TrackFeature collection (" << key << ") is empty " << endmsg;
      return v;
    }
  }





  ////////////////////////////////////////////////////////////////////////////////////////////
  /// select offline electrons
  ////////////////////////////////////////////////////////////////////////////////////////////
  unsigned processElectrons( TrigTrackSelector& selectorRef, const unsigned int selection=0, 
			     bool   raw_track=false,  
			     double ETOffline=0,
#                            ifdef XAODTRACKING_TRACKPARTICLE_H
			     const std::string& containerName = "Electrons"
#                            else
			     const std::string& containerName = "ElectronAODCollection"
#                            endif
			     )  {

    m_provider->msg(MSG::INFO) << " Fetching offline electrons: " << containerName << endmsg;

    selectorRef.clear();

#   ifdef XAODTRACKING_TRACKPARTICLE_H
    typedef xAOD::ElectronContainer     Container;
#   else
    typedef ElectronContainer           Container;
#   endif


    const Container* container = 0;
    
    if( ! m_provider->evtStore()->template contains<Container>(containerName) ) {
      m_provider->msg(MSG::WARNING) << "Error No Electron Container " << containerName << " !" << endmsg;
      return 0;
    }
    
    StatusCode sc=m_provider->evtStore()->retrieve( container, containerName);
    if( sc.isFailure() || !container ) {
      m_provider->msg(MSG::WARNING) << "Error retrieving container: " << containerName << " !" << endmsg;
      return 0;
    }

    m_provider->msg(MSG::INFO) << "Event with " <<  container->size() << " Electron object(s) " << endmsg;

    Container::const_iterator elec     = container->begin();
    Container::const_iterator elec_end = container->end();

    for( ; elec!=elec_end ; ++elec ){
      //m_provider->msg(MSG::DEBUG) << " Electron "       << (*elec)
      //	       << ",  eta "          << (*elec)->eta()
      //	       << ",  phi "          << (*elec)->phi()
      //	       << ",  ET "           << (*elec)->pt()
      //	       << ",  author "       << (*elec)->author()
      //	       << ",  trackParticle " << (*elec)->trackParticle()
      //	       << ",  conversion "    << (*elec)->conversion()
      //	       << ",  mediumPP "      << ((*elec)->isem(egammaPID::ElectronMediumPP)==0)
      //	       << endmsg;

      bool good_electron = false;
#     ifdef XAODTRACKING_TRACKPARTICLE_H
      good_electron = TIDA::isGoodOffline( *(*elec), selection, ETOffline );
#     else
      good_electron = TIDA::isGoodOffline( *(*elec));
#     endif

      if (good_electron) { 
	if ( raw_track ) selectorRef.selectTrack( xAOD::EgammaHelpers::getOriginalTrackParticle( *elec ) );
	else             selectorRef.selectTrack( (*elec)->trackParticle() );
      }
    }

    return selectorRef.tracks().size();
  }



  ////////////////////////////////////////////////////////////////////////////////////////////
  /// select offline muons
  ////////////////////////////////////////////////////////////////////////////////////////////
  unsigned processMuons(     TrigTrackSelector& selectorRef,  const unsigned int selection=0, 
			     double ETOffline=0,
#                            ifdef XAODTRACKING_TRACKPARTICLE_H
                             const std::string& containerName = "Muons"
#                            else
                             const std::string& containerName = "StacoMuonCollection"
#                            endif
			     )  {

#   ifdef XAODTRACKING_TRACKPARTICLE_H
    typedef xAOD::MuonContainer     Container;
#   else
    typedef Analysis::MuonContainer Container;
#   endif

    m_provider->msg(MSG::DEBUG) << " Offline muons (" << containerName << ")" << endmsg;

    selectorRef.clear();

    const Container*     container = 0;

    if( ! m_provider->evtStore()->template contains<Container>(containerName) ) {
      m_provider->msg(MSG::WARNING) << "Error No MuonCollection" << containerName << " !" << endmsg;
      return 0;
    }

    StatusCode sc=m_provider->evtStore()->retrieve( container, containerName );
    if( sc.isFailure() || !container ) {
      m_provider->msg(MSG::WARNING) << "Error retrieving " << containerName << " !" << endmsg;
      return 0;
    }

    auto muon     = container->begin();
    auto muon_end = container->end();

    //    std::cout << "SUTT Offline muons " << container->size() << "\t threshold " << ETOffline << std::endl;

    for( ; muon!=muon_end ; ++muon ){
#     ifdef XAODTRACKING_TRACKPARTICLE_H
      if ( TIDA::isGoodOffline(*(*muon), selection, ETOffline ) ) selectorRef.selectTrack(*((*muon)->inDetTrackParticleLink()));
#     else
      if ( TIDA::isGoodOffline(*(*muon)) ) selectorRef.selectTrack((*muon)->inDetTrackParticle());
#     endif
    }


    //    std::cout <<              "SUTT found  " << selectorRef.tracks().size() << " muons for " << containerName << std::endl;
    m_provider->msg(MSG::DEBUG) << "found  " << selectorRef.tracks().size() << " muons for " << containerName << endmsg;

    return selectorRef.tracks().size();
}



    ////////////////////////////////////////////////////////////////////////////////////////////
    /// select offline taus
    ////////////////////////////////////////////////////////////////////////////////////////////
unsigned processTaus( TrigTrackSelector& selectorRef,
		      const unsigned selection=0,
		      int            requireNtracks=0,
		      double         EtCutOffline=0,
#                     ifdef XAODTRACKING_TRACKPARTICLE_H
		      const std::string& containerName = "TauJets"
#                     else
		      const std::string& containerName = "TauRecContainer"
#                     endif
			   ) {
  
# ifdef XAODTRACKING_TRACKPARTICLE_H
  typedef xAOD::TauJetContainer     Container; 
# else
  typedef Analysis::TauJetContainer Container;
# endif

  selectorRef.clear();

  const Container* container = 0;

  selectorRef.clear();

  m_provider->msg(MSG::DEBUG) << " Offline taus " << containerName << endmsg;

  if ( !m_provider->evtStore()->template contains<Container>(containerName)) {
    m_provider->msg(MSG::WARNING) << " Offline taus not found" << endmsg;
    return 0;
  }
  

  StatusCode sc = m_provider->evtStore()->retrieve( container, containerName);
  if (sc != StatusCode::SUCCESS) {
    m_provider->msg(MSG::WARNING) << " Offline tau retrieval not successful" << endmsg;
    return 0;
  }

  Container::const_iterator tau     = container->begin();
  Container::const_iterator tau_end = container->end();

  unsigned Ntaus = 0;

  for ( ; tau!=tau_end ; ++tau ) {

#   ifdef XAODTRACKING_TRACKPARTICLE_H
    //      unsigned N = (*tau)->nTracks();

#   ifndef XAODTAU_VERSIONS_TAUJET_V3_H
    int N = (*tau)->nTracks();
    // std::cout << "SUTT no tau detail " << N << "\t3prong: " << doThreeProng << std::endl;
#   else
    int N=0;
    (*tau)->detail( xAOD::TauJetParameters::nChargedTracks, N );
    // std::cout << "SUTT tau detail: N " << N << "\t3prong: " << doThreeProng << std::endl;
#   endif

#   else
    unsigned N = (*tau)->numTrack(); 
#   endif


    bool good_tau = false;
#   ifdef XAODTRACKING_TRACKPARTICLE_H
    good_tau = TIDA::isGoodOffline( *(*tau), selection, requireNtracks, EtCutOffline );
#   else
    good_tau = TIDA::isGoodOffline( *(*tau), requireNtracks, EtCutOffline );
#   endif

    //   std::cout << "SUTT tau ntracks: " << N << "\tgoodtau: " << good_tau << "\tpt: " << (*tau)->p4().Et() << "\t3prong: " << doThreeProng << std::endl;  
      
    if (good_tau){
      for ( unsigned i=N ; i-- ; )  {
#       ifdef XAODTAU_TAUTRACK_H  
        selectorRef.selectTrack((*tau)->track(i)->track());
#       else
        selectorRef.selectTrack((*tau)->track(i));
#       endif
        Ntaus++;
      }
    }
  }

  return selectorRef.tracks().size();

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

  bool                   m_keepAllEvents;

  bool                   m_useHighestPT;
  
  int                    m_vtxIndex;

  bool                   m_filterOnRoi;

  bool                   m_requireDecision;

};

//}


#endif  // TrigInDetAnalysisUtils_T_AnalysisConfig_H

