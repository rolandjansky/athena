/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_H__
#define __LArWheelCalculator_H__

// This class separates some of the geometry details of the LAr
// endcap.
// 26-May-2009 AMS: remove all previous comments from here as obsoleted

#include <vector>

#include "CLHEP/Vector/ThreeVector.h"

#define USE_DISTANCECALCULATOR_IMPL 1
#define USE_FANCALCULATOR_IMPL 1

#define LARWC_SINCOS_POLY 5
#define LARWC_DTNF_NEW

class IRDBRecordset;
class RDBParamRecords;

// uncomment this to be able to select parameterized_slant_angle
// calculation method in runtime
//#define LARWHEELCALCULATOR_PSA_DEVELOPMENT

//#define HARDDEBUG

namespace LArWheelCalculator_Impl {
	class IDistanceCalculator;
	class DistanceCalculatorSaggingOff;
	class DistanceCalculatorSaggingOn;

	class IFanCalculator;
	class ModuleFanCalculator;
	template <typename SaggingType> class WheelFanCalculator;
	template <typename SaggingType> class DistanceToTheNeutralFibre_OfFan;
}

class LArWheelCalculator
{
	friend class LArWheelCalculator_Impl::DistanceCalculatorSaggingOff;
	friend class LArWheelCalculator_Impl::DistanceCalculatorSaggingOn;
	friend class LArWheelCalculator_Impl::ModuleFanCalculator;
	template <typename SaggingType> friend class LArWheelCalculator_Impl::WheelFanCalculator;
	template <typename SaggingType> friend class LArWheelCalculator_Impl::DistanceToTheNeutralFibre_OfFan;

  public:
	typedef enum {
		InnerAbsorberWheel,  OuterAbsorberWheel,
		InnerElectrodWheel,  OuterElectrodWheel,
		InnerAbsorberModule, OuterAbsorberModule,
		InnerElectrodModule, OuterElectrodModule,
		BackInnerBarretteWheel,       BackOuterBarretteWheel,
		BackInnerBarretteWheelCalib,  BackOuterBarretteWheelCalib,
		BackInnerBarretteModule,      BackOuterBarretteModule,
		BackInnerBarretteModuleCalib, BackOuterBarretteModuleCalib,
		InnerGlueWheel, OuterGlueWheel,
		InnerLeadWheel, OuterLeadWheel
	} LArWheelCalculator_t;

	LArWheelCalculator(LArWheelCalculator_t a_wheelType, int zside = 1);
	virtual ~LArWheelCalculator();

	static const char *LArWheelCalculatorTypeString(LArWheelCalculator_t);
	static double GetFanHalfThickness(LArWheelCalculator_t);

  // "Get constant" methods:
	double GetWheelThickness(void) const { return m_WheelThickness; }
	double GetdWRPtoFrontFace(void) const { return m_dWRPtoFrontFace; }
	double GetStraightStartSection(void) const { return m_StraightStartSection; }
	virtual LArWheelCalculator_t type() const { return m_type; }
  // "zShift" is the z-distance (cm) that the EM endcap is shifted
  // (due to cabling, etc.)
	double zShift() const { return m_zShift; }
	int GetNumberOfWaves(void) const { return m_NumberOfWaves; }
	int GetNumberOfHalfWaves(void) const { return m_NumberOfHalfWaves; }
	int GetNumberOfFans(void) const { return m_NumberOfFans; }
	double GetFanStepOnPhi(void) const { return m_FanStepOnPhi; }
	double GetHalfWaveLength(void) const { return m_HalfWaveLength; }
	double GetWheelRefPoint(void) const { return m_zWheelRefPoint; }
	double GetFanHalfThickness(void) const { return m_FanHalfThickness; }
	bool GetisModule(void) const { return m_isModule; }
	double GetWheelInnerRadius(double *) const;
	void GetWheelOuterRadius(double *) const;

  // "set constant" method:

	void SetStartGapNumber(int n) { m_ZeroGapNumber = n; }

  // geometry methods:

	double DistanceToTheNearestFan(CLHEP::Hep3Vector &p);
	double DistanceToTheNeutralFibre(const CLHEP::Hep3Vector &p) const;
	CLHEP::Hep3Vector NearestPointOnNeutralFibre(const CLHEP::Hep3Vector &p) const;
	int GetPhiGap(const CLHEP::Hep3Vector &p){ return GetPhiGapAndSide(p).first; }
	int PhiGapNumberForWheel(int) const;
	std::pair<int, int>GetPhiGapAndSide(const CLHEP::Hep3Vector &p);
	double AmplitudeOfSurface(const CLHEP::Hep3Vector& P, int side) const;

  protected:
	LArWheelCalculator_t m_type;

	int m_AtlasZside;
	bool m_SaggingOn; // !
	bool m_phiRotation;
	bool m_slant_use_default;
	double m_slant_parametrization[5]; // pol4
	double m_sin_parametrization[7]; // up to pol6
	double m_cos_parametrization[7];
	std::vector<std::vector<double> > m_sagging_parameter; // !
	double m_WheelThickness;
	double m_HalfWheelThickness;
	double m_ActiveLength;
	double m_StraightStartSection;
	double m_dWRPtoFrontFace;


	double m_zWheelFrontFace, m_zWheelBackFace;
	double m_HalfGapBetweenWheels;
	double m_zWheelRefPoint;
	double m_dMechFocaltoWRP;
	double m_dElecFocaltoWRP;
	double m_rOuterCutoff;
	double m_eta_hi, m_eta_mid, m_eta_low;

	double m_zShift;
	double m_QuarterWaveLength;
	double m_HalfWaveLength;
	double m_FanFoldRadius;
	double m_ZeroFanPhi;
	double m_ZeroFanPhi_ForDetNeaFan;
	double m_FanStepOnPhi;
	int m_NumberOfWaves;
	int m_NumberOfHalfWaves;
	int m_NumberOfFans;
	int m_HalfNumberOfFans;
	double m_FanHalfThickness;
	int m_ZeroGapNumber;
	int m_FirstFan;
	int m_LastFan;

	bool m_isModule;
	bool m_isElectrode;
	bool m_isInner;
	bool m_isBarrette;
	bool m_isBarretteCalib;

	int m_fan_number; // !

	void outer_wheel_init(const RDBParamRecords &);
	void inner_wheel_init(const RDBParamRecords &);
	void module_init(void);
#ifndef USE_DISTANCECALCULATOR_IMPL
  double get_sagging(const CLHEP::Hep3Vector &P) const;
#endif

#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
//optimized geom. param. for folding design
  public:
	inline static void SetOptFoldA(std::string optFoldA) {OptFoldA=optFoldA;}
	inline static std::string GetOptFoldA(void) {return OptFoldA;}

  private:
	static std::string OptFoldA; //"param" "table" "iter"
	static double *FoldAInner, *FoldAOuter;
	static bool FoldAInnerPrepared,FoldAOuterPrepared;
	static double RminAInner,RminAOuter,RmaxAInner,RmaxAOuter,RstepA;
	double * FoldA;
	double RminA,RmaxA;
	double AlfInt;
	double AlfExt;
	double ROpt[2];
	double ZlOpt[2];
	double AlphIter(double,double) const;
#endif

  protected:
	void set_m_fan_number(const int &fan_number)
	{
		m_fan_number = fan_number;
		if(m_fan_number < 0) m_fan_number += m_NumberOfFans;
		m_fan_number += m_ZeroGapNumber;
		if(m_fan_number >= m_NumberOfFans) m_fan_number -= m_NumberOfFans;
	}

  protected:
	double parameterized_slant_angle(double) const;
	void parameterized_sincos(const double, double &, double &) const;
	void parameterized_sin(const double, double &, double &) const;

  private:
  	LArWheelCalculator_Impl::IDistanceCalculator *m_distanceCalcImpl;
  	LArWheelCalculator_Impl::IFanCalculator *m_fanCalcImpl;

	void fill_sincos_parameterization(void);
};

#endif // __LArWheelCalculator_H__
