/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingTools/BaseObject.h"

BaseObject::BaseObject(ObjectType tag, const std::string& obj_name) : m_tag(tag), m_name(obj_name), m_message(nullptr) {
    IMessageSvc* msgSvc = nullptr;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
    if (sc.isFailure()) { std::cout << "Can't locate the MessageSvc" << std::endl; }
    m_message = std::make_shared<MsgStream>(msgSvc, obj_name);
}

BaseObject::BaseObject(const BaseObject& obj) {
    m_tag = obj.tag();
    m_name = obj.name();
    m_message = obj.m_message;
}

BaseObject::~BaseObject() = default;

void BaseObject::display_error(__osstream& display) const {
#ifdef LVL1_STANDALONE
    std::cout << display.str() << std::endl;
#else
    (*m_message) << MSG::ERROR << display.str() << endmsg;
#endif
}

void BaseObject::display_debug(__osstream& display) const {
#ifdef LVL1_STANDALONE
    std::cout << display.str() << std::endl;
#else
    (*m_message) << MSG::DEBUG << display.str() << endmsg;
#endif
}

void BaseObject::display_warning(__osstream& display) const {
#ifdef LVL1_STANDALONE
    std::cout << display.str() << std::endl;
#else
    (*m_message) << MSG::WARNING << display.str() << endmsg;

#endif
}

void BaseObject::display_info(__osstream& display) const {
#ifdef LVL1_STANDALONE
    std::cout << display.str() << std::endl;
#else
    (*m_message) << MSG::INFO << display.str() << endmsg;

#endif
}

BaseObject& BaseObject::operator=(const BaseObject& obj) {
    if (this != &obj) {
        m_tag = obj.m_tag;
        m_name = obj.m_name;
        m_message = obj.m_message;
    }
    return *this;
}
