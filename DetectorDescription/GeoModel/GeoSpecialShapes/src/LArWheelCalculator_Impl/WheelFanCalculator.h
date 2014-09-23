/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_Impl_WheelFanCalculator_H__
#define __LArWheelCalculator_Impl_WheelFanCalculator_H__

// This is an interface of distance calculation to parts of the LAr endcap.

#include "IFanCalculator.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoSpecialShapes/LArWheelCalculator.h"

#include "GaudiKernel/PhysicalConstants.h"
using namespace Gaudi::Units;

#ifdef HARDDEBUG
#undef HARDDEBUG
#endif

namespace LArWheelCalculator_Impl {
  // mode marker classes
  class SaggingOn_t {};
  class SaggingOff_t {};


template <typename SaggingType>
class DistanceToTheNeutralFibre_OfFan {
};

template<>
class DistanceToTheNeutralFibre_OfFan<SaggingOn_t> {
  public:
	static inline double calculate(LArWheelCalculator* _lwc, int fan_number,     CLHEP::Hep3Vector &p) {
		_lwc->set_m_fan_number(fan_number);
		return _lwc->DistanceToTheNeutralFibre(p);
	}
};

template<>
class DistanceToTheNeutralFibre_OfFan<SaggingOff_t> {
  public:
	static inline double calculate(LArWheelCalculator* _lwc, int /*fan_number*/, CLHEP::Hep3Vector &p) {
		return _lwc->DistanceToTheNeutralFibre(p);
	}
};

enum FanSearchDirection_t {
  FORWARD = 1,    // delta =  1
  BACKWARD = -1   // delta = -1
};


template <typename SaggingType, FanSearchDirection_t dir >
 class StepFan {};


template <FanSearchDirection_t dir >
 class StepFan<SaggingOff_t, dir> {
   public:
  static inline void next(int &/*fan_number*/) {}
  static inline void adjust(int &/*fan_number*/) {}
};


template <>
 class StepFan<SaggingOn_t, FORWARD> {
   public:
  static inline void next(int &fan_number) {
	  fan_number++;
  }
  static inline void adjust(int &fan_number) {
	  fan_number--;
  }
};

template <>
 class StepFan<SaggingOn_t, BACKWARD> {
   public:
  static inline void next(int &fan_number) {
	  fan_number--;
  }
  static inline void adjust(int &/*fan_number*/) {  }
};

template <FanSearchDirection_t dir>
  class DoSearch {};

template <>
 class DoSearch<FORWARD> {
   public:
	 template <typename T >
	 static inline bool pred(T val) {
		return (val > 0.);
	 }
 };

template <>
 class DoSearch<BACKWARD> {
   public:
	 template <typename T >
	 static inline bool pred(T val) {
		return (val < 0.);
	 }
};

template <typename SaggingType, FanSearchDirection_t dir, class NFDistance >
 inline void rotate_to_nearest_fan(LArWheelCalculator* _lwc, int &fan_number, const double angle, CLHEP::Hep3Vector &p) {
		p.rotateZ(angle);
		StepFan<SaggingType, dir>::next(fan_number);
		//fan_number += delta;
		double d1 = NFDistance::calculate(_lwc, fan_number, p);

		//while(d0 * d1 > 0.) -> dir*d1 > 0 -> FORWARD: d1 > 0., BACKWARD: d1 < 0.

		while ( DoSearch<dir>::pred(d1) ) { // search:
			p.rotateZ(angle);
			StepFan<SaggingType, dir>::next(fan_number);
			//fan_number += delta;

			d1 = NFDistance::calculate(_lwc, fan_number, p);
			//lwc()->set_m_fan_number(fan_number);
			//d1 = lwc()->DistanceToTheNeutralFibre(p);

	#ifdef HARDDEBUG
		printf("DistanceToTheNearestFan: step FN %4d %4d\n", fan_number, _lwc->m_fan_number);
	#endif
		}
	// if signs of d1 and d0 are different, the point is between current pair
		StepFan<SaggingType, dir>::adjust(fan_number);
		//if(delta > 0) fan_number --;
}


  template <typename SaggingType>
  class WheelFanCalculator : public IFanCalculator {
    public:
	  WheelFanCalculator(LArWheelCalculator* lwc, IRDBAccessSvc* /*rdbAccess*/, const DecodeVersionKey & /*larVersionKey*/) : m_lwc(lwc)
	  {
	  }

  // geometry methods:
	  virtual double DistanceToTheNearestFan(CLHEP::Hep3Vector &p) const {
		int fan_number = int((p.phi() - halfpi - lwc()->m_ZeroFanPhi_ForDetNeaFan) / lwc()->m_FanStepOnPhi);
		const double angle = lwc()->m_FanStepOnPhi * fan_number + lwc()->m_ZeroFanPhi_ForDetNeaFan;
	#ifdef HARDDEBUG
		printf("DistanceToTheNearestFan: initial FN %4d\n", fan_number);
	#endif
		p.rotateZ(-angle);
	// determine search direction
		typedef DistanceToTheNeutralFibre_OfFan<SaggingType> NFDistance;

		const double d0 = NFDistance::calculate(lwc(), fan_number, p);
//		lwc()->set_m_fan_number(fan_number);
//		double d0 = lwc()->DistanceToTheNeutralFibre(p);

		const int delta = (d0 < 0.) ? -1 : 1;
		//int delta = 1;          // delta = signof(d0)
		//if(d0 < 0.) delta = -1; // search direction has been determined

		const double step_angle = - lwc()->m_FanStepOnPhi * delta;

		if (delta > 0) { // forward search
			rotate_to_nearest_fan< SaggingType, FORWARD, NFDistance >( lwc(), fan_number, step_angle, p);
		} else { // backward search
			rotate_to_nearest_fan< SaggingType, BACKWARD, NFDistance >( lwc(), fan_number, step_angle, p);
		}

		/*
		double d1 = d0;
		do { // search:
			p.rotateZ(angle);
			fan_number += delta;

			d1 = NFDistance::calculate(lwc(), fan_number, p);
			//lwc()->set_m_fan_number(fan_number);
			//d1 = lwc()->DistanceToTheNeutralFibre(p);

	#ifdef HARDDEBUG
		printf("DistanceToTheNearestFan: step FN %4d %4d\n", fan_number, lwc()->m_fan_number);
	#endif
		} while(d0 * d1 > 0.);
	// if signs of d1 and d0 are different, the point is between current pair
		if(delta > 0) fan_number --;
		*/

		p.rotateZ(-0.5 * step_angle);
	#ifdef HARDDEBUG
		printf("DistanceToTheNearestFan: final FN %4d\n", fan_number);
	#endif

		//return NFDistance::calculate(lwc(), fan_number, p);
		// ensure the correct value of LArWheelCalculator::m_fan_number for code that relies on side effects
		lwc()->set_m_fan_number(fan_number);
		return lwc()->DistanceToTheNeutralFibre(p);
	  }

	  virtual int PhiGapNumberForWheel(int i) const {
		return i;
	  }

	  virtual std::pair<int, int> GetPhiGapAndSide(const CLHEP::Hep3Vector &p) const {
		CLHEP::Hep3Vector p1 = p;

		int fan_number = int((p.phi() - halfpi - lwc()->m_ZeroFanPhi) / lwc()->m_FanStepOnPhi);
		const double angle = lwc()->m_FanStepOnPhi * fan_number + lwc()->m_ZeroFanPhi;
		p1.rotateZ(-angle);

		typedef DistanceToTheNeutralFibre_OfFan<SaggingType> NFDistance;

		const double d0 = NFDistance::calculate(lwc(), fan_number, p1);
//		lwc()->set_m_fan_number(fan_number);
//		double d0 = lwc()->DistanceToTheNeutralFibre(p1);

		double d1 = d0;

		const int delta = (d0 < 0.) ? -1 : 1;
		//int delta = 1;
		//if(d0 < 0.) delta = -1;
		const double step_angle = - lwc()->m_FanStepOnPhi * delta;
		do {
			p1.rotateZ(step_angle);
			fan_number += delta;
			d1 = NFDistance::calculate(lwc(), fan_number, p1);
			//lwc()->set_m_fan_number(fan_number);
			//d1 = lwc()->DistanceToTheNeutralFibre(p1);
		} while(d0 * d1 > 0.);
		if(delta > 0) fan_number --;
		if(!lwc()->m_isElectrode) fan_number ++;

		p1.rotateZ(-0.5 * step_angle);

		// ensure the correct value of LArWheelCalculator::m_fan_number for code that relies on side effects
		lwc()->set_m_fan_number(fan_number);
		double dd = lwc()->DistanceToTheNeutralFibre(p1);
		int side = dd < 0.? -1: 1;
	#ifdef HARDDEBUG
		printf("GetPhiGapAndSide: MFN %4d\n", lwc()->m_fan_number);
	#endif
		return std::pair<int, int>(lwc()->m_fan_number, side);
	  }

	  inline LArWheelCalculator *lwc() const { return m_lwc; };

	private:
	  LArWheelCalculator* m_lwc;

  };

}
#endif // __LArWheelCalculator_Impl_WheelFanCalculator_H__
