/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************
 * Author: Denis Damazio
 * Revision
 *    Joao Victor Pinto, jodafons@cern.ch
 *    werner freund,     wsfreund@cern.ch
 *
 * Implements the RingerReFex (3nd revision).
 *******************************************************/

#include <vector>
#include <map>
#include <cmath>
#include <numeric>
#include "RingerReFex.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
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
using namespace Monitored;




//!=================================================================================

RingerReFex::RingerReFex(const std::string& type, const std::string& name, const IInterface* parent) 
  : IReAlgToolCalo(type, name, parent)
{}

//!=================================================================================

bool RingerReFex::configurationInvalid(){
  
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

  if (!m_monTool.empty()){
    ATH_MSG_DEBUG("Retrieving monTool");
    CHECK(m_monTool.retrieve());
  }else{
    ATH_MSG_INFO("No monTool configured. NO MONITORING");
  }


  ATH_MSG_DEBUG( "Ringer algorithm initialization completed successfully." );
  ATH_MSG_DEBUG( "User parameters are:                                          " );
  ATH_MSG_DEBUG( "Using Global Center               : " << m_globalCenter         );
  ATH_MSG_DEBUG( "Using Tile cells                  : " << m_useTile              );
  ATH_MSG_DEBUG( "Search Window in Eta              : " << m_etaSearchWindowSize  );
  ATH_MSG_DEBUG( "Search Window in Phi              : " << m_phiSearchWindowSize  );

  return StatusCode::SUCCESS;
}




//!=================================================================================

StatusCode RingerReFex::execute( xAOD::TrigEMCluster &emCluster, 
                                 const IRoiDescriptor& roi, 
                                 const CaloDetDescrElement*& , 
                                 const EventContext& context) const
{

  
  auto total_time      = Monitored::Timer("TIME_total");
  auto load_cells_time = Monitored::Timer("TIME_load_cells");
  auto mon             = Monitored::Group(m_monTool, total_time, load_cells_time);
  
  total_time.start();
 
  SG::WriteHandle<xAOD::TrigRingerRingsContainer> ringsCollection = 
  SG::WriteHandle<xAOD::TrigRingerRingsContainer>( m_ringerContainerKey, context );

  ATH_CHECK( ringsCollection.record( std::make_unique<xAOD::TrigRingerRingsContainer>(),  
             std::make_unique<xAOD::TrigRingerRingsAuxContainer>() ) );



  // Check if the cluster is in the Eta region
  bool accept=false; 
  for(unsigned i=0; i<m_etaBins.size();i+=2){
    if((std::abs(emCluster.eta()) > m_etaBins[i]) && (std::abs(emCluster.eta()) <= m_etaBins[i+1]))  accept=true;
  }

  
  if(!accept){
    auto dummyRinger = new xAOD::TrigRingerRings();
    ringsCollection->push_back( dummyRinger ); 
    return StatusCode::SUCCESS;
  }
  

  std::vector<RingerReFex::RingSet> vec_rs;

  for (unsigned rs = 0; rs < m_nRings.size(); ++rs) {  

    auto obj = RingerReFex::RingSet( m_nRings[rs], 
                                     m_deltaEta[rs], 
                                     m_deltaPhi[rs], 
                                     m_detectors[rs], 
                                     m_samplings[rs], 
                                     m_samples[rs],
                                     m_doQuarter[rs], 
                                     m_doEtaAxesDivision[rs], 
                                     m_doPhiAxesDivision[rs]);
    vec_rs.push_back( obj );
  }



  std::vector<const CaloCell*> vec_tile_cells;
 
  // Cache all Tile cells for instance
  if ( m_useTile ){

    // Get all cells for the Tile calorimeter
    TileCellCollection sel;
    TileCellCollection::const_iterator it, itBegin, itEnd;
 
    load_cells_time.start();
    if( m_dataSvc->loadCollections( context, roi, sel ).isFailure() ){
      load_cells_time.stop();
      ATH_MSG_ERROR( "Error retrieving TileCalorimeter cells!" );
      return StatusCode::FAILURE;
    }
    load_cells_time.stop();

    itBegin = sel.begin();
    itEnd = sel.end();
    for(it = itBegin; it != itEnd; ++it) 
    {
      if(!*it)  continue;
      vec_tile_cells.push_back(static_cast<const CaloCell *>(*it));
    }
  }


  // This is the main loop
  for( auto& rs : vec_rs ){
 
    const CaloCell* hotCell = nullptr;
    double ehot=-999e30;
    
    for ( auto det : rs.detectors() ){

      DETID det_id= static_cast<DETID>(det.first);
      int sampling = det.second;

      if( det_id == TILE ){

        for (std::vector<const CaloCell *>::const_iterator it = vec_tile_cells.begin(); it != vec_tile_cells.end(); ++it) 
        {
          if( !rs.isValid(*it) ) continue;
          
          rs.push_back(*it);

          if( !m_globalCenter ){
            if( maxCell( *it, ehot, emCluster.eta(), emCluster.phi() ) )
              hotCell=*it;
          }
        }


      }else{// TTEM, TTHEC, FCALEM and FCALHAD

        // Load all cells
        LArTT_Selector<LArCellCont> sel;
        LArTT_Selector<LArCellCont>::const_iterator it, itBegin, itEnd;
        load_cells_time.start();
        if( m_dataSvc->loadCollections( context, roi, det_id, sampling, sel ).isFailure() )
        {
          load_cells_time.stop();
          ATH_MSG_ERROR( "Failure while trying to retrieve cell information for the "<< det_id <<" calorimeter." );
          return StatusCode::FAILURE;
        }
      
        load_cells_time.stop();

        itBegin = sel.begin();
        itEnd   = sel.end();
       
        // Loop over all LAr cells
        // Append all valid cells and search the hottest cell
        for(it = itBegin; it != itEnd; ++it) 
        {
          if(!*it) continue;
          // LAr object to CaloCell
          const CaloCell *it_tmp = static_cast<const CaloCell*>(*it);

          // Check if the current cells is allow into this rs
          if( !rs.isValid( it_tmp ) ) continue;
          rs.push_back(it_tmp);
        
          if( !m_globalCenter ){
            if( maxCell( *it, ehot, emCluster.eta(), emCluster.phi() ) )
              hotCell=*it;
          }

        }

      }// Is TILE?

    }// Loop over all det configs


    // Use all Tile cells in cache
    if (m_globalCenter || !hotCell) {
      rs.buildRings( emCluster.eta(), emCluster.phi() );
    }else {
      rs.buildRings( hotCell->eta(), hotCell->phi() );
    }

  }// Loop over all ringer sets



  
  std::vector<float> ref_rings;
  for (std::vector<RingerReFex::RingSet>::iterator it=vec_rs.begin(); it!=vec_rs.end(); ++it)
  {
    auto rings = it->rings();
    ref_rings.insert(ref_rings.end(), rings.begin(), rings.end());
  }
    
  auto ptrigRingerRings= new xAOD::TrigRingerRings();
  ringsCollection->push_back( ptrigRingerRings );  
  ptrigRingerRings->setRings(ref_rings);
  //ptrigRingerRings->auxdecor<int>("type") = 1;
  if (m_decorateWithCells){
    std::vector<float> cells_eta;
    std::vector<float> cells_et;
    std::vector<float> cells_phi;
    std::vector<int>    cells_sampling;
    std::vector<int>    cells_size;
    std::vector<double> rings_sum;
    for( auto& rs : vec_rs )
      rs.fill_cells_info(cells_eta, cells_phi, cells_et, cells_sampling, cells_size, rings_sum);
    ptrigRingerRings->auxdecor< std::vector<float> >("cells_eta") = cells_eta;
    ptrigRingerRings->auxdecor< std::vector<float> >("cells_et") = cells_et;
    ptrigRingerRings->auxdecor< std::vector<float> >("cells_phi") = cells_phi;
    ptrigRingerRings->auxdecor< std::vector<int> >("cells_sampling") = cells_sampling;
    ptrigRingerRings->auxdecor< std::vector<int> >("cells_size") = cells_size;
    if (m_doQuarter[0]) ptrigRingerRings->auxdecor< std::vector< double > >("asym_rings_sum") = rings_sum;
    else  ptrigRingerRings->auxdecor< std::vector< double > >("rings_sum") = rings_sum;
  }

  auto clusLink = ElementLink<xAOD::TrigEMClusterContainer>(m_clusterContainerKey.key(),0,context);
  ptrigRingerRings->setEmClusterLink( clusLink  );
  

  if(msgLvl(MSG::DEBUG))
    printRings(vec_rs, emCluster);
  
  total_time.stop();
  return StatusCode::SUCCESS;
}


//!=================================================================================
void RingerReFex::RingSet::fill_cells_info(std::vector<float> &cells_eta, std::vector<float> &cells_phi, std::vector<float> &cells_et, std::vector<int> &cells_sampling, std::vector<int> &cells_size, std::vector<double>  &rings_sum){
  for (std::vector<const CaloCell*>::const_iterator it=m_cells.begin(); it!=m_cells.end(); ++it) {
    cells_eta.push_back((*it)->eta());
    cells_phi.push_back((*it)->phi());
    cells_et.push_back((*it)->energy());
    auto sampling = (*it)->caloDDE()->getSampling();
    cells_sampling.push_back((int) sampling);
  }
  cells_size.push_back(m_cells.size());
  double sum = 0;
  for (auto ring : m_rings) sum+=ring;
  rings_sum.push_back(sum);
}

inline bool RingerReFex::maxCell ( const CaloCell* cell, double &energy, const double eta_ref, const double phi_ref ) const 
{
  const double etamin = eta_ref - (0.5 * m_etaSearchWindowSize);
  const double etamax = eta_ref + (0.5 * m_etaSearchWindowSize);
  const double phimin = phi_ref - (0.5 * m_phiSearchWindowSize);
  const double phimax = phi_ref + (0.5 * m_phiSearchWindowSize);

  //are we, possibly at the wrap-around region for phi?
  bool wrap = Ringer::check_wrap_around(phi_ref, false);
  bool reverse_wrap = Ringer::check_wrap_around(phi_ref, true);

  if(!cell) return false;
  double this_eta = cell->eta();
  double this_phi = cell->phi();
  if (this_eta > etamin && this_eta < etamax){
    double phi_use = this_phi; //use this value for phi (wrap protection)
    if (wrap) phi_use = Ringer::fix_wrap_around(phi_use, false);
    if (reverse_wrap) phi_use = Ringer::fix_wrap_around(phi_use, true);
    if (phi_use > phimin && phi_use < phimax){
      if (cell->energy() > energy) {
        energy = cell->energy();
        return true;
      }
    }
  }
  return false;
}


//!=================================================================================


void RingerReFex::printRings( std::vector<RingSet> &vec_rs, const xAOD::TrigEMCluster &cl ) const 
{
  ATH_MSG_INFO("--------------- Cluster Information (2) ----------------");
  
  //auto cl = ring->emCluster();
  ATH_MSG_INFO("et = "<< cl.et() << " eta = " << cl.eta()  << " roiword = " << cl.RoIword());
   
    
  for(unsigned rs=0; rs<vec_rs.size(); ++rs){
    ATH_MSG_INFO("RingSet number is: " << rs);  
    ATH_MSG_INFO("search eta window: " << m_etaSearchWindowSize << " search phi window: " << m_phiSearchWindowSize);
    ATH_MSG_INFO("deta: " << m_deltaEta[rs] << " dphi: " << m_deltaPhi[rs]);
    ATH_MSG_INFO("Pattern has size equal than: " << vec_rs.at(rs).rings().size());
    
    double tot=0;
    for(unsigned i=0;i<vec_rs.at(rs).rings().size();++i){
      tot+=vec_rs.at(rs).rings()[i];
      ATH_MSG_INFO("   Ring " << i << " energy:  " << vec_rs.at(rs).rings()[i] << " MeVs.");
    }
    ATH_MSG_INFO( "RingSet = " << rs << " Total energy = " << tot );
  }// Loop over ringSets
  ATH_MSG_INFO("---------------- End of information -----------------");

}

//!=================================================================================
//!=================================================================================


RingerReFex::RingSet::RingSet ( unsigned int maxRings, double deta,  double dphi, 
                                const std::vector<int> &detectors,
                                const std::vector<int> &samplings,
                                const std::vector<int> &samples,
                                bool doQuarter,
                                bool doEtaAxesDivision,
                                bool doPhiAxesDivision
                                 )
    : m_deltaEta(deta),
      m_deltaPhi(dphi),
      m_detectors(detectors),
      m_samplings(samplings),
      m_samples(samples),
      m_doQuarter(doQuarter),
      m_doEtaAxesDivision(doEtaAxesDivision),
      m_doPhiAxesDivision(doPhiAxesDivision)
{

  if(doQuarter){
    if(doEtaAxesDivision && doPhiAxesDivision){
      m_rings.resize( (maxRings-1)*4 + 1 );
    }else{
      m_rings.resize( (maxRings-1)*2 + 2 );
    }
  }else{
    m_rings.resize( maxRings );
  }


}
  
//!=================================================================================

const std::vector< std::pair< int, int > > RingerReFex::RingSet::detectors() const
{
  std::vector< std::pair<int,int> > vec;
  for ( unsigned i=0; i<m_detectors.size(); ++i )
    vec.push_back( std::make_pair( m_detectors[i], m_samplings[i] ) );
  return vec;
}
 
//!=================================================================================

bool RingerReFex::RingSet::isValid( const CaloCell *cell ) const
{
  for (std::vector<int>::const_iterator it=m_samples.begin(); it!=m_samples.end(); ++it)
  {
    if ( cell->caloDDE()->getSampling() == static_cast<CaloSampling::CaloSample>(*it) )
      return true;
  
  }
  return false;
}

//!=================================================================================

const std::vector<double>& RingerReFex::RingSet::rings() const
{
  return m_rings;
}

//!=================================================================================

void RingerReFex::RingSet::push_back( const CaloCell *cell ){
  m_cells.push_back(cell);
}

//!=================================================================================

void RingerReFex::RingSet::clear(){
  m_rings.clear();
  m_cells.clear();
}

//!=================================================================================

void RingerReFex::RingSet::buildRings( const double eta_center, const double phi_center)
{ 
  // cache cosh eta value
  const double cosh_eta = std::cosh(std::abs(eta_center));
  
  //are we, possibly at the wrap-around region for phi?
  const bool wrap = Ringer::check_wrap_around(phi_center, false);
  const bool reverse_wrap = Ringer::check_wrap_around(phi_center, true);

  // Loop over all cells
  for (std::vector<const CaloCell*>::const_iterator it=m_cells.begin(); it!=m_cells.end(); ++it) {
   
    if( !(*it) )
      continue;
    
    double phi_use = (*it)->phi(); //use this value for phi (wrap protection)
    if (wrap) phi_use = Ringer::fix_wrap_around(phi_use, false);
    else if (reverse_wrap) phi_use = Ringer::fix_wrap_around(phi_use, true);
    
    // Measure delta eta and delta phi to find out on which ring we are
    const double deltaEta = ((*it)->eta() - eta_center)/m_deltaEta;
    const double deltaPhi = (phi_use - phi_center)/m_deltaPhi;
    
    // ring index
    unsigned int i = 0;
    

    const double deltaGreater = std::max(std::abs(deltaEta), std::abs(deltaPhi));
    i = static_cast<unsigned int>( std::floor (deltaGreater) );
    // Certain compilers have problems with round(), so let's do it ourselves
    if ( (deltaGreater - (double) i) > 0.5) ++i;
  
    if(m_doQuarter){ // only for i>0, the central ring is not divided
      bool etaPositive = ( deltaEta > 0 )?true:false;
      bool phiPositive = ( deltaPhi > 0)?true:false;
      // i > 0 (not central ring)
      // Correct position in which we shall fill the ring:
      if ( m_doEtaAxesDivision && m_doPhiAxesDivision && i ){
        if (phiPositive){
          if (etaPositive){
            i = (i * 4) - 3;
          }
          else{
            i = (i * 4) - 2;
          }
        }
        else if (etaPositive){
          i = (i * 4) - 1;
        }
        else{
          i = (i * 4);
        }
      } else if ((m_doEtaAxesDivision || m_doPhiAxesDivision) && i){
        if (m_doEtaAxesDivision){
          i = (etaPositive)?(i * 2):((i * 2) - 1);
        }
        else {
          i = (phiPositive)?(i * 2):((i * 2) - 1);
        }
      }
    }// Do quarter?

    // Fill
    if (i < m_rings.size()) 
      m_rings[i] += (*it)->energy() / cosh_eta;
    
  }// Loop over each

}

//!=================================================================================







