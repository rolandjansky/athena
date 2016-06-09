/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************
Filename : H1WeightToolCSC12Generic.cxx
Author   : Walter Lampl
Created  : Feb 2007

***********************************************************************/

#include "H1WeightToolCSC12Generic.h"
#include "CaloEvent/CaloCell.h"
//#include "CaloEvent/CaloSampling.h"

#include "CLHEP/Units/SystemOfUnits.h"
//#include "GaudiKernel/Service.h"
//#include "GaudiKernel/MsgStream.h"

#include <math.h>
#include <string>

using CLHEP::millimeter;
using CLHEP::GeV;

//////////////////////////////////////////////////////////////////////
// Constructor/destructor
//////////////////////////////////////////////////////////////////////

H1WeightToolCSC12Generic::H1WeightToolCSC12Generic(const std::string& name,
                                     const std::string& type,
                                     const IInterface* parent)
                  : AthAlgTool(name,type,parent)
{ 
  declareInterface<IHadronicCalibrationTool>(this);
  declareInterface<ICellWeightTool>(this);
  declareConstant("wtEMB0", m_wtEMB0);
  declareConstant("wtEME0", m_wtEME0);	
  declareConstant("wtEMB1", m_wtEMB1);	
  declareConstant("wtEMB2", m_wtEMB2);	
  declareConstant("wtEME1", m_wtEME1);	
  declareConstant("wtEME2", m_wtEME2);	
  declareConstant("wtTile1", m_wtTile1);
  declareConstant("wtTile2", m_wtTile2);
  declareConstant("wtHec1", m_wtHec1);	
  declareConstant("wtHec2", m_wtHec2);	
  declareConstant("wtFCal1", m_wtFCal1);
  declareConstant("wtFCal2", m_wtFCal2);
  declareConstant("wtGap", m_wtGap);
  declareConstant("wtScint", m_wtScint);
  declareConstant("wtCryo",m_wtCryo);//l1.21519;
  declareConstant("etaFit",m_etaFit);

  finish_ctor(); //From ToolWithConstantsMixin
}

H1WeightToolCSC12Generic::~H1WeightToolCSC12Generic()
{ }

StatusCode H1WeightToolCSC12Generic::initialize() {
  return  CaloRec::ToolWithConstantsMixin::initialize();
}


StatusCode H1WeightToolCSC12Generic::setProperty (const std::string& propname,
						  const std::string& value)
{
  StatusCode sc;
  sc=AlgTool::setProperty (propname, value);
  if (sc.isFailure())
    return sc;

  sc=CaloRec::ToolWithConstantsMixin::setProperty (propname, value);
  if (sc.isFailure())
    return sc;

  return StatusCode::SUCCESS;
}
StatusCode H1WeightToolCSC12Generic::setProperty (const Property& p)
{
  StatusCode sc;
  sc=AlgTool::setProperty (p);
  if (sc.isFailure())
    return sc;

  sc=CaloRec::ToolWithConstantsMixin::setProperty (p);
  if (sc.isFailure())
    return sc;

  return StatusCode::SUCCESS;
}



double H1WeightToolCSC12Generic::etCell(const CaloCell* thisCell, double weight) 
{

  double etcell = weight * thisCell->et() * wtCell(thisCell); 
  return etcell;

}


double H1WeightToolCSC12Generic::wtCell(const CaloCell* thisCell)
{




  const double mm3 = millimeter*millimeter*millimeter;
  const double log2 = 0.69314718056;
  const double inv_log2 = 1. / log2;
  
  int dim=m_wtEMB1.size(); //FIXME: Assume same size for all m_wtXXXXX

  // Process cell

  double ecell = thisCell->e();
  double etaa = fabs( thisCell->eta() );
  double vol = thisCell->caloDDE()->volume();
  const CaloSampling::CaloSample s = thisCell->caloDDE()->getSampling();//CaloSampling::getSampling( *thisCell );

  int iET=0;
  if( fabs(ecell) > 0 && vol > 0 ) {
    iET=static_cast<int>(log(fabs(ecell)/fabs(vol)*(mm3/GeV))*inv_log2+26);
    if(iET<0) iET=0;
    if(iET >= dim) iET = dim-1;
  } else {
    iET=-1;
  }

  double wt = 0;

  switch ( s ) {
  
    // Presampler
    case CaloSampling::PreSamplerB:
      wt = m_wtEMB0[0];
      break;

    case CaloSampling::PreSamplerE:
      wt = m_wtEME0[0];
      break;

    // First EM layer

    case CaloSampling::EMB1:
      wt = m_wtEMB0[1];
      break;

    case CaloSampling::EME1:
      wt = m_wtEME0[1];
      break;

 
    // EM Barrel
    case CaloSampling::EMB2:
    case CaloSampling::EMB3:
      if(iET >= 0 && etaa < 0.8) wt = m_wtEMB1[iET];
      if(iET >= 0 && etaa >= 0.8) wt = m_wtEMB2[iET];
      break;
  
    // EM Endcap
    case CaloSampling::EME2:
    case CaloSampling::EME3:
      if(iET >= 0 && etaa < 2.5) wt = m_wtEME1[iET];
      if(iET >= 0 && etaa >= 2.5) wt = m_wtEME2[iET];
      break;
  
    // Tile
    case CaloSampling::TileBar0:
    case CaloSampling::TileBar1:
    case CaloSampling::TileBar2:
      if(iET >= 0) wt = m_wtTile1[iET];
      break;
  
    case CaloSampling::TileExt0:
    case CaloSampling::TileExt1:
    case CaloSampling::TileExt2:
      if(iET >= 0) wt = m_wtTile2[iET];
      break;

    // Had endcap
    case CaloSampling::HEC0:
    case CaloSampling::HEC1:
    case CaloSampling::HEC2:
    case CaloSampling::HEC3:
      if(iET>=0 && etaa<2.5) wt = m_wtHec1[iET];
      if(iET>=0 && etaa>=2.5) wt = m_wtHec2[iET];
      break;
  
    // FCal
    case CaloSampling::FCAL0:
      if(iET >= 0) wt = m_wtFCal1[iET];
      break;

    case CaloSampling::FCAL1:
    case CaloSampling::FCAL2:
      if(iET >= 0) wt = m_wtFCal2[iET];
      break;
  
    // Gap calorimeter
    case CaloSampling::TileGap3:
      wt = m_wtGap;
      break;

    // Gap scintillators
    case CaloSampling::TileGap1:
    case CaloSampling::TileGap2:
      wt = m_wtScint;
      break;
        
    case CaloSampling::MINIFCAL0:  
    case CaloSampling::MINIFCAL1:  
    case CaloSampling::MINIFCAL2:  
    case CaloSampling::MINIFCAL3:  
    case CaloSampling::Unknown:
      break;

  } // end switch

  return wt;

}


double H1WeightToolCSC12Generic::etCryo(double etAccb3, double etTile1) 
{

  if( etAccb3*etTile1 > 0 ) {
    return wtCryo() * sqrt( etAccb3*etTile1 );
  } else {
    return 0;
  }

}

double H1WeightToolCSC12Generic::wtCryo() 
{
  return m_wtCryo;
}

// Jet Energy scale

double H1WeightToolCSC12Generic::jetScale(double e, double eta) 
{
  
  int nEtaBins=m_etaFit.size();
  if (nEtaBins==0) 
    return 1.0;

  int ieta = static_cast<int>(fabs(eta)*(1./0.10));
  if (ieta>=nEtaBins) ieta=nEtaBins-1;
  //if(ieta > 49) ieta = 49;
  double ptlog = log(fabs(e)/(cosh(eta)*GeV));
  if(ptlog < 2.3) ptlog = 2.3;
  if(ptlog > m_etaFit[ieta][4]) ptlog = m_etaFit[ieta][4];
  double func = m_etaFit[ieta][0] + m_etaFit[ieta][1]/ptlog
    + m_etaFit[ieta][2]/(ptlog*ptlog)
    + m_etaFit[ieta][3]/(ptlog*ptlog*ptlog);
  
  return 1./func;

}
