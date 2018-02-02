/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************************************
//  Filename : TileMuFeature.h
//  Author   : Heuijin Lim
//  Created  : Sep. 2007
//
//  DESCRIPTION:
//    Contain the muon candidates from TileCal and the combined tracks from
//    Inner-Detector.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

#ifndef TRIGMUONEVENT_TILEMUFEATURE_H
#define TRIGMUONEVENT_TILEMUFEATURE_H
 
#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"

#include <vector>
#include <string>
#include <map>
#include <math.h>

class TileMuFeature 
{ 
 public: 

  TileMuFeature();

  TileMuFeature ( float eta, float phi, const std::vector<float> & ener, float qual );
  TileMuFeature ( float eta, float phi, std::vector<float> && ener, float qual );

  /** Destructor **/
  //virtual ~TileMuFeature() {}
  //~TileMuFeature();

  //void Set( float eta, float phi, std::vector<float> & ener, float qual ); 

  /** Copy pointer constructor */
  TileMuFeature(const TileMuFeature* muon_feature);

  /** Copy reference constructor */
  TileMuFeature(const TileMuFeature& muon_feature);

  /** Assignement operator */
  TileMuFeature& operator=(const TileMuFeature& muon_feature); 
  TileMuFeature& operator=(TileMuFeature&& muon_feature); 

  /** Accessor */
  float eta()                 const { return m_eta; }
  float phi()                 const { return m_phi; }
  const std::vector<float>& enedep() const { return m_energy_deposited; }
  float quality()             const { return m_quality_factor; }

  //void        print    (void) const;
  // Convertion operator to a std::string
  // Can be used in a cast operation : (std::string) TileMuFeature
  //operator std::string() const;

  /*
  void SetIDTrk(float Pt_IDTrk, float Eta_IDTrk, float EtaTR_IDTrk, float Phi_IDTrk, float PhiTR_IDTrk, float zPos_IDTrk, int Typ_IDTrk); 
  float Pt_IDTrk()    const        { return m_Pt_IDTrk; 	}
  float Eta_IDTrk()   const	    { return m_Eta_IDTrk; 	}
  float EtaTR_IDTrk() const	    { return m_EtaTR_IDTrk;     }
  float Phi_IDTrk()   const	    { return m_Phi_IDTrk;	}
  float PhiTR_IDTrk() const  	    { return m_PhiTR_IDTrk;	}
  float zPos_IDTrk()  const	    { return m_zPos_IDTrk;	}
  int    Typ_IDTrk()   const        { return m_IDTrk; 		}
  */

 private:

  float m_eta;
  float m_phi;
  std::vector<float> m_energy_deposited;
  float m_quality_factor;

  /*
  float m_Pt_IDTrk, m_Eta_IDTrk, m_EtaTR_IDTrk;
  float m_Phi_IDTrk, m_PhiTR_IDTrk, m_zPos_IDTrk;
  int    m_IDTrk; 
  */  
};

/** Helper function for printing the object */
std::string str( const TileMuFeature& d );

/** Helper operator for printing the object */
MsgStream& operator<< ( MsgStream& m, const TileMuFeature& d );

/** Operator comparing two CombinedMuonFeature objects for equality */
bool operator== ( const TileMuFeature& a, const TileMuFeature& b );

/** Operator comparing two CombinedMuonFeature objects for inequality */
inline bool operator!= ( const TileMuFeature& a, const TileMuFeature& b )
{
  return !( a == b );
}

/** Comparison with feedback */
void diff( const TileMuFeature& a, const TileMuFeature& b, std::map< std::string, double >& variableChange );

CLASS_DEF(TileMuFeature,             1503,     0)
CLASS_DEF(DataVector<TileMuFeature>, 49567812, 0)

#include "TrigMuonEvent/TileMuFeatureContainer.h"

#endif    
