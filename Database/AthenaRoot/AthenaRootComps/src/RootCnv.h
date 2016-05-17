///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootCnv.h
// Header file for class Athena::RootCnv
// Author Peter van Gemmeren <gemmeren@anl.gov>
///////////////////////////////////////////////////////////////////
#ifndef ATHENAROOTCOMPS_ROOTCNV_H
#define ATHENAROOTCOMPS_ROOTCNV_H 1

/** @file RootCnv.h
 *  @brief This file contains the class definition for the Athena::RootCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

// fwk includes
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "DataModelRoot/RootType.h"

// fwd declares
class IAthenaRootCnvSvc;
class ITPCnvBase;

/// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

namespace Athena {
/** @class Athena::RootCnv
 *  @brief This class provides the abstract converter to translate an object to/from its
 *  persistent ROOT representation.
 **/
class RootCnv : public ::Converter, public ::AthMessaging {
  friend class CnvFactory<Athena::RootCnv>;

public:
   /// Standard Constructor
  RootCnv(const CLID& id, ISvcLocator* pSvcLocator);
  RootCnv(ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~RootCnv();

  /// Gaudi Service Interface method implementations:
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /// Retrieve the class type of the data store the converter uses.
  long repSvcType() const;

  /// Create a transient object from a ROOT persistent representation.
  /// @param pAddr [IN] IOpaqueAddress of ROOT persistent representation.
  /// @param pObj [OUT] pointer to the transient object.
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

  /// Resolve the references of the created transient object.
  /// @param pAddr [IN] IOpaqueAddress of ROOT persistent representation.
  /// @param pObj [OUT] pointer to the transient object.
  virtual StatusCode fillObjRefs(IOpaqueAddress* pAddr, DataObject* pObj);

  /// Create a ROOT persistent representation for a transient object.
  /// @param pObj [IN] pointer to the transient object.
  /// @param pAddr [OUT] IOpaqueAddress of ROOT persistent representation.
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Resolve the references of the created transient object.
  /// @param pAddr [IN] IOpaqueAddress of ROOT persistent representation.
  /// @param pObj [OUT] pointer to the transient object.
  virtual StatusCode fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj);

  /// @return storage type.
  static long storageType();

  /// @return class ID.
  static const CLID& classID() { return CLID_NULL; }

private:
  /// Default constructor:
  RootCnv(); //< not implemented
  RootCnv(const ::Athena::RootCnv&); //< not implemented
  RootCnv& operator=(const ::Athena::RootCnv&); //< not implemented

protected:
  /// ServiceHandle to the conversion service
  ServiceHandle< ::IAthenaRootCnvSvc> m_cnvSvc;

private:
  /// transient type
  RootType m_ttype;

  /// persistent type
  RootType m_ptype;

  ITPCnvBase* m_tpcnv;
};

} //< end namespace Athena

#endif /* !ATHENAROOTCOMPS_ROOTCNV_H */
