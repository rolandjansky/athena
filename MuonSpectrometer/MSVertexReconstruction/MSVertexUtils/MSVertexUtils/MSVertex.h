/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXUTILS_MSVERTEX_H
#define MSVERTEXUTILS_MSVERTEX_H


#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTracking/TrackParticle.h"

  
class MSVertex {
  
 public:
  
  MSVertex();
  MSVertex(const MSVertex&);
  MSVertex(int, const Amg::Vector3D&, float, float, int, int, int);
  MSVertex(int, const Amg::Vector3D&, const std::vector<xAOD::TrackParticle*>&, float, float, int, int, int);
  MSVertex& operator= (const MSVertex& msvx);

  virtual ~MSVertex();

  MSVertex* clone();

  void setPosition(const Amg::Vector3D&);

  const Amg::Vector3D& getPosition() const;

  const std::vector<xAOD::TrackParticle*>* getTracks() const;

  void setAuthor(const int);

  int getAuthor() const;

  float getChi2Probability() const;
  float getChi2() const;

  int getNTracks() const;

  void setNMDT(const int);
  void setNRPC(const int);
  void setNTGC(const int);

  int getNMDT() const;
  int getNRPC() const;
  int getNTGC() const;

private:

  unsigned int m_author;

  Amg::Vector3D m_position;

  std::vector<xAOD::TrackParticle*> m_tracks; 

  float m_chi2prob, m_chi2;
  
  int m_nMDT, m_nRPC, m_nTGC;
  
};

std::string str(const MSVertex& a);

MsgStream& operator<< (MsgStream& m, const MSVertex& a);

bool operator== (const MSVertex& a, const MSVertex& b);

inline bool operator!= (const MSVertex& a, const MSVertex& b) {return !(a == b);}


#endif // MSVERTEXUTILS_MSVERTEX_H
