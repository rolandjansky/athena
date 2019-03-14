/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// LArCalorimeter/LArDetDescr/LArRecoMaterialTool.cxx
// -------------------------------------------------------------
//****************************************************************************

#include "LArDetDescr/LArRecoMaterialTool.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>

#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/Units.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"

#include <iostream>
#include <iomanip>

static const InterfaceID IID_LArRecoMaterialTool("LArRecoMaterialTool", 1, 0);

const InterfaceID& LArRecoMaterialTool::interfaceID( ) 
{ return IID_LArRecoMaterialTool; }

LArRecoMaterialTool::LArRecoMaterialTool(const std::string& type, 
				   const std::string& name, 
				   const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_calo_id(0),m_dm_id(0)
{
  declareInterface<LArRecoMaterialTool>( this );
}

StatusCode
LArRecoMaterialTool::initialize()
{
  ATH_CHECK( detStore()->retrieve (m_calo_id, "CaloCell_ID") );
  ATH_CHECK( detStore()->retrieve (m_dm_id,   "CaloDM_ID") );
  
  // These vectors will contain the results :
  m_alignvol_number.resize(0);
  m_alignvol_mass.resize(0);
  m_alignvol_x0.resize(0);
  m_alignvol_dEdX.resize(0);
  m_alignvol_aveA.resize(0);
  m_alignvol_aveZ.resize(0);

  // ---------- Fill arrays :

  // going though the full tree is too slow to be used as default, 
  // if you want to do it set this flag to true :
  // ( will become a property at some point ! )
  m_parseGeoModelForMaterial = false;

  this->ScanTree();
  //this->print();
    

  // ---------- That's it   


  ATH_MSG_INFO (" LArRecoMaterialTool successfully initialized ");
  return StatusCode::SUCCESS;
}

LArRecoMaterialTool::~LArRecoMaterialTool()
{
}

StatusCode
LArRecoMaterialTool::finalize()
{
  return StatusCode::SUCCESS;
}


bool 
LArRecoMaterialTool::get_material (CaloSubdetNames::ALIGNVOL alvol, 
				   double& mass, double& volume,
				   double& x0, double& dEdx,
				   double& aveA, double& aveZ
				   ) const
{
  bool result = false;

  for ( unsigned int i = 0; i < m_alignvol_number.size(); i++ )
    {
      if ( m_alignvol_number[i] == alvol ) {
	mass = m_alignvol_mass[i];
	x0 = m_alignvol_x0[i];
	dEdx = m_alignvol_dEdX[i];
	aveA = m_alignvol_aveA[i];
	aveZ = m_alignvol_aveZ[i];
	// FIXME
	volume = 1000.;
	result = true;
      }
    }

  return result;
}

void 
LArRecoMaterialTool::get_default_material (double& density,
					   double& x0, double& dEdx,
					   double& aveA, double& aveZ
					   ) const
{
  // FIXME ! should take real LAr numbers 
  density = 1.;
  x0 = 5.;
  dEdx = 1.1e-24;
  aveA = 39.948;
  aveZ = 18.;

}

void
LArRecoMaterialTool::print () const
{
    
  for ( unsigned int i = 0; i < m_alignvol_number.size(); i++ )
    {
      ATH_MSG_INFO (" + Result for ALIGNVOL " << m_alignvol_number[i]);
      ATH_MSG_INFO ("    - mass     [gram]  " << m_alignvol_mass[i]);
      ATH_MSG_INFO ("    - X0               " << m_alignvol_x0[i]);
      ATH_MSG_INFO ("    - DeDx             " << m_alignvol_dEdX[i]);
      ATH_MSG_INFO ("    - AverageA         " << m_alignvol_aveA[i]);
      ATH_MSG_INFO ("    - AverageZ         " << m_alignvol_aveZ[i]);    
    }
}

void
LArRecoMaterialTool::ScanTree()
{
  double mass = 0.;
  double  x0 = 0.;
  double dEdx = 0.;
  double aveA = 0.;
  double aveZ = 0.;

  bool result = false;

  ATH_MSG_INFO (" Entering method ScanTree() ");

  result = this->ScanCRYO(CaloSubdetNames::LARCRYO_B,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanCRYO(CaloSubdetNames::LARCRYO_EC_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanCRYO(CaloSubdetNames::LARCRYO_EC_NEG,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanCRYO(CaloSubdetNames::SOLENOID,mass,x0,dEdx,aveA,aveZ);

  result = this->ScanPS(CaloSubdetNames::PRESAMPLER_B_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanPS(CaloSubdetNames::PRESAMPLER_B_NEG,mass,x0,dEdx,aveA,aveZ);

  result = this->ScanEMB(CaloSubdetNames::EMB_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanEMB(CaloSubdetNames::EMB_NEG,mass,x0,dEdx,aveA,aveZ);

  result = this->ScanPS(CaloSubdetNames::PRESAMPLER_EC_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanPS(CaloSubdetNames::PRESAMPLER_EC_NEG,mass,x0,dEdx,aveA,aveZ);

  result = this->ScanEMEC(CaloSubdetNames::EMEC_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanEMEC(CaloSubdetNames::EMEC_NEG,mass,x0,dEdx,aveA,aveZ);

  result = this->ScanHEC(CaloSubdetNames::HEC1_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanHEC(CaloSubdetNames::HEC1_NEG,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanHEC(CaloSubdetNames::HEC2_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanHEC(CaloSubdetNames::HEC2_NEG,mass,x0,dEdx,aveA,aveZ);

  result = this->ScanFCAL(CaloSubdetNames::FCAL1_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanFCAL(CaloSubdetNames::FCAL1_NEG,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanFCAL(CaloSubdetNames::FCAL2_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanFCAL(CaloSubdetNames::FCAL2_NEG,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanFCAL(CaloSubdetNames::FCAL3_POS,mass,x0,dEdx,aveA,aveZ);
  result = this->ScanFCAL(CaloSubdetNames::FCAL3_NEG,mass,x0,dEdx,aveA,aveZ);

  if (!result)
    ATH_MSG_INFO (" If seems to have failed ?");

  return;
}

bool
LArRecoMaterialTool::ScanCRYO(CaloSubdetNames::ALIGNVOL alvol,
			      double& mass, double& x0, double& dEdx,
			      double& aveA, double& aveZ)
{
  mass =  0.;
  x0 = 0.;
  dEdx = 0.;
  aveA = 0.;
  aveZ = 0.;

  if (    alvol != CaloSubdetNames::LARCRYO_B 
       && alvol != CaloSubdetNames::LARCRYO_EC_POS
       && alvol != CaloSubdetNames::LARCRYO_EC_NEG
       && alvol != CaloSubdetNames::SOLENOID ) {
    ATH_MSG_INFO ("method ScanCryo does not support CaloSubdetNames::ALIGNVOL " << (int) alvol);
    return false;
  }

  // For EC, link to GeoModel does not exist (yet) : take barrel numbers

  // Shortcut : 
  if ( !m_parseGeoModelForMaterial )
    {
      //if ( alvol == CaloSubdetNames::LARCRYO_B || alvol == CaloSubdetNames::SOLENOID) {
	mass = 3.61158e+07  ;
	x0 = 89.411;
	dEdx = 7.47033e-24 ;
	aveA = 27.9728;
	aveZ = 13.4253 ; 
#if 0
      } else {
	mass = 3.61158e+07  ;
	x0 = 89.411;
	dEdx = 7.47033e-24 ;
	aveA = 27.9728;
	aveZ = 13.4253 ; 
      } 
#endif
    }
#if 0
  // Dead code as long as m_parseGeoModelForMaterial is false.
  // Comment out to avoid coverity warnings.
  else {

    // Go for it : 
    m_child_massFractions.clear();
    m_child_volumeFractions.clear();
    m_child_x0Fractions.clear();
    m_child_dEdXFractions.clear();
    m_child_averageAFractions.clear();
    m_child_averageZFractions.clear();
    m_child_Rho.clear();
    m_child_Volume.clear();
    m_child_Mass.clear();
    
    double       childVolume         = 0.;
    double       childMass           = 0.;
    double       childX0             = 0.;
    double       childDeDx           = 0.;
    double       childAverageA       = 0.;
    double       childAverageZ       = 0.;
    double       childRho            = 0.;
    
    const StoredPhysVol* storedPV = 0;
    std::string key = map_av(alvol);
    if(detStore()->contains<StoredPhysVol>(key))
    {
     if(detStore()->retrieve(storedPV,key)==StatusCode::FAILURE)
     {
       ATH_MSG_INFO ("Unable to retrieve Stored PV " << key);
     }
   }
    
    if (!storedPV) {
      ATH_MSG_INFO ("no Volume for " << key);
      return false;
    }

    const GeoFullPhysVol* larTopVolLink  = storedPV->getPhysVol();
    
    // get the number of direct childs
    unsigned int larChilds  = larTopVolLink->getNChildVols();
    
    for (unsigned int ichild = 0; ichild<larChilds; ++ichild){
      
      bool stop = false;
      
      // Skip some some of the children : list will get longer in future
      if ( alvol == CaloSubdetNames::LARCRYO_B && 
	   larTopVolLink->getNameOfChildVol(ichild) == "Total LAR Volume") stop = true;
      if ( alvol == CaloSubdetNames::SOLENOID && 
	   larTopVolLink->getNameOfChildVol(ichild) == "Total LAR Volume") stop = true;
      
      if (!stop) {
	
	ATH_MSG_DEBUG ("CaloSubdetNames::ALIGNVOL " << (int) alvol
                       <<" Direct Child " << ichild << " -  with name : " 
                       << larTopVolLink->getNameOfChildVol(ichild));
	
	PVConstLink  childVolLink          = larTopVolLink->getChildVol(ichild);
	
	m_massFractions.clear();
	m_volumeFractions.clear();
	m_x0Fractions.clear();
	m_dEdXFractions.clear();
	m_averageAFractions.clear();
	m_averageZFractions.clear();
	
	this->blendGeoVolume(childVolLink,
			     m_volumeFractions,
			     m_massFractions,
			     m_x0Fractions,
			     m_dEdXFractions,
			     m_averageAFractions,
			     m_averageZFractions);
	
	this->averageFraction(m_volumeFractions,
			      m_massFractions,
			      m_x0Fractions,
			      m_dEdXFractions,
			      m_averageAFractions,
			      m_averageZFractions,
			      childMass, childVolume, 
			      childX0, childDeDx,
			      childAverageA, childAverageZ, childRho );
	
	m_child_x0Fractions.push_back( childX0 );
	m_child_dEdXFractions.push_back( childDeDx );
	m_child_averageAFractions.push_back( childAverageA );
	m_child_averageZFractions.push_back( childAverageZ );
	m_child_Rho.push_back( childRho );
	m_child_Volume.push_back( childVolume );
	m_child_Mass.push_back( childMass );
	
      }
    }
    
    // Now do the final average and store it :
    
    ATH_MSG_DEBUG ("Final average for  CaloSubdetNames::ALIGNVOL " << (int) alvol
                   << " !!!!! ");
    
    this->averageFraction(m_child_Volume, m_child_Mass,
			  m_child_x0Fractions,
			  m_child_dEdXFractions,
			  m_child_averageAFractions,
			  m_child_averageZFractions,
			  childMass, childVolume, 
			  childX0, childDeDx,
			  childAverageA, childAverageZ, childRho );
    
    mass =  childMass/GeoModelKernelUnits::gram;
    x0 = childX0;
    dEdx = childDeDx;
    aveA = childAverageA;
    aveZ = childAverageZ;
  }
#endif 

  m_alignvol_number.push_back( alvol );
  m_alignvol_mass.push_back( mass );
  m_alignvol_x0.push_back( x0 );
  m_alignvol_dEdX.push_back( dEdx );
  m_alignvol_aveA.push_back( aveA );
  m_alignvol_aveZ.push_back( aveZ );

  return true;
}

bool
LArRecoMaterialTool::ScanPS(CaloSubdetNames::ALIGNVOL alvol,
			    double& mass, double& x0, double& dEdx,
			    double& aveA, double& aveZ)
{
  mass =  0.;
  x0 = 0.;
  dEdx = 0.;
  aveA = 0.;
  aveZ = 0.;

  if (    alvol != CaloSubdetNames::PRESAMPLER_B_POS 
       && alvol != CaloSubdetNames::PRESAMPLER_B_NEG
       && alvol != CaloSubdetNames::PRESAMPLER_EC_POS 
       && alvol != CaloSubdetNames::PRESAMPLER_EC_NEG 
	  ) {
    ATH_MSG_INFO ("method ScanCryo does not support CaloSubdetNames::ALIGNVOL " << (int) alvol);
    return false;
  }

  // for EMEC take Barrel numbers 

  // Shortcut : 
  if (!m_parseGeoModelForMaterial || 
      alvol == CaloSubdetNames::PRESAMPLER_EC_POS || 
      alvol == CaloSubdetNames::PRESAMPLER_EC_NEG)
    {
      mass = 1.80785e+06  ;
      x0 = 141.837;
      dEdx = 3.55847e-24;
      aveA = 39.5576;
      aveZ = 17.8274; 
    } 

  else {

    // Go for it : 
    m_child_massFractions.clear();
    m_child_volumeFractions.clear();
    m_child_x0Fractions.clear();
    m_child_dEdXFractions.clear();
    m_child_averageAFractions.clear();
    m_child_averageZFractions.clear();
    m_child_Rho.clear();
    m_child_Volume.clear();
    m_child_Mass.clear();
    
    double       childVolume         = 0.;
    double       childMass           = 0.;
    double       childX0             = 0.;
    double       childDeDx           = 0.;
    double       childAverageA       = 0.;
    double       childAverageZ       = 0.;
    double       childRho            = 0.;
    
    StoredPhysVol* storedPV = 0;
    std::string key = map_av(alvol);
    if(detStore()->contains<StoredPhysVol>(key))
    {
     if(detStore()->retrieve(storedPV,key)==StatusCode::FAILURE)
     {
       ATH_MSG_INFO ("Unable to retrieve Stored PV " << key);
     }
    }
    
    if (!storedPV) {
      ATH_MSG_INFO ("no Volume for " << key);
      return false;
    }

    const GeoFullPhysVol* larTopVolLink  = storedPV->getPhysVol();
    
    // get the number of direct childs
    unsigned int larChilds  = larTopVolLink->getNChildVols();
    
    for (unsigned int ichild = 0; ichild<larChilds; ++ichild){
      
      bool stop = false;
      
      // Skip some some of the children : list will get longer in future
      //if ( alvol == CaloSubdetNames::LARCRYO_B && 
      //   larTopVolLink->getNameOfChildVol(ichild) == "Total LAR Volume") stop = true;
      
      if (!stop) {
	
	ATH_MSG_DEBUG ("CaloSubdetNames::ALIGNVOL " << (int) alvol
                       <<" Direct Child " << ichild << " -  with name : " 
                       << larTopVolLink->getNameOfChildVol(ichild));
	
	PVConstLink  childVolLink          = larTopVolLink->getChildVol(ichild);
	
	m_massFractions.clear();
	m_volumeFractions.clear();
	m_x0Fractions.clear();
	m_dEdXFractions.clear();
	m_averageAFractions.clear();
	m_averageZFractions.clear();
	
	this->blendGeoVolume(childVolLink,
			     m_volumeFractions,
			     m_massFractions,
			     m_x0Fractions,
			     m_dEdXFractions,
			     m_averageAFractions,
			     m_averageZFractions);
	
	this->averageFraction(m_volumeFractions,
			      m_massFractions,
			      m_x0Fractions,
			      m_dEdXFractions,
			      m_averageAFractions,
			      m_averageZFractions,
			      childMass, childVolume, 
			      childX0, childDeDx,
			      childAverageA, childAverageZ, childRho );
	
	m_child_x0Fractions.push_back( childX0 );
	m_child_dEdXFractions.push_back( childDeDx );
	m_child_averageAFractions.push_back( childAverageA );
	m_child_averageZFractions.push_back( childAverageZ );
	m_child_Rho.push_back( childRho );
	m_child_Volume.push_back( childVolume );
	m_child_Mass.push_back( childMass );
	
      }
    }
    
    // Now do the final average and store it :
    
    ATH_MSG_DEBUG ("Final average for  CaloSubdetNames::ALIGNVOL " << (int) alvol
                   << " !!!!! ");
    
    this->averageFraction(m_child_Volume, m_child_Mass,
			  m_child_x0Fractions,
			  m_child_dEdXFractions,
			  m_child_averageAFractions,
			  m_child_averageZFractions,
			  childMass, childVolume, 
			  childX0, childDeDx,
			  childAverageA, childAverageZ, childRho );
    
    mass =  childMass*(1./GeoModelKernelUnits::gram);
    x0 = childX0;
    dEdx = childDeDx;
    aveA = childAverageA;
    aveZ = childAverageZ;
  } 

  m_alignvol_number.push_back( alvol );
  m_alignvol_mass.push_back( mass );
  m_alignvol_x0.push_back( x0 );
  m_alignvol_dEdX.push_back( dEdx );
  m_alignvol_aveA.push_back( aveA );
  m_alignvol_aveZ.push_back( aveZ );

  return true;
}

bool
LArRecoMaterialTool::ScanEMB(CaloSubdetNames::ALIGNVOL alvol,
			      double& mass, double& x0, double& dEdx,
			      double& aveA, double& aveZ)
{
  mass =  0.;
  x0 = 0.;
  dEdx = 0.;
  aveA = 0.;
  aveZ = 0.;

  if ( alvol != CaloSubdetNames::EMB_POS && 
       alvol != CaloSubdetNames::EMB_NEG ) {
    ATH_MSG_INFO ("method ScanEMB does not support CaloSubdetNames::ALIGNVOL " << (int) alvol);
    return false;
  }

  // Shortcut : 
  if (!m_parseGeoModelForMaterial)
    {
      mass = 9.40439e+07 ;
      x0 = 93.9601;
      dEdx = 6.0789e-24;
      aveA = 98.5676;
      aveZ = 39.7406;      
    }
  else {

  // Here we go : 
    m_child_massFractions.clear();
    m_child_volumeFractions.clear();
    m_child_x0Fractions.clear();
    m_child_dEdXFractions.clear();
    m_child_averageAFractions.clear();
    m_child_averageZFractions.clear();
    m_child_Rho.clear();
    m_child_Volume.clear();
    m_child_Mass.clear();
    
    double       childVolume         = 0.;
    double       childMass           = 0.;
    double       childX0             = 0.;
    double       childDeDx           = 0.;
    double       childAverageA       = 0.;
    double       childAverageZ       = 0.;
    double       childRho            = 0.;
    
    StoredPhysVol* storedPV = 0;
    std::string key = map_av(alvol);
    if(detStore()->contains<StoredPhysVol>(key))
    {
     if(detStore()->retrieve(storedPV,key)==StatusCode::FAILURE)
     {
       ATH_MSG_INFO ("Unable to retrieve Stored PV " << key);
     }
    }
    
    if (!storedPV) {
      ATH_MSG_INFO ("no Volume for " << key);
      mass =  0.;
      x0 = 0.;
      dEdx = 0.;
      aveA = 0.;
      aveZ = 0.;
      return false;
    }

    const GeoFullPhysVol* larTopVolLink  = storedPV->getPhysVol();

    // get the number of direct childs
    unsigned int larChilds  = larTopVolLink->getNChildVols();
    
    for (unsigned int ichild = 0; ichild<larChilds; ++ichild){
      
      bool stop = false;
      
      // Skip some some of the children : list will get longer in future
      //if ( alvol == CaloSubdetNames::LARCRYO_B && 
      //	   larTopVolLink->getNameOfChildVol(ichild) == "Total LAR Volume") stop = true;
      
      if (!stop) {

        ATH_MSG_DEBUG ("CaloSubdetNames::ALIGNVOL " << (int) alvol
                       <<" Direct Child " << ichild << " -  with name : " 
                       << larTopVolLink->getNameOfChildVol(ichild));
	
	PVConstLink  childVolLink          = larTopVolLink->getChildVol(ichild);
	
	m_massFractions.clear();
	m_volumeFractions.clear();
	m_x0Fractions.clear();
	m_dEdXFractions.clear();
	m_averageAFractions.clear();
	m_averageZFractions.clear();
	
	this->blendGeoVolume(childVolLink,
			     m_volumeFractions,
			     m_massFractions,
			     m_x0Fractions,
			     m_dEdXFractions,
			     m_averageAFractions,
			     m_averageZFractions);
	
	this->averageFraction(m_volumeFractions,
			      m_massFractions,
			      m_x0Fractions,
			      m_dEdXFractions,
			      m_averageAFractions,
			      m_averageZFractions,
			      childMass, childVolume, 
			      childX0, childDeDx,
			      childAverageA, childAverageZ, childRho );
	
	m_child_x0Fractions.push_back( childX0 );
	m_child_dEdXFractions.push_back( childDeDx );
	m_child_averageAFractions.push_back( childAverageA );
	m_child_averageZFractions.push_back( childAverageZ );
	m_child_Rho.push_back( childRho );
	m_child_Volume.push_back( childVolume );
	m_child_Mass.push_back( childMass );
	
      }
    }
    
    // Now do the final average and store it :
    
    ATH_MSG_DEBUG ("Final average for  CaloSubdetNames::ALIGNVOL " << (int) alvol
                   << " !!!!! ");
    
    this->averageFraction(m_child_Volume, m_child_Mass,
			  m_child_x0Fractions,
			  m_child_dEdXFractions,
			  m_child_averageAFractions,
			  m_child_averageZFractions,
			  childMass, childVolume, 
			  childX0, childDeDx,
			  childAverageA, childAverageZ, childRho );
    
    mass =  childMass*(1./GeoModelKernelUnits::gram);
    x0 = childX0;
    dEdx = childDeDx;
    aveA = childAverageA;
    aveZ = childAverageZ;
  }
  
  m_alignvol_number.push_back( alvol );
  m_alignvol_mass.push_back( mass );
  m_alignvol_x0.push_back( x0 );
  m_alignvol_dEdX.push_back( dEdx );
  m_alignvol_aveA.push_back( aveA );
  m_alignvol_aveZ.push_back( aveZ );

  return true;
}

bool
LArRecoMaterialTool::ScanEMEC(CaloSubdetNames::ALIGNVOL alvol,
			      double& mass, double& x0, double& dEdx,
			      double& aveA, double& aveZ)
{
  mass =  0.;
  x0 = 0.;
  dEdx = 0.;
  aveA = 0.;
  aveZ = 0.;

  if ( alvol != CaloSubdetNames::EMEC_POS && alvol != CaloSubdetNames::EMEC_NEG ) {
    ATH_MSG_INFO ("method ScanEMEM does not support CaloSubdetNames::ALIGNVOL " << (int) alvol);
    return false;
  }

  // EMEC use a couple of custom volumes :
  // LAr::EMEC::OuterWheel::Absorber has volume=0 mm**3 and 5 elements
  // LAr::EMEC::OuterWheel::Electrode has volume=0 mm**3 and 5 elements
  // ==> with volume = 0 avarages cannot work !!! 
  // Take EMB's density for now ....
  mass = 9.40439e+07 ;
  x0 = 93.9601;
  dEdx = 6.0789e-24;
  aveA = 98.5676;
  aveZ = 39.7406;      
  

  m_alignvol_number.push_back( alvol );
  m_alignvol_mass.push_back( mass );
  m_alignvol_x0.push_back( x0 );
  m_alignvol_dEdX.push_back( dEdx );
  m_alignvol_aveA.push_back( aveA );
  m_alignvol_aveZ.push_back( aveZ );

  return true;
}

bool
LArRecoMaterialTool::ScanHEC(CaloSubdetNames::ALIGNVOL alvol,
			      double& mass, double& x0, double& dEdx,
			      double& aveA, double& aveZ)
{
  mass =  0.;
  x0 = 0.;
  dEdx = 0.;
  aveA = 0.;
  aveZ = 0.;

  if ( alvol != CaloSubdetNames::HEC1_POS && 
       alvol != CaloSubdetNames::HEC1_NEG &&
       alvol != CaloSubdetNames::HEC2_POS && 
       alvol != CaloSubdetNames::HEC2_NEG ) {
    ATH_MSG_INFO ("method ScanEMB does not support CaloSubdetNames::ALIGNVOL " 
                  << (int) alvol);
    return false;
  }

  // Shortcut : 
  if (!m_parseGeoModelForMaterial)
    {
      mass =  6.06935e+06;
      x0 = 91.1845;
      dEdx =  1.14141e-23;
      aveA = 58.9808;
      aveZ = 26.8777;      
    }
  else {

  // Here we go : 
    m_child_massFractions.clear();
    m_child_volumeFractions.clear();
    m_child_x0Fractions.clear();
    m_child_dEdXFractions.clear();
    m_child_averageAFractions.clear();
    m_child_averageZFractions.clear();
    m_child_Rho.clear();
    m_child_Volume.clear();
    m_child_Mass.clear();
    
    double       childVolume         = 0.;
    double       childMass           = 0.;
    double       childX0             = 0.;
    double       childDeDx           = 0.;
    double       childAverageA       = 0.;
    double       childAverageZ       = 0.;
    double       childRho            = 0.;
    
    StoredPhysVol* storedPV = 0;
    std::string key = map_av(alvol);
    if(detStore()->contains<StoredPhysVol>(key))
    {
     if(detStore()->retrieve(storedPV,key)==StatusCode::FAILURE)
     {
       ATH_MSG_INFO ("Unable to retrieve Stored PV " << key);
     }
    }
    
    if (!storedPV) {
      ATH_MSG_INFO ("no Volume for " << key);
      mass =  0.;
      x0 = 0.;
      dEdx = 0.;
      aveA = 0.;
      aveZ = 0.;
      return false;
    }

    const GeoFullPhysVol* larTopVolLink  = storedPV->getPhysVol();

    // get the number of direct childs
    unsigned int larChilds  = larTopVolLink->getNChildVols();
    
    for (unsigned int ichild = 0; ichild<larChilds; ++ichild){
      
      bool stop = false;
      
      // Skip some some of the children : list will get longer in future
      //if ( alvol == CaloSubdetNames::LARCRYO_B && 
      //	   larTopVolLink->getNameOfChildVol(ichild) == "Total LAR Volume") stop = true;
      
      if (!stop) {

        ATH_MSG_DEBUG ("CaloSubdetNames::ALIGNVOL " << (int) alvol
                       <<" Direct Child " << ichild << " -  with name : " 
                       << larTopVolLink->getNameOfChildVol(ichild));
	
	PVConstLink  childVolLink          = larTopVolLink->getChildVol(ichild);
	
	m_massFractions.clear();
	m_volumeFractions.clear();
	m_x0Fractions.clear();
	m_dEdXFractions.clear();
	m_averageAFractions.clear();
	m_averageZFractions.clear();
	
	this->blendGeoVolume(childVolLink,
			     m_volumeFractions,
			     m_massFractions,
			     m_x0Fractions,
			     m_dEdXFractions,
			     m_averageAFractions,
			     m_averageZFractions);
	
	this->averageFraction(m_volumeFractions,
			      m_massFractions,
			      m_x0Fractions,
			      m_dEdXFractions,
			      m_averageAFractions,
			      m_averageZFractions,
			      childMass, childVolume, 
			      childX0, childDeDx,
			      childAverageA, childAverageZ, childRho );
	
	m_child_x0Fractions.push_back( childX0 );
	m_child_dEdXFractions.push_back( childDeDx );
	m_child_averageAFractions.push_back( childAverageA );
	m_child_averageZFractions.push_back( childAverageZ );
	m_child_Rho.push_back( childRho );
	m_child_Volume.push_back( childVolume );
	m_child_Mass.push_back( childMass );
	
      }
    }
    
    // Now do the final average and store it :

    ATH_MSG_DEBUG ("Final average for  CaloSubdetNames::ALIGNVOL " << (int) alvol
                   << " !!!!! ");
    
    this->averageFraction(m_child_Volume, m_child_Mass,
			  m_child_x0Fractions,
			  m_child_dEdXFractions,
			  m_child_averageAFractions,
			  m_child_averageZFractions,
			  childMass, childVolume, 
			  childX0, childDeDx,
			  childAverageA, childAverageZ, childRho );
    
    mass =  childMass*(1./GeoModelKernelUnits::gram);
    x0 = childX0;
    dEdx = childDeDx;
    aveA = childAverageA;
    aveZ = childAverageZ;
  }
  
  m_alignvol_number.push_back( alvol );
  m_alignvol_mass.push_back( mass );
  m_alignvol_x0.push_back( x0 );
  m_alignvol_dEdX.push_back( dEdx );
  m_alignvol_aveA.push_back( aveA );
  m_alignvol_aveZ.push_back( aveZ );

  return true;
}

bool
LArRecoMaterialTool::ScanFCAL(CaloSubdetNames::ALIGNVOL alvol,
			      double& mass, double& x0, double& dEdx,
			      double& aveA, double& aveZ)
{
  mass =  0.;
  x0 = 0.;
  dEdx = 0.;
  aveA = 0.;
  aveZ = 0.;

  if ( alvol != CaloSubdetNames::FCAL1_POS && 
       alvol != CaloSubdetNames::FCAL1_NEG && 
       alvol != CaloSubdetNames::FCAL2_POS && 
       alvol != CaloSubdetNames::FCAL2_NEG && 
       alvol != CaloSubdetNames::FCAL3_POS && 
       alvol != CaloSubdetNames::FCAL3_NEG  ) {
    ATH_MSG_INFO ("method ScanEMB does not support CaloSubdetNames::ALIGNVOL " 
                  << (int) alvol);
    return false;
  }

  // Shortcut : 
  if (!m_parseGeoModelForMaterial)
    {
      if (alvol == CaloSubdetNames::FCAL1_POS || alvol == CaloSubdetNames::FCAL1_NEG) {
	mass = 47825.7;
	x0 = 127.627;
	dEdx = 4.36158e-24;
	aveA = 42.5374;
	aveZ = 19.2601; 
      } 
      else if (alvol == CaloSubdetNames::FCAL2_POS || alvol == CaloSubdetNames::FCAL2_NEG) {
	mass = 1.77759e+06;
	x0 = 81.6747;
	dEdx = 1.9157e-23;
	aveA = 159.937;
	aveZ = 64.5724; 
      }
      else if (alvol == CaloSubdetNames::FCAL3_POS || alvol == CaloSubdetNames::FCAL3_NEG) {
	mass = 1.80016e+06;
	x0 = 82.9447;
	dEdx = 1.86512e-23;
	aveA = 158.884;
	aveZ = 64.1669; 
      }

    }

  else {

  // Here we go : 
    m_child_massFractions.clear();
    m_child_volumeFractions.clear();
    m_child_x0Fractions.clear();
    m_child_dEdXFractions.clear();
    m_child_averageAFractions.clear();
    m_child_averageZFractions.clear();
    m_child_Rho.clear();
    m_child_Volume.clear();
    m_child_Mass.clear();
    
    double       childVolume         = 0.;
    double       childMass           = 0.;
    double       childX0             = 0.;
    double       childDeDx           = 0.;
    double       childAverageA       = 0.;
    double       childAverageZ       = 0.;
    double       childRho            = 0.;
    
    StoredPhysVol* storedPV = 0;
    std::string key = map_av(alvol);
    if(detStore()->contains<StoredPhysVol>(key))
    {
     if(detStore()->retrieve(storedPV,key)==StatusCode::FAILURE)
     {
       ATH_MSG_INFO ("Unable to retrieve Stored PV " << key);
     }
    }
    
    if (!storedPV) {
      ATH_MSG_INFO ("no Volume for " << key);
      mass =  0.;
      x0 = 0.;
      dEdx = 0.;
      aveA = 0.;
      aveZ = 0.;
      return false;
    }

    const GeoFullPhysVol* larTopVolLink  = storedPV->getPhysVol();
    
    // get the number of direct childs
    unsigned int larChilds  = larTopVolLink->getNChildVols();
    
    for (unsigned int ichild = 0; ichild<larChilds; ++ichild){
      
      bool stop = false;
      
      // Skip some some of the children : list will get longer in future
      //if ( alvol == CaloSubdetNames::LARCRYO_B && 
      //	   larTopVolLink->getNameOfChildVol(ichild) == "Total LAR Volume") stop = true;
      
      if (!stop) {

        ATH_MSG_DEBUG ("CaloSubdetNames::ALIGNVOL " << (int) alvol
                       <<" Direct Child " << ichild << " -  with name : " 
                       << larTopVolLink->getNameOfChildVol(ichild));
	
	PVConstLink  childVolLink          = larTopVolLink->getChildVol(ichild);
	
	m_massFractions.clear();
	m_volumeFractions.clear();
	m_x0Fractions.clear();
	m_dEdXFractions.clear();
	m_averageAFractions.clear();
	m_averageZFractions.clear();
	
	this->blendGeoVolume(childVolLink,
			     m_volumeFractions,
			     m_massFractions,
			     m_x0Fractions,
			     m_dEdXFractions,
			     m_averageAFractions,
			     m_averageZFractions);
	
	this->averageFraction(m_volumeFractions,
			      m_massFractions,
			      m_x0Fractions,
			      m_dEdXFractions,
			      m_averageAFractions,
			      m_averageZFractions,
			      childMass, childVolume, 
			      childX0, childDeDx,
			      childAverageA, childAverageZ, childRho );
	
	m_child_x0Fractions.push_back( childX0 );
	m_child_dEdXFractions.push_back( childDeDx );
	m_child_averageAFractions.push_back( childAverageA );
	m_child_averageZFractions.push_back( childAverageZ );
	m_child_Rho.push_back( childRho );
	m_child_Volume.push_back( childVolume );
	m_child_Mass.push_back( childMass );
	
      }
    }
    
    // Now do the final average and store it :

    ATH_MSG_DEBUG ("Final average for  CaloSubdetNames::ALIGNVOL " << (int) alvol
                   << " !!!!! ");
    
    this->averageFraction(m_child_Volume, m_child_Mass,
			  m_child_x0Fractions,
			  m_child_dEdXFractions,
			  m_child_averageAFractions,
			  m_child_averageZFractions,
			  childMass, childVolume, 
			  childX0, childDeDx,
			  childAverageA, childAverageZ, childRho );
    
    mass =  childMass*(1./GeoModelKernelUnits::gram);
    x0 = childX0;
    dEdx = childDeDx;
    aveA = childAverageA;
    aveZ = childAverageZ;
  }
  
  m_alignvol_number.push_back( alvol );
  m_alignvol_mass.push_back( mass );
  m_alignvol_x0.push_back( x0 );
  m_alignvol_dEdX.push_back( dEdx );
  m_alignvol_aveA.push_back( aveA );
  m_alignvol_aveZ.push_back( aveZ );

  return true;
}

void
LArRecoMaterialTool::blendGeoVolume(GeoPVConstLink& geoVolLink,
				    std::vector<double>& volume,
				    std::vector<double>& mass,
				    std::vector<double>& x0,
				    std::vector<double>& dEdX,
				    std::vector<double>& aveA,
				    std::vector<double>& aveZ) const
{

  const GeoLogVol* currentVol = geoVolLink->getLogVol();
  if (currentVol){
    this->addMaterialFraction(*currentVol, volume, mass, x0, dEdX, aveA, aveZ);
    unsigned int currentVolChilds = geoVolLink->getNChildVols();
    for (unsigned int ichild = 0; ichild < currentVolChilds; ichild++){
      GeoPVConstLink childVolumeLink = geoVolLink->getChildVol(ichild);
      this->blendGeoVolume(childVolumeLink, volume, mass, x0, dEdX, aveA, aveZ);
    } 
  }    
}

void 
LArRecoMaterialTool::addMaterialFraction(const GeoLogVol& geoVol,
					 std::vector<double>& volume,
					 std::vector<double>& mass,
					 std::vector<double>& x0,
					 std::vector<double>& dEdX,
					 std::vector<double>& aveA,
					 std::vector<double>& aveZ) const
{
   const GeoShape*     childShape    = geoVol.getShape();
   const GeoMaterial*  childMaterial = geoVol.getMaterial();
   
   double childVolume                = childShape->volume();

   double childNumberOfElements      = childMaterial->getNumElements();
   double childX0                    = childMaterial->getRadLength ();
   double childDeDx                  = childMaterial->getDeDxMin();
   double childRho                   = childMaterial->getDensity();

   ATH_MSG_DEBUG ("addMaterialFraction : " << geoVol.getName() 
                  << " has volume=" << childVolume << " mm**3 and " 
                  << childNumberOfElements << " elements ");
   
   for (unsigned int iEl=0; iEl<childNumberOfElements; iEl++){
     const GeoElement* geoEl = childMaterial->getElement(iEl);
     double fraction = childMaterial->getFraction(iEl);

     ATH_MSG_DEBUG ("         direct child : " << geoEl->getName() << " fraction = " << fraction 
                    << " A= " << geoEl->getA()*(1./GeoModelKernelUnits::gram) << " Z= " << geoEl->getZ());

     volume.push_back(fraction*childVolume);
     mass.push_back(fraction*childVolume*childRho);
     x0.push_back(fraction*childX0);
     dEdX.push_back(fraction*childDeDx);
     aveA.push_back(fraction*(geoEl->getA()*(1./GeoModelKernelUnits::gram)));
     aveZ.push_back(fraction*(geoEl->getZ()));             
   }
   
}

void 
LArRecoMaterialTool::averageFraction (std::vector<double>& volumeFractions, 
				      std::vector<double>& massFractions,
				      std::vector<double>& x0Fractions,
				      std::vector<double>& dEdXFractions,
				      std::vector<double>& averageAFractions,
				      std::vector<double>& averageZFractions,
				      double& childMass, double& childVolume, 
				      double& childX0, double& childDeDx,
				      double& childAverageA, double& childAverageZ, double& childRho )
{
  unsigned int parsedVolumes = massFractions.size();
  ATH_MSG_DEBUG (" averageFraction : will average over... " 
                 << parsedVolumes << " elements ");

  childMass = 0.;
  childVolume = 0.;
  childX0       = 0.;
  childDeDx     = 0.;
  childAverageA = 0.;
  childAverageZ = 0.;
  childRho      = 0.;

  for (unsigned int ivols = 0; ivols < parsedVolumes; ++ivols) {
    childMass    += massFractions[ivols];
    childVolume  += volumeFractions[ivols];
  }
  
  for (unsigned int ivolfrac = 0; ivolfrac < parsedVolumes; ++ivolfrac){
    double volfrac  = volumeFractions[ivolfrac]/childVolume;
    childX0        += volfrac * x0Fractions[ivolfrac];
    childDeDx      += volfrac * dEdXFractions[ivolfrac];
    double massfrac = massFractions[ivolfrac]/childMass;
    childAverageA  += massfrac * averageAFractions[ivolfrac];
    childAverageZ  += massfrac * averageZFractions[ivolfrac];
  }  
    
  childRho = childMass/(GeoModelKernelUnits::gram*childVolume);
  
  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("  + averaged over " << parsedVolumes << " volumes ");
  ATH_MSG_DEBUG ("    - volume   [mm^3]        : " << childVolume);
  ATH_MSG_DEBUG ("    - mass     [gram]        : " << childMass*(1./GeoModelKernelUnits::gram));
  ATH_MSG_DEBUG ("    - rho      [gram/mm^3]   : " << childRho);
  ATH_MSG_DEBUG ("    - X0                     : " << childX0);
  ATH_MSG_DEBUG ("    - DeDx                   : " << childDeDx);
  ATH_MSG_DEBUG ("    - AverageA               : " << childAverageA);
  ATH_MSG_DEBUG ("    - AverageZ               : " << childAverageZ);
  ATH_MSG_DEBUG ("");

}

std::string 
LArRecoMaterialTool::map_av(CaloSubdetNames::ALIGNVOL alvol)
{
  switch(alvol)
  {
  case CaloSubdetNames::LARCRYO_B:
    return std::string("LARCRYO_B");
  case CaloSubdetNames::LARCRYO_EC_POS:
    return std::string("LARCRYO_EC_POS");
  case CaloSubdetNames::LARCRYO_EC_NEG:
    return std::string("LARCRYO_EC_NEG");
  case CaloSubdetNames::SOLENOID:
    return std::string("SOLENOID");
  case CaloSubdetNames::PRESAMPLER_B_POS:
    return std::string("PRESAMPLER_B_POS");
  case CaloSubdetNames::PRESAMPLER_B_NEG:
    return std::string("PRESAMPLER_B_NEG");
  case CaloSubdetNames::EMB_POS:
    return std::string("EMB_POS");
  case CaloSubdetNames::EMB_NEG:
    return std::string("EMB_NEG");
  case CaloSubdetNames::PRESAMPLER_EC_POS:
    return std::string("PRESAMPLER_EC_POS");
  case CaloSubdetNames::PRESAMPLER_EC_NEG:
    return std::string("PRESAMPLER_EC_NEG");
  case CaloSubdetNames::EMEC_POS:
    return std::string("EMEC_POS");
  case CaloSubdetNames::EMEC_NEG:
    return std::string("EMEC_NEG");
  case CaloSubdetNames::HEC1_POS:
    return std::string("HEC1_POS");
  case CaloSubdetNames::HEC1_NEG:
    return std::string("HEC1_NEG");
  case CaloSubdetNames::HEC2_POS:
    return std::string("HEC2_POS");
  case CaloSubdetNames::HEC2_NEG:
    return std::string("HEC2_NEG");
  case CaloSubdetNames::FCAL1_POS:
    return std::string("FCAL1_POS");
  case CaloSubdetNames::FCAL1_NEG:
    return std::string("FCAL1_NEG");
  case CaloSubdetNames::FCAL2_POS:
    return std::string("FCAL2_POS");
  case CaloSubdetNames::FCAL2_NEG:
    return std::string("FCAL2_NEG");
  case CaloSubdetNames::FCAL3_POS:
    return std::string("FCAL3_POS");
  case CaloSubdetNames::FCAL3_NEG:
    return std::string("FCAL3_NEG");
  default:
    return std::string("");
  }
}
