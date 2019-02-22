/* Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration */
#include "CaloRec/CaloTowerGeometrySvc.h"
#include "CaloRec/CaloTopoClusterFromTowerHelpers.h"

#include <cmath>
#include <cstdio>
#include <fstream>

namespace { constexpr auto _pi = 3.14159265358979323846; }

CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::m_invalidIndex = index_t(-1); 
double                        CaloTowerGeometrySvc::m_invalidValue = -999.;

CaloTowerGeometrySvc::CaloTowerGeometrySvc(const std::string& name,ISvcLocator* pSvcLocator)
  : AthService(name,pSvcLocator)
  , m_caloDDM((const CaloDetDescrManager*)0)
  , m_logFileName("logfile")
  , m_towerEtaWidth(0.)
  , m_towerPhiWidth(0.)
  , m_towerArea(0.)
  , m_towerBins(0)                              
  , m_maxCellHash(0)                            //----------------------------------------//
  , m_towerEtaBins(100)                         // Default tower definition is "hadronic" //
  , m_towerEtaMin(-5.0)                         // towers of size 0.1 x pi/32.            // 
  , m_towerEtaMax(5.0)                          //----------------------------------------//
  , m_adjustEta(true)
  , m_towerPhiBins(64)                          
  , m_towerPhiMin(-_pi)                         //----------------------------------------//
  , m_towerPhiMax(_pi)                          // FCal vertical and horizontal cell      //
  , m_fcal1Xslice(4.)                           // slicing creates "mini-cells" which are //
  , m_fcal1Yslice(4.)                           // then projected onto towers. The mini-  //
  , m_fcal2Xslice(4.)                           // cell signal is 1/(Nx x Ny) x Ecell,    //
  , m_fcal2Yslice(6.)                           // where Nx(y) are the number of x(y)     //
  , m_fcal3Xslice(6.)                           // slices.                                //
  , m_fcal3Yslice(6.)                           //----------------------------------------//
{
  // Properties                                               
  declareProperty("TowerEtaBins",               m_towerEtaBins, "Number of pseudorapidity bins in tower grid");
  declareProperty("TowerEtaMin",                m_towerEtaMin,  "Lower boundary of pseudorapidity range");
  declareProperty("TowerEtaMax",                m_towerEtaMax,  "Upper boundary of pseudorapidity range");
  declareProperty("AdjustFCalToTowerEtaBounds", m_adjustEta,    "Adjust FCal cells to eta boundaries");
  declareProperty("TowerPhiBins",               m_towerPhiBins, "Number of azimuthal bins in tower grid");
  declareProperty("TowerPhiMin",                m_towerPhiMin,  "Lower boundary of azimuthal range");
  declareProperty("TowerPhiMax",                m_towerPhiMax,  "Upper boundary of azimuthal range");
  // change only for R&D
  declareProperty("FCal1NSlicesX", m_fcal1Xslice,   "Number of X slices for FCal1 cells");
  declareProperty("FCal1NSlicesY", m_fcal1Yslice,   "Number of Y slices for FCal1 cells");
  declareProperty("FCal2NSlicesX", m_fcal2Xslice,   "Number of X slices for FCal2 cells");
  declareProperty("FCal2NSlicesY", m_fcal2Yslice,   "Number of Y slices for FCal2 cells");
  declareProperty("FCal3NSlicesX", m_fcal3Xslice,   "Number of X slices for FCal3 cells");
  declareProperty("FCal3NSlicesY", m_fcal3Yslice,   "Number of Y slices for FCal3 cells");
}

//-------------//
// Gaudi stuff //
//-------------//

StatusCode CaloTowerGeometrySvc::queryInterface(const InterfaceID& riid,void** ppvInterface)
{
  if ( CaloTowerGeometrySvc::interfaceID().versionMatch(riid) ) { 
    *ppvInterface = this;
    return StatusCode::SUCCESS;
  } else {
    return AthService::queryInterface(riid,ppvInterface); 
  }
}

//-------------------------//
// Initialize and Finalize //
//-------------------------//

StatusCode CaloTowerGeometrySvc::initialize()
{
  // set up services and managers
  if ( f_setupSvc().isFailure() ) {
    ATH_MSG_ERROR("Cannot allocate the calorimeter detector description manager");
    return StatusCode::FAILURE;
  }

  // prepare FCal segmentation 
  m_ndxFCal[0] = m_fcal1Xslice; m_ndxFCal[1] = m_fcal2Xslice; m_ndxFCal[2] = m_fcal3Xslice;
  m_ndyFCal[0] = m_fcal1Yslice; m_ndyFCal[1] = m_fcal2Yslice; m_ndyFCal[2] = m_fcal3Yslice;
  for ( uint_t i(0); i<m_ndxFCal.size(); ++i ) { m_wgtFCal[i] = 1./(m_ndxFCal.at(i)*m_ndyFCal.at(i)); }

  // other derived quantities
  if ( m_towerEtaBins > 0 ) { 
    m_towerEtaWidth = (m_towerEtaMax-m_towerEtaMin)/((double)m_towerEtaBins); 
  } else {
    ATH_MSG_ERROR("Number of tower eta bins is invalid (" << m_towerEtaBins << " bins)");
    return StatusCode::FAILURE;
  }
  if ( m_towerPhiBins > 0 ) { 
    m_towerPhiWidth = (m_towerPhiMax-m_towerPhiMin)/((double)m_towerPhiBins);
  } else {
    ATH_MSG_ERROR("Number of tower phi bins is invalid (" << m_towerPhiBins << " bins)");
    return StatusCode::FAILURE;
  }

  m_towerBins   = m_towerEtaBins*m_towerPhiBins;
  m_towerArea   = m_towerEtaWidth*m_towerPhiWidth;
  m_maxCellHash = f_caloDDM()->element_size();

  char buffer[256];
  int  nbuf(sprintf(buffer,"Tower description - Eta(bins,min,max,width) = (%3zu,%6.3f,%6.3f,%6.4f)",m_towerEtaBins,m_towerEtaMin,m_towerEtaMax,m_towerEtaWidth));
  ATH_MSG_INFO( std::string(buffer,nbuf) );
  nbuf = sprintf(buffer,"Tower description - Phi(bins,min,max,width) = (%3zu,%6.3f,%6.3f,%6.4f)",m_towerPhiBins,m_towerPhiMin,m_towerPhiMax,m_towerPhiWidth);
  ATH_MSG_INFO( std::string(buffer,nbuf) );
  nbuf = sprintf(buffer,"Tower description - maximum number of towers %5zu; (eta,phi)-space area %6.4f; maximum cell hash index %6zu",m_towerBins,m_towerArea,m_maxCellHash);
  ATH_MSG_INFO( std::string(buffer,nbuf) );

  if ( this->msgLvl(MSG::VERBOSE) ) {
    std::vector<std::string> logger; logger.reserve(m_towerBins+10);
    char buffer[1024];
    int nbuf(sprintf(buffer,"[CaloTowerGeometrySvc::%s] +-------- Tower Index Mapping ---------+------------+------------+",this->name().c_str()));
    logger.push_back(std::string(buffer,nbuf));
    nbuf = sprintf(buffer,"[CaloTowerGeometrySvc::%s] | %10.10s | %10.10s | %10.10s | %10.10s | %10.10s |",
		   this->name().c_str(),"TowerIndex", " EtaIndex ", " PhiIndex ","     Eta     ","     Phi    ");
    logger.push_back(std::string(buffer,nbuf));
    nbuf = sprintf(buffer,"[CaloTowerGeometrySvc::%s] +------------+------------+------------+------------+------------+",this->name().c_str());
    logger.push_back(std::string(buffer,nbuf));
    for ( size_t i(0); i<m_towerBins; ++i ) { 
      size_t etaIndex(this->etaIndexFromTowerIndex(i));
      size_t phiIndex(this->phiIndexFromTowerIndex(i));
      double eta(this->towerEta(i)); double phi(this->towerPhi(i));
      nbuf = sprintf(buffer,"[CaloTowerGeometrySvc::%s] |    %5zu   |    %5zu   |    %5zu   |   %6.3f   |   %6.3f   |",this->name().c_str(),i,etaIndex,phiIndex,eta,phi);
      logger.push_back(std::string(buffer,nbuf));
    }
    nbuf = sprintf(buffer,"[CaloTowerGeometrySvc::%s] +------------+------------+------------+------------+------------+",this->name().c_str());
    logger.push_back(std::string(buffer,nbuf));
    // 
    std::string logFileName = m_logFileName+"."+this->name()+".dat";
    std::ofstream logfile;
    logfile.open(logFileName); for ( auto mlog : logger ) { logfile << mlog << std::endl; } logfile.close();
  }
      
  return f_setupTowerGrid(); 
}

StatusCode CaloTowerGeometrySvc::finalize()
{ return StatusCode::SUCCESS; }

//-------//
// Setup //
//-------//

StatusCode CaloTowerGeometrySvc::f_setupTowerGrid()
{
  // initialized
  if ( m_maxCellHash == 0 ) { 
    ATH_MSG_ERROR("Service not initialized? Maximum cell hash is " << m_maxCellHash ); 
    return StatusCode::FAILURE;
  }

  // payload template
  elementvector_t ev;

  // set up lookup table
  ATH_MSG_INFO( "Setting up cell-to-tower lookup for " << m_maxCellHash << " calorimeter cells" );
  m_towerLookup.resize(m_maxCellHash,ev); 

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

StatusCode CaloTowerGeometrySvc::f_setupTowerGridFCal(const CaloDetDescrElement* pCaloDDE)
{
  //-----------------------------------------------------------------------------------------//
  // FCal special - the rectangular (in linear space) calorimeter cells are sub-divided into //
  // small cells and then shared across as many towers as the small cells cover.             //
  //-----------------------------------------------------------------------------------------//

  // collect geometrical variables
  int    cLayer(pCaloDDE->getLayer()-1);   // FCal layer number 1..3 -> array indices 0..2

  double cXpos(pCaloDDE->x());             // FCal cell x position (cell center)             
  double cYpos(pCaloDDE->y());             // FCal cell y position (cell center)
  double cZpos(pCaloDDE->z());             // FCal cell z position (cell center)
  double cZpos2(cZpos*cZpos); 

  double cXwid(pCaloDDE->dx());            // FCal cell x full width 
  double cYwid(pCaloDDE->dy());            // FCal cell y full width 

  // double xSlice(cXwid/m_ndxFCal[cLayer]);  // FCal cell x slize width
  // double ySlice(cYwid/m_ndyFCal[cLayer]);  // FCal cell y slice width
  double cWght(m_wgtFCal[cLayer]);         // FCal cell geometrical (signal) weight

  int nXslice((int)m_ndxFCal[cLayer]);     // FCal number of x slices
  int nYslice((int)m_ndyFCal[cLayer]);     // FCal number of y slices
  double cXstp(cXwid/((double)nXslice));   // FCal slice x width
  double cYstp(cYwid/((double)nYslice));   // FCal slice y width 

  // fill cell fragments
  //  double xoff(cXpos-cXwid/2.+cXstp/2.); double yoff(cYpos-cYwid/2.+cYstp/2.);
  double x(cXpos-(cXwid-cXstp)/2.); 
  double xlim(cXpos+cXwid/2.); double ylim(cYpos+cYwid/2.);
  double etaOrig(0.); 
  //  for ( int ix(0); ix < nXslice; ++ix ) {
  //   double x(xoff+ix*cXstp);
  while ( x < xlim ) { 
    //    for ( int iy(0); iy < nYslice; ++iy ) { 
    //      double y(yoff+iy*cYstp);
    double y(cYpos-(cYwid-cYstp)/2.);
    while ( y < ylim ) { 
      double r(std::sqrt(x*x+y*y+cZpos2));
      double eta(-0.5*std::log((r-cZpos)/(r+cZpos)));
      bool etaAdjusted(false);
      if ( m_adjustEta ) { 
	if ( eta < m_towerEtaMin ) {
	  etaAdjusted = true;
	  etaOrig     = eta;
	  eta = m_towerEtaMin+m_towerEtaWidth/2.; 
	} else if ( eta > m_towerEtaMax ) {
	  etaAdjusted = true;
	  etaOrig     = eta;
	  eta = m_towerEtaMax-m_towerEtaWidth/2.;
	}
      }
      double phi(CaloPhiRange::fix(std::atan2(y,x)));
      index_t towerIdx(this->towerIndex(eta,phi));
      // tower index not valid
      if ( isInvalidIndex(towerIdx) ) { 
	ATH_MSG_WARNING("Found invalid tower index for FCal cell (eta,phi) = (" << eta << "," << phi << ") at (x,y,z) = (" << x << "," << y << "," << cZpos << ") [cell ignored]");
      } else { 
	// add tower to lookup
	if ( etaAdjusted ) { 
	  ATH_MSG_WARNING("FCal cell direction (eta,phi) = (" << etaOrig << "," << phi << ") for cell at (x,y,z) = (" 
			  << x << "," << y << "," << cZpos << ") adjusted to (eta,phi) = (" << eta << "," << phi << ") [cell adjusted]");
	}
	f_assign(pCaloDDE->calo_hash(),towerIdx,cWght);
      } // tower index ok
      y += cYstp;
    } // loop on y fragments
    x += cXstp;
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
  uint_t kEta(static_cast<uint_t>(cEtaWid/m_towerEtaWidth+0.5)); kEta = kEta == 0 ? 1 : kEta; // fully contained cell may have 0 fragments (protection)
  uint_t kPhi(static_cast<uint_t>(cPhiWid/m_towerPhiWidth+0.5)); kPhi = kPhi == 0 ? 1 : kPhi; 

  // print out
  if ( kEta > 1 || kPhi > 1 ) {
    ATH_MSG_VERBOSE("Found cell [" << pCaloDDE->calo_hash() << "/0x" << std::hex << pCaloDDE->identify().get_compact() << std::dec << "] spawning several towers."
		    << " Neta = " << kEta << ", Nphi = " << kPhi );
  }
  
  // share cells
  double cWght(1./((double)kEta*kPhi));            // area weight
  double sEta(cEtaWid/((double)kEta));           // step size (pseudorapidity)
  double sPhi(cPhiWid/((double)kPhi));           // step size (azimuth)
  double oEta(cEtaPos-sEta/2.);                  // offset (pseudorapidity)
  double oPhi(cPhiPos-sPhi/2.);                  // offset (azimuth)

  // loop over cell fragments
  for ( uint_t ie(1); ie<=kEta; ++ie ) {
    double ceta(oEta+((double)ie-0.5)*sEta);     // eta of fragment
    for ( uint_t ip(1); ip<=kPhi; ++ip ) {
      double cphi(oPhi+((double)ip-0.5)*sPhi);   // phi fragment
      // tower index
      index_t towerIdx(this->towerIndex(ceta,cphi));
      if ( isInvalidIndex(towerIdx) ) { 
	ATH_MSG_ERROR("Found invalid tower index for non-FCal cell (id,eta,phi) = (" << pCaloDDE->calo_hash() << "," << ceta << "," << cphi << ")");
	return StatusCode::FAILURE;
      } // invalid tower index
      //      m_towerLookup[pCaloDDE->calo_hash()].emplace_back(towerIdx,cWght); // add to tower lookup
      f_assign(pCaloDDE->calo_hash(),towerIdx,cWght);
      if ( msgLvl(MSG::DEBUG) && towerIdx > 3200 && towerIdx < 3300 ) { 
	double el(this->towerEta(towerIdx)-this->etaWidth()/2.); double eh(this->towerEta(towerIdx)+this->etaWidth()/2.);
	double pl(this->towerPhi(towerIdx)-this->phiWidth()/2.); double ph(this->towerPhi(towerIdx)+this->phiWidth()/2.);
	if ( cEtaPos >= el && cEtaPos <= eh && cPhiPos >= pl && cPhiPos <= ph ) { 
	  ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("Cell [%6zu] at (%6.3f,%6.3f) contributes to tower [%5zu] at ([%6.3f-%6.3f],[%6.3f-%6.3f]) with weight %6.4f",
						  (size_t)pCaloDDE->calo_hash(),cEtaPos,cPhiPos,el,eh,pl,ph,cWght) );
	} else { 
	  ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("Cell [%6zu] at (%6.3f,%6.3f) contributes to tower [%5zu] at ([%6.3f-%6.3f],[%6.3f-%6.3f]) with weight %6.4f [OUT_OF_BOUNDS]",
						  (size_t)pCaloDDE->calo_hash(),cEtaPos,cPhiPos,el,eh,pl,ph,cWght) );
	}
      } // debugging central region
    } // phi fragment loop
  } // eta fragment loop
  return StatusCode::SUCCESS;
} 

//------//
// Fill //
//------//

double CaloTowerGeometrySvc::f_assign(IdentifierHash cellHash,index_t towerIdx,double wght) 
{
  // check if cell-tower already related
  uint_t cidx(static_cast<uint_t>(cellHash));
  for ( auto elm : m_towerLookup.at(cidx) ) { 
    if ( towerIndex(elm) == towerIdx ) { std::get<1>(elm) += wght; return cellWeight(elm); }
  }
  // not yet related
  m_towerLookup[cidx].emplace_back(towerIdx,wght);
  return cellWeight(m_towerLookup.at(cidx).back()); 
}

//--------//
// Access //
//--------//

StatusCode CaloTowerGeometrySvc::access(const CaloCell* pCell,std::vector<index_t>& towerIdx,std::vector<double>& towerWghts) const
{ return this->access(f_caloDDE(pCell)->calo_hash(),towerIdx,towerWghts); }

StatusCode CaloTowerGeometrySvc::access(IdentifierHash cellHash,std::vector<index_t>& towerIdx,std::vector<double>& towerWghts) const 
{
  towerIdx.clear();
  towerWghts.clear();

  uint_t cidx(static_cast<uint_t>(cellHash));

  if ( cidx >= m_towerLookup.size() ) { 
    ATH_MSG_WARNING("Invalid cell hash index " << cellHash << ", corresponding index " << cidx << " not found in tower lookup");
    return StatusCode::SUCCESS;
  }

  if ( towerIdx.capacity()   < m_towerLookup.at(cidx).size() ) { towerIdx.reserve(m_towerLookup.at(cidx).size());   }
  if ( towerWghts.capacity() < m_towerLookup.at(cidx).size() ) { towerWghts.reserve(m_towerLookup.at(cidx).size()); }

  for ( const auto& elm : m_towerLookup.at(cidx) ) { towerIdx.push_back(towerIndex(elm)); towerWghts.push_back(cellWeight(elm)); }

  return StatusCode::SUCCESS;
}

CaloTowerGeometrySvc::elementvector_t CaloTowerGeometrySvc::getTowers(const CaloCell* pCell) const
{ return pCell != 0 ? this->getTowers(f_caloDDE(pCell)->calo_hash()) : elementvector_t(); }

CaloTowerGeometrySvc::elementvector_t CaloTowerGeometrySvc::getTowers(IdentifierHash cellHash) const
{
  // check input
  uint_t cidx(static_cast<uint_t>(cellHash)); 
  return cidx < m_towerLookup.size() ? m_towerLookup.at(cidx) : elementvector_t();  
} 

//-----------------------//
// Tower Geometry Helper //
//-----------------------//

double CaloTowerGeometrySvc::cellWeight(IdentifierHash cellHash,index_t towerIdx) const
{
  index_t cidx(static_cast<uint_t>(cellHash));
  double cwght(0.);

  if ( cidx < m_towerLookup.size() ) {
    for ( auto elm : m_towerLookup.at(cidx) ) { 
      if ( towerIndex(elm) == towerIdx ) { cwght = cellWeight(elm); break; } 
    } 
  }
  return cwght;
}

//---------------//
// Index Helpers //
//---------------//

CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::etaIndex(IdentifierHash cellHash) const
{
  const auto cdde = f_caloDDE(cellHash); 
  return cdde != 0 ? etaIndex(cdde->eta()) : invalidIndex(); 
}

CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::etaIndex(double eta) const
{ 
  return eta >= m_towerEtaMin && eta <= m_towerEtaMax 
    ? index_t(std::min(static_cast<uint_t>((eta-m_towerEtaMin)/m_towerEtaWidth),m_towerEtaBins-1))
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
    ? index_t(std::min(static_cast<uint_t>((phi-m_towerPhiMin)/m_towerPhiWidth),m_towerPhiBins-1))
    : invalidIndex();
}
