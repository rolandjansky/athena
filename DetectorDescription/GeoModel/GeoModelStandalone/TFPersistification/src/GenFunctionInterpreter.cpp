#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/ArrayFunctionIO.h"
#include "TFPersistification/ModIO.h"
#include "TFPersistification/FixedConstantIO.h"
#include "TFPersistification/RectangularIO.h"
#include "TFPersistification/AbsIO.h"
#include "TFPersistification/SinIO.h"
#include "TFPersistification/ASinIO.h"
#include "TFPersistification/CosIO.h"
#include "TFPersistification/ACosIO.h"
#include "TFPersistification/TanIO.h"
#include "TFPersistification/ATanIO.h"
#include "TFPersistification/SqrtIO.h"
#include "TFPersistification/SquareIO.h"
#include "TFPersistification/GenfunIO.h"

#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Argument.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoXF.h"

#include <iterator>

GenFunctionInterpreter::~GenFunctionInterpreter() {
  for (auto i=m_readerMap.begin();i!=m_readerMap.end();i++) delete (*i).second;
}

GenFunctionInterpreter::GenFunctionInterpreter() {
  new ArrayFunctionReader(this);
  new ModReader(this);
  new FixedConstantReader(this);
  new RectangularReader(this);
  new SinReader(this);
  new CosReader(this);
  new TanReader(this);
  new ASinReader(this);
  new ACosReader(this);
  new ATanReader(this);
  new AbsReader(this);
  new SqrtReader(this);
  new SquareReader(this);
  new VariableReader(this);
  new ConstTimesFunctionReader(this);
  new ConstPlusFunctionReader(this);
  new ConstMinusFunctionReader(this);
  new ConstOverFunctionReader(this);
  new FunctionCompositionReader(this);
  new FunctionProductReader(this);
  new FunctionSumReader(this);
  new FunctionQuotientReader(this);
  new FunctionDifferenceReader(this);
  new FunctionNegationReader(this);
  new FunctionNoopReader(this);
}

void GenFunctionInterpreter::add(const std::string & str, const GenFunctionReader * reader) {
  reader=m_readerMap[str]=reader;
}


GFPTR GenFunctionInterpreter::interpret(std::string::const_iterator sBegin, std::string::const_iterator sEnd) const {

  auto begin=std::find(sBegin, sEnd, '(');
  std::reverse_iterator<std::string::const_iterator> rBegin(sEnd);
  std::reverse_iterator<std::string::const_iterator> rEnd(sBegin);
  auto end=std::find(rBegin, rEnd, ')').base();
  

  if (end==sBegin || begin==sEnd) {
    // Base case. This is the bottom of the tree!
    std::string exprString(sBegin,sEnd);
    auto rIter = m_readerMap.find(exprString);
    if (rIter==m_readerMap.end()) {
      std::ostringstream stream;
      stream << "Ominous warning in GenFunctionInterpreter: cannot parse function " << exprString;
      throw std::runtime_error (stream.str());
    }
    const GenFunctionReader *reader = (*rIter).second;
    return reader->execute(begin,begin);//exec w/null expression
  }
  std::string op(sBegin,begin);
  auto argBegin=begin+1, argEnd=end-1;
  auto rIter = m_readerMap.find(op);
  if (rIter==m_readerMap.end()) {
    std::ostringstream stream;
    stream << "Ominous warning in GenFunctionInterpreter: cannot interpret function " << op;
    throw std::runtime_error (stream.str());
  }
  const GenFunctionReader *reader = (*rIter).second;
  return reader->execute(argBegin,argEnd);
}
