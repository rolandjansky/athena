/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BadChanContainerHashed_H
#define BadChanContainerHashed_H

template <class Chan, class State, class Hasher>
class BadChanContainerHashed {
public:

  typedef std::pair<Chan,State>                BadChanEntry;
  typedef std::vector<BadChanEntry>            BadChanVec;
  typedef typename BadChanVec::const_iterator  const_iterator;
  typedef typename BadChanVec::size_type       size_type;

public:

  BadChanContainerHashed() {}

  explicit BadChanContainerHashed( const Hasher& h) : m_hasher(h) {}

  BadChanContainerHashed( const BadChanVec& vec) { set(vec);}

  State status( const Chan& channel) const;

  State status( const Chan& feb, int ch) const {return m_vec.at( m_hasher.hash( feb, ch));} 

  const_iterator begin() const {return m_bcv.begin();}
  const_iterator end() const {return m_bcv.end();}
  size_type      size() const {return m_bcv.size();}

  /// Deletes the contents and sets size to 0 (same as stl::vector<T>::clear())
  void clear() {m_vec.clear(); m_bcv.clear();}

  void set( const BadChanVec& cont);

  void setHasher( const Hasher& hasher) { m_hasher = hasher;}

private:

  typedef unsigned int         HashType;

  std::vector<State>   m_vec;
  Hasher               m_hasher;
  BadChanVec           m_bcv;

  // these methods should move to a Hasher class
  HashType hash( Chan ch) const {return m_hasher.hash(ch);}
  // Chan unhash( HashType h) const;
  HashType maxHash() const {return m_hasher.maxHash();}

};

template <class Chan, class State, class Hasher>
inline State BadChanContainerHashed<Chan,State,Hasher>::status( const Chan& channel) const 
{
  return m_vec[ hash( channel)];
}

template <class Chan, class State, class Hasher>
inline void BadChanContainerHashed<Chan,State,Hasher>::set( const BadChanVec& cont)
{
  State good; // default constructed state is good
  if (m_vec.size() == maxHash()) {
    for (typename std::vector<State>::iterator i=m_vec.begin(); i!=m_vec.end(); ++i) {
      *i = good;  // reset all channels to good
    }
  }
  else {
    std::vector<State> tmp( maxHash(), good);  // all channels good
    m_vec.swap(tmp);
  }
  for (typename BadChanVec::const_iterator i=cont.begin(); i!=cont.end(); ++i) {
    // m_vec.at( hash( i->first)) = i->second; // skip invalid hash values
    HashType h = hash( i->first);
    if (h < maxHash()) m_vec[h] = i->second;
  }

  m_bcv = cont;
}


#endif
