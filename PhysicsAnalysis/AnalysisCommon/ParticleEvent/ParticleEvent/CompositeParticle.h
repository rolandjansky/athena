///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticle.h 
// Header file for class CompositeParticle
// Author: Frank Paige            - Jan 2004
// Author: Hong Ma                - May 2004
// Author: Ketevi A. Assamagan    - May 2004
// Author: Kyle Cranmer           - May 2004
// Author: Adam Cunha             - Apr 2007
// Author: S.Binet<binet@cern.ch> - Apr 2008
// Author: Karsten Koeneke        - Jan 2011
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENT_COMPOSITEPARTICLE_H 
#define PARTICLEEVENT_COMPOSITEPARTICLE_H 

// STL includes
//#include <iosfwd>
#include <vector>
#include <algorithm>

#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "SGTools/CLASS_DEF.h"

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "FourMom/Lib/P4ImplPxPyPzE.h"
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"

// ParticleEvent includes
#include "ParticleEvent/ParticleImpl.h"
#include "ParticleEvent/Lib/Base.h"
#include "ParticleEvent/CompositeParticleNavigation.h"

// Forward declaration

/** @class CompositeParticle is a class which is made from the composition
 *         of other particles. Its 4-momentum is the sum of all its
 *         constituents.
 */
class CompositeParticle : public ParticleImpl<
                                    CompositeParticleNavigation,
                                    P4ImplPxPyPzE,
                                    ParticleEvent::Base
                                 >
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  typedef ParticleImpl<
             CompositeParticleNavigation,
             P4ImplPxPyPzE,
             ParticleEvent::Base
          >
          Base_t; // easy on the handwriting..
  typedef Base_t::navigable_type   Navigable_t;
  typedef Navigable_t::object_iter ConstituentsIter_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /* --- compiler generated constructors should be good enough ---
  /// Default constructor: 
  CompositeParticle();

  /// Copy constructor: 
  CompositeParticle( const CompositeParticle& rhs );

  /// Assignment operator: 
  CompositeParticle& operator=( const CompositeParticle& rhs ); 
  */

  /// Constructor with parameters: 

  /// Destructor: 
  virtual ~CompositeParticle() = default;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// return the vector of constituents *by value*. FIXME: inefficient
  std::vector<const INavigable4Momentum*> get_constituents() const 
  { 
    std::vector<const INavigable4Momentum*> constituents;
    this->get_constituents<INavigable4Momentum>(constituents);
    return constituents;
  }

  /** Fill a vector of elements with the constituents which compose that
   *  @c CompositeParticle. Try to use this method rather than the one
   *  above, as it is more efficient.
   */
  template <class Element>
  void get_constituents (std::vector<const Element*>& cont) const 
  {
    const std::size_t iMax = this->navigableBase().size();
    cont.clear();
    cont.reserve( iMax );
    for ( std::size_t i = 0; i != iMax; ++i ) {
      const Element* constituent = this->get_constituent<Element>(i);
      if ( constituent ) {
	cont.push_back( constituent );
      }
    }
  }

  /** @brief Return a constituent given its index
   *  @param idx The index of the constituent
   *  @return The constituent with the asked for type
   *          return NULL if index was invalid or dyn-cast failed.
   */
  template <class Element>
  const Element* get_constituent (std::size_t idx) const 
  {
    typedef typename Base_t::navigable_type navigable_type;
    typedef typename navigable_type::object_iter object_iter;

    const Element * element = 0;
    if (idx < this->navigableBase().size()) {
      object_iter o = this->navigableBase().begin();
      //can't use it as NavigatorIterator does not implement the
      // RandomIterator concept !!
      //std::advance(o, idx);
      // so we use this O(n) approach:
      while ( idx-- ) { ++o; }
      element = dynamic_cast<const Element*>(*o);
    }
    return element;
  }

  /** @brief check if a given particle is contained by this composite
   */
  bool contains (const INavigable4Momentum* particle) const;

  /** @brief check if a given particle is contained by this composite
   */
  bool contains (const INav4MomLink& ipart) const
  { return contains(*ipart); }

  /** @brief iterator over constituents of this composite
   */
  ConstituentsIter_t constituents_begin() const
  { return this->navigableBase().begin(); }

  /** @brief iterator over constituents of this composite
   */
  ConstituentsIter_t constituents_end() const
  { return this->navigableBase().end(); }

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  //========================================================================
  /** @name Filling the @c CompositeParticle */
  //@{
  void add( const INav4MomLink& ipart );
  void add( const INav4MomLinkContainer* iparts );
  void add( const INav4MomLinkContainer& iparts );
  void add( const INav4MomLink& p1, const INav4MomLink& p2 );
  void add( const INav4MomLink& p1, const INav4MomLink& p2, 
            const INav4MomLink& p3 );
  void add( const INav4MomLink& p1, const INav4MomLink& p2,
            const INav4MomLink& p3, const INav4MomLink& p4 );
  //@}
  //========================================================================
  
  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// helper method to put elements into the underlying Navigable container
  void putElement( const INav4MomLink& ipart );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const CompositeParticle& o );

/// define a CLID for the @c CompositeParticle
CLASS_DEF( CompositeParticle , 56025822 , 1 )

#endif //> PARTICLEEVENT_COMPOSITEPARTICLE_H
