#ifndef _GenfunIO_h_
#define _GenfunIO_h_

#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"


class ConstTimesFunctionReader: public GenFunctionReader {

 public:
  
  ConstTimesFunctionReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};

class ConstTimesFunctionRecorder: public GenFunctionRecorder {

 public:
  
  ConstTimesFunctionRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};


class ConstPlusFunctionReader: public GenFunctionReader {

 public:
  
  ConstPlusFunctionReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class ConstPlusFunctionRecorder: public GenFunctionRecorder {

 public:
  
  ConstPlusFunctionRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};

class ConstMinusFunctionReader: public GenFunctionReader {

 public:
  
  ConstMinusFunctionReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class ConstMinusFunctionRecorder: public GenFunctionRecorder {

 public:
  
  ConstMinusFunctionRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};

class ConstOverFunctionReader: public GenFunctionReader {

 public:
  
  ConstOverFunctionReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class ConstOverFunctionRecorder: public GenFunctionRecorder {

 public:
  
  ConstOverFunctionRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};


class FunctionCompositionReader: public GenFunctionReader {

 public:
  
  FunctionCompositionReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class FunctionCompositionRecorder: public GenFunctionRecorder {

 public:
  
  FunctionCompositionRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};


class FunctionNegationReader: public GenFunctionReader {

 public:
  
  FunctionNegationReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class FunctionNegationRecorder: public GenFunctionRecorder {

 public:
  
  FunctionNegationRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};

class FunctionNoopReader: public GenFunctionReader {

 public:
  
  FunctionNoopReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class FunctionNoopRecorder: public GenFunctionRecorder {

 public:
  
  FunctionNoopRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};

class FunctionProductReader: public GenFunctionReader {

 public:
  
  FunctionProductReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class FunctionProductRecorder: public GenFunctionRecorder {

 public:
  
  FunctionProductRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};

class FunctionQuotientReader: public GenFunctionReader {

 public:
  
  FunctionQuotientReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class FunctionQuotientRecorder: public GenFunctionRecorder {

 public:
  
  FunctionQuotientRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};


class FunctionSumReader: public GenFunctionReader {

 public:
  
  FunctionSumReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class FunctionSumRecorder: public GenFunctionRecorder {

 public:
  
  FunctionSumRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};

class FunctionDifferenceReader: public GenFunctionReader {

 public:
  
  FunctionDifferenceReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end ) const;
  
};


class FunctionDifferenceRecorder: public GenFunctionRecorder {

 public:
  
  FunctionDifferenceRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};


class VariableReader: public GenFunctionReader {

 public:
  
  VariableReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class VariableRecorder: public GenFunctionRecorder {

 public:
  
  VariableRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

