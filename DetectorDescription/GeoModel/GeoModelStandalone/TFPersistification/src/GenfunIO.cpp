#define private public
#include "GeoGenericFunctions/AbsFunction.h"
#undef  private

#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenfunIO.h"

#include "GeoGenericFunctions/Variable.h"

#include <sstream>

ConstTimesFunctionReader::ConstTimesFunctionReader(GenFunctionInterpreter *interpreter):GenFunctionReader("ConstTimesFunction", interpreter) {}

inline double atof(std::string::const_iterator begin, std::string::const_iterator end) {

  std::string atom(begin,end);
  std::istringstream stream(atom);
  double x=0;
  stream >> x;
  return x;
  
}

GFPTR  ConstTimesFunctionReader::execute(std::string::const_iterator begin, std::string::const_iterator end) const {
  auto sep=split(begin,end);
  double c=atof(begin,sep);
  return GFPTR(new GeoGenfun::ConstTimesFunction(c,getInterpreter()->interpret(sep+1,end).get()));
}


ConstTimesFunctionRecorder::ConstTimesFunctionRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::ConstTimesFunction), persistifier) {}

void ConstTimesFunctionRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::ConstTimesFunction * ptr = dynamic_cast<const GeoGenfun::ConstTimesFunction *> (&F);
  if (!ptr) throw std::runtime_error("Error in ConstTimesFunctionRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  double c1=ptr->_constant;
  stream << "ConstTimesFunction" << "(";
  stream << c1;
  stream << ",";
  getPersistifier()->persistify(*ptr->_arg);
  stream << ")";
}

ConstPlusFunctionReader::ConstPlusFunctionReader(GenFunctionInterpreter *interpreter):GenFunctionReader("ConstPlusFunction", interpreter) {}


GFPTR  ConstPlusFunctionReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  auto sep=split(begin, end);
  double c=atof(begin,sep);
  return GFPTR(new GeoGenfun::ConstPlusFunction(c,getInterpreter()->interpret(sep+1,end).get()));
}

ConstPlusFunctionRecorder::ConstPlusFunctionRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::ConstPlusFunction),persistifier) {}

void ConstPlusFunctionRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::ConstPlusFunction * ptr = dynamic_cast<const GeoGenfun::ConstPlusFunction *> (&F);
  if (!ptr) throw std::runtime_error("Error in ConstPlusFunctionRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  double c1=ptr->_constant;
  stream << "ConstPlusFunction" << "(";
  stream << c1;
  stream << ",";
  getPersistifier()->persistify(*ptr->_arg);
  stream << ")";
}

ConstMinusFunctionReader::ConstMinusFunctionReader(GenFunctionInterpreter *interpreter):GenFunctionReader("ConstMinusFunction", interpreter) {}


GFPTR  ConstMinusFunctionReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  auto sep=split(begin,end);
  double c=atof(begin,sep);
  return GFPTR(new GeoGenfun::ConstMinusFunction(c,getInterpreter()->interpret(sep+1,end).get()));
}

ConstMinusFunctionRecorder::ConstMinusFunctionRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::ConstMinusFunction),persistifier) {}

void ConstMinusFunctionRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::ConstMinusFunction * ptr = dynamic_cast<const GeoGenfun::ConstMinusFunction *> (&F);
  if (!ptr) throw std::runtime_error("Error in ConstMinusFunctionRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  double c1=ptr->_constant;
  stream << "ConstMinusFunction" << "(";
  stream << c1;
  stream << ",";
  getPersistifier()->persistify(*ptr->_arg);
  stream << ")";
}

ConstOverFunctionReader::ConstOverFunctionReader(GenFunctionInterpreter *interpreter):GenFunctionReader("ConstOverFunction", interpreter) {}


GFPTR  ConstOverFunctionReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  auto sep=split(begin,end);
  double c=atof(begin,sep);
  return GFPTR(new GeoGenfun::ConstOverFunction(c,getInterpreter()->interpret(sep+1,end).get()));
}

ConstOverFunctionRecorder::ConstOverFunctionRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::ConstOverFunction),persistifier) {}

void ConstOverFunctionRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::ConstOverFunction * ptr = dynamic_cast<const GeoGenfun::ConstOverFunction *> (&F);
  if (!ptr) throw std::runtime_error("Error in ConstOverFunctionRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  double c1=ptr->_constant;
  stream << "ConstOverFunction" << "(";
  stream << c1;
  stream << ",";
  getPersistifier()->persistify(*ptr->_arg);
  stream << ")";
}

FunctionCompositionReader::FunctionCompositionReader(GenFunctionInterpreter *interpreter):GenFunctionReader("FunctionComposition", interpreter) {}


GFPTR  FunctionCompositionReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  auto sep=split(begin,end);
  return GFPTR(new GeoGenfun::FunctionComposition(getInterpreter()->interpret(begin,sep).get(),getInterpreter()->interpret(sep+1,end).get()));
}

FunctionCompositionRecorder::FunctionCompositionRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::FunctionComposition), persistifier) {}

void FunctionCompositionRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::FunctionComposition * ptr = dynamic_cast<const GeoGenfun::FunctionComposition *> (&F);
  if (!ptr) throw std::runtime_error("Error in FunctionCompositionRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "FunctionComposition(";
  getPersistifier()->persistify(*ptr->_arg1);
  stream << ",";
  getPersistifier()->persistify(*ptr->_arg2);
  stream << ")";
}

FunctionNegationReader::FunctionNegationReader(GenFunctionInterpreter *interpreter):GenFunctionReader("FunctionNegation",interpreter) {}


GFPTR  FunctionNegationReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  return GFPTR(new GeoGenfun::FunctionNegation(getInterpreter()->interpret(begin,end).get()));
}

FunctionNegationRecorder::FunctionNegationRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::FunctionNegation),persistifier) {}

void FunctionNegationRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::FunctionNegation * ptr = dynamic_cast<const GeoGenfun::FunctionNegation *> (&F);
  if (!ptr) throw std::runtime_error("Error in FunctionNegationRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream  << "FunctionNegation(";
  getPersistifier()->persistify(*ptr->_arg1);
  stream << ")";
}

FunctionNoopReader::FunctionNoopReader(GenFunctionInterpreter *interpreter):GenFunctionReader("FunctionNoop",interpreter) {}


GFPTR  FunctionNoopReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  return GFPTR(new GeoGenfun::FunctionNoop(getInterpreter()->interpret(begin,end).get()));
}

FunctionNoopRecorder::FunctionNoopRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::FunctionNoop),persistifier) {}

void FunctionNoopRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::FunctionNoop * ptr = dynamic_cast<const GeoGenfun::FunctionNoop *> (&F);
  if (!ptr) throw std::runtime_error("Error in FunctionNoopRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream  << "FunctionNoop(";
  getPersistifier()->persistify(*ptr->_arg1);
  stream << ")";
}

FunctionProductReader::FunctionProductReader(GenFunctionInterpreter *interpreter):GenFunctionReader("FunctionProduct",interpreter) {}


GFPTR  FunctionProductReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  auto sep=split(begin,end);
  return GFPTR(new GeoGenfun::FunctionProduct(getInterpreter()->interpret(begin,sep).get(),getInterpreter()->interpret(sep+1,end).get()));
}

FunctionProductRecorder::FunctionProductRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::FunctionProduct), persistifier) {}

void FunctionProductRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::FunctionProduct * ptr = dynamic_cast<const GeoGenfun::FunctionProduct *> (&F);
  if (!ptr) throw std::runtime_error("Error in FunctionProductRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "FunctionProduct(";
  getPersistifier()->persistify(*ptr->_arg1);
  stream << ",";
  getPersistifier()->persistify(*ptr->_arg2);
  stream << ")";
}

FunctionSumReader::FunctionSumReader(GenFunctionInterpreter *interpreter):GenFunctionReader("FunctionSum",interpreter) {}


GFPTR  FunctionSumReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  auto sep=split(begin,end);
  return GFPTR(new GeoGenfun::FunctionSum(getInterpreter()->interpret(begin,sep).get(),getInterpreter()->interpret(sep+1,end).get()));
}

FunctionSumRecorder::FunctionSumRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::FunctionSum), persistifier) {}

void FunctionSumRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::FunctionSum * ptr = dynamic_cast<const GeoGenfun::FunctionSum *> (&F);
  if (!ptr) throw std::runtime_error("Error in FunctionSumRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "FunctionSum(";
  getPersistifier()->persistify(*ptr->_arg1);
  stream << ",";
  getPersistifier()->persistify(*ptr->_arg2);
  stream << ")";
}






FunctionQuotientReader::FunctionQuotientReader(GenFunctionInterpreter *interpreter):GenFunctionReader("FunctionQuotient",interpreter) {}


GFPTR  FunctionQuotientReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  auto sep=split(begin,end);
  return GFPTR(new GeoGenfun::FunctionQuotient(getInterpreter()->interpret(begin,sep).get(),getInterpreter()->interpret(sep+1,end).get()));
}

FunctionQuotientRecorder::FunctionQuotientRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::FunctionQuotient), persistifier) {}

void FunctionQuotientRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::FunctionQuotient * ptr = dynamic_cast<const GeoGenfun::FunctionQuotient *> (&F);
  if (!ptr) throw std::runtime_error("Error in FunctionQuotientRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "FunctionQuotient(";
  getPersistifier()->persistify(*ptr->_arg1);
  stream << ",";
  getPersistifier()->persistify(*ptr->_arg2);
  stream << ")";
}

FunctionDifferenceReader::FunctionDifferenceReader(GenFunctionInterpreter *interpreter):GenFunctionReader("FunctionDifference",interpreter) {}


GFPTR  FunctionDifferenceReader::execute(std::string::const_iterator begin, std::string::const_iterator end ) const {
  auto sep=split(begin,end);
  return GFPTR(new GeoGenfun::FunctionDifference(getInterpreter()->interpret(begin,sep).get(),getInterpreter()->interpret(sep+1,end).get()));
}

FunctionDifferenceRecorder::FunctionDifferenceRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::FunctionDifference), persistifier) {}

void FunctionDifferenceRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::FunctionDifference * ptr = dynamic_cast<const GeoGenfun::FunctionDifference *> (&F);
  if (!ptr) throw std::runtime_error("Error in FunctionDifferenceRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "FunctionDifference(";
  getPersistifier()->persistify(*ptr->_arg1);
  stream << ",";
  getPersistifier()->persistify(*ptr->_arg2);
  stream << ")";
}





VariableReader::VariableReader(GenFunctionInterpreter *interpreter):GenFunctionReader("X",interpreter) {}


GFPTR  VariableReader::execute(std::string::const_iterator, std::string::const_iterator) const {
  return GFPTR(new GeoGenfun::Variable());
}


VariableRecorder::VariableRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::Variable), persistifier) {}

void VariableRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::Variable * ptr = dynamic_cast<const GeoGenfun::Variable *> (&F);
  if (!ptr) throw std::runtime_error("Error in VariableRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "X";
}
