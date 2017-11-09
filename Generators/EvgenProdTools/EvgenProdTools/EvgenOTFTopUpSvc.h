/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef EVGENPRODTOOLS_EVGENOTFTOPUPSVC_H
#define EVGENPRODTOOLS_EVGENOTFTOPUPSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GenInterfaces/IEvgenOTFTopUpSvc.h"
#include <string>

/// @brief Service to hold some information for topping up
///  algorithms that are running on the fly.  It's a bit tricky,
///  but this is mostly just a service for holding some numbers...
///
class EvgenOTFTopUpSvc : public AthService , public IEvgenOTFTopUpSvc {
public:

  // Constructor and destructor
  EvgenOTFTopUpSvc(const std::string& name, ISvcLocator* pSvcLocator);

  ~EvgenOTFTopUpSvc() {;} //!< Nothing to delete...

  // Standard sequence functions - don't actually need an excecute
  //  Could use init() as a chance to run OTF for the first time,
  //  but that applied to the OTF algorithm, not this service
  StatusCode finalize();

  // Getters
  virtual long   getNPerFile()    const { return m_nPerFile; }
  virtual long   getNUsedSoFar()  const { return m_nUsedSoFar; }
  virtual double getEfficiency()  const { return m_efficiency; }
  virtual bool   getNewFileFlag() const { return m_newFileFlag; }

  // Setters - first for Pythia/Herwig to call when they use up events
  virtual void   usedEvents() { ++m_nUsedSoFar; }
  virtual void   usedEvents( const int n ) { m_nUsedSoFar+=n; }

  // New file created by the OTF generator algorithm
  virtual void   newFile( std::string& s ) { ++m_nIterations; m_newFileFlag=true; m_newFileName=s; m_nUsedSoFar=0; }

  // New file grabbed by the showering generator
  virtual const std::string& getFile() { m_newFileFlag=false; return m_newFileName; }

  static const InterfaceID& interfaceID() { return IID_IEvgenOTFTopUpSvc; }
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

private:

  bool        m_execDuringInit; //!< Execute OTF during initialization?  (Set in JO)
  long        m_nPerFile;       //!< Number of events per LHE file  (Set in JO)
  long        m_nUsedSoFar;     //!< Number of events used so far by Pythia/Herwig  (counter)
  double      m_efficiency;     //!< Rough efficiency in Pythia/Herwig  (Set in JO)
  bool        m_newFileFlag;    //!< Just produced a new file with the OTF generator  
  std::string m_newFileName;    //!< Name of the new OTF generated file

  long        m_nTotal;         //!< Book-keeping: total number of generated events 
  long        m_nIterations;    //!< Book-keeping: total number of OTF iterations
};

#endif

#endif
