/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <iostream>
#include <algorithm>

#include <boost/tokenizer.hpp>

#include "GaudiKernel/System.h"

#include "TrigNavigation/Navigation.h"
#include "AthContainers/AuxElement.h"

using namespace HLT;

Navigation::Navigation(  const std::string& type, const std::string& name,
                         const IInterface* parent )
  : AthAlgTool(type, name, parent),
    NavigationCore(msg()),
    m_serializerServiceHandle("TrigSerializeCnvSvc", name),
    m_clidSvc("ClassIDSvc", name),
    m_fullholderfactory(m_objectsKeyPrefix)
{

  declareProperty("ClassesToPayload", m_classesToPayloadProperty,
                  "List of classes which need to be serialized together with the Navigation.");

  declareProperty("ClassesToPayload_DSonly", m_classesToPayloadProperty_DSonly,
                  "List of classes which need to be serialized together with the Navigation (Only in DataScouting collection).");

  declareProperty("ClassesFromPayloadIgnore", m_classesFromPayloadIgnoreProperty,
                  "List of classes (Type[#Key]) to ignore on deserialization of the Navigation.");

  declareProperty("ClassesToPreregister", m_classesToPreregisterProperty,
                  "List of classes which need to be put in SG independently if they appear in event.");

  declareProperty("ObjectsKeyPrefix", m_objectsKeyPrefix="HLT", "The prefix which all Trigger EDM objects will get, by default it is HLT");
  declareProperty("ObjectsIndexOffset", m_objectsIndexOffset=0, "The offset with which the objects idx is be shifted.");
  declareProperty("ReadonlyHolders", m_readonly = false, "read only flag for holders (cannot create new feature containers");
}

Navigation::~Navigation() {
}


StatusCode Navigation::initialize() {

  // initialize some pointers of our base class
  //m_log = &msg();
  m_storeGate = evtStore().operator->();

  ATH_CHECK( m_serializerServiceHandle.retrieve() );
  m_serializerSvc = m_serializerServiceHandle.operator->();

  m_fullholderfactory.prepare(m_storeGate, m_serializerSvc, m_readonly);
  m_holderfactory = &m_fullholderfactory;

  ATH_CHECK(m_clidSvc.retrieve());

  // payload def
  ATH_CHECK( classKey2CLIDKey(m_classesToPayloadProperty,  m_classesToPayload) );
  ATH_CHECK( classKey2CLIDKey(m_classesToPayloadProperty_DSonly,  m_classesToPayload_DSonly) );

  // ignored classes
  std::vector<CSPair> ignore;
  ATH_CHECK( classKey2CLIDKey(m_classesFromPayloadIgnoreProperty, ignore) );
  if ( !ignore.empty() ) {
    ATH_MSG_INFO( "Ignoring " << m_classesFromPayloadIgnoreProperty << " during deserialization");
  }
  for (const auto& c : ignore) {
    ATH_MSG_DEBUG("Ignoring CLID " << c.first << (c.second.empty() ? "" : " with label "+c.second) <<
                 " during deserialization");
    m_fullholderfactory.addClassToIgnore(c.first, c.second);
  }

  // initialize converters
  for (const CSPair& p : m_classesToPayload) {
    const CLID cl = p.first;
    StatusCode stmp = m_serializerSvc->addConverter(cl);
    if (stmp.isFailure())
      ATH_MSG_WARNING("Initialization of a converter for CLID=" << cl << " failed");
  }

  // preregistration def
  ATH_CHECK( classKey2CLIDKey(m_classesToPreregisterProperty,  m_classesToPreregister) );

  // print out registered holders
  if ( msgLvl(MSG::VERBOSE) ) {
    for (const auto& [clid, holder] : HLT::TypeMaps::holders()) {
      if (holder==nullptr)
        ATH_MSG_ERROR("static type information not intialized. Holder is null pointer");
      else
        ATH_MSG_VERBOSE(*holder);
    }
  }

  // load libraries
  for ( const std::string& dll : m_dlls ) {
    System::ImageHandle handle = 0;
    if ( System::loadDynamicLib( dll, &handle)  != 1 )
      ATH_MSG_WARNING("failed to load " << dll);
    else
      ATH_MSG_DEBUG("forcibly loaded library " << dll);
  }

  ATH_MSG_DEBUG(" successfully initialized Navigation");

  return StatusCode::SUCCESS;
}

StatusCode
Navigation::classKey2CLIDKey(const std::vector<std::string>& property,
                             std::vector<CSPair>& decoded ) {
  // translate Class names into CLID numbers

  for ( const std::string& cname : property ) {
    CLID clid{0};
    std::string key;
    std::string type;

    if ( cname.find('#') != std::string::npos ) {
      type = cname.substr(0, cname.find('#') );
      key  = cname.substr(cname.find('#')+1 );
    } else {
      type = cname;
      key = "";
    }

    if ( m_clidSvc->getIDOfTypeName(type, clid).isFailure() ) {
      ATH_MSG_ERROR("Unable to get CLID for class: " << cname);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Recognized CLID : " << type << " and key: " << key);

    xAOD::AuxSelection sel;

    // anything after a dot is a list of dynamic Aux attributes, separated by dots
    size_t dotpos = key.find('.');
    if( dotpos == std::string::npos ) {
      // If no explicit selection, we want to select nothing (this is a
      // difference from the offline logic).  But an empty selection list
      // in AuxSelection means to accept everything.  So add a dummy name
      // that shouldn't match anything.
      const char* dummyName = "__dummyThatShouldNotMatch";
      [[maybe_unused]]
      static const SG::AuxElement::Accessor<int> dummyVar (dummyName);
      static const std::set<std::string> dummySet { dummyName };
      sel.selectAux (dummySet);
    }
    else {
      std::string aux_attr = key.substr(dotpos+1);
      key.erase (dotpos, std::string::npos);

      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep(".");
      tokenizer tokens (aux_attr, sep);
      sel.selectAux (std::set<std::string> (tokens.begin(), tokens.end()));
    }

    decoded.emplace_back (clid, key, std::move (sel));
  }
  return StatusCode::SUCCESS;
}

MsgStream& HLT::operator<< ( MsgStream& m, const Navigation& nav ) {
  m << (NavigationCore&)nav;
  return m;
}
