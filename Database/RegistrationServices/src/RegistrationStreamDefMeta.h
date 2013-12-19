/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGISTRATIONSVC_REGSTREAMDEFMETA_H
# define REGISTRATIONSVC_REGSTREAMDEFMETA_H

/**
 * @file RegistrationStreamDefMeta.h
 *
 * @brief AlgTool which takes references provided by RegStream
 * finds the appropriate CORAL object in storegate and stores
 * them in a POOL collection.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Jack Cranshaw <Jack.Cranshaw@cern.ch>
 *
 * $Id: RegistrationStreamDefMeta.h,v 1.1 2009-04-28 19:04:06 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include <map>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IAddressCreator;
class StoreGateSvc;
class CollectionMetadata;

/**
 * @class RegistrationStreamDefMeta
 *
 * @brief AlgTool which takes references provided by RegStream
 * finds the appropriate CORAL object in storegate and stores
 * them in a POOL collection.
 *
 */

class RegistrationStreamDefMeta : public AthAlgorithm
{
public:

  /// Standard RegistrationStreamDefMeta Constructor
  RegistrationStreamDefMeta(const std::string& name,
                            ISvcLocator* pSvcLocator);

  /// Initialize RegistrationStreamDefMeta
  virtual StatusCode initialize();
  virtual StatusCode execute();

  /// Terminate RegistrationStreamDefMeta
  virtual StatusCode finalize();

protected:

  /// Standard Destructor
  virtual ~RegistrationStreamDefMeta();

  /// name of store:
  StringProperty           m_storeName;

  /// Metadata fields from properties
  unsigned int m_passnum;
  std::string m_passname, m_stream, m_config, m_project;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // REGISTRATIONSVC_REGSTREAMDEFMETA_H
