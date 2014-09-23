/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ModuleFanCalculator.h"

#include "GeoSpecialShapes/LArWheelCalculator.h"

#include "GaudiKernel/PhysicalConstants.h"
using namespace Gaudi::Units;

#ifdef HARDDEBUG
#undef HARDDEBUG
#endif

namespace LArWheelCalculator_Impl {
    ModuleFanCalculator::ModuleFanCalculator(LArWheelCalculator* lwc, IRDBAccessSvc* /*rdbAccess*/, const DecodeVersionKey & /*larVersionKey*/) :
    		m_lwc(lwc)
    {
    }

	double ModuleFanCalculator::DistanceToTheNearestFan(CLHEP::Hep3Vector &p) const {
		int fan_number = int((p.phi() - halfpi - lwc()->m_ZeroFanPhi_ForDetNeaFan) / lwc()->m_FanStepOnPhi);
		double angle = lwc()->m_FanStepOnPhi * fan_number + lwc()->m_ZeroFanPhi_ForDetNeaFan;
	#ifdef HARDDEBUG
		printf("DistanceToTheNearestFan: initial FN %4d\n", fan_number);
	#endif
		p.rotateZ(-angle);
	// determine search direction
		lwc()->set_m_fan_number(fan_number);
		double d0 = lwc()->DistanceToTheNeutralFibre(p);
		double d1 = d0;
		int delta = 1;
		if(d0 < 0.) delta = -1; // search direction has been determined
		angle = - lwc()->m_FanStepOnPhi * delta;
		do { // search:
			p.rotateZ(angle);
			fan_number += delta;
			lwc()->set_m_fan_number(fan_number);
	#ifdef HARDDEBUG
		printf("DistanceToTheNearestFan: step FN %4d %4d\n", fan_number, lwc()->m_fan_number);
	#endif
			d1 = lwc()->DistanceToTheNeutralFibre(p);
		} while(d0 * d1 > 0.);
	// if signs of d1 and d0 are different, the point is between current pair
		if(delta > 0) fan_number --;

		lwc()->m_fan_number = fan_number;
		if(lwc()->m_fan_number < lwc()->m_FirstFan) {
			p.rotateZ((lwc()->m_fan_number - lwc()->m_FirstFan) * lwc()->m_FanStepOnPhi);
			lwc()->m_fan_number = lwc()->m_FirstFan;
		} else if(lwc()->m_fan_number >= lwc()->m_LastFan) {
			p.rotateZ((lwc()->m_fan_number - lwc()->m_LastFan + 1) * lwc()->m_FanStepOnPhi);
			lwc()->m_fan_number = lwc()->m_LastFan - 1;
		}

		p.rotateZ(-0.5 * angle);
	#ifdef HARDDEBUG
		printf("DistanceToTheNearestFan: final MFN %4d\n", lwc()->m_fan_number);
	#endif
		return lwc()->DistanceToTheNeutralFibre(p);
	}

	int ModuleFanCalculator::PhiGapNumberForWheel(int i) const {
		i += lwc()->m_ZeroGapNumber;
		i -= lwc()->m_LastFan / 2;
		if(i < 0) i += lwc()->m_NumberOfFans;
		if(i >= lwc()->m_NumberOfFans) i -= lwc()->m_NumberOfFans;
		return i;
	}

	std::pair<int, int> ModuleFanCalculator::GetPhiGapAndSide(const CLHEP::Hep3Vector &p) const {
		static CLHEP::Hep3Vector p1;
		p1 = p;

		int fan_number = int((p.phi() - halfpi - lwc()->m_ZeroFanPhi) / lwc()->m_FanStepOnPhi);

		double angle = lwc()->m_FanStepOnPhi * fan_number + lwc()->m_ZeroFanPhi;
		p1.rotateZ(-angle);

		lwc()->set_m_fan_number(fan_number);
		double d0 = lwc()->DistanceToTheNeutralFibre(p1);
		double d1 = d0;

		int delta = 1;
		if(d0 < 0.) delta = -1;
		angle = - lwc()->m_FanStepOnPhi * delta;
		do {
			p1.rotateZ(angle);
			fan_number += delta;
			lwc()->set_m_fan_number(fan_number);
			d1 = lwc()->DistanceToTheNeutralFibre(p1);
		} while(d0 * d1 > 0.);
		if(delta > 0) fan_number --;
		if(!lwc()->m_isElectrode) fan_number ++;

		lwc()->m_fan_number = fan_number;
		if(lwc()->m_fan_number < lwc()->m_FirstFan) lwc()->m_fan_number = lwc()->m_FirstFan - 1;
		else if(lwc()->m_fan_number > lwc()->m_LastFan) lwc()->m_fan_number = lwc()->m_LastFan;

		p1.rotateZ(-0.5 * angle);
		double dd = lwc()->DistanceToTheNeutralFibre(p1);
		int side = dd < 0.? -1: 1;
	#ifdef HARDDEBUG
		printf("GetPhiGapAndSide: MFN %4d\n", lwc()->m_fan_number);
	#endif
		return std::pair<int, int>(lwc()->m_fan_number, side);
	}
}
