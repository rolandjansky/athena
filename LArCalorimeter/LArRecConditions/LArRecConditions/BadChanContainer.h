/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BadChanContainer_H
#define BadChanContainer_H

#include <vector>
#include <algorithm>


template <class Chan, class State>
class BadChanContainer {
public:

  typedef std::pair<Chan,State>                BadChanEntry;
  typedef std::vector<BadChanEntry>            BadChanVec;
  typedef typename BadChanVec::const_iterator  const_iterator;
  typedef typename BadChanVec::size_type       size_type;

  struct ChannelLess {
    bool operator()(const BadChanEntry& a, const BadChanEntry& b) {
      return a.first < b.first;
    }
  };

public:

  BadChanContainer() {}

	
  BadChanContainer( const BadChanVec& vec) : m_cont(vec) {
	std::sort(m_cont.begin(), m_cont.end(), ChannelLess());
  }

  State status( Chan channel) const {

    const_iterator i = 
      std::lower_bound( m_cont.begin(), m_cont.end(), BadChanEntry( channel,(LArBadChannel) 0),
		   ChannelLess());
    if (i != m_cont.end() && i->first == channel) return i->second;
    else 
	return State(0);
  }

  const_iterator begin() const {return m_cont.begin();}
  const_iterator end() const {return m_cont.end();}
  size_type      size() const {return m_cont.size();}

  /// Deletes the contents and sets size to 0 (same as stl::vector<T>::clear())
  void clear() {m_cont.clear();}

  void set( const BadChanVec& cont) {
    m_cont = cont;
    std::sort(m_cont.begin(), m_cont.end(), ChannelLess());
  }

private:

  BadChanVec m_cont;

};


#endif
