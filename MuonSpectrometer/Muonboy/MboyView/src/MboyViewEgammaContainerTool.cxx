/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"
#include "egammaEvent/egammaContainer.h"
#include "CaloIdentifier/CaloID.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
/////////////////////////////////////////////////////////
#include "MboyView/MboyViewEgammaContainerTool.h"

static const InterfaceID IID_IMboyViewEgammaContainerTool("MboyViewEgammaContainerTool", 1, 0);

const InterfaceID& MboyViewEgammaContainerTool::interfaceID()
{
  return IID_IMboyViewEgammaContainerTool;
}

MboyViewEgammaContainerTool::MboyViewEgammaContainerTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewEgammaContainerTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   


}

MboyViewEgammaContainerTool::~MboyViewEgammaContainerTool(){}

// Initialize
StatusCode MboyViewEgammaContainerTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewEgammaContainerTool::finalize(){return StatusCode::SUCCESS;}

size_t MboyViewEgammaContainerTool::get_id_value(Identifier const & id) const
{
    std::istringstream iss(id.getString());
    size_t value = 0;
    if (! (iss >> std::hex >> value))
    {
        ATH_MSG_WARNING("Could not parse calorimeter cell ID");
    }
    return value;
}

StatusCode MboyViewEgammaContainerTool::ViewEgammaContainer(
               std::string EgammaContainerLocation,
               std::string EgammaEMTrackMatchContainerName,
               std::ofstream* pOut,
               float& EtMin,
               float& EtaMin,
	       float& CaloMinimumE
){

//Access
  if (!(evtStore()->contains<egammaContainer>(EgammaContainerLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << EgammaContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  StatusCode sc = StatusCode::SUCCESS;

  const egammaContainer* pContainerEgamma ;
  sc = evtStore()->retrieve(pContainerEgamma, EgammaContainerLocation);
  if (sc.isFailure()) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( "pContainerEgamma not found at " << EgammaContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

//Dump
  float Zero = 0.;
  const CaloCluster* cluster;
  const EMTrackMatch* trkmatch; 

  for (egammaContainer::const_iterator egamma = pContainerEgamma->begin(); egamma!=pContainerEgamma->end(); ++egamma){
    int isEM = (*egamma)->isem();
    int Charge = 0;
    //     std::cout << " egamma :  isEM " << isEM << std::endl;
    //     (*egamma)->print();
    // Electromagnetic
    //(isEM & 0x7FF)==0 means all cuts except TRT
    if ( (isEM & 0x7FF)!=0 ) continue ;
    cluster = (*egamma)->cluster();
    if ( !cluster ) continue;
    // cut on Pt & eta
    if ( (fabs(cluster->eta()) < EtaMin) && 
	 (cluster->pt() > EtMin) &&
	 cluster ) {
      // Get associated Trk is any
      trkmatch = (*egamma)->detail<EMTrackMatch>(EgammaEMTrackMatchContainerName);
      float PosX = Zero;
      float PosY = Zero;
      float PosZ = Zero;
      float PX = Zero;
      float PY = Zero;
      float PZ = Zero;
      if ( trkmatch ) {
	if( (*egamma)->trackParticle()->charge() > 0 ) {
	  Charge = +1;
	} else if( (*egamma)->trackParticle()->charge() < 0 ) {
	  Charge = -1;
	} 
	const Trk::Track* pTrack    = (*egamma)->trackParticle()->originalTrack();
	const Trk::Perigee* pPerigee = pTrack->perigeeParameters();
	PosX    = (pPerigee->position()).x() ;
	PosY    = (pPerigee->position()).y() ;
	PosZ    = (pPerigee->position()).z() ;
	PX      = (pPerigee->momentum()).x() ;
	PY      = (pPerigee->momentum()).y() ;
	PZ      = (pPerigee->momentum()).z() ;
      }    

      // Write Out 
      *pOut << "EGA" << " "
	    << EgammaContainerLocation << " "
	    << std::setw(12)<<std::setprecision(3) <<PosX  << " "
	    << std::setw(12)<<std::setprecision(3) <<PosY  << " "
	    << std::setw(12)<<std::setprecision(3) <<PosZ  << " "
	    << std::setw(12)<<std::setprecision(3) <<PX       << " "
	    << std::setw(12)<<std::setprecision(3) <<PY      << " "
	    << std::setw(12)<<std::setprecision(3) <<PZ      << " "
	    << std::setw(12)<<std::setprecision(3) <<cluster->e()   << " "
	    << std::setw(12)<<std::setprecision(3) <<cluster->eta()    << " "
	    << std::setw(12)<<std::setprecision(3) <<cluster->phi()    << " "
	    << std::setw(3)<<std::setprecision(0) <<Charge  << " "
	    << std::endl;

      CaloCluster::cell_iterator f_cell = cluster->cell_begin();
      CaloCluster::cell_iterator l_cell  = cluster->cell_end();
      // count number of cells above minimum energy
      int  n_cell = 0;
      for ( ; f_cell!=l_cell; ++f_cell)
	{
	  // Apply Energy Threshold
	  float e  = (*f_cell)->energy() ; 
	  if ( e < CaloMinimumE ) continue;
	  ++n_cell;
	}
      *pOut 
	<< std::setw(10)<<n_cell  << " "
	<< std::endl;

//    int nlines = n_cell%10;
//       int nremain = n_cell - nlines*10;
      f_cell = cluster->cell_begin();
      n_cell = 0;
      for ( ; f_cell!=l_cell; ++f_cell)
	{
	  // Apply Energy Threshold
	  float e  = (*f_cell)->energy() ; 
	  if ( e < CaloMinimumE ) continue;
	  ++n_cell;
	  //	  const CaloCell* cell = (*f_cell) ;
	  Identifier id=(*f_cell)->caloDDE()->identify() ;
	  *pOut << std::setw(10)<< get_id_value(id)  << " ";
	  if ( n_cell%10 == 0 ) 
	    *pOut << std::endl;
	}
      *pOut << std::endl;
    }
  }
  
  return StatusCode::SUCCESS;
  
}

