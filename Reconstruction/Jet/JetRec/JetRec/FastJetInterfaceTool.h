// -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_FASTJETINTERFACETOOL_H
#define JETREC_FASTJETINTERFACETOOL_H

//#include "AthenaBaseComps/AthAlgTool.h"
#include "AsgTools/AsgTool.h"


#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/AreaDefinition.hh"

#include "JetInterface/IFastJetInterfaceTool.h"

#include <map>
#include <string>
#include <vector>
#include <stdint.h>

#ifndef XAOD_STANDALONE
// support contribs only in athena for now
#include "fastjet/SISConePlugin.hh"
#include "fastjet/CMSIterativeConePlugin.hh"
#endif 

// #include <iostream>

namespace FastJetInterface
{
  /*! @brief Mapping keyword on @c fastjet jet algorithm tag */ 
  typedef std::map<std::string,fastjet::JetAlgorithm>          algomap_t;
  /*! @brief Mapping keyword on @c fastjet clustering strategy tag */ 
  typedef std::map<std::string,fastjet::Strategy>          strategymap_t;
  /*! @brief Mapping keyword on @c fastjet recombination scheme tag */ 
  typedef std::map<std::string,fastjet::RecombinationScheme> schememap_t;
  /*! @brief Mapping keyword on @c fastjet SIS Cone split merge scale tag */
#ifndef XAOD_STANDALONE
  typedef std::map<std::string,fastjet::SISConePlugin::SplitMergeScale> splitMergeScaleMap_t;
#else
  typedef std::map<std::string,int> splitMergeScaleMap_t;
#endif
  /*! @brief Mapping keyword on @c fastjet area type tag */ 
  typedef std::map<std::string,fastjet::AreaType>              areamap_t;

  /*! @brief @c fastjet data model for jet */ 
  typedef fastjet::PseudoJet   fjet_t;
  /*! @brief Container for @c fastjet jets */
  typedef std::vector<fjet_t>  fjetlist_t;  

  // static const algomap_t&      getKnownAlgorithms();
  // static const strategymap_t&  getKnownStrategies();
  // static const schememap_t&    getKnownRecombinationSchemes();
  // static const areamap_t&      getKnownAreas();

  /*! @brief General configuration check
   *
   *  Retrieves @c fastjet tag for given key.
   *  
   *  @return @c true if tag for given key is available, else @c false
   *
   *  @param[in]  key reference to non-modifiable key
   *  @param[out] tag reference to modifiable tag data; will be set if key is
   *                  found, else unchanged
   *  @param[in]  map dictionary relating key to tag
   */
  template<class D,class M>
  static bool chkConfig(const std::string& key,D& tag,const M& map)
  {
    //    std::cout << "Looking for key <" << key << "> in " << map.size() << " entries." << std::endl;
    typename M::const_iterator fMap(map.find(key));
    if ( fMap != map.end() ) { tag = (*fMap).second; return true; }
    else { return false; } 
  }

  /*! @brief General configuration keyword finder 
   *
   *  Generalized linear search for keyword associated with given @c fastjet
   *  tag.
   *
   *  @return Reference to non-modifiable keyword if @c fastjet tag is known,
   *          else reference to non-modifiable invalid keyword indicator.
   *
   *  @param[in] tag @c fastjet tag to be checked
   *  @param[in] map dictionary mapping keyword to @c fastjet tag
   */
  template<class D,class M>
  const std::string& cfgName(D tag,const M& map,const std::string& invalidKey)
  {
    typename M::const_iterator fMap(map.begin());
    typename M::const_iterator lMap(map.end());
    while ( fMap != lMap && (*fMap).second != tag ) { ++fMap; }
    return fMap != lMap ? (*fMap).first : invalidKey; 
  }
}

/*! @namespace FastJetInterface
 * 
 *  @brief Collection of types for the internal dictionary
 */


class FastJetInterfaceTool : public asg::AsgTool, 
                             virtual public IFastJetInterfaceTool {
  
public:

  ASG_TOOL_CLASS(FastJetInterfaceTool, IFastJetInterfaceTool)

  /*! @brief Standard @c AlgTool constructor */ 
  FastJetInterfaceTool(const std::string& n);
  /*! @brief Base class destructor */
  virtual ~FastJetInterfaceTool();

  /*! @brief Initialize tool
   * 
   *  Checks properties and configure @c fastjet   
   */
  virtual StatusCode initialize();

  /*! @brief Execute method 
  *
  *  Executes the requested @c fastjet configuration.
  *
  *  @param[in]  inJets reference to non-modifiable list of input objects 
  *  @param[out] outJets reference to modifiable list of output jets
  * 
  *  @return @c StatusCode::SUCCESS independent of successful execution
  */
  virtual StatusCode execute(const fjetlist_t& inJets,fjetlist_t& outJets);
  
  /*! @brief Access @c fastjet @c ClusterSequence
   *
   *  @return Pointer to actual @c ClusterSequence object. Note that this 
   *  object will only be updated after invoking the 
   *  @c FastJetInterfaceTool::execute(...) method.
   *
   *  The object pointed to can be modified by client. 
   */
  virtual fastjet::ClusterSequence* clusterSequence();
  
  /*! @brief Access @c fastjet @c ClusterSequence
   *
   *  @return Const pointer to actual @c ClusterSequence object. Note that this 
   *  object will only be updated after invoking the 
   *  @c FastJetInterfaceTool::execute(...) method.
   *
   *  The object pointed to cannot be modified by client. 
   */
  virtual const fastjet::ClusterSequence* clusterSequence() const;

  /*! @brief Access to specific cluster sequence
   * 
   *  The template parameter @c S is expect to be set to an object type derived
   *  from the basic @c fastjet @c ClusterSequence  
   *
   *  @return Pointer to actual @c S object. Note that this 
   *  object will only be updated after invoking the 
   *  @c FastJetInterfaceTool::execute(...) method.
   *
   *  The object pointed to can be modified by client. 
   */
  template<class S>
  S* specificClusterSequence() 
  { return dynamic_cast<S*>(this->clusterSequence()); }


  /*! @brief Access to specific cluster sequence
   * 
   *  The template parameter @c S is expect to be set to an object type derived
   *  from the basic @c fastjet @c ClusterSequence  
   *
   *  @return Const pointer to actual @c S object. Note that this 
   *  object will only be updated after invoking the 
   *  @c FastJetInterfaceTool::execute(...) method.
   *
   *  The object pointed to cannot be modified by client. 
   */
  template<class S>
  const S* specificClusterSequence() const
  { return dynamic_cast<const S*>(this->clusterSequence()); }

  /*! @brief Access @c fastjet @c JetDefinition
   *
   *  @return Const pointer to actual @c JetDefinition object. 
   *
   *  The object pointed to cannot be modified by client. 
   */
  const fastjet::JetDefinition* getJetDefinition() const;
  
  /*! @brief Access @c fastjet @c AreaDefinition
   *
   *  @return Const pointer to actual @c AreaDefinition object. 
   *
   *  The object pointed to cannot be modified by client. 
   */
  const fastjet::AreaDefinition* getAreaDefinition() const;
  
  /*! @brief Access @c fastjet @c area definition type
   *
   *  @return Const string of actual @c area definition type. 
   *
   *  The string returned cannot be modified by client. 
   */
  const std::string getAreaDefinitionType() const;

  /*! @brief Get known jet algorithms */
  const FastJetInterface::algomap_t&     getKnownAlgorithms();
  /*! @brief Get known jet finding strategies */
  const FastJetInterface::strategymap_t& getKnownStrategies();
  /*! @brief Get known recombination schemes */
  const FastJetInterface::schememap_t&   getKnownRecombinationSchemes();
  /*! @brief Get known split merge scales */
  const FastJetInterface::splitMergeScaleMap_t& getKnownSplitMergeScales();
  /*! @brief Get known area types */
  const FastJetInterface::areamap_t&     getKnownAreas();

protected:

  /*! @brief @c fastjet property: cluster sequence type */
  std::string m_clusterSequenceType;
  /*! @brief @c fastjet property: jet algorithm */
  std::string m_jetAlgorithmType;
  /*! @brief @c fastjet property: cluster strategy */
  std::string m_clusterStrategyType;
  /*! @brief @c fastjet property: recombination scheme */
  std::string m_recombinationSchemeType;
  
  /*! @brief @c fastjet property for CMS Cone plugin: seed threshold */
  double m_CMS_seedThreshold;
  /*! @brief @c fastjet property for SIS Cone plugin: overlap threshold */
  double m_SIS_overlapThreshold;
  /*! @brief @c fastjet property for SIS Cone plugin: max number of passes */
  int m_SIS_nPass;
  /*! @brief @c fastjet property for SIS Cone plugin: protojet min pt */
  double m_SIS_protojetPtMin;
  /*! @brief @c fastjet property for SIS Cone plugin: do caching */
  bool m_SIS_doCaching;
  /*! @brief @c fastjet property for SIS Cone plugin: split merge scale */
  std::string m_SIS_splitMergeScale_STRING;
  /*! @brief @c fastjet property for SIS Cone plugin: split merge stop scale */
  double m_SIS_splitMergeStopScale;  

  /*! @brief Jet algorithm property: radius/distance parameter */
  double m_radius;
  /*! @brief Jet algorithm control property: inclusive/exclusive jet finding */
  bool m_inclusive;
  /*! @brief Exclusive jet finder property: @c d cut */
  double m_exclusiveDcut;
  /*! @brief Exclusive jet finder property: number of jets requested */
  int m_exclusiveNjets;
  /*! @brief Inclusive jet finder property: pTmin cut */
  double m_pTmin;

  /*! @brief Tool property: flag controls if jet area is calculated */
  bool m_doJetArea;
  /*! @brief @c fastjet property: jet area definition */
  std::string m_jetAreaDefinitionType;
  /*! @brief @c fastjet property: Voronoi area effective radius */
  double m_voronoiEffectiveRfact;
  /*! @brief @c fastjet property: ghost area rapidity limit (max) */
  double m_ghostedMaxRap;
  /*! @brief @c fastjet property: ghost area rapidity limit (min) */
  double m_ghostedMinRap;
  /*! @brief @c fastjet property: ghost area calculation repeatitions */
  int m_ghostedRepeat;
  /*! @brief @c fastjet property: size of area coverded by each ghost */
  double m_ghostedArea;
  /*! @brief @c fastjet property: scatter of ghosts on grid */ 
  double m_ghostedGridScatter;
  /*! @brief @c fastjet property: scatter of kT of ghosts */
  double m_ghostedKtScatter;
  /*! @brief @c fastjet property: average kT of ghosts */
  double m_ghostedMeanKt;
  
  /*! @brief Check configuration keyword for jet algorithm 
  *
  *  @return @c true if keyword is valid, else @c false
  * 
  *  @param[in]  key client provided keyword 
  *  @param[out] fjalg @c fastjet tag 
  */
  bool checkConfig(const std::string& key,fastjet::JetAlgorithm& fjalg);

  /*! @brief Check configuration keyword for jet clustering strategy 
  *
  *  @return @c true if keyword is valid, else @c false
  * 
  *  @param[in]  key client provided keyword 
  *  @param[out] fjstr @c fastjet tag 
  */
  bool checkConfig(const std::string& key,fastjet::Strategy& fjstr);

  /*! @brief Check configuration keyword for recombination scheme 
  *
  *  @return @c true if keyword is valid, else @c false
  * 
  *  @param[in]  key client provided keyword 
  *  @param[out] fjrs @c fastjet tag 
  */
  bool checkConfig(const std::string& key,fastjet::RecombinationScheme& fjrs);

  /*! @brief Check configuration keyword for SIS Cone split merge scale
   *
   *  @return @c true if keyword is valid, else @c false
   *
   *  @param[in] key client provided keyword
   *  @param[out] fjsms @c fastjet tag
   */
#ifndef XAOD_STANDALONE
  bool checkConfig(const std::string& key,fastjet::SISConePlugin::SplitMergeScale& fjsms);
#endif
  /*! @brief Check configuration keyword for jet area type
  *
  *  @return @c true if keyword is valid, else @c false
  * 
  *  @param[in]  key client provided keyword 
  *  @param[out] fjart @c fastjet tag 
  */
  bool checkConfig(const std::string& tag,fastjet::AreaType& fjart);

  /*! @brief Get keyword for @c fastjet configuration
   *
   *  @return Keyword if @c fastjet tag is known, else reference to "unknown" 
   * 
   *  @param[in] fjalg @c fastjet jet algorithm tag
   */
  const std::string& configName(fastjet::JetAlgorithm        fjalg);

  /*! @brief Get keyword for @c fastjet configuration
   *
   *  @return Keyword if @c fastjet tag is known, else reference to "unknown" 
   * 
   *  @param[in] fjstr @c fastjet jet clustering strategy tag
   */
  const std::string& configName(fastjet::Strategy            fjstr);

  /*! @brief Get keyword for @c fastjet configuration
   *
   *  @return Keyword if @c fastjet tag is known, else reference to "unknown" 
   * 
   *  @param[in] fjrs @c fastjet recombination scheme tag
   */
  const std::string& configName(fastjet::RecombinationScheme fjrs);
  
  /*! @brief Get keyword for @c fastjet configuration
   *
   *  @return Keyword if @c fastjet tag is known, else reference to "unknown"
   *
   *  @param[in] fjsms @c fastjet SIS Cone split merge scale tag
   */
#ifndef XAOD_STANDALONE
  const std::string& configName(fastjet::SISConePlugin::SplitMergeScale fjsms);
#endif

  /*! @brief Get keyword for @c fastjet configuration
   *
   *  @return Keyword if @c fastjet tag is known, else reference to "unknown" 
   * 
   *  @param[in] fjrs @c fastjet area type tag
   */
  const std::string& configName(fastjet::AreaType            fjart);


private:

  /*! @brief Reference for invalid @c fastjet tag */
  static std::string m_invalidKeyReference;

  /*! @brief Map of jet algorithm keywords and @c fastjet tags */
  static FastJetInterface::algomap_t     s_knownAlgorithms;
  /*! @brief Map of jet clustering strategy keywords and @c fastjet tags */
  static FastJetInterface::strategymap_t s_knownStrategies;
  /*! @brief Map of recombination scheme keywords and @c fastjet tags */
  static FastJetInterface::schememap_t   s_knownRecombinationSchemes;
  /*! @brief Map of split merge scale keywords and @c fastjet tags */
  static FastJetInterface::splitMergeScaleMap_t s_knownSplitMergeScales;
  /*! @brief Map of jet area keywords and @c fastjet tags */
  static FastJetInterface::areamap_t     s_knownAreas;

  /*! @brief Execution failure counter */
  unsigned int m_failedExecCtr;
  /*! @brief Execution failure counter reporting cut-off */
  static unsigned int m_failedExecCtrMax;

  


protected:
  
  /*! @brief @c fastjet tag: jet algorithm */
  fastjet::JetAlgorithm        m_jetAlgorithm;
  /*! @brief @c fastjet tag: jet clustering strategy */
  fastjet::Strategy            m_strategy;
  /*! @brief @c fastjet tag: jet area type */
  fastjet::AreaType            m_areaType;
  /*! @brief @c fastjet tag: recombination scheme */
  fastjet::RecombinationScheme m_recombinationScheme;
  /*! @brief @c fastjet tag: SIS split merge scale */
#ifndef XAOD_STANDALONE
  fastjet::SISConePlugin::SplitMergeScale m_SIS_splitMergeScale;
#endif
  /*! @brief Pointer to jet definition */
  fastjet::JetDefinition*   m_jetDefinition;
  /*! @brief Pointer to actual cluster sequence */
  fastjet::ClusterSequence* m_clusterSequence;
  /*! @brief Pointer to area definition */
  fastjet::AreaDefinition*  m_areaDefinition;

  /// Base seed for random generator. Constructed from m_jetAlgorithm and m_radius
  uint64_t m_baseRNDSeed;
  /// Seed modifier for random generator : a user property (default to 0, i.e m_baseRNDSeed is unmodified)
  uint64_t m_userRNDSeed;


  /*! @brief Processor function pointer type */
  typedef StatusCode 
  (FastJetInterfaceTool::*PROCESSOR)(const FastJetInterface::fjetlist_t& inJets,
				     FastJetInterface::fjetlist_t& outJets);
  /*! @brief Extractor function pointer type */
  typedef StatusCode 
  (FastJetInterfaceTool::*EXTRACTOR)(FastJetInterface::fjetlist_t& outJets);

  /*! @brief Pointer to processor */
  PROCESSOR m_processor;
  /*! @brief Pointer to extractor */
  EXTRACTOR m_extractor;

  /*! @brief Processor implementation: cluster sequence without area 
   *           calculation
   *
   *  Sets up and executes a @c fastjet::ClusterSequence with finds 
   *  jets without area calculation. 
   *
   *  @return @c StatusCode::SUCCESS if execution ok, @c StatusCode::FAILURE
   *          if e.g. pointer to @c fastjet::ClusterSequence invalid, or 
   *          returned jet list is empty.
   *
   *  @param[in] inJets reference to non-modifiable list of 
   *                    @c fastjet::PseudoJet objects
   *  @param[out] outJets reference to modifiable list of @c fastjet::PseudoJet
   *              objects to store output  
   */
  StatusCode f_processWithoutArea(const FastJetInterface::fjetlist_t& inJets,
				  FastJetInterface::fjetlist_t& outJets);

  /*! @brief Processor implementation: cluster sequence with area calculation
   *
   *  Sets up and executes a @c fastjet::ClusterSequence with finds 
   *  jets with a client configured area calculation. 
   *
   *  @return @c StatusCode::SUCCESS if execution ok, @c StatusCode::FAILURE
   *          if e.g. pointer to @c fastjet::ClusterSequence invalid, or 
   *          returned jet list is empty.
   *
   *  @param[in] inJets reference to non-modifiable list of 
   *                    @c fastjet::PseudoJet objects
   *  @param[out] outJets reference to modifiable list of @c fastjet::PseudoJet
   *              objects to store output  
   */
  StatusCode f_processWithArea(const FastJetInterface::fjetlist_t& inJets,
			       FastJetInterface::fjetlist_t& outJets);

  /*! @brief Configures jet area calculation strategy
   *
   *  @return @c StatusCode::SUCCESS if area algorithm configured alright, else
   *          @c StatusCode::FAILURE (most likely indicates invalid area
   *          algorithm specs in the properties of the tool).
   */ 
  StatusCode configJetAreas();
  /*! @brief Inclusive extractor implementation
   * 
   *  Extracts inclusive jets above a given @f$ p_T @f$ cut, as defined in
   *  the @c InclusivePtMin property. 
   *
   *  @param[out] outJets reference to modifiable @c PseudoJet collection 
   *              containing inclusive jet sample
   *  
   *  @return @c StatusCode::SUCCESS if everything is ok.
   */
  StatusCode f_extractInclusive(FastJetInterface::fjetlist_t& outJets);
  /*! @brief Exclusive extractor implementation
   *
   *  Extracts exclusive jet sample defined by @f$ d_{cut} @f$ .
   * 
   *  @param[out] outJets reference to modifiable @c PseudoJet collection 
   *              containing exclusive jet sample
   *  
   *  @return @c StatusCode::SUCCESS if everything is ok.
   *  
   */
  StatusCode f_extractExclDcut(FastJetInterface::fjetlist_t& outJets);
  /*! @brief Exclusive extractor implementation
   *
   *  Extracts exclusive jet sample defined by @f$ N_{jets} @f$ .
   * 
   *  @param[out] outJets reference to modifiable @c PseudoJet collection 
   *              containing exclusive jet sample
   *  
   *  @return @c StatusCode::SUCCESS if everything is ok.
   *  
   */
  StatusCode f_extractExclNjets(FastJetInterface::fjetlist_t& outJets);



  /// Set the area rnd seed according to run/event numbers
  virtual void updateRandomSeeds();


};

/*! @class FastJetInterfaceTool
 *
 *  @brief Wrapper for @c fastjet verion 2.4.4. 
 *
 *  This wrapper provides the dictionary to translate a Athena tool 
 *  configuration to known @c fastjet configurations at initilization. It
 *  executes the requested @c fastjet methods and provides access to the 
 *  results. The data interfaces are purely based on the @c fastjet data model.
 *  This tool should therefore be wrapped by a module linking the @c fastjet
 *  data model to the Athena event data models for jet inputs.
 *
 *  The present version implements a rather large subset of @c fastjet 
 *  functionality as is available in @c fastjet version 2.4.4. Accessing the
 *  largely improved features of @c fastjet versions 3.0.0 and up require 
 *  adaptations of this tool. 
 *
 *  Remarks on process control:
 *
 *  If @c Inclusive is @c false , exclusive jets are returned (a) based on
 *  @f$ d_{cut} @f$ if @c ExclusiveDcut > 0 or (b) based on @f$ N_{jet} @f$ if
 *  @c ExclusiveDcut < 0. 
 */

// inline 
// const FastJetInterface::algomap_t& FastJetInterface::getKnownAlgorithms()
// { return FastJetInterfaceTool::getKnownAlgorithms(); }

// inline 
// const FastJetInterface::strategymap_t& FastJetInterface::getKnownStrategies()
// { return FastJetInteraceTool::getKnownStrategies(); }

// inline
// const FastJetInterface::schememap_t& 
// FastJetInterface::getKnownRecombinationSchemes()
// { return FastJetInterfaceTool::getKnownRecombinationSchemes(); }

// inline 
// const FastJetInterface::areamap_t& FastJetInterface::getKnownAreas()
// { return FastJetInterfaceTool::getKnownAreas(); }

#endif
