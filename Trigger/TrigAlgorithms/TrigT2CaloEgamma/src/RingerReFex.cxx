/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************
 * Author: Denis Damazio
 * Revision
 *    Joao Victor Pinto, jodafons@cern.ch
 *    werner freund,     wsfreund@cern.ch
 *
 * Implements the RingerFex.
 *******************************************************/

#include <vector>
#include <map>
#include <cmath>
#include <numeric>
#include "RingerReFex.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"
#include "TrigT2CaloCommon/IReAlgToolCalo.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "PhiComps.h"
using namespace Monitored;


//!=================================================================================

RingerReFex::RingerReFex(const std::string& type, const std::string& name, const IInterface* parent) 
  : IReAlgToolCalo(type, name, parent)

{}

//!=================================================================================

bool RingerReFex::configurationInvalid(){

  if ((m_nRings.size() != m_dphiRings.size()) || (m_nRings.size() != m_detaRings.size()) || 
      (m_nRings.size() != m_nlayersRings.size())) {
    ATH_MSG_FATAL( "Wrong cnfiguration: Sizes of two properties do not match." );
    return true;
  }

  if((m_etaBins.size() < 2) || (m_etaBins.size()%2 != 0)){
    ATH_MSG_FATAL( "Wrong eta range configuration. Size must be equal or more than two." );
    return true;
  }
  return false;
}

//!=================================================================================

StatusCode RingerReFex::initialize() 
{

  if (IReAlgToolCalo::initialize().isFailure()) {
    ATH_MSG_FATAL( "Could not initialize base tool IReAlgToolCalo." );
    return StatusCode::FAILURE;
  }
  
  if(configurationInvalid()){
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_ringerContainerKey.initialize() );
  ATH_CHECK( m_clusterContainerKey.initialize() );

  m_maxRingsAccumulated = std::accumulate(m_nRings.begin(), m_nRings.end(), 0);

  if (!m_monTool.empty()){
    ATH_MSG_DEBUG("Retrieving monTool");
    CHECK(m_monTool.retrieve());
  }else{
    ATH_MSG_INFO("No monTool configured. NO MONITORING");
  }





  ATH_MSG_DEBUG( "Ringer algorithm initialization completed successfully." );
  ATH_MSG_DEBUG( "User parameters are:                                          " );
  ATH_MSG_DEBUG( "Using Global Center               : " << m_global_center        );
  ATH_MSG_DEBUG( "Using Hadronic cells              : " << m_useHad          );
  ATH_MSG_DEBUG( "Search Window in Eta              : " << m_etaSearchWindowSize  );
  ATH_MSG_DEBUG( "Search Window in Phi              : " << m_phiSearchWindowSize  );

  return StatusCode::SUCCESS;
}


//!=================================================================================


// Calculates the maximum energy cell in a CaloCell collection.
inline void RingerReFex::maxCell ( const std::vector<const CaloCell*>& vcell, 
			                double& eta, 
                      double& phi, 
                      const double eta_ref, 
			                const double phi_ref, 
                      const double eta_window, 
			                const double phi_window) const 
{
  double current = -999e30;
  const CaloCell* c = 0;
  const CaloCell* cit = 0;
  const double etamin = eta_ref - (0.5 * eta_window);
  const double etamax = eta_ref + (0.5 * eta_window);
  const double phimin = phi_ref - (0.5 * phi_window);
  const double phimax = phi_ref + (0.5 * phi_window);

  //are we, possibly at the wrap-around region for phi?
  bool wrap = Ringer::check_wrap_around(phi_ref, false);
  bool reverse_wrap = Ringer::check_wrap_around(phi_ref, true);

  //get at least the first cell, in case of panic (all zeroes for instance)
  for (std::vector<const CaloCell*>::const_iterator it = vcell.begin(); it != vcell.end(); ++it) {
    if(!*it) continue;
    cit = *it;
    double this_eta = cit->eta();
    double this_phi = cit->phi();
    if (this_eta > etamin && this_eta < etamax){
      double phi_use = this_phi; //use this value for phi (wrap protection)
      if (wrap) phi_use = Ringer::fix_wrap_around(phi_use, false);
      if (reverse_wrap) phi_use = Ringer::fix_wrap_around(phi_use, true);
      if (phi_use > phimin && phi_use < phimax){
        if (cit->energy() > current) {
          c = cit;
          current = cit->energy();
        }
      }
    }
  } // end of loop over cells
  if (!c) {
    eta = eta_ref;
    phi = phi_ref;
  }
  else {
    eta = c->eta();
    phi = c->phi();
  }
}




//!=================================================================================

StatusCode RingerReFex::execute(xAOD::TrigEMCluster &rtrigEmCluster, 
                              const IRoiDescriptor& roi, 
                              const CaloDetDescrElement*& , 
                              const EventContext& context) const
{

  
  auto total_time = Monitored::Timer("TIME_total");
  auto load_cells_time = Monitored::Timer("TIME_load_cells");
  auto mon = Monitored::Group(m_monTool, total_time, load_cells_time);
  
  total_time.start();

  double my_eta             = rtrigEmCluster.eta();
  double my_phi             = rtrigEmCluster.phi();
 

  // Check if the cluster is in the Eta region
  bool accept=false; 
  for(unsigned i=0; i<m_etaBins.size();i+=2){
    if((abs(my_eta) > m_etaBins[i]) && (abs(my_eta) <= m_etaBins[i+1]))  accept=true;
  }  
  if(!accept)
    return StatusCode::SUCCESS;

  
  
  bool wrap                 = Ringer::check_wrap_around(rtrigEmCluster.phi(), false);
  bool reverse_wrap         = Ringer::check_wrap_around(rtrigEmCluster.phi(), true);





  // Use this vector to hold HEC and TILE cells for 
  // the last three ring sets
  std::vector<const CaloCell*> vecTemp_for_tile;
 

  // Create all seven ring sets that will be used
  // to build the ringer shape
  std::vector<RingerReFex::RingSet> rset;
  // Create all ring sets here.
  unsigned int lc = 0; // layer count 
  for (unsigned int i = 0; i < m_nRings.size(); ++i) {  
    std::vector<CaloSampling::CaloSample> samples;
    for (unsigned int j = lc; j < lc + m_nlayersRings[i]; ++j){
      samples.push_back( static_cast<CaloSampling::CaloSample>(m_layersRings[j]) );
    }
    rset.push_back( RingerReFex::RingSet( m_nRings[i], m_maxCells[i], m_detaRings[i], m_dphiRings[i], samples ) );
    lc += m_nlayersRings[i];
  }



  unsigned int rs = 0;
  const CaloCell *it_tmp = nullptr;
  // Getting LAr cells and populating RingSets
  // Code has to be repetitve to make it really fast
  // Fist loop will be: TTEM for PS, EM1, EM2 and EM3 for the first 
  // four ringsets. After that we will loop over TTHEC. Four loops
  // once again to fill the vec temp for tile. This cells will be
  // distributed for your respective ring set in the end.
  for (unsigned det = TTEM; det <= TTHEC; det++){
    for (int sampling = 0; sampling < 4; ++sampling, rs++) {
      
      if(rs==7)  rs--;
      
      // This vector will hold all calo cells
      std::vector<const CaloCell*> vecTemp;
      vecTemp.reserve( rset.at(rs).maxCells() );

      /** iterators to LArCells. To be used when online
      LArCellCont (fast ByteStream Decoder) are to be used.
      Used as input for TrigDataAccess::LoadCollections */
      LArTT_Selector<LArCellCont> sel;
      LArTT_Selector<LArCellCont>::const_iterator it, itBegin, itEnd;
  
      load_cells_time.start();

      // If !(!(useHad==False) and det=!TTHEC) True
      // If !(!(useHad==False) and det==TTHEC) False
      // If !(!(useHad==True) and det==??) True
      // If we dont use hadronic cells, we will skip the load collection
      // and the ringg set will be empty. This will effect the ring set 4,5 and 6.
      if( !(!m_useHad && det==TTHEC) ){
      
        if( m_dataSvc->loadCollections( context, roi, static_cast<DETID>(det), sampling, sel ).isFailure() )
        {
          ATH_MSG_ERROR( "Failure while trying to retrieve cell information for the "<< det <<" calorimeter." );
          return StatusCode::FAILURE;
        }

      }

      load_cells_time.stop();

      itBegin = sel.begin();
      itEnd   = sel.end();
 
      if(rs < 4){ // For TTEM sampling (first 4 ring sets)

        if(!m_global_center){
          double current = -999e30;
          double this_eta = 0.0;
          double this_phi = 0.0;
          const CaloCell* c = nullptr;
          // Note that etamin (calculated above) is different from etamin (Region Selector)!
          const double etamin = rtrigEmCluster.eta() - (0.5 * m_etaSearchWindowSize);
          const double etamax = rtrigEmCluster.eta() + (0.5 * m_etaSearchWindowSize);
          const double phimin = rtrigEmCluster.phi() - (0.5 * m_phiSearchWindowSize);
          const double phimax = rtrigEmCluster.phi() + (0.5 * m_phiSearchWindowSize);         

 
          // Loop over cells
          for(it = itBegin; it != itEnd; ++it) 
          {
            if(!*it) continue;

            // LAr object to CaloCell
            it_tmp = static_cast<const CaloCell*>(*it);

            if( !rset.at(rs).belongs( it_tmp ) ) continue;

            vecTemp.push_back(it_tmp);
            this_eta = it_tmp->eta();
            this_phi = it_tmp->phi();
            
            
            if ( this_eta >  etamin && this_eta < etamax ){
              double phi_use = this_phi; //use this value for phi (wrap protection)
              if (wrap) phi_use = Ringer::fix_wrap_around(phi_use, false);
              if (reverse_wrap) phi_use = Ringer::fix_wrap_around(phi_use, true);
              if ( phi_use > phimin && phi_use < phimax ){
                if (it_tmp->energy() > current) {
                  c = it_tmp;
                  current = c->energy();
                }
              }
            } 
            
          }

          if (!c) {
            my_eta = rtrigEmCluster.eta();
            my_phi = rtrigEmCluster.phi();
          }else {
            my_eta = c->eta();
            my_phi = c->phi();
          }

        }else{
          for( it = itBegin; it != itEnd; ++it) {
            // LAr object to CaloCell

            if(!*it)  continue;
            it_tmp = static_cast<const CaloCell *> (*it);

            if( !rset.at(rs).belongs( it_tmp ) ) continue;
            vecTemp.push_back(it_tmp);
          }

        }

        // Skip if there is no cells to add
        if ( !vecTemp.empty() )
          rset.at(rs).push_back(vecTemp, my_eta, my_phi);
      
      } // end if rs < 4
      else{ // rs = 4, 5, 6
        // this part will hold the HEC calo sample
        for(it = itBegin; it != itEnd; ++it) {
          if(!*it) continue;
          it_tmp = static_cast<const CaloCell *> (*it);
          vecTemp_for_tile.push_back(it_tmp);
        } // end of loop over cells
      } // end if rs >= 4

    } // end of loop over samplings
  } // end of loop over detectors



  const TileCell *itt_tmp = nullptr;
  // Get all cells for the Tile calorimeter. After this loop we
  // will have HEC and TILE samples inside of the vecTemp_for_Tile
  //for (unsigned int iR=0;iR<m_dataSvc->TileContSize(); ++iR) 
  if(m_useHad){   
    /** iterators to TileCells. To be used when on/offline
    TileCellCont/IDC (fast/slow ByteStream Decoder) are to be used.
    Used as input for TrigDataAccess::LoadCollections */
    TileCellCollection sel;
    TileCellCollection::const_iterator itt, ittBegin, ittEnd;
 
    load_cells_time.start();
    if( m_dataSvc->loadCollections( context, roi, sel ).isFailure() ){
      ATH_MSG_ERROR( "Error retrieving TileCalorimeter cells!" );
      return StatusCode::FAILURE;
    }
    load_cells_time.stop();

    ittBegin = sel.begin();
    ittEnd = sel.end();
    for(itt = ittBegin; itt != ittEnd; ++itt) 
    {
      if(!*itt)  continue;

      itt_tmp = *itt;
      vecTemp_for_tile.push_back(static_cast<const CaloCell *>(itt_tmp));
    } // end of loop over cells
  }
  //} // end of loop over containers

  
  // Fill the last three ringsets with HEC and TILE cells
  rs = 4; const CaloCell *ct_tmp=nullptr;
  for (std::vector<RingerReFex::RingSet>::iterator jt=rset.begin()+rs; jt!=rset.end(); ++jt, rs++) {
    
    std::vector<const CaloCell*> vecTemp;
    vecTemp.reserve( jt->maxCells() );
    for (std::vector<const CaloCell *>::const_iterator ct = vecTemp_for_tile.begin(); ct != vecTemp_for_tile.end(); ++ct) {
      ct_tmp = static_cast<const CaloCell *> (*ct);
      if( !jt->belongs( ct_tmp ) ) continue;
	    vecTemp.push_back(ct_tmp);
    }
    // Skip if there is no cells to add
    if ( vecTemp.empty() ) continue;    

    if (!m_global_center) {
      double my_eta2, my_phi2;
      maxCell(vecTemp, my_eta2, my_phi2, rtrigEmCluster.eta(), rtrigEmCluster.phi(), m_etaSearchWindowSize, m_phiSearchWindowSize);
      jt->push_back( vecTemp, my_eta2, my_phi2 );
    }else {
      jt->push_back( vecTemp, rtrigEmCluster.eta(), rtrigEmCluster.phi() );
    }

  } // end of loop over ringsets
 


  
  std::vector<float> ref_rings;
  ref_rings.reserve( m_maxRingsAccumulated );
  // Fill the rings energy to the xAOD object
  for (std::vector<RingerReFex::RingSet>::iterator jt=rset.begin(); jt!=rset.end(); ++jt) {
    ref_rings.insert(ref_rings.end(), jt->pattern().begin(), jt->pattern().end());
  } 
  

  
  //Create the container to attach all rings
  SG::WriteHandle<xAOD::TrigRingerRingsContainer> ringsCollection = SG::WriteHandle<xAOD::TrigRingerRingsContainer>( m_ringerContainerKey, context );
  ATH_CHECK( ringsCollection.record( std::make_unique<xAOD::TrigRingerRingsContainer>(),  std::make_unique<xAOD::TrigRingerRingsAuxContainer>() ) );
  auto ptrigRingerRings= new xAOD::TrigRingerRings();
  ringsCollection->push_back( ptrigRingerRings );  
  ptrigRingerRings->setRings(ref_rings);
  auto clusLink = ElementLink<xAOD::TrigEMClusterContainer>(m_clusterContainerKey.key(),0,context);
  ptrigRingerRings->setEmClusterLink( clusLink  );

  if(msgLvl(MSG::DEBUG))
    printRings(rset, ptrigRingerRings);
  
 
  total_time.stop();
  
  return StatusCode::SUCCESS;
}


void RingerReFex::printRings( std::vector<RingSet> &rset, xAOD::TrigRingerRings *ring )
const {

  ATH_MSG_DEBUG("--------------- Cluster Information  ----------------");
  for(unsigned rs=0; rs<rset.size(); ++rs){
    ATH_MSG_DEBUG("RingSet number is: " << rs);  
    for(unsigned i=0;i<rset.at(rs).samples().size();i++)
      ATH_MSG_DEBUG("   Calo layer ID is : "<< rset.at(rs).samples()[i] );
    ATH_MSG_DEBUG("search eta window: " << m_etaSearchWindowSize << " search phi window: " << m_phiSearchWindowSize);
    ATH_MSG_DEBUG("deta: " << m_detaRings[rs] << " dphi: " << m_dphiRings[rs]);
    ATH_MSG_DEBUG("Pattern has size equal than: " << rset.at(rs).pattern().size());
    for(unsigned i=0;i<rset.at(rs).pattern().size();++i)
      ATH_MSG_DEBUG("   Ring " << i << " energy:  " << rset.at(rs).pattern()[i] << " MeVs.");
  }// Loop over ringSets

  //ATH_MSG_DEBUG("Rings energy: ");
  //for(unsigned i=0;i<ref_rings.size();++i)
  //  ATH_MSG_DEBUG("   Ring " <<i<< "  =  "<<ref_rings.at(i) << "  MeV");
  ATH_MSG_DEBUG("Cluster test link:");
  auto c = ring->emCluster();
  ATH_MSG_DEBUG("et = "<< c->et() << " eta = " << c->eta()  << " roiword = " << c->RoIword());
  ATH_MSG_DEBUG("---------------- End of information -----------------");
 

}

//!=================================================================================
//!=================================================================================
//!=================================================================================
//!=================================================================================
//!=================================================================================
//!=================================================================================
//!=================================================================================
//!=================================================================================


RingerReFex::RingSet::RingSet (unsigned int maxRings, 
                    unsigned int maxCells, 
                    double etasz, 
                    double phisz, 
                    const std::vector<CaloSampling::CaloSample> &samples)
    : m_etasz(etasz),
      m_phisz(phisz),
      m_maxCells(maxCells),
      m_rings(maxRings),
      m_samples(samples),
      m_cachedOverEtasize(1/etasz),
      m_cachedOverPhisize(1/phisz)
{}
  
  
RingerReFex::RingSet::~RingSet()
{}
 

bool RingerReFex::RingSet::belongs( const CaloCell *c ) const
{
  for (std::vector<CaloSampling::CaloSample>::const_iterator kt=m_samples.begin(); kt!=m_samples.end(); ++kt) 
  {
    if ( c->caloDDE()->getSampling() == static_cast<CaloSampling::CaloSample>(*kt) )
      return true;
  }
  return false;
}

 
void RingerReFex::RingSet::push_back( const std::vector<const CaloCell*>& c, 
                                      const double eta_center, const double phi_center)
{ 
  if( m_rings.empty() ||  m_samples.empty())  return;

  //typedef std::vector<CaloSampling::CaloSample> samp_type;
  // Used later to multiply by the ring energy and get Et instead of E
  const double one_over = 1 / std::cosh(std::fabs(eta_center));
  
  //are we, possibly at the wrap-around region for phi?
  const bool wrap = Ringer::check_wrap_around(phi_center, false);
  const bool reverse_wrap = Ringer::check_wrap_around(phi_center, true);

  // Loop over all cells
  for (std::vector<const CaloCell*>::const_iterator it=c.begin(); it!=c.end(); ++it) {
   

    if( !(*it) ){ 
      continue;
    }

    // if I get here, is because I have to include the cell energy. It
    // calculates which ring the cells should be added to, then, it adds
    // up the cell value there and goes to the next cell. No need to do
    // anything later, because the sums are already correct!
    double phi_use = (*it)->phi(); //use this value for phi (wrap protection)
    if (wrap) phi_use = Ringer::fix_wrap_around(phi_use, false);
    else if (reverse_wrap) phi_use = Ringer::fix_wrap_around(phi_use, true);
    
    // Measure delta eta and delta phi to find out on which ring we are
    unsigned int i = 0;
    const double deltaEta = ((*it)->eta() - eta_center)*m_cachedOverEtasize;
    const double deltaPhi = (phi_use - phi_center)*m_cachedOverPhisize;
    const double deltaGreater = std::max(fabs(deltaEta), fabs(deltaPhi));
    i = static_cast<unsigned int>( std::floor (deltaGreater) );
    // Certain compilers have problems with round(), so let's do it ourselves
    if ( (deltaGreater - (double) i) > 0.5) ++i;
    
    if (i < m_rings.size()) {
      //give us Et instead of E
      m_rings[i] += (*it)->energy();
    }    
  }

  for (unsigned i=0; i < m_rings.size(); i++) 
  {
    m_rings[i] *= one_over;
  }

}













