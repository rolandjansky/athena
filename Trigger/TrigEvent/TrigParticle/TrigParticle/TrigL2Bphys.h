// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

File: TrigL2Bphys.h

Description: To be created by hypothesis algorithms at end of Level 2 
             chains. To be used as a way to communicate L2 decisions and 
             to keep information available only online so far.
  
Design: Must be light and easy to serialize AND to store in ESD; should 
             avoid ElementLinks and try to persistify pointers instead; 
             should avoid inheritance to be easy to serialize.

Authour: J.Kirk - Wed May 10 

*/

#ifndef TRIG_L2BPHYS
#define TRIG_L2BPHYS

#include <vector>
#include <iostream>
#include <string>
#include <map>

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "DataModel/ElementLink.h"
#include "DataModel/ElementLinkVector.h"
class TrigL2BphysContainer;
class TrigL2Bphys;


ELEMENTLINK_FWD(TrigL2BphysContainer, TrigL2Bphys);


class TrigL2Bphys  {
 public:
  
  /** enum for different particle types */
  enum pType{PHIKK=0, DSPHIPI=1, BMUMU=2, BMUMUX=3, JPSIEE=4, JPSIMUMU=5, MULTIMU=6, BKMUMU=7, BDKSTMUMU=8, BSPHIMUMU=9, LBLMUMU=10, BCDSMUMU=11};
  /** constructors; note: default constructor needed by POOL */
  TrigL2Bphys();

  /** constructor for particle of type "particleType" with mass "mass" and a secondary decay (given as ElementLink to another TrigEFBphys) */  
  TrigL2Bphys(int roi, float eta, float phi, pType particleType, 
	float mass, const ElementLink<TrigL2BphysContainer>& sDecay);

  /** constructor for particle of type "particleType" with mass "mass"*/
  TrigL2Bphys(int roi, float eta, float phi, pType particleType, float mass);


  /** copy constructor */
  TrigL2Bphys(const TrigL2Bphys& te);

  /** destructor */
  ~TrigL2Bphys();
  
  /** accessor method:  ID of L1 RoI */
  int   roiId()    const { return m_roiID;    }
  /** accessor method: particle Type */
  pType particleType() const { return m_particleType;    }
  /** accessor method: eta */
  float eta()      const { return m_eta;      }
  /** accessor method: phi */
  float phi()      const { return m_phi;      }
  /** accessor method: mass */
  float mass()     const { return m_mass;    }
  /** accessor method: mass from vertex fit */
  float fitmass()  const { return m_fitmass;    }
  /** accessor method: chi2 from vertex fit */
  float fitchi2()  const { return m_fitchi2;    }
  /** accessor method: ndof from vertex fit */
  int fitndof()  const { return m_fitndof;    }
  /** accessor method: x position of vertex */
  float fitx()  const { return m_fitx;    }
  /** accessor method: y position of vertex */
  float fity()  const { return m_fity;    }
  /** accessor method: z position of vertex */
  float fitz()  const { return m_fitz;    }

  /** accessor method: secondary decay particle */
  const TrigL2Bphys* pSecondDecay() const ;
  const ElementLink<TrigL2BphysContainer>& secondaryDecayLink() const;

  /** accessor method:vector of tracks used to make particle */
  const ElementLinkVector<TrigInDetTrackCollection>& trackVector() const {return m_trackVector;}

  /** set method: mass from vertex fit **/
  void fitmass(float FitMass);
  /** set method: chi2 from vertex fit **/
  void fitchi2(float FitChi2);
  /** set method: ndof from vertex fit **/
  void fitndof(int FitNdof);
  /** set method: x position of vertex  **/
  void fitx(float FitX);
  /** set method: y position of vertex  **/
  void fity(float FitY);
  /** set method: z position of vertex  **/
  void fitz(float FitZ);
  /** set method: add track to particle  **/
  void addTrack(const ElementLink<TrigInDetTrackCollection>& track);
  /** set method: link to secondary decay particle  **/
  void pSecondDecay(ElementLink<TrigL2BphysContainer> p_secondaryDecay);

 private:
  
  // private data members

  int   m_roiID;
  pType   m_particleType;
  float m_eta; 
  float m_phi; 
  float m_mass;
  float m_fitmass;
  float m_fitchi2;
  int m_fitndof;
  float m_fitx;
  float m_fity;
  float m_fitz;
  ElementLink<TrigL2BphysContainer> m_secondaryDecay; // secondary decay particle
  ElementLinkVector<TrigInDetTrackCollection> m_trackVector; // vector of EL to tracks
};

std::string str( const TrigL2Bphys& p );                      //<! printing helper
MsgStream& operator<< ( MsgStream& m, const TrigL2Bphys& p ); //<! printing helper (wraps above)
bool operator== ( const TrigL2Bphys& a, const TrigL2Bphys& b ); 
inline bool operator!= ( const TrigL2Bphys& a, const TrigL2Bphys& b ) { return !(a==b); }
/** @brief comparison with feedback
 * Function compares two objects and returns "semi verbose" output 
 * in the form of map where there are varaibel names and differences
 * between two obejcts
 * @param variableChange - map to record the differences
 * In case of collections (or objects when the size may be different) that information can also be returned in variableChange
 */
void diff( const TrigL2Bphys& a, const TrigL2Bphys& b, std::map<std::string, double>& variableChange ); 


CLASS_DEF( TrigL2Bphys , 118841261 , 1 )

#endif // TRIG_BPHYS
