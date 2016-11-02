/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArG3Escale_TDR.cxx
PACKAGE:  offline/LArCalorimeter/LArCellRec

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  Energy Scale Factor for G3 digits.

Updated: May 10, 2000    (SR, HM)
         Migrated to Athena Framework from PASO

Updated: Jan. 7, 2001    (SR)
         Adhere to QC standards (based on comments from S. Albrand)

********************************************************************/

// INCLUDE LAr header files:
#include "LArCellRec/LArG3Escale_TDR.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCell.h"
#include <iostream>
#include <math.h>


// Constants needed for computation of the Energy Scale:

    const double LArG3Escale_TDR::s_gap0 = 0.13;
    const double LArG3Escale_TDR::s_etacr = 2.5;
    const double LArG3Escale_TDR::s_ecdg_scale = 1.055;
    const double LArG3Escale_TDR::s_sfgin[4] = 
                 {1.1753, -0.44802, 0.16489, -0.00994};
    const double LArG3Escale_TDR::s_sfgout[4] = 
                 {1.1205, -0.32666, 0.26374, -0.02668};


// Constructor:

LArG3Escale_TDR::LArG3Escale_TDR(const std::string& type, 
				 const std::string& name, 
				 const IInterface* parent)
  : CaloCellCorrection(type, name, parent) ,m_OverallScale(1.000), m_FromHit(true)  ,m_emID(NULL),m_hecID(NULL),m_fcalID(NULL)

{ 

  // Name of Cell Container to be registered in TDS
  declareProperty("OverallScale",m_OverallScale);  

  declareProperty("FromHit",m_FromHit);
}


StatusCode LArG3Escale_TDR::initialize()
{
  ATH_MSG_INFO( name()  );
  ATH_MSG_INFO( " Properties: OverallScale= "<<m_OverallScale
                << " FromHit ="<<m_FromHit );

   const CaloIdManager* mgr = (CaloDetDescrManager::instance())->getCalo_Mgr();
   m_emID = mgr->getEM_ID();
   m_hecID = mgr->getHEC_ID();
   m_fcalID = mgr->getFCAL_ID();

   return StatusCode::SUCCESS; 

} 

// Desctructor

LArG3Escale_TDR::~LArG3Escale_TDR()
{  }


// MakeCorrection:  This is called with a pointer to the Cell Object.

void LArG3Escale_TDR::MakeCorrection(CaloCell* theCell)
{

  float eta = theCell->eta();
  Identifier id =  theCell->ID();
 
  double scale=1. ;


  if(m_emID->is_lar_em(id))                     // For EM Calorimeter:
  {
    int bar_ec = 0; 
    if(m_emID->is_em_endcap(id)) bar_ec=1; 
    int sam    = m_emID->sampling(id) ;
    double abseta = fabs(eta) ;

    if(sam == 0)       // PreSampler
    {

      if(bar_ec == 0)                           // Barrel Calorimeter
      { 
	if(abseta<0.8)                         
	{
	  scale = scaleb(abseta) * 3.2;
        }
	else if(abseta<1.4)                    
	{
	  scale = scaleb(abseta) * 3.7;
	}
	else 
	{
	  scale = scaleb(abseta)*((abseta-1.3)+1.)*(3.86+0.289*abseta);
	}
      }

      if(bar_ec == 1)                           // Endcap Calorimeter
      {
	double rr = 6.6 ; 
	if(abseta < 1.6) rr = 10.*(abseta-1.5)*6.6 ; 
	if(abseta > 1.7) rr = 6.                   ;
	scale = rr*scalee(abseta)                  ; 
      }

    }
    else                              // For Non-PS EM Calorimeters
    {
      if (bar_ec == 0) scale = scaleb(abseta) ;  // barrel 
      if (bar_ec == 1) scale = scalee(abseta) ;  // endcap 
    }

  }                                     // ENDIF for EM CALORIMETER


  else if( m_emID->is_tile(id))           // TILE CALORIMETER:
  { 
    // don't know how to do this yet.  need equiv of LArEM_ID.
  }


  else if( m_emID->is_lar_hec(id))            // HEC CALORIMETER
  {
    int sam    = m_hecID->sampling(id);
    if(sam == 0 || sam == 1)                // First Wheel
    {
      scale = 22.9;
    }
    else                                   // Second Wheel
    { 
      scale = 44.9;                          
    }
  }                                        // ENDIF for HEC CALORIMETER


  else if( m_emID->is_lar_fcal(id))              // FCAL:
  {
    int sam    = m_fcalID->module(id)-1;
    if(sam==0)                             // FCAL EM part
    {
      scale = 68.4;
    }
    else if (sam==1)                       // FCAL: First Hadronic part
    {
      scale = 134.4;
    }
    else                                   // FCAL: Second Hadronic part
    {
      scale = 103.9;
    }
  }                                       // ENDIF for FCAL

// Correct cell energy with scale

  float energy = theCell->energy();
 
  //m_FromHit=false;//A SUPPRIMER, juste pour les essais
 
  if (m_FromHit) setenergy(theCell, energy*scale*m_OverallScale);
  //if using LArCellBuilderFromLArHit

  else //FULL (DIGITIZATION) or if using LArCellBuilderFromLArHitWithNoise
  {    
    //    LArEpart2Edep conv;  
    //    float Epart2Edep=conv.get(id);  
    float Epart2Edep=1.;  
    setenergy(theCell, energy*scale*Epart2Edep);
  }
  
 
}  

///////////////////////////////////////////////////////////////////

// scaleb returns the scale factor for the EM barrel calorimeter (NOT PS):

double LArG3Escale_TDR::scaleb(double abseta)
{

  double scale ; 
  if(abseta<0.8) 
  {
    scale = 5.47 + 0.22*abseta; 
  }
  else  
  {
    scale = 4.48 + 0.28*abseta; 
  }

  return scale; 

}

///////////////////////////////////////////////////////////////////

// scalee returns the scale factor for the EM endcap calorimeter (NOT PS):

double LArG3Escale_TDR::scalee(double abseta)
{
  double scale; 
  double corr; 
  
  if(abseta > s_etacr)                // Inner Wheel
  {
    double sfg13 = s_sfgin[0] + s_sfgin[1]*abseta +
      s_sfgin[2]*abseta*abseta + s_sfgin[3]*abseta*abseta*abseta;
    corr = 0.8589 + 0.06758*abseta ; 

    scale = (1.-(abseta-1.8)*0.04)/(sfg13*s_ecdg_scale*pow(s_gap0,1.3))*corr;
  }

  else 
  {
    double sfg13 = s_sfgout[0] + s_sfgout[1]*abseta + 
      s_sfgout[2]*abseta*abseta + s_sfgout[3]*abseta*abseta*abseta;
         
    if(abseta < 1.56) 
    {
      corr = 1.;
    }

    else if( 1.56 < abseta &&  abseta < 1.59) 
    {
      corr = 1.6885 - 0.44170*abseta;
    }

    else if(1.59 < abseta && abseta < 1.70) 
    {
      corr = 0.9857;
    }

    else if(1.7 < abseta && abseta < 1.85) 
    {
      corr = 0.70978 + 0.1626*abseta; 
    }

    else if(1.85 < abseta && abseta < 2.1)  
    {
      corr = 1.01059;
    }

    else                  
    {
      corr = 0.4286 + 0.555*abseta - 0.13204*abseta*abseta;
    }

    scale = (1.-(abseta-1.8)*0.04)/(sfg13*s_ecdg_scale*pow(s_gap0,1.3))*corr;
    
  }

  return scale; 
}






