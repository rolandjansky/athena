/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// Templated class for the Hit collections in athena
// There is a bunch of ifdef __CINT__ to make this class
// intelligible to AthenaRoot and work out a persistency mechanism
//


#ifndef AtlasHitsVector_H
#define AtlasHitsVector_H
//
//
// vector class
#include <vector>
#include "HitManagement/AthenaHitsVector.h"
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
class AtlasHitsVector
{
public:
  //
  // additional typedef
  typedef T base_value_type;
  typedef std::vector<T>                           CONT;
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
  AtlasHitsVector<T>( ) {}
  //
  // methods not provided to rootcint
#else
  AtlasHitsVector<T>(std::string collectionName="DefaultCollectionName", const unsigned int mySize=100)
  {
    IMessageSvc* msgSvc(Athena::getMessageSvc());
    MsgStream log(msgSvc, "AtlasHitsVector");
    log << MSG::DEBUG << " initialized AtlasHitVector " << collectionName << endmsg;

    m_name = collectionName;
    m_hitvector.reserve(mySize);
  }

  ~AtlasHitsVector<T> () {
    std::vector<T>().swap(m_hitvector);
  }
  void Clear()
  {
    m_hitvector.clear();
    std::vector<T>().swap(m_hitvector);
  }

  void Insert(const T& h)
  {
    m_hitvector.push_back(h);
  }
  void Insert(T&& h)
  {
    m_hitvector.push_back( std::move(h) );
  }
  template <class... Args> void Emplace(Args&&... args)
  {
    m_hitvector.emplace_back( std::forward<Args>(args)... );
  }
  int  Size() const
  {
    return size();
  }
#endif // __CINT__

  explicit AtlasHitsVector(const AtlasHitsVector<T>& rhs)
    : m_hitvector(rhs.m_hitvector) {}

  // Conversion
  explicit AtlasHitsVector(const AthenaHitsVector<T>& rhs) {
    m_hitvector.reserve(rhs.Size());
    typename AthenaHitsVector<T>::const_iterator i(rhs.begin()), e(rhs.end());
    while (i != e) {m_hitvector.push_back( T( (**i) ) ); ++i;}
  }

  AtlasHitsVector<T>& operator=(const AtlasHitsVector<T>& rhs) {
    if (this != &rhs) {
      m_hitvector=rhs.m_hitvector;
    }
    return *this;
  }

  /// assignment deletes old elements and deep copies the new ones
  // Assignment from the AthenaHitsVector form
  AtlasHitsVector<T>& operator=(const AthenaHitsVector<T>& rhs) {
    this->Clear();
    m_hitvector.reserve(rhs.Size());
    typename AthenaHitsVector<T>::const_iterator i(rhs.begin()), e(rhs.end());
    while (i != e) {m_hitvector.push_back( T( (**i) ) ); ++i;}
    return *this;
  }

  const std::string& Name() const {return m_name;}

  void setName(const std::string& name) {m_name = name;}
  //
  // vector methods.
  const std::vector<T>& getVector() const {return m_hitvector;}

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

  void push_back(const T& t ) { m_hitvector.push_back(t);}

  T At(unsigned int pos) const {
    return m_hitvector.at(pos);
  }

  const T operator[] (size_type n) const {return m_hitvector[n];}

  void clear() {
    m_hitvector.clear();
    std::vector<T>().swap(m_hitvector);
  }

  void reserve (size_type n) {  m_hitvector.reserve (n); }

  void resize (size_type n) {  m_hitvector.resize (n); }


protected:
  std::string     m_name;
  std::vector<T>  m_hitvector;


public:
  // Used to ensure that the DVLInfo gets registered
  // when the dictionary for this class is loaded.
  static const std::type_info* initHelper()
  { return DataModel_detail::DVLInfo<AtlasHitsVector<T> >::initHelper(); }
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
void dvl_makecontainer (size_t nreserve, AtlasHitsVector<T>*& cont)
{
  cont = new AtlasHitsVector<T> ("", nreserve);
}


// Ensure that the DVLInfo gets registered
// when the dictionary for this class is loaded.
template <class T>
const std::type_info* const AtlasHitsVector<T>::s_info = AtlasHitsVector<T>::initHelper();


#endif
