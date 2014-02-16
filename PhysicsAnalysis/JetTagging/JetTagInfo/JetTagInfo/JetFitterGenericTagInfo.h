/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************-*-c++-*-*************************************
                        JetFitterTagInfo.h  -  Description
                             -------------------
    begin   : 2012
    authors : Dan Guest
    email   : dguest@cern.ch


 ***************************************************************************/


#ifndef JETTAGINFO_GENERICTAGINFO_H
#define JETTAGINFO_GENERICTAGINFO_H

#include "JetTagInfo/IJetFitterTagInfo.h"
#include <vector>
#include <iostream>
#include <map>
#include <utility>

namespace Trk
{
  class RecVertex;
}

namespace Analysis
{

  /** Generic replacement for JetFitterTagInfo

      Internally stores the same variables as JetFitterTagInfo, 
      plus any other int or double type variables, within an std::map. 
      Created to streamline the coding of a classifier with many inputs. 
   */
  
class JetFitterGenericTagInfo : public IJetFitterTagInfo
{
public:

  // may be needed to trick reflex
  typedef std::map<std::string,int> IntMap; 
  typedef std::map<std::string,double> DoubleMap; 

  // --- lookup strings for backward compatibility  ---

  static const char* N_VTX; 
  static const char* N_SINGLE_TRACKS;
  static const char* N_TRACKS_AT_VTX; 
  static const char* MASS;
  static const char* ENERGY_FRACTION;
  static const char* SIGNIFICANCE_3D;
  static const char* DELTA_ETA;
  static const char* DELTA_PHI;

  /** default constructor */
  JetFitterGenericTagInfo();

  /** constructor with infotype */
  JetFitterGenericTagInfo(TagInfoType x);


  /** default destructor */
  virtual ~JetFitterGenericTagInfo();

  virtual JetFitterGenericTagInfo* clone() const;
  
  void clear(); 

  /** number of vertices with more than one track. */
  int nVTX() const;

  /** number of single track vertices */
  int nSingleTracks() const;

  /** number of tracks at vertices (with more than one track - sum) */
  int nTracksAtVtx() const;

  /** The energy fraction: Evtx/Ejet of charges tracks */
  double energyFraction() const;

  /** The invariant mass of the tracks fitted to the vertices with at 
      least 2 tracks. */
  double mass() const;

  /** significance3d of all vertices with more than one track 
      or -if not there-  */
  /** of all single track vertices */

  double significance3d() const;

  /** get deltaphi between sum of all momenta at vertices 
      and the fitted B-meson flight direction */
  double deltaphi() const;

  /** get deltaeta between sum of all momenta at vertices 
      and the fitted B-meson flight direction */
  double deltaeta() const;

  /** clean out temporary variables (if you're saving info)
      returns the number of variables removed */
  int clearTemporary(); 

  /** returns numeric_limits::min() for unset values */
  double getDouble(std::string) const; 
  int getInt(std::string) const; 

  /** these versions will throw a range_error if the value isn't set*/
  double getDoubleOrThrow(std::string) const; // <- may throw range_error
  int getIntOrThrow(std::string) const; // <- may throw range_error

  DoubleMap::const_iterator getDoublesBegin() const; 
  DoubleMap::const_iterator getDoublesEnd() const; 
  IntMap::const_iterator getIntsBegin() const; 
  IntMap::const_iterator getIntsEnd() const; 

  //** returns true if the value is already set */
  bool setDouble(std::string, double); 
  bool setTemporaryDouble(std::string, double); 
  bool setInt(std::string, int); 
  bool setTemporaryInt(std::string, int); 
    
  /** Set-methods for the private data members of the class. */
  void setnVTX(int nVTX);
  void setnSingleTracks(int nSingleTracks);
  void setnTracksAtVtx(int nTracksAtVtx);
  void setMass(double mass);
  void setEnergyFraction(double energyFraction);
  void setSignificance3d(double significance3d);
  void setDeltaeta(double deltaeta);
  void setDeltaphi(double deltaphi);

    
private:
  
  DoubleMap m_experimental_doubles; 
  IntMap m_experimental_ints; 
  std::vector<std::string> m_temporary_variables; 
 
  /** copy constructor */
  JetFitterGenericTagInfo(const JetFitterGenericTagInfo& rhs);

  /** assigenment operator */
  JetFitterGenericTagInfo &operator= (const JetFitterGenericTagInfo& rhs);


}; // End class



} // End namespace
#endif
