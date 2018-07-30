#include "JetRec/IConstituentExtractor.h"

IConstituentExtractor::~IConstituentExtractor(){}

std::ostream& operator << (std::ostream& os, const IConstituentExtractor* e){
  os <<  e->toString(0);
  return os;
}
