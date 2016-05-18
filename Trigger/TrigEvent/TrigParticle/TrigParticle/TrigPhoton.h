// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

File: TrigPhoton.h

Description: To be created by fex algorithms in Level 2 chains. 
             To be used as a way to communicate L2 decisions and 
             to keep information to be available offline.
  
Design: Must be light and easy to serialize AND to store in ESD; 


Authour: R.Goncalo - Tue Jun 06 22:02:43 BST 2005
Modified: RG Nov  7 2006: to inherit from INav4M and 4-mom base
Modified: A.Hamilton (20 June 2008) to use ElementLink to TrigEMCluster
Modified: A.Hamilton (Aug 2009) - changed constructor to be similar to TrigElectron
Modified: V.Dao      (Oct 2009) - changed constructor back to fill the deltaEta, deltaPhi variables
*/

#ifndef TRIG_PHOTON
#define TRIG_PHOTON

#include <vector>
#include <iostream>
#include <string>

#include "FourMom/P4PtEtaPhiM.h"
#include "Navigation/Navigable.h"
#include "EventKernel/INavigable4Momentum.h"

#include "DataModel/ElementLink.h"

#include "TrigCaloEvent/TrigEMClusterContainer.h"


class TrigPhoton : public P4PtEtaPhiM,  
		   public NavigableTerminalNode ,
		   virtual public INavigable4Momentum {
 
 public:
  
  /** constructors; note: default constructor needed by POOL */
  TrigPhoton();
  
  // Oct. 2009 - Valerio Dao  replace previous constructor to set dEta and dPhi
  //TrigPhoton(int roi, 
  //	     const TrigEMClusterContainer* cluster_cont, 
  //	     unsigned int cluster_index );
   

  TrigPhoton(int roi, float dphi, float deta,
  	     const TrigEMClusterContainer* cluster_cont, 
	     unsigned int cluster_index );

  /** Initialize without accessing cluster objects. */
  TrigPhoton(float pt,
             float eta,
             float phi,

             // roi word
             unsigned int roi,
             bool valid,

             // Cluster
             const ElementLink< TrigEMClusterContainer >& cluster,
             float HadEt,
             float energyRatio,
             float rCore,
             float deta,
             float dphi,
             float Fside,
             float Weta2,
             float F0,
             float F1,
             float F2,
             float F3);



  TrigPhoton(const TrigPhoton& te);

  ~TrigPhoton();
  
  void  validate(bool v) { m_valid = v;       } // OBSOLETE!: to disappear in next iteration
  bool  isValid()  const { return m_valid;    }
  
  /** accessor to get RoI ID (should also introduce roiWord */
  int   roiId()    const { return m_roiID;    }// OBSOLETE!: to disappear in next iteration
  int   roiWord()  const { return m_roiID;    } 

  /** accessor EM cluster et */
  float Et()       const { return this->pt(); }// OBSOLETE!: to disappear in next iteration

  /** accessor for hadronic energy in 1st hadronic sampling behind cluster */
  float HadEt()    const { return m_HadEt;    }// OBSOLETE!: to disappear in next iteration
  float HadEt1()   const { return m_HadEt;    }

  /** accessor for fraction difference of EM cluster emaxs1 and e2tsts1 */
  float energyRatio() const { return m_energyRatio;    }// OBSOLETE!: to disappear in next iteration
  float Eratio()      const { return m_energyRatio;    }

  /** cluster energy in 3x7 cells / energy in 7x7 cells in 2nd sampling */
  float rCore() const { return m_rCore;    }// OBSOLETE!: to disappear in next iteration
  float Reta()  const { return m_rCore;    }

  /** energy in a 7 strips (around hottest strip) minus energy
      in 3 strips divided by energy in 3 strips */
  float Fside() const { return m_Fside; }

  /** cluster width (based on a 3x5 cluster - 2nd layer) */
  float Weta2() const { return m_Weta2; }

  /** accessor for difference between cluster and Roi phi */
  float dPhi()  const { return m_dPhi;  }

  /** accessor for difference between cluster and Roi eta */
  float dEta()  const { return m_dEta;  }

  /** accessors for calibrated energy and energy fraction in calo samples */
  //  float E()     const;
  float F0()    const { return m_cl_e_frac_S0; } 
  float F1()    const { return m_cl_e_frac_S1; } 
  float F2()    const { return m_cl_e_frac_S2; } 
  float F3()    const { return m_cl_e_frac_S2; } 
  
  /** other accessors for shower shapes */

  /** accessor for pointer to TrigEMCluster */
  const TrigEMCluster* cluster() const;
  const ElementLink< TrigEMClusterContainer >& clusterLink() const
  { return m_cluster; }

 private:
  // private data members
  int   m_roiID;
  float m_HadEt; 
  float m_energyRatio;
  float m_rCore;
  float m_dPhi;
  float m_dEta;
  //  float m_cl_energy;    
  float m_cl_e_frac_S0; // Calibrated energy  per-sampling
  float m_cl_e_frac_S1;
  float m_cl_e_frac_S2;
  float m_cl_e_frac_S3;
  float m_Fside;        // fracs1 from TrigEMCluster
  float m_Weta2;        // weta2 from TrigEMCluster
  bool  m_valid;

  ElementLink< TrigEMClusterContainer > m_cluster;

};

/** comparison operators */
bool operator==( const TrigPhoton& tp1,  const TrigPhoton& tp2 );

inline bool operator!=( const TrigPhoton& tp1,  const TrigPhoton& tp2 ) {
  return !( tp1 == tp2 );
}

void diff( const TrigPhoton& tp1,  const TrigPhoton& tp2, std::map<std::string, double>& v_diff );

/** overloaded << opetrator to print TrigPhoton quantities */
MsgStream& operator<< ( MsgStream& msg, const TrigPhoton& tp );

CLASS_DEF( TrigPhoton , 203968502 , 1 )

#endif // TRIG_PHOTON
