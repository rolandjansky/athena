/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUCLUSTERDETAILS_H
#define TRIGTAUCLUSTERDETAILS_H

/*******************************************************

NAME:		TrigTauClusterDetails.h
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHOR:	Denis M.P. Casado

PURPOSE:	Keep Details of TrigTauCluster
DATE:		January 14th, 2009

******************************************************/
// includes section
#include <math.h>
// for printing out
#include <iostream>
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/CLASS_DEF.h"

/** Number of EM Samples */
const int NUMEMSAMP = 4;
/** Number of HAD Samples */
const int NUMHADSAMP = 3;

class TrigTauClusterDetails {

public:
	/**  Constructor, please note the default values    */
	TrigTauClusterDetails ();
	/** Destructor  */
	~TrigTauClusterDetails ();
	/**  Copy Pointer Constructor   */
	TrigTauClusterDetails ( const TrigTauClusterDetails* trigemcluster ) ;
	/**  Copy Reference Constructor   */
	TrigTauClusterDetails ( const TrigTauClusterDetails& trigemcluster ) ;

        // Print out variables for Debug
	/**  Prints out cluster variables to std::cout   */
        void print ( void ) const;
	/**  Prints out cluster variables to MsgStream   */
        void print ( MsgStream& log ) const;

	/**  set EM Radius per sampling */
	inline void setEMRadius      ( int samp, float emradius        )
			{ m_EMRadius[samp]      = emradius;      };
	/**  set HAD Radius per sampling */
	inline void setHADRadius      ( int samp, float hadradius        )
			{ m_HADRadius[samp]      = hadradius;      };
	/**  set Width in EM samplings */
	inline void setEMenergyWidth    ( int samp, float stripwidth   )
			{ m_EMenergyWidth[samp]    = stripwidth;    };
	/**  set Width in HAD samplings */
        inline void setHADenergyWidth    ( int samp, float stripwidth  )
                        { m_HADenergyWidth[samp]    = stripwidth;    };

	/**  set EM Energy in the mormalization region (0.3x0.3) */
        inline void setEMenergyWide  (int samp, float e ) 
                        { m_EMenergyWide[samp] = e ; };
	/**  set EM Energy in the wide region (0.2x0.2) */
        inline void setEMenergyMedium  (int samp, float e ) 
                        { m_EMenergyMedium[samp] = e ; };
	/**  set EM Energy in the narrow region (0.1x0.1) */
        inline void setEMenergyNarrow  (int samp, float e ) 
                        { m_EMenergyNarrow[samp] = e ; };
	/**  set HAD Energy in the mormalization region (0.3x0.3) */
        inline void setHADenergyWide  (int samp, float e ) 
                        { m_HADenergyWide[samp] = e ; };
	/**  set HAD Energy in the wide region (0.2x0.2) */
        inline void setHADenergyMedium  (int samp, float e )
                        { m_HADenergyMedium[samp] = e ; };
	/**  set HAD Energy in the narrow region (0.1x0.1) */
        inline void setHADenergyNarrow  (int samp, float e )
                        { m_HADenergyNarrow[samp] = e ; };
	
	// get internal variables methods
	/**  get EM Radius per sampling */
	inline float EMRadius      (int samp ) const { return m_EMRadius[samp]      ; };
	/**  get HAD Radius per sampling */
	inline float HADRadius      (int samp ) const { return m_HADRadius[samp]      ; };
	/**  get Width in EM samplings */
	inline float EMenergyWidth (int samp ) const 
                      { return m_EMenergyWidth[samp] ; };
	/**  get Width in HAD samplings */
        inline float HADenergyWidth (int samp ) const 
                      { return m_HADenergyWidth[samp] ; };

	/**  get EM Energy in the mormalization region (0.3x0.3) */
        inline float EMenergyWide(int samp) const {return m_EMenergyWide[samp];};
	/**  get EM Energy in the wide region (0.2x0.2) */
        inline float EMenergyMedium(int samp) const {return m_EMenergyMedium[samp];};
	/**  get EM Energy in the narrow region (0.1x0.1) */
        inline float EMenergyNarrow(int samp) const {return m_EMenergyNarrow[samp];};

	/**  get HAD Energy in the normalization region (0.3x0.3) */
        inline float HADenergyWide(int samp) const {return m_HADenergyWide[samp];};
	/**  get HAD Energy in the wide region (0.2x0.2) */
        inline float HADenergyMedium(int samp) const {return m_HADenergyMedium[samp];};
	/**  get HAD Energy in the narrow region (0.1x0.1) */
        inline float HADenergyNarrow(int samp) const {return m_HADenergyNarrow[samp];};


// internal variables must be private
private:
	/**  EM Radius per sampling */
	float m_EMRadius[NUMEMSAMP];
	/**  HAD Radius per sampling */
	float m_HADRadius[NUMHADSAMP];
	/**  Width in EM samplings */
	float m_EMenergyWidth[NUMEMSAMP];
	/**  Width in HAD samplings */
        float m_HADenergyWidth[NUMHADSAMP];

	/**  EM Energy in the mormalization region (0.3x0.3) */
        float m_EMenergyWide[NUMEMSAMP];
	/**  EM Energy in the wide region (0.2x0.2) */
        float m_EMenergyMedium[NUMEMSAMP];
	/**  EM Energy in the narrow region (0.1x0.1) */
        float m_EMenergyNarrow[NUMEMSAMP];

	/**  HAD Energy in the normalization region (0.3x0.3) */
        float m_HADenergyWide[NUMHADSAMP];
	/**  HAD Energy in the wide region (0.2x0.2) */
        float m_HADenergyMedium[NUMHADSAMP];
	/**  HAD Energy in the narrow region (0.1x0.1) */
        float m_HADenergyNarrow[NUMHADSAMP];

};

/// Helper function for printing the object
std::string str( const TrigTauClusterDetails& tau );

/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const TrigTauClusterDetails& tau );

/// Operator comparing two TrigTauCluster objects for equality
bool operator== ( const TrigTauClusterDetails& left, const TrigTauClusterDetails& right );
/// Operator comparing two TrigTauCluster objects for inequality
inline bool operator!= ( const TrigTauClusterDetails& left, const TrigTauClusterDetails& right ) {
   return !( left == right );
}
	
/// Comparison with feedback
void diff( const TrigTauClusterDetails& left, const TrigTauClusterDetails& right,
           std::map< std::string, double >& varChange );


// CLIDSvc is already loaded in base class
CLASS_DEF ( TrigTauClusterDetails, 56592609, 1)

#endif
