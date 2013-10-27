/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParameters_hh
#define TRTParameters_hh

#include "globals.hh"
#include <map>
#include "AthenaKernel/MsgStreamMember.h"


class TRTParameters
{
public:
  ~TRTParameters();
  
  static TRTParameters* GetPointer()
  {
    if (!pParameters)
      pParameters = new TRTParameters;
    return pParameters;
  }
  
  int GetInteger(std::string) const;
  double GetDouble(std::string) const;
  void GetIntegerArray(std::string, int, int*) const;
  void GetDoubleArray(std::string, int, double*) const;
  void GetPartOfIntegerArray(std::string, int, int*) const;
  void GetPartOfDoubleArray(std::string, int, double*) const;
  int GetElementOfIntegerArray(std::string, int) const;

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)  const  { return m_msg.get().level() <= lvl; }

private:
  TRTParameters();
  
  void ReadInputFile(std::string);
  void PrintListOfParameters() const;
  
  int printMessages;
  
  std::multimap<std::string, double, std::less<std::string> >
  multimapOfParameters;
  
  typedef
  std::multimap<std::string, double, std::less<std::string> >::const_iterator
  multimapIterator;
  
  static TRTParameters* pParameters;

  mutable Athena::MsgStreamMember m_msg;

};

#endif
