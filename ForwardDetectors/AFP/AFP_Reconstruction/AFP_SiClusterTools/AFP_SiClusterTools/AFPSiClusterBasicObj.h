/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_AFPSICLUSTERBASICOBJ_H
#define AFP_SICLUSTERTOOLS_AFPSICLUSTERBASICOBJ_H 1

/**
 * @file   AFPSiClusterBasicObj.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-22
 * 
 * @brief  Full definition of AFPSiClusterBasicObj
 */

#include <list>
#include "xAODForward/AFPSiHit.h"

/**
 *  @brief Class representing basic silicon pixels cluster, designed
 *  to be used for creating the clusters.
 * 
 *  Simple class contains only charge, column and row information. The
 *  main purpose of this class is to provide object that can be
 *  modified in the clustering algorithm.
 */
class AFPSiClusterBasicObj
{
public:
  /// Constructs object from the hit. Horizontal and vertical errors are set to \f$1/\sqrt{12} \approx 0.289\f$ i.e. standard deviation of uniform distribution.
  AFPSiClusterBasicObj (const xAOD::AFPSiHit* hit, const int clusterAlgID) :
    //    m_row(hit->pixelRowIDChip()), m_rowErr(0.289), m_column(hit->pixelColIDChip()), m_columnErr(0.289), m_charge(hit->depositedCharge()), m_clusterAlgID (clusterAlgID) {m_hits.push_back(hit);}
    m_horizID(hit->pixelHorizID()), m_horizIDErr(0.289), m_vertID(hit->pixelVertID()), m_vertIDErr(0.289), m_charge(hit->depositedCharge()), m_clusterAlgID (clusterAlgID) {m_hits.push_back(hit);}

  /// Constructs object with given horizontal and vertical positions (ID numbers) and charge. VertID and horizID errors are set to \f$1/\sqrt{12} \approx 0.289\f$ i.e. standard deviation of uniform distribution.
  AFPSiClusterBasicObj (const float horizID, const float vertID, const float charge, const int clusterAlgID) :
    m_horizID(horizID), m_horizIDErr(0.289), m_vertID(vertID), m_vertIDErr(0.289), m_charge(charge), m_clusterAlgID (clusterAlgID) {}

  AFPSiClusterBasicObj (const float horizID, const float horizIDErr, const float vertID, const float vertIDErr, const float charge, const int clusterAlgID) :
    m_horizID(horizID), m_horizIDErr(horizIDErr), m_vertID(vertID), m_vertIDErr(vertIDErr), m_charge(charge), m_clusterAlgID (clusterAlgID) {}

  /// @copydoc AFPSiClusterBasicObj::m_horizID
  float horizID() const {return m_horizID;}

  void setHorizID(const float horizID) {m_horizID = horizID;}

  /// @copydoc AFPSiClusterBasicObj::m_horizIDErr
  float horizIDErr() const {return m_horizIDErr;}

  void setHorizIDErr(const float horizIDErr) {m_horizIDErr = horizIDErr;}

  /// @copydoc AFPSiClusterBasicObj::m_vertID
  float vertID() const {return m_vertID;}

  void setVertID(const float vertID) {m_vertID = vertID;}

  /// @copydoc AFPSiClusterBasicObj::m_vertIDErr
  float vertIDErr() const {return m_vertIDErr;}
  
  void setVertIDErr(const float vertIDErr) {m_vertIDErr = vertIDErr;}  
  
  /// @copydoc AFPSiClusterBasicObj::m_charge
  double charge() const {return m_charge;}

  void setCharge(const double charge) {m_charge = charge;}

  /// @copydoc AFPSiClusterBasicObj::m_clusterAlgID
  int clusterAlgID () const {return m_clusterAlgID;}

  void setClusterAlgID (const int clusterAlgID) {m_clusterAlgID = clusterAlgID;}

  /// @copydoc AFPSiClusterBasicObj::m_hits
  std::list<const xAOD::AFPSiHit*>& hits() {return m_hits;}

  /// @copydoc AFPSiClusterBasicObj::m_hits
  const std::list<const xAOD::AFPSiHit*>& hits() const {return m_hits;}

protected:
  float m_horizID;			///< horizontal position (ID number) of the cluster (float because it can be weighted mean of hits)
  float m_horizIDErr;			///< uncertainty of horizontal position (ID number) in fraction
  float m_vertID;			///< vertical position (ID number) of the cluster (float because it can be weighted mean of hits)
  float m_vertIDErr;			///< uncertainty of vertical position (ID number) in fraction
  double m_charge;			///< charge of the cluster
  int m_clusterAlgID;			///< clustering algorithm ID number (use xAOD::AFPClusterAlgID for labels)


  std::list<const xAOD::AFPSiHit*> m_hits; ///< list of pixels used to form the cluster
};


#endif	// AFP_SICLUSTERTOOLS_AFPSICLUSTERBASICOBJ_H
