/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingTools/BaseObject.h"

namespace {
    std::string to_string(MSG::Level lvl) {
        if (lvl == MSG::VERBOSE) return "VEBOSE";
        if (lvl == MSG::DEBUG) return "DEBUG";
        if (lvl == MSG::INFO) return "INFO";
        if (lvl == MSG::WARNING) return "WARNING";
        if (lvl == MSG::ERROR) return "ERROR";
        if (lvl == MSG::FATAL) return "FATAL";
        return "UNKNOWN";
    }
}  // namespace

ObjectType BaseObject::tag() const { return m_tag; }
std::string BaseObject::name() const { return m_name; }

void BaseObject::dump_message(__osstream& display, MSG::Level lvl) const {
#ifdef LVL1_STANDALONE
    std::cout << display.str() << std::endl;
#else
    if (m_message) {
        (*m_message) << lvl << display.str() << endmsg;
    } else {
        std::cout << to_string(lvl) << display.str() << std::endl;
    }
#endif
}
bool BaseObject::msgLevel(MSG::Level lvl) const {
    if (m_message) return m_message->level() >= lvl;
    return false;
}

BaseObject::BaseObject(ObjectType tag, const std::string& obj_name, IMessageSvc* msgSvc) : m_tag{tag}, m_name{obj_name}, m_msgSvc{msgSvc} {
    /// To be fixed in a follow up MR
    if (false && !msgSvc) {
        ISvcLocator* svcLocator = Gaudi::svcLocator();
        StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
        if (sc.isFailure()) { std::cout << "Can't locate the MessageSvc" << std::endl; }
    }
    m_message = std::make_unique<MsgStream>(msgSvc, obj_name);
}

BaseObject::BaseObject(const BaseObject& obj) {
    m_tag = obj.tag();
    m_name = obj.name();
    m_msgSvc = obj.m_msgSvc;
    if (m_msgSvc) { m_message = std::make_unique<MsgStream>(m_msgSvc, m_name); }
}
BaseObject& BaseObject::operator=(const BaseObject& obj) {
    if (this != &obj) {
        m_tag = obj.m_tag;
        m_name = obj.m_name;
        m_msgSvc = obj.m_msgSvc;
        if (m_msgSvc)
            m_message = std::make_unique<MsgStream>(m_msgSvc, m_name);
        else
            m_message.reset();
    }
    return *this;
}

void BaseObject::display_error(__osstream& display) const { dump_message(display, MSG::ERROR); }

void BaseObject::display_debug(__osstream& display) const { dump_message(display, MSG::DEBUG); }

void BaseObject::display_warning(__osstream& display) const { dump_message(display, MSG::WARNING); }
void BaseObject::display_info(__osstream& display) const { dump_message(display, MSG::INFO); }
void BaseObject::display_verbose(__osstream& display) const { dump_message(display, MSG::VERBOSE); }
