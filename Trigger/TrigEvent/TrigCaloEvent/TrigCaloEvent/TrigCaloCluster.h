/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOCLUSTER_H
#define TRIGCALOCLUSTER_H


/*******************************************************

NAME:		TrigCaloCluster.h
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHORS:	Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:	Keep the important output variables
		from the Calorimeter LVL2 Trigger.
		Optionally seed ID Trigger and EF.
		Variables here should be RAW variables
		not corrected.
KNOWTOINHERIT:	TrigEmCluster, TrigTauCluster,
		TrigJetCluster
DATE:		October 17th, 2005

******************************************************/

// for printing out
#include <iostream>
#include "GaudiKernel/MsgStream.h"
// t have quantities per sample
#include "CaloGeoHelpers/CaloSampling.h"
// CLID for persistency
#include "AthenaKernel/CLASS_DEF.h"

const int MAXSIZE = CaloSampling::CaloSample::Unknown;

/**  Base Cluster Class. Raw Variables (no calibration).   */
class TrigCaloCluster {

public:
	/**  Constructor, please note the default values    */
	TrigCaloCluster (float energy = 0.0, float eta = -10.0,
		float phi = -10.0, long roi_word = 0);
	/**  Destructor   */
	~TrigCaloCluster ();
	/**  Copy Pointer Constructor   */
	TrigCaloCluster ( const TrigCaloCluster* trigcalocluster );
	/**  Copy Reference Constructor   */
	TrigCaloCluster ( const TrigCaloCluster& trigcalocluster );

	/**  Prints out cluster variables to std::cout   */
	void print ( void ) const;
	/**  Prints out cluster variables to MsgStream   */
	void print ( MsgStream& log) const;

	// set methods
	/**  set Raw Energy (no calibration)   */
	inline void setRawEnergy( float energy  )
			{ m_rawEnergy = energy;      };
	/**  set Raw Et (no calibration)   */
	inline void setRawEt( float et          )
			{ m_rawEt = et;              };
	/**  set Raw Et (no calibration) per sampling layer  */
	inline void setRawEnergy( CaloSampling::CaloSample sample,
			float energy )
			{ m_rawEnergyS[sample] = energy; };
	/**  set Raw Eta (no calibration)  */
	inline void setRawEta   ( float eta     )
			{ m_rawEta    = eta;         };
	/**  set Raw Phi (no calibration)  */
	inline void setRawPhi   ( float phi     )
			{ m_rawPhi    = phi;         };
	/**  set RoI Word  */
	inline void setRoIword  ( long roiword   )
			{ m_roiWord   = roiword;     };
	/**  set number of cells used from RoI  */
	inline void setNCells   ( int ncells     )
			{ m_numberUsedCells = ncells;};
	/**  set quality of cluster built (to be defined)  */
	inline void setquality  ( unsigned int quality )
			{ m_quality   = quality;     };
	
	// get methods
	/**  get Raw Energy (no calibration)   */
	inline float rawEnergy( ) const
			{ return m_rawEnergy       ;};
	/**  get Raw Et (no calibration)   */
	inline float rawEt    ( ) const
			{ return m_rawEt           ;};
	/**  get Raw Energy (no calibration) per sampling layer  */
	inline float rawEnergy( CaloSampling::CaloSample sample ) const
			{ return m_rawEnergyS[sample]; };
	/**  get Raw Eta (no calibration)   */
	inline float rawEta   ( ) const
			{ return m_rawEta          ;};
	/**  get Raw Phi (no calibration)   */
	inline float rawPhi   ( ) const
			{ return m_rawPhi          ;};
	/**  get RoI Word   */
	inline long  RoIword  ( ) const
			{ return m_roiWord         ;};
	/**  get number of cells used from RoI  */
	inline int   nCells   ( ) const
			{ return m_numberUsedCells ;};
	/**  get quality of cluster built (to be defined)  */
	inline unsigned int quality  ( ) const
			{ return m_quality         ;};

	// internal variables must be protected to be
	// modified in daughter classes like TrigEM/Tau Clusters.
protected:
	/** Raw Energy (no calibration) */
	float m_rawEnergy;
	/** Raw Et (no calibration) */
	float m_rawEt;
	/** Raw Energy per sample (upto 25 samples - no calibration) */
	float m_rawEnergyS[MAXSIZE];
	/** Raw Eta (no calibration) */
	float m_rawEta;
	/** Raw Phi (no calibration) */
	float m_rawPhi;
	/** RoI Word */
	long  m_roiWord;
	/** Number of cells used from RoI */
	int   m_numberUsedCells;
	/**  quality of cluster built (to be defined)  */
	unsigned int m_quality;

};

// stream output
std::string str( const TrigCaloCluster& d );
MsgStream& operator<< ( MsgStream& m, const TrigCaloCluster& d );

// comparison
bool operator== (const TrigCaloCluster& a, const TrigCaloCluster& b);
inline bool operator!= ( const TrigCaloCluster& a,
	const TrigCaloCluster& b ) { return !(a==b); }

void diff( const TrigCaloCluster& clus1, const TrigCaloCluster& clus2,
           std::map< std::string, double >& varChange );

CLASS_DEF ( TrigCaloCluster, 220079886, 1)

#endif
