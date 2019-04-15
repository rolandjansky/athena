#include "TFPersistification/TransFunctionInterpreter.h"
#include "TFPersistification/TransFunctionReader.h"
#include "GeoModelKernel/GeoXF.h"

const GenFunctionInterpreter * TransFunctionInterpreter::getGenFunctionInterpreter() const {
  return &m_fInterpreter;
}



TransFunctionInterpreter::~TransFunctionInterpreter() {
  for (auto i=m_readerMap.begin();i!=m_readerMap.end();i++) {
    delete (*i).second;
  }
}

TransFunctionInterpreter::TransFunctionInterpreter(){
  new PowReader(this);
  new PreMultReader(this);
  new PostMultReader(this);
  new ProductReader(this);
}

void TransFunctionInterpreter::add(const std::string & str, const TransFunctionReader * reader) {
  reader=m_readerMap[str]=reader;
}


TFPTR TransFunctionInterpreter::interpret(const std::string &exprString) const {
  size_t begin = exprString.find_first_of("{");
  size_t end = exprString.find_last_of("}");
  if (!((begin!=std::string::npos) && (end!=std::string::npos))) {
    throw std::runtime_error ("Ominous warning in TransFunctionPersistier:  cannot parse function");
  }
  std::string op = exprString.substr(0,begin);
  std::string argument = exprString.substr(begin+1,end-begin-1);
  // for debug only!
  // std::cout << begin << " " << end << " " << op << " " << argument << std::endl;
  // for (auto const& x : readerMap) {
  //     std::cout << x.first  // string (key)
  //               << " : "
  //               << x.second // string's value
  //               << std::endl ;
  // }
  auto rIter = m_readerMap.find(op);
  if (rIter==m_readerMap.end()) {
    throw std::runtime_error ("Ominous warning in TransFunctionPersistier:  cannot interpret function");
  }
  const TransFunctionReader *reader = (*rIter).second;
  return reader->execute(argument);
}
