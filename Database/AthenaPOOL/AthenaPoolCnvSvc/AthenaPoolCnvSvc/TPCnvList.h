// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvc/TPCnvList.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Helper for calling TP converters from an Athena converter.
 */


#ifndef ATHENAPOOLCNVSVC_TPCNVLIST_H
#define ATHENAPOOLCNVSVC_TPCNVLIST_H


#include "AthenaPoolCnvSvc/TPCnvElt.h"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/transform.hpp"
#include <boost/fusion/mpl.hpp>
#include "boost/fusion/container/vector.hpp"
#include "boost/fusion/container/vector/convert.hpp"
#include "boost/fusion/algorithm/iteration/accumulate.hpp"


namespace AthenaPoolCnvSvc {


/**
 * @brief Helper for calling TP converters from an Athena converter.
 */
template <class CNV, class TRANS, class ... TPCNVS>
class TPCnvList
{
  /// Metafunction to wrap a TP converter in @c TPCnvElt.
  template <class TPCNV>
  struct wrap_tpcnv
  {
    typedef TPCnvElt<CNV, TPCNV> type;
  };


  /// Functional to iterate over TP converters and call @c createTransient
  /// on each.  Stop once one succeeds.
  struct do_create_transient
  {
    do_create_transient (CNV& parent, MsgStream& msg);
  
    template <class ELT>
    typename ELT::Trans_t* operator() (typename ELT::Trans_t* p, ELT& elt);

    CNV& m_parent;
    MsgStream& m_msg;
  };


  /// Functional to iterate over TP converters and call @c persToTrans
  /// on each.  Stop once one succeeds.
  struct do_pers_to_trans
  {
    do_pers_to_trans (CNV& parent, TRANS* trans, MsgStream& msg);
    
    template <class ELT>
    bool operator() (bool found, ELT& elt);

    CNV& m_parent;
    TRANS* m_trans;
    MsgStream& m_msg;
  };


public:
  // Convert the list of TP converters <T1, T2, ...> to a boost fusion vector
  // of converters wrapped by TPCnvElt:
  //   boost::fusion::vector<TPCnvElt<T1>, TPCnvElt<T2>, ...>
  // We can create in instance of this vector to get an object that holds
  // all the TP converter instances.
  typedef boost::mpl::vector<TPCNVS...> vec_t;
  typedef typename boost::mpl::transform<vec_t, wrap_tpcnv<boost::mpl::_1> >::type list_mpl_t;
  typedef typename boost::fusion::result_of::as_vector<list_mpl_t>::type list_t;


  /**
   * @brief Read the persistent object and convert it to transient.
   * @param parent The top-level pool converter object.
   * @param msg MsgStream for error reporting.
   *
   * Returns a newly-allocated object.
   * If the type of the persistent object on the file does not match the
   * the type of any of our TP converters, return nullptr.
   * Other errors are reported by raising exceptions.
   */
  TRANS* createTransient (CNV& parent, MsgStream& msg);

  
  /**
   * @brief Read the persistent object and convert it to transient.
   * @param parent The top-level pool converter object.
   * @param trans The transient object to modify.
   * @param msg MsgStream for error reporting.
   *
   * Overwrites the provided transient object.
   * If the type of the persistent object on the file does not match the
   * the type of any of our TP converters, return false.
   * Other errors are reported by raising exceptions.
   */
  bool persToTrans (CNV& parent, TRANS* trans, MsgStream& msg);
  

private:
  /// List of TP converter instances, wrapped by @c TPCnvElt.
  list_t m_list;
};


} // namespace AthenaPoolCnvSvc


#include "AthenaPoolCnvSvc/TPCnvList.icc"


#endif // not ATHENAPOOLCNVSVC_TPCNVLIST_H


