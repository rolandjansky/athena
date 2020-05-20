/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBIDENTIFIER_MULTIDIMARRAY_H
#define MUONCALIBIDENTIFIER_MULTIDIMARRAY_H

/***************************************************************************
 * Variable Multidimensional Array
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 20 April 2004
 * Last Update  : 26 April 2005
 ***************************************************************************/

#include <iostream>
#include <sstream>
#include <limits.h>
#include <float.h>

// general default value calls default constructor

template <class T, unsigned int N> class MultiDimArray;

/** General recursive subtyping trait. 
for MultiDimArray class. */  

template <class T, unsigned int I>
class MultiDimArrayTypes {
public:
   /** define ThisType as a MultiDimArray<T,I>*/
   typedef MultiDimArray<T,I>   ThisType;
   /** define the SubType as MultiDimArrayTypes<T,I-1> */
   typedef MultiDimArray<T,I-1> SubType;
   /** @returns ThisType */ 
   static ThisType defaultThisType();
};

/** General recursive subtyping trait.
 *  Special case with N=0 corresponds to ValueType and there is no SubType.*/  

template<class T>
class MultiDimArrayTypes<T,0> {
public:
   /** define ThisType as T::ValueType */
   typedef typename T::ValueType ThisType;
   /** @returns T::defaultValue() */ 
   static ThisType defaultThisType() { return T::defaultValue(); }
   // no subtype
};

/** General recursive subtyping trait.
 *  Special case with N=1 needed to end recursion.*/  

template<class T>
class MultiDimArrayTypes<T,1> {
public:
   /** define ThisType as a MultiDimArray<T,N> with N=1 */
   typedef MultiDimArray<T,1> ThisType;
   /** define the SubType as MultiDimArrayTypes<T,0>::ThisType */
   typedef typename MultiDimArrayTypes<T,0>::ThisType SubType;
   /** @returns ThisType */ 
   static ThisType defaultThisType();
};

/** Multi-dimensional array with a compile-time number of dimensions, and a
run-time complete freedom over the size of the dimensions. 
It is used by IdentifierToHash to provide fast access in identifier to hash 
conversions. It is obtained as a vector of pointers to MultiDimArrayTypes<T,N>.*/

template <class T, unsigned int N>
class MultiDimArray {
public:
   //
   // several needed type definitions
   //
   typedef MultiDimArray<T,N> ThisType;
   typedef typename MultiDimArrayTypes<T,N>::SubType SubType;
   typedef typename T::ValueType ValueType;
   // default (invalid) values for subtypes
   static SubType defaultSubType();
   static ValueType defaultValueType();
   static const SubType& invalidSubType();
   MultiDimArray();
   ~MultiDimArray();
   MultiDimArray( const MultiDimArray& );
   MultiDimArray& operator=( const MultiDimArray& );
   void clear();
   /* Read-write access to subtype at index. If subtype at index does not exist,
      it will be created. */
   SubType& operator[]( int index );
   /* Read-only access to subtype at index.  If subtype at index does not exist,
      an invalid subtype is returned. */
   const SubType& operator[]( int index ) const;
   /* Minimum index of this field */
   int minIndex() const;
   /* Maximum index of this field */
   int maxIndex() const;
   /** Size of this field */
   unsigned int size() const;
   /** Depth of this field, i.e. the number of fields from this fields downwards,
    *  including this field. It is equal to this field number plus one. */
   unsigned int depth() const;
   /** Check that an index is in the range of the this field */
   bool isInRange( int index ) const;
   /** The total number of elements, including  invalid (=default) holes,
       from this field downwards */
   unsigned int totalSize() const;
   /** The total number of valid (=non-default) elements from this field downwards */
   unsigned int validSize() const;
   /** Dump the complete table to an output stream */
   void dump( std::ostream& os = std::cout ) const;
   /** Dump the complete table to an output stream, with an additional prefix before each line. */
   void dump( const std::string& prefix, std::ostream& os = std::cout ) const;
   /** Dump the complete table into a string */
   std::string dumpToString() const; 
   /** Dump the complete table into a string, with an additional prefix before each line. */
   std::string dumpToString( const std::string& prefix ) const; 
   /** Dump one entry with given indices to an output stream */
   template< class K > 
   void dumpOneEntry( const K& indices, std::ostream& os = std::cout ) const;
   /** Dump one entry with given indices into a string */
   template< class K > 
   std::string dumpOneEntryToString( const K& indices ) const;
private:
   SubType* m_data;
   int m_minIndex;
   unsigned int m_size;
   static SubType s_invalidSubType;
#ifdef MULTIDIMARRAY_DEBUG
   static unsigned int s_objectCount;
#endif
};

template <class T, unsigned int I>
inline typename MultiDimArrayTypes<T,I>::ThisType MultiDimArrayTypes<T,I>::defaultThisType() {
   return ThisType();
}

template <class T>
inline typename MultiDimArrayTypes<T,1>::ThisType MultiDimArrayTypes<T,1>::defaultThisType() {
   return ThisType();
}

template <class T, unsigned int N>
inline typename MultiDimArray<T,N>::SubType MultiDimArray<T,N>::defaultSubType() {
   return MultiDimArrayTypes<T,N-1>::defaultThisType();
}

template <class T, unsigned int N>
inline typename MultiDimArray<T,N>::ValueType MultiDimArray<T,N>::defaultValueType() {
   return MultiDimArrayTypes<T,0>::defaultThisType();
}

template <class T, unsigned int N>
inline const typename MultiDimArray<T,N>::SubType& MultiDimArray<T,N>::invalidSubType() {
   return s_invalidSubType;
}

template <class T, unsigned int N>
inline MultiDimArray<T,N>::MultiDimArray()
   : m_data(0), m_minIndex(0), m_size(0)
{
#ifdef MULTIDIMARRAY_DEBUG
   ++s_objectCount;
   MsgStream log(Athena::getMessageSvc(),"MultiDimArray<T,N>");
   log<<MSG::DEBUG<<"Creating MultiDimArray<" << N << ">" << " at " << this << " #objects: " << s_objectCount<<endmsg;
#endif
}

template <class T, unsigned int N>
inline MultiDimArray<T,N>::~MultiDimArray() {
#ifdef MULTIDIMARRAY_DEBUG
   --s_objectCount;
   MsgStream log(Athena::getMessageSvc(),"MultiDimArray<T,N>");
   log<<MSG::DEBUG<<"Deleting MultiDimArray<" << N << ">" << " at " << this << " #objects: " << s_objectCount<<endmsg;
#endif
   delete[] m_data;
}

template <class T, unsigned int N>
inline MultiDimArray<T,N>::MultiDimArray( const MultiDimArray& rhs )
   : m_data(0), m_minIndex(0), m_size(0)
{
#ifdef MULTIDIMARRAY_DEBUG
   ++s_objectCount;
   MsgStream log(Athena::getMessageSvc(),"MultiDimArray<T,N>");
   log<<MSG::DEBUG<<"Copying  MultiDimArray<" << N << ">" << " at " << this << " #objects: " << s_objectCount<<endmsg;
#endif
   operator=( rhs );
}

template <class T, unsigned int N>
inline void MultiDimArray<T,N>::clear() {
   delete[] m_data;
   m_data = 0;
   m_size = 0;
   m_minIndex = 0;
}

template <class T, unsigned int N>
inline int MultiDimArray<T,N>::minIndex() const {
   return m_minIndex;
}

template <class T, unsigned int N>
inline int MultiDimArray<T,N>::maxIndex() const {
   return m_minIndex + m_size - 1;
}

template <class T, unsigned int N>
inline unsigned int MultiDimArray<T,N>::size() const {
   return m_size;
}

template <class T, unsigned int N>
inline unsigned int MultiDimArray<T,N>::depth() const {
   return N;
}

template <class T, unsigned int N>
inline bool MultiDimArray<T,N>::isInRange( int index ) const {
   return index >= minIndex() && index <= maxIndex();
}

template <class T, unsigned int N>
MultiDimArray<T,N>& MultiDimArray<T,N>::operator=( const MultiDimArray<T,N>& rhs ) {
#ifdef MULTIDIMARRAY_DEBUG
   MsgStream log(Athena::getMessageSvc(),"MultiDimArray<T,N>");
   log<<MSG::DEBUG<<"Assigning MultiDimArray<" << N << ">" << " to " << this << " from " << &rhs<<endmsg;
#endif
   // ensure equal data size
   if ( m_size != rhs.m_size ) {
      delete[] m_data;
      m_size = rhs.m_size;
      if ( m_size ) {
	 m_data = new SubType[m_size];
      } else {
	 m_data = 0;
      }
   }
   // copy the data
   for ( unsigned int i = 0; i < m_size; ++i ) m_data[i] = rhs.m_data[i];
   // set others
   m_minIndex = rhs.m_minIndex;
   
   return *this;
}

template <class T, unsigned int N>
inline const typename MultiDimArray<T,N>::SubType& MultiDimArray<T,N>::operator[]( int index ) const {
   if ( !isInRange( index ) ) {
      MsgStream log(Athena::getMessageSvc(),"MultiDimArray<T,N>");
      log<<MSG::WARNING<<"MultiDimArray<" << N << ">::operator["<< index << "]" << " index out of range (" << minIndex() << "," << maxIndex() << ")"<<endmsg;
      return s_invalidSubType;
   }
   return m_data[index - m_minIndex];
}

template <class T, unsigned int N>
typename MultiDimArray<T,N>::SubType& MultiDimArray<T,N>::operator[]( int index ) {
#ifdef MULTIDIMARRAY_DEBUG
      MsgStream log(Athena::getMessageSvc(),"MultiDimArray<T,N>");
#endif   if ( !m_data ) {
      m_minIndex = index;
      m_size = 1;
      m_data = new SubType[1];
      m_data[0] = s_invalidSubType;
#ifdef MULTIDIMARRAY_DEBUG
      log<<MSG::DEBUG<<"MultiDimArray<" << N << ">::operator["<< index << "]" << " at " << this << ":" << " new data array. size=" << m_size << " range=(" << minIndex() << "," << maxIndex() << ")"<<endmsg;
#endif
   } else if ( index > maxIndex() ) {
      // add to vector at back
      unsigned int oldSize = m_size;
      int nAdd = index - maxIndex();
      m_size += nAdd;
      SubType* newData = new SubType[m_size];
      // copy old data and set new data to invalid
      unsigned int i = 0;
      for ( ; i < oldSize; ++i ) newData[i] = m_data[i];
      for ( ; i < m_size ; ++i ) newData[i] = s_invalidSubType;
      delete[] m_data;
      m_data = newData;
#ifdef MULTIDIMARRAY_DEBUG
      log<<MSG::DEBUG<<"MultiDimArray<" << N << ">::operator["<< index << "]" << " at " << this << ":" << " added " << nAdd << " at back. size=" << m_size << " range=(" << minIndex() << "," << maxIndex() << ")"<<endmsg;
#endif
   } else if ( index < minIndex() ) {
      // add to vector at front
      unsigned int nAdd = minIndex() - index;
      m_size += nAdd;
      SubType* newData = new SubType[m_size];
      // copy old data and set new data to invalid
      unsigned int i = 0;
      for ( ; i < nAdd  ; ++i ) newData[i] = s_invalidSubType;
      for ( ; i < m_size; ++i ) newData[i] = m_data[i-nAdd];
      delete[] m_data;
      m_data = newData;
      m_minIndex = index;
#ifdef MULTIDIMARRAY_DEBUG
      log<<MSG::DEBUG<<"MultiDimArray<" << N << ">::operator["<< index << "]" << " at " << this << ":" << " added " << nAdd << " at front. size=" << m_size << " range=(" << minIndex() << "," << maxIndex() << ")"<<endmsg;
#endif
   }
   // return ref to indexed value
   return m_data[index - m_minIndex];
}


template <class T, unsigned int N>
static unsigned int totalSize( const MultiDimArray<T,N>& ht ) {
   unsigned int nTotal = 0;
   int idxMin = ht.minIndex();
   int idxMax = ht.maxIndex();
   for ( int idx = idxMin; idx <= idxMax; ++idx ) {
      nTotal += totalSize( ht[idx] );
   }
   return nTotal;
}

template <class T, unsigned int N>
static unsigned int validSize( const MultiDimArray<T,N>& ht ) {
   unsigned int nTotal = 0;
   int idxMin = ht.minIndex();
   int idxMax = ht.maxIndex();
   for ( int idx = idxMin; idx <= idxMax; ++idx ) {
      nTotal += validSize( ht[idx] );
   }
   return nTotal;
}

template<class T>
static unsigned int totalSize( const MultiDimArray<T,1>& ht ) {
   return ht.size();
}

template<class T>
static unsigned int validSize( const MultiDimArray<T,1>& ht ) {
   unsigned int cnt = 0;
   int idxMin = ht.minIndex();
   int idxMax = ht.maxIndex();
   for ( int idx = idxMin; idx <= idxMax; ++idx ) {
      if ( ht[idx] != MultiDimArray<T,1>::invalidSubType() ) ++cnt;
   }
   return cnt;
}

template <class T, unsigned int N>
inline unsigned int MultiDimArray<T,N>::totalSize() const {
   return ::totalSize( *this );
}

template <class T, unsigned int N>
inline unsigned int MultiDimArray<T,N>::validSize() const {
   return ::validSize( *this );
}

template <class T, unsigned int N>
void dump( const MultiDimArray<T,N>& idh, std::ostream& os = std::cout,
	   const std::string& prefix = "(" ) {
   int idxMin = idh.minIndex();
   int idxMax = idh.maxIndex();
   for ( int idx = idxMin; idx <= idxMax; ++idx ) {
      std::ostringstream oss;
      oss << idx << ",";
      dump( idh[idx], os, prefix + oss.str() );
   }
}

template <class T>
void dump( const MultiDimArray<T,1>& idh, std::ostream& os = std::cout,
	   const std::string& prefix = "(" ) {
   int idxMin = idh.minIndex();
   int idxMax = idh.maxIndex();
   for ( int idx = idxMin; idx <= idxMax; ++idx ) {
      os << prefix << idx << "): hash=" << idh[idx] << "\n";
   }
}

template <class T, unsigned int N>
inline void MultiDimArray<T,N>::dump( std::ostream& os ) const {
   ::dump( *this, os );
   os.flush();
}

template <class T, unsigned int N>
inline void MultiDimArray<T,N>::dump( const std::string& prefix, std::ostream& os ) const {
   ::dump( *this, os, prefix + "(" );
   os.flush();
}

template <class T, unsigned int N>
inline std::string MultiDimArray<T,N>::dumpToString() const {
   std::ostringstream oss;
   dump( oss );
   return oss.str();
}

template <class T, unsigned int N>
inline std::string MultiDimArray<T,N>::dumpToString( const std::string& prefix ) const {
   std::ostringstream oss;
   dump( prefix, oss );
   return oss.str();
}

template <class T, unsigned int N, class K>
void dumpOneEntry( const MultiDimArray<T,N>& idh, const K& indices,
		   std::ostream& os = std::cout, const std::string& prefix = "(" ) {
   int idx = indices[N-1];
   std::ostringstream oss;
   oss << idx << ",";
   dumpOneEntry( idh[idx], indices, os, prefix + oss.str() );
}

template<class T, class K>
void dumpOneEntry( const MultiDimArray<T,1>& idh, const K& indices,
		   std::ostream& os = std::cout, const std::string& prefix = "(" ) {
   int idx = indices[0];
   os << prefix << idx << ")=" << idh[idx];
}

template <class T, unsigned int N>
template <class K>
inline void MultiDimArray<T,N>::dumpOneEntry( const K& indices, std::ostream& os ) const {
   ::dumpOneEntry( *this, indices, os );
}

template <class T, unsigned int N>
template< class K > 
std::string MultiDimArray<T,N>::dumpOneEntryToString( const K& indices ) const {
   std::ostringstream oss;
   dumpOneEntry( indices, oss );
   return oss.str();
}

// to ease output
template <class T, unsigned int N>
std::ostream& operator<<( std::ostream& os, const MultiDimArray<T,N>& idh ) {
   dump( idh, os );
   return os;
}

//
//static members
//
template <class T, unsigned int N>
typename MultiDimArray<T,N>::SubType MultiDimArray<T,N>::s_invalidSubType = MultiDimArray<T,N>::defaultSubType();

#ifdef MULTIDIMARRAY_DEBUG
template <class T, unsigned int N>
unsigned int MultiDimArray<T,N>::s_objectCount = 0;
#endif




#endif // MUONCALIBIDENTIFIER_MULTIDIMARRAY_H
