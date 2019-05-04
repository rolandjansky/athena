#include "TFPersistification/GenFunctionRecorder.h"
#include "TFPersistification/GenFunctionPersistifier.h"
#include <stdexcept>
#include <typeinfo>


GenFunctionRecorder::GenFunctionRecorder(const std::type_info & info, GenFunctionPersistifier * persistifier):
  m_persistifier(persistifier) {
  persistifier->add(info, this);
}

const GenFunctionPersistifier *GenFunctionRecorder::getPersistifier() const {
  return m_persistifier;
}

GenFunctionRecorder::~GenFunctionRecorder () {}
