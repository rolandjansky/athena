/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetRecTool.cxx

#include "JetRec/JetRecTool.h"
#include <iomanip>
#ifdef USE_BOOST_FOREACH
#include <boost/foreach.hpp>
#endif
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include <fstream>
//#include "AthenaKernel/errorcheck.h"

#include "xAODCore/ShallowCopy.h"
#include "JetEDM/FastJetLink.h"
#include "xAODJet/Jet_PseudoJet.icc"

typedef ToolHandleArray<IPseudoJetGetter> PseudoJetGetterArray;
typedef ToolHandleArray<IJetModifier> ModifierArray;

using std::string;
using std::setw;
using std::fixed;
using std::setprecision;
using xAOD::JetContainer;
using xAOD::Jet;


namespace {
  
  // void deepCopyJets( const xAOD::JetContainer & jetsin, xAOD::JetContainer & jetsout){

  // } 

  xAOD::JetContainer* shallowCopyJets( const xAOD::JetContainer& jetsin){
    xAOD::JetContainer & shallowcopy = *(xAOD:: shallowCopyContainer( jetsin).first);
    for (size_t i=0;i<shallowcopy.size(); i++) {
      const fastjet::PseudoJet * pseudoJ = jetsin[i]->getPseudoJet();
      if( pseudoJ ) shallowcopy[i]->setPseudoJet(pseudoJ);
    }
    return &shallowcopy;
  } 

}

//**********************************************************************

JetRecTool::JetRecTool(std::string myname)
: AsgTool(myname), m_intool(""),
  m_finder(""), m_groomer(""),
  m_trigger(false),
  m_shallowCopy(true),
  m_initCount(0),
  m_find(false), m_groom(false), m_copy(false)  {
  declareProperty("OutputContainer", m_outcoll);
  declareProperty("InputContainer", m_incoll);
  declareProperty("InputTool", m_intool);
  declareProperty("PseudoJetGetters", m_pjgetters);
  declareProperty("JetFinder", m_finder);
  declareProperty("JetGroomer", m_groomer);
  declareProperty("JetModifiers", m_modifiers);
  declareProperty("Trigger", m_trigger);
  declareProperty("Timer", m_timer =0);
  declareProperty("ShallowCopy", m_shallowCopy =true);

}

//**********************************************************************

StatusCode JetRecTool::initialize() {
  ATH_MSG_INFO("Initializing JetRecTool " << name() << ".");
  ++m_initCount;
  // Fetch the reconstruction mode.
  bool needinp = false;
  bool needout = false;
  string mode = "pseudojets";
  if ( m_outcoll.size() ) {
    m_outcolls.push_back(m_outcoll);
    if ( ! m_finder.empty() ) {
      mode = "find";
      m_find = true;
      needout = true;
      if ( m_pjgetters.size() == 0 ) {
        ATH_MSG_ERROR("Jet finding requested with no inputs.");
        return StatusCode::FAILURE;
      }
    } else if ( ! m_groomer.empty() ) {
      mode = "groom";
      m_groom = true;
      needinp = true;
      needout = true;
    } else {
      mode = "copy";
      m_copy = true;
      needinp = true;
      needout = true;
    }
  }
  m_shallowCopy &= m_copy; // m_shallowCopy is false if not copy mode

  ATH_MSG_INFO("Jet reconstruction mode: " << mode);
  // Check/set the input jet collection name.
  if ( needinp ) {
    if ( m_incoll.size() == 0 ) {
      if ( ! m_intool.retrieve() ) {
        ATH_MSG_ERROR("Input collection must be specified.");
        return StatusCode::FAILURE;
      } else {
        const AsgTool* pasgtool = dynamic_cast<const asg::AsgTool*>(&*m_intool);
        if ( pasgtool != 0 ) {
          const string* pval = pasgtool->getProperty<std::string>("OutputContainer");
          if ( pval != 0 ) {
            m_incoll = *pval;
          }
        }
        if ( m_incoll.size() == 0 ) {
          ATH_MSG_ERROR("Input tool does not have output collection name.");
          return StatusCode::FAILURE;
        }
      }
    }
    m_incolls.push_back(m_incoll);
  }
  // Check/set the output jet collection name.
  if ( needout ) {
    if ( m_outcoll.size() == 0 ) {
      ATH_MSG_ERROR("Output collection must be specified.");
      return StatusCode::FAILURE;
    }
  }
  // Other checks.
  if ( m_find ) {
    if ( m_pjgetters.size() == 0 ) {
      ATH_MSG_ERROR("Jet finding requested with no inputs.");
      return StatusCode::FAILURE;
    }
  } else if ( m_groom ) {
  } else if ( m_copy ) {
  } else {
    if ( m_pjgetters.size() == 0 ) {
      ATH_MSG_ERROR("No action requested.");
      return StatusCode::FAILURE;
    }
  }
  // Fetch the pseudojet getters.
  StatusCode rstat = StatusCode::SUCCESS;
  string prefix = "--- ";
  ATH_MSG_INFO(prefix << "JetRecTool " << name() << " has " << m_pjgetters.size()
               << " pseudojet getters.");
  m_getclocks.resize(m_pjgetters.size());
  unsigned int iclk = 0;
  for ( PseudoJetGetterArray::const_iterator iget=m_pjgetters.begin();
          iget!=m_pjgetters.end(); ++iget ) {
    ToolHandle<IPseudoJetGetter> hget = *iget;
    if ( hget.retrieve().isSuccess() ) {
      ATH_MSG_INFO(prefix << "Retrieved " << hget->name());
    } else {
      ATH_MSG_ERROR(prefix << "Unable to retrieve IPseudoJetGetter");
      rstat = StatusCode::FAILURE;
    }
    // Extract the input type from the first getter.
    if ( iget == m_pjgetters.begin() ) {
      ATH_MSG_INFO(prefix << "Extracting input type from primary getter.");
      string tname = hget->label();
      ATH_MSG_INFO(prefix << "Input label: " << tname);
      m_inputtype = xAOD::JetInput::inputType(tname);
      if ( m_inputtype == xAOD::JetInput::Uncategorized ) {
        ATH_MSG_ERROR("Invalid label for first pseudojet getter: " << tname);
        rstat = StatusCode::FAILURE;
      }
    }
    m_getclocks[iclk++].Reset();
    hget->inputContainerNames(m_incolls);
    hget->outputContainerNames(m_outcolls);
  }
  ATH_MSG_INFO(prefix << "Input type: " << m_inputtype);
  // Fetch the jet modifiers.
  ATH_MSG_INFO(prefix << "JetRecTool " << name() << " has " << m_modifiers.size()
               << " jet modifiers.");
  m_modclocks.resize(m_modifiers.size());
  iclk = 0;
  for ( ModifierArray::const_iterator imod=m_modifiers.begin();
        imod!=m_modifiers.end(); ++imod ) {
    ToolHandle<IJetModifier> hmod = *imod;
    if ( hmod.retrieve().isSuccess() ) {
      ATH_MSG_INFO(prefix << "Retrieved " << hmod->name());
    } else {
      ATH_MSG_ERROR(prefix << "Unable to retrieve IJetModifier");
      rstat = StatusCode::FAILURE;
    }
    m_modclocks[iclk++].Reset();
    hmod->inputContainerNames(m_incolls);
  }
  ATH_MSG_INFO(prefix << "Input collection names:");
#ifdef USE_BOOST_FOREACH
  BOOST_FOREACH(string name, m_incolls) ATH_MSG_INFO(prefix << "  " << name);
#else
  for (const auto& name : m_incolls) ATH_MSG_INFO(prefix << "  " << name);
#endif
  ATH_MSG_INFO(prefix << "Output collection names:");
#ifdef USE_BOOST_FOREACH
  BOOST_FOREACH(string name, m_outcolls) ATH_MSG_INFO(prefix << "  " << name);
#else
  for (const auto& name : m_outcolls) ATH_MSG_INFO(prefix << "  " << name);
#endif
  m_totclock.Reset();
  m_inpclock.Reset();
  m_actclock.Reset();
  m_modclock.Reset();
  m_nevt = 0;
  return rstat;
}

//**********************************************************************

StatusCode JetRecTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  string tname = "";
  unsigned int wname = 50;
  ATH_MSG_INFO("Event count: " << m_nevt);
  std::ofstream outfile;
  bool saveToFile =   m_timer > 10 ;
  if( saveToFile ) outfile.open(name()+".txt",std::ofstream::out | std::ofstream::trunc);

  if ( m_timer > 0 ) {
    ATH_MSG_INFO("  " << setw(wname) << "Time [sec/event]"
                 << setw(10) << "CPU" << setw(10) << "Wall");
    double tctime = m_totclock.CpuTime();
    double twtime = m_totclock.RealTime();
    double ictime = m_inpclock.CpuTime();
    double iwtime = m_inpclock.RealTime();
    double actime = m_actclock.CpuTime();
    double awtime = m_actclock.RealTime();
    double mctime = m_modclock.CpuTime();
    double mwtime = m_modclock.RealTime();
    double avg_tctime = 0.0;
    double avg_twtime = 0.0;
    double avg_ictime = 0.0;
    double avg_iwtime = 0.0;
    double avg_actime = 0.0;
    double avg_awtime = 0.0;
    double avg_mctime = 0.0;
    double avg_mwtime = 0.0;
    if ( m_nevt > 0 ) {
      avg_tctime = tctime/double(m_nevt);
      avg_twtime = twtime/double(m_nevt);
      avg_ictime = ictime/double(m_nevt);
      avg_iwtime = iwtime/double(m_nevt);
      avg_actime = actime/double(m_nevt);
      avg_awtime = awtime/double(m_nevt);
      avg_mctime = mctime/double(m_nevt);
      avg_mwtime = mwtime/double(m_nevt);
    }
    ATH_MSG_INFO("  " << setw(wname) << "Input"
                 << fixed << setprecision(3) << setw(10) << avg_ictime
                 << fixed << setprecision(3) << setw(10) << avg_iwtime);
    ATH_MSG_INFO("  " << setw(wname) << "Action"
                 << fixed << setprecision(3) << setw(10) << avg_actime
                 << fixed << setprecision(3) << setw(10) << avg_awtime);
    ATH_MSG_INFO("  " << setw(wname) << "Modifiers"
                 << fixed << setprecision(3) << setw(10) << avg_mctime
                 << fixed << setprecision(3) << setw(10) << avg_mwtime);
    ATH_MSG_INFO("  " << setw(wname) << "Total"
                 << fixed << setprecision(3) << setw(10) << avg_tctime
                 << fixed << setprecision(3) << setw(10) << avg_twtime);

    if( saveToFile ) outfile << "input," << avg_ictime << ","<< avg_iwtime << std::endl;
    if( saveToFile ) outfile << "finding," << avg_actime << ","<< avg_awtime << std::endl;
  }
  
  if ( m_timer > 1 && m_nevt > 0 ) {
    unsigned int ntool = m_pjgetters.size();
    ATH_MSG_INFO("  CPU/wall time [s] for " << ntool << " getters:");
    for ( unsigned int itool=0; itool<ntool; ++itool ) {
      string tname;
      TStopwatch& clock = m_getclocks[itool];
      tname = m_pjgetters[itool]->name();
      if ( tname.substr(0,8) == "ToolSvc." ) tname = tname.substr(8);
      double tctime = clock.CpuTime()/double(m_nevt);
      double twtime = clock.RealTime()/double(m_nevt);
      ATH_MSG_INFO("  " << setw(50) << tname
                   << fixed << setprecision(3) << setw(10) << tctime
                   << fixed << setprecision(3) << setw(10) << twtime);
    }
  }

  if ( m_timer > 1 && m_nevt > 0 ) {
    unsigned int ntool = m_modifiers.size();
    ATH_MSG_INFO("  CPU/wall time [s] for " << ntool << " modifiers:");
    assert( m_modclocks.size() == m_modifiers.size() );
    for ( unsigned int itool=0; itool<ntool; ++itool ) {
      string tname;
      TStopwatch& clock = m_modclocks[itool];
      tname = m_modifiers[itool]->name();
      if ( tname.substr(0,8) == "ToolSvc." ) tname = tname.substr(8);
      double tctime = clock.CpuTime()/double(m_nevt);
      double twtime = clock.RealTime()/double(m_nevt);
      ATH_MSG_INFO("  " << setw(50) << tname
                   << fixed << setprecision(3) << setw(10) << tctime
                   << fixed << setprecision(3) << setw(10) << twtime);
      if( saveToFile ) outfile << tname<< "," << tctime << ","<< twtime << std::endl;

    }
  }
  if( saveToFile ) outfile.close();
  return StatusCode::SUCCESS;
}

//**********************************************************************

const JetContainer* JetRecTool::build() const {
  if ( m_initCount == 0 ) {
    ATH_MSG_WARNING("Build requested before initialization.");
    return 0;
  }
  m_totclock.Start(false);
  ATH_MSG_DEBUG("Building jets with " << name() << ".");
  int naction = 0;
  ++m_nevt;

  // Retrieve jet inputs.
  PseudoJetVector psjs;
  if ( m_pjgetters.size() ) {
    m_inpclock.Start(false);
    ATH_MSG_DEBUG("Fetching pseudojet inputs.");
    unsigned int iclk = 0;
    for ( PseudoJetGetterArray::const_iterator iget=m_pjgetters.begin();
          iget!=m_pjgetters.end(); ++iget ) {
      m_getclocks[iclk].Start(false);
      ToolHandle<IPseudoJetGetter> hget = *iget;
      const PseudoJetVector* newpsjs = hget->get();
      if ( newpsjs == 0 ) {
        ATH_MSG_ERROR("Pseudojets not found.");
        m_totclock.Stop();
        m_inpclock.Stop();
        m_getclocks[iclk++].Stop();
        return 0;
      } else {
        ATH_MSG_DEBUG("Added input count: " << newpsjs->size());
        psjs.insert(psjs.end(), newpsjs->begin(), newpsjs->end());
      }
      ++naction;
      m_getclocks[iclk++].Stop();
    }
    ATH_MSG_DEBUG("Total input count: " << psjs.size());
    m_inpclock.Stop();
  }

  // Retrieve the old jet collection.
  const JetContainer* pjetsin = 0;
  if ( m_groom || m_copy ) {
    m_inpclock.Start(false);
    if ( m_incoll.size() && evtStore()->contains<JetContainer>(m_incoll)) {
      pjetsin = evtStore()->retrieve<const JetContainer>(m_incoll);
    }
    if ( pjetsin==0 && !m_intool.empty() ) {
      ATH_MSG_DEBUG("Excuting input tool.");
      if ( m_intool->execute() ) {
        ATH_MSG_WARNING("Input tool execution failed.");
      }
      pjetsin = evtStore()->retrieve<const JetContainer>(m_incoll);
    }
    if ( pjetsin == 0 ) {
      ATH_MSG_ERROR("Unable to retrieve input jet container: " << m_incoll);
      m_totclock.Stop();
      m_inpclock.Stop();
      return 0;
    }
    ATH_MSG_DEBUG("Input collection " << m_incoll << " jet multiplicity is "
                  << pjetsin->size());
    m_inpclock.Stop();
  }

  // The new jet collection.
  JetContainer* pjets = 0;

  // Fill output container by finding, grooming or copying.
  if ( m_outcoll.size() ) {
    m_actclock.Start(false);
    ATH_MSG_DEBUG("Creating output container.");
    if ( evtStore()->contains<JetContainer>(m_outcoll) ) {
      ATH_MSG_ERROR("Jet collection already exists: " << m_outcoll);
      m_totclock.Stop();
      m_actclock.Stop();
      return 0;
    }

    if(m_shallowCopy) {
      ATH_MSG_DEBUG("Shallow-copying jets.");
      pjets = shallowCopyJets(*pjetsin);
      ++naction;
    } else {
      pjets = new JetContainer;
      if ( m_trigger ) {
        ATH_MSG_DEBUG("Attaching online Aux container.");
        pjets->setStore(new xAOD::JetTrigAuxContainer);
      } else {
        ATH_MSG_DEBUG("Attaching offline Aux container.");
        pjets->setStore(new xAOD::JetAuxContainer);
      }
    }
    
    // Find jets.
    if ( ! m_finder.empty() ) {
      ATH_MSG_DEBUG("Finding jets.");
      m_finder->find(psjs, *pjets, m_inputtype);
      ++naction;
    // Groom jets.
    } else if ( ! m_groomer.empty() ) {
      ATH_MSG_DEBUG("Grooming " << pjetsin->size() << " jets.");
      for ( JetContainer::const_iterator ijet=pjetsin->begin();
            ijet!=pjetsin->end(); ++ijet ) {
        m_groomer->groom(**ijet, *pjets);
      }
      ++naction;
    // Copy jets.
    } else if ( (pjetsin != 0 ) && !m_shallowCopy) {
      ATH_MSG_DEBUG("Copying " << pjetsin->size() << " jets.");
#ifdef USE_BOOST_FOREACH
      BOOST_FOREACH(const Jet* poldjet, *pjetsin) {
#else
      for ( const Jet* poldjet : *pjetsin ) {
#endif
        Jet* pnewjet = new Jet;
        *pnewjet = *poldjet;
        pjets->push_back(pnewjet);
      }
      ++naction;
    }  // End list of options.
    m_actclock.Stop();
  }  // End filling of output container.

  // Modify jets.
  unsigned int nmod = m_modifiers.size();
  if ( nmod ) {
    m_modclock.Start(false);
    if ( pjets == 0 ) {
      ATH_MSG_WARNING("There is no jet collection to modify.");
    } else {
      ATH_MSG_DEBUG("Executing " << nmod << " jet modifiers.");
      unsigned int iclk = 0;
      for ( ModifierArray::const_iterator imod=m_modifiers.begin();
           imod!=m_modifiers.end(); ++imod ) {
        m_modclocks[iclk].Start(false);
        ATH_MSG_DEBUG("  Executing modifier " << imod->name());
        ATH_MSG_VERBOSE("    @ " << *imod);
        (*imod)->modify(*pjets) ;
        ++naction;
        m_modclocks[iclk++].Stop();
      }
    }
    m_modclock.Stop();
  }
  if ( naction == 0 ) {
    ATH_MSG_ERROR("Invalid configuration.");
  }
  m_totclock.Stop();
  return pjets;
}

//**********************************************************************

int JetRecTool::execute() const {
  if ( m_initCount == 0 ) {
    ATH_MSG_WARNING("Execute requested before initialization.");
    return 1;
  }
  const xAOD::JetContainer* pjets = build();
  if ( pjets == 0 ) {
    ATH_MSG_ERROR("Unable to retrieve container");
   return 1;
  }
  if ( m_trigger ) {
    return record<xAOD::JetTrigAuxContainer>(pjets);
  } 
  if (m_shallowCopy) {
    return record<xAOD::ShallowAuxContainer>(pjets);
  }
  return record<xAOD::JetAuxContainer>(pjets);
}

//**********************************************************************

template <typename TAux>
int JetRecTool::record(const xAOD::JetContainer* pjets) const {
  TAux* pjetsaux =
    dynamic_cast<TAux*>(pjets->getStore());
    ATH_MSG_DEBUG("Check Aux store: " << pjets << " ... " << &pjets->auxbase() << " ... " << pjetsaux );
  if ( pjetsaux == 0 ) {
    ATH_MSG_ERROR("Unable to retrieve Aux container");
   return 2;
  }
  ATH_MSG_VERBOSE("Recording new Jet Aux container.");
  StatusCode sca = evtStore()->record(pjetsaux, m_outcoll+"Aux.");
  if ( sca.isFailure() ) {
    ATH_MSG_ERROR("Unable to write new Aux Jet collection to event store: " << m_outcoll);
    return 3;
  }
  ATH_MSG_VERBOSE("Recording new Jet container.");
#ifdef XAOD_STANDALONE
  // TEvent only records mutable containers.
  xAOD::JetContainer* pmutjets = const_cast<xAOD::JetContainer*>(pjets);
  StatusCode sc = evtStore()->record(pmutjets, m_outcoll);
#else
  StatusCode sc = evtStore()->record(pjets, m_outcoll);
#endif
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Unable to write new Jet collection to event store: " << m_outcoll);
    return 4;
  }
  ATH_MSG_DEBUG("Created new Jet collection in event store: " << m_outcoll);
  return 0;
}

//**********************************************************************

void JetRecTool::print() const {
  ATH_MSG_INFO("Properties for JetRecTool " << name());
  ATH_MSG_INFO("  OutputContainer: " << m_outcoll);
  if ( m_incoll.size() == 0 ) ATH_MSG_INFO("  InputContainer is not defined");
  else ATH_MSG_INFO("  InputContainer: " << m_incoll);
  if ( m_intool.empty() ) {
    ATH_MSG_INFO("  InputTool is not defined");
  } else {
    ATH_MSG_INFO("  InputTool: " << m_intool->name());
  }
  if ( m_pjgetters.size() ) {
    ATH_MSG_INFO("  Pseudojet getter count is " << m_pjgetters.size());
    for ( PseudoJetGetterArray::const_iterator iget=m_pjgetters.begin();
            iget!=m_pjgetters.end(); ++iget ) {
      ToolHandle<IPseudoJetGetter> hget = *iget;
      ATH_MSG_INFO("    " << hget->name());
    }
  }
  if ( m_finder.empty() ) {
    ATH_MSG_INFO("  Jet finder is not defined");
  } else {
    ATH_MSG_INFO("  Jet finder: " << m_finder->name());
    m_finder->print();
  }
  if ( m_groomer.empty() ) {
    ATH_MSG_INFO("  Jet groomer is not defined");
  } else {
    ATH_MSG_INFO("  Jet groomer: " << m_groomer->name());
    m_groomer->print();
  }
  if ( m_modifiers.size() ) {
    ATH_MSG_INFO("  Modifier count is " << m_modifiers.size());
    for ( ModifierArray::const_iterator imod=m_modifiers.begin();
           imod!=m_modifiers.end(); ++imod ) {
      ATH_MSG_INFO("    Modifier " << imod->name());
      if ( msgLvl(MSG::DEBUG) ) {
        ToolHandle<IJetModifier> hmod = *imod;
        hmod->print();
      }
    }
  }
}

//**********************************************************************

int JetRecTool::inputContainerNames(std::vector<std::string>& connames) {
  if ( m_initCount == 0 ) {
    ATH_MSG_WARNING("Input container list requested before initialization.");
    return 1;
  }
  connames.insert(connames.end(), m_incolls.begin(), m_incolls.end());
  return 0;
}

//**********************************************************************

int JetRecTool::outputContainerNames(std::vector<std::string>& connames) {
  if ( m_initCount == 0 ) {
    ATH_MSG_WARNING("Output container list requested before initialization.");
    return 1;
  }
  connames.insert(connames.end(), m_outcolls.begin(), m_outcolls.end());
  return 0;
}

//**********************************************************************
