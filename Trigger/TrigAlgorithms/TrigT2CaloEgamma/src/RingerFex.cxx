/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "TrigT2CaloEgamma/RingerFex.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
//!=================================================================================
const double RingerFex::ENERGY_THRESHOLD = 0.001;
const double RingerFex::PI_THRESHOLD = 1.0;
//!=================================================================================
RingerFex::RingerFex(const std::string& type, const std::string& name, const IInterface* parent) 
  : IAlgToolCalo(type, name, parent)
{
  declareProperty("EtaBins",              m_etaBins                             );
  declareProperty("GlobalCenter",         m_global_center = false               );
  declareProperty("EtaSearchWindowSize",  m_etaSearchWindowSize = 0.1           );
  declareProperty("PhiSearchWindowSize",  m_phiSearchWindowSize = 0.1           );
  declareProperty("DEtaRings",            m_detaRings                           );
  declareProperty("DPhiRings",            m_dphiRings                           );
  declareProperty("NRings",               m_nRings                              );
  declareProperty("LayersRings",          m_layersRings                         );
  declareProperty("NLayersRings",         m_nlayersRings                        );
  declareProperty("NMaxCells",            m_maxCells                            );
  declareProperty("HltFeature",           m_hlt_feature = "HLT_TrigT2CaloEgamma");
  declareProperty("Feature",              m_feature     = "TrigT2CaloEgamma"    );
  declareProperty("HistLabel",            m_histLabel   = ""                    );

  m_key = "";
  m_caloDDE = 0;

}
//!=================================================================================
StatusCode RingerFex::finalize() {
  for(unsigned i=0;i<m_cell_for_br.size();i++){
    m_cell_for_br[i].clear();
  }
  m_cell_for_br.clear();
  return StatusCode::SUCCESS;
}
//!=================================================================================
StatusCode RingerFex::initialize() {

  if (IAlgToolCalo::initialize().isFailure()) {
    ATH_MSG_FATAL( "Could not initialize base tool IAlgToolCalo." );
    return StatusCode::FAILURE;
  }

  if ((m_nRings.size() != m_dphiRings.size()) || (m_nRings.size() != m_detaRings.size()) || (m_nRings.size() != m_nlayersRings.size())) {
    ATH_MSG_FATAL( "Wrong configuration parameters for ring building." );
    return StatusCode::FAILURE;
  }

  if((m_etaBins.size() < 2) || (m_etaBins.size()%2 != 0)){
    ATH_MSG_FATAL( "Wrong eta range configuration. Size must be equal or more than two." );
    return StatusCode::FAILURE;
  }

  unsigned int layersCount = 0;
  for (unsigned int i = 0; i < m_nRings.size(); ++i) {
    std::vector<const CaloCell*> cell;
    cell.reserve(m_maxCells[i]);
    m_cell_for_br.push_back(cell);
    std::vector<CaloSampling::CaloSample> layers;
    for (unsigned int j = layersCount; j < layersCount + m_nlayersRings[i]; ++j){
      layers.push_back((CaloSampling::CaloSample) m_layersRings[j]);
    }
    layersCount += m_nlayersRings[i];
    m_ringsSet.push_back(RingSet(m_nRings[i], m_maxCells[i], m_detaRings[i], m_dphiRings[i], layers));
  } // end of loop over ringsets

  m_parent = dynamic_cast<T2CaloEgamma *>(const_cast<IInterface*>(this->parent()));
  if (!m_parent) {
    ATH_MSG_ERROR( "Failed to get T2CaloEgamma parent algorithm instance." );
    return StatusCode::FAILURE;
  }
  if (m_timersvc){
    if(m_histLabel!="") m_histLabel="_"+m_histLabel; ///insert underscore
    m_getRingsTimer = m_parent->addTimer("Time_RingerFexGetRings"+m_histLabel);
    m_RingerFexExecuteTimer = m_parent->addTimer("Time_RingerFexExecute"+m_histLabel);
    m_RingerRegSelTimer = m_parent->addTimer("Time_RingerFexRegSel"+m_histLabel);
  }

  ATH_MSG_DEBUG( "RingerAlgorithm initialization completed successfully." );
  for(unsigned i=0; i < m_etaBins.size(); i+=2)
  {
    ATH_MSG_DEBUG( "Eta bins for accept are: " << m_etaBins[i] << " < |eta| <= " << m_etaBins[i+1] );
  }
  ATH_MSG_DEBUG( "User parameters are:" );
  ATH_MSG_DEBUG( "Using Global Center               : " << m_global_center );
  ATH_MSG_DEBUG( "Search Window in Eta              : " << m_etaSearchWindowSize );
  ATH_MSG_DEBUG( "Search Window in Phi              : " << m_phiSearchWindowSize );

  // reserve space for cells, better do it now than later
  m_maxCellsAccumulated = std::accumulate(m_maxCells.begin()+4, m_maxCells.end(), 0);
  m_cell.reserve(m_maxCellsAccumulated);
  m_maxRingsAccumulated = std::accumulate(m_nRings.begin(), m_nRings.end(), 0);

  return StatusCode::SUCCESS;
}
//!=================================================================================
xAOD::TrigRingerRings *RingerFex::getRings() {

  // Creating the pattern based on the generated rings.
  xAOD::TrigRingerRings *these_rings = 0;
  these_rings = new xAOD::TrigRingerRings();
  these_rings->makePrivateStore();
  std::vector<float> ref_rings;
  these_rings->setRings(ref_rings);
  ref_rings.reserve( m_maxRingsAccumulated );

  for (std::vector<RingSet>::iterator jt=m_ringsSet.begin(); jt!=m_ringsSet.end(); ++jt) {
    ref_rings.insert(ref_rings.end(), jt->pattern().begin(), jt->pattern().end());
  } // end of loop over ringsets

  these_rings->setRings(ref_rings);

  return these_rings;
}
//!=================================================================================
void RingerFex::maxCell (const std::vector<const CaloCell*>& vcell, 
			 double& eta, double& phi, const double& eta_ref, 
			 const double& phi_ref, const double& eta_window, 
			 const double& phi_window) 
{
  double current = -999e30;
  double this_eta = 0.0;
  double this_phi = 0.0;
  const CaloCell* c = 0;
  const CaloCell* cit = 0;
  const double etamin = eta_ref - (0.5 * eta_window);
  const double etamax = eta_ref + (0.5 * eta_window);
  const double phimin = phi_ref - (0.5 * phi_window);
  const double phimax = phi_ref + (0.5 * phi_window);

  //are we, possibly at the wrap-around region for phi?
  bool wrap = check_wrap_around(phi_ref, false);
  bool reverse_wrap = check_wrap_around(phi_ref, true);
 
  //get at least the first cell, in case of panic (all zeroes for instance)
  for (std::vector<const CaloCell*>::const_iterator it = vcell.begin();
       it != vcell.end(); ++it) {
    cit = *it;
    this_eta = cit->eta();
    this_phi = cit->phi();
    if (this_eta > etamin && this_eta < etamax){
      double phi_use = this_phi; //use this value for phi (wrap protection)
      if (wrap) phi_use = fix_wrap_around(phi_use, false);
      if (reverse_wrap) phi_use = fix_wrap_around(phi_use, true);
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
StatusCode RingerFex::execute(xAOD::TrigEMCluster &rtrigEmCluster, const IRoiDescriptor& roi)
{
  m_error = 0x0;
  bool accept=false;
  float eta=std::fabs(rtrigEmCluster.eta());
  
  ATH_MSG_DEBUG( "Event with eta =  " << eta );
  
  if (m_timersvc) m_RingerFexExecuteTimer->start();
  if (m_timersvc) m_getRingsTimer->start();

  ///find eta bin  
  for(unsigned i=0; i<m_etaBins.size();i+=2){
    if((eta > m_etaBins[i]) && (eta <= m_etaBins[i+1])){
      ATH_MSG_DEBUG("Eta bins accept are :" << m_etaBins[i] << " < |eta| <= " << m_etaBins[i+1]);
      accept=true;
    }
  }  
  
  if(!accept)
  {
    ATH_MSG_DEBUG( "Event with |eta| = " << eta << " was rejected." );
    return StatusCode::SUCCESS;
  }

  if ((getCells(roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), m_ringsSet, m_global_center,/*
             */ rtrigEmCluster.eta(), rtrigEmCluster.phi(), m_etaSearchWindowSize, m_phiSearchWindowSize)).isFailure()) {

    ATH_MSG_ERROR( "Failed to get cells from Region Selector." );
    return StatusCode::FAILURE;
  }

  // RingSets to xAOD::TrigRingerRings
  xAOD::TrigRingerRings *rings = getRings();

  if (!rings) {
    ATH_MSG_ERROR( "Error generating the rings!" );
    return StatusCode::FAILURE;
  }

  if (m_timersvc) m_getRingsTimer->stop();

  // parse between RingerFex and T2CaloEgamma (m_parent)
  //rings->setRoIword( rtrigEmCluster.RoIword() );
  if (m_parent) {
    m_parent->setRingsFeature(rings,m_key,m_feature);
  }

  if (m_timersvc) m_RingerFexExecuteTimer->stop();
  return StatusCode::SUCCESS;
}
//!=================================================================================
StatusCode RingerFex::getCells(double etamin, double etamax, double phimin, double phimax,
			                         std::vector<RingSet>& rset, 
			                         bool global_center,
			                         const double& eta, const double& phi, 
			                         const double& eta_window,
			                         const double& phi_window)
{
  const CaloCell *it_tmp;
  const TileCell *itt_tmp;
  unsigned int rs = 0;

  double my_eta = eta;
  double my_phi = phi;

  // Are we, possibly at the wrap-around region for phi?
  bool wrap = check_wrap_around(phi, false);
  bool reverse_wrap = check_wrap_around(phi, true);
  if (m_timersvc)
  {
    m_RingerRegSelTimer->start();
    m_RingerRegSelTimer->pause();
  }
  // clear HEC and TILE cells
  m_cell.clear();

  // Getting LAr cells and populating RingSets
  // Code has to be repetitve to make it really fast
  for (unsigned det = TTEM; det <= TTHEC; det++){
    for (int sampling = 0; sampling < 4; ++sampling, rs++) {
      
      if (rs == 7) rs--;
      else { 
        rset.at(rs).reset();
        m_cell_for_br[rs].clear();
      }

      // RingSet config layer
      const std::vector<CaloSampling::CaloSample>& dets = rset.at(rs).detectors();
      if(dets.empty())  continue;

      if (m_timersvc) m_RingerRegSelTimer->resume();
      m_data->RegionSelector(sampling,etamin,etamax,phimin,phimax, static_cast<const DETID> (det));
      if ( m_data->LoadCollections(m_iBegin, m_iEnd).isFailure() ) {
        ATH_MSG_ERROR( "Failure while trying to retrieve cell information for the "<< det <<" calorimeter." );
        return StatusCode::FAILURE;
      }
      if (m_timersvc) m_RingerRegSelTimer->pause();
      std::vector<const CaloCell*> &vecTemp = m_cell_for_br[rs];
      
      if(rs < 4){
        if(!global_center){
          double current = -999e30;
          double this_eta = 0.0;
          double this_phi = 0.0;
          const CaloCell* c = 0;
          // Note that etamin2 (calculated above) is different from etamin (Region Selector)!
          const double etamin2 = eta - (0.5 * eta_window);
          const double etamax2 = eta + (0.5 * eta_window);
          const double phimin2 = phi - (0.5 * phi_window);
          const double phimax2 = phi + (0.5 * phi_window);         

          for(m_it = m_iBegin; m_it != m_iEnd; ++m_it) {
            it_tmp = (CaloCell *) *m_it;    
            bool found=false;
            for (std::vector<CaloSampling::CaloSample>::const_iterator kt=dets.begin(); kt!=dets.end(); ++kt) {
	            if (((CaloSampling::CaloSample) (*kt)) == it_tmp->caloDDE()->getSampling()){
	              found=true;
                break;
              }
            }// Loop over config layers
            if(!found)  continue;

            vecTemp.push_back(it_tmp);
            this_eta = it_tmp->eta();
            this_phi = it_tmp->phi();
            if (this_eta > etamin2 && this_eta < etamax2){
              double phi_use = this_phi; //use this value for phi (wrap protection)
              if (wrap) phi_use = fix_wrap_around(phi_use, false);
              if (reverse_wrap) phi_use = fix_wrap_around(phi_use, true);
              if (phi_use > phimin2 && phi_use < phimax2){
                if (it_tmp->energy() > current) {
                  c = it_tmp;
                  current = c->energy();
                }
              }
            }
          }
          if (!c) {
            my_eta = eta;
            my_phi = phi;
          }
          else {
            my_eta = c->eta();
            my_phi = c->phi();
          }
        } // end if not global center
        else{
          for(m_it = m_iBegin; m_it != m_iEnd; ++m_it) {
            it_tmp = (CaloCell *) *m_it;
            bool found=false;
            for (std::vector<CaloSampling::CaloSample>::const_iterator kt=dets.begin(); kt!=dets.end(); ++kt) {
	            if (((CaloSampling::CaloSample) (*kt)) == it_tmp->caloDDE()->getSampling()){
	              found=true;
                break;
              }
            }// Loop over config layers
            if(!found)  continue;
            vecTemp.push_back(it_tmp);
          }
        }

        if ( !(m_cell_for_br[rs]).size() ) {
          continue;
        }
        rset.at(rs).add(m_cell_for_br[rs], my_eta, my_phi);
      } // end if rs < 4
      else{
        // this part will hold the HEC part
        for(m_it = m_iBegin; m_it != m_iEnd; ++m_it) {
          it_tmp = (CaloCell *) *m_it;
          //vecTemp.push_back(it_tmp);
          m_cell.push_back(it_tmp);
        } // end of loop over cells
      } // end if rs >= 4
    } // end of loop over samplings
  } // end of loop over detectors


  // Getting TileCal cells and populating RingSets
  if (m_timersvc) m_RingerRegSelTimer->resume();
  m_data->RegionSelector(0,etamin,etamax,phimin,phimax,TILE);
  //m_cell.clear();
  for (unsigned int iR=0;iR<m_data->TileContSize();++iR) {
    if ( m_data->LoadCollections(m_itBegin, m_itEnd,iR,!iR).isFailure() ) {
      ATH_MSG_ERROR( "Error retrieving TileCalorimeter cells!" );
      return StatusCode::FAILURE;
    }
    for(m_itt = m_itBegin; m_itt != m_itEnd; ++m_itt) { //loop over cells
      itt_tmp = *m_itt;
      m_cell.push_back((CaloCell *) itt_tmp);
    } // end of loop over cells
  } // end of loop over containers
  if (m_timersvc) m_RingerRegSelTimer->stop();
  
  rs = 4;
  CaloSampling::CaloSample cs_temp;
  for (std::vector<RingSet>::iterator jt=rset.begin()+rs; jt!=rset.end(); ++jt, rs++) {
    const std::vector<CaloSampling::CaloSample>& dets = jt->detectors();
    for (std::vector<const CaloCell *>::const_iterator ct = m_cell.begin(); ct != m_cell.end(); ++ct) {
      //cs_temp = CaloSampling::getSampling(*(*ct));
      cs_temp = (*ct)->caloDDE()->getSampling();
      for (std::vector<CaloSampling::CaloSample>::const_iterator kt=dets.begin(); kt!=dets.end(); ++kt) {
	      if (((CaloSampling::CaloSample) (*kt)) == cs_temp){
	        (m_cell_for_br[rs]).push_back(*ct);
        }
      } // for relevant detectors
    } // end of loop over cells
    if ( !(m_cell_for_br[rs]).size() ) {
      continue;
    }
    if (!global_center) {
      double my_eta, my_phi;
      maxCell(m_cell_for_br[rs], my_eta, my_phi, eta, phi, eta_window, phi_window);
      jt->add(m_cell_for_br[rs], my_eta, my_phi);
    }
    else {
      jt->add(m_cell_for_br[rs], eta, phi);
    }
  } // end of loop over ringsets
 

  // debug RingSet
  if(msgLvl(MSG::DEBUG))
  {
    ATH_MSG_DEBUG("--------------- Cluster Information  ----------------");
    for(unsigned rs=0; rs<rset.size(); ++rs){
      ATH_MSG_DEBUG("RingSet number is: " << rs);  
      for(unsigned i=0;i<rset.at(rs).detectors().size();i++)
        ATH_MSG_DEBUG("   Calo layer ID is : "<< rset.at(rs).detectors()[i] );
      ATH_MSG_DEBUG("search eta window: " << eta_window << " search phi window: " << phi_window);
      ATH_MSG_DEBUG("deta: " << m_detaRings[rs] << " dphi: " << m_dphiRings[rs]);
      ATH_MSG_DEBUG("eta: " << my_eta << " phi: " << my_phi);
      ATH_MSG_DEBUG("Pattern has size equal than: " << rset.at(rs).pattern().size());
      for(unsigned i=0;i<rset.at(rs).pattern().size();++i)
        ATH_MSG_DEBUG("   sample " << i << " energy:  " << rset.at(rs).pattern()[i] << " MeVs.");
    }// Loop over ringSets
    ATH_MSG_DEBUG("---------------- End of information -----------------");
  }

  return StatusCode::SUCCESS;
}
//!=================================================================================
//!=================================================================================
//!=================================================================================
//!=================================================================================
RingerFex::RingSet::RingSet (unsigned int max, unsigned int maxCells, double etasz, double phisz, const std::vector<CaloSampling::CaloSample> &detectors)
  : m_etasz(etasz),
    m_phisz(phisz),
    m_max(max),
    m_maxCells(maxCells),
    m_detector(detectors),
    m_val(max),
    m_cachedOverEtasize(1/etasz),
    m_cachedOverPhisize(1/phisz)
{;}
//!=================================================================================
RingerFex::RingSet::RingSet ()
  : m_etasz(0.1),
    m_phisz(0.1),
    m_max(0),
    m_maxCells(512),
    m_detector(0),
    m_val(0),
    m_cachedOverEtasize(1),
    m_cachedOverPhisize(1)
{
}
//!=================================================================================
RingerFex::RingSet::~RingSet()
{;}
//!=================================================================================
void RingerFex::RingSet::add (const std::vector<const CaloCell*>& c,
			   const double& eta_center, const double& phi_center)
{
  // some protection
  if(m_val.empty() ||  m_detector.empty()) return;

  typedef std::vector<const CaloCell*> vec_type;
  //typedef std::vector<CaloSampling::CaloSample> samp_type;

  // Used later to multiply by the ring energy and get Et instead of E
  const double one_over = 1 / std::cosh(std::fabs(eta_center));
  
  //are we, possibly at the wrap-around region for phi?
  const bool wrap = RingerFex::check_wrap_around(phi_center, false);
  const bool reverse_wrap = RingerFex::check_wrap_around(phi_center, true);

  //for all cells
  for (vec_type::const_iterator it=c.begin(); it!=c.end(); ++it) {

    //std::cout << "current caloDDE->getSampling() is: " << (*it)->caloDDE()->getSampling() << std::endl;
    
    //if I get here, is because I have to include the cell energy. It
    //calculates which ring the cells should be added to, then, it adds
    //up the cell value there and goes to the next cell. No need to do
    //anything later, because the sums are already correct!
    
    double phi_use = (*it)->phi(); //use this value for phi (wrap protection)
    if (wrap) phi_use = RingerFex::fix_wrap_around(phi_use, false);
    else if (reverse_wrap) phi_use = RingerFex::fix_wrap_around(phi_use, true);

    // Measure delta eta and delta phi to find out on which ring we are
    unsigned int i = 0;
    const double deltaEta = ((*it)->eta() - eta_center)*m_cachedOverEtasize;
    const double deltaPhi = (phi_use - phi_center)*m_cachedOverPhisize;
    const double deltaGreater = std::max(fabs(deltaEta), fabs(deltaPhi));
    i = static_cast<unsigned int>( std::floor (deltaGreater) );
    // Certain compilers have problems with round(), so let's do it ourselves
    if ( (deltaGreater - (double) i) > 0.5) ++i;

    if (i < m_val.size()) {
      //give us Et instead of E
      m_val[i] += (*it)->energy();
    }
    
  } //end for all cells

  for (unsigned i=0; i < m_val.size(); i++) {
    m_val[i] *= one_over;
  } // end for all rings

}
//!=================================================================================

