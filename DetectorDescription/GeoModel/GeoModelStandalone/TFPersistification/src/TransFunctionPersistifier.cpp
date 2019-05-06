#include "TFPersistification/TransFunctionPersistifier.h"
#include "TFPersistification/TransFunctionRecorder.h"
#include "GeoModelKernel/GeoXF.h"

const GenFunctionPersistifier * TransFunctionPersistifier::getGenFunctionPersistifier() const {
  return &m_persistifier;
}

const std::string & TransFunctionPersistifier::getCodedString() const {
  
  m_codedString=getStream().str();
  delete m_stream;
  m_stream=NULL;
  return m_codedString;
}


TransFunctionPersistifier::~TransFunctionPersistifier() {
  delete m_stream;
  for (auto i=m_recorderMap.begin(); i!=m_recorderMap.end();i++) {
    delete (*i).second;
  }
}
TransFunctionPersistifier::TransFunctionPersistifier():m_stream(NULL) {
  new PreMultRecorder(this);
  new PostMultRecorder(this);
  new PowRecorder(this);
  new ProductRecorder(this);
}

void TransFunctionPersistifier::add(const std::type_info & tInfo, const TransFunctionRecorder * recorder) {
  m_recorderMap[tInfo.name()]=recorder;
}

std::ostringstream & TransFunctionPersistifier::getStream() const {
  if (!m_stream) m_stream = new std::ostringstream();
  return *m_stream;
}

void TransFunctionPersistifier::persistify( const GeoXF::Function & f) const {
  
  auto rIter = m_recorderMap.find(typeid(f).name());
  if (rIter!=m_recorderMap.end()) {
    const TransFunctionRecorder *recorder = (*rIter).second;
    recorder->execute(f);
  }
  else {
      std::ostringstream message;
    message << "Ominous warning in TransFunctionPersistier: cannot persistify function with type id: " << typeid(f).name();
    throw std::runtime_error (message.str());
  }
    
    //throw std::runtime_error ("Ominous warning in TransFunctionPersistier:  cannot persistify function");
 
}
