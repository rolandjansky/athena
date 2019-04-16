#include "TFPersistification/TransFunctionPersistifier.h"
#include "TFPersistification/TransFunctionRecorder.h"
#include "GeoModelKernel/GeoXF.h"

const GenFunctionPersistifier * TransFunctionPersistifier::getGenFunctionPersistifier() const {
  return &fPersistifier;
}

const std::string & TransFunctionPersistifier::getCodedString() const {
  
  codedString=getStream().str();
  delete stream;
  stream=NULL;
  return codedString;
}


TransFunctionPersistifier::~TransFunctionPersistifier() {
  delete stream;
  for (auto i=recorderMap.begin(); i!=recorderMap.end();i++) {
    delete (*i).second;
  }
}
TransFunctionPersistifier::TransFunctionPersistifier():stream(NULL) {
  new PreMultRecorder(this);
  new PostMultRecorder(this);
  new PowRecorder(this);
  new ProductRecorder(this);
}

void TransFunctionPersistifier::add(const std::type_info & tInfo, const TransFunctionRecorder * recorder) {
  recorderMap[tInfo.name()]=recorder;
}

std::ostringstream & TransFunctionPersistifier::getStream() const {
  if (!stream) stream = new std::ostringstream();
  return *stream;
}

void TransFunctionPersistifier::persistify( const GeoXF::Function & f) const {
  
  auto rIter = recorderMap.find(typeid(f).name());
  if (rIter!=recorderMap.end()) {
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
