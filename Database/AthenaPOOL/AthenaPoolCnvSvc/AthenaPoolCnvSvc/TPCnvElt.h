// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvc/TPCnvElt.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Helper for calling a TP converter from an Athena converter.
 */


#ifndef ATHENAPOOLCNVSVC_TPCNVELT_H
#define ATHENAPOOLCNVSVC_TPCNVELT_H


#include "PersistentDataModel/Guid.h"
#include <typeinfo>
#include <memory>
class MsgStream;


/**
 * @brief Placeholder for the case where no conversion is to be done.
 */
template <class TRANS> class T_TPCnvNull
{
};


namespace AthenaPoolCnvSvc {


/**
 * @brief Given a @c type_info, get the corresponding pool guid.
 * @param ti @c type_info to look for.
 *
 * Throws an exception on errors.
 */
Guid guidFromTypeinfo (const std::type_info& ti);


/**
 * @brief Helper for calling a TP converter from an Athena converter.
 *
 * This is used to allow calling one out of a templated list of TP converters.
 * The methods here test if the current guid matches the guid for this
 * converter, and calls it if so.
 *
 * CNV is the top-level pool converter (that reads the persistent object).
 * TPCNV is the TP converter class to be called.  For the case where no
 * conversion is to be done, use T_TPCnvNull<TRANS> for the TP converter
 * class.
 */
template <class CNV, class TPCNV>
class TPCnvElt
{
public:
  /// Make available the persistent and transient types.
  typedef typename TPCNV::Trans_t Trans_t;
  typedef typename TPCNV::Pers_t  Pers_t;

  
  /**
   * @brief Constructor.
   */
  TPCnvElt();


  /**
   * @brief Read the persistent object and convert it to transient.
   * @param parent The top-level pool converter object.
   * @param msg MsgStream for error reporting.
   *
   * Returns a newly-allocated object.
   * If the type of the persistent object on the file does not match the
   * type that this converter handles, return nullptr.
   * Other errors are reported by raising exceptions.
   */
  Trans_t* createTransient (CNV& parent, MsgStream& msg);


  /**
   * @brief Read the persistent object and convert it to transient.
   * @param parent The top-level pool converter object.
   * @param trans The transient object to modify.
   * @param msg MsgStream for error reporting.
   *
   * Overwrites the provided transient object.
   * If the type of the persistent object on the file does not match the
   * type that this converter handles, returns false.
   * Other errors are reported by raising exceptions.
   */
  bool persToTrans (CNV& parent, Trans_t* trans, MsgStream& msg);

  
private:
  /// GUID for the persistent class we read.
  Guid m_guid;

  /// The underlying TP converter.
  TPCNV m_cnv;
};


/**
 * @brief Helper for calling a TP converter from an Athena converter.
 *        Specialization for the case of no conversion.
 */
template <class CNV, class TRANS>
class TPCnvElt<CNV, T_TPCnvNull<TRANS> >
{
public:
  /// Make available the persistent and transient types.
  typedef TRANS Trans_t;
  typedef TRANS  Pers_t;

  
  /**
   * @brief Constructor.
   */
  TPCnvElt();


  /**
   * @brief Read the persistent object and convert it to transient.
   * @param parent The top-level pool converter object.
   * @param msg MsgStream for error reporting.
   *
   * Returns a newly-allocated object.
   * If the type of the persistent object on the file does not match the
   * type that this converter handles, return nullptr.
   * Other errors are reported by raising exceptions.
   */
  Trans_t* createTransient (CNV& parent, MsgStream& msg);


  /**
   * @brief Read the persistent object and convert it to transient.
   * @param parent The top-level pool converter object.
   * @param trans The transient object to modify.
   * @param msg MsgStream for error reporting.
   *
   * Overwrites the provided transient object.
   * If the type of the persistent object on the file does not match the
   * type that this converter handles, returns false.
   * Other errors are reported by raising exceptions.
   */
  bool persToTrans (CNV& parent, Trans_t* trans, MsgStream& msg);

  
private:
  /// GUID for the persistent class we read.
  Guid m_guid;
};


} // namespace AthenaPoolCnvSvc


#include "AthenaPoolCnvSvc/TPCnvElt.icc"


#endif // not ATHENAPOOLCNVSVC_TPCNVELT_H
