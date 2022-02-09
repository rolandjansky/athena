/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParameters_hh
#define TRTParameters_hh

#include "globals.hh"
#include <map>
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTParameters
{
public:
  static const TRTParameters* GetPointer();
  
  int GetInteger(const std::string&) const;
  double GetDouble(const std::string&) const;
  void GetIntegerArray(const std::string&, int, int*) const;
  void GetDoubleArray(const std::string&, int, double*) const;
  void GetPartOfIntegerArray(const std::string&, int, int*) const;
  void GetPartOfDoubleArray(const std::string&, int, double*) const;
  int GetElementOfIntegerArray(const std::string&, int) const;

  MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl) { return m_msg.get().level() <= lvl; }

private:
  TRTParameters();
  ~TRTParameters();
  
  void ReadInputFile(const std::string&);
  void PrintListOfParameters() const;
  
  std::multimap<std::string, double, std::less<std::string> >
  m_multimapOfParameters;
  
  typedef
  std::multimap<std::string, double, std::less<std::string> >::const_iterator
  multimapIterator;
  
  Athena::MsgStreamMember m_msg;
};

#endif
