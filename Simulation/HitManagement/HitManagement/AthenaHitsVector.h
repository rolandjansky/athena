/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// Templated class for the Hit collections in athena
// There is a bunch of ifdef __CINT__ to make this class
// intelligible to AthenaRoot and work out a persistency mechanism
// 


#ifndef AthenaHitsVector_H
#define AthenaHitsVector_H
// 
//
// vector class
#include <vector>
#include "AthContainers/tools/DVLInfo.h"

//
// Gaudi includes, not provided to rootcint
#ifndef __CINT__
  #include "GaudiKernel/ISvcLocator.h"
  #include "AthenaKernel/getMessageSvc.h"
  #include "GaudiKernel/MsgStream.h"
  #include "GaudiKernel/IMessageSvc.h"
#endif

//
template <typename T> 
  class AthenaHitsVector 
{
public:
  //
  // additional typedef 
  typedef T base_value_type; 
  typedef std::vector<T*>                           CONT; 
  typedef typename CONT::value_type value_type; 
  typedef typename CONT::pointer pointer;
  typedef typename CONT::const_pointer const_pointer;
  typedef typename CONT::iterator iterator;
  typedef typename CONT::const_iterator const_iterator;
  typedef typename CONT::reference reference;
  typedef typename CONT::const_reference const_reference;
  typedef typename CONT::size_type size_type;
  typedef typename CONT::difference_type difference_type;
  //
  // default constructor for rootcint
#ifdef __CINT__ 
  AthenaHitsVector<T>( ) {} 
  //
  // methods not provided to rootcint
#else 
  AthenaHitsVector<T>(std::string collectionName="DefaultCollectionName")
    {
      IMessageSvc* msgSvc(Athena::getMessageSvc());
      MsgStream log(msgSvc, "AthenaHitsVector");
      log << MSG::DEBUG << " initialized." << endmsg;
      
      m_name = collectionName;
    }
  ~AthenaHitsVector<T> () {Clear();}
  void Clear()
    {
	for (unsigned int i=0;i<m_hitvector.size();i++)
		delete m_hitvector[i];
	m_hitvector.clear();
    }

  void Insert(T* h)
    {
      m_hitvector.push_back(h);
    }
  int  Size() const
    {
      return size();
    }
#endif // __CINT__
  /// constructor makes deep copy of elements
  explicit AthenaHitsVector(const AthenaHitsVector<T>& rhs) {
    m_hitvector.reserve(rhs.m_hitvector.size());
    const_iterator i(rhs.begin()), e(rhs.end());
    while (i != e) {m_hitvector.push_back((0 != *i) ? new T(**i) : 0); ++i;}
  }

  /// assignment deletes old elements and deep copies the new ones
  AthenaHitsVector<T>& operator=(const AthenaHitsVector<T>& rhs) {
    if (this != &rhs) {
      this->Clear();
      m_hitvector.reserve(rhs.m_hitvector.size());
      const_iterator i(rhs.begin()), e(rhs.end());
      while (i != e) {m_hitvector.push_back((0 != *i) ? new T(**i) : 0); ++i;}
    }
    return *this;
  }

  const std::string& Name() const {return m_name;}

  void setName(const std::string& name) {m_name = name;}
  //
  // vector methods. 
  const std::vector<T*>& getVector() {return m_hitvector;}

  bool empty() const { return m_hitvector.empty(); } 

  const_iterator begin() const 
    { return m_hitvector.begin(); } 
  
  const_iterator end() const 
    { return m_hitvector.end(); } 
  
  iterator begin()  
    { return m_hitvector.begin(); } 
  
  iterator end()  
    { return m_hitvector.end(); } 
  
  size_type size() const { return m_hitvector.size(); }
  
  void push_back(T* t ) { m_hitvector.push_back(t);} 

  T* At(unsigned int pos) const {
	  return m_hitvector.at(pos);
	  }
  
  const T* operator[] (size_type n) const {return m_hitvector[n];}
  
  void resize(size_type sz) {
    if (sz < size()) {
      iterator i(m_hitvector.begin()+sz), e(m_hitvector.end());
      while (i != e) delete *i++;
      m_hitvector.resize(sz);      
    } else {
      m_hitvector.insert(m_hitvector.end(), sz - m_hitvector.size(), 0);
    }
  }

  void clear() {
      for (unsigned int i=0;i<m_hitvector.size();i++)
          delete m_hitvector[i];
      m_hitvector.clear();
  }

  void reserve (size_type n) {  m_hitvector.reserve (n); }


 protected:
  std::string     m_name;
  std::vector<T*> m_hitvector;


public:
  // Used to ensure that the DVLInfo gets registered
  // when the dictionary for this class is loaded.
  static const std::type_info* initHelper()
  { return DataModel_detail::DVLInfo<AthenaHitsVector<T> >::initHelper(); };
  static const std::type_info* const s_info;
};


/**
 * @brief Construct a new container.
 * @param nreserve Number of elements for which to reserve space.
 *                 (Ignored if not appropriate.)
 *
 * This is broken out from the @c make method to allow specializing
 * just this method.
 */
template <class T>
void dvl_makecontainer (size_t nreserve, AthenaHitsVector<T>*& cont)
{
  cont = new AthenaHitsVector<T>;
  cont->reserve (nreserve);
}


// Ensure that the DVLInfo gets registered
// when the dictionary for this class is loaded.
template <class T>
const std::type_info* const AthenaHitsVector<T>::s_info = AthenaHitsVector<T>::initHelper();


#endif
