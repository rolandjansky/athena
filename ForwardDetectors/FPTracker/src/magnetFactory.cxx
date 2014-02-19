/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/magnetFactory.h"
#include "FPTracker/Magnet.h"
#include "FPTracker/IBender.h"
#include "FPTracker/DipoleBender.h"
#include "FPTracker/NullBender.h"
#include "FPTracker/QuadrupoleBender.h"
#include "FPTracker/QuadFocuserVertical.h"
#include "FPTracker/QuadFocuserHorizontal.h"
#include "FPTracker/QuadFocuserNull.h"
#include "FPTracker/FPTrackerConstants.h"
#include <string>
#include <stdexcept>

namespace FPTracker{

  std::string typeToString(Magnet::Type type)
  {
    if ( type == Magnet::hbDipole )    { return "hbDipole"; }
    if ( type == Magnet::vbDipole )    { return "vbDipole"; }
    if ( type == Magnet::hfQuadrupole ){ return "hfQuad";   }
    if ( type == Magnet::vfQuadrupole) { return "vfQuad";   }
    throw std::logic_error("unknown magnet type");
  } 

  IQuadFocuser::Ptr_t quadFocuserFactory( Magnet::Type type)
  {
    
    if ( type == Magnet::hfQuadrupole ){return IQuadFocuser::Ptr_t(new QuadFocuserHorizontal);}
    if ( type == Magnet::vfQuadrupole ){return IQuadFocuser::Ptr_t(new QuadFocuserVertical);}
      
    throw std::logic_error("bad magnet type for QuadFocuser");
  }
  

  IBender::ConstPtr_t benderFactory( Magnet::Type type,
				     double length,
				     double strength,
				     double pbeam0,
				     int side
				     )
  {
    if( type == Magnet::hbDipole )
      {
	return IBender::ConstPtr_t(new DipoleBender(hBendplane, 
						    length, 
						    strength, 
						    pbeam0,
						    side)
				   );
      }
    
    if ( type == Magnet::vbDipole )
      {
	return IBender::ConstPtr_t(new DipoleBender(vBendplane,
						    length,
						    strength,
						    pbeam0,
						    side)
				   );
      }
    
    if ( type == Magnet::hfQuadrupole )
      {
	return IBender::ConstPtr_t( new QuadrupoleBender(quadFocuserFactory(type),
							 length, 
							 strength,
							 side)
				    );
      }
    
    if ( type == Magnet::vfQuadrupole )
      {
	return IBender::ConstPtr_t( new QuadrupoleBender(quadFocuserFactory(type),
							 length,
							 strength,
							 side)
				    );
      }
    
    throw std::logic_error("unknown magnet type");
  }
  
  Magnet::ConstPtr_t magnetFactory(
				   double x,
				   double y,
				   double center,
				   double strength,
				   double length,
				   int apertype,
				   double A1,
				   double A2,
				   double A3,
				   double A4,
				   double X,
				   double pbeam0,
				   Side side,
				   Magnet::Type type){
    
    IBender::ConstPtr_t bender( benderFactory(type, length, strength, pbeam0, side) );
    std::string label( typeToString(type) );
    
    return Magnet::ConstPtr_t(new Magnet( x,
					  y, 
					  center, 
					  strength, 
					  length, 
					  apertype, 
					  A1, 
					  A2, 
					  A3, 
					  A4, 
					  X, 
					  side, 
					  bender, 
					  label
					  )
			      );
    
  }
  
}
