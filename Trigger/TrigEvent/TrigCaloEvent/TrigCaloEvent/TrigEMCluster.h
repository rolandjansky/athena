/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEMCLUSTER_H
#define TRIGEMCLUSTER_H


/*******************************************************

NAME:		TrigEMCluster.h
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHORS:	Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:	Keep the important output variables
		from the Calorimeter LVL2 egamma Trigger.
		Optionally seed ID Trigger and EF.
		Variables here should be corrected variables
		not raw (in mother class).
INHERITSFROM:	TrigCaloCluster
DATE:		October 17th, 2005

******************************************************/
// includes section
#include "TrigCaloEvent/TrigCaloCluster.h"
#include "AthLinks/ElementLink.h"
#include "TrigCaloEvent/RingerRingsContainer.h"

/** Class with calibrated variables for egamma clustering.
	Inherits all Raw variables from TrigCaloCluster */
class TrigEMCluster : public TrigCaloCluster {

public:
	/**  Constructor, please note the default values    */
	TrigEMCluster (float energy = 0.0, float eta = -10.0,
		float phi = -10.0, long roi_word = 0);
	/** Destructor  */
	~TrigEMCluster ();
	/**  Copy Pointer Constructor   */
	TrigEMCluster ( const TrigEMCluster* trigemcluster ) ;
	/**  Copy Reference Constructor   */
	TrigEMCluster ( const TrigEMCluster& trigemcluster ) ;

	/**  Prints out cluster variables to std::cout   */
        void print ( void ) const;
	/**  Prints out cluster variables to MsgStream   */
        void print ( MsgStream& log ) const;

	// set internal variables methods
	/**  set Energy (calibrated)   */
	inline void setEnergy   ( float energy  ) { m_Energy  = energy;  };
	/**  set Et (calibrated) - note that setting Energy
		this is not set by default   */
	inline void setEt       ( float et      ) { m_Et      = et;      };
	/**  set Energy (calibrated) per sampling layer  */
	inline void setEnergy( CaloSampling::CaloSample sample, float
			energy ) { m_EnergyS[sample] = energy; };
	/**  set Eta (calibrated) */
	inline void setEta      ( float eta     ) { m_Eta     = eta;     };
	/**  set Phi (calibrated) */
	inline void setPhi      ( float phi     ) { m_Phi     = phi;     };
	/**  set Energy in a 3x7 cluster (no calibration) around hottest cell */
	inline void set_e237    ( float e237    ) { m_e237    = e237;    };
	/**  set Energy in a 7x7 cluster (no calibration) around hottest cell */
	inline void set_e277    ( float e277    ) { m_e277    = e277;    };
	/**  set Energy in a 7 strips (around hottest strip) minus energy
		in 3 strips divided by energy in 3 strips */
	inline void set_fracs1  ( float fracs1  ) { m_fracs1  = fracs1;  };
	/**  set cluster width (based on a 3x5 cluster - 2nd layer) */
	inline void set_weta2   ( float weta2   ) { m_weta2   = weta2;   };
	/**  set hadronic Energy (first hadronic layer) */
	inline void set_ehad1   ( float ehad1   ) { m_ehad1   = ehad1;   };
	/**  set Eta sampling 1 (strip layer) */
	inline void set_Eta1    ( float eta1    ) { m_Eta1    = eta1;    };
	/**  set maximum energy in sampling 1 (strip layer) */
	inline void set_emaxs1  ( float emaxs1  ) { m_emaxs1  = emaxs1;  };
	/**  set second maximum energy in sampling 1 (strip layer) */
	inline void set_e2tsts1 ( float e2tsts1 ) { m_e2tsts1 = e2tsts1; };
	/**  set Energy in a 3x3 cluster (no calibration) around hottest cell */
	inline void set_e233    ( float e233    ) { m_e233    = e233;    };
	/**  set width in first layer */
	inline void set_wstot    ( float wstot    ) { m_wstot    = wstot;    };
	
	// get internal variables methods
	/**  get Energy (calibrated)   */
	inline float energy  (  ) const { return m_Energy  ; };
	/**  get Energy (calibrated)   */
	inline float e       (  ) const { return m_Energy  ; };
	/**  get Et (calibrated)   */
	inline float et      (  ) const { return m_Et      ; };
	/**  get Energy (calibrated) per sampling layer  */
        inline float energy( CaloSampling::CaloSample sample ) const
                        { return m_EnergyS[sample]; };
	/**  get Energy (calibrated) per sampling layer  */
        inline float energyInSample( CaloSampling::CaloSample sample ) const
                        { return m_EnergyS[sample]; };
	/**  get Eta (calibrated) */
	inline float eta     (  ) const { return m_Eta     ; };
	/**  get Phi (calibrated) */
	inline float phi     (  ) const { return m_Phi     ; };
	/**  get Energy in a 3x7 cluster (no calibration) around hottest cell */
	inline float e237    (  ) const { return m_e237    ; };
	/**  get Energy in a 7x7 cluster (no calibration) around hottest cell */
	inline float e277    (  ) const { return m_e277    ; };
	/**  get Energy in a 7 strips (around hottest strip) minus energy
		in 3 strips divided by energy in 3 strips */
	inline float fracs1  (  ) const { return m_fracs1  ; };
	/**  get cluster width (based on a 3x5 cluster - 2nd layer) */
	inline float weta2   (  ) const { return m_weta2   ; };
	/**  get hadronic Energy (first hadronic layer) */
	inline float ehad1   (  ) const { return m_ehad1   ; };
	/**  get Eta sampling 1 (strip layer) */
	inline float Eta1    (  ) const { return m_Eta1    ; };
	/**  get maximum energy in sampling 1 (strip layer) */
	inline float emaxs1  (  ) const { return m_emaxs1  ; };
	/**  get second maximum energy in sampling 1 (strip layer) */
	inline float e2tsts1 (  ) const { return m_e2tsts1 ; };
	/**  get Energy in a 3x3 cluster (no calibration) around hottest cell */
	inline float e233    (  ) const { return m_e233    ; };
	/**  get width in first layer */
	inline float wstot    (  ) const { return m_wstot    ; };

	bool isValid() const;
	void setRings(const RingerRingsContainer &ring_con, unsigned int ring_idx);
	const RingerRings *rings() const;
        void setRings(const ElementLink<RingerRingsContainer>& ringsLink);
        const ElementLink<RingerRingsContainer>& ringsLink() const;


// internal variables must be private
private:
	/**  Energy (calibrated)   */
	float m_Energy;
	/**  Energy (calibrated)   */
	float m_Et;
	/**  Energy (calibrated) per sampling layer  */
        float m_EnergyS[MAXSIZE];
	/**  Eta (calibrated) */
	float m_Eta;
	/**  Phi (calibrated) */
	float m_Phi;
	/**  Energy in a 3x7 cluster (no calibration) around hottest cell */
	float m_e237;
	/**  Energy in a 7x7 cluster (no calibration) around hottest cell */
	float m_e277;
	/**  Energy in a 7 strips (around hottest strip) minus energy
                in 3 strips divided by energy in 3 strips */
	float m_fracs1;
	/**  cluster width (based on a 3x5 cluster - 2nd layer) */
	float m_weta2;
	/**  hadronic Energy (first hadronic layer) */
	float m_ehad1;
	/**  Eta sampling 1 (strip layer) */
	float m_Eta1;
	/**  maximum energy in sampling 1 (strip layer) */
	float m_emaxs1;
	/**  get second maximum energy in sampling 1 (strip layer) */
	float m_e2tsts1;
	/**  Energy in a 3x3 cluster (no calibration) around hottest cell */
	float m_e233;
	/**  width in first layer */
	float m_wstot;

	ElementLink<RingerRingsContainer> m_rings;

	template <class T>
	friend class TrigEMClusterConverterBase;
};

// stream output
std::string str( const TrigEMCluster& d );
MsgStream& operator<< ( MsgStream& m, const TrigEMCluster& d );

//comparison
bool operator== (const TrigEMCluster& a, const TrigEMCluster& b);
inline bool operator!= ( const TrigEMCluster& a,
        const TrigEMCluster& b ) { return !(a==b); }

// CLIDSvc is already loaded in base class
CLASS_DEF ( TrigEMCluster, 31349851, 1)
#endif
