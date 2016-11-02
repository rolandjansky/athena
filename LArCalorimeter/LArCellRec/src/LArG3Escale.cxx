/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArG3Escale.cxx
PACKAGE:  offline/LArCalorimeter/LArCellRec

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  Energy Scale Factor for G3 digits.

Updated: May 10, 2000    (SR, HM)
         Migrated to Athena Framework from PASO

Updated: Jan. 7, 2001    (SR)
         Adhere to QC standards (based on comments from S. Albrand)

Updated: Feb. 2003 (LC)
         New calibration for DC1 

Updated : Jul 2003 (DR)
           use m_emid instead of m_atlasID
********************************************************************/

// INCLUDE LAr header files:
#include "LArCellRec/LArG3Escale.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include <iostream>
#include <math.h>


// Constants needed for computation of the Energy Scale:
    
    const int LArG3Escale::s_nbinbar = 62;
    const int LArG3Escale::s_nbinend = 46;
    const double LArG3Escale::s_etaminbar = 0.;	
    const double LArG3Escale::s_etamaxbar = 1.55;
    const double LArG3Escale::s_etaminend = 1.35;	
    const double LArG3Escale::s_etamaxend = 2.5;	
    const double LArG3Escale::s_etamin = 0.;	
    const double LArG3Escale::s_etamax = 2.5;

    const double LArG3Escale::s_gap0 = 0.13;
    const double LArG3Escale::s_ecdg_scale = 1.055;
    const double LArG3Escale::s_sfgin[4] = 
                 {1.1753, -0.44802, 0.16489, -0.00994};

// em barrel scale factors

    const double LArG3Escale::s_emscale_bar[62] =
        { 
           5.3383856, 5.3434567, 5.3485279, 5.3535995, 5.3726468,
           5.3807988, 5.3841310, 5.3859582, 5.3718100, 5.3745236,
           5.3716364, 5.3909812, 5.3916497, 5.3910971, 5.3973241,
           5.4070711, 5.4016976, 5.4157686, 5.4467711, 5.4325094,
           5.4449029, 5.4443102, 5.4547639, 5.4576511, 5.4680963,
           5.4620037, 5.4754019, 5.4844403, 5.4826965, 5.4976768,
           5.4856224, 5.4854326, 4.7246713, 4.7319145, 4.7392358,
           4.7412691, 4.7423482, 4.7656946, 4.7722678, 4.7809372,
           4.7901206, 4.7901669, 4.8078542, 4.8040199, 4.8220425,
           4.8334398, 4.8404350, 4.8450317, 4.8588505, 4.8738647,
           4.8901429, 4.9027538, 4.9293861, 5.0080881, 4.9099035,
       	   4.9186210, 4.9273386, 4.9360566, 4.9447741, 4.9534916,
           4.9622097, 4.97092724,
        };

// em endcap scale factors

   const double LArG3Escale::s_emscale_end[46] =
        {  
          11.9396935,
          11.8774414,11.8151903,11.7529392,11.6906872,11.6284361, 
          11.5661840,11.5039330,11.4416819,11.3794298,11.3500156,
          11.2489481,11.1728344,11.1127405,11.0528755,11.0059071,
          10.9656696,10.8858118,10.7092981,10.6296921,10.5661554,
          10.5074797,10.4930916,10.3630590,10.3193207,10.2439079,
          10.1953955,10.1040621,10.0342159, 9.9444113, 9.8400021,
           9.7621927, 9.6465502, 9.6191769, 9.5111284, 9.4343185,
           9.3129807, 9.1882210, 9.0866137, 9.0436354, 8.9812555,
           8.8506536, 8.7930737, 8.6987019, 8.6032343, 8.5066729, 
        };

// weights for the presampler,strips,middle and back
	
   const double LArG3Escale::m_table_p[4][s_numEtaBins] = {
     {
       2.159336, 2.159365, 2.159426, 2.159524, 2.185450, 1.784150,
       1.468300, 1.577280, 2.357230, 2.143870, 2.429050, 2.128170,
       1.711000, 2.063040, 2.188510, 2.285910, 2.375140, 2.358500,
       1.631560, 2.199280, 1.992240, 2.426500, 2.317250, 2.492310,
       2.537180, 2.754830, 2.606900, 2.742630, 2.821300, 2.660760,
       3.231350, 4.261270, 4.440460, 4.739960, 3.802220, 3.971200,
       3.873020, 3.780380, 3.719170, 3.674910, 3.645370, 3.638160,
       3.656870, 3.648160, 3.543830, 3.654490, 3.692520, 3.761490,
       3.697630, 3.649310, 3.745320, 3.657730, 3.586540, 3.437779,
       3.827440, 3.968080, 3.825220, 3.928960, 3.870620, 3.582299,
       4.707510, 4.172850, 2.602250, 2.750780, 6.006220, 6.913410,
       6.518250, 6.634570, 6.406190, 5.479100, 4.903650, 4.953980,
       0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
       0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
       0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
       0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
       0.000000, 0.000000, 0.000000, 0.000000
     },
     {
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.088920, 1.090670, 1.080880, 1.058810, 1.041350, 1.040150,
       1.024850, 1.027870, 1.008870, 1.013690, 1.010470, 0.997779,
       1.011620, 1.010720, 1.018570, 0.995325, 0.998770, 0.991472,
       1.012050, 1.022670, 1.047580, 1.033470, 1.009190, 1.030510,
       1.036582, 1.040966, 1.045282, 1.049457
     },
     {
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,

// Note that the following non 1 weights are meaningless (not used!!)

       0.300938, 0.295500, 0.257021, 0.285664, 0.279562, 0.243856,
       0.119355, 0.116621, 0.113299, 0.079486, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
       1.000000, 1.000000, 1.000000, 1.000000
     },
    {
       3.293558, 3.287675, 3.281793, 3.275910, 3.122819, 2.996900,
       2.985870, 2.954540, 3.194080, 3.272640, 3.433830, 3.002840,
       3.228300, 3.456140, 3.335890, 3.152420, 3.574670, 2.849230,
       2.002860, 2.182840, 1.954540, 1.986730, 1.908350, 1.657820,
       1.623290, 1.662690, 1.510320, 1.375820, 1.419890, 1.220120,
       0.662087, 0.782405, 0.755827, 2.191330, 1.790190, 1.771680,
       1.814130, 1.391820, 1.303030, 1.242170, 1.168910, 1.255750,
       1.038260, 1.120300, 1.074110, 0.993337, 1.040780, 0.977912,
       0.984398, 0.982676, 0.884917, 1.120490, 1.284270, 0.154091,
       2.055120, 1.479640, 1.299350, 1.048460, 0.974970, 0.894497,
       1.000000, 1.000000, 1.000000, 1.000000, 1.310010, 1.264220,
       1.388090, 1.183090, 1.415110, 1.267950, 1.224720, 1.299080,
       1.433390, 1.361880, 1.460400, 1.464050, 1.275200, 1.449930,
       1.321960, 1.281420, 1.228370, 1.230360, 1.147990, 1.136030,
       1.144620, 1.159600, 1.285750, 1.118030, 1.120420, 1.119130,
       1.091780, 1.247480, 1.327550, 1.106430, 1.099030, 1.150530,
       1.085429, 1.069589, 1.053206, 1.036276
     }
  };

// Constructor:

LArG3Escale::LArG3Escale(const std::string& type, const std::string& name, 
			 const IInterface* parent)
  : CaloCellCorrection(type, name, parent) , 
    m_Weights(true),
    m_OverallScale(1.000) ,
    m_emID(NULL),
    m_hecID(NULL),
    m_fcalID(NULL),
    m_WorkMode(1)
{ 
  declareProperty("Weights",m_Weights); 
  declareProperty("WorkMode",m_WorkMode); 
}


StatusCode LArG3Escale::initialize()
{
  ATH_MSG_INFO( name()  );
  if(m_Weights) ATH_MSG_INFO( " Properties: Weights= true" );
  else          ATH_MSG_INFO( " Properties: Weights= false" );
  ATH_MSG_INFO( " Properties: OverallScale= "<<m_OverallScale );

   // pointer to DD manager and helpers:
   const CaloIdManager* mgr = (CaloDetDescrManager::instance())->getCalo_Mgr();
   m_emID = mgr->getEM_ID();
   m_hecID = mgr->getHEC_ID();
   m_fcalID = mgr->getFCAL_ID();

   return StatusCode::SUCCESS; 
} 

// Desctructor

LArG3Escale::~LArG3Escale()
{  }

// MakeCorrection:  This is called with a pointer to the Cell Object.

void LArG3Escale::MakeCorrection(CaloCell* theCell)
{
  double energy = theCell->energy();
  double scale = this->correction(theCell->caloDDE());
  setenergy(theCell, energy*scale);
}  


// MakeCorrection:  This is called with a pointer to the Cell Object.

double  LArG3Escale::correction(const CaloDetDescrElement* theCaloDDE)
{

  float eta = theCaloDDE->eta();
  Identifier id =  theCaloDDE->identify();
 
  double scale=1. ;

  if(m_emID->is_lar_em(id))                     // For EM Calorimeter:
  {
    int bar_ec = 0; 
    if(m_emID->is_em_endcap(id)) bar_ec=1; 
    int sampling = m_emID->sampling(id) ;
    double abseta = fabs(eta) ;

    if(bar_ec == 1){
      sampling += 4;
    }

    scale = LArScale(bar_ec,abseta);    
    if(m_Weights) scale = scale * GetWgt(sampling,abseta);
 
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
  else {
    std::cout << "LARG3ESCALE WARNING unknown identifier" << std::endl;
    return 0.;
    
  }
 
 
// Correct cell energy with scale
  
  if(m_WorkMode==1 && !m_emID->is_lar_em(id))
    return 1.;
  else 
    return 1.;
  
}  



////////////////////////////////////////////////////////////////////
// larscale returns the scale factor for barrel and endcap
////////////////////////////////////////////////////////////////////

double LArG3Escale::LArScale(int bar_ec, double abseta)
{

 double scale = 0;

 if (bar_ec == 0)               // scale factors for barrel cells
    {
       if (abseta < s_etamaxbar) 
       {    
         const double granularity = (s_etamaxbar - s_etaminbar)/s_nbinbar;
         int ind = (int)((abseta - s_etaminbar)/granularity);
         scale = s_emscale_bar[ind];
       }
    }    
    else if (bar_ec == 1)          // scale factors for endcap cells        
    {
       if (abseta < s_etamaxend)
       {
         const double granularity = (s_etamaxend - s_etaminend)/s_nbinend;
         int ind = (int)((abseta - s_etaminend)/granularity);
         scale = s_emscale_end[ind]; 
       }
       else 
       {
           scale = scalee(abseta) ;  // inner wheel 
       }
    }
    return scale;
}
///////////////////////////////////////////////////////////////////
// scalee returns the scale factor for the Inner Wheel of em calo
///////////////////////////////////////////////////////////////////

double LArG3Escale::scalee(double abseta)
{
  double scale; 
  double corr; 
  
  double sfg13 = s_sfgin[0] + s_sfgin[1]*abseta +
      s_sfgin[2]*abseta*abseta + s_sfgin[3]*abseta*abseta*abseta;
  corr = 0.8589 + 0.06758*abseta ; 

  scale = (1.-(abseta-1.8)*0.04)/(sfg13*s_ecdg_scale*pow(s_gap0,1.3))*corr;

  return scale; 
}

///////////////////////////////////////////////////////////////////
// GetWgt returns the weight for cells
///////////////////////////////////////////////////////////////////

// For  eta  < 1.35 and eta > 1.6, the weights are 
// presampler, strips, middle section and back 
// his global calibration should be applied to LArApplyWgts
// 1.35 < n < 1.6 w_presampler (ec+barrel), Weight for the 
// strips + middle + back barrel, Weight for the scintillator,
// W_ec (strips + middle + back) endcap 

/* This is the table for the different weights....
there are four different bins in the eta region...
----------------------------------------------
Bin# |  Eta < 1.35  | 1.35<eta<1.6 | eta > 1.6
----------------------------------------------
   0 | Presampler   | Presampler   |  Presampler
----------------------------------------------
   1 | Strip        | Strips/Middle|  Strips
     |              | Back Barrel  |
----------------------------------------------
   2 | Middle       | Scintillator |  Middle
----------------------------------------------
   3 | Back         | Strips/Middle|  Back
     |              | Back Endcap  |
----------------------------------------------

Scintillator weight is not used here. Weighted gap scintillator 
energy is added at the cluster level in LArGapCorrection.

*/

double LArG3Escale::GetWgt(int layer,double aeta)
{
  if (aeta > s_etamax || aeta < s_etamin)
    return 1.;
  else
  {  
     const double granularity = (s_etamax - s_etamin)/s_numEtaBins;
     int ind = (int)((aeta - s_etamin)/granularity);
     if ( aeta < 1.35 || aeta > 1.6 ) 
     {
        return m_table_p[layer%4][ind] ;
     }
     else 
     {
        if ( layer%4 ==0 ) 
        {
           return m_table_p[0][ind];  // presampler EC + Barrrel
        }
        else 
        {
           if ( layer>0 && layer < 4) 
           {
              return m_table_p[1][ind]; // Weights strips+mid+back barrel
           }
           else
           {
              return m_table_p[3][ind]; // Weights endcap strips + back +barr$
           }
        }        
     }
  }
//    return m_table_p[0][0] ;

}




