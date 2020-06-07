// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaPoolCnvSvc/T_AthenaPoolCreateFuncs.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Helpers for calling TP converters.
 *
 * This file providers two helpers for calling TP converters from the
 * templated pool converters.
 *
 * Some TP converters derive from ITPConverter.  In that case, we want to
 * call createPersistent / createTransient().  But some TP converters do not;
 * in that case, we should call transToPers() / persToTrans() directly.
 *
 * In the first case, we actually call the *WithKey versions.
 * We don't support passing the key to converters that don't derive
 * from ITPConverter.
 */


#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLCREATEFUNCS_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLCREATEFUNCS_H


#include "TPTools/ITPConverter.h"
#include "GaudiKernel/MsgStream.h"
#include <memory>
#include <type_traits>

namespace EventContainers{
  class IdentifiableContainerBase;
}

namespace AthenaPoolCnvSvc {


/**
 * @brief Create a persistent object from a transient object.
 * @param cnv The TP converter to call.
 * @param transObj The transient object to convert.
 * @param The SG key of the object.
 * @param log For logging messages.
 *
 * This version is used when CNV derives from ITPConverter.
 * We call TPCNV::createPersistentWithKey().
 */
template <class TPCNV,
          typename std::enable_if_t<std::is_base_of_v<ITPConverter, TPCNV> >* = nullptr>
std::unique_ptr<typename TPCNV::Pers_t>
createPersistent (TPCNV& cnv,
                  const typename TPCNV::Trans_t* transObj,
                  const std::string& key,
                  MsgStream& log)
{
  return std::unique_ptr<typename TPCNV::Pers_t>
    (cnv.createPersistentWithKey (transObj, key, log));
}


/**
 * @brief Create a persistent object from a transient object.
 * @param cnv The TP converter to call.
 * @param transObj The transient object to convert.
 * @param The SG key of the object.
 * @param log For logging messages.
 *
 * This version is used when CNV does not derivce from ITPConverter.
 * We create the persistent object directly and call TPCnv::transToPers().
 */
template <class TPCNV,
          typename std::enable_if_t<!std::is_base_of_v<ITPConverter, TPCNV> >* = nullptr>
std::unique_ptr<typename TPCNV::Pers_t>
createPersistent (TPCNV& cnv,
                  const typename TPCNV::Trans_t* transObj,
                  const std::string& /*key*/,
                  MsgStream& log)
{
  auto pers = std::make_unique<typename TPCNV::Pers_t>();
  cnv.transToPers(transObj, pers.get(), log);
  return pers;
}


/**
 * @brief Create a transient object from a persistent object.
 * @param cnv The TP converter to call.
 * @param persObj The persistent object to convert.
 * @param The SG key of the object.
 * @param log For logging messages.
 *
 * This version is used when CNV derives from ITPConverter.
 * We call TPCNV::createTransientWithKey().
 */
template <class TPCNV,
          typename std::enable_if_t<std::is_base_of_v<ITPConverter, TPCNV> >* = nullptr>
std::unique_ptr<typename TPCNV::Trans_t>
createTransient (TPCNV& cnv,
                 const typename TPCNV::Pers_t* persObj,
                 const std::string& key,
                 MsgStream& log)
{
  return std::unique_ptr<typename TPCNV::Trans_t>
    (cnv.createTransientWithKey (persObj, key, log));
}


/**
 * @brief Create a transient object from a persistent object.
 * @param cnv The TP converter to call.
 * @param persObj The persistent object to convert.
 * @param The SG key of the object.
 * @param log For logging messages.
 *
 * This version is used when CNV does not derivce from ITPConverter.
 * We create the transient object directly and call TPCnv::persToTrans().
 */
template <class TPCNV,
          typename std::enable_if_t<!std::is_base_of_v<ITPConverter, TPCNV> >* = nullptr>
std::unique_ptr<typename TPCNV::Trans_t>
createTransient (TPCNV& cnv,
                 const typename TPCNV::Pers_t* persObj,
                 const std::string& /*key*/,
                 MsgStream& log)
{
  typedef typename TPCNV::Trans_t Trans_t;
  if constexpr(std::is_base_of< EventContainers::IdentifiableContainerBase, Trans_t>::value &&
               !std::is_default_constructible<Trans_t>::value)
  {
    log << "IdentifiableContainerBase is not compatible with createTransient" << endmsg;
    return std::unique_ptr<Trans_t>();
  }

  auto trans = std::make_unique<Trans_t>();
  cnv.persToTrans(persObj, trans.get(), log);
  return trans;
}


} // namespace AthenaPoolCnvSvc


#endif // not ATHENAPOOLCNVSVC_T_ATHENAPOOLCREATEFUNCS_H
