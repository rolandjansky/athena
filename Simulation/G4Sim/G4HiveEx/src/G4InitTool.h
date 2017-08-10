/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4HIVEEX_G4INITTOOL_H
#define G4HIVEEX_G4INITTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IThreadInitTool.h"

#include <string>
#include <atomic>

class G4InitTool: virtual public extends1<AthAlgTool, IThreadInitTool> {

public:
  G4InitTool( const std::string&, const std::string&, const IInterface* );

  // the magic method this tool provides
  virtual void initThread();
  virtual void terminateThread();
  virtual unsigned int nInit() const { return m_nInitThreads; }

private:
  // Number of threads that have been initialized
  std::atomic_uint m_nInitThreads;

};

#endif
