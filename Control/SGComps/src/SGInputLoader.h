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
#include "GaudiKernel/DataObjID.h"


class SGInputLoader
  : public ::AthAlgorithm
{ 

 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  SGInputLoader( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~SGInputLoader(); 

  /// this Alg is Clonable (for AthenaMT)
  bool isClonable() const override { return true; }

  // Assignment operator: 
  //SGInputLoader &operator=(const SGInputLoader &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;

 private: 

  /// Default constructor: 
  SGInputLoader();

  void loader(Property&);
  bool loadObjs(const DataObjIDColl& objs) const;

  /// Containers
  DataObjIDColl m_load;

  bool m_dump{false}, m_first{true};

  Gaudi::Property<bool> m_failEvt{true};
  Gaudi::Property<bool> m_loadProxies{true};

}; 


#endif //> !SGCOMPS_SGINPUTLOADER_H
