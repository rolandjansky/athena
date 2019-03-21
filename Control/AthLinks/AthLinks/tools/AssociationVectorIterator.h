/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ASSOCIATION_CONTEXT
/*! \brief iterator for association vectors (internal use only)
 *
 * \c AssociationVectorIterator behaves with many respects like a
 * STL iterator, but is also incomplete with respect to the STL implementation.
 * It can only be used in the context of the association objects, and should 
 * not be included directly into client code.
 */

 class AssociationVectorIterator
   : public std::iterator<std::bidirectional_iterator_tag, asso_type*>
 {
 public:
   /*! \brief alternate constructor
    *
    * The alternate constructor is the only meaningful constructor. It requires
    * an argument 
    * \param \c assoStore is a reference to the association store in the 
    * implementation class
    */
   AssociationVectorIterator(const asso_store& assoStore)
     : m_store(&assoStore)
     , m_actual(assoStore.begin())
   { }

   /*! \brief alternate constructor
    *
    * The alternate constructor is the only meaningful constructor. It requires
    * an argument 
    * \param \c assoStore is a reference to the association store in the 
    * implementation class
    * \param \c pos is a reference to a position in the association store
    */
   AssociationVectorIterator(const asso_store& assoStore,
			     const asso_store_iterator& pos)
     : m_store(&assoStore)
     , m_actual(pos)
   { }

   /*! \brief destructor */
   ~AssociationVectorIterator() { };
  
   /*! \name Iterator Methods
    *
    * The \c begin(), \c end() and \c find() methods are usually provided by 
    * the store, here the iterator itself provides it. The iterator forward 
    * (\c next()) and backward (\c prev()) advancement methods are provided to 
    * support the corresponding operators. 
    */
   /*@{*/
   AssociationVectorIterator begin() 
   { 
     if ( m_store) {
       return AssociationVectorIterator(*m_store); 
     } else {
       throw std::runtime_error( "Null pointer to store of associations" );
       // dead-code
       return AssociationVectorIterator( *m_store );
     }
   }

   AssociationVectorIterator end()   
   { 
     if ( m_store ) {
       return AssociationVectorIterator(*m_store); 
     } else {
       throw std::runtime_error( "Null pointer to store of associations" );
       // dead-code
       return AssociationVectorIterator( *m_store );
     }
   }

   AssociationVectorIterator begin() const
   { 
     if ( m_store) {
       return AssociationVectorIterator(*m_store, m_store->begin()); 
     } else {
       throw std::runtime_error( "Null pointer to store of associations" );
       // dead-code
       return AssociationVectorIterator( *m_store );
     }
   }

   AssociationVectorIterator end() const
   { 
     if ( m_store ) {
       return AssociationVectorIterator(*m_store, m_store->end()); 
     } else {
       throw std::runtime_error( "Null pointer to store of associations" );
       // dead-code
       return AssociationVectorIterator( *m_store );
     }
   }

   AssociationVectorIterator next()  { ++m_actual; return *this; }
   AssociationVectorIterator prev()  { --m_actual; return *this; }

   AssociationVectorIterator find(const asso_type* anAsso) const
   {
     typename asso_store::const_iterator assoEnd = m_store->end();
     //m_actual     = m_store->begin();
     //size_t iCtr  = 0;
     for ( typename asso_store::const_iterator itr = m_store->begin();
	   itr != assoEnd; ++itr ) {
       if ( **itr == anAsso ) {
	 return AssociationVectorIterator( *m_store, itr );
       }
     }
     return this->end();
   }
   /*@}*/
   
   /*! \name Iterator Operations */
   /*@{*/

   /*! \brief post-forward advancement operator */
   AssociationVectorIterator operator++()    
   { 
     AssociationVectorIterator tmp(*this);
     ++m_actual;
     return tmp;
   } 

   /*! \brief pre-forward advancement operator */
   AssociationVectorIterator operator++(int) { return this->next(); }

   /*! \brief post-forward advancement operator */
   AssociationVectorIterator operator--()    { return this->prev(); }

   /*! \brief post-forward advancement operator */
   AssociationVectorIterator operator--(int) 
   { 
     AssociationVectorIterator tmp(*this);
     --m_actual;
     return tmp;
   }
   
   //   bool operator==(AssociationVectorIterator& anOther)
   //   { return m_actual == anOther.m_actual; }
   bool operator==(const AssociationVectorIterator& anOther) const
   { return m_actual == anOther.m_actual; }  
   
   //   bool operator!=(AssociationVectorIterator& anOther)
   //   { return m_actual != anOther.m_actual; }
   bool operator!=(const AssociationVectorIterator& anOther) const
   { return m_actual != anOther.m_actual; }  
   
   /*! \brief object retrieval */
   const asso_type* operator*() const
   {
     return m_actual != m_store->end()
       ? *(*m_actual)
       : 0;
   }
   const asso_type* operator->() const
   {
     return m_actual != m_store->end()
       ? *(*m_actual)
       : 0;
   }

   asso_link getLink() const
   {
     return *m_actual;
   }

   bool isValid() const
   {
     return m_actual != m_store->end() && m_actual->isValid();
   }

   /*@}*/
   
   /*! \name store parameters */
   /*@{*/
   size_t size() const { return m_store->size(); }
   /*@}*/
   
 private:
  
   /*! \brief default constructor */
   AssociationVectorIterator() : 
     m_store (0),
     m_actual()
   { };

 private:
  
   /*! \brief pointer to store */
   const asso_store*   m_store;

   /*! \brief actual store iterator */
   //asso_store_iterator m_actual;
   typename asso_store::const_iterator m_actual;
 };

#endif // ASSOCIATION_CONTEXT
