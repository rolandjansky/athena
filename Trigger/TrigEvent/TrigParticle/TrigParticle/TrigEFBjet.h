/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigEFBjet.h
// PACKAGE:  Trigger/TrigEvent/TrigParticle
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIG_EF_BJET
#define TRIG_EF_BJET

#include <vector>
#include <iostream>
#include <string>

#include "FourMom/P4PtEtaPhiM.h"
#include "Navigation/Navigable.h"
#include "EventKernel/INavigable4Momentum.h"

#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"


/**
 * @brief Class representing a b-jet candidate created at EF.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 */


class TrigEFBjet: public P4PtEtaPhiM,  public NavigableTerminalNode ,
      virtual public INavigable4Momentum  {

 public:
  
  /** @brief Constructor. */
  TrigEFBjet();

  /** @brief Constructor used by TrigBjetFex algorithm. */
  TrigEFBjet(int, float, float, const Rec::TrackParticleContainer*, 
	     const VxContainer*, const VxContainer*, float, float,
	     float xcomb, float xIP1d, float xIP2d, float xIP3d, float xChi2,
	     float xSv, float xmvtx, float xevtx, float xnvtx);

  /** @brief Destructor. */
  ~TrigEFBjet();
  
  /** @brief To retrieve identifier of the RoI (unique to each LVL1 RoI). */
  int   roiId()  const;

  /** @brief To retrieve reconstructed z of primary vertex. */
  float prmVtx() const;

  /** @brief To validate TrigEFBjet object. */
  void  validate(bool v);
  /** @brief To know if TrigEFBjet object is valid. */
  bool  isValid()  const;

  /** @brief To retrieve pointer to track collection of type TrackParticle. */
  const Rec::TrackParticleContainer*  TrackCollection()     const;
  /** @brief To retrieve pointer to primary vertex collection of type VxVertex. */
  const VxContainer*                  PrmVertexCollection() const;
  /** @brief To retrieve pointer to secondary vertex collection of type VxVertex. */
  const VxContainer*                  SecVertexCollection() const;

  /** @brief To retrieve best likelihood weight (combination of IP3D and SV). */
  float xComb() const;
  /** @brief To retrieve likelihood weight based on longitudinal impact parameter. */
  float xIP1D() const;
  /** @brief To retrieve likelihood weight based on transverse impact parameter. */
  float xIP2D() const;
  /** @brief To retrieve likelihood weight based on 2D combination of impact parameters. */
  float xIP3D() const;
  /** @brief To retrieve probability weight based on chi2 of track fit. */
  float xCHI2() const;
  /** @brief To retrieve likelihood weight based on secondary vertex. */
  float xSV()   const;
  /** @brief To retrieve likelihood weight based on mass of secondary vertex. */
  float xMVtx() const;
  /** @brief To retrieve likelihood weight based on energy of secondary vertex. */
  float xEVtx() const;
  /** @brief To retrieve likelihood weight based on number of tracks linked to secondary vertex. */
  float xNVtx() const;

 private:

  /** @brief Boolean variable to declare the object as a valid one. */
  bool m_valid;

  /** @brief Identifier of the RoI. */
  int   m_roiID;

  /** @brief Pointer to the track collection. */
  const Rec::TrackParticleContainer*  m_track;
  /** @brief Pointer to the primary vertex collection. */
  const VxContainer*                  m_prmvertex;
  /** @brief Pointer to the secondary vertex collection. */
  const VxContainer*                  m_secvertex;

  /** @brief Z coordinate of the primary vertex. */
  float m_prmVtx;

  /** @brief Best likelihood weight (combination of IP3D and SV) - not filled yet. */
  float m_xcomb;
  /** @brief Likelihood weight based on transverse impact parameter. */
  float m_xIP1d;
  /** @brief Likelihood weight based on longitudinal impact parameter. */
  float m_xIP2d;
  /** @brief Likelihood weight based on 2D combination of impact parameters. */
  float m_xIP3d;
  /** @brief Probability weight based on chi2 of track fit. */
  float m_xChi2;
  /** @brief Likelihood weight based on secondary vertex - not filled yet. */
  float m_xSv;
  /** @brief Likelihood weight based on mass of secondary vertex - not filled yet. */
  float m_xmvtx;
  /** @brief Likelihood weight based on energy of secondary vertex - not filled yet. */
  float m_xevtx;
  /** @brief Likelihood weight based on number of tracks linked to secondary vertex - not filled yet. */
  float m_xnvtx;

};



std::string str(const TrigEFBjet& a);


MsgStream& operator<< (MsgStream& m, const TrigEFBjet& a);


bool operator== (const TrigEFBjet& a, const TrigEFBjet& b);


inline bool operator!= (const TrigEFBjet& a, const TrigEFBjet& b) {return !(a == b);}


void diff(const TrigEFBjet& a, const TrigEFBjet& b, std::map< std::string, double >& variableChange);


CLASS_DEF(TrigEFBjet, 86579774, 1)

#endif
