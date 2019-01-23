
#include "CaloRec/CaloTowerGeometrySvc.h"

#include <cmath>
#include <cstdio>

CaloTowerGeometrySvc::CaloTowerGeometrySvc(const std::string& name,ISvcLocator* pSvcLocator)
  : AthService(name,pSvcLocator)
  , m_detectorStore("StoreGateSvc/DetectorStore",name)
  , m_caloDDM((CaloDetDescrManager*)0)
  , m_towerEtaWidth(0.)
  , m_towerPhiWidth(0.)
  , m_towerArea(0.)
  , m_towerEtaBins(100)                         ////////////////////////////////////////////
  , m_towerEtaMin(-5.0)                         // Default tower definition is "hadronic" //
  , m_towerEtaMax(5.0)                          // towers of size 0.1 x pi/32.            // 
  , m_towerPhiBins(64)                          ////////////////////////////////////////////
  , m_towerPhiMin(-std::M_PI)                   
  , m_towerPhiMax(-std::M_PI)                   ////////////////////////////////////////////
  , m_towerBins(0)                              // FCal vertical and horizontal cell      //
  , m_fcal1Xslice(4.)                           // slicing creates "mini-cells" which are //
  , m_fcal1Yslice(4.)                           // then projected onto towers. The mini-  //
  , m_fcal2Xslice(4.)                           // cell signal is 1/(Nx x Ny) x Ecell,    //
  , m_fcal2Yslice(6.)                           // where Nx(y) are the number of x(y)     //
  , m_fcal3Xslice(6.)                           // slices.                                //
  , m_fcal3Yslice(6.)                           ////////////////////////////////////////////
{                                               
  declareProperty("TowerEtaBins",m_towerEtaBins,    "Number of pseudorapidity bins in tower grid");
  declareProperty("TowerEtaMin", m_towerEtaMin,     "Lower boundary of pseudorapidity range");
  declareProperty("TowerEtaMax", m_towerEtaMax,     "Upper boundary of pseudorapidity range");
  declareProperty("TowerPhiBins",m_towerPhiBins,    "Number of azimuthal bins in tower grid");
  declareProperty("TowerPhiMin", m_towerPhiMin,     "Lower boundary of azimuthal range");
  declareProperty("TowerPhiMax", m_towerPhiMax,     "Upper boundary of azimuthal range");
  // change only for R&D
  declareProperty("FCal1NSlicesX", m_fcal1Xslice,   "Number of X slices for FCal1 cells");
  declareProperty("FCal1NSlicesY", m_fcal1Yslice,   "Number of Y slices for FCal1 cells");
  declareProperty("FCal2NSlicesX", m_fcal2Xslice,   "Number of X slices for FCal2 cells");
  declareProperty("FCal2NSlicesY", m_fcal2Yslice,   "Number of Y slices for FCal2 cells");
  declareProperty("FCal3NSlicesX", m_fcal3Xslice,   "Number of X slices for FCal3 cells");
  declareProperty("FCal3NSlicesY", m_fcal3Yslice,   "Number of Y slices for FCal3 cells");
  // no need to change -- really!
  declareProperty("DetStore",      m_detectorStore, "Handle to detector store");
}

/////////////////////////////
// Initialize and Finalize //
/////////////////////////////

StatusCode CaloTowerGeometrySvc::initialize()
{ 
  // allocate the calorimeter detector description manager
  if ( detStore()->retrieve(m_CaloDDM).isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve the calorimeter detector description manager from the detector store");
    return StatusCode::FAILURE;
  }

  // prepare FCal segmentation 
  m_ndxFCal[0] = m_fcal1Xslice; m_ndxFCal[1] = m_fcal2Xslice; m_ndxFCal[2] = m_fcal3Xslice;
  m_ndyFCal[0] = m_fcal1Yslice; m_ndyFCal[1] = m_fcal2Yslice; m_ndyFCal[2] = m_fcal3Yslice;
  for ( size_t i(0); i<m_ndxFCal.size(); ++i ) { m_wgtFCal[i] = 1./(m_ndxFCal.at(i)*m_ndyFCal.at(i)); }

  // other derived quantities
  if ( m_towerEtaBins > 0 ) { 
    m_towerEtaWidth = (m_towerEtaMax-m_towerEtaMin)/((double)m_towerEtaBins); 
  } else {
    ATH_MSG_ERROR("Number of tower eta bins is " << m_towerEtaBins );
    return StatusCode::FAILURE;
  }
  if ( m_towerPhiBins > 0 ) { 
    m_towerPhiWidth = (m_towerPhiMax-m_towerPhiMin)/((double)m_towerPhiBins);
  } else {
    ATH_MSG_ERROR("Number of tower phi bins is " << m_towerPhiBins );
    return StatusCode::FAILURE;
  }

  m_towerBins = m_towerEtaBins*m_towerPhiBins;
  m_towerArea = m_towerEtaWidth*m_towerPhiWidth;

  char buffer[512];
  int  nbuf(sprintf(buffer,"Tower description (EtaBins,EtaMin,EtaMax,EtaWidth) = (%3z,%6.3f,%6.3f%,%6.4f) (PhiBins,PhiMin,PhiMax,PhiWidth) = (%3z,%6.3f,%6.3f%,%6.4f) Max number of towers: %z Area: %6.4f",
		    m_towerEtaBins, m_towerEtaMin, m_towerEtaMax, m_towerEtaWidth, m_towerPhiBins, m_towerPhiMin, m_towerPhiMax, m_towerPhiWidth, m_towerBins, m_towerArea ));
  ATH_MSG_INFO("Tower description (EtaBins,EtaMin,EtaMax,EtaWidth) = (" << std::string(buffer,nbuf) );

  return f_setupTowerGrid(); 
}

StatusCode CaloTowerGeometrySvc::finalize()
{ return StatusCode::SUCCESS; }

///////////
// Setup //
///////////

StatusCode CaloTowerGeometrySvc::f_setupTowerGrid()
{

  // allocate calorimeter detector descriptoin manager
  const CaloDetDesrManager* pCaloDDM(f_caloDDM());
  if ( pCaloDDM == 0 ) { 
    ATH_MSG_ERROR("Initialization failed - could not retrieve calorimeter detector description manager from detector store");
    return StatusCode::FAILURE;
  }

  // payload template
  elementvector_t ev;

  // set up lookup table
  size_t ncells(pCaloDDM->element_size()); 
  ATH_MSG_DEBUG( "Setting up cell-to-tower lookup for " << ncells << " calorimeter cells" );
  m_towerLookup.resize(ncells,ev); 

  // loop cells
  for ( auto fcell(m_caloDDM->element_begin()); fcell != m_caloDDM->element_end(); ++fcell ) {
    // reference cell descriptor
    const CaloDetDescrElement* pCaloDDE = *fcell; 
    // check hash id validity
    index_t cidx(pCaloDDE->calo_hash());
    if ( cidx >= m_towerLookup.size() ) { 
      ATH_MSG_WARNING( "Cell hash identifier out of range " << cidx << "/" << m_towerLookup.size() << ", ignore cell" );
    } else { 
      if ( pCaloDDE->is_lar_fcal() ) { 
	if ( this->f_setupTowerGridFCal(pCaloDDE).isFailure() ) { return StatusCode::FAILURE; }
      } else {
	if ( this->f_setupTowerGridProj(pCaloDDE).isFailure() ) { return StatusCode::FAILURE; }
      } 
    } // cell hash in range?    
  } // loop cell descriptors
  return StatusCode::SUCCESS;
}

StatusCode CaloTowerGeometrySvc::setupTowerGridFCal(const CaloDetDescrElement* pCaloDDE)
{
  /////////////////////////////////////////////////////////////////////////////////////////////
  // FCal special - the rectangular (in linear space) calorimeter cells are sub-divided into //
  // small cells and then shared across as many towers as the small cells cover.             //
  /////////////////////////////////////////////////////////////////////////////////////////////

  // collect geometrical variables
  int    cLayer(pCaloDDE->getLayer()-1);   // FCal layer number 1..3 -> array indices 0..2

  double cXpos(pCaloDDE->x());             // FCal cell x position (cell center)             
  double cYpos(pCaloDDE->y());             // FCal cell y position (cell center)
  double cZpos(pCaloDDE->z());             // FCal cell z position (cell center)
  double cZpos2(cZpos*cZpos); 

  double cXwid(pCaloDDE->dx());            // FCal cell x full width 
  double cYwid(pCaloDDE->dy());            // FCal cell y full width 

  double xSlice(cXwid/m_ndxFCal[cLayer]);  // FCal cell x slize width
  double ySlice(cYwid/m_ndyFCal[cLayer]);  // FCal cell y slice width
  double cWght(m_wgtFCal[cLayer]);         // FCal cell geometrical (signal) weight

  int nXslice((int)m_ndxFCal[cLayer]);     // FCal number of x slices
  int nYslice((int)m_ndyFCal[cLayer]);     // FCal number of y slices

  // fill cell fragments
  double xoff(cXpos-cXwid/2.); double yoff(cYpos-cYwid/2.);
  for ( int ix(0); ix < nXslice; ++ix ) {
    double x(xoff+ix*cXwid);
    for ( int iy(0); iy < nYslice; ++iy ) { 
      double y(yoff+iy*cYwid);
      double r(std::sqrt(x*x+y*y+cZpos2));
      double eta(-0.5*std::log((r-cZpos)/(r+cZpos)));
      double phi(CaloPhiRange::fix(std::atan2(y,x)));
      index_t towerIdx(this->towerIndex(eta,phi));
      // tower index not valid
      if ( isInvalidIndex(towerIdx) ) { 
	ATH_MSG_ERROR("Found invalid tower index for FCal cell (eta,phi) = (" << eta << "," << phi << ") at (x,y) = (" << x << "," << y << ")");
	return StatusCode::FAILURE;
      }
      // add tower to lookup
      m_towerLookup[pCaloDDE->calo_hash()].emplace_back(towerIdx,cWght);
    } // loop on y fragments
  } // loop on x fragments
  return StatusCode::SUCCESS;
}

StatusCode CaloTowerGeometrySvc::f_setupTowerGridProj(const CaloDetDescrElement* pCaloDDE)
{
  // projective readout calos - collect geometrical variables
  double cEtaPos(pCaloDDE->eta_raw());            // projective cell center in pseudorapidity
  double cEtaWid(pCaloDDE->deta());               // projective cell width in pseudorapidity
  double cPhiPos(pCaloDDE->phi_raw());            // projective cell center in azimuth
  double cPhiWid(pCaloDDE->dphi());               // projective cell width in azimuth

  // check cell-tower overlap area fractions
  size_t kEta((size_t)(cEtaWid/m_towerEtaWidth+0.5)); kEta = kEta == 0 ? 1 : kEta; // fully contained cell may have 0 fragments (protection)
  size_t kPhi((size_t)(cPhiWid/m_towerPhiWidth+0.5)); kPhi = kPhi == 0 ? 1 : kPhi; 

  // print out
  if ( kEta > 1 || kPhi > 1 ) {
    ATH_MSG_VERBOSE("Found cell [" << pCaloDDE->calo_hash() << "/0x" << std::hex << pCaloDDE->identify().get_compact() << std::dec << "] spawning several towers."
		    << " Neta = " << kEta << ", Nphi = " << kPhi );
  }
  
  // share cells
  double wgt(1./((double)kEta*kPhi));            // area weight
  double sEta(cEtaWid/((double)kEta));           // step size (pseudorapidity)
  double sPhi(cPhiWid/((double)kPhi));           // step size (azimuth)
  double oEta(cEtaPos-sEta/2.);                  // offset (pseudorapidity)
  double oPhi(cPhiPos-sPhi/2.);                  // offset (azimuth)

  // loop over cell fragments
  for ( size_t ie(1); ie<=kEta; ++ie ) {
    double ceta(oEta+((double)ie-0.5)*sEta);     // eta of fragment
    for ( size_t ip(1); ip<=kPhi; ++ip ) {
      double cphi(oPhi+((double)ip-0.5)*sPhi);   // phi fragment
      // tower index
      index_t towerIdx(this->towerIndex(ceta,cphi));
      if ( isInvalidIndex(towerIdx) ) { 
	ATH_MSG_ERROR("Found invalid tower index for non-FCal cell (id,eta,phi) = (" << pCaloDDE->calo_hash() << "," << ceta << "," << cphi << ")");
	return StatusCode::FAILURE;
      } // invalid tower index
      m_towerLookup[pCaloDDE->calo_hash()].emplace_back(towerIdx,wgt); // add to tower lookup
    } // phi fragment loop
  } // eta fragment loop
  return StatusCode::SUCCESS;
} 


////////////
// Access //
////////////

StatusCode CaloTowerGeometrySvc::access(const CaloCell* pCell,std::vector<size_t>& towerIdx,std::vector<double>& towerWghts) const
{ return this->access(f_caloDDE(pCell)->calo_hash(),towerIdx,towerWghts); }

StatusCode CaloTowerGeometrySvc::access(IdentifierHash cellHash,std::vector<size_t>& towerIdx,std::vector<double>& towerWghts) const 
{
  towerIdx.clear();
  towerWghts.clear();

  size_t cidx((size_t)cellHash);

  if ( cidx > m_towerLookup.size() ) { 
    ATH_MSG_WARNING("Invalid cell hash index " << cellHash << ", corresponding index " << cidx << " not found in tower lookup");
    return StatusCode::SUCCESS;
  }

  if ( towerIdx.capacity() < m_towerLookup.at(cidx).size() ) { towerIdx.reserve(m_towerLookup.at(cidx).size()); }
  if ( towerWghts.capacity() < m_towerLookup.at(cidx).size() ) { towerWghts.reserve(m_towerLookup.at(cidx).size()); }

  for ( const auto& elm : m_towerLookup.at(cidx) ) { towerIdx.push_back((size_t)towerIndex(elm)); towerWghts.push_back(cellWeight(elm)); }

  return StatusCode::SUCCESS;
}

CaloTowerGeometrySvc::elementvector_t CaloTowerGeometrySvc::getTowers(const CaloCell* pCell) const
{ return this->getTowers(f_caloDDE(pCell)->calo_hash()); }

CaloTowerGeometrySvc::elementvector_t CaloTowerGeometrySvc::getTowers(IdentifierHash cellHash) const
{
  // check input
  index_t cidx((index_t)cellHash); 
  return cidx < m_towerLookup.size() ? m_towerLookup.at(cidx) : elementvector_t();  
} 

///////////////////////////
// Tower Geometry Helper //
///////////////////////////

double CaloTowerGeometrySvc::cellWeight(IdentifierHash cellHash,index_t toweridx) const
{
  index_t cidx((index_t)cellHash);
  double cwght(0.);

  if ( cidx < m_towerLookup.size() ) {
    for ( auto elm : m_towerLookup.at(cidx) ) { 
      if ( towerIndex(elm) == towerIdx ) { cwght = cellWeight(elm); break; } 
    } 
  }
  return cwght;
}

///////////////////
// Index Helpers //
///////////////////

CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::etaIndex(IdentifierHash cellHash) const
{
  const auto cdde = f_caloDDE(cellHash); 
  return cdde != 0 ? etaIndex(cdde->eta()) : invalidIndex(); 
}

CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::etaIndex(double eta) const
{ 
  return eta >= m_towerEtaMin && eta <= m_towerEtaMax 
    ? index_t(std::min((size_t)((eta-m_towerEtaMin)/m_towerEtaWidth),m_towerEtaBins-1))
    : invalidIndex(); 
}

CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::phiIndex(IdentifierHash cellHash) const
{
  const auto cdde = f_caloDDE(cellHash);
  return cdde != 0 ? phiIndex(cdde->phi()) : invalidIndex(); 
}

CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::phiIndex(double phi) const
{
  double dphi(CaloPhiRange::diff(phi,m_towerPhiMin));
  return dphi >= m_towerPhiMin && dphi <= m_towerPhiMax 
    ? index_t(std::min((size_t)((phi-m_towerPhiMin)/m_towerPhiWidth),m_towerPhiBins-1))
    : invalidIndex();
}


CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::towerIndex(index_t etaIdx,index_t phiIdx) const 
{
  if ( isInvalidIndex(etaIdx) || etaIdx >= m_towerEtaBins ) { return invalidIndex(); }
  if ( isInvalidIndex(phiIdx) || phiIdx >= m_towerPhiBins ) { return invalidIndex(); }
  return etaIdx+phiIdx*m_towerPhiBins;
} 

