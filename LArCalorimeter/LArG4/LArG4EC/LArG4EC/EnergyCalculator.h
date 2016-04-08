/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EnergyCalculator.h
// Prepared 10-Apr-2002 Bill Seligman
// from code written by Jozsef Toth.
// 07-May-2003 AMS: now EnergyCalculator is not a singleton
// 02-July-2003 J.T. Charge collection added
// 20-July-2004 J.T. FieldMapVersion variable is added
// 25-May-2005 J.T. -calling sequence of GapAdj changed
//                  -new variables and function for reading and handling
//                    values of HV of power supplies
//                  - IonReco :for suppress signal because of ion recombination
//                  - DriftVelo: Walkowiak's formula for drift velocity
//  Sept-2006 J.T. - collect signal from the Barrette volume
//  Nov -2006 J.T. - fieldmap array structures changed,
//                 - lengths defined dinamically
//                 - field map for first/last fold and for Barrett volume are included
// May 2009 AMS move to namespace LArG4::EC
//              duplicated data members removed

#ifndef LArG4_EC_EnergyCalculator_H
#define LArG4_EC_EnergyCalculator_H

#include <string>
#include <stdexcept>

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVCalculator.h"

#include "GeoSpecialShapes/LArWheelCalculator.h"

#include "G4ThreeVector.hh"
#include "globals.hh"


class LArG4BirksLaw;
class MsgStream;


namespace LArG4 {
	class VCalibrationCalculator;

namespace EC {


class EnergyCalculator : public LArVCalculator
{
  public:
/////////////////////////////////////////////
// The interface for LArVCalculator.
	virtual G4float OOTcut() const { return m_OOTcut; }
	virtual void SetOutOfTimeCut(G4double c) { m_OOTcut = c; }

	virtual G4bool Process(const G4Step* a_step){return Process(a_step, m_hdata);}
        virtual G4bool Process(const G4Step*, std::vector<LArHitData>&);

	virtual G4bool FindIdentifier(const G4Step *, std::vector<LArHitData>&,
                                      G4ThreeVector &, G4ThreeVector &);

	virtual const LArG4Identifier& identifier(int i = 0) const {
		if(i != 0) throw std::range_error("Multiple hits not yet implemented");
                if(m_hdata.size()<1) throw std::range_error("No hit yet");
		return m_hdata[0].id;
	}

	virtual G4double time(int i = 0) const {
		if(i != 0) throw std::range_error("Multiple hits not yet implemented");
                if(m_hdata.size()<1) throw std::range_error("No hit yet");
		return m_hdata[0].time;
	}
	virtual G4double energy(int i = 0) const {
		if(i != 0) throw std::range_error("Multiple hits not yet implemented");
                if(m_hdata.size()<1) throw std::range_error("No hit yet");
		return m_hdata[0].energy;
	}
	virtual G4bool isInTime(int i = 0) const {
		if(i != 0) throw std::range_error("Multiple hits not yet implemented");
		return m_isInTime;
	}
	virtual G4bool isOutOfTime(int i = 0) const {
		if(i != 0) throw std::range_error("Multiple hits not yet implemented");
		return(!m_isInTime);
	}

  private:

  // The results of the Process calculation:
	//LArG4Identifier m_identifier;
	//G4double m_time;
	//G4double m_energy;
        std::vector<LArHitData> m_hdata;

	G4bool   m_isInTime;
	G4int    m_compartment;

	G4float  m_OOTcut;

  public:
	typedef enum {
		EMEC_ECOR_ROPT,
		EMEC_ECOR_OFF,
		EMEC_ECOR_GADJ,
		EMEC_ECOR_CHCL,
		EMEC_ECOR_GADJ_OLD,
		EMEC_ECOR_GADJ_E,
		EMEC_ECOR_GADJ_S,
		EMEC_ECOR_GADJ_SE,
		EMEC_ECOR_CHCL1
	} EnergyCorrection_t;

	EnergyCalculator(
		LArWheelCalculator::LArWheelCalculator_t,
		EnergyCorrection_t corr = EMEC_ECOR_ROPT,
		G4int zside = 1);

	virtual ~EnergyCalculator();

  private:

	G4bool (EnergyCalculator::*Process_type) (const G4Step*, std::vector<LArHitData>&);
	G4bool (EnergyCalculator::*FindIdentifier_type) (
                                                          const G4Step *,
                                                          std::vector<LArHitData>&,
                                                          G4ThreeVector &,
                                                          G4ThreeVector &);
	G4double (EnergyCalculator::*GetHV_Value_type) (const G4ThreeVector &p) const;
	G4double (EnergyCalculator::*GetGapSize_type) (const G4ThreeVector &p) const;
	G4double (EnergyCalculator::*distance_to_the_nearest_electrode_type) (const G4ThreeVector &p) const;

	G4bool Process_Default(const G4Step*, std::vector<LArHitData>&);
	G4bool Process_Barrett(const G4Step*, std::vector<LArHitData>&);
	G4bool FindIdentifier_Default(const G4Step *, std::vector<LArHitData>&, G4ThreeVector &, G4ThreeVector &);
	G4bool FindIdentifier_Barrett(const G4Step *, std::vector<LArHitData>&, G4ThreeVector &, G4ThreeVector &);
	G4bool FindDMIdentifier_Barrett(const G4Step* step, std::vector<LArHitData>&);
	G4bool GetCompartment_Barrett(G4ThreeVector,G4double,G4double,G4double,
				       G4int &, G4int &) const;
	G4double GetHV_Value_Default(const G4ThreeVector& p) const { return GetHV_Value(p);}
	G4double GetHV_Value_Barrett(const G4ThreeVector& p) const;
	G4double GetGapSize_Default(const G4ThreeVector &p) const { return GetGapSize(p);}  // need to make const
	G4double GetGapSize_Barrett(const G4ThreeVector &p) const; // need to make const
	G4int GetPhiGap_Barrett(const G4ThreeVector &p) const;
	G4double distance_to_the_nearest_electrode_Default(const G4ThreeVector &p) const
	  { return distance_to_the_nearest_electrode(p); }
	G4double distance_to_the_nearest_electrode_Barrett(const G4ThreeVector &p) const;

	static VCalibrationCalculator* m_supportCalculator;

	static void SetConst_OuterBarrett(void);
	static void SetConst_InnerBarrett(void);
	G4bool GetVolumeIndex(const G4Step *) const;
	static       G4bool   SetConstOuterBarrett;
	static       G4bool   SetConstInnerBarrett;
 	static const G4double LongBarThickness;//       =   20. *mm;
	static const G4double ColdCorrection;//         =1.0036256;
	static const G4double StripWidth;//             =3.*mm/ColdCorrection;
	static const G4double KapGap;//                 =1.*mm/ColdCorrection;
	static const G4double EdgeWidth;//              =1.*mm;
	static const G4double DistOfEndofCuFromBack;//  =22.77*mm/ColdCorrection;
	static const G4double DistOfStartofCuFromBack;//=31.*mm; // frontface of the barrette
	static const G4double ZmaxOfSignal;// DistOfStartofCuFromBack - DistOfEndofCuFromBack + EdgeWidth;
	static       G4double RefzDist; // = dElecFocaltoWRP+dWRPtoFrontFace+WheelThickness+  // used as const after initialization
	                                // +dWRPtoFrontFace+ LongBarThickness
	                                // -DistOfEndofCuFromBack
	static const G4double S3_Etalim[21];
	static const G4double Rmeas_outer[50];
	static const G4double Zmeas_outer[2];
	static       G4double S3_Rlim[21];
	static       G4double rlim[50];
	static       G4double zlim[4];
	static G4int ModuleNumber,PhiDivNumber;
	static G4double PhiStartOfPhiDiv;

// **************************************************************************

	EnergyCorrection_t m_correction_type;
	
	G4double (EnergyCalculator::*ecorr_method) (G4double, const G4ThreeVector&, const G4ThreeVector&); // need to make const
	G4double dummy_correction_method(G4double e, const G4ThreeVector&, const G4ThreeVector&) // need to make const
	{ return e; }
	G4double GapAdjustment_old(G4double, const G4ThreeVector&, const G4ThreeVector&); // need to make const
	G4double GapAdjustment    (G4double, const G4ThreeVector&, const G4ThreeVector&); // need to make const
	G4double GapAdjustment_E  (G4double, const G4ThreeVector&, const G4ThreeVector&); // need to make const
	G4double GapAdjustment_s  (G4double, const G4ThreeVector&, const G4ThreeVector&); // need to make const
	G4double GapAdjustment__sE (G4double, const G4ThreeVector&, const G4ThreeVector&); // need to make const
	G4double CalculateChargeCollection(G4double, const G4ThreeVector&, const G4ThreeVector&); // need to make const !!!
	G4double CalculateChargeCollection1(G4double, const G4ThreeVector&, const G4ThreeVector&); // need to make const !!!

	G4double m_GApower;

// **************************************************************************
//Declaration of variables,functions  for charge collection
//J.T
// **************************************************************************


//variables specific for wheel geometry
	G4int PhiGapNumber, PhiHalfGapNumber;
	G4double WaveLength, FanEleThicknessOld,FanEleFoldRadiusOld;
	G4int HalfWaveNumber, SignofZinHalfWave, SignofSlopeofHalfWave;
	G4double SinPhiGap, CosPhiGap, ZinHalfWave;
	G4double FanAbsThickness, FanEleThickness, HalfEleThickness;
	G4double ElectrodeFanHalfThickness;

//variables specific for Efield calculation

  static       G4bool   FieldMapsRead;
  static       G4String FieldMapVersion;

  static const G4double GridSize;
  static const G4double AverageGap;

  struct Fold_Efield_Map{
                  G4bool    FieldMapPrepared;
                  G4double*  FieldMap;           // [NumberOfRadialLayers][ZYWeight][MaxNofPoints];
                  G4double* MinZofLayer;        //these are limits of the
                  G4double* MaxZofLayer;        //area where  the FieldMap can
                  G4double* MinYofLayer;        //be used for interpolation
                  G4double* MaxYofLayer;
                  G4int*    NofColofLayer;      // a column is parallel to y
                  G4int*    NofRowofLayer;      // a row is  parallel to z
                  G4int*    NofPointsinLayer;
                  G4int*    pLayer;};

  struct Wheel_Efield_Map {G4bool          FieldMapPrepared;
                           G4int           NumberOfRadialLayer;
                           G4double*       RadiusOfLayers;
                           G4double*       FoldinAngleOfLayers;
                           G4double*       HalfLArGapSizeOfLayers;
                           Fold_Efield_Map* Fold;
                           Fold_Efield_Map Fold0;
                           Fold_Efield_Map Fold1;
                           G4double  GridShift;};

  static Wheel_Efield_Map ChCollInner,ChCollOuter;
  Wheel_Efield_Map* ChCollWheelType;
  Fold_Efield_Map*  ChCollFoldType;
  G4int PointFoldMapArea;

  void CreateArrays(Wheel_Efield_Map &, G4int);
  inline G4int Index(Fold_Efield_Map* foldmap, G4int i, G4int j, G4int k ) const
    {return foldmap->pLayer[i]+j*foldmap->NofPointsinLayer[i]+k;};
  void SetFoldArea(G4double);

  G4double Ylimits[4];

//HV for current calculation

  static       G4bool   HVMapRead;
  static       G4String HVMapVersion;
  static const G4double AverageHV;
  static const G4double AverageEfield;
  static const G4double AverageCurrent;
  static const G4String HVEMECMapFileName;  //{"HVEMECMap.dat"};

  static const G4int NofAtlasSide     = 2;
  static const G4int NofEtaSection    = 9;
  static const G4int NofElectrodeSide = 2;
  static const G4int NofElectrodesOut = 768;
  static const G4double HV_Etalim[NofEtaSection+1]; // = {1.375,1.5,1.6,1.8,2.,2.1,2.3,2.5,2.8,3.2};
  static G4int HV_Start_phi[NofAtlasSide][NofEtaSection][NofElectrodeSide];
  static G4double HV_Values[NofAtlasSide][NofEtaSection][NofElectrodeSide][NofElectrodesOut];

  static const G4double LArTemperature_ECC0;//={88.15}; //K
  static const G4double LArTemperature_ECC1;//={88.37};
  static const G4double LArTemperature_ECC5;//={87.97};
  static const G4double LArTemperature_av ;// ={88.16};

  G4int NofPhiSections;
  G4int NumberOfElectrodesInPhiSection;

  void GetHVMap(const G4String);
  G4double GetHV_Value(const G4ThreeVector& p) const;
  G4double GetHV_Value_ChColl_Wheel( const G4ThreeVector& , G4int , G4int) const;

//Efield in [kv/cm], driftvelo in [mm/microsec], Temperature in [K]

  inline static G4double IonReco(G4double Efield) {
    if(Efield<=0.000001){return 0.;}
    if(Efield>2.)       {return (1./(1.  +0.36/Efield));}
                         return (1./(1.04+0.28/Efield));
  }

  inline static G4double DriftVelo(G4double T, G4double Efield) {
    if( Efield <= 0.000001) {return 0.;}
               return ( (-0.01481*(T-90.371)+1.)*
                        ( 0.141*Efield*log(1.+12.4/Efield)+
                          1.627*pow(Efield,0.317) )
                         -0.0075*(T-90.371)
                      );
  }

// variables for phigap number comparison

  G4int calculatorPhiGap,chcollPhiGap;

// functions specific for geometry

  void     SetHalfWave(G4double);
  void     GetPhiGap(G4double *); // need to make const
  void     SetYlimitsofPhigapinWheel(G4double,G4double);
  G4double YofSurface(G4double,G4double,G4double) const;
  inline   G4double YofNeutralFibre(G4double alpha,G4double rho) const{
    return YofSurface(alpha,rho,0.);
  }
  G4double FoldingAngle(G4double) const;
  G4double HalfLArGapSize(G4double, G4double) const;

   //  functions specific for charge coll.

  void     IniGeomforFieldMaps(void);
  void     GetFieldMaps(const G4String);
  void     PrepareFieldMap(void);
  G4double GetCurrent(G4double *,G4double *,G4double);
  void     TransformWheeltoFieldMap(G4double *,G4double *);
  void     SetYlimitsofPhigapinFieldMap(G4int);
  void     TransFromBarrtoWheel(G4double*, G4double*);
  G4double GetWeightfromFieldMap(G4int,G4double,G4double);
  G4double HalfLArGapSizeOld(G4double) const;

  static       G4double CHC_Esr;
  static const G4int    CHCMaxPrint=00;
  static       G4int    CHCIprint;
  static       G4double CHCEbad;
  static       G4double CHCEtotal;
  static       G4double CHCStotal;

  private:
//  inline void vector_to_msg(G4ThreeVector &v) const;

  /* to be developed...
     std::pair<double, double>DxToFans(Hep3Vector &p);
     double XDistanceToTheNeutralFibre(const Hep3Vector& P) const;
  */
	G4double GetGapSize(const G4ThreeVector &p) const;  // need to make const

	//  public:
	G4double distance_to_the_nearest_electrode(const G4ThreeVector &p) const;

	LArG4BirksLaw *birksLaw;
	LArWheelCalculator *m_lwc;
	const LArWheelCalculator * lwc() const { return m_lwc; }

	MsgStream *m_msg;

	void get_HV_map_from_DB(void);
	G4bool m_DB_HV;

// Aug 2007 AMS, lost Aug 2008, restored May 2009
	LArWheelCalculator *m_electrode_calculator;
	const LArWheelCalculator * elc() const { return m_electrode_calculator; }
	
	G4double GetCurrent1(const G4ThreeVector &, const G4ThreeVector &, G4double); // need to make const
	G4double get_HV_value(const G4ThreeVector&, const std::pair<G4int, G4int> &) const;

	EnergyCalculator (const EnergyCalculator&);
	EnergyCalculator& operator= (const EnergyCalculator&);
};

} // namespace EC
} // namespace LArG4

#endif // LArG4_EC_EnergyCalculator_H
