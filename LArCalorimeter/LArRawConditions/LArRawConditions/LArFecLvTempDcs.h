/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARFECLVTEMPDCS_H
#define LARFECLVTEMPDCS_H

/*******************************************************

NAME:           LArFecLvTempDcs.h
PACKAGE:        LArCalorimeter/LArRawConditions
AUTHORS:        Denis O. Damazio
 
PURPOSE:        Keep the important output variables
		from the DCS. Check also the tool
		"Name of the Tool"
KNOWTOINHERIT:  
DATE:           June 14th, 2006
 
******************************************************/

#include <string>
#include <vector>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

class LArFecLvTempDcsOcem {
   public:
        LArFecLvTempDcsOcem();
	~LArFecLvTempDcsOcem(){};
	void print(MsgStream& log) const;
	// Set methods
	// OCEM Power On (operator->PS)
	inline void powerOn ( const bool& p )
		{ m_powerOn = p; };
	// OCEM Power Output On (operator->PS)
	inline void powerOut ( const bool& p )
		{ m_powerOut = p; };
	// Interlock from DSS
	inline void interlock ( const bool& p )
		{ m_interlock = p; };
	// OverCurrent in PS
	inline void overCurrent ( const bool& p )
		{ m_overCurrent = p; };
	// OverTemperature in PS
	inline void overTemperature ( const bool& p )
		{ m_overTemperature = p; };
	// Local (Manual - 0 ) or remote (PVSS - 1) control of PS
	inline void remote ( const bool& p )
		{ m_remote = p; };
	// PS Voltage Output measure
	inline void voltage ( const float& p )
		{ m_voltage = p; };
	// PS Current Output measure
	inline void current ( const float& p )
		{ m_current = p; };
	// OCEM Power On (PS->operator)
	inline void on ( const bool& p )
		{ m_on = p; };
	// OCEM Power Out (PS->operator)
	inline void out ( const bool& p )
		{ m_out = p; };
	// OCEM Can Node State
	inline void state ( const bool& p )
		{ m_state = p; };

	// Get Methods
	// OCEM Power On (operator->PS)
	inline bool powerOn(void) const
		{ return m_powerOn; };
	// OCEM Power Out (operator->PS)
	inline bool powerOut(void) const
		{ return m_powerOut; };
	// Interlock from DSS
	inline bool interlock(void) const
		{ return m_interlock; };
	// OverCurrent in PS
	inline bool overCurrent(void) const
		{ return m_overCurrent; };
	// OverTemperature in PS
	inline bool overTemperature(void) const
		{ return m_overTemperature; };
	// Local (Manual - 0 ) or remote (PVSS - 1) control of PS
	inline bool remote(void) const
		{ return m_remote; };
	// PS Voltage Output measure
	inline float voltage(void) const
		{ return m_voltage; };
	// PS Current Output measure
	inline float current(void) const
		{ return m_current; };
	// OCEM Power On (PS->operator)
	inline bool on(void) const
		{ return m_on; };
	// OCEM Power Out (PS->operator)
	inline bool out(void) const
		{ return m_out; };
	// OCEM Can Node State
	inline int state(void) const
		{ return m_state; };
	
   private:
	bool  m_powerOn;		// OCEM Power On (operator->PS)
	bool  m_powerOut;		// OCEM Power Output On (operator->PS)
	bool  m_interlock;		// Interlock from DSS
	bool  m_overCurrent;		// OverCurrent in PS
	bool  m_overTemperature;	// OverTemperature in PS
	bool  m_remote;			// Local (Manual - 0 ) or remote
					// (PVSS - 1) control of PS
	float m_voltage;		// PS Voltage Output measure
	float m_current;		// PS Current Output measure
	bool  m_on; 			// OCEM Power On (PS->operator)
	bool  m_out; 			// OCEM Power On (PS->operator)
	int   m_state; 			// OCEM Can Node State
};

class LArFecLvTempDcsManifold {
   public:
	LArFecLvTempDcsManifold(){};
	~LArFecLvTempDcsManifold(){};
	void print( MsgStream& log ) const;
	// Set Methods
	// Four Manifold Temperatures in a vector
	inline void temperature( const std::vector<float> p ){
		m_temps.clear();
		for(std::vector<float>::const_iterator i=p.begin();
			i!=p.end(); ++i) m_temps.push_back(*i);
	}
	// Get Methods
	// Four Manifold Temperatures in a vector
	inline void getTemperature(std::vector<float>& p ) const {
		for(std::vector<float>::const_iterator i=m_temps.begin();
			i!=m_temps.end(); ++i) p.push_back(*i);
	};
   private:
	std::vector<float> m_temps;		// Four Manifold Temperatures
						// in a vector
};

class LArFecLvTempDcsVolts {
   public:
	LArFecLvTempDcsVolts(){};
	~LArFecLvTempDcsVolts(){};
	void print( MsgStream& log ) const;
	// Set Methods
	// Seven Crate Voltages in a vector
	inline void voltage( const std::vector<float> p ){
		m_volts.clear();
		for(std::vector<float>::const_iterator i=p.begin();
			i!=p.end(); ++i) m_volts.push_back(*i);
	}
	// Get Methods
	// Seven Crate Voltages in a vector
	inline void getVoltage(std::vector<float>& p ) const {
		for(std::vector<float>::const_iterator i=m_volts.begin();
			i!=m_volts.end(); ++i) p.push_back(*i);
	};
   private:
	std::vector<float> m_volts;	// Seven Voltages in a vector
};

class LArFecLvTempDcsLvps {
   public:
	LArFecLvTempDcsLvps();
	~LArFecLvTempDcsLvps(){};
	void print( MsgStream& log ) const;
	// Set methods
	// All Voltages Good LVPS
	inline void allVoltGood( const bool& p )
		{ m_AVG = p; };
	// All Modules Good LVPS
	inline void allModGood( const bool& p )
		{ m_AMG = p; };
	// OverTemperature flags LVPS
	inline void overTemperature( const bool& p )
		{ m_PSOT = p; };
	// OverTemperatureOverRide flags LVPS
	inline void overTemperatureOverRide( const bool& p )
		{ m_PSOTOR = p; };
	// Temperature LVPS
	inline void temperature( const float& p )
		{ m_temperature = p; };

	// Get Method
	// All Voltages Good LVPS
	inline bool allVoltGood( void ) const
		{ return m_AVG; };
	// All Modules Good LVPS
	inline bool allModGood( void ) const
		{ return m_AMG; };
	// OverTemperature flags LVPS
	inline bool overTemperature( void ) const
		{ return m_PSOT; };
	// OverTemperatureOverRide flags LVPS
	inline bool overTemperatureOverRide( void ) const
		{ return m_PSOTOR; };
	// Temperature LVPS
	inline float temperature( void ) const
		{ return m_temperature; };
	
   private:
	bool  m_AVG;			// All Voltages Good LVPS
	bool  m_AMG;			// All Modules Good LVPS
	bool  m_PSOT;			// OverTemperature flags LVPS
	bool  m_PSOTOR;			// OverTemperatureOverRide LVPS
	float m_temperature;		// Temperature LVPS
};

class LArFecLvTempDcs {
     // LAr Front-End Crate Low-Voltage and Temperature
     // from DCS. We have also HV from DCS and Febs
     // from TDAQ.
     // Hardware specific elements to be returned as references
     // One should not request the reference itself, but header
     // use it directly, like : 
     // LArFecLvTempDCS m_DCS;
     // m_DCS->getOCEM().voltage();
     // Constructor (to be defined in cxx file)
   public:
     LArFecLvTempDcs();
     ~LArFecLvTempDcs(){};
     void print( MsgStream& log ) const;
     // OCEM (280V Power Supply)
     LArFecLvTempDcsOcem& getOCEM()
		{ return m_OCEM;};
     // Manifold Temperatures
     LArFecLvTempDcsManifold& getManifold()
		{ return m_manifold;};
     // Low-Voltage Power Supply (MDI)
     LArFecLvTempDcsLvps& getLVPS()
		{ return m_lvps;};
     // CrateVoltages
     LArFecLvTempDcsVolts& getVolts()
		{ return m_volts;};

     // Some simple objects methods
     // ELMB Status
     inline bool ELMBStat() const
	{ return m_ELMBStat; };
     inline void ELMBStat(const bool& p)
	{ m_ELMBStat = p; };
     inline float febMin() const
	{ return m_febMin; };
     inline void febMin(const float& p)
	{ m_febMin = p; };
     inline float febMax() const
	{ return m_febMax; };
     inline void febMax(const float& p)
	{ m_febMax = p; };
     // crateName
     inline std::string crateName() const
	{ return m_crateName; };
     inline void crateName(const std::string cratename)
	{ m_crateName.clear(); m_crateName.assign( cratename ); };
   protected:
     LArFecLvTempDcsOcem m_OCEM;
     LArFecLvTempDcsManifold m_manifold;
     LArFecLvTempDcsVolts m_volts;
     LArFecLvTempDcsLvps m_lvps;
     std::string m_crateName; // crateName
     bool m_ELMBStat; // ELMBStat
     float m_febMin; // Minimal Feb Temperature in crate
     float m_febMax; // Maximal Feb Temperature in crate
};

#endif
