/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUCLUSTER_H
#define TRIGTAUCLUSTER_H


/*******************************************************

NAME:		TrigTauCluster.h
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHORS:	Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:	Keep the important output variables
		from the Calorimeter LVL2 Tau Trigger.
		Optionally seed ID Trigger and EF.
		Variables here should be corrected variables
		not raw (in mother class).
INHERITSFROM:	TrigCaloCluster
DATE:		October 17th, 2005

******************************************************/
// includes section
#include <math.h>
#include "AthLinks/ElementLink.h"
#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"

/** Class with calibrated variables for tau clustering.
        Inherits all Raw variables from TrigCaloCluster */
class TrigTauCluster : public TrigCaloCluster
{
public:
	/**  Constructors, please note the default values    */
	TrigTauCluster(float energy = 0.0, float eta = -10.0,
                float phi = -10.0, long roi_word = 0);
	TrigTauCluster(const TrigTauClusterDetailsContainer *details_cont,
	                unsigned int details_index,
                        float energy = 0.0, float eta = -10.0,
                        float phi = -10.0, long roi_word = 0);
	/** Destructor  */
	~TrigTauCluster();
	/**  Copy Pointer Constructor   */
	TrigTauCluster(const TrigTauCluster* trigemcluster) ;
	/**  Copy Reference Constructor   */
	TrigTauCluster(const TrigTauCluster& trigemcluster) ;

	// Print out variables for Debug
	/**  Prints out cluster variables to std::cout   */
	void print(void) const;
	/**  Prints out cluster variables to MsgStream   */
	void print(MsgStream& log) const;

	// set internal variables methods
	/**  set EM Energy */
	inline void setEMenergy(float energy) { m_EMenergy = energy; };
	/**  set HAD Energy */
	inline void setHADenergy(float energy) { m_HADenergy = energy; };
	/**  set Eta of the cluster */
	inline void setEta(float eta) { setRawEta(eta); };
	/**  set Phi of the cluster */
	inline void setPhi(float phi) { setRawPhi(phi); };
	/**  set EM Radius*/
	inline void setEMRadius2(float emradius) { m_EMRadius2 = emradius; };
	/**  set Total Calo Radius*/
	inline void setCaloRadius(float caloradius) { m_CaloRadius = caloradius; };
	/**  set Isolation Fraction */
	inline void setIsoFrac(float isofrac) { m_IsoFrac = isofrac; };
	/**  set Width in the strip sampling */
	inline void setStripWidth(float stripwidth) { m_stripWidth = stripwidth; };
	/**  set strip Width as in offline */
	inline void setStripWidthOffline(float stripwidthOffline) { m_stripWidthOffline = stripwidthOffline; };
	/**  set Number of fired strips */
	inline void setNumStripCells(int numstripcells) { m_numStripCells = numstripcells; };
	/**  set Number of Cells in the cluster */
	inline void setNumTotCells(int numtotcells) { m_numTotCells = numtotcells; };
	/**  set Calibrated Energy */
	inline void setEnergyCalib(float e) { m_eCalib = e; };
	/** set EM Radius 3S */
	inline void setEMRadius3S(float emradius3s) { m_EMRadius3S = emradius3s; };
	/** set Core Fraction */
	inline void setCoreFrac(float coreFrac) { m_CoreFrac = coreFrac; };
	/** set EM Fraction */
	inline void setEMFrac(float emFrac) { m_EMFrac = emFrac; };
	/** set Hadronic Radius */
	inline void setHadRadius(float hadRadius) { m_HadRadius = hadRadius; };

	// get internal variables methods
	/**  get Raw Energy */
	inline float energy() const { return rawEnergy(); };
	/**  get Raw Et */
	inline float et() const { return rawEnergy() / cosh(eta()); };
	/**  get EM calibrated Energy */
	inline float EMCalibEnergy() const { return 0; };
	/**  get EM Energy */
	inline float EMenergy() const { return m_EMenergy; };
	/**  get HAD Energy */
	inline float HADenergy() const { return m_HADenergy;};
	/**  get Energy per sampling */
	inline float energy(CaloSampling::CaloSample /* sample */) const { return 0.; };
	/**  get Energy per sampling */
	inline float energyInSample(CaloSampling::CaloSample /* sample */) const { return 0.; };
	/**  get Eta of the cluster */
	inline float eta() const { return rawEta(); };
	/**  get Phi of the cluster */
	inline float phi() const { return rawPhi(); };
	/**  get EM Radius */
	inline float EMRadius2() const { return m_EMRadius2; };
	/**  get Total Calo Radius */
	inline float CaloRadius() const { return m_CaloRadius; };
	/**  get EM Radius per sampling */
	inline float EMRadius(int samp) const { return m_clusterDetails.isValid() && clusterDetails() != 0 ? clusterDetails()->EMRadius(samp) : 0; };
	/**  get Isolation Fraction */
	inline float IsoFrac() const { return m_IsoFrac; };
	/**  get Width in EM samplings */
	inline float EMenergyWidth(int samp) const { return m_clusterDetails.isValid() && clusterDetails() != 0 ? clusterDetails()->EMenergyWidth(samp) : 0; };
	/**  get Width in HAD samplings */
	inline float HADenergyWidth(int samp) const { return m_clusterDetails.isValid() && clusterDetails() != 0 ? clusterDetails()->HADenergyWidth(samp) : 0; };
	/**  get Number of fired strips */
	inline int numStripCells() const { return m_numStripCells; };
	/**  get Number of total cells in the cluster */
	inline int numTotCells() const { return m_numTotCells; };
	/**  get Width in the strip sampling */
	inline float stripWidth() const { return m_stripWidth; };
	/**  get Width as in offline */
	inline float stripWidthOffline() const { return m_stripWidthOffline; };
	/**  get Calibrated Energy */
	inline float eCalib() const { return m_eCalib; };
	/**  get EM calibrated Energy */
	inline float eEMCalib() const { return 0; };
	/** get EM Radius 3S */
	inline float EMRadius3S() const { return m_EMRadius3S; };
	/** get Core Fraction */
	inline float CoreFrac() const { return m_CoreFrac; };
	/** get EM Fraction */
	inline float EMFrac() const { return m_EMFrac; };
	/** get Hadronic Radius */
	inline float HadRadius() const { return m_HadRadius; };

	/**  get EM Energy in the wide region (0.3x0.3) */
	inline float EMenergyWide(int samp) const { return  m_clusterDetails.isValid() && clusterDetails() != 0 ? clusterDetails()->EMenergyWide(samp) : 0; };
	/**  get EM Energy in the medium region (0.2x0.2) */
	inline float EMenergyMedium(int  samp) const { return  m_clusterDetails.isValid() && clusterDetails() != 0 ? clusterDetails()->EMenergyMedium(samp) : 0; };
	/**  get EM Energy in the narrow region (0.1x0.1) */
	inline float EMenergyNarrow(int samp) const { return  m_clusterDetails.isValid() && clusterDetails() != 0 ? clusterDetails()->EMenergyNarrow(samp) : 0; };

	/**  get HAD Energy in the wide region (0.3x0.3) */
	inline float HADenergyWide(int samp) const { return  m_clusterDetails.isValid() && clusterDetails() != 0 ? clusterDetails()->HADenergyWide(samp) : 0;};
	/**  get HAD Energy in the medium region (0.2x0.2) */
	inline float HADenergyMedium(int samp) const { return  m_clusterDetails.isValid() && clusterDetails() != 0 ? clusterDetails()->HADenergyMedium(samp) : 0;};
	/**  get HAD Energy in the narrow region (0.1x0.1) */
	inline float HADenergyNarrow(int samp) const { return  m_clusterDetails.isValid() && clusterDetails() != 0 ? clusterDetails()->HADenergyNarrow(samp) : 0;};

	/** accessor to TrigTauClusterDetails* **/
	const TrigTauClusterDetails* clusterDetails() const;
	const ElementLink<TrigTauClusterDetailsContainer>& clusterDetailsLink() const;
        void setClusterDetails (const ElementLink<TrigTauClusterDetailsContainer>& clusterDetailsLink);


// internal variables must be private
private:
	/**  EM Energy */
	float m_EMenergy;
	/**  HAD Energy */
	float m_HADenergy;
	/**  Calibrated Energy */
	float m_eCalib;
	/**  EM Radius */
	float m_EMRadius2;
	/**  Total Calo Radius */
	float m_CaloRadius;
	/**  Isolation Fraction */
	float m_IsoFrac;
	/**  Number of fired strips */
	int m_numStripCells;
	/**  Total number of cells in the cluster */
	int m_numTotCells;
	/**  Width in the strip sampling */
	float m_stripWidth;
	/**  Width calculated as in Offline tauRec */
	float m_stripWidthOffline;
	/** EM Radius 3S */
	float m_EMRadius3S;
	/** Core Fraction */
	float m_CoreFrac;
	/** EM Fraction */
	float m_EMFrac;
	/** Hadronic Radius */
	float m_HadRadius;
	
	bool m_valid;
	/** ElementLink to TrigTauClusterDetailsContainer **/
	ElementLink<TrigTauClusterDetailsContainer> m_clusterDetails;

};

/// Helper function for printing the object
std::string str(const TrigTauCluster& tau);

/// Helper operator for printing the object
MsgStream& operator<< (MsgStream& m, const TrigTauCluster& tau);

/// Operator comparing two TrigTauCluster objects for equality
bool operator== (const TrigTauCluster& left, const TrigTauCluster& right);
/// Operator comparing two TrigTauCluster objects for inequality
inline bool operator!= (const TrigTauCluster& left, const TrigTauCluster& right)
{
	return !(left == right);
}

/// Comparison with feedback
void diff(const TrigTauCluster& left, const TrigTauCluster& right, std::map<std::string, double>& varChange);


// CLIDSvc is already loaded in base class
CLASS_DEF ( TrigTauCluster, 213051884, 1)

#include "TrigCaloEvent/TrigTauClusterContainer.h"
#endif
