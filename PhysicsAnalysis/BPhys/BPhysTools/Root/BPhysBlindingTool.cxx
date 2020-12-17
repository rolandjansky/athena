/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// system include:
#include "boost/tokenizer.hpp"
#include <boost/algorithm/string.hpp>
#include <set>
#include <cmath>

// EDM includes:
#include "xAODEventInfo/EventInfo.h"

// ROOT includes
#include "TString.h"

// Local include(s):
#include "BPhysTools/BPhysBlindingTool.h"

namespace xAOD {

  //--------------------------------------------------------------------------
  // Constructor
  //--------------------------------------------------------------------------
  BPhysBlindingTool::BPhysBlindingTool( const std::string& name )
    : asg::AsgTool( name ),
      m_vtxContainer(nullptr), m_vtxAuxContainer(nullptr),
      m_cachedRun(-1), m_cachedEvent(-1),
      m_eventsForBlindingSeen(0),
      m_candidatesForBlindingSeen(0),
      m_eventsForUnblindingSeen(0),
      m_candidatesForUnblindingSeen(0),
      m_eventsBlinded(0),
      m_candidatesBlinded(0),
      m_eventsUnblinded(0),
      m_candidatesUnblinded(0) {

#ifdef ASGTOOL_ATHENA
    declareInterface< IBPhysBlindingTool >( this );
#endif // ASGTOOL_ATHENA

    // Vertex container
    declareProperty("VertexContainerName", m_vertexContainerName = "");

    // List of variables to blind
    // (as concatenated string using . as delimiter) 
    declareProperty("VarToBlindNames", m_varToBlindNames = "");

    // Flag to indicate candidates for blinding
    // Left empty: Blind values for all candidates.
    declareProperty("BlindingFlag"   , m_blindingFlag = "");
    
    // Offsets applied to values before blinding
    // List must have same length as VarToBlindNames or zero.
    declareProperty("BlindingOffsets", m_vOffsets);

    // Scale factors applied before blinding
    // List must have same length as VarToBlindNames or zero.
    declareProperty("BlindingFactors", m_vFactors);

    // Flip signs to negative range?
    declareProperty("NegativeSigns"  , m_vNegSigns);
    
    // Key for blinding
    declareProperty("BlindingKey"    , m_blindKey = "");
    
    // Key for unblinding
    declareProperty("UnblindingKey"  , m_unblindKey = "");

  }
  //--------------------------------------------------------------------------
  StatusCode BPhysBlindingTool::initialize() {
    
    // Greet the user:
    ATH_MSG_DEBUG( "Initializing xAOD::BPhysBlindingTool" );

    // Setup of variables
    if ( m_vertexContainerName == "" ) {
      ATH_MSG_INFO("No vertex container name provided.");
    }

    if ( m_varToBlindNames != ""  ) {
      m_vVarNames = getTokens(m_varToBlindNames, ".,:;|");
    }

    // Blinding and unblinding keys
    if ( m_blindKey == "" && m_unblindKey == "" ) {
      ATH_MSG_ERROR("You must at least set a key for blinding or unblinding!");
    } else {
      if ( m_blindKey != "" ) {
        m_senc.setPubKey(m_blindKey);
        ATH_MSG_INFO("Setting blinding key.");
      }
      if ( m_unblindKey != "" ) {
        m_senc.setPrivKey(m_unblindKey);
        ATH_MSG_INFO("Setting unblinding key.");
      }
    }

    // make sure offsets vector is of correct length
    if ( m_vOffsets.size() < m_vVarNames.size() ) {
      for (uint i=m_vOffsets.size(); i<m_vVarNames.size(); ++i) {
        m_vOffsets.push_back(0.);
      }
      ATH_MSG_INFO("Extending BlindingOffsets list ...");
    } else if ( m_vOffsets.size() > m_vVarNames.size() ) {
      ATH_MSG_WARNING("BlindingOffsets list longer than VarToBlindNames.");
    }
    
    // make sure scale factors vector is of correct length
    if ( m_vFactors.size() < m_vVarNames.size() ) {
      for (uint i=m_vFactors.size(); i<m_vVarNames.size(); ++i) {
        m_vFactors.push_back(1.);
      }
      ATH_MSG_INFO("Extending BlindingOffsets list ...");
    } else if ( m_vFactors.size() > m_vVarNames.size() ) {
      ATH_MSG_WARNING("BlindingFactors list longer than VarToBlindNames.");
    }

    // make sure negative signs vector is of correct length
    if ( m_vNegSigns.size() < m_vVarNames.size() ) {
      for (uint i=m_vNegSigns.size(); i<m_vVarNames.size(); ++i) {
        m_vNegSigns.push_back(1.);
      }
      ATH_MSG_INFO("Extending NegativeSigns list ...");
    } else if ( m_vNegSigns.size() > m_vVarNames.size() ) {
      ATH_MSG_WARNING("NegativeSigns list longer than VarToBlindNames.");
    }

    // some info for the job log
    ATH_MSG_INFO("VertexContainerName        : " << m_vertexContainerName);
    ATH_MSG_INFO("BlindingFlag               : " << m_blindingFlag);
    ATH_MSG_INFO("VarToBlindNames            : " << m_varToBlindNames);
    ATH_MSG_INFO("BlindingOffsets            : " << vecToString(m_vOffsets));
    ATH_MSG_INFO("BlindingFactors            : " << vecToString(m_vFactors));
    ATH_MSG_INFO("NegativeSigns              : " << vecToString(m_vNegSigns));
    ATH_MSG_INFO("BlindingKey                : " << m_blindKey);
    ATH_MSG_INFO("UnblindingKey              : " << m_unblindKey);

    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysBlindingTool::finalize() {

    ATH_MSG_DEBUG( "Finalizing xAOD::BPhysBlindingTool" );

    ATH_MSG_INFO("Statistics for " << name() << ":");
    ATH_MSG_INFO(Form("N_eventsForBlindingSeen       : %10ld",
                      m_eventsForBlindingSeen));
    ATH_MSG_INFO(Form("N_eventsBlinded               : %10ld",
                      m_eventsBlinded));
    ATH_MSG_INFO(Form("N_eventsForUnblindingSeen     : %10ld",
                      m_eventsForUnblindingSeen));
    ATH_MSG_INFO(Form("N_eventsUnblinded             : %10ld",
                      m_eventsUnblinded));
    ATH_MSG_INFO(Form("N_candidatesForBlindingSeen   : %10ld",
                      m_candidatesForBlindingSeen));
    ATH_MSG_INFO(Form("N_candidatesBlinded           : %10ld",
                      m_candidatesBlinded));
    ATH_MSG_INFO(Form("N_candidatesForUnblindingSeen : %10ld",
                      m_candidatesForUnblindingSeen));
    ATH_MSG_INFO(Form("N_candidatesUnblinded         : %10ld",
                      m_candidatesUnblinded));

    // Return gracefully:
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  // Simply blind one positive float value
  //--------------------------------------------------------------------------
  float BPhysBlindingTool::doBlind(const float& val) {

    return m_senc.encrypt(val);
  }  
  //--------------------------------------------------------------------------
  // Simply unblind one positive float value
  //--------------------------------------------------------------------------
  float BPhysBlindingTool::doUnblind(const float& val) {

    return m_senc.decrypt(val);
  }
  //--------------------------------------------------------------------------
  // Simply blind one (positive) float value
  //--------------------------------------------------------------------------
  float BPhysBlindingTool::doBlind(const float& val,
                                   const bool&  negativeSign,
                                   const float& offset,
                                   const float& factor) {

    // adjustment if requested
    float bval(val);
    float cval = val*factor + offset;
    if ( cval > 0. ) {
      // perform actual blinding
      bval = m_senc.encrypt(cval);
      if (negativeSign) bval *= -1.;
    } else {
      ATH_MSG_WARNING("Blinding: Corrected value not positive: "
                      << val << Form(" (%a) -> ", val)
                      << cval << Form(" (%a)", cval));
    } // if cval > 0

    return bval;
  }  
  //--------------------------------------------------------------------------
  // Simply unblind one (positive) float value
  //--------------------------------------------------------------------------
  float BPhysBlindingTool::doUnblind(const float& val,
                                     const bool&  negativeSign,
                                     const float& offset,
                                     const float& factor) {

    float bval(val), cval(val);
    if (negativeSign) bval *= -1.;
    // if ( bval > 0. || isnan(bval) ) {
    if ( bval > 0. || !std::isnormal(bval) ) {
      // perform actual unblinding
      cval = m_senc.decrypt(bval);
      if ( factor != 0. ) {
        cval = (cval - offset)/factor;
      } else {
        ATH_MSG_WARNING("Unblinding: BlindingFactor == 0!: "
                        << val << Form(" (%a)", val));
      } // if m_vFactors[ivtx] != 0
    } else {
      ATH_MSG_WARNING("Unblinding: Corrected value not positive: "
                      << val << Form(" (%a) -> ", val)
                      << bval << Form(" (%a)", bval));
    } // if bval > 0

    return cval;
  }
  //--------------------------------------------------------------------------
  // Perform blinding of requested variables 
  //--------------------------------------------------------------------------
  StatusCode BPhysBlindingTool::doBlind() {

    if ( m_blindKey == "" ) {
      ATH_MSG_WARNING("Can not blind without blinding key!");
    } else {
      ATH_CHECK( doBlindingAction(false) );
    }
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Perform unblinding of requested variables 
  //--------------------------------------------------------------------------
  StatusCode BPhysBlindingTool::doUnblind() {
    
    if ( m_unblindKey == "" ) {
      ATH_MSG_WARNING("Can not unblind without unblinding key!");
    } else {
      ATH_CHECK( doBlindingAction(true) );
    }

    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  // Protected methods
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  // Perform blinding or unblinding action
  //--------------------------------------------------------------------------
  StatusCode BPhysBlindingTool::doBlindingAction(bool unblind) {

    ATH_CHECK(cacheEvent());

    // counters
    if ( unblind ) {
      ++m_eventsForUnblindingSeen;
    } else {
      ++m_eventsForBlindingSeen;
    }
    
    if ( m_vVarNames.size() > 0 ) {
      long candidatesBlinded(0);
      long candidatesUnblinded(0);
      // loop over vertices
      // int ivtx(0);
      for (xAOD::VertexContainer::const_iterator
             vtxItr = m_vtxContainer->begin();
           vtxItr != m_vtxContainer->end(); ++vtxItr) {
        // counters
        if ( unblind ) {
          ++m_candidatesForUnblindingSeen;
        } else {
          ++m_candidatesForBlindingSeen;
        }
        const xAOD::Vertex* vtx = *vtxItr;
        // check whether to apply (un-)blinding to this candidate
        if ( m_blindingFlag == "" || pass(*vtx, m_blindingFlag) ) {
          // counters
          if ( unblind ) {
            ++candidatesUnblinded;
          } else {
            ++candidatesBlinded;
          }
          // loop over variable names
          for (size_t iv=0; iv<m_vVarNames.size(); ++iv) {
            SG::AuxElement::Decorator<float> floatDec(m_vVarNames[iv]);
            // check for variable
            if ( floatDec.isAvailable(*vtx) ) {
              float val = floatDec(*vtx);
              if ( unblind ) {
                // unblinding
                floatDec(*vtx) = doUnblind(val, m_vNegSigns[iv],
                                           m_vOffsets[iv], m_vFactors[iv]);
                ATH_MSG_DEBUG("Unblind: " << val << Form(" (%a) -> ", val)
                              << floatDec(*vtx)
                              << Form(" (%a)", floatDec(*vtx)));
              } else {
                // blinding
                floatDec(*vtx) = doBlind(val, m_vNegSigns[iv],
                                         m_vOffsets[iv], m_vFactors[iv]);
                ATH_MSG_DEBUG("Blind: " << val << Form(" (%a) -> ", val)
                              << floatDec(*vtx)
                              << Form(" (%a)", floatDec(*vtx)));
              } // if unblind
            } else {
              ATH_MSG_WARNING("Missing variable " << m_vVarNames[iv]);
            } // if isAvailable
          } // for m_vVarNames
        } // if blinding
      } // for iv
      // counters
      if ( unblind ) {
        m_candidatesUnblinded += candidatesUnblinded;
        if ( candidatesUnblinded > 0 ) ++m_eventsUnblinded;
      } else {
        m_candidatesBlinded += candidatesBlinded;
        if ( candidatesBlinded > 0 ) ++m_eventsBlinded;
      }
    } // if m_vVarNames.size()
    
    // Return gracefully:
    return StatusCode::SUCCESS;    
  }
  //--------------------------------------------------------------------------
  // Cache current event.
  //
  // Call this once per event.
  // Repeated calls for the same run/event are not updating the cache again.
  //--------------------------------------------------------------------------
  StatusCode BPhysBlindingTool::cacheEvent() {

    ATH_MSG_DEBUG("BPhysBlindingTool::cacheEvent -- begin");
    
    const xAOD::EventInfo* eventInfo = NULL;
    ATH_CHECK(evtStore()->retrieve(eventInfo, "EventInfo"));

    if ( m_cachedRun   != (int)eventInfo->runNumber() ||
         m_cachedEvent != (int)eventInfo->eventNumber() ) {

      // note update
      m_cachedRun   = eventInfo->runNumber();
      m_cachedEvent = eventInfo->eventNumber();

      ATH_MSG_DEBUG("BPhysBlindingTool::cacheEvent: caching now: "
                    << "run " << m_cachedRun << " event " << m_cachedEvent);

      // retrieve vertices container
      m_vtxContainer    = nullptr;
      m_vtxAuxContainer = nullptr;

      if ( evtStore()->transientContains<xAOD::VertexContainer>(m_vertexContainerName) ) {
        ATH_MSG_DEBUG("In transient store: " << m_vertexContainerName);
        ATH_CHECK(evtStore()->retrieve(m_vtxContainer,
                                       m_vertexContainerName));
        ATH_CHECK(evtStore()->retrieve(m_vtxAuxContainer,
                                       m_vertexContainerName+"Aux."));
      }  else {
        ATH_MSG_DEBUG("Not in transient store: " << m_vertexContainerName);
        const xAOD::VertexContainer*    constVtxContainer    = nullptr;
        const xAOD::VertexAuxContainer* constVtxAuxContainer = nullptr;
        ATH_CHECK(evtStore()->retrieve(constVtxContainer,
                                       m_vertexContainerName));
        ATH_CHECK(evtStore()->retrieve(constVtxAuxContainer,
                                       m_vertexContainerName+"Aux."));
        // create a copy
        m_vtxContainer    = new xAOD::VertexContainer();
        m_vtxAuxContainer = new xAOD::VertexAuxContainer();
        m_vtxContainer->setStore(m_vtxAuxContainer);
        for (const xAOD::Vertex* constVtx : *constVtxContainer) {
          xAOD::Vertex* vtx = new xAOD::Vertex();
          m_vtxContainer->push_back(vtx);
          *vtx = *constVtx;
        }
        ATH_CHECK(evtStore()->record(m_vtxContainer,
                                     m_vertexContainerName));
        ATH_CHECK(evtStore()->record(m_vtxAuxContainer,
                                     m_vertexContainerName+"Aux."));
      }
      
      ATH_MSG_DEBUG("Found vertex collection with key "
                    << m_vertexContainerName);

    } // if new run/event
    
    ATH_MSG_DEBUG("BPhysBlindingTool::cacheEvent -- end");
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  } 
  //--------------------------------------------------------------------------
  // Helper to check whether an element is marked as passing a specific
  // hypothesis.
  //--------------------------------------------------------------------------
  bool BPhysBlindingTool::pass(const SG::AuxElement& em, std::string hypo) {
    
    if ( !boost::algorithm::starts_with(hypo, "passed_") )
      hypo = "passed_" + hypo;
    SG::AuxElement::Accessor<Char_t> flagAcc(hypo);
    return flagAcc.isAvailable(em) && flagAcc(em) != 0;
  }
  //--------------------------------------------------------------------------
  // Tokenize a string using certain separators
  //--------------------------------------------------------------------------
  std::vector<std::string>
  BPhysBlindingTool::getTokens(std::string input, std::string seperators) {
    
    std::vector<std::string> tokens;
    boost::char_separator<char> sep(seperators.c_str());
    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer_t;
    Tokenizer_t tokenizer(input, sep);
    for (auto& token : tokenizer) {
      tokens.push_back(token);
    }
    return tokens;
  }
  //--------------------------------------------------------------------------
  // Format vector of floats as string
  //--------------------------------------------------------------------------
  std::string BPhysBlindingTool::vecToString(const std::vector<float>& v)
     const {
     std::string str("[");
     for (unsigned int i=0; i<v.size(); ++i) {
       str += std::to_string(v[i]);
       if ( i < v.size()-1 ) str += ",";
     }
     str += "]";
     return str;
  }
  //--------------------------------------------------------------------------
  // Format vector of bools as string
  //--------------------------------------------------------------------------
  std::string BPhysBlindingTool::vecToString(const std::vector<bool>& v)
     const {
     std::string str("[");
     for (unsigned int i=0; i<v.size(); ++i) {
       str += std::to_string(v[i]);
       if ( i < v.size()-1 ) str += ",";
     }
     str += "]";
     return str;
  }
  //--------------------------------------------------------------------------
} // namespace xAOD
