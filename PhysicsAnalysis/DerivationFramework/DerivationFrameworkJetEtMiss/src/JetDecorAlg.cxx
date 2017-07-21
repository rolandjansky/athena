/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetDecorAlg.cxx

#include "JetDecorAlg.h"
#include <sstream>
#include <map>
#include "xAODJet/JetContainer.h"

namespace DerivationFramework {

  using std::string;
  using std::vector;
  using std::map;
  using namespace xAOD;

  vector<string> &split(const string &s, char delim, vector<string> &elems) {
    std::stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
      elems.push_back(item);
    }
    return elems;
  }

  vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
  }

  static SG::AuxElement::Accessor< ElementLink<IParticleContainer> > dec_origlink("originalObjectLink");

  enum CopyType{
    Unknown,
    Float,
    Int,
    VectorFloat,
    VectorInt
  };

  //**********************************************************************

  JetDecorAlg::JetDecorAlg(const string& name,
			 ISvcLocator* pSvcLocator )
  : ::AthAlgorithm( name, pSvcLocator ) {
    declareProperty( "InputCollection", m_inputColl);
    declareProperty( "MomentsToCopy", m_momentsToCopy);
  }

  //**********************************************************************

  JetDecorAlg::~JetDecorAlg() { }

  //**********************************************************************

  StatusCode JetDecorAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");

    ATH_MSG_INFO("Copying the following moments from " << m_inputColl << " to original collection:");
    vector<string> split_for_type;
    vector<string> split_for_alias;
    for(const auto& momstr : m_momentsToCopy) {
      split_for_type.clear();
      split_for_alias.clear();
      split(momstr,'#',split_for_type);
      if(split_for_type.size()!=2) {
	ATH_MSG_ERROR("Failed to split moment string \"" << momstr << "\" for type. Format is \"Type#Name[@Alias]\"");
	ATH_MSG_ERROR("Type should be \"float\", \"int\", \"vector<float>\" or \"vector<int>\"");
	return StatusCode::FAILURE;
      } else {
	CopyType typeToCopy(Unknown);
	// 0 = float, 1 = int, 10 = vector<float>, 11 = vector<int>
	if ( split_for_type[0].compare("float")==0 ) {
	  typeToCopy=Float;
	} else if( split_for_type[0].compare("int")==0 ) {
	  typeToCopy=Int;
	} else if( split_for_type[0].compare("vector<float>")==0 ) {
	  typeToCopy=VectorFloat;
	} else if( split_for_type[0].compare("vector<int>")==0 ) {
	  typeToCopy=VectorInt;
	} else {
	  ATH_MSG_ERROR("Invalid type \"" << split_for_type[0] << "\" supplied. Format is \"Type#Name[@Alias]\"");
	  ATH_MSG_ERROR("Type should be either \"float\" or \"vector<float>\"");
	  return StatusCode::FAILURE;
	}
	split(split_for_type[1],'@',split_for_alias);
	switch(split_for_alias.size()) {
	case 1: split_for_alias.push_back(split_for_alias[0]);
	case 2: break;
	default:
	  ATH_MSG_ERROR("Failed to split name string \"" << split_for_type[1] << "\" for alias. Format is \"Type#Name[@Alias]\"");
	  return StatusCode::FAILURE;
	}
	switch(typeToCopy) {
	case Float:
	  ATH_MSG_INFO("float: " << split_for_alias[0] << " ==> " << split_for_alias[1]);
	  m_floatMoments.push_back(std::make_pair(split_for_alias[0],split_for_alias[1]));
	  break;
	case Int:
	  ATH_MSG_INFO("int: " << split_for_alias[0] << " ==> " << split_for_alias[1]);
	  m_intMoments.push_back(std::make_pair(split_for_alias[0],split_for_alias[1]));
	  break;
	case VectorFloat:
	  ATH_MSG_INFO("vector<float>: " << split_for_alias[0] << " ==> " << split_for_alias[1]);
	  m_vfloatMoments.push_back(std::make_pair(split_for_alias[0],split_for_alias[1]));
	  break;
	case VectorInt:
	  ATH_MSG_INFO("vector<int>: " << split_for_alias[0] << " ==> " << split_for_alias[1]);
	  m_vintMoments.push_back(std::make_pair(split_for_alias[0],split_for_alias[1]));
	  break;
	default:
	  ATH_MSG_ERROR("Invalid type specifier!");
	  return StatusCode::FAILURE;
	}
      }
      split_for_type.clear();
    }

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode JetDecorAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

   StatusCode JetDecorAlg::execute() { 
      ATH_MSG_VERBOSE("Executing " << name() << "...");

      const JetContainer* inputJets(0);
      ATH_CHECK( evtStore()->retrieve(inputJets,m_inputColl) );

      if(inputJets->size()>0) {
         if(!dec_origlink.isAvailable(*inputJets->front())) {
            ATH_MSG_WARNING("Input collection supplied without originalObjectLink");
            return StatusCode::SUCCESS;
         }

         // Do a very evil hack:
         const JetContainer* origCont =
            dynamic_cast< const JetContainer* >( ( *dec_origlink( *( inputJets->front() ) ) )->container() );
         if( ! origCont ) {
            ATH_MSG_FATAL( "Couldn't cast the jet container to xAOD::JetContainer" );
            return StatusCode::FAILURE;
         }
         const SG::IConstAuxStore* cstore = origCont->getConstStore();
         const SG::IAuxStore* store = dynamic_cast< const SG::IAuxStore* >( cstore );
         SG::IAuxStore* ncstore = const_cast< SG::IAuxStore* >( store );
         if( ! ncstore ) {
            ATH_MSG_FATAL( "Couldn't cast the jet auxiliary container to SG::IAuxStore" );
            return StatusCode::FAILURE;
         }
         JetContainer* ncCont = const_cast< JetContainer* >( origCont );
         ncCont->setStore( ncstore );

         for(const auto& jet_source : *inputJets) {
            const Jet* cjet_target = static_cast<const Jet*>(*dec_origlink(*jet_source));
            if(!cjet_target) {
               ATH_MSG_WARNING("Invalid target jet returned!");
               return StatusCode::SUCCESS;
            }
            Jet* jet_target = const_cast< Jet* >( cjet_target );

            // Loop over moments.
            for(const auto& mompair : m_floatMoments) {
               ATH_MSG_VERBOSE("Copy " << mompair.first << " to " << mompair.second);
               jet_target->auxdecor<float>(mompair.second) = jet_source->auxdata<float>(mompair.first);
               jet_target->auxdata<float>(mompair.first) = jet_source->auxdata<float>(mompair.first);
               ATH_MSG_VERBOSE("Check: " << mompair.first << " (" << jet_source->auxdata<float>(mompair.first)
                               << ") vs " << mompair.second << "(" << jet_target->auxdecor<float>(mompair.second) << ")");
            }
            for(const auto& mompair : m_intMoments) {
               ATH_MSG_VERBOSE("Copy " << mompair.first << " to " << mompair.second);
               jet_target->auxdecor<int>(mompair.second) = jet_source->auxdata<int>(mompair.first);
               jet_target->auxdata<int>(mompair.first) = jet_source->auxdata<int>(mompair.first);
               ATH_MSG_VERBOSE("Check: " << mompair.first << " (" << jet_source->auxdata<int>(mompair.first)
                               << ") vs " << mompair.second << "(" << jet_target->auxdecor<int>(mompair.second) << ")");
            }
            for(const auto& mompair : m_vfloatMoments) {
               ATH_MSG_VERBOSE("Copy " << mompair.first << " to " << mompair.second);
               jet_target->auxdecor<vector<float> >(mompair.second) = jet_source->auxdata<vector<float> >(mompair.first);
               jet_target->auxdata<vector<float> >(mompair.first) = jet_source->auxdata<vector<float> >(mompair.first);
               ATH_MSG_VERBOSE("Check " << mompair.first << " (size=" << jet_source->auxdata<vector<float> >(mompair.first).size()
                               << ") vs " << mompair.second << "(size=" << jet_target->auxdecor<vector<float> >(mompair.second).size() << ")");
            }
            for(const auto& mompair : m_vintMoments) {
               ATH_MSG_VERBOSE("Copy " << mompair.first << " to " << mompair.second);
               jet_target->auxdecor<vector<int> >(mompair.second) = jet_source->auxdata<vector<int> >(mompair.first);
               jet_target->auxdata<vector<int> >(mompair.first) = jet_source->auxdata<vector<int> >(mompair.first);
               ATH_MSG_VERBOSE("Check " << mompair.first << " (size=" << jet_source->auxdata<vector<int> >(mompair.first).size()
                               << ") vs " << mompair.second << "(size=" << jet_target->auxdecor<vector<int> >(mompair.second).size() << ")");
            }
         }

         // Undo the very evil hack:
         ncCont->setConstStore( cstore );
      }

      return StatusCode::SUCCESS;
   }

  //**********************************************************************

}

