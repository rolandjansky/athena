/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTMATCHINFO_H
#define HTTMATCHINFO_H

#include <TObject.h>

class HTTMatchInfo : public TObject {
 public:
  HTTMatchInfo() : m_barcode(0), m_evtindex(-1) {;}
  HTTMatchInfo(int v1, int v2) : m_barcode(v1), m_evtindex(v2) {;}

  int barcode() const { return m_barcode; }
  int evtindex() const { return m_evtindex; }

  bool operator==(const HTTMatchInfo& o) const { return (m_barcode==o.m_barcode)&&(m_evtindex==o.m_evtindex); }
  bool operator<(const HTTMatchInfo& o) const { if (m_evtindex!=o.m_evtindex) return (m_evtindex<o.m_evtindex); else return m_barcode<o.m_barcode; }


 private:
  int m_barcode;
  int m_evtindex;


  ClassDef(HTTMatchInfo,1)
};

std::ostream& operator<<(std::ostream& , const HTTMatchInfo& );
#endif // HTTMATCHINFO_H
