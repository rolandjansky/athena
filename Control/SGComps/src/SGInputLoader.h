///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SGInputLoader.h 
// Header file for class SGInputLoader
/////////////////////////////////////////////////////////////////// 
#ifndef SGCOMPS_SGINPUTLOADER_H
#define SGCOMPS_SGINPUTLOADER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/DataObjID.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class SGInputLoader
  : public ::AthAlgorithm
{ 

 public: 

  SGInputLoader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SGInputLoader(); 

  /// this Alg is Clonable (for AthenaMT)
  bool isClonable() const override { return true; }

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;

 private: 

  /// Default constructor: 
  SGInputLoader();

  void loader(Gaudi::Details::PropertyBase&);
  bool loadObjs(const DataObjIDColl& objs) const;

  /// Properties
  DataObjIDColl m_load;    // create Output data dependencies for these objects

  Gaudi::Property< bool > m_dump {this, "ShowEventDump", false, 
      "printout contents of EventStore at end of execute()"};
  
  Gaudi::Property<bool> m_failEvt{this, "FailIfNoProxy", true, 
      "execute returns FAILURE if a requested proxy is not found in store"};
      
  Gaudi::Property<bool> m_loadProxies{this, "LoadExtraProxies", true,
      "load the Proxies attributed to the SGInputLoader by the Scheduler"};

  // other
  bool m_first{true};

}; 


#endif //> !SGCOMPS_SGINPUTLOADER_H
