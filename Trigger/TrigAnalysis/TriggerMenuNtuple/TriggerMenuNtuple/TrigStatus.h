/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigStatus_h__
#define __TrigStatus_h__
/*
  TrigStatus.h
*/
#include <string>

class TrigStatus {
public:
  TrigStatus(const std::string& name="", int status=0);
  ~TrigStatus();

  std::string name() const { return mName; }
  int status() const { return mStatus; }
  bool isPassed() const { return (mStatus != 0); }

  void setName(const std::string& name) { mName = name; }
  void setStatus(int n) { mStatus = n; }

protected:
  std::string mName;
  int mStatus;
  
};

#endif // __TrigStatus_h__
