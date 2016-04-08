/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTHITCOLLECTION_P2_H
#define TRTHITCOLLECTION_P2_H

/*

Author: Rob Duxfield <r.duxfield@sheffield.ac.uk>

*/

#include <vector>
#include <string>

class TRT_HitCollection_p2   
{
 public:
  /// Default constructor
  TRT_HitCollection_p2 ();
  // private:

  std::vector<float>          m_hit1_startX;    //  1 element per string
  std::vector<float>          m_hit1_startY;    //
  std::vector<float>          m_hit1_startZ;    //
  std::vector<unsigned short> m_nHits;          //

  std::vector<float>          m_hitEne;         //  1 element per hit
  std::vector<float>          m_meanTime;       //
  std::vector<float>          m_kinEne;         //
  std::vector<float>          m_endX;           //
  std::vector<float>          m_endY;           //
  std::vector<float>          m_endZ;           //
  std::vector<unsigned long>  m_hitId;          //

  std::vector<unsigned long>  m_barcode;
  std::vector<unsigned short> m_nBC;

  std::vector<long>           m_id;
  std::vector<unsigned short> m_nId;
};


// inlines

inline
TRT_HitCollection_p2::TRT_HitCollection_p2 () {}

#endif
