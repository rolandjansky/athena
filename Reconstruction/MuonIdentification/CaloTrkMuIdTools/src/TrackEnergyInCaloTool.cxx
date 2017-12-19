/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTrkMuIdTools/TrackEnergyInCaloTool.h"

// --- Tracking ---
#include "TrkEventPrimitives/PropDirection.h"
// --- Calo ---
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"

///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool constructor
///////////////////////////////////////////////////////////////////////////////
TrackEnergyInCaloTool::TrackEnergyInCaloTool(const std::string& type, const std::string& name, const IInterface* pInterface) :
  AthAlgTool(type, name, pInterface),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_calosurf("CaloSurfaceBuilder")
{
  declareInterface<ITrackEnergyInCaloTool>(this);
  declareProperty("ExtrapolatorHandle", m_extrapolator);
  declareProperty("CaloSurfaceBuilder", m_calosurf);
}

///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool destructor
///////////////////////////////////////////////////////////////////////////////
TrackEnergyInCaloTool::~TrackEnergyInCaloTool(){}

///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::initialize()
///////////////////////////////////////////////////////////////////////////////
StatusCode TrackEnergyInCaloTool::initialize() {
    
  ATH_MSG_INFO("initialize() " << name());

  if ( AlgTool::initialize().isFailure() ) {
    ATH_MSG_ERROR("AlgTool couldn't be initialized!");
    return StatusCode::FAILURE;
  } 
  
  if ( detStore()->retrieve(m_calo_dd).isFailure() ) {
    ATH_MSG_WARNING(" Unable to retrieve CaloDetDescrManager from DetectorStore calling the CaloDetDescrManager::instance() method");
    m_calo_dd = CaloDetDescrManager::instance();
  }
  if ( !m_calo_dd ) {
    return StatusCode::FAILURE;
  }

  if ( m_extrapolator.retrieve().isFailure() )  {
    ATH_MSG_FATAL("Could not find Tool " << m_extrapolator.typeAndName() <<". Exiting.");
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG("Successfully created tool  " << m_extrapolator.typeAndName());
  }
  
  if( m_calosurf.retrieve().isFailure() ){
    ATH_MSG_FATAL("Could not find Tool " << m_calosurf);
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_INFO("Successfully created tool  "<< m_calosurf);
  }
  
  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::finalize()
///////////////////////////////////////////////////////////////////////////////
StatusCode  TrackEnergyInCaloTool::finalize() {

  if (AlgTool::finalize().isFailure()) {
    ATH_MSG_ERROR("AlgTool couldn't be finalized()!");
    return StatusCode::FAILURE;
  } 
  
  ATH_MSG_INFO("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::paramInSample
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackEnergyInCaloTool::paramInSample(const Trk::Track *trk,
								  const double offset,  
								  CaloCell_ID::CaloSample sample, 
								  const bool checkBoundary, 
								  Trk::ParticleHypothesis particleHypo) const
{

 ATH_MSG_DEBUG("in paramInSample()");

 Trk::Surface* surf = 0;
 double trketa = 0.;
 // Take eta of the last measured hit as best guess and create surface :
 const DataVector <const Trk::TrackParameters>* paramvec =  trk->trackParameters();
 if (paramvec) { 
   // Take eta of the last measured hit as best guess and create surface :
   DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
   --itEnd;
   trketa = (*itEnd)->position().eta();
    surf = m_calosurf->CreateUserSurface (sample,offset,trketa);
 } 
 const Trk::TrackParameters*  param = 0;
 if(surf){
   param = m_extrapolator->extrapolate(*trk, *surf,   
				       Trk::alongMomentum,
				       checkBoundary,
				       particleHypo); 
   
   if(param)
     ATH_MSG_DEBUG("Extrapolation succesful");
   else
     ATH_MSG_DEBUG("Extrapolation failed");
   
   delete surf; 
 }
 return param;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::paramInSample
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackEnergyInCaloTool::paramInSample(const Trk::TrackParameters *par, 
								  const double offset, 
								  const CaloCell_ID::CaloSample sample, 
								  const bool checkBoundary, 
								  Trk::ParticleHypothesis particleHypo) const
{

 ATH_MSG_DEBUG("in paramInSample()");
 
 const Trk::TrackParameters*  param = 0;
 if (par) {
   double trketa = par->eta();
   Trk::Surface* surf = m_calosurf->CreateUserSurface (sample,offset,trketa);
   if(surf){
     param = m_extrapolator->extrapolate(*par, *surf,
					 Trk::alongMomentum,
					 checkBoundary,
					 particleHypo); 
     if(param)
       ATH_MSG_DEBUG("Extrapolation succesful");
     else
       ATH_MSG_DEBUG("Extrapolation failed");
     
     delete surf; 
   }
 }
 return param;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::paramInNextSample
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackEnergyInCaloTool::paramInNextSample(const Trk::Track *trk,
								     const double offset,  
								     CaloCell_ID::CaloSample sample, 
								     const bool checkBoundary, 
								     Trk::ParticleHypothesis particleHypo) const
{
  double trketa = 0.;
  // Take eta of the last measured hit as best guess and create surface :
  const DataVector <const Trk::TrackParameters>* paramvec = trk->trackParameters();
  if (paramvec) {
    // Take eta of the last measured hit as best guess and create surface :
    DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
    --itEnd;
    trketa = (*itEnd)->eta();
  }

  ATH_MSG_DEBUG("in paramInNextSample()");

  CaloCell_ID::CaloSample next = getNextSample(sample, trketa);
  const Trk::TrackParameters*  param = paramInSample(trk,offset,next, checkBoundary,particleHypo );
  return param;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::paramInNextSample
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackEnergyInCaloTool::paramInNextSample(const Trk::TrackParameters *par, 
								     const double offset, 
								     const CaloCell_ID::CaloSample sample, 
								     const bool checkBoundary, 
								     Trk::ParticleHypothesis particleHypo) const
{

 ATH_MSG_DEBUG("in paramInNextSample()");
 
 double trketa = par->eta();
 CaloCell_ID::CaloSample next = getNextSample(sample, trketa);
 const Trk::TrackParameters* param = paramInSample(par,offset,next, checkBoundary,particleHypo );  
 return param;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::paramInLastSample
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackEnergyInCaloTool::paramInLastSample(const Trk::Track *trk, 
								     const double offset, 
								     const CaloCell_ID::CaloSample sample, 
								     Trk::ParticleHypothesis particleHypo) const
{

  ATH_MSG_DEBUG("in paramInLastSample()");

  const Trk::TrackParameters* param =0;
  if(sample != CaloCell_ID::TileBar2 && sample != CaloCell_ID::TileExt2 &&
    sample != CaloCell_ID::TileGap2 &&   sample != CaloCell_ID::TileGap3 
    && sample != CaloCell_ID::HEC3 )
   return param;

  Trk::Surface* surf = 0;
  double trketa = 0.;

  // Take eta of the last measured hit as best guess and create surface :
  const DataVector <const Trk::TrackParameters>* paramvec = trk->trackParameters();
  if (paramvec) { 
   // Take eta of the last measured hit as best guess and create surface :
   DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
   --itEnd;
   trketa = (*itEnd)->eta();
   surf = m_calosurf->CreateLastSurface(sample,offset,trketa);
  } 
  if(trk){
   if (surf){
     param  = m_extrapolator->extrapolate(*trk, *surf,
  				  Trk::alongMomentum,
  				  false,
  				  particleHypo);  
     if(param)
       ATH_MSG_DEBUG("Extrapolation succesful");
     else
       ATH_MSG_DEBUG("Extrapolation failed");
 
     delete surf;
   }
  } 
  return param;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::paramInLastSample
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackEnergyInCaloTool::paramInLastSample(const Trk::TrackParameters *par, 
								      const double offset, 
								      const CaloCell_ID::CaloSample sample, 
								      Trk::ParticleHypothesis particleHypo) const
{

 ATH_MSG_DEBUG("in paramInLastSample()");

 const Trk::TrackParameters* param =0;
 if(sample != CaloCell_ID::TileBar2 && sample != CaloCell_ID::TileExt2 &&
    sample != CaloCell_ID::TileGap2 &&   sample != CaloCell_ID::TileGap3 
    && sample != CaloCell_ID::HEC3 )
   return param;
 if(par){
   double trketa = par->eta();
   Trk::Surface* surf = m_calosurf->CreateLastSurface (sample,offset,trketa);
   if (surf){
     param = m_extrapolator->extrapolate(*par, *surf,
					 Trk::alongMomentum,
					 false,
					 particleHypo); 
     if(param)
       ATH_MSG_DEBUG("Extrapolation succesful");
     else
       ATH_MSG_DEBUG("Extrapolation failed");
     delete surf;  
   }
 }
   return param;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::paramAfterGirder
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackEnergyInCaloTool::paramAfterGirder(const Trk::Track *trk, 
								     Trk::ParticleHypothesis particleHypo)const
{

  ATH_MSG_DEBUG("in paramAfterGirder");
 Trk::Surface* surf = m_calosurf->CreateGirderSurface();
 const Trk::TrackParameters* param = m_extrapolator->extrapolate(*trk, *surf,
								 Trk::alongMomentum,
								 false,
								 particleHypo); 
 if(param)
    ATH_MSG_DEBUG("Extrapolation succesful");
 else
    ATH_MSG_DEBUG("Extrapolation failed");

 delete surf; 
 return param;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::paramAfterGirder
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackEnergyInCaloTool::paramAfterGirder(const Trk::TrackParameters *par, 
								    Trk::ParticleHypothesis particleHypo)const
{

  ATH_MSG_DEBUG("in paramAfterGirder");
  Trk::Surface* surf = m_calosurf->CreateGirderSurface();
  const Trk::TrackParameters* param = m_extrapolator->extrapolate(*par, *surf,
								  Trk::alongMomentum,
								  false,
								  particleHypo); 
 if(param)
    ATH_MSG_DEBUG("Extrapolation succesful");
 else
    ATH_MSG_DEBUG("Extrapolation failed");

  delete surf; 
  return param;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::paramInAllSamples
///////////////////////////////////////////////////////////////////////////////
std::vector < CaloPair > TrackEnergyInCaloTool::paramInAllSamples(const Trk::Track *trk,
									const double offset,
									const bool checkBoundary, 
									Trk::ParticleHypothesis particleHypo) const
{

  ATH_MSG_DEBUG("in paramInAllSamples");
  
  std::vector < CaloPair >  pairs;
  const Trk::TrackParameters*  param = 0;  
  Trk::Surface* surf = 0;
  CaloCell_ID::CaloSample sample = CaloCell_ID::Unknown;
  
  double eta = 0.;
  // Take eta of the last measured hit as best guess and create surface :
  const DataVector <const Trk::TrackParameters>* paramvec = trk->trackParameters();
  if (paramvec) { 
    // Take eta of the last measured hit as best guess and create surface :
    DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
    --itEnd;
    param = *itEnd;
    eta = param->eta();
  } 
  
  //Extrapolate first to the entrance of the EM Calorimeter
  int layer=0;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating track with eta:"<<eta<<" to EM sample: " <<sample);

  surf = m_calosurf->CreateUserSurface(sample,offset,eta);
  if (surf){
    const Trk::TrackParameters* par = paramInSample(trk,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }


  layer = 1;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to EM sample: " <<sample);


  surf = m_calosurf->CreateUserSurface(sample,offset,eta);
  if (surf){
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }


  layer = 2;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to EM sample: " <<sample);


  surf = m_calosurf->CreateUserSurface(sample,offset,eta);
  if (surf){
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }

  if ( eta > etamin(CaloCell_ID::HEC0, false) ){
    layer = 3;
    sample = getSample(eta, layer);
    ATH_MSG_DEBUG("Extrapolating to HEC0 sample: " <<sample);
    surf = m_calosurf->CreateUserSurface (sample,offset,eta);
    if (surf){
      const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
      if (par)
        param = par;
      delete surf;
      if (param){
        ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
        eta = param->eta();
        CaloPair mypair(param,sample);
        pairs.push_back( mypair );
      }
      else
        ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
    }
  }


  layer = 4;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to sample: " <<sample);
  surf = m_calosurf->CreateUserSurface (sample,offset,eta);
  if (surf){
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }

  layer = 5;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to sample: " <<sample);
  surf = m_calosurf->CreateUserSurface (sample,offset,eta);
  if (surf){
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }




  layer = 6;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to sample: " <<sample);
  surf = m_calosurf->CreateUserSurface (sample,offset,eta);
  if (surf){
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }



  //Get TrackParameters inLastSurface
  sample = getSample(eta, layer);
  if(sample == CaloCell_ID::TileBar2 || sample == CaloCell_ID::TileGap2 || sample == CaloCell_ID::TileGap3 || sample == CaloCell_ID::HEC3){
    ATH_MSG_DEBUG("Extrapolating to LastSurface ");
    surf = m_calosurf->CreateLastSurface (sample,offset,eta);
    if (surf){
      const Trk::TrackParameters* par = paramInLastSample(param,offset,sample, particleHypo );
      if (par)
        param = par;
      delete surf;
      if (param){
        ATH_MSG_DEBUG("Extrapolation succesful Last Suface succesfull");
        eta = param->eta();
        CaloPair mypair(param,CaloCell_ID::Unknown);
        pairs.push_back( mypair );
      }
      else
        ATH_MSG_DEBUG("Extrapolation succesful Last Suface failed");
    }
  }


  //Get TrackParameters after the girder in the barrel
  if ( eta < etamax(CaloCell_ID::TileExt2, true) ){
    ATH_MSG_DEBUG("Extrapolating to Girder ");
    const Trk::TrackParameters* par = paramAfterGirder(param,particleHypo);
    if (par)
      param = par;
    if(param){
      ATH_MSG_DEBUG("Extrapolation succesful suface After the Girder successful");
      CaloPair mypair(param,CaloCell_ID::Unknown);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation succesful suface After the Girder failed");
  }
  return pairs;
}


std::vector < CaloPair > TrackEnergyInCaloTool::paramInAllSamples(const Trk::TrackParameters *param, 
									const double offset,
									const bool checkBoundary, 
									Trk::ParticleHypothesis particleHypo) const
{

  ATH_MSG_DEBUG("in paramInAllSamples");
  
  std::vector< CaloPair >  pairs;
  Trk::Surface* surf  =  0;
  CaloCell_ID::CaloSample sample = CaloCell_ID::Unknown;
  double eta = param->eta();
 
  //Extrapolate first to the entrance of the EM Calorimeter
  int layer =0;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating track with eta:"<<eta<<" to EM sample: " <<sample);
  
  
  surf = m_calosurf->CreateUserSurface(sample,offset,eta);
  if (surf){  
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }

  layer = 1;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to EM sample: " <<sample);
  
  
  surf = m_calosurf->CreateUserSurface(sample,offset,eta);
  if (surf){  
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }

  
  layer = 2;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to EM sample: " <<sample);
  
  
  surf = m_calosurf->CreateUserSurface(sample,offset,eta);
  if (surf){    
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
        delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }

  
  
  if ( eta > etamin(CaloCell_ID::HEC0, false) ){
    layer = 3;
    sample = getSample(eta, layer);
    ATH_MSG_DEBUG("Extrapolating to HEC0 sample: " <<sample);
    surf = m_calosurf->CreateUserSurface (sample,offset,eta);
    if (surf){
      const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
      if (par)
	param = par;
      delete surf;
      if (param){
	ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
	eta = param->eta();
	CaloPair mypair(param,sample);
	pairs.push_back( mypair );
      }
      else
	ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
    }
  }
  
  
  layer = 4;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to sample: " <<sample);
  surf = m_calosurf->CreateUserSurface (sample,offset,eta); 
  if (surf){
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }

  
  
  layer = 5;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to sample: " <<sample);
  surf = m_calosurf->CreateUserSurface (sample,offset,eta);
  if (surf){ 
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }

  
  
  layer = 6;
  sample = getSample(eta, layer);
  ATH_MSG_DEBUG("Extrapolating to sample: " <<sample);
  surf = m_calosurf->CreateUserSurface (sample,offset,eta);
  if (surf){ 
    const Trk::TrackParameters* par = paramInSample(param,offset,sample, checkBoundary,particleHypo );
    if (par)
      param = par;
    delete surf;
    if (param){
      ATH_MSG_DEBUG("Extrapolation succesful in sample " <<sample);
      eta = param->eta();
      CaloPair mypair(param,sample);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation to sample "<<sample<<" failed");
  }

  
  
  //Get TrackParameters inLastSurface
  sample = getSample(eta, layer);
  if(sample == CaloCell_ID::TileBar2 || sample == CaloCell_ID::TileGap2 || sample == CaloCell_ID::TileGap3 || sample == CaloCell_ID::HEC3){
    ATH_MSG_DEBUG("Extrapolating to LastSurface ");
    surf = m_calosurf->CreateLastSurface (sample,offset,eta);
    if (surf){ 
      const Trk::TrackParameters* par = paramInLastSample(param,offset,sample, particleHypo );
      if (par)
	param = par;
      delete surf;
      if (param){
	ATH_MSG_DEBUG("Extrapolation succesful Last Suface succesfull");
	eta = param->eta();
	CaloPair mypair(param,CaloCell_ID::Unknown);
	pairs.push_back( mypair );
    }
      else
	ATH_MSG_DEBUG("Extrapolation succesful Last Suface failed");
    }
  }
  
  
  //Get TrackParameters after the girder in the barrel
  if ( eta < etamax(CaloCell_ID::TileExt2, true) ){
    ATH_MSG_DEBUG("Extrapolating to Girder ");
    const Trk::TrackParameters* par = paramAfterGirder(param,particleHypo);
    if (par)
      param = par;
    if(param){
      ATH_MSG_DEBUG("Extrapolation succesful suface After the Girder successful");
      CaloPair mypair(param,CaloCell_ID::Unknown);
      pairs.push_back( mypair );
    }
    else
      ATH_MSG_DEBUG("Extrapolation succesful suface After the Girder failed");
  }
  return pairs;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::etamax
///////////////////////////////////////////////////////////////////////////////
double TrackEnergyInCaloTool::etamax(const CaloCell_ID::CaloSample sample, const bool isTile) const
{
  double  etamax = 0; 

  for (const CaloDetDescriptor* descr :
         isTile ? m_calo_dd->tile_descriptors_range()
                : m_calo_dd->calo_descriptors_range())
  {
    if (descr) {
      if ( descr->getSampling(0) == sample){ 
	if( etamax  < descr->calo_eta_max() )  etamax = descr->calo_eta_max();
      }
    }
  }
  return etamax;
} 


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::etamin
///////////////////////////////////////////////////////////////////////////////
double TrackEnergyInCaloTool::etamin(const CaloCell_ID::CaloSample sample, const bool isTile) const
{
  double  etamin = 99999.; 
  for (const CaloDetDescriptor* descr :
         isTile ? m_calo_dd->tile_descriptors_range()
                : m_calo_dd->calo_descriptors_range())
  {
    if (descr) {
      if ( descr->getSampling(0) == sample){ 
	if( etamin  > descr->calo_eta_min() )  etamin = descr->calo_eta_min();
      }
    }
  }
  return etamin;
} 


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::energy
///////////////////////////////////////////////////////////////////////////////
double TrackEnergyInCaloTool::energy(const Trk::TrackParameters *param, 
					   Trk::ParticleHypothesis particleHypo) const 
{
  static Trk::ParticleMasses masses;
  double mass =masses.mass[particleHypo]; 
  if (param){
    double pX = param->momentum().x();
    double pY = param->momentum().y();
    double pZ = param->momentum().z();
    double energy =  sqrt( mass*mass + pX*pX + pY*pY + pZ*pZ );  
    return energy; 
  }
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::getNextSample
///////////////////////////////////////////////////////////////////////////////
CaloCell_ID::CaloSample TrackEnergyInCaloTool::getNextSample( const CaloCell_ID::CaloSample sample, double eta) const
{
  eta = fabs(eta);
  
  switch(sample){
    //Barrel
  case (CaloCell_ID::EMB1):
    if( eta < etamax(CaloCell_ID::EMB2, false) )
      return CaloCell_ID::EMB2;
    else if (  eta < etamax(CaloCell_ID::TileExt0, false) )
      return CaloCell_ID::TileExt0;
    break;
    
  case (CaloCell_ID::EMB2):
    if( eta < etamax(CaloCell_ID::EMB3, false) )
      return CaloCell_ID::EMB3;
    else if (  eta < etamax(CaloCell_ID::TileGap3, true) )
      return CaloCell_ID::TileGap3;
    else if (  eta < etamax(CaloCell_ID::TileExt0, true) )
      return CaloCell_ID::TileExt0;
    break;
  
  case (CaloCell_ID::EMB3):
    if( eta < etamax(CaloCell_ID::TileBar0, true) )
      return CaloCell_ID::TileBar0;
    else if( eta < etamax(CaloCell_ID::TileGap3, true) )
      return CaloCell_ID::TileGap3;
    else if( eta < etamax(CaloCell_ID::TileExt0, true) )
      return CaloCell_ID::TileExt0;
    break;

  case (CaloCell_ID::TileBar0):
    if( eta < etamax(CaloCell_ID::TileBar1, true) )
      return CaloCell_ID::TileBar1;
    else if( eta < etamax(CaloCell_ID::TileGap1, true) )
      return CaloCell_ID::TileGap1;
    else if( eta < etamax(CaloCell_ID::TileExt1, true) )
      return CaloCell_ID::TileExt1;
    break;
  
  case (CaloCell_ID::TileBar1):
    if( eta < etamax(CaloCell_ID::TileBar2, true) )
      return CaloCell_ID::TileBar2;
    else if( eta < etamax(CaloCell_ID::TileGap2, true) )
      return CaloCell_ID::TileGap3;
    else if( eta < etamax(CaloCell_ID::TileExt2, true) )
      return CaloCell_ID::TileExt2;
    break;
  
  case (CaloCell_ID::TileGap3 ):
    if( eta < etamax(CaloCell_ID::TileGap1, true) )
      return CaloCell_ID::TileGap3;
    else if( eta < etamax(CaloCell_ID::TileExt1, false) )
      return CaloCell_ID::TileExt1;
    break;
  
  case (CaloCell_ID::TileGap1):
    if( eta < etamax(CaloCell_ID::TileGap2, true) )
      return CaloCell_ID::TileGap3;
    else if( eta < etamax(CaloCell_ID::TileExt2, true) )
      return CaloCell_ID::TileExt2;
    break;

  case (CaloCell_ID::TileExt0 ):
    if( eta < etamax(CaloCell_ID::TileExt1, true) )
      return CaloCell_ID::TileExt1;
    break;
  
  case (CaloCell_ID::TileExt1):
    if( eta < etamax(CaloCell_ID::TileExt2, true) )
      return CaloCell_ID::TileExt2;
    break;
  
  
  //EndCap
  case (CaloCell_ID::EME1):
    if( eta > etamin(CaloCell_ID::EME2, false) )
      return CaloCell_ID::EME2;
    else if (  eta < etamax(CaloCell_ID::TileExt0, true) )
      return CaloCell_ID::TileExt0;
    break;  

  case (CaloCell_ID::EME2):
    if( eta > etamin(CaloCell_ID::EME3, false) )
      return CaloCell_ID::EME3;
    else if (  eta < etamax(CaloCell_ID::TileExt0, true) )
      return CaloCell_ID::TileExt0;
    break;

  case (CaloCell_ID::EME3):
    if( eta > etamin(CaloCell_ID::HEC0, false) )
      return CaloCell_ID::HEC0;
    else if (  eta < etamax(CaloCell_ID::TileExt0, true) )
      return CaloCell_ID::TileExt0;
    break;

  case (CaloCell_ID::HEC0):
    if( eta > etamin(CaloCell_ID::HEC1, false) )
      return CaloCell_ID::HEC1;
    else if (  eta < etamax(CaloCell_ID::TileExt0, true) )
      return CaloCell_ID::TileExt0;
    break;

  case (CaloCell_ID::HEC1):
    if( eta > etamin(CaloCell_ID::HEC2, false) )
      return CaloCell_ID::HEC2;
    else if (  eta < etamax(CaloCell_ID::TileExt0, true) )
      return CaloCell_ID::TileExt0;
    break;

  case (CaloCell_ID::HEC2):
    if( eta > etamin(CaloCell_ID::HEC3, false) )
      return CaloCell_ID::HEC3;
    else if (  eta < etamax(CaloCell_ID::TileExt0, true) )
      return CaloCell_ID::TileExt0;
    break;
  
  default:
    return CaloCell_ID::Unknown;
  }
  return CaloCell_ID::Unknown;
}


///////////////////////////////////////////////////////////////////////////////
// TrackEnergyInCaloTool::getSample
///////////////////////////////////////////////////////////////////////////////
CaloCell_ID::CaloSample TrackEnergyInCaloTool::getSample(double eta, const int layer) const
{
  CaloCell_ID::CaloSample sample =  CaloCell_ID::Unknown;
  eta = fabs(eta);
  switch (layer){
  case 0:  //EME
    if ( eta < etamax(CaloCell_ID::EMB1, false) )
      sample = CaloCell_ID::EMB1;
    else if ( eta > etamin(CaloCell_ID::EME1, false) )
      sample = CaloCell_ID::EME1;
    break;
    
  case 1:  //EME
    if ( eta < etamax(CaloCell_ID::EMB2, false) )
      sample = CaloCell_ID::EMB2;
    else if ( eta < etamax(CaloCell_ID::EME2, false) )
      sample = CaloCell_ID::EME2;
    break;
    
  case 2:  //EME
    if ( eta < etamax(CaloCell_ID::EMB3, false) )
      sample = CaloCell_ID::EMB3;
    else if ( eta < etamax(CaloCell_ID::EME3, false) )
      sample = CaloCell_ID::EME3;
    break;
    
  case 3: //HEC0
    if ( eta > etamin(CaloCell_ID::HEC0, false) )
      sample = CaloCell_ID::HEC0;
    break;
    
  case 4: //Last Layer-2
    if ( eta < etamax(CaloCell_ID::TileBar0, true) )
      sample = CaloCell_ID::TileBar0;
    else if( eta > etamax(CaloCell_ID::TileBar0, true) && eta <  etamin(CaloCell_ID::TileGap3, true) )
      ATH_MSG_DEBUG("track is pointing to the crack in between the TileBar0 and TileGap3");
    else if ( eta > etamin(CaloCell_ID::TileGap3, true) && eta < etamin(CaloCell_ID::TileExt0, true) )
      sample = CaloCell_ID::TileGap3;
    else if ( eta > etamin(CaloCell_ID::TileExt0, true) && eta < etamax(CaloCell_ID::TileExt0, true) )
      sample = CaloCell_ID::TileExt0;
    else if (  eta > etamax(CaloCell_ID::TileExt0, true) &&  eta < etamin(CaloCell_ID::HEC1, false) )
      ATH_MSG_DEBUG("track is pointing the transition between Barrel and Endcap, there's not Last Layer -2 here ");
    else if ( eta > etamin(CaloCell_ID::HEC1, false) )
      sample = CaloCell_ID::HEC1;
    break;
    
  case 5:  //Last Layer-1
    if ( eta < etamax(CaloCell_ID::TileBar1, true) )
      sample = CaloCell_ID::TileBar1;
    else if( eta > etamax(CaloCell_ID::TileBar1, true) && eta <  etamin(CaloCell_ID::TileGap1, true) )
      ATH_MSG_DEBUG("track is pointing to the crack in between the TileBar1 and TileGap1");
    else if ( eta > etamin(CaloCell_ID::TileGap1, true) && eta < etamin(CaloCell_ID::TileExt1, true) )
      sample = CaloCell_ID::TileGap1;
    else if ( eta > etamin(CaloCell_ID::TileExt1, true) && eta < etamax(CaloCell_ID::TileExt1, true) )
      sample = CaloCell_ID::TileExt1;
    else if (  eta > etamax(CaloCell_ID::TileExt1, true) &&  eta < etamin(CaloCell_ID::HEC2, false) )
      ATH_MSG_DEBUG("track is pointing the transition between Barrel and Endcap, there's not Last Layer-1 here ");
    else if ( eta > etamin(CaloCell_ID::HEC2, false) )
      sample = CaloCell_ID::HEC2;
    break;
    
    
  case 6:  //Last Layer
    if ( eta < etamax(CaloCell_ID::TileBar2, true) )
      sample = CaloCell_ID::TileBar2;
    else if( eta > etamax(CaloCell_ID::TileBar2, true) && eta <  etamin(CaloCell_ID::TileGap2, true) )
      ATH_MSG_DEBUG("track is pointing to the crack in between the TileBar2 and TileGap2");
    else if ( eta > etamin(CaloCell_ID::TileGap2, true) && eta < etamin(CaloCell_ID::TileExt2, true) )
      sample = CaloCell_ID::TileGap3;
    else if ( eta > etamin(CaloCell_ID::TileExt2, true) && eta < etamax(CaloCell_ID::TileExt2, true) )
      sample = CaloCell_ID::TileExt2;
    else if (  eta > etamax(CaloCell_ID::TileExt2, true) &&  eta < etamin(CaloCell_ID::HEC3, false) )
      ATH_MSG_DEBUG("track is pointing the transition between Barrel and Endcap, there's not Last Layer here ");
    else if ( eta > etamin(CaloCell_ID::HEC3, false) )
      sample = CaloCell_ID::HEC3;
    break;
  }
  return sample;
}

