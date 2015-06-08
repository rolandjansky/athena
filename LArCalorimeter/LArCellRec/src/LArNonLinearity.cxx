/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArNonLinearity.cxx
PACKAGE:  offline/LArCalorimeter/LArCellRec

AUTHORS:  G.Unal
CREATED:  August 2003

PURPOSE:  non linearity if only linear calibration fit is used
          (for EM barrel)

********************************************************************/

// INCLUDE LAr header files:
#include "LArCellRec/LArNonLinearity.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "CaloEvent/CaloCell.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
// Units
#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <math.h>

using CLHEP::GeV;

// Constants needed for computation of the Energy Scale:
    
    const double LArNonLinearity::m_etatrans = 0.8;

// index:  0 = front A, 1 = front B, 2=middble A, 3=middle B
//         4 = back A and 5 = back B  (A=eta<0.8 and B=eta>0.8)

// transitions energies

    const double LArNonLinearity::m_etrans[6] =
     {6.5*GeV, 6.0*GeV, 26.0*GeV, 50.0*GeV, 17.5*GeV, 25.0*GeV };

// maximum energy validity

    const double LArNonLinearity::m_emax[6] = 
      {60.*GeV, 60.*GeV, 260.*GeV, 500.*GeV, 170.*GeV, 250.*GeV};

// parameters of polynomial fit. These values give residuals in GeV
    const double LArNonLinearity::m_p0[6][2] =
      {{0.44e-2, 0.158}, {0.691e-3, 0.121},            // front
       {0.166e-1, 0.226}, {0.347e-1, 0.753},           // middle
       {0.869e-2, 0.269}, {0.422e-1, 0.3598}};         // back
    const double LArNonLinearity::m_p1[6][2] =
      {{-0.16e-2/GeV, -0.159e-1/GeV}, {+0.379e-2/GeV, -0.100e-1/GeV},     // front
       {-0.105e-2/GeV, -0.1559e-2/GeV}, {-0.605e-2/GeV, -0.452e-2/GeV},   // middle
       {+0.248e-2/GeV, -0.252e-2/GeV}, {-0.690e-2/GeV, -0.881e-2/GeV}}; // back
    const double LArNonLinearity::m_p2[6][2] =
      {{-0.45e-3/(GeV*GeV),+0.406e-3/(GeV*GeV)}, 
       {-0.252e-2/(GeV*GeV), +0.159e-3/(GeV*GeV)},    // front
       {-0.208e-3/(GeV*GeV),-0.263e-4/(GeV*GeV)}, 
       {-0.66e-3/(GeV*GeV), -0.146e-4/(GeV*GeV)},    // middle
       {-0.920e-3/(GeV*GeV),-0.645e-4/(GeV*GeV)}, 
       {+0.689e-4/(GeV*GeV), +0.4773e-4/(GeV*GeV)}}; // back
    const double LArNonLinearity::m_p3[6][2] =
      {{0.1037e-3/(GeV*GeV*GeV), -0.291e-5/(GeV*GeV*GeV)}, 
       {+0.332e-3/(GeV*GeV*GeV), -0.309e-7/(GeV*GeV*GeV)},    // front
       {+0.100e-4/(GeV*GeV*GeV), +0.1300e-6/(GeV*GeV*GeV)}, 
       {+0.115e-4/(GeV*GeV*GeV), +0.545e-7/(GeV*GeV*GeV)},  // middle
       {0.472e-4/(GeV*GeV*GeV), 0.459e-6/(GeV*GeV*GeV)}, 
       {0.759e-5/(GeV*GeV*GeV), -0.543e-7/(GeV*GeV*GeV)}};   // back

// Constructor:

LArNonLinearity::LArNonLinearity(const std::string& type, const std::string& name, 
			 const IInterface* parent)
  : CaloCellCorrection(type, name, parent),m_emID(NULL),m_hecID(NULL),m_fcalID(NULL)

{ 
  
}


StatusCode LArNonLinearity::initialize()
{
   MsgStream log(msgSvc(), name());

  StoreGateSvc* detStore;
  if (service("DetectorStore", detStore).isFailure()) {
    log << MSG::ERROR   << "Unable to access DetectoreStore" << endreq ;
    return StatusCode::FAILURE;
  }

  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    log << MSG::ERROR << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = detStore->regFcn(&IGeoModelSvc::geoInit,
			  geoModel,
			  &LArNonLinearity::geoInit,this);
    if(sc.isFailure())
    {
      log << MSG::ERROR << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return sc;
}

StatusCode
LArNonLinearity::geoInit(IOVSVC_CALLBACK_ARGS)
{
   MsgStream log(msgSvc(), name());

   log<<MSG::INFO<< name() << endreq;
   log<<MSG::INFO<< " Initialize LArNonLinearity " << endreq;

  StoreGateSvc* detStore;
  StatusCode sc = service ( "DetectorStore" , detStore ) ;
  if (sc.isFailure()) 
  {
    log << MSG::ERROR
           << "Unable to access pointer to DetectorStore"
           << endreq; 
    return StatusCode::FAILURE;
  }

   // pointer to DD manager and helpers:
  const DataHandle<CaloIdManager> caloIdMgr;
  sc = detStore->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    log << MSG::ERROR
           << "Unable to retrieve CaloIdManager from DetectoreStore"
           << endreq; 
    return StatusCode::FAILURE;
  }   

  m_emID = caloIdMgr->getEM_ID();
  m_hecID = caloIdMgr->getHEC_ID();
  m_fcalID = caloIdMgr->getFCAL_ID();

  return StatusCode::SUCCESS; 
} 

// Desctructor

LArNonLinearity::~LArNonLinearity()
{  }

// MakeCorrection:  This is called with a pointer to the Cell Object.

void LArNonLinearity::MakeCorrection(CaloCell* theCell)
{
  MsgStream log(msgSvc(), name());

  float eta = theCell->eta();
  Identifier id =  theCell->ID();
 
  double corr=0. ;                 // corr=residual to subtract to
                                   // true energy
  double energy = theCell->energy();

  if(m_emID->is_lar_em(id) &&
     m_emID->is_em_barrel(id) )     // only for barrel EM Calorimeter:
  {

    int sampling = m_emID->sampling(id);
    if (sampling >0 && sampling < 4)  // nothing for barrel PS
    {

// check A or B electrodes
      int ielec;
      if (fabs(eta)<m_etatrans) {ielec=0;}
      else {ielec=1;}
      int index =ielec + 2*(sampling-1);   // from 0 to 5 (see above)

      if (energy>2.*GeV) log << MSG::DEBUG  << " energy " << energy
                       << " sampling " << sampling
                       << " eta " << eta
                       << " index " << index << endreq;

// check E less than E maximum validity
 
      if (energy < m_emax[index] )
      {
// energy range (high gain or middle gain)
        int irange;
        if (energy < m_etrans[index] ) {irange=0;}
        else {irange=1;}

        if (irange==0)   
        {                        // ignore offset for high gain
         corr = m_p1[index][irange] *energy
               +m_p2[index][irange] *energy*energy
               +m_p3[index][irange] *energy*energy*energy;
        } else
        {
         corr = m_p0[index][irange] 
               +m_p1[index][irange] *energy
               +m_p2[index][irange] *energy*energy
               +m_p3[index][irange] *energy*energy*energy;
        }
// convert corr back to GEV
        corr = corr*GeV;

        if (energy>2.*GeV)
        {
        log << MSG::DEBUG  << " pol coeff " << m_p0[index][irange] << " "
            << m_p1[index][irange] << " " << m_p2[index][irange] << " "
            << m_p3[index][irange] << endreq;
        log << MSG::DEBUG << "energy,index,corr = " << energy << " " << index << " " << corr << endreq;
        }
      
      }
    }
  }                                        // ENDIF for barrel EM


// Correct cell energy 

  setenergy(theCell, energy-corr);
  
}  
