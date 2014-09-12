/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FCBUF_H
#define FCBUF_H
#include <vector>
namespace pool{
  template <class Item>
    class FCBuf{
    public:
      FCBuf( const size_t& mymax){
	m_buf.reserve(mymax);
	m_max=mymax;
      }
      ~FCBuf(){}
      bool full(){
	return m_buf.size()==m_max;
      }
      bool empty(){
	return m_buf.empty();
      }
      size_t maxsize() const {
	return m_max;
      }
      size_t size() const{
	return m_buf.size();
      }
      bool isBoundary(const size_t& cur) const{
	return (cur%m_max)==0;
      }
      size_t nBoundary( const size_t& cur) const{
	return (size_t)(cur/m_max);
      }
      void clear(){
	m_buf.clear();
      }
      const Item& operator[] ( size_t& i) {
	return m_buf[i];
      }
      void push_back( const Item& i){
	m_buf.push_back(i);
      }
      std::vector<Item>& getData(){
	return m_buf;
      }
    private:
      size_t m_max;
      std::vector<Item> m_buf;
    };


}//ns pool
#endif
