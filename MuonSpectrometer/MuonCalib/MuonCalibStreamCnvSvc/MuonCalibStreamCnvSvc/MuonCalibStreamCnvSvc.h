/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAMCNVSVC_MUONCALIBSTREAMCNVSVC_H
#define MUONCALIBSTREAMCNVSVC_MUONCALIBSTREAMCNVSVC_H

#include "GaudiKernel/ConversionSvc.h"

// Forward declarations
template <class TYPE> class SvcFactory;

class MuonCalibStreamCnvSvc : public ConversionSvc
{
  /// Allow the factory class access to the constructor
  friend class SvcFactory<MuonCalibStreamCnvSvc>;
public:
  /**@name MuonCalibStreamCnvSvc overrides     */
  //@{
  /// Initialize the service.
  virtual StatusCode initialize();

  // query interface method
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  /// Update state of the service
  virtual StatusCode updateServiceState(IOpaqueAddress* pAddress);
  //@}


protected:
  /**@name: Object implementation     */
  //@{
  /// Standard Constructor
  MuonCalibStreamCnvSvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~MuonCalibStreamCnvSvc();

  // initialize the converters for these data class.
  std::vector<std::string> m_initCnvs ; 

private:

};
#endif
