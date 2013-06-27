/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWBinLabels                         //
//                                                            //
//  Description: A mem-efficient, LWPools using, analogy of   //
//               std::map<unsigned,std::string>, needed for   //
//               SetBinLabel/GetBinLabel functionality.       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2009                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWBINLABELS_H
#define LWBINLABELS_H

#include <utility>

class TAxis;
class LWBinLabels {
public:

  LWBinLabels();
  ~LWBinLabels();

  const char * getBinLabel(unsigned bin) const;//0 if not set
  void setBinLabel(unsigned bin, const char* label);

  void apply(TAxis*) const;
  unsigned size() const { return  m_size; }

private:
  LWBinLabels( const LWBinLabels & );
  LWBinLabels & operator= ( const LWBinLabels & );

  unsigned m_size;
  unsigned m_allocated;
  typedef std::pair<unsigned,char*> Entry;
  Entry * m_list;
  void grow();
  static bool cmp(const Entry& e1, const Entry& e2) { return e1.first<e2.first; }

};

#endif
