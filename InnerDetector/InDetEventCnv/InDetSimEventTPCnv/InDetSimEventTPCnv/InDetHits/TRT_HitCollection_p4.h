/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTHITCOLLECTION_P4_H
#define TRTHITCOLLECTION_P4_H

/*
   Author: Rob Duxfield <r.duxfield@sheffield.ac.uk> Spring 2008
   _p3 Integer compression: <Andrew.Beddall@cern.ch> Spring 2009
   _p4 HepMcParticleLink_p2 <Olivier.Arnaez@cern.ch> Fall 2015
   See http://cern.ch/beddall/TRThitCompression/
*/

#include <vector>
#include <string>

class TRT_HitCollection_p4
{
 public:
  /// Default constructor
  TRT_HitCollection_p4 ();

  //private:

  //
  //  1 element per string (a string resides in one straw; there may be more than one string in a straw)
  //

  std::vector<unsigned short> m_nHits;       // number of hits in the string (0,1,2 ... ,hundreds).
  std::vector<unsigned short> m_strawId2b;   // straw id | 24-bit
  std::vector<unsigned char>  m_strawId1b;   // straw id | integer.
  std::vector<unsigned char>  m_startR;      // hit start radius (0, 2 mm) [not always stored].
  std::vector<unsigned char>  m_startPhi;    // hit start phi (-pi, pi).
  std::vector<unsigned char>  m_startZ;      // hit start z (-365, +365 mm), and 1-bit startRflag.

  //
  //  1 element per hit, there are typically 1 or 2 hits per string, but can be hundreds!
  //

  std::vector<unsigned short> m_kinEne;      // short float, kinematic energy of the particle causing the hit.
  std::vector<unsigned short> m_steplength;  // short float, g4 step length; endZ is derived from this.
  std::vector<unsigned char>  m_endR;        // hit end radius (0, 2 mm) [Not always stored].
  std::vector<unsigned char>  m_endPhi;      // hit end phi (-pi, pi).
  std::vector<unsigned short> m_meanTime;    // time to center of the hit, and 1-bit idZsign and 1-bit endRflag.
  std::vector<float>          m_meanTimeof;  // t >= 75 ns overflow to a float.

  //
  // much less frequent
  //

  std::vector<float>          m_hitEne; // energy deposited; *only stored for photons* (m_id=22)
  std::vector<unsigned short> m_nId;
  std::vector<unsigned int>   m_barcode;
  std::vector<unsigned short> m_mcEvtIndex;
  std::vector<char>           m_evtColl;
  std::vector<unsigned short> m_nBC;
  std::vector<int>            m_id;     // particle code.

};

// inlines

inline
TRT_HitCollection_p4::TRT_HitCollection_p4 () {}

#endif
