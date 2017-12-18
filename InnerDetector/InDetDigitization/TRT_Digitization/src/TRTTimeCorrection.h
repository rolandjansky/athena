/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTTIMECORRECTION_H
#define TRTTIMECORRECTION_H

#include <vector>
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "Identifier/Identifier.h"

namespace InDetDD {
  class TRT_DetectorManager;
}

class TRTDigSettings;
class TRT_ID;

class ITRT_CalDbSvc;

#include "GeoPrimitives/GeoPrimitives.h"


/**
 * Time correction
 */
class TRTTimeCorrection {

public:

  TRTTimeCorrection(const std::string& name,
		    const TRTDigSettings* digset,
		    const InDetDD::TRT_DetectorManager* detmgr,
		    const TRT_ID*);

  ~TRTTimeCorrection();

  /**
   * Returns the time it would take to travel at light-speed from
   * (0,0,0) to the farthest end of the wire (where the electronics are).
   * @param strawID: straw ID
   * @return time correction
   */

  double TimeShift(const int& strawID);

  /**
   * Calculates the time between the signal reaching the wire and when it
   * reaches the electronics.
   * Result are two times: direct and reflected signal.
   * @param strawID: straw ID
   * @param meanZ:   z coordinate of signal in straw local coordinates
   *                 (z counted positive away from electronics)
   * @param propagationTime1: propagation time - direct signal
   * @param propagationTime2: propagation time - reflected signal
   */
  void PropagationTime(const int& strawID,
		       const double& meanZ,
		       double& propagationTime1,
		       double& propagationTime2);

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }


private:

  Identifier getIdentifier ( int hitID,
			     bool& statusok) ; //Note: Changed from const due to message service hick ups


  void Initialize();

  /** Time shift for barrel straws */
  double calculateTimeShift_Barrel( const unsigned int& iPhi,
				    const unsigned int& iRing,
				    const unsigned int& iLayer,
				    const unsigned int& iStraw,
				    const int strawID) ; //Note: Changed from const due to message service hick ups

  /** Time shift for end cap straws */
  double calculateTimeShift_EndCap( const unsigned int& iPhi,
				    const unsigned int& iWheel,
				    const unsigned int& iLayer,
				    const int strawID) ; //Note: Changed from const due to message service hick ups

  /** Time shift from straw endpoints in global system */
  double calculateTimeShiftFromStrawEnds( const Amg::Vector3D& strawend1_globalcoord,
				          const Amg::Vector3D& strawend2_globalcoord,
				          const int strawID) ;
  /**
   * Calculate the distance along the wire the signal travels before reaching
   * the electronics. Both the direct and reflected signal. The signal starts
   * in the middle of the straw.
   */
  void calculateSignalDists_Barrel(const unsigned int& iRing,
				   const unsigned int& iLayer,
				   double& direct_dist,
				   double& reflect_dist ) const;
  /**
   * Calculate the distance along the wire the signal travels before reaching
   * the electronics. Both the direct and reflected signal. The signal starts
   * in the middle of the straw.
   */
  void calculateSignalDists_EndCap(const unsigned int& iWheel,
				   double& direct_dist,
				   double& reflect_dist ) const;

  const TRTDigSettings* m_settings;
  const InDetDD::TRT_DetectorManager* m_detmgr;
 protected:
  const TRT_ID* m_trt_id;
 private:
  /** Cached timeshifts */
  std::vector< std::vector<  std::vector<double> > >
    m_timeShiftForEndCapPlanes;
  /** Cached timeshifts */
  std::vector< std::vector< std::vector< std::vector<double> > > >
    m_timeShiftForBarrelStraws;

  /** Cached distances */
  std::vector<double> m_directDistsForEndCapWheels;
  /** Cached distances */
  std::vector<double> m_reflectedDistsForEndCapWheels;
  /** Cached distances */
  std::vector< std::vector<double> > m_directDistsForBarrelLayers;
  /** Cached distances */
  std::vector< std::vector<double> > m_reflectedDistsForBarrelLayers;

  //TODO/FIXME: substitute these with use of hitidhelper:
  const unsigned int m_subdetectorMask;
  const unsigned int m_right5Bits;
  const unsigned int m_shift5Bits;
  const unsigned int m_shift10Bits;
  const unsigned int m_shift15Bits;

  /** Value used to denote an uninitialized value. */
  const double m_notInitVal;

  //Digitization parameters
  double m_signalPropagationSpeed;
  double m_lengthDeadRegion;
  double m_maxVertexDisplacement;
  bool m_timeShiftPhiSectSymmetry;
  bool m_getT0FromData;

  ServiceHandle< ITRT_CalDbSvc > m_trtcaldbsvc;

  mutable Athena::MsgStreamMember m_msg;

};

#endif
