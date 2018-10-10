/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetElementKeyBase.h  -  description
                             -------------------
    begin                : Monday August 20 2001
    email                : e.moyse@qmw.ac.uk
     
  Converted to base class JetElementKeyBase by Alan Watson, 20/01/06
***************************************************************************/


#ifndef JetElementKeyBase_H
#define JetElementKeyBase_H

#include <iostream>
#include <stdlib.h>
#include <vector>

#ifndef  TRIGGERSPACE
// running in Athena
#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/JetElement.h"
#include "TrigT1CaloUtils/KeyUtilities.h"
#include "TrigT1CaloUtils/BinAndCoord.h"
#include "TrigT1Interfaces/Coordinate.h"

#else
// running in TRIGGERSPACE
#include "KeyUtilities.h"
#include "BinAndCoord.h"
#include "Coordinate.h"

#endif



namespace LVL1 {

/**
  The JetElementKeyBase object provides the key for each JetElement or JetInput
  depending on its eta,phi coords. <p>
  The key is an integer number that uniquely
  identifies each tower, and is needed by the STL map container: the map holds
  trigger towers and we retrieve them using their keys.
  */
class JetElementKeyBase : public KeyUtilities {
 
public:
  enum JERegion {
    Barrel=0,
    EndBarrel=1,
    EndCap1=2,
    EndCap2=3,
    EndCap3=4,
    FCAL=5
  };
  virtual ~JetElementKeyBase();
#ifndef  TRIGGERSPACE
// only applicable if running in Athena
  /** returns key of passed tower*/
  virtual unsigned int jeKey( const xAOD::TriggerTower& tower) ;
  /** returns key of passed jetelement*/
  virtual unsigned int jeKey( const xAOD::JetElement& jetElement) ;
#endif
  /** returns the key of the passed coords */
  virtual unsigned int jeKey( const double phi, const double eta);
  /** returns the key of the passed Coordinate */
  virtual unsigned int jeKey( const Coordinate coord);
  /**returns key of JE in +ve eta dir.*/
  unsigned int leftEtaKey(const double phi, const double eta);
  /**returns key of JE in -ve eta dir.*/
  unsigned int rightEtaKey(const double phi, const double eta);
  /**returns key of next JE in +ve phi dir.*/
  unsigned int upPhiKey(const double phi, const double eta);
  /**returns key of next JE in -ve phi dir.*/
  unsigned int downPhiKey(const double phi, const double eta);
  
  /** returns the central coordinate of the JE which contains the passed coord*/
  Coordinate getCentre(const double phi, const double eta);
  Coordinate getCentre(const Coordinate coord);

  /** Functions for finding coordinates of neighbouring JEs */
  Coordinate rightEta(const double phi,const double eta);
  Coordinate leftEta(const double phi,const double eta);
  Coordinate downPhi(const double phi,const double eta);
  Coordinate upPhi(const double phi,const double eta);
  Coordinate rightEta(const Coordinate coord);
  Coordinate leftEta(const Coordinate coord);
  Coordinate downPhi(const Coordinate coord);
  Coordinate upPhi(const Coordinate coord);
  Coordinate upperLeft(const double phi,const double eta);
  Coordinate upperRight(const double phi,const double eta);
  Coordinate lowerLeft(const double phi,const double eta);
  Coordinate lowerRight(const double phi,const double eta);
  Coordinate upperLeft(const Coordinate coord);
  Coordinate upperRight(const Coordinate coord);
  Coordinate lowerLeft(const Coordinate coord);
  Coordinate lowerRight(const Coordinate coord);
  /**return height of JE*/
  double dPhi(const double phi, const double eta) const ;
  /**return height of JE*/
  double dPhi() const;
  /**return height of JE*/
  double dPhi(const Coordinate& coord) const;
  /**return width of JE*/
  double dEta(const double phi, const double eta) const ;
  /**return width of JE*/
  double dEta() const ;
  /**return width of JE*/
  double dEta(const Coordinate& coord) const;
  /** returns region number at eta, where region is:*/
  unsigned int region(double eta) const;
  /*
  Region    Eta
  Barrel    <2.2
  EndBarrel 2.2 - 2.4
  EndCap1   2.4 - 2.7
  EndCap2   2.7 - 2.9
  EndCap3   2.9 - 3.2
  FCAL      3.2 - 4.9 */
  JERegion jeRegion(const Coordinate& coord) const;
  /** returns TRUE if this coordinate is in the FCAL*/
  bool isFCAL(double eta) const;
  
protected: // Methods we don't want accessed except through inheritance
  
  // User should construct a JetElementKey or JetInputKey, not this base class
  JetElementKeyBase(double phi, double eta);
  JetElementKeyBase(const Coordinate coord);
  JetElementKeyBase();

  void setupRegionData();
  void setupJetRegionData();
  
private: // Internal methods of the base class
  /** converts integer phi, eta
  coordinates to phi, eta trigger bins. */
  BinAndCoord* calculateTriggerBin(ICoordinate* iCoord);

  /** this is a bit over the top, but it looks to see
  if the calculated coordinate matches a coordinate centre
  that JetElementKeyBase would return. It doesn't return any errors or
  anything as it's purely being used in tests external to Athena */
  void analCoordChecking(const Coordinate& coord);
  
private:
//	double m_phi;
//	double m_eta;
  /**set to true to turn debugging info on*/
  bool m_debug;
  /**holds central coords of JEs in regions:
  Region 	Eta
  0   	<2.2
  1   	2.2-2.4
  2   	2.4-2.7
  3			2.7-2.9
  4			2.9-3.2
  5			3.2-4.9 */
  std::vector<double> m_regionEtaCentre;
  /**holds width of JEs in regions:*/
  std::vector<double> m_regionWidth;
  /**holds height of JEs in regions:*/
  std::vector<double> m_regionHeight;
};
}
#endif
