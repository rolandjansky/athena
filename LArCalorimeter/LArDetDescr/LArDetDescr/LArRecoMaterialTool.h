/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
//
// 3.8.2005 Creation of the tool by claire.bourdarios@cern.ch
//
// ***************************************************************************

#ifndef LARDETDESCR_LARRECOMATERIALTOOL_H
#define LARDETDESCR_LARRECOMATERIALTOOL_H

#include "CaloDetDescr/ICaloRecoMaterialTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

//done in the interface : #include "CLHEP/Geometry/Transform3D.h"

class CaloCell_ID;
class CaloDM_ID;
class IMessageSvc;
class StoreGateSvc;

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"

#include <string>

//<<<<<< INCLUDES >>>>>>

/** This class implements the CaloDetDescr/ICaloRecoMaterialTool and provides the average
    materials needed for the LAr part of the Calorimeter/CaloTrackingGeometry volumes.

    For this, it uses the PVLinks provided by the LArSubdetPosHelper. 

    Note that it means that eta/phi dependencies of the material within a given GeoModel
    volume is not supported : although an interface using the CaloDM identifiers is foreseen,
    numbers will be averaged. 

    Interface and implementation are preliminary !!!!

 **/

class LArRecoMaterialTool : public AthAlgTool,
  virtual public ICaloRecoMaterialTool
{
public:
  
  // Constructors:

  LArRecoMaterialTool(const std::string& type, 
		  const std::string& name, 
		  const IInterface* parent); 

  ~LArRecoMaterialTool();
  static const InterfaceID& interfaceID( ) ; 
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual
  bool get_material (CaloSubdetNames::ALIGNVOL alvol, 
		     double& mass, double& volume,
		     double& x0, double& dEdx,
		     double& aveA, double& aveZ
		     ) const  override;

  /** default material used to fill the envelope */
  virtual
  void get_default_material (double& density,
			     double& x0, double& dEdx,
			     double& aveA, double& aveZ
			     ) const override;

  void print() const override;

private:

  /** Private bool set at initialisation : 
      decides if the GeoModel is parsed or bypassed */
  bool  m_parseGeoModelForMaterial;

  /**  Private method called at initialisation : 
       fills all arrays by calling the "optimised" methods */
  void ScanTree(); 

  /** Private optimised methods for each subsystem */

  bool ScanCRYO(CaloSubdetNames::ALIGNVOL  alvol,
		double& mass, double& x0, double& dEdx,
		double& aveA, double& aveZ); 

  bool ScanPS( CaloSubdetNames::ALIGNVOL alvol,
	       double& mass, double& x0, double& dEdx,
	       double& aveA, double& aveZ); 

  bool ScanEMB( CaloSubdetNames::ALIGNVOL alvol,
		double& mass, double& x0, double& dEdx,
		double& aveA, double& aveZ); 

  bool ScanEMEC( CaloSubdetNames::ALIGNVOL alvol,
		double& mass, double& x0, double& dEdx,
		double& aveA, double& aveZ); 

  bool ScanHEC( CaloSubdetNames::ALIGNVOL alvol,
		double& mass, double& x0, double& dEdx,
		double& aveA, double& aveZ); 

  bool ScanFCAL( CaloSubdetNames::ALIGNVOL alvol,
		double& mass, double& x0, double& dEdx,
		double& aveA, double& aveZ); 

  /** Private method to blend GeoLogVolume */
  void blendGeoVolume(GeoPVConstLink& geoVolLink,
		      std::vector<double>& volume,
		      std::vector<double>& mass,
		      std::vector<double>& x0,
		      std::vector<double>& dEdX,
		      std::vector<double>& aveA,
		      std::vector<double>& aveZ) const;
                          
  /** Private method to add material */
  void addMaterialFraction(const GeoLogVol& geoVol,
			   std::vector<double>& volume, 
			   std::vector<double>& mass,
			   std::vector<double>& x0,
			   std::vector<double>& dEdX,
			   std::vector<double>& aveA,
			   std::vector<double>& aveZ) const;

  /** Private method to average results stored in private arrays */
  void averageFraction (std::vector<double>& volumeFractions, 
			std::vector<double>& massFractions,
			std::vector<double>& x0Fractions,
			std::vector<double>& dEdXFractions,
			std::vector<double>& aveAFractions,
			std::vector<double>& aveZFractions,
			double& mass, double& volume, 
			double& x0, double& dEdx,
			double& aveA, double& aveZ, double& rho );

  // mapping between ALIGNVOL and strings (need to retrieve StoredPV-s)
  std::string map_av(CaloSubdetNames::ALIGNVOL alvol);


  const CaloCell_ID*        m_calo_id;
  const CaloDM_ID*          m_dm_id;

  // Store results per ALIGNVOL :
  std::vector<CaloSubdetNames::ALIGNVOL>  m_alignvol_number;
  std::vector<double> m_alignvol_mass;
  std::vector<double> m_alignvol_x0;
  std::vector<double> m_alignvol_dEdX;
  std::vector<double> m_alignvol_aveA;
  std::vector<double> m_alignvol_aveZ;

  // intermediate vectors, cleared and filled within several methods :     
  std::vector<double>   m_massFractions;
  std::vector<double>   m_volumeFractions;
  std::vector<double>   m_x0Fractions;
  std::vector<double>   m_dEdXFractions;
  std::vector<double>   m_averageAFractions;
  std::vector<double>   m_averageZFractions;

  std::vector<double>   m_child_massFractions;
  std::vector<double>   m_child_volumeFractions;
  std::vector<double>   m_child_x0Fractions;
  std::vector<double>   m_child_dEdXFractions;
  std::vector<double>   m_child_averageAFractions;
  std::vector<double>   m_child_averageZFractions;
  std::vector<double>   m_child_Rho;
  std::vector<double>   m_child_Volume;
  std::vector<double>   m_child_Mass;

};

#endif // LARDETDESCR_LARMATERIALTOOL_H






