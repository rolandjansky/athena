/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File:    TrigCountDumper.cxx
 * Author:  Roger Moore <rwmoore@ualberta.ca>
 * Created: 5/11/11 (Guy Fawkes Day!)
 * Description:
 *    Source code for the methods of the TrigCountDumper Athena algorithm.
 */

// System includes
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

// Xerces C++ includes
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/StdOutFormatTarget.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"
	
// Gaudi includes
#include "SGTools/crc64.h"

// ATLAS Includes
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigValAlgs/TrigCountDumper.h"

#include "TrigConfigSvc/DSConfigSvc.h"

#include "TrigSteeringEvent/Lvl1Item.h"

using namespace xercesc;
using namespace trigcount;

// Helper class in a local namespace to convert normal strings into
// XML compatible unicode strings
namespace {
  class XStr {
    public:
      /** Constructor taking a templated argument type.
       This constructor takes a single argument of a given type. First
       it uses a string stream to convert the type into a string and
       then it transcodes the string into an XML unicode string which
       is stored inside the class.
       @param toTranscode data to transcode into XML unicode
      */
      template<typename T> XStr(T toTranscode);
      /** Constructor taking a const character pointer.
       This constructor will transcode the string given by the pointer.
       As a non-template constructor this should always take precedence over
       the templated constructor.
       @param toTranscode pointer to the string to stranscode
      */
      XStr(const char *toTranscode);
      ~XStr() {
        xercesc::XMLString::release(&m_unicode);
      }
      const XMLCh *unicode(void) const {
        return m_unicode;
      }
      /** Cast operator to allow this class to act as an XMLCh pointer.
       This cast operator means that a class instance can be used as a
       XMLCh pointer. This allows an instance to be passed as an argument
       to Xerces function which require a const XMLCh pointer argument. Since
       the class has a templated constructor using a string stream this means
       that simple strings, integers or floats can be passed as arguments
       very simply.
       @return XMLCh pointer with the transcoded string
      */
      operator const XMLCh *(void) const;
    private :
      XMLCh *m_unicode;
  };

  // Templated constructor
  template<typename T> inline XStr::XStr(T toTranscode) {
    std::stringstream ss;
    ss << toTranscode;
    m_unicode=xercesc::XMLString::transcode(ss.str().c_str());
  }

  // Constructor for const char pointers
  inline XStr::XStr(const char *toTranscode) {
    m_unicode=xercesc::XMLString::transcode(toTranscode);
  }
  
  inline XStr::operator const XMLCh *(void) const {
    return m_unicode;
  }
}
// Use a pre-processor macro to define the conversion function because a
// function call would cause the XStr class to go out of scope
// when it returned which would release the converted string data!
//#define X(str) XStr(str).unicode()	

namespace trigcount {

  // Method implementations for the TriggerCount class
  DOMElement *TriggerCount::xml(DOMDocument *doc) const {
    // Create the trigger element to contain the data
    DOMElement *top=doc->createElement(XStr("count"));
    // Add the trigger name as an attribute
    top->setAttribute(XStr("trigger"), XStr(m_name));
    // Add the prescale as an attribute
    top->setAttribute(XStr("prescale"), XStr(m_prescale));
    // Add the passthrough as an attribute
    top->setAttribute(XStr("passthrough"), XStr(m_passthrough));
    // Add the lower chain name as an attribute
    top->setAttribute(XStr("lowerchain"), XStr(m_lower));
    // Create the trigger element to contain the raw count and put the
    // value attribute to the number of counts
    DOMElement *raw=doc->createElement(XStr("raw"));
    raw->setAttribute(XStr("value"),XStr(m_raw));
    // Add the raw count to the top element
    top->appendChild(raw);
    // Create the trigger element to contain the actual count and put the
    // value attribute to the number of counts
    DOMElement *actual=doc->createElement(XStr("actual"));
    actual->setAttribute(XStr("value"),XStr(m_count));
    // Add the actual count to the top element
    top->appendChild(actual);
    return top;
  }
  
  std::iostream &TriggerCount::operator<<(std::iostream &ostr) const {
    ostr << "<count trigger=\"" << m_name << "\" count=\"" 
    << m_count << "\" prescale=\"" << m_prescale << "\"/>" << std::endl;
    return ostr;
  }

  
  // Method implementations for the TriggerMenu class

  DOMElement *TriggerMenu::xml(DOMDocument *doc) const {
    // Create the menu element to contain the triggers
    DOMElement *top=doc->createElement(XStr("menu"));
    // Add the super master key, event count and mean pileup
    // as attributes to the top level element
    top->setAttribute(XStr("smk"), XStr(m_smk));
    top->setAttribute(XStr("nevents"), XStr(m_count));
    top->setAttribute(XStr("mu"), XStr(mu()));
    // Loop over all the triggers in the menu and add elements
    // for then as children of the menu element
    for(TriggerMenu::const_iterator i=this->begin();i!=this->end();++i) {
      top->appendChild(i->second.xml(doc));
    }
    return top;
  }
  
  std::iostream &TriggerMenu::operator<<(std::iostream &ostr) const {
    ostr << "<menu smk=\"" << m_smk << "\" nevents=\"" 
    << m_count << "\" mu=\"" << mu() << "\">" << std::endl;
    for(TriggerMenu::const_iterator i=begin();i!=end();++i) {
      ostr << "  "; //<< i->second;
    }
    ostr << "</menu>" << std::endl;
    return ostr;
  }
  
} // end trigcount namespace


TrigCountDumper::TrigCountDumper(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_configSvc( "TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_dsSvc( "TrigConf::DSConfigSvc/DSConfigSvc", name) {
  declareProperty("OutputFile",m_ofname="trigger_counts.xml","Name of the output file");
  declareProperty("Release",m_release="","Release version");
  declareProperty("Dataset",m_dataset="","Dataset name");
  declareProperty("Labels",m_labels,"Dataset labels");
  declareProperty("EventInfoName",m_eventInfoName="","The name of the EventInfo container" );
}

StatusCode TrigCountDumper::initialize(void) {
  ATH_MSG_INFO("Initializing Trigger Count dump algorithm");
  ATH_MSG_INFO("Outputfile : " << m_ofname);
  // Get the handle to the trigger configuration service
  CHECK(m_configSvc.retrieve());
  // Get the handle to the trigger decision tool
  CHECK(m_trigDec.retrieve());
  // Enable the expert methods for the trigger decision tool
  m_trigDec->ExperimentalAndExpertMethods()->enable();
  return StatusCode::SUCCESS;
}


StatusCode TrigCountDumper::finalize() {
  ATH_MSG_INFO("TrigCountDumper: Generating XML document");
  // Try to initialize the Xerces XML library. Details about this come
  // from the Xerces C++ documentation at http://xerces.apache.org/xerces-c/
  // The basic code framework to create and fill the document was taken from
  // the Xerces sample code CreateDOMDocument available here:
  // http://svn.apache.org/viewvc/xerces/c/trunk/samples/src/CreateDOMDocument/
  try {
    // Initialize the library. Must call terminate function once we are done
    XMLPlatformUtils::Initialize();
  } catch (const XMLException& toCatch) {
    // The library failed to initialize so generate an error message and exit
    ATH_MSG_ERROR("TrigCountDumper: Failed to initialize Xerces XML library");
    return StatusCode::FAILURE;
  }
  // Get a pointer to the Xerces DOM implementation. DOM is an API to access XML
  // documents in memory and so is the API suitable for generating a new XML
  // document from scratch. DOM = Document Object Model
  DOMImplementation *dom=DOMImplementationRegistry::getDOMImplementation(XStr("Core"));
  if(!dom) {
    // There was no DOM implementation found in Xerces so print an error message
    // and then exit
    ATH_MSG_ERROR("Unable to find a DOM XML implementation in Xerces");
    XMLPlatformUtils::Terminate();
    return StatusCode::FAILURE;
  }
  // Create a new XML document which we will fill with the trigger counts
  DOMDocument *doc=dom->createDocument(0,                     // root element namespace URI
                                       XStr("TriggerCounts"), // root element name
                                       0);                    // document type object (DTD).
  // Get the root element of the document
  DOMElement *rootElem=doc->getDocumentElement();
  // Create the dataset element
  DOMElement *ds=doc->createElement(XStr("dataset"));
  // Add a name attribute to the dataset element
  ds->setAttribute(XStr("name"),XStr(m_dataset));
  // Add a release attribute to the dataset element
  ds->setAttribute(XStr("release"),XStr(m_release));
  // Loop over the configured labels and add these to the dataset
  for(std::vector<std::string>::const_iterator i=m_labels.begin();
      i!=m_labels.end();++i) {
    // Create an element for the label
    DOMElement *label=doc->createElement(XStr("label"));
    // Create a text node containing the label text
    DOMText *labtext=doc->createTextNode(XStr(*i));
    // Add the text node to the label
    label->appendChild(labtext);
    // Add the label to the dataset
    ds->appendChild(label);
  }
  // Add the dataset element to the root
  rootElem->appendChild(ds);
  // Loop over all the menus found in the data so they can be added to the document
  for(MenuMap_t::const_iterator i=m_menu.begin();i!=m_menu.end();++i) {
    rootElem->appendChild(i->second.xml(doc));
  }
  // At this point we have all the data stored in the XML document so now we
  // write it out to a file...which is somewhat of an involved process!
  // First we have to create a serializer from the DOM implementation
  DOMLSSerializer *serializer=static_cast<DOMImplementationLS *>(dom)->createLSSerializer();
  // Make the output more human readable by inserting line feeds.
  if (serializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
    serializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
  // The end-of-line sequence of characters to be used in the XML being written out (UNIX standard)
  serializer->setNewLine(XStr("\n"));
  // Create the output target
  XMLFormatTarget *target;
  if(!m_ofname.empty())
    target=new LocalFileFormatTarget(XStr(m_ofname));
  else
    target=new StdOutFormatTarget();
  // Create a new empty output destination object
  DOMLSOutput *output=static_cast<DOMImplementationLS *>(dom)->createLSOutput();
  // Tell the output stream to use the given file, or stdout, target
  output->setByteStream(target);
  // Tell everyone that we are about to write the XML data out
  ATH_MSG_INFO("TrigCountDumper: Writing output to " << m_ofname);
  // Write the document to the output using the serializer
  serializer->write(doc,output);
  // Cleanup.
  serializer->release();
  delete target;
  output->release();
  return StatusCode::SUCCESS;
}


StatusCode TrigCountDumper::execute(void) {
  // Get a reference to the expert interface
  Trig::ExpertMethods &expert=*m_trigDec->ExperimentalAndExpertMethods();
  // Get the super master and pre-scale keys from the configuration service.
  // It is not clear that we will use the prescale keys but we'll retreive
  // them in case we need them in the future.
  uint32_t smk    = m_configSvc->masterKey();
  uint32_t l1psk  = m_configSvc->lvl1PrescaleKey();
  uint32_t hltpsk = m_configSvc->hltPrescaleKey();
  // If the keys returned by the configuration service don't seem to make sense,
  // use something else as the SMK. (Needed mostly for MC test jobs.)
  if(((smk==0) && (l1psk==0) && (hltpsk==0)) || (static_cast<int>(smk) < 0 ) ||
     (static_cast<int>(l1psk) < 0 ) || (static_cast<int>(hltpsk) < 0 ) ) {
    // See if we are reading an AOD:
    if(!m_dsSvc) {
      ATH_MSG_FATAL("The trigger configuration keys don't seem to make sense, and we're not using "
        << "TrigConf::DSConfigSvc...");
      return StatusCode::FAILURE;
    }
    TrigConf::DSConfigSvc *dsSvc=dynamic_cast<TrigConf::DSConfigSvc *>(m_dsSvc.operator->());
    if(!dsSvc) {
      ATH_MSG_FATAL("The trigger configuration keys don't seem to make sense, and we're not using "
        << "TrigConf::DSConfigSvc...");
      return StatusCode::FAILURE;
    }
    // Turn the configuration source name (probably an XML file in this case) into an
    // imaginary Super Master Key:
    smk    = SG::crc64(dsSvc->configurationSource()) & 0xffff;
    l1psk  = 0;
    hltpsk = 0;
  }

  // At this point we now have a super master key so check whether this is one we have
  // already seen or whether we need to create a new menu to count it
  if(m_menu.find(smk)==m_menu.end()) {
    ATH_MSG_INFO("Found new trigger menu with SMK=" << smk);
    // This super master key was not found so create a new menu entry
    // Note: this is inefficient in that it creates and then copies the trigger
    // menu into the map. However this should only ever be performed once or twice
    // per run so the overhead is minimal and it avoids have to worry about memory
    // management (so call me lazy!).
    m_menu[smk]=trigcount::TriggerMenu(smk);
    // Get a list of all the L1 triggers in this menu configuration
    std::vector<std::string> trigs=m_trigDec->getListOfTriggers("L1_.*");
    // Loop over all the L1 triggers and add them to the count menu
    for(std::vector<std::string>::const_iterator i=trigs.begin();i!=trigs.end();++i) {
      ATH_MSG_DEBUG("Adding L1 trigger " << *i);
      const LVL1CTP::Lvl1Item *l1=expert.getItemDetails(*i);
      // Check that there is a valid L1 item pointer, if not then raise an error
      if(!l1) {
        ATH_MSG_ERROR("No Lvl1Item class found for " << *i << ", set prescale to -1");
        m_menu[smk].addTrigger(trigcount::TriggerCount(*i,-1.,0.));
      } else {
        m_menu[smk].addTrigger(trigcount::TriggerCount(*i,l1->prescaleFactor(),0.));
      }
    }
    // Get a list of all the HLT triggers in this menu configuration
    trigs=m_trigDec->getListOfTriggers("HLT_.*");
    // Loop over all the triggers and add them to the count menu
    for(std::vector<std::string>::const_iterator i=trigs.begin();i!=trigs.end();++i) {
      ATH_MSG_DEBUG("Adding HLT trigger " << *i 
                    << " with prescale " << m_trigDec->getPrescale(*i));
      const TrigConf::HLTChain *ch=expert.getChainConfigurationDetails(*i);
      // Check for the expert chain interface
      if(!ch) {
        ATH_MSG_ERROR("No Chain class found for " << *i << ", set passthrough to -1");
        m_menu[smk].addTrigger(trigcount::TriggerCount(*i,m_trigDec->getPrescale(*i),-1.));
      } else {
        m_menu[smk].addTrigger(trigcount::TriggerCount(*i,ch->prescale(),ch->pass_through()));
        m_menu[smk][*i].setLower(ch->lower_chain_name());
      }
    }
    ATH_MSG_DEBUG("Trigger menu configured");
  }
  
  // Now that we know we have a trigger menu for this event get a reference to it.
  // This avoids having to look it up in the map everytime we access it.
  trigcount::TriggerMenu &menu=m_menu[smk];
  // Extract the event info object from store gate
  const EventInfo *eventInfo;
  StatusCode  sc;
  if (m_eventInfoName == "") {
    sc=evtStore()->retrieve(eventInfo);
  } else {
    sc=evtStore()->retrieve(eventInfo ,m_eventInfoName);
  }
  // Check for a null point or failure code because the call can succeed but not
  // find a valid event info object pointer
  if(sc.isFailure() || !eventInfo) {
    ATH_MSG_INFO("EventInfo container '" << m_eventInfoName
                 << "' could not be retrieved from StoreGate!");
    return StatusCode::SUCCESS;
  }
  
  // Add an event to the menu with the given number of interactions
  menu.addEvent(eventInfo->actualInteractionsPerCrossing());
  
  // Loop over all the HLT triggers to find which ones passed
  std::vector<std::string> trigs=m_trigDec->getListOfTriggers("HLT_.*");
  for(std::vector<std::string>::const_iterator i=trigs.begin();i!=trigs.end();++i) {
    // Check to see if the trigger passed regardless of any prescale value. If it does then
    // add this event to the count
    if(m_trigDec->isPassed(*i,TrigDefs::allowResurrectedDecision|TrigDefs::requireDecision)) {
      // Access the actual pass value, with prescale implemented. If this is true then the
      // actual count will be incremented as well as the raw count
      menu[*i].addEvent(m_trigDec->isPassed(*i));
    }
  }

  // Loop over all the L1 triggers to find which ones passed
  trigs=m_trigDec->getListOfTriggers("L1_.*");
  for(std::vector<std::string>::const_iterator i=trigs.begin();i!=trigs.end();++i) {
    const LVL1CTP::Lvl1Item *l1=expert.getItemDetails(*i);
    // Check that there is a valid L1 item pointer, if not then raise an error
    if(!l1) {
      ATH_MSG_ERROR("No Lvl1Item class found for " << *i << ", using TrigDecision data only");
      if(m_trigDec->isPassed(*i,TrigDefs::allowResurrectedDecision|TrigDefs::requireDecision)) {
        // Access the actual pass value, with prescale implemented. If this is true then the
        // actual count will be incremented as well as the raw count
        menu[*i].addEvent(m_trigDec->isPassed(*i));
      }
    } else {
      // We have a Lvl1Item class so we use this to get the raw and prescaled counts
      if(l1->isPassedBeforePrescale()) {
        menu[*i].addEvent(l1->isPassed());
      }
    }
  } // end loop over L1 triggers
  return StatusCode::SUCCESS;
}
