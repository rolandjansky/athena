/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//	============================================================
//	McCnvSvc.h
//	------------------------------------------------------------
//
//	Package   : McEventSelector 
//
//	Author    : Marjorie Shapiro
//      Modified Nov 10 I.H.
//	============================================================
#ifndef MCEVENTSELECTOR_MCCNVSVC_H
#define MCEVENTSELECTOR_MCCNVSVC_H 1

#include "GaudiKernel/ConversionSvc.h"  

// Forward declarations
template <class TYPE> class SvcFactory;

class McCnvSvc : public ConversionSvc	{
  /// Allow the factory class access to the constructor
  friend class SvcFactory<McCnvSvc>;
public:
  /**@name: Object implementation     */
  //@{
  /// Standard Constructor
  McCnvSvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~McCnvSvc();
  
  /**@name McCnvSvc overrides     */
  //@{
  /// Initialize the service.
  virtual StatusCode initialize();

  /// Update state of the service
  virtual StatusCode updateServiceState(IOpaqueAddress* pAddress);
  //@}

  /// storage type
  static long storageType() ;
  virtual long repSvcType() const;
};

#endif // MCEVENTSELECTOR_MCCNVSVC_H

