/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "KitManager.h"
#include <sstream>
#include <stdexcept>

KitManagerBase::KitManagerBase() {}
KitManagerBase::~KitManagerBase() {}

const void *KitManagerBase::kitPtr(const std::string &name) const {
  std::lock_guard<std::mutex> lock (m_mutex);
  return m_registry.at(name);
}

bool KitManagerBase::registerKit(const std::string& name, const void *a_kit) {
  std::lock_guard<std::mutex> lock (m_mutex);
  std::pair<std::string, const void *> elm = std::make_pair(name, a_kit);
  if (!m_registry.insert(elm).second) {
    std::stringstream message;
    message << "Failed to register kit " << elm.first;
    throw std::runtime_error(message.str());
  }
  return true;
}

void KitManagerBase::dumpKits(std::ostream &out) const {
  std::lock_guard<std::mutex> lock (m_mutex);
  for(const std::pair<const std::string, const void *> &elm: m_registry) {
    out << " " << elm.first;
  }
}
