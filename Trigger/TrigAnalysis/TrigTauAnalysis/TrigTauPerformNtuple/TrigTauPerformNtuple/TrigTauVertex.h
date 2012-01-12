/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUVERTEX_H_
#define _TRIGTAUVERTEX_H_

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauVertex
/// Package : TauTrigPerformNtuple
/// Author  : Joerg Mechnich (Nikhef Amsterdam)
/// Created : 
///
/// DESCRIPTION:
///         offline vertex
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <TObject.h>

class TrigTauVertex: public TObject
{
 public:
  TrigTauVertex();
  void reset();

  float distance( const TrigTauVertex& other) const;
  
public :
  float          m_x;
  float          m_y;
  float          m_z;

  float          m_chi2;
  float          m_nDoF;

  unsigned int   m_nTracks;
  unsigned int   m_type;
  
  std::vector<float> m_trackWeight; // one float for each track.
  
 private:
  ClassDef(TrigTauVertex, 2);
};

#endif //TRIGTAUVERTEX_H
