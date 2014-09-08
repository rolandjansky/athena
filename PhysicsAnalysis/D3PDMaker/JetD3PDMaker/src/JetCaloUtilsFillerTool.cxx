/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetCaloUtilsFillerTool.cxx
 * @author Michele Petteni < mpetteni AT cern.ch > 
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date November, 2009
 * @brief Block filler tool for Jet Timings
 */
	
#include "JetCaloUtilsFillerTool.h"
#include "JetEvent/Jet.h"
//#include "JetUtils/JetCaloQualityUtils.h"
//#include "JetUtils/JetCaloHelper.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"
#include <iostream>


namespace {


double hecF(const Jet* jet)
{
  double e_hec =
    jet->getShape("energy_"+CaloSampling::getSamplingName(CaloSampling::HEC0)) +
    jet->getShape("energy_"+CaloSampling::getSamplingName(CaloSampling::HEC1)) +
    jet->getShape("energy_"+CaloSampling::getSamplingName(CaloSampling::HEC2)) +
    jet->getShape("energy_"+CaloSampling::getSamplingName(CaloSampling::HEC3));
  
  double e_jet = jet->getRawE();
  
  if(e_jet!=0) return  e_hec/e_jet;
  else return -999;
}

double tileGap3F(const Jet* jet)
{
  double e_tileGap3 =
    jet->getShape("energy_"+CaloSampling::getSamplingName(CaloSampling::TileGap3));
  double e_jet = jet->getRawE();

  if(e_jet!=0) return  e_tileGap3/e_jet;
  else return -999;
}


double jetEMFraction(const Jet * jet)
{
  std::string base="energy_";
  unsigned int nsamp = CaloSampling::getNumberOfSamplings();
  double e_HAD = 0;
  double e_EM = 0;

  for (unsigned int i=0; i < nsamp; i++) {
    double e = jet->getShape(base + CaloSampling::getSamplingName(i));
    if (CaloSamplingHelper::isEMSampling ((CaloSampling::CaloSample)(i)))
      e_EM += e;
    else if (CaloSamplingHelper::isHADSampling ((CaloSampling::CaloSample)(i)))
      e_HAD += e;
  }
  
  if( (e_EM==0) || ((e_EM+e_HAD)==0) ) return 0.;
  return (e_EM / (e_EM+e_HAD));
}



double fracSamplingMax(const Jet* jet, int& SamplingMax)
{
  std::string base="energy_";
  double fracSamplingMax=-999999999.;    
  double sumE_samplings=0.;
  unsigned int nsamp = CaloSampling::getNumberOfSamplings();
  for ( unsigned int i = 0; i < nsamp; i++)
    {
      double e = jet->getShape(base+ CaloSampling::getSamplingName(i));
      sumE_samplings+=e;
      if (e>fracSamplingMax)
	{
	  fracSamplingMax=e;
	  SamplingMax=i;
	}
    }

  if(sumE_samplings!=0)
    fracSamplingMax/=sumE_samplings;
  else fracSamplingMax=0;
  
  return fracSamplingMax;
}


bool isUgly(const Jet* jet)
{

  double fcor = jet->getMoment("BCH_CORR_CELL");
  double tileGap3f = tileGap3F(jet);

  if (fcor>0.5) return true;
  if (tileGap3f >0.5) return true;

  return false;
}


}




namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetCaloUtilsFillerTool::JetCaloUtilsFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
  {
    // Extra properties for Signal States
    book().ignore(); // Avoid coverity warnings.
  }
  

  /**
   * @brief Book variables for this block.
   */
  StatusCode JetCaloUtilsFillerTool::book()
  {
    
    
    CHECK( addVariable ("hecf"       , m_hecf));
    CHECK( addVariable ("tgap3f"     , m_tileGap3f));
    CHECK( addVariable ("isUgly"     , m_isUgly     ));
    CHECK( addVariable ("isBadLooseMinus" , m_isBadLooseMinus ));
    CHECK( addVariable ("isBadLoose" , m_isBadLoose ));
    CHECK( addVariable ("isBadMedium" , m_isBadMedium ));
    CHECK( addVariable ("isBadTight" , m_isBadTight ));

		return StatusCode::SUCCESS;
  }
  
  
  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  
  StatusCode JetCaloUtilsFillerTool::fill(const Jet& jet)
  {
    // Protection against infinite recursion:
    // reconstruction can produce jets with themselves
    // as constituents.
    if (jet.begin() != jet.end()) {
      const Jet* j1 = dynamic_cast<const Jet*>(*jet.begin());
      if (j1 && j1->begin() != j1->end()) {
	const INavigable* j2 =
	  dynamic_cast<const Jet*>(*j1->begin());
	if (j1 == j2) return StatusCode::SUCCESS;
      }
    }
    
    // FIXME: These should be put into userdata.
    *m_hecf      = hecF(&jet);
    *m_tileGap3f = tileGap3F(&jet);
    
    double quality = jet.getMoment("LArQuality");
    double HecQ    = jet.getMoment("HECQuality");
    double NegE    = jet.getMoment("NegativeE");
    double emf     = jetEMFraction(&jet);
    double hecf    = *m_hecf ;
    double time    = jet.getMoment("Timing"); 
    double pt      = jet.pt();
    double sumpt   = jet.getMoment("sumPtTrk_pv0_500MeV");
    if (sumpt == 0)
      sumpt        = jet.getMoment("sumPtTrk");
    double chf     = pt != 0 ? sumpt/pt : 0;
    
    double AvgLArQ  = jet.getMoment("AverageLArQF");
    double LArQmean = AvgLArQ/65535. ; 
    double em_eta   = jet.eta(P4SignalState::JETEMSCALE) ;
    
    int SamplingMax = CaloSampling::Unknown;
    double fmax     = fracSamplingMax(&jet, SamplingMax);
    
    *m_isUgly     = isUgly(&jet) ;
    *m_isBadLooseMinus = JetCaloUtilsFillerTool::isBad( LooseMinusBad , quality, NegE, emf, hecf, time, fmax, em_eta, chf, HecQ, LArQmean ) ;
    *m_isBadLoose = JetCaloUtilsFillerTool::isBad( LooseBad, quality, NegE, emf, hecf, time, fmax, em_eta, chf, HecQ, LArQmean ) ;
    *m_isBadMedium= JetCaloUtilsFillerTool::isBad(MediumBad, quality, NegE, emf, hecf, time, fmax, em_eta, chf, HecQ, LArQmean ) ;
    *m_isBadTight = JetCaloUtilsFillerTool::isBad( TightBad, quality, NegE, emf, hecf, time, fmax, em_eta, chf, HecQ, LArQmean ) ;

    
    
    return StatusCode::SUCCESS;
  }

  //
  bool JetCaloUtilsFillerTool::isBad(BadJetCategory criteria, 
				     double quality, double NegE,  
				     double emf,     double hecf, 
				     double time,    double fmax, 
				     double eta,     double chf ,
				     double HecQ,    double LArQmean )
  {

    if( criteria==LooseMinusBad || criteria==LooseBad || criteria==MediumBad || criteria==TightBad )
      {
	// HEC spike
	if( hecf>0.5 && fabs(HecQ)>0.5 && LArQmean>0.8)         return true;
	if( fabs(NegE)>60000./*MeV*/)                          return true;
	// EM coherent noise
	if( emf>0.95 && fabs(quality)>0.8 && LArQmean>0.8 && fabs(eta)<2.8 )   return true;
	// Cosmics and Beam background
	if( emf<0.05 && chf<0.05 && fabs(eta)<2. )             return true;
	if( emf<0.05 && fabs(eta)>2. )                         return true;
	if( fmax>0.99&&fabs(eta)<2)                            return true;
      }



    if( criteria==LooseBad || criteria==MediumBad || criteria==TightBad )
      {
	// HEC spike
	if( hecf>0.5 && fabs(HecQ)>0.5 )                       return true;
	// EM coherent noise
	if( emf>0.95 && fabs(quality)>0.8 && fabs(eta)<2.8 )   return true;
	// Cosmics and Beam background
	if( fabs(time)>25. )                                   return true;
      }
    
  
    if(criteria==MediumBad || criteria==TightBad)
      {
	// HEC spike
	if( hecf> 1-fabs(HecQ))                               return true; 
	// EM coherent noise
	if( emf>0.9 && fabs(quality)>0.8 && fabs(eta)<2.8 )   return true;
	// Cosmics and Beam background
	if( fabs(time)>10. )                                  return true;
	if( emf < 0.05 && chf < 0.1  && fabs(eta)<2. )        return true;
	if( emf > 0.95 && chf < 0.05 && fabs(eta)<2. )        return true;
      }
    
    if( criteria==TightBad)
      {
	// EM coherent noise
	if ( fabs(quality)>0.95 )                   return true ;
	if (emf>0.98 && fabs(quality)>0.05)         return true ;
	// Cosmics and Beam background
	if (emf<0.1 && chf < 0.2 && fabs(eta)<2.5 ) return true;
	if (emf>0.9 && chf < 0.1 && fabs(eta)<2.5 ) return true;
	if (chf<0.01 && fabs(eta)<2.5 )             return true;
	if (emf<0.1 && fabs(eta)>2.5 )              return true;


      }
    return false;
  }


} // namespace D3PD
