// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetTagFinder.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2012
 * @brief Common code for retrieving jet tag information from a jet.
 */


#ifndef JETTAGD3PDMAKER_JETTAGFINDER_H
#define JETTAGD3PDMAKER_JETTAGFINDER_H


#include "SGTools/ClassName.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/AlgTool.h"
#include <string>
#include <vector>
class Jet;
class JetTagInfoBase;


namespace D3PD {


/**
 * @brief Common code for retrieving jet tag information from a jet.
 *
 * Include this as a member of a filler/association class.
 * Pass to the constructor a pointer to the tool and the default tag info key.
 * It will define the properties InfoType and AllowMissing.
 * Call getTagInfoBase or getTagInfo<T> to get a taginfo object.
 */
class JetTagFinder
{
public:
  /**
   * @brief Constructor.
   * @param filler The tool of which we are a part.
   * @param infoTypeDefault Default tag info key string.
   * @param allowMissingDefault Default value for the AllowMissing property.
   */
  JetTagFinder (AlgTool& filler,
                const std::string& infoTypeDefault,
                bool allowMissingDefault = false);


  /**
   * @brief Retrieve a taginfo object.
   * @param j Jet from which the object is to be retrieved.
   * @return The taginfo object or 0.
   *
   * If the object can't be found, we return 0; we also issue
   * a warning unless AllowMissing is set.
   */
  const JetTagInfoBase* getTagInfoBase (const Jet& j) const;


  /**
   * @brief Retrieve a taginfo object of type T.
   * @param j Jet from which the object is to be retrieved.
   * @return The taginfo object or 0.
   *
   * If the object can't be found, we return 0; we also issue
   * a warning unless AllowMissing is set.
   */
  template <class T>
  const T* getTagInfo (const Jet& j) const;


private:
  /// Reference to the tool of which we are a part.
  AlgTool& m_filler;

  /// Property: Requested taginfo type key.  May contain multiple keys
  /// separated by commas to search for multiple type.
  std::string m_infoType;

  /// Property: If true, don't warn about missing taginfo objects.
  bool m_allowMissing;

  /// List of taginfo type keys for which we'll search.
  mutable std::vector<std::string> m_keys;
};


/**
 * @brief Retrieve a taginfo object of type T.
 * @param j Jet from which the object is to be retrieved.
 * @return The taginfo object or 0.
 *
 * If the object can't be found, we return 0; we also issue
 * a warning unless AllowMissing is set.
 */
template <class T>
const T* JetTagFinder::getTagInfo (const Jet& j) const
{
  const JetTagInfoBase* ti = getTagInfoBase (j);
  if (!ti) return 0;
  const T* tt = dynamic_cast<const T*> (ti);
  if (!tt) {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, m_filler.name())
      << "Can't cast tag info " << m_infoType
      << " to " << ClassName<T>::name();
  }
  return tt;
}


} // namespace D3PD



#endif // not JETTAGD3PDMAKER_JETTAGFINDER_H
