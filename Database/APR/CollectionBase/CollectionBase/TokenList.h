/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_TOKENLIST_H
#define COLLECTIONBASE_TOKENLIST_H

#include <string>
#include <vector>
#include <map>
#include <iosfwd>


class Token;

namespace pool {

   /**
    * @class TokenList TokenList.h CollectionBase/TokenList.h
    *
    * A container class to facilitate the use of Token objects.
    */
   class TokenList
   {
  public:
     /// Default constructor.
     TokenList();

     /**
      * Constructor that takes a list of Token names as input.
      *
      * @param specification List of Token names
      */
     TokenList( const std::vector<std::string>& specification );

     /** 
      * Copy constructor. Copies both the data and the Token list specification.
      *
      * @param rhs Token list to copy.
      */
     TokenList( const TokenList& rhs );

     /// Default destructor.
     ~TokenList();

     /**
      * Assignment operator. Copies both the data and the Token list specification.
      *
      * @param rhs Token list to copy.
      */
     TokenList& operator=( const TokenList& rhs );

     /**
      * Equality operator. Only compares the Token values, not their names.
      *
      * @param rhs The Token list to compare to.
      */
     bool operator==( const TokenList& rhs ) const;

     /**
      * Inequality operator. Only compares the Token values, not their names.
      *
      * @param rhs The Token list to compare to.
      */
     bool operator!=( const TokenList& rhs ) const;

     /**
      * Extends the Token list by one element.
      *
      * @param name Token name to be added to the Token list 
      */
     void extend( const std::string& name );


     /// Returns the size of the Token list.
     size_t size() const;

     /// delete the content of the list
     void	clear();

     /**
      * Returns a reference to a metadata Token given its name.
      *
      * @param name Name of Token.
      */
     Token& operator[]( const std::string& name );

     /**
      * Returns a constant reference to a metadata Token given its name.
      *
      * @param name Name of Token.
      */
     const Token& operator[]( const std::string& name ) const;

     /** 
      * Returns a reference to a metadata Token given its index.
      * 
      * @param name Index of Token.
      */
     Token& operator[]( unsigned int index );

     /** 
      * Returns a constant reference to a metadata Token given its index.
      * 
      * @param name Index of Token.
      */
     const Token& operator[]( unsigned int index ) const;

     /**
      * Outputs the Token names and associated values to a specified stream.
      *
      * @param os Stream to which Token information is to be output.
      */
     std::ostream& toOutputStream( std::ostream& os ) const;

     /// Base class for forward iterators used to navigate TokenList objects.
     class iterator_base 
     {
    public:
       bool operator==( const iterator_base& rhs ) const {
	  return m_list == rhs.m_list && m_index == rhs.m_index;
       }
       bool operator!=( const iterator_base& rhs ) const { return !(*this==rhs); }
       void operator++() { ++m_index; }
       bool next() { return ++m_index < m_list->size(); }
       const std::string& tokenName() const { return m_list->m_tokenNames[ m_index ]; }
    protected:
       iterator_base( const TokenList* list, size_t idx = 0 ) 
	     : m_list( list ), m_index( idx ) {}
       // default copy ctor and assignment.
       virtual ~iterator_base() {}

       const TokenList*	m_list;
       size_t		m_index;
     };

     /// Forward iterator class for navigation of TokenList objects.
     class iterator : public iterator_base
     {
      public:
        friend class TokenList;
         ~iterator() {};
         iterator( const iterator_base& rhs ) : iterator_base( rhs ) {}
         iterator& operator=( const iterator_base& rhs ) { 
	    iterator_base::operator=( rhs );
	    return *this; 
         }
      private:
         iterator( TokenList* list, size_t idx = 0 ) : iterator_base( list, idx ) {}
      public:
         Token* operator->() { return m_list->m_tokenVector[ m_index ]; }
         Token& operator*()  { return *operator->(); }
         iterator& operator++() { iterator_base::operator++(); return *this; }
     };

     /// Returns a forward iterator pointing to first element in Token list.
     iterator begin();

     /// Returns a forward iterator pointing to last element in Token list.
     iterator end();

     /// Constant forward iterator class for navigation of TokenList objects.
     class const_iterator : public iterator_base
     {
      public:
      friend class TokenList;
        ~const_iterator() {};
         const_iterator( const iterator_base& rhs ): iterator_base( rhs ) {}
         const_iterator& operator=( const iterator_base& rhs )       { 
	    iterator_base::operator=( rhs ); 
	    return *this; 
         }
      private:
         const_iterator( const TokenList* list, size_t idx = 0 ) : iterator_base( list, idx ) {}
      public:
         const Token* operator->() const { return m_list->m_tokenVector[ m_index ]; }
         const Token& operator*() const  { return *operator->(); }
         const_iterator& operator++() { iterator_base::operator++(); return *this; }
     };

     /// Returns a constant forward iterator pointing to first element in Token list.
     const_iterator begin() const;

     /// Returns a constant forward iterator pointing to last element in Token list.
     const_iterator end() const;

  protected:
     // implementation of the constructor
     void createFromSpec( const std::vector<std::string>& specification );
     
  private:
     /// Token names
     std::vector< std::string > m_tokenNames;

     /// Token vector (corresponding to m_tokenNames)
     std::vector< Token* > m_tokenVector;

     /// Map of Tokens with token name as key. Points to the same Token objects as m_tokenVector
     std::map< std::string, Token* > m_tokenMap;
   };
}


// Inline methods.
inline size_t
pool::TokenList::size() const
{
   return m_tokenVector.size();
}

inline bool
pool::TokenList::operator!=( const pool::TokenList& rhs ) const
{
   return !( *this == rhs );
}

inline pool::TokenList::iterator
pool::TokenList::begin()
{
   return pool::TokenList::iterator( this );
}

inline pool::TokenList::iterator
pool::TokenList::end()
{
   return pool::TokenList::iterator( this, size() );
}

inline pool::TokenList::const_iterator
pool::TokenList::begin() const
{
   return pool::TokenList::const_iterator( this );
}

inline pool::TokenList::const_iterator
pool::TokenList::end() const
{
   return pool::TokenList::const_iterator( this, size() );
}

#endif

