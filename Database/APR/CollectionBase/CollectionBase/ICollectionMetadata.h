/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICollectionMetadata_h
#define ICollectionMetadata_h

#include <string>

namespace pool {

    class ICollectionMetadata {    
     public:

      class ICollectionMetadataIterator {
     public:
	virtual ~ICollectionMetadataIterator() {}
	virtual bool operator==( const ICollectionMetadataIterator& rhs ) const = 0;
	virtual bool operator!=( const ICollectionMetadataIterator& rhs ) const = 0;
	virtual void operator++() = 0;

	virtual const std::string&	key() const = 0;
	virtual const char *		value() const = 0;
      };


    public:     
     class const_iterator  {
       
     public:
       bool operator==( const const_iterator& rhs ) const { return *m_iterator == *rhs.m_iterator; }
       bool operator!=( const const_iterator& rhs ) const { return *m_iterator != *rhs.m_iterator; }
       const_iterator&	operator++() { m_iterator->operator++(); return *this; }

       const std::string&	key() const { return m_iterator->key(); }
       const char *		value() const { return m_iterator->value(); }
       
       ~const_iterator() { delete m_iterator; }

       const_iterator( ICollectionMetadataIterator* iter )
	     : m_iterator( iter ) {}
       
       const_iterator& operator=( const const_iterator& rhs ) { 
	  m_iterator = rhs.releaseIter();
	  return *this; 
       }

    protected:
       ICollectionMetadataIterator* releaseIter() const {
	  ICollectionMetadataIterator *iter = m_iterator;
	  m_iterator = 0;
	  return iter;
       }
       
       
       mutable ICollectionMetadataIterator*	m_iterator;      
     };


   public:
      /// return number of metadata entries for this collection
      virtual unsigned long long	entries() const = 0;
	        
      virtual bool 		existsKey( const std::string& key ) const = 0;

      virtual const char*	getValueForKey( const std::string& key ) const = 0;

      virtual void 		setValueForKey( const std::string& key,
						const std::string& val ) = 0;

      virtual const_iterator	begin() const = 0;

      virtual const_iterator	end() const = 0;

      
      virtual	~ICollectionMetadata() {} 

    };
}	

#endif
