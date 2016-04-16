// emacs: this is -*- c++ -*-
//
//   @file    TrigSuperRoi.h        
//            Container to contain multiple RoIs so that the 
//            RegionSelector can know whether to return a single list
//            for multiple RoIs etc       
//  
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: cTrigSuperRoi.h, v0.0   Thu  5 Dec 2013 16:07:30 CET sutt $

#if !defined(TRIGSTEERINGEVENT_TRIGSUPERROI_H)  && !defined(XAOD_STANDALONE)
#define TRIGSTEERINGEVENT_TRIGSUPERROI_H

#include <stdint.h>
#include <map>
#include <ostream>

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "CLIDSvc/CLASS_DEF.h"


/**
 * @brief Describes the Region of Ineterest geometry
 *  It has basically 9 parameters
 *
 * -# zed       : z position of RoI
 * -# zedMinus  : z positions at rear of RoI
 * -# zedPlus   : z positions at front of RoI
 * -# phi       : azimuthal angle (radians) of RoI in range from [-pi, pi]
 * -# etaMinus  : minimum azimuthal angle   
 * -# phiPlus   : maximum azimuthal angle  
 * -# eta       : pseudo-rapidity of RoI 
 * -# etaMinus  : pseudo-rapidity at zedMinus
 * -# etaPlus   : pseudo-rapidity at zedPlus 
 * \warning An attempt to cnstruct the objects of this class with phi0 out of allowed range results in throwing exception
 */

class TrigSuperRoi : public TrigRoiDescriptor {

public:

  /**
   * @brief default constructor
   */
  TrigSuperRoi(bool fullscan=true);
  
  /**
   * @brief constructor
   * @param eta      eta of RoI
   * @param etaMinus eta at rear  of RoI
   * @param etaPlus  eta at front of RoI
   * @param phi      phi of RoI
   * @param phiMinus minimum phi of RoI
   * @param phiPlus  maximum phi of RoI
   * @param zed      zed of RoI
   * @param zedMinus zed at rear  of RoI
   * @param zedPlus  zed at front of RoI
   */
  TrigSuperRoi(double eta, double etaMinus, double etaPlus, 
	       double phi, double phiMinus, double phiPlus, 
	       double zed, double zedMinus, double zedPlus);
  
  // Destructor
 virtual  ~TrigSuperRoi();
  
private:

  /// ??? is ths still needed?
  std::vector<uint32_t> m_serialized;

};


// std::string str( const TrigSuperRoi& d );                      //<! printing helper
// MsgStream& operator<< ( MsgStream& m, const TrigSuperRoi& d ); //<! printing helper (wraps above)

CLASS_DEF( TrigSuperRoi, 147993055, 1 )


#endif // TRIGSUPERROI_H
