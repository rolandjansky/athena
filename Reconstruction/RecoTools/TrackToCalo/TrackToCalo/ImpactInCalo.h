/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMPACTINCALO_H
#define IMPACTINCALO_H
/*************************************************************************
	Package:
	File: ImpactInCalo.h
	Description:
           
*************************************************************************/

//<<<<<< INCLUDES

/**

The ImpactInCalo collection is created by the TrackToCaloAlg algorithm, 
which loops on all Tracks of a given collection and extrapolates them 
to the LAr presampler (_0), strips (_1), middle (_2), back (_3) and to the Tiles.

For each impact point, the cartesian coordinate x,y,z are in the 
official coordinate system, whereas etaCaloLocal and phiCaloLocal are the variables
to be compared with the CaloCluster's.

The Track DIRECTION at the impact point is also given, to avoid confusion with the
POSITION of the impact point.

This collection is used by the CBNT_TrackToCalo class.

 */

class ImpactInCalo  {

 public:
  // constructors
  ImpactInCalo() {};
  
  ImpactInCalo( double x_0, double y_0, double z_0, 
		double etaCaloLocal_0, double phiCaloLocal_0,
		double trketa_at_0, double trkphi_at_0,
		double x_1, double y_1, double z_1,
		double etaCaloLocal_1, double phiCaloLocal_1,
		double trketa_at_1, double trkphi_at_1,
		double x_2, double y_2, double z_2, 
		double etaCaloLocal_2, double phiCaloLocal_2,
		double trketa_at_2, double trkphi_at_2,
		double x_3, double y_3, double z_3, 
		double etaCaloLocal_3, double phiCaloLocal_3,
		double trketa_at_3, double trkphi_at_3,
		double x_tile, double y_tile, double z_tile, 
		double etaCaloLocal_tile, double phiCaloLocal_tile,
		double trketa_at_tile, double trkphi_at_tile )
    :
    m_x_0(x_0), m_y_0(y_0), m_z_0(z_0),
    m_etaCaloLocal_0(etaCaloLocal_0), m_phiCaloLocal_0(phiCaloLocal_0),
    m_trketa_at_0(trketa_at_0),m_trkphi_at_0(trkphi_at_0),
    m_x_1(x_1), m_y_1(y_1), m_z_1(z_1),
    m_etaCaloLocal_1(etaCaloLocal_1), m_phiCaloLocal_1(phiCaloLocal_1),
    m_trketa_at_1(trketa_at_1),m_trkphi_at_1(trkphi_at_1),
    m_x_2(x_2), m_y_2(y_2), m_z_2(z_2),
    m_etaCaloLocal_2(etaCaloLocal_2), m_phiCaloLocal_2(phiCaloLocal_2),
    m_trketa_at_2(trketa_at_2),m_trkphi_at_2(trkphi_at_2),
    m_x_3(x_3), m_y_3(y_3), m_z_3(z_3),
    m_etaCaloLocal_3(etaCaloLocal_3), m_phiCaloLocal_3(phiCaloLocal_3),
    m_trketa_at_3(trketa_at_3),m_trkphi_at_3(trkphi_at_3),
    m_x_tile(x_tile), m_y_tile(y_tile), m_z_tile(z_tile),
    m_etaCaloLocal_tile(etaCaloLocal_tile), m_phiCaloLocal_tile(phiCaloLocal_tile),
    m_trketa_at_tile(trketa_at_tile),m_trkphi_at_tile(trkphi_at_tile)
    {};

  // destructor
  virtual ~ImpactInCalo(){};
  //	virtual ~ImpactInCalo(){};
  
  void print() const ;
  
  // gets
  
  // Presampler
  inline double x_0() const { return m_x_0; }
  inline double y_0() const { return m_y_0; }
  inline double z_0() const { return m_z_0; }
  inline double etaCaloLocal_0() const { return m_etaCaloLocal_0; } 
  inline double phiCaloLocal_0() const { return m_phiCaloLocal_0; }
  inline double trketa_at_0() const { return m_trketa_at_0; };
  inline double trkphi_at_0() const { return m_trkphi_at_0; };
  
  // Strip
  inline double x_1() const { return m_x_1; }
  inline double y_1() const { return m_y_1; }
  inline double z_1() const { return m_z_1; }
  inline double etaCaloLocal_1() const { return m_etaCaloLocal_1; } 
  inline double phiCaloLocal_1() const { return m_phiCaloLocal_1; }
  inline double trketa_at_1() const { return m_trketa_at_1; };
  inline double trkphi_at_1() const { return m_trkphi_at_1; };
  
  // Middle
  inline double x_2() const { return m_x_2; }
  inline double y_2() const { return m_y_2; }
  inline double z_2() const { return m_z_2; }
  inline double etaCaloLocal_2() const { return m_etaCaloLocal_2; } 
  inline double phiCaloLocal_2() const { return m_phiCaloLocal_2; }
  inline double trketa_at_2() const { return m_trketa_at_2; };
  inline double trkphi_at_2() const { return m_trkphi_at_2; };
  
  // Back
  inline double x_3() const { return m_x_3; }
  inline double y_3() const { return m_y_3; }
  inline double z_3() const { return m_z_3; }
  inline double etaCaloLocal_3() const { return m_etaCaloLocal_3; } 
  inline double phiCaloLocal_3() const { return m_phiCaloLocal_3; }
  inline double trketa_at_3() const { return m_trketa_at_3; };
  inline double trkphi_at_3() const { return m_trkphi_at_3; };
  
  // Tile
  inline double x_tile() const { return m_x_tile; }
  inline double y_tile() const { return m_y_tile; }
  inline double z_tile() const { return m_z_tile; }
  inline double etaCaloLocal_tile() const { return m_etaCaloLocal_tile; } 
  inline double phiCaloLocal_tile() const { return m_phiCaloLocal_tile; }	
  inline double trketa_at_tile() const { return m_trketa_at_tile; };
  inline double trkphi_at_tile() const { return m_trkphi_at_tile; };
  
 private:
  
  // Presampler
  double m_x_0;
  double m_y_0;
  double m_z_0;
  double m_etaCaloLocal_0; 
  double m_phiCaloLocal_0;
  double m_trketa_at_0;
  double m_trkphi_at_0;
  
  // Strip
  double m_x_1;
  double m_y_1;
  double m_z_1;
  double m_etaCaloLocal_1; 
  double m_phiCaloLocal_1;
  double m_trketa_at_1;
  double m_trkphi_at_1;
  
  // Middle
  double m_x_2;
  double m_y_2;
  double m_z_2;
  double m_etaCaloLocal_2; 
  double m_phiCaloLocal_2;
  double m_trketa_at_2;
  double m_trkphi_at_2;
  
  // Back
  double m_x_3;
  double m_y_3;
  double m_z_3;
  double m_etaCaloLocal_3; 
  double m_phiCaloLocal_3;
  double m_trketa_at_3;
  double m_trkphi_at_3;
  
  // Tile
  double m_x_tile;
  double m_y_tile;
  double m_z_tile;
  double m_etaCaloLocal_tile; 
  double m_phiCaloLocal_tile;
  double m_trketa_at_tile;
  double m_trkphi_at_tile;
  
};


#endif // IMPACTINCALO_H
