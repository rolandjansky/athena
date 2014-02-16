/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************-*-c++-*-*************************************
                        IJetFitterTagInfo.h  -  Description
                             -------------------
    begin   : 2012
    authors : Dan Guest
    email   : dguest@cern.ch
              
    changes : 

 ***************************************************************************/


#ifndef JETTAGINFO_JETFITTERBASEINFO_H
#define JETTAGINFO_JETFITTERBASEINFO_H

#include "JetTagInfo/BaseTagInfo.h"
#include <vector>
#include <iostream>
#include <map>
#include <utility>
// #include <boost/unordered_map.hpp> 

namespace Trk
{
  class RecVertex;
}

namespace Analysis
{

/** The IJetFitterTagInfo class:
    Abstract baseclass for JetFitterTagInfo and JetFitterGenericTagInfo. 
    Can be used where JetFitterTagInfo was used before
*/
  
class IJetFitterTagInfo : public virtual BaseTagInfo
{
public:

  // may be needed to trick reflex... maybe not
  typedef std::map<std::string,int> IntMap; 
  typedef std::map<std::string,double> DoubleMap; 

  // --- lookup strings for backward compatibility  ---

  /** default destructor */
  virtual ~IJetFitterTagInfo(){};

  virtual IJetFitterTagInfo* clone() const = 0;
    
  /** number of vertices with more than one track. */
  virtual int nVTX() const = 0;

  /** number of single track vertices */
  virtual int nSingleTracks() const = 0;

  /** number of tracks at vertices (with more than one track - sum) */
  virtual int nTracksAtVtx() const = 0;

  /** The energy fraction: Evtx/Ejet of charges tracks */
  virtual double energyFraction() const = 0;

  /** The invariant mass of the tracks fitted to the vertices with at least 2 tracks. */
  virtual double mass() const = 0;

  /** significance3d of all vertices with more than one track or -if not there-  */
  /** of all single track vertices */

  virtual double significance3d() const = 0;
  

  /** get deltaphi between sum of all momenta at vertices 
      and the fitted B-meson flight direction */
  virtual double deltaphi() const = 0;

  /** get deltaeta between sum of all momenta at vertices 
      and the fitted B-meson flight direction */
  virtual double deltaeta() const = 0;


  /** Set-methods for the private data members of the class. */
  virtual  void setnVTX(int nVTX) = 0;
  virtual  void setnSingleTracks(int nSingleTracks) = 0;
  virtual  void setnTracksAtVtx(int nTracksAtVtx) = 0;
  virtual  void setMass(double mass) = 0;
  virtual  void setEnergyFraction(double energyFraction) = 0;
  virtual  void setSignificance3d(double significance3d) = 0;
  virtual  void setDeltaeta(double deltaeta) = 0;
  virtual  void setDeltaphi(double deltaphi) = 0;
    
private:


}; // End class



} // End namespace
#endif
