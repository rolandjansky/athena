/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHLINKS_TOOLS_SGELVREF_H
#define ATHLINKS_TOOLS_SGELVREF_H

#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include <algorithm>
#include <exception>
#include <functional>
#include <vector>
#include <boost/iterator_adaptors.hpp>



template <typename DOBJ>
class ElementLinkVector;

  /** @class SG::ELVRef
   *  @brief a short ref to an ElementLink into an ElementLinkVector.
   *  Used to be an internal class of ElementLinkVector.
   *  Had to pull out because of brain-damaged pool converters
   */

namespace SG {
  template <typename DOBJ>
  class ELVRef
  {
  public:
    typedef typename std::vector< DataLink<DOBJ> > DataLinkVector;
    typedef ElementLinkVector<DOBJ> ElemLinkVec;
    typedef ElementLink<DOBJ> ElemLink;
    typedef typename ElemLink::IndexingPolicy IndexingPolicy;
    typedef typename ElemLink::ElementConstPointer ElementConstPointer;
    typedef typename ElemLink::index_type index_type;
    typedef typename ElemLink::stored_index_type stored_index_type;
    typedef typename ElemLink::ID_type ID_type;

    ///STL required
    ELVRef(const ELVRef& rhs);
    ///STL required
    ELVRef& operator=(const ELVRef& rhs);
    ///standard constructor
    ELVRef(const ElemLink& link=ElemLink());
    /// Constructor from indices.
    ELVRef(typename DataLinkVector::size_type hostIndex,
           index_type elementIndex,
           const ElemLinkVec& owner);
    ///dataID of the host
    ID_type dataID() const;
    /// index into container (non-const, calculates and sets on demand):
    index_type elementIndex() const;
    ///element pointer in ElementLink
    ElementConstPointer cptr() const { return m_link.cptr(); }
    
    /// get the corresponding ElementLink. O(1)
    const ElemLink& elementLink() const { return m_link; }
    
    /// \name comparison ops (STL required)
    //@{	
    bool operator == (const ELVRef& rhs) const {
      return ( (index() == rhs.index()) && (dataID() == rhs.dataID()) );
    }
    bool operator < (const ELVRef& rhs) const {
      return ( (index() < rhs.index()) ||
	       ( (index() == rhs.index()) && (dataID() < rhs.dataID()) ) );
    }
    //@}
    
   /// called by handlers to change state:
    bool toPersistent(const ElemLinkVec& owner);
    bool toPersistent(const ElemLinkVec& owner,
                      typename DataLinkVector::size_type& shortref);
    bool toTransient(const ElemLinkVec& owner);
    bool doRemap();

    const stored_index_type& index() const
    {
      return m_index;
    }

  private:
    
    //Transient members only.
    ElemLink m_link; ///! the element pointer

  // Persistent members:
    typename DataLinkVector::size_type m_shortRef; ///! index of the host dobj

    stored_index_type m_index;
  };
}
#endif // not ATHLINKS_TOOLS_SGELVREF_H
