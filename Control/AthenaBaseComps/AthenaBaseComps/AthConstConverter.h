// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaBaseComps/AthConstConverter.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2020
 * @brief Gaudi converter base class with const interfaces.
 */


#ifndef ATHENABASECOMPS_ATHCONSTCONVERTER_H
#define ATHENABASECOMPS_ATHCONSTCONVERTER_H


#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GaudiKernel/Converter.h"


/**
 * @brief Gaudi converter base class with const interfaces.
 *
 * This is a version of Gaudi Converter which uses const
 * @c createRepConst and @c createObjConst methods instead
 * of @c createObj and @c createRep.  This can be used for converters
 * that one wants to execute concurrently in MT jobs.
 *
 * As a bonus, we also include @c AthMessaging functionality.
 */
class AthConstConverter : public Converter, public AthMessaging
{
public:
  AthConstConverter (long storage_type,
                     const CLID& class_type,
                     ISvcLocator* svc,
                     const std::string& name)
    : Converter (storage_type, class_type, svc),
      AthMessaging (msgSvc().get(), name),
      m_name (name)
  {
  }


  /// Create the transient representation of an object.
  virtual StatusCode createObjConst( IOpaqueAddress* pAddress,
                                     DataObject*& refpObject ) const;

  /// Convert the transient object to the requested representation.
  virtual StatusCode createRepConst( DataObject* pObject,
                                     IOpaqueAddress*& refpAddress ) const;


  /// Create the transient representation of an object.
  // Non-const version; just calls the const version.
  virtual StatusCode createObj( IOpaqueAddress* pAddress,
                                DataObject*& refpObject ) override final;


  /// Convert the transient object to the requested representation.
  // Non-const version; just calls the const version.
  virtual StatusCode createRep( DataObject* pObject,
                                IOpaqueAddress*& refpAddress ) override final;


  std::string name() const { return m_name; }


private:
  std::string m_name;
};


#endif // not ATHENABASECOMPS_ATHCONSTCONVERTER_H
