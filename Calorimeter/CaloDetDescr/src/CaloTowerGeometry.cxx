/* Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration */
#include "CaloDetDescr/CaloTowerGeometry.h"

namespace { constexpr auto pi = 3.14159265358979323846; }

const CaloTowerGeometry::index_t CaloTowerGeometry::m_invalidIndex = index_t(-1); 
const double                     CaloTowerGeometry::m_invalidValue = -999.;

CaloTowerGeometry::CaloTowerGeometry(const CaloDetDescrManager* caloDDM) :
    m_caloDDM(caloDDM)
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
  , m_towerPhiMin(-pi)                         //----------------------------------------//
  , m_towerPhiMax(pi)                          // FCal vertical and horizontal cell      //
  , m_fcal1Xslice(8.)                           // slicing creates "mini-cells" which are //
  , m_fcal1Yslice(8.)                           // then projected onto towers. The mini-  //
  , m_fcal2Xslice(8.)                           // cell signal is 1/(Nx x Ny) x Ecell,    //
  , m_fcal2Yslice(12.)                          // where Nx(y) are the number of x(y)     //
  , m_fcal3Xslice(12.)                          // slices.                                //
  , m_fcal3Yslice(12.)                          //----------------------------------------//
{}


StatusCode CaloTowerGeometry::initialize(MsgStream& msg)
{
  // prepare FCal segmentation 
  m_ndxFCal[0] = m_fcal1Xslice; m_ndxFCal[1] = m_fcal2Xslice; m_ndxFCal[2] = m_fcal3Xslice;
  m_ndyFCal[0] = m_fcal1Yslice; m_ndyFCal[1] = m_fcal2Yslice; m_ndyFCal[2] = m_fcal3Yslice;
  for ( uint_t i(0); i<m_ndxFCal.size(); ++i ) { m_wgtFCal[i] = 1./(m_ndxFCal.at(i)*m_ndyFCal.at(i)); }

  // other derived quantities
  if ( m_towerEtaBins > 0 ) { 
    m_towerEtaWidth = (m_towerEtaMax-m_towerEtaMin)/((double)m_towerEtaBins); 
  } else {
    msg << MSG::ERROR << "Number of tower eta bins is invalid (" << m_towerEtaBins << " bins)" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_towerPhiBins > 0 ) { 
    m_towerPhiWidth = (m_towerPhiMax-m_towerPhiMin)/((double)m_towerPhiBins);
  } else {
    msg << MSG::ERROR << "Number of tower phi bins is invalid (" << m_towerPhiBins << " bins)" << endmsg;
    return StatusCode::FAILURE;
  }

  m_towerBins     = m_towerEtaBins*m_towerPhiBins;
  m_towerArea     = m_towerEtaWidth*m_towerPhiWidth;
  m_maxCellHash   = m_caloDDM->element_size()-1;
  m_numberOfCells = m_caloDDM->element_size(); 

  return f_setupTowerGrid(msg); 
}

StatusCode CaloTowerGeometry::f_setupTowerGrid(MsgStream& msg)
{
  // initialized
  if ( m_maxCellHash == 0 ) { 
    msg << "Service not initialized? Maximum cell hash is " << m_maxCellHash << endmsg;
    return StatusCode::FAILURE;
  }

  // payload template
  elementvector_t ev;

  // set up lookup table
  msg << "Setting up cell-to-tower lookup for " << m_numberOfCells << " calorimeter cells" << endmsg;
  m_towerLookup.resize(m_numberOfCells,ev);

  // loop cells
  for ( auto fcell(m_caloDDM->element_begin()); fcell != m_caloDDM->element_end(); ++fcell ) {
    // reference cell descriptor
    const CaloDetDescrElement* pCaloDDE = *fcell; 
    // check hash id validity
    index_t cidx(pCaloDDE->calo_hash());
    if ( cidx >= m_towerLookup.size() ) { 
      msg << MSG::WARNING << "Cell hash identifier out of range " << cidx << "/" << m_towerLookup.size() << ", ignore cell" << endmsg;
    } else {
      if ( pCaloDDE->is_lar_fcal() ) { 
	if ( this->f_setupTowerGridFCal(pCaloDDE,msg).isFailure() ) { return StatusCode::FAILURE; }
      } else {
	if ( this->f_setupTowerGridProj(pCaloDDE,msg).isFailure() ) { return StatusCode::FAILURE; }
      } 
    } // cell hash in range?    
  } // loop cell descriptors

  return StatusCode::SUCCESS;
}

StatusCode CaloTowerGeometry::f_setupTowerGridFCal(const CaloDetDescrElement* pCaloDDE, MsgStream& msg)
{
  //-----------------------------------------------------------------------------------------//
  // FCal special - the rectangular (in linear space) calorimeter cells are sub-divided into //
  // small cells and then shared across as many towers as the small cells cover.             //
  //-----------------------------------------------------------------------------------------//

  // collect geometrical variables
  int    cLayer(pCaloDDE->getLayer()-1);   // FCal layer number 1..3 -> array indices 0..2

  double cXpos(pCaloDDE->x_raw());             // FCal cell x position (cell center)             
  double cYpos(pCaloDDE->y_raw());             // FCal cell y position (cell center)
  double cZpos(pCaloDDE->z_raw());             // FCal cell z position (cell center)
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
	msg << MSG::WARNING << "Found invalid tower index for FCal cell (eta,phi) = (" << eta << "," << phi << ") at (x,y,z) = (" 
	    << x << "," << y << "," << cZpos << ") [cell ignored]" << endmsg;
      } else { 
	// add tower to lookup
	if ( etaAdjusted ) { 
	  msg << MSG::WARNING << "FCal cell direction (eta,phi) = (" << etaOrig << "," << phi << ") for cell at (x,y,z) = (" 
	      << x << "," << y << "," << cZpos << ") adjusted to (eta,phi) = (" << eta << "," << phi << ") [cell adjusted]" << endmsg;
	}
	f_assign(pCaloDDE->calo_hash(),towerIdx,cWght);
      } // tower index ok
      y += cYstp;
    } // loop on y fragments
    x += cXstp;
  } // loop on x fragments
  return StatusCode::SUCCESS;
}

StatusCode CaloTowerGeometry::f_setupTowerGridProj(const CaloDetDescrElement* pCaloDDE,MsgStream& msg)
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
    msg << MSG::VERBOSE << "Found cell [" << pCaloDDE->calo_hash() << "/0x" << std::hex << pCaloDDE->identify().get_compact() 
	<< std::dec << "] spawning several towers."
	<< " Neta = " << kEta << ", Nphi = " << kPhi << endmsg;
  }
  
  // share cells
  double cWght(1./((double)kEta*kPhi));          // area weight
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
	msg << MSG::ERROR << "Found invalid tower index for non-FCal cell (id,eta,phi) = (" << pCaloDDE->calo_hash() 
	    << "," << ceta << "," << cphi << ")" << endmsg;
	return StatusCode::FAILURE;
      } // invalid tower index
      f_assign(pCaloDDE->calo_hash(),towerIdx,cWght);
    } // phi fragment loop
  } // eta fragment loop
  return StatusCode::SUCCESS;
} 

//------//
// Fill //
//------//

double CaloTowerGeometry::f_assign(IdentifierHash cellHash,index_t towerIdx,double wght) 
{
  // check if cell-tower already related
  uint_t cidx(static_cast<uint_t>(cellHash));
  for ( element_t& elm : m_towerLookup.at(cidx) ) { 
    if ( towerIndex(elm) == towerIdx ) { std::get<1>(elm) += wght; return cellWeight(elm); }  
  }
  // not yet related
  m_towerLookup[cidx].emplace_back(towerIdx,wght);
  return cellWeight(m_towerLookup.at(cidx).back()); 
}

//--------//
// Access //
//--------//

StatusCode CaloTowerGeometry::access(IdentifierHash cellHash,std::vector<index_t>& towerIdx,std::vector<double>& towerWghts) const 
{
  towerIdx.clear();
  towerWghts.clear();

  uint_t cidx(static_cast<uint_t>(cellHash));

  if ( cidx >= m_towerLookup.size() ) { 
    //ATH_MSG_WARNING("Invalid cell hash index " << cellHash << ", corresponding index " << cidx << " not found in tower lookup");
    return StatusCode::SUCCESS;
  }

  if ( towerIdx.capacity()   < m_towerLookup.at(cidx).size() ) { towerIdx.reserve(m_towerLookup.at(cidx).size());   }
  if ( towerWghts.capacity() < m_towerLookup.at(cidx).size() ) { towerWghts.reserve(m_towerLookup.at(cidx).size()); }

  for ( const auto& elm : m_towerLookup.at(cidx) ) { towerIdx.push_back(towerIndex(elm)); towerWghts.push_back(cellWeight(elm)); }

  return StatusCode::SUCCESS;
}

CaloTowerGeometry::elementvector_t CaloTowerGeometry::getTowers(IdentifierHash cellHash) const
{
  // check input
  uint_t cidx(static_cast<uint_t>(cellHash)); 
  if ( cidx >= m_towerLookup.size() ) { 
    //ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("invalid cell hash %6zu beyond range (max hash is %6zu)",cidx,m_maxCellHash) ); 
    return elementvector_t();
  } else {
    return m_towerLookup.at(cidx);
  }
} 

//-----------------------//
// Tower Geometry Helper //
//-----------------------//

double CaloTowerGeometry::cellWeight(IdentifierHash cellHash,index_t towerIdx) const
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

CaloTowerGeometry::index_t CaloTowerGeometry::etaIndex(IdentifierHash cellHash) const
{
  const auto *const cdde = f_caloDDE(cellHash); 
  return cdde != nullptr ? etaIndex(cdde->eta()) : invalidIndex(); 
}

CaloTowerGeometry::index_t CaloTowerGeometry::etaIndex(double eta) const
{ 
  return eta >= m_towerEtaMin && eta <= m_towerEtaMax 
    ? index_t(std::min(static_cast<uint_t>((eta-m_towerEtaMin)/m_towerEtaWidth),m_towerEtaBins-1))
    : invalidIndex(); 
}

CaloTowerGeometry::index_t CaloTowerGeometry::phiIndex(IdentifierHash cellHash) const
{
  const auto *const cdde = f_caloDDE(cellHash);
  return cdde != nullptr ? phiIndex(cdde->phi()) : invalidIndex(); 
}

CaloTowerGeometry::index_t CaloTowerGeometry::phiIndex(double phi) const
{
  double dphi(CaloPhiRange::diff(phi,m_towerPhiMin));
  return dphi >= m_towerPhiMin && dphi <= m_towerPhiMax 
    ? index_t(std::min(static_cast<uint_t>((phi-m_towerPhiMin)/m_towerPhiWidth),m_towerPhiBins-1))
    : invalidIndex();
}
