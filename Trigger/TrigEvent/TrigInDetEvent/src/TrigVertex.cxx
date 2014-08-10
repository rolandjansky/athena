/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigVertex.h"

std::string str( const TrigVertex& v )                      //<! printing helper
{
  std::stringstream ss;
  const HepGeom::Point3D<double>& pos = v.position();
  int nTracks=0;

  if(v.tracks()!=NULL) nTracks=v.tracks()->size();

  ss << "AlgorithmID: "<<v.algorithmId()
     << "x: "<<pos.x()
     <<" y: "<<pos.y()
     <<" z: "<<pos.z()
     <<" chi2: "<<v.chi2()
     <<" ndof: "<<v.ndof()
     <<" ntracks: "<<nTracks
     <<" mass: "<<v.mass()
     <<" energyFraction: "<<v.energyFraction()
     <<" nTwoTracksSecVtx: "<<v.nTwoTracksSecVtx();
  return ss.str();

}

MsgStream& operator<< ( MsgStream& m, const TrigVertex& v ) //<! printing helper (wraps above)
{
  m << str(v);
  return m;
}

bool operator== ( const TrigVertex& a, const TrigVertex& b )
{
  const double epsilon=1e-8;
  int nTracksA=0;
  int nTracksB=0;

  if(a.tracks()!=NULL) nTracksA=a.tracks()->size();
  if(b.tracks()!=NULL) nTracksB=b.tracks()->size();
  const HepGeom::Point3D<double>& posA = a.position();
  const HepGeom::Point3D<double>& posB = b.position();

  if(a.algorithmId() != b.algorithmId()) return false;
  if(nTracksA != nTracksB) return false;
  if(a.ndof() != b.ndof()) return false;
  if(fabs(posA.x()-posB.x())>epsilon) return false;
  if(fabs(posA.y()-posB.y())>epsilon) return false;
  if(fabs(posA.z()-posB.z())>epsilon) return false;
  if(fabs(a.chi2()-b.chi2())>epsilon) return false;
  if(fabs(a.mass()-b.mass())>epsilon) return false;
  return true;
}

/** @brief comparison with feedback
 * Function compares two objects and returns "semi verbose" output 
 * in the form of map where there are variable names and differences
 * between two objects
 * @param variableChange - map to record the differences
 * In case of collections (or objects when the size may be different) that information can also be returned in variableChange
 */
void diff( const TrigVertex& a, const TrigVertex& b, std::map<std::string, double>& variableChange )
{
  int nTracksA=0;
  int nTracksB=0;

  if(a.tracks()!=NULL) nTracksA=a.tracks()->size();
  if(b.tracks()!=NULL) nTracksB=b.tracks()->size();
  const HepGeom::Point3D<double>& posA = a.position();
  const HepGeom::Point3D<double>& posB = b.position();

  variableChange["x"] = posA.x() - posB.x();
  variableChange["y"] = posA.y() - posB.y();
  variableChange["z"] = posA.z() - posB.z();
  variableChange["ntracks"] = (double)(nTracksA - nTracksB);
  variableChange["chi2"] = a.chi2() - b.chi2();
  variableChange["ndof"] = (double)(a.ndof() - b.ndof());
  variableChange["mass"] = a.mass() - b.mass();
  variableChange["energyFraction"] = a.energyFraction() - b.energyFraction();
  variableChange["nTwoTracksSecVtx"] = (double)(a.nTwoTracksSecVtx() - b.nTwoTracksSecVtx());
}
