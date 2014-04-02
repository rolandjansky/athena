/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGENALGS_SPLAYER_H
#define INDETALIGNGENALGS_SPLAYER_H
// SPLayer.h  - helper class for InDetAlignNt for processing overlaps
// definition of one layer of space points

#include <vector>
#include <string>

class TH1;

class SPLayer {
 public:
  SPLayer(const int,const int,const int,const int,const int,const int, 
	  const std::string&, const std::vector<TH1*>& );
  int incnmod(); // increment the number of modules
  int incnhit(); // increment the number of hits
  // read methods
  int dettype() const;
  int bec() const;
  int layer() const;
  int side() const;
  int nmod() const;
  int nhit() const;
  TH1* hptr(int) const;
  std::string name() const;

 private:
  int m_hbase;
  int m_hbase2;
  int m_dettype;
  int m_bec;
  int m_layer;
  int m_side;
  int m_nmod;
  int m_nhit;
  std::string m_name;
  std::vector<TH1*> m_histlist;
};

inline int SPLayer::incnmod() { return ++m_nmod; }
inline int SPLayer::incnhit() { return ++m_nhit; }

inline int SPLayer::dettype() const { return m_dettype; }
inline int SPLayer::bec() const { return m_bec; }
inline int SPLayer::layer() const { return m_layer; }
inline int SPLayer::side() const { return m_side; }
inline int SPLayer::nmod() const { return m_nmod; }
inline int SPLayer::nhit() const { return m_nhit; }
inline std::string SPLayer::name() const { return m_name; }
inline TH1* SPLayer::hptr(int i) const { return m_histlist[i]; }

#endif // INDETALIGNGENALGS_SPLAYER_H


