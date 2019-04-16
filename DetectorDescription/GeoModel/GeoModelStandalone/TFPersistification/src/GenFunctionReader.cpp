#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/GenFunctionReader.h"

#include <stdexcept>



GenFunctionReader::GenFunctionReader(const std::string & name, GenFunctionInterpreter * interpreter):
  m_name(name),m_interpreter(interpreter) {
  interpreter->add(name, this);
}

const std::string & GenFunctionReader::getName() const {
  return m_name;
}

const GenFunctionInterpreter *GenFunctionReader::getInterpreter() const {
  return m_interpreter;
}
#ifdef NOT_THIS_ONE
std::string::const_iterator GenFunctionReader::split(std::string::const_iterator begin, std::string::const_iterator end) const {
  auto cSearch=begin;
  
  while (1) {
    auto cComma=std::find(cSearch,end,',');
    int cOpen=std::count(begin,cComma,'(');
    int cClose=std::count(begin,cComma,')');
    if (cOpen==cClose) return cComma;
    cSearch = ++cComma;
  }
  //
  // Make the compiler happy, but the statement should never occur.
  //
  return end; 
}

#endif
std::string::const_iterator GenFunctionReader::split(std::string::const_iterator begin, std::string::const_iterator end) const {
  using namespace std;

  auto cSearch=begin;
  std::string::const_iterator cComma=end;

  int pCount=0;
  while (1) {
    for (auto i=cSearch;i!=end;i++) {
      char c=*i;
      if (c==',') {
	cComma=i;
	if (pCount==0) {
	  return cComma;
	}
	else {
	  break;
	}
      }
      else if (c==')') {
	pCount--;
      }
      else if (c=='(') {
	pCount++;
      }
    }
    cSearch = ++cComma;
  }
  return end;
}

GenFunctionReader::~GenFunctionReader () {}

