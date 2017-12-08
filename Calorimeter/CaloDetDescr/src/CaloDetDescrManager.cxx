/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calorimeter detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloDetDescrManager.cxx,v 1.69 2009-03-03 14:34:12 gunal Exp $
//<version>	$Name: not supported by cvs2svn $


// Calo includes
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

// Athena/Gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include <cmath>

CaloDetDescrManager_Base::CaloDetDescrManager_Base():
  m_cell_id(0),
  m_calo_mgr(0)
{
}

CaloDetDescrManager_Base::~CaloDetDescrManager_Base()
{
  unsigned tile_min = m_subCalo_min[CaloCell_ID::TILE];
  unsigned tile_max = m_subCalo_max[CaloCell_ID::TILE];
  for (size_t i = 0; i < m_element_vec.size(); i++) {
    // Tile elements are deleted by TileDetDescrManager.
    if (i >= tile_min && i < tile_max) continue;
    delete m_element_vec[i];
  }

  for (size_t i = 0; i < m_descr_vec.size(); i++)
    delete m_descr_vec[i];
}

void CaloDetDescrManager_Base::initialize ()
{
  // set sizes :
  m_element_vec.resize(m_cell_id->calo_cell_hash_max());  
  m_descr_vec.resize(m_cell_id->calo_region_hash_max()); 

  // --- --- Temporary, for testing only --- ---
  for(unsigned int i=0; i<m_element_vec.size(); i++)
    m_element_vec[i] = 0;

  for(unsigned int i=0; i<m_descr_vec.size(); i++)
    m_descr_vec[i] = 0;
  // --- --- Temporary, for testing only --- ---

  // cache offsets :
  int nb = CaloCell_ID::NSUBCALO;
  
  m_subCalo_min.resize(nb);
  m_subCalo_max.resize(nb);
  m_subCalo_begin.resize(nb);
  m_subCalo_end.resize(nb);
  
  for(int i = 0; i < nb; i++) {
    m_cell_id->calo_cell_hash_range(i,m_subCalo_min[i],m_subCalo_max[i]);

    m_subCalo_begin[i] = m_element_vec.begin() + m_subCalo_min[i];
    m_subCalo_end[i]   = m_element_vec.begin() + m_subCalo_max[i];
  }
}

void CaloDetDescrManager_Base::print() const
{
  std::cout << " CaloDetDescrManager_Base: \n";
 
  std::cout << " Number of Calo descriptors   : " << m_descr_vec.size() << "\n";

  int ntot = m_descr_vec.size();
  for(int i=0 ; i<ntot; i++ ) 
  {
    std::cout << "\n ------ Calo descriptor number " << i << "\n";
    if(m_descr_vec[i])
      m_descr_vec[i]->print();
  }
}

const CaloCell_Base_ID* CaloDetDescrManager_Base::getCaloCell_ID() const
{
  return m_cell_id;
}

const CaloIdManager* CaloDetDescrManager_Base::getCalo_Mgr() const
{
  return m_calo_mgr;
}

CaloDetDescrManager_Base::calo_element_vec_size
CaloDetDescrManager_Base::element_size() const
{
  return m_element_vec.size();
}

CaloDetDescrManager_Base::calo_element_const_iterator
CaloDetDescrManager_Base::element_begin() const
{
  return m_element_vec.begin();
}
    
CaloDetDescrManager_Base::calo_element_const_iterator
CaloDetDescrManager_Base::element_end() const
{
  return m_element_vec.end();
}
    
CaloDetDescrManager_Base::calo_element_range
CaloDetDescrManager_Base::element_range() const
{
  return calo_element_range (m_element_vec.begin(), 
                             m_element_vec.end());
}
    
CaloDetDescrManager_Base::calo_element_const_iterator
CaloDetDescrManager_Base::element_begin(CaloCell_ID::SUBCALO subCalo) const
{
  int sCal = (int)subCalo;
  return  m_subCalo_begin[sCal];
}
    
CaloDetDescrManager_Base::calo_element_const_iterator
CaloDetDescrManager_Base::element_end(CaloCell_ID::SUBCALO subCalo) const 
{
  int sCal = (int) subCalo;
  return  m_subCalo_end[sCal];
}

CaloDetDescrManager_Base::calo_element_range
CaloDetDescrManager_Base::element_range(CaloCell_ID::SUBCALO subCalo) const 
{
  int sCal = (int) subCalo;
  return calo_element_range (m_subCalo_begin[sCal],
                             m_subCalo_end[sCal]);
}

const CaloDetDescrElement*
CaloDetDescrManager_Base::get_element(const Identifier& cellId) const
{
  IdentifierHash hash = m_cell_id->calo_cell_hash(cellId);
  return get_element(hash);
}

CaloDetDescrElement*
CaloDetDescrManager_Base::get_element_nonconst(const Identifier& cellId)
{
  IdentifierHash hash = m_cell_id->calo_cell_hash(cellId);
  return get_element_nonconst(hash);
}

const CaloDetDescrElement*
CaloDetDescrManager_Base::get_element (CaloCell_ID::SUBCALO subCalo,
                                       const IdentifierHash& subCaloCellHash) const
{
  // meaningless interface for Tiles, will only work for LAr

  int sCal = (int) subCalo;
  if ( subCaloCellHash + m_subCalo_min[sCal] < m_element_vec.size() ) 
    return m_element_vec[subCaloCellHash + m_subCalo_min[sCal]] ;
  else return 0 ;
}  

const CaloDetDescrElement*
CaloDetDescrManager_Base::get_element (CaloCell_ID::SUBCALO subCalo,
                                       int sampling_or_module, 
                                       bool barrel,
                                       double eta, 
                                       double phi) const
{

  bool inCell=false;
  int niter=0;
  const CaloDetDescrElement* elt=0 ;
  const CaloDetDescrElement* elt_best=0 ;
  static CaloPhiRange range;

  double eta2=eta;
  double phi2=phi;
  double deltabest=9999.;
  //std::cout << " in get_element0 " << subCalo << " " << sampling_or_module << " " << barrel << " " << eta << " " << phi << std::endl;


  while (!inCell && niter<3) {

    const CaloDetDescriptor* reg = get_descriptor(subCalo,sampling_or_module,barrel,eta2,phi2);
    if (!reg) return 0;

    if (reg->is_lar_fcal()) return get_element_FCAL(reg,eta,phi);

    int ieta = reg->eta_channel(eta2);
    int iphi = reg->phi_channel(phi2);
    //std::cout << "      ieta,iphi " << ieta << " " << iphi << std::endl;

    if(ieta < 0) return elt;
    if(iphi < 0) return elt;

    Identifier regId = reg->identify();
    Identifier cellId = m_cell_id->cell_id(regId, ieta, iphi); 
    IdentifierHash caloCellHash = m_cell_id->calo_cell_hash(cellId);
    // Check for invalid identifier.
    if (m_cell_id->cell_id (caloCellHash) != cellId) return elt;
    elt = get_element(caloCellHash);
    //std::cout << "    element  " << elt->eta() << " " << elt->phi() << " " << elt->deta() << " " << elt->dphi() << std::endl; 
    double deta=std::fabs(eta-elt->eta())-0.5*elt->deta();
    double dphi = std::fabs(range.fix(phi-elt->phi())) - 0.5*elt->dphi();
    if (deta>0. || dphi > 0.) {
      double delta=0.;
      if (deta>0.) { 
         eta2=eta2 + (eta-elt->eta());
         delta = delta + deta;
      }
      if (dphi>0.) {
         phi2=range.fix(phi2+(phi-elt->phi()));
         delta = delta + dphi;
      }
      //std::cout << "  need to iteration => eta2,phi2 " << eta2 << " " << phi2 << std::endl;
      if (delta<deltabest) {
          deltabest=delta;
          elt_best = elt;
      }
      niter++;
    }
    else inCell=true;
  }
  if (!inCell) {
     elt = elt_best;
    // std::cout << " after iteration get_element0 " << inCell << " " << eta << " " << phi << " " << elt->eta() << " " << elt->phi() << " " << deltabest << std::endl;
  }



  return elt;
}

const CaloDetDescrElement*
CaloDetDescrManager_Base::get_element(CaloCell_ID::CaloSample sample,
                                      double eta, 
                                      double phi) const
{
  const CaloDetDescrElement* elt=0;
  static CaloPhiRange range;
  
  // For LAr loop on regions :

  if ( sample != CaloCell_ID::TileBar0 && sample != CaloCell_ID::TileBar1 &&
       sample != CaloCell_ID::TileBar2 && sample != CaloCell_ID::TileGap1 && 
       sample != CaloCell_ID::TileGap2 && sample != CaloCell_ID::TileGap3 &&
       sample != CaloCell_ID::TileExt0 && sample != CaloCell_ID::TileExt1 &&
       sample != CaloCell_ID::TileExt2 ) {

    bool inCell=false;
    int niter=0;
    const CaloDetDescrElement* elt_best=0;

    double eta2=eta;
    double phi2=phi;
    double deltabest=9999.;

    while (!inCell && niter<3) {

      const CaloDetDescriptor* reg = get_descriptor(sample,eta2,phi2);
      if ( ! reg ) return elt;

      if (reg->is_lar_fcal()) return get_element_FCAL(reg,eta2,phi2);      

      int ieta = reg->eta_channel(eta2);
      int iphi = reg->phi_channel(phi2);

      if(ieta < 0) return elt;
      if(iphi < 0) return elt;

      Identifier regId = reg->identify();
      Identifier cellId = m_cell_id->cell_id(regId, ieta, iphi);  
      IdentifierHash caloCellHash = m_cell_id->calo_cell_hash(cellId);
      // Check for invalid identifier.
      if (m_cell_id->cell_id (caloCellHash) != cellId) return elt;
      elt = get_element(caloCellHash);
      double deta=std::fabs(eta-elt->eta())-0.5*elt->deta();
      double dphi = std::fabs(range.fix(phi-elt->phi())) - 0.5*elt->dphi();
      if (deta>0. || dphi > 0.) {
        double delta=0.;
        if (deta>0.) {
         eta2=eta2 + (eta-elt->eta());
         delta = delta + deta;
        }
        if (dphi>0.) {
          phi2=range.fix(phi2+(phi-elt->phi()));
          delta = delta + dphi;
        }
        if (delta<deltabest) {
           deltabest=delta;
           elt_best = elt;
        }
        niter++;
      }
      else inCell=true;
    }
    if (!inCell) {
      elt = elt_best;
      //std::cout << " after iteration get_element " << inCell << " " << eta << " " << phi << " " << elt->eta() << " " << elt->phi() << " " << deltabest << std::endl;
    }


  } else {

    // For Tiles loop on elements :
  
    for ( unsigned int i = m_subCalo_min[CaloCell_ID::TILE]; 
	  i < m_subCalo_max[CaloCell_ID::TILE]; i++ ) {
      const CaloDetDescrElement* pt = m_element_vec[i];
      if (pt)
	if ( pt->getSampling() == sample &&
	     pt->eta()+pt->deta()/2 <= eta && 
	     pt->eta()-pt->deta()/2 >= eta &&
	     pt->phi()+pt->phi()/2 <= phi && 
	     pt->phi()-pt->phi()/2 >= phi ) elt = pt ; 
    }
  }
  return elt;
}

// Cut and paste, but from raw eta,phi
const CaloDetDescrElement*
CaloDetDescrManager_Base::get_element_raw(CaloCell_ID::CaloSample sample,
                                          double eta, 
                                          double phi) const
{
  //std::cout << " ----- in get_element_raw for eta,phi raw " << eta << " " << phi << std::endl;
  const CaloDetDescrElement* elt=0;
  static CaloPhiRange range;
  
  // For LAr loop on regions :

  if ( sample != CaloCell_ID::TileBar0 && sample != CaloCell_ID::TileBar1 &&
       sample != CaloCell_ID::TileBar2 && sample != CaloCell_ID::TileGap1 && 
       sample != CaloCell_ID::TileGap2 && sample != CaloCell_ID::TileGap3 &&
       sample != CaloCell_ID::TileExt0 && sample != CaloCell_ID::TileExt1 &&
       sample != CaloCell_ID::TileExt2 ) {

    bool inCell=false;
    int niter=0;
    const CaloDetDescrElement* elt_best=0 ;

    double eta2=eta;
    double phi2=phi;
    double deltabest=9999.;

    while (!inCell && niter<3) {

      const CaloDetDescriptor* reg = get_descriptor_raw(sample,eta2,phi2);
      if ( ! reg ) return elt;
      //std::cout << " get_element_raw: found region   calo etamin,etamax " << reg->calo_eta_min() << " " << reg->calo_eta_max() << std::endl;

      if (reg->is_lar_fcal()) return get_element_FCAL_raw(reg,eta,phi);      
 
      int ieta = reg->eta_channel_raw(eta2);
      int iphi = reg->phi_channel_raw(phi2);

      if(ieta < 0) return elt;
      if(iphi < 0) return elt;

      Identifier regId = reg->identify();
      Identifier cellId = m_cell_id->cell_id(regId, ieta, iphi);  
      IdentifierHash caloCellHash = m_cell_id->calo_cell_hash(cellId);
      // Check for invalid identifier.
      if (m_cell_id->cell_id (caloCellHash) != cellId) return elt;
      elt = get_element(caloCellHash);
      //std::cout << "  elt raw,phi raw " << elt->eta_raw() << " " << elt->phi_raw() << std::endl;
      double deta=std::fabs(eta-elt->eta_raw())-0.5*elt->deta();
      double dphi = std::fabs(range.fix(phi-elt->phi_raw())) - 0.5*elt->dphi();
      if (deta>0. || dphi > 0.) {
        double delta=0.;
        if (deta>0.) {
         eta2=eta2 + (eta-elt->eta_raw());
         delta = delta + deta;
        }
        if (dphi>0.) {
          phi2=range.fix(phi2+(phi-elt->phi_raw()));
          delta = delta + dphi;
        }
        if (delta<deltabest) {
           deltabest=delta;
           elt_best = elt;
        }
        niter++;
      }
      else {
      //std::cout << " found cell " << elt->eta_raw() << " " << elt->phi_raw() << std::endl;
      inCell=true;
      }
    }
    if (!inCell) {
      elt=elt_best; 
      //std::cout << " after iteration raw " << inCell << " " << eta << " " << phi << " " << elt->eta_raw() << " " << elt->phi_raw() << " " << deltabest << std::endl;
    }


  } else {

    // For Tiles loop on elements :
  
    for ( unsigned int i = m_subCalo_min[CaloCell_ID::TILE]; 
	  i < m_subCalo_max[CaloCell_ID::TILE]; i++ ) {
      const CaloDetDescrElement* pt = m_element_vec[i];
      if (pt)
	if ( pt->getSampling() == sample &&
	     pt->eta()+pt->deta()/2 <= eta && 
	     pt->eta()-pt->deta()/2 >= eta &&
	     pt->phi()+pt->phi()/2 <= phi && 
	     pt->phi()-pt->phi()/2 >= phi ) elt = pt ; 
    }
  }
  return elt;
}



const CaloDetDescrElement*
CaloDetDescrManager_Base::get_element_FCAL(const CaloDetDescriptor* descr,
                                           double eta,
                                           double phi) const
{
  const CaloDetDescrElement* elt=0;

  //std::cout << " in get_element_FCAL " << descr->reg_min() << " " << descr->reg_max() <<  " eta,phi " << eta << " " << phi << std::endl;
  if (eta < (descr->reg_min()-0.01) || eta > (descr->reg_max()+0.01) ) return elt;

  Identifier regId = descr->identify();
  Identifier cellId;
  IdentifierHash caloCellHash;
  const CaloDetDescrElement* elt2;
  double drmax=9999.;
  for (int ieta=0; ieta<descr->n_eta(); ieta++) {
     for (int iphi=0; iphi<descr->n_phi(); iphi++) {
       cellId = m_cell_id->cell_id(regId, ieta, iphi);
       caloCellHash = m_cell_id->calo_cell_hash(cellId);
       elt2 = get_element (caloCellHash);
       double deta = (eta-elt2->eta());
       double dphi = CaloPhiRange::diff(phi,elt2->phi());
       double dr = (deta*deta+dphi*dphi);
       if (dr<drmax) {
          drmax=dr;
          elt = elt2;
       }
     }
  }

  //if (elt) std::cout << " element found " << elt->eta() << " " << elt->phi() << std::endl;
  //else     std::cout <<"  element not found " << std::endl;

  return elt;
}

const CaloDetDescrElement*
CaloDetDescrManager_Base::get_element_FCAL_raw(const CaloDetDescriptor* descr,
                                               double eta,
                                               double phi) const
{
  const CaloDetDescrElement* elt=0;

  //std::cout << " in get_element_FCAL " << descr->reg_min() << " " << descr->reg_max() <<  " eta,phi " << eta << " " << phi << std::endl;
  if (std::fabs(eta) < (descr->calo_eta_min()-0.01) || std::fabs(eta) > (descr->calo_eta_max()+0.01) ) return elt;

  Identifier regId = descr->identify();
  Identifier cellId;
  IdentifierHash caloCellHash;
  const CaloDetDescrElement* elt2;
  double drmax=9999.;
  for (int ieta=0; ieta<descr->n_eta(); ieta++) {
     for (int iphi=0; iphi<descr->n_phi(); iphi++) {
       cellId = m_cell_id->cell_id(regId, ieta, iphi);
       caloCellHash = m_cell_id->calo_cell_hash(cellId);
       elt2 = get_element (caloCellHash);
       double deta = (eta-elt2->eta_raw());
       double dphi = CaloPhiRange::diff(phi,elt2->phi_raw());
       double dr = (deta*deta+dphi*dphi);
       if (dr<drmax) {
          drmax=dr;
          elt = elt2;
       }
     }
  }

  //if (elt) std::cout << " element found " << elt->eta() << " " << elt->phi() << std::endl;
  //else     std::cout <<"  element not found " << std::endl;

  return elt;
}



CaloDetDescrManager_Base::calo_descr_const_iterator
CaloDetDescrManager_Base::calo_descriptors_begin() const
{
  return m_descr_vec.begin();
}

CaloDetDescrManager_Base::calo_descr_const_iterator
CaloDetDescrManager_Base::calo_descriptors_end() const
{
  return m_descr_vec.end();
}

CaloDetDescrManager_Base::calo_descr_size
CaloDetDescrManager_Base::calo_descriptors_size() const
{
  return m_descr_vec.size();
}

CaloDetDescrManager_Base::calo_descr_range
CaloDetDescrManager_Base::calo_descriptors_range() const
{
  return calo_descr_range (m_descr_vec.begin(), m_descr_vec.end());
}

CaloDetDescrManager_Base::calo_descr_const_iterator
CaloDetDescrManager_Base::tile_descriptors_begin() const
{
  return m_tile_descr_vec.begin();
}
 
CaloDetDescrManager_Base::calo_descr_const_iterator
CaloDetDescrManager_Base::tile_descriptors_end() const
{
  return m_tile_descr_vec.end();
}
 
CaloDetDescrManager_Base::calo_descr_size
CaloDetDescrManager_Base::tile_descriptors_size() const
{
  return m_tile_descr_vec.size();
}

CaloDetDescrManager_Base::calo_descr_range
CaloDetDescrManager_Base::tile_descriptors_range() const
{
  return calo_descr_range (m_tile_descr_vec.begin(),
                           m_tile_descr_vec.end());
}
 
const CaloDetDescriptor*
CaloDetDescrManager_Base::get_descriptor(const Identifier& regionId) const
{
  IdentifierHash hash = m_cell_id->calo_region_hash(regionId);
  if(hash < m_descr_vec.size()) 
    return m_descr_vec[hash] ;
  else 
    return 0;
}

CaloDetDescriptor*
CaloDetDescrManager_Base::get_descriptor_nonconst(const Identifier& regionId)
{
  IdentifierHash hash = m_cell_id->calo_region_hash(regionId);
  if(hash < m_descr_vec.size()) 
    return m_descr_vec[hash] ;
  else 
    return 0;
}

const CaloDetDescriptor*
CaloDetDescrManager_Base::get_descriptor(CaloCell_ID::SUBCALO subCalo,
                                         int sampling_or_module, 
                                         bool barrel,
                                         double eta, 
                                         double phi) const
{
  const CaloDetDescriptor* desc0 = 0;
  const CaloDetDescriptor* desc1 = 0;
  const CaloDetDescriptor* desc2 = 0;
  const CaloDetDescriptor* desc3 = 0;

  if(subCalo == CaloCell_ID::TILE) return 0;
  if(subCalo == CaloCell_ID::LARHEC && barrel == true) return 0;
  if(subCalo == CaloCell_ID::LARFCAL && barrel == true) return 0;
   
  for (unsigned int i=0; i<m_descr_vec.size(); i++) 
  {
    const CaloDetDescriptor* reg = m_descr_vec[i];

    if(reg) 
    {
      
      // for em differentiate barrel from endcap :
      if ((subCalo != CaloCell_ID::LAREM 
	   || (barrel == false && reg->is_lar_em_endcap()) 
	   || (barrel == true && reg->is_lar_em_barrel())
	   )
	  && ((subCalo == CaloCell_ID::LAREM && reg->is_lar_em())
	      || (subCalo == CaloCell_ID::LARHEC && reg->is_lar_hec())
	      || (subCalo == CaloCell_ID::LARFCAL && reg->is_lar_fcal())
	      )
	  )
      {

	if(reg->eta_channel(eta) >=0 && reg->phi_channel(phi) >=0) 
	{
	  // this works only for LAr, for Tiles the sampling depends on the
	  // cell
	  int reg_sampl = reg->getSampling(0);
	  // using enums as running indices is not safe! Therefore ask
	  // for each sampling explicitly
	  if(reg->is_lar_em_barrel()) 
	  {
	    if(reg_sampl == CaloCell_ID::PreSamplerB) desc0 = reg;	   
	    else if(reg_sampl == CaloCell_ID::EMB1) desc1 = reg;	   
	    else if(reg_sampl == CaloCell_ID::EMB2) desc2 = reg;	   
	    else if(reg_sampl == CaloCell_ID::EMB3) desc3 = reg;	   
	  }
	  else if(reg->is_lar_em_endcap()) 
	  {
	    if(reg_sampl == CaloCell_ID::PreSamplerE) desc0 = reg;	   
	    else if(reg_sampl == CaloCell_ID::EME1) desc1 = reg;	   
	    else if(reg_sampl == CaloCell_ID::EME2) desc2 = reg;	   
	    else if(reg_sampl == CaloCell_ID::EME3) desc3 = reg;	   
	  }
	  else if(reg->is_lar_hec()) 
	  {
	    if(reg_sampl == CaloCell_ID::HEC0) desc0 = reg;	   
	    else if(reg_sampl == CaloCell_ID::HEC1) desc1 = reg;	   
	    else if(reg_sampl == CaloCell_ID::HEC2) desc2 = reg;	   
	    else if(reg_sampl == CaloCell_ID::HEC3) desc3 = reg;	   
	  }
	  else if(reg->is_lar_fcal()) 
	  {
	    if(reg_sampl == CaloCell_ID::FCAL0) desc0  = reg;	   
	    else if(reg_sampl == CaloCell_ID::FCAL1) desc1 = reg;	   
	    else if(reg_sampl == CaloCell_ID::FCAL2) desc2 = reg;	   
	  }
	} //if(reg->eta_channel(eta) >=0 && reg->phi_channel(phi) >=0)
      }
    } //if(reg)
  } // iteration over descriptors

  if(sampling_or_module == 0) return desc0;
  else if(sampling_or_module == 1) return desc1;
  else if(sampling_or_module == 2) return desc2;
  else if(sampling_or_module == 3) return desc3;
  else return 0;
}

const CaloDetDescriptor*
CaloDetDescrManager_Base::get_descriptor (CaloCell_ID::CaloSample sample,
                                          double eta, double phi) const
{
  // note that this code does not work in the FCal as eta indices depend
  // on eta and phi

  const CaloDetDescriptor* desc = 0;

  if ( sample  == CaloCell_ID::TileBar0 ||
       sample  == CaloCell_ID::TileBar1 ||
       sample  == CaloCell_ID::TileBar2 ||
       sample  == CaloCell_ID::TileGap1 ||
       sample  == CaloCell_ID::TileGap2 ||
       sample  == CaloCell_ID::TileGap3 ||
       sample  == CaloCell_ID::TileExt0 ||
       sample  == CaloCell_ID::TileExt1 ||
       sample  == CaloCell_ID::TileExt2 )  return desc;

  for ( unsigned int i=0; i<m_descr_vec.size(); i++ ) {
    const CaloDetDescriptor* reg = m_descr_vec[i];

    if (reg) {
      if ( reg->eta_channel(eta) >=0 && reg->phi_channel(phi) >=0) {
        // this works only for LAr EM and HEC, for Tiles the sampling
        // depends on the cell
        int reg_sampl = reg->getSampling(0);
        if ( reg_sampl == sample ) desc  = reg;
      }
    }
  }

  return desc;
}

const CaloDetDescriptor*
CaloDetDescrManager_Base::get_descriptor_raw (CaloCell_ID::CaloSample sample, 
                                              double eta, double phi) const
{
  // note that this code does not work in the FCal as eta indices depend
  // on eta and phi 

  //std::cout << " in CaloDetDescrManager_Base::get_descriptor_raw " << std::endl;

  const CaloDetDescriptor* desc = 0;

  if ( sample  == CaloCell_ID::TileBar0 ||
       sample  == CaloCell_ID::TileBar1 ||
       sample  == CaloCell_ID::TileBar2 ||
       sample  == CaloCell_ID::TileGap1 ||
       sample  == CaloCell_ID::TileGap2 ||
       sample  == CaloCell_ID::TileGap3 ||
       sample  == CaloCell_ID::TileExt0 ||
       sample  == CaloCell_ID::TileExt1 ||
       sample  == CaloCell_ID::TileExt2 )  return desc;

  for ( unsigned int i=0; i<m_descr_vec.size(); i++ ) { 
    const CaloDetDescriptor* reg = m_descr_vec[i];

    if (reg) { 
      int reg_sampl = reg->getSampling(0);
      if ( reg_sampl == sample ) {
        if ( reg->eta_channel_raw(eta) >=0 && reg->phi_channel_raw(phi) >=0) {
        // this works only for LAr EM and HEC, for Tiles the sampling
        // depends on the cell
           desc=reg;
           return desc;
        }
      }
    }
  }

  return desc;
}


void CaloDetDescrManager_Base::add(CaloDetDescrElement* element)
{
  m_element_vec[element->calo_hash()] = element;
}

void CaloDetDescrManager_Base::add(CaloDetDescriptor* descr)
{
    m_descr_vec[descr->calo_hash()] = descr;
}

void CaloDetDescrManager_Base::add_tile(CaloDetDescriptor* descr)
{
  m_tile_descr_vec.push_back(descr);
}

CaloDetDescrElement* CaloDetDescrManager_Base::release_element (IdentifierHash hash)
{
  CaloDetDescrElement* old = m_element_vec[hash];
  m_element_vec[hash] = nullptr;
  return old;
}

void CaloDetDescrManager_Base::set_helper(const CaloCell_Base_ID*  idHelper)
{
    m_cell_id = idHelper;
}

void CaloDetDescrManager_Base::set_helper(const CaloIdManager* idHelper)
{
    m_calo_mgr = idHelper;
}

void CaloDetDescrManager_Base::cellsInZone(double eta_min, double eta_max,
                                           double phi_min, double phi_max,
                                           std::vector<IdentifierHash> & cell_list ) const
{
  cell_list.clear();
  if ( eta_min >= eta_max ) return;
  if ( phi_min >= phi_max ) return;

  std::vector<IdentifierHash> one_list;

  // For LAr use the descriptors :
  for ( unsigned int i=0; i<m_descr_vec.size(); i++ ) {
    const CaloDetDescriptor* reg = m_descr_vec[i];
    if ( reg ) {
      cellsInZone(eta_min, eta_max, phi_min, phi_max, reg, one_list);
      if (cell_list.empty())
        cell_list.swap (one_list);
      else
        cell_list.insert (cell_list.end(), one_list.begin(), one_list.end());
    }
  }

  // Now Tiles :
  cellsInZone(eta_min, eta_max, phi_min, phi_max, CaloCell_ID::TILE, one_list);
  cell_list.insert (cell_list.end(), one_list.begin(), one_list.end());
}

void CaloDetDescrManager_Base::cellsInZone(double eta_min, double eta_max,
                                           double phi_min, double phi_max,
                                           CaloCell_ID::SUBCALO subCalo,
                                           std::vector<IdentifierHash> & cell_list) const
{
  cell_list.clear();
  if ( eta_min >= eta_max ) return;
  if ( phi_min >= phi_max ) return;

  std::vector<IdentifierHash> one_list;


  if ( subCalo != CaloCell_ID::TILE ) {

    for ( unsigned int i=0; i<m_descr_vec.size(); i++ ) {
      const CaloDetDescriptor* reg = m_descr_vec[i];
      if ( reg )
        if ( subCalo == reg->getSubCalo() ) {
          cellsInZone(eta_min, eta_max, phi_min, phi_max, reg, one_list);
          if (cell_list.empty())
            cell_list.swap (one_list);
          else
            cell_list.insert (cell_list.end(),
                              one_list.begin(), one_list.end());
        }
    }

  } else {

    // For Tiles loop on elements :
    const CaloDetDescrElement* tile_elt;
    for ( unsigned int i = m_subCalo_min[subCalo];
          i < m_subCalo_max[subCalo]; i++ ) {
      tile_elt = m_element_vec[i];
      if (tile_elt) {

        if ( tile_elt->eta()+tile_elt->deta()/2. > eta_min &&
             tile_elt->eta()-tile_elt->deta()/2. < eta_max) {
 
          double dphi1 = CaloPhiRange::diff(tile_elt->phi()+tile_elt->dphi()/2.,phi_min);
          double dphi2 = CaloPhiRange::diff(phi_max,tile_elt->phi()-tile_elt->dphi()/2);

          if(dphi1 >=0.  && dphi2 >= 0.)  {
	     if (cell_list.empty()) {
	     // Guess a good size, incl 3 samplings.
	     cell_list.reserve (static_cast<int>(
                              ((eta_max-eta_min) / tile_elt->deta()) *
                              ((phi_max-phi_min) / tile_elt->dphi()) * 3));
	     }
	     cell_list.push_back(tile_elt->calo_hash());
	  }  // phi cut
        }    // eta cut
      }      // tile_elt exits
    }
  }
}

void CaloDetDescrManager_Base::cellsInZone(double eta_min, double eta_max,
                                           double phi_min, double phi_max,
                                           CaloCell_ID::CaloSample sample,
                                           std::vector<IdentifierHash> & cell_list) const
{
  cell_list.clear();
  if ( eta_min >= eta_max ) return;
  if ( phi_min >= phi_max ) return;

  std::vector<IdentifierHash> one_list;

  if ( sample != CaloCell_ID::TileBar0 &&
       sample != CaloCell_ID::TileBar1 &&
       sample != CaloCell_ID::TileBar2 &&
       sample != CaloCell_ID::TileGap1 &&
       sample != CaloCell_ID::TileGap2 &&
       sample != CaloCell_ID::TileGap3 &&
       sample != CaloCell_ID::TileExt0 &&
       sample != CaloCell_ID::TileExt1 &&
       sample != CaloCell_ID::TileExt2    ) {

    for ( unsigned int i=0; i<m_descr_vec.size(); i++ ) {
      const CaloDetDescriptor* reg = m_descr_vec[i];
      if ( reg )
        // this works only for LAr, for Tiles the sampling depends on the
        // cell
        if ( sample == reg->getSampling(0) ) {
          cellsInZone(eta_min, eta_max, phi_min, phi_max, reg, one_list);
          if (cell_list.empty())
            cell_list.swap (one_list);
          else
            cell_list.insert (cell_list.end(),
                              one_list.begin(), one_list.end());
        }
    }

  } else {

    // For Tiles loop on elements :
    int nb = CaloCell_ID::TILE;
    const CaloDetDescrElement* tile_elt;

    for ( unsigned int i = m_subCalo_min[nb]; i < m_subCalo_max[nb]; i++ ) {
      tile_elt = m_element_vec[i];
      if (tile_elt ){
        if (tile_elt->getSampling() == sample) {

          if ( tile_elt->eta()+tile_elt->deta()/2. > eta_min &&
               tile_elt->eta()-tile_elt->deta()/2. < eta_max) {
 
            double dphi1 = CaloPhiRange::diff(tile_elt->phi()+tile_elt->dphi()/2.,phi_min);
            double dphi2 = CaloPhiRange::diff(phi_max,tile_elt->phi()-tile_elt->dphi()/2);

            if(dphi1 >=0.  && dphi2 >= 0.)  {
               if (cell_list.empty()) {
               // Guess a good size, incl 3 samplings.
               cell_list.reserve (static_cast<int>(
                                ((eta_max-eta_min) / tile_elt->deta()) *
                                ((phi_max-phi_min) / tile_elt->dphi()) * 3));
               }       
               cell_list.push_back(tile_elt->calo_hash());
            }  // phi cut
          }    // eta cut
        }      // sampling cut
      }      // tile_elt exists
    }
  }
}

void CaloDetDescrManager_Base::cellsInZone(double eta_min, double eta_max,
                                           double phi_min, double phi_max,
                                           CaloCell_ID::SUBCALO subCalo,
                                           int sampling_or_module,
                                           std::vector<IdentifierHash> & cell_list) const
{
  cell_list.clear();
  if ( eta_min >= eta_max ) return;
  if ( phi_min >= phi_max ) return;

  // sampling_or_module has no meaning for tiles
  if ( subCalo == CaloCell_ID::TILE ) return;

  std::vector<IdentifierHash> one_list;

  for ( unsigned int i=0; i<m_descr_vec.size(); i++ ) {
    const CaloDetDescriptor* reg = m_descr_vec[i];
    if ( reg ) {
      Identifier regId = reg->identify();
      if ( subCalo == reg->getSubCalo() &&
           m_cell_id->sampling(regId) == sampling_or_module ) {
        cellsInZone(eta_min, eta_max, phi_min, phi_max, reg, one_list);
        if (cell_list.empty())
          cell_list.swap (one_list);
        else
          cell_list.insert (cell_list.end(),
                            one_list.begin(), one_list.end());
      }
    }
  }
}

void
CaloDetDescrManager_Base::cellsInZone(double eta, double phi, int ncell_eta,
                                      int ncell_phi, CaloCell_ID::SUBCALO subCalo,
                                      int sampling_or_module,
                                      bool barrel,
                                      std::vector<IdentifierHash> & cell_list) const
{

  cell_list.clear();
  double eta_min, eta_max, phi_min, phi_max;
  double weta = (ncell_eta-1)/2.;
  double wphi = (ncell_phi-1)/2.;

  std::vector<IdentifierHash> one_list;

  if ( subCalo != CaloCell_ID::TILE ) {

    for ( unsigned int i=0; i<m_descr_vec.size(); i++ ) {
      const CaloDetDescriptor* reg = m_descr_vec[i];
      if ( reg ) {
        Identifier regId = reg->identify();
        if ( subCalo == reg->getSubCalo() &&
             m_cell_id->sampling(regId) == sampling_or_module) {

          double eta_gran = m_cell_id->etaGranularity(regId);
          double phi_gran = m_cell_id->phiGranularity(regId);

          eta_min = eta-weta*eta_gran;
          eta_max = eta+weta*eta_gran;
          phi_min = phi-wphi*phi_gran;
          phi_max = phi+wphi*phi_gran;
          cellsInZone(eta_min, eta_max, phi_min, phi_max, reg, one_list);
          cell_list.reserve (cell_list.size() + one_list.size());
          for ( unsigned int i = 0; i<one_list.size(); i++ ){
            // I know that cellsInZone catches more cells than needed
            // => look and cut
            const CaloDetDescrElement* elt = get_element (one_list[i]);
            double elt_eta = elt->eta();
            double elt_phi = elt->phi();
            if ( barrel == elt->is_lar_em_barrel() )
              if ( std::abs(eta - elt_eta) <= weta )
                if ( std::abs(phi - elt_phi) <= wphi )
                  cell_list.push_back(one_list[i]);
          }
        }
      }
    }
  }
  else {

    // For Tiles loop on elements :
    // loosy logic. since I do not know the grannularity in the
    // region, I use the one of each element. Hum...
    const CaloDetDescrElement* tile_elt;
    for ( unsigned int i = m_subCalo_min[subCalo];
          i < m_subCalo_max[subCalo]; i++ ) {
      tile_elt = m_element_vec[i];
      if (tile_elt) {
        eta_min = eta-weta*tile_elt->deta();
        eta_max = eta+weta*tile_elt->deta();

        if ( tile_elt->eta()+tile_elt->deta()/2. > eta_min &&
           tile_elt->eta()-tile_elt->deta()/2. < eta_max) {
          phi_min = phi-wphi*tile_elt->dphi();
          phi_max = phi+wphi*tile_elt->dphi();
          double dphi1 = CaloPhiRange::diff(tile_elt->phi()+tile_elt->dphi()/2.,phi_min);
          double dphi2 = CaloPhiRange::diff(phi_max,tile_elt->phi()-tile_elt->dphi()/2);

          if(dphi1 >=0. && dphi2 >= 0. ) {
	    if (cell_list.empty()) {
	      // Guess a good size, incl 3 samplings.
	      cell_list.reserve (static_cast<int> (
                               ((eta_max-eta_min) / tile_elt->deta()) *
                               ((phi_max-phi_min) / tile_elt->dphi()) * 3));
	    }
	    cell_list.push_back(tile_elt->calo_hash());
	  }   // phi cut
        }     // eta cut
      }       // tile elt exisits
    }
  }

}

void CaloDetDescrManager_Base::cellsInZone(double eta_min, double eta_max,
                                           double phi_min, double phi_max,
                                           const CaloDetDescriptor* descr,
                                           std::vector<IdentifierHash> & cell_list) const
{

  cell_list.clear();

  if ( ! descr ) return;
  if ( eta_min >= eta_max ) return;

// FCAL case : explicit loop over all cells to account for the fact that eta and phi are not independent

  if (descr->is_lar_fcal()) {
     //   std::cout << " in FCAl case  descr etamin/max " << descr->reg_min() << " " << descr->reg_max() <<
     //    "   zone etamin/max " << eta_min << " " << eta_max << std::endl;
      if (descr->reg_min() > eta_max ||  descr->reg_max() < eta_min) return;
      Identifier regId = descr->identify();
      Identifier cellId;
      IdentifierHash caloCellHash;
      const CaloDetDescrElement* elt;
      IdentifierHash prevHash(0);
      // std::cout <<"    neta,nphi " << descr->n_eta() << " " << descr->n_phi() << std::endl;
      for (int ieta=0; ieta<descr->n_eta(); ieta++) {
         for (int iphi=0; iphi<descr->n_phi(); iphi++) {
           cellId = m_cell_id->cell_id(regId, ieta, iphi);
           caloCellHash = m_cell_id->calo_cell_hash(cellId);
           // this is required because otherwise caloCellHash of unconnected channels with be the same as some connected channels (
           if (caloCellHash != prevHash) {
             prevHash=caloCellHash;
             elt = get_element (caloCellHash);
             if (elt) {
                if ( elt->eta()+elt->deta()/2. > eta_min &&
                     elt->eta()-elt->deta()/2. < eta_max) {

                   double dphi1 = CaloPhiRange::diff(elt->phi()+elt->dphi()/2.,phi_min);
                   double dphi2 = CaloPhiRange::diff(phi_max,elt->phi()-elt->dphi()/2);

                   if(dphi1 >=0.  && dphi2 >= 0.)  {
                     cell_list.push_back(caloCellHash);
                     //std::cout << " elt in ! eta= " << elt->eta() << " phi= " << elt->phi()  << " " << ieta << " " << iphi << "hash Id " << caloCellHash << " id " << cellId << std::endl;
                   }  // phi cut
                }    // eta cut
             }       // if(elt)
             //else {
             //   std::cout << " no FCAL elt for this cell " << ieta << " " << iphi << " " << cellId << " " << caloCellHash << std::endl;
             // }
           } 
         }    // loop over iphi
      }       // loop over iea
  }

// everything but FCAL
  else {
    if ( ! descr->is_in(eta_min,eta_max,phi_min,phi_max)) return;

    double zone_min = eta_min;
    double zone_max = eta_max;
    int ieta_min = 0;
    int ieta_max = 0;

    // loop will start at ieta_min-1 (included) and finish at ieta_max+1 (included)
    // ==> be careful at the edges of the descriptor.
    // check on which side we are ( for eta<0 channel ordering is swapped )
    if ( descr->calo_sign() > 0 ) {

      if ( descr->reg_min() >= zone_min ){
        zone_min = descr->reg_min();
        ieta_min = 1;
        //std::cout << " POS ! setting zone_min to reg_min = " << zone_min << " and ieta_min to " << ieta_min << std::endl;
      }
      else {
        ieta_min = descr->eta_channel(zone_min);
        if (ieta_min < 0)
          ieta_min = 1;
      }

      if ( descr->reg_max() <= zone_max ) {
        zone_max = descr->reg_max();
        ieta_max = descr->n_eta() -1;
        //std::cout << " POS ! setting zone_max to reg_max = " << zone_max << " and ieta_max to " << ieta_max << std::endl;
      }
      else {
        ieta_max = descr->eta_channel(zone_max);
        if (ieta_max < 1)
          ieta_max = descr->n_eta() - 1;
      }

    }
    else {

      if ( descr->reg_min() >= zone_min ){
        zone_min = descr->reg_min();
        ieta_max = descr->n_eta() - 1;
        //std::cout << " NEG ! setting zone_min to reg_min = " << zone_min << " and ieta_max to " << ieta_max << std::endl;
      }
      else {
        ieta_max = descr->eta_channel(zone_min);
        if (ieta_max < 1)
          ieta_max = descr->n_eta() - 1;
      }

      if ( descr->reg_max() <= zone_max ) {
        zone_max = descr->reg_max();
        ieta_min = 1 ;
        //std::cout << " NEG ! setting zone_max to reg_max = " << zone_max << " and ieta_min to " << ieta_min << std::endl;
      }
      else {
        ieta_min = descr->eta_channel(zone_max - 0.0001);
        if (ieta_min < 0)
          ieta_min = 1;
      }

    }


  // phi :

  // for a full detector, the window limits cannot be outside... BUT :
  //      for zones where phi_min is < 0 and phi_max > 0  ( around 0 ),
  //      or phi_max is < 0 ( around M_PI )
  //      the channel number iphi_min is larger than iphi_max and
  //      the search will have to be done in 2 steps.

  // for a subset ( H8 or H6 testbeams ), if the window is larger than the
  //      module the min/max channel numbers will be -1 => mess in loops
  //      ==> reduce the window size to the module size


    double zone_phi_min = CaloPhiRange::fix (phi_min);
    double zone_phi_max = CaloPhiRange::fix (phi_max);

    //    *** *** *** TEST BEAM!!! *** *** ***
    if (m_lar_geometry == "H8" || m_lar_geometry == "H6") {
      double margin = descr->dphi()*0.1;

      // FIXME : ideal values are used here, this should NOT be !!!!

      if ( descr->calo_phi_min() > zone_phi_min) zone_phi_min = descr->calo_phi_min() + margin;
      if ( descr->calo_phi_max() < zone_phi_max) zone_phi_max = descr->calo_phi_max() - margin;
    }

    int iphi_min = descr->phi_channel(zone_phi_min);
    int iphi_max = descr->phi_channel(zone_phi_max);

// add more safety in case of strips to protect against misalignment
    int reg_sampl=descr->getSampling(0);
    if (reg_sampl == CaloCell_ID::EMB1 || reg_sampl == CaloCell_ID::EME1) {
       ieta_min = ieta_min -1;
       ieta_max = ieta_max +1;
    }


    Identifier regId = descr->identify();
    Identifier cellId;
    IdentifierHash caloCellHash;
    const CaloDetDescrElement* elt;

  
    /* 
    std::cout << " cellsInZone : eta ! reg " << descr->reg_min() << " " << descr->reg_max()
              << " zone " << zone_min << " " << zone_max
              << " channels " << ieta_min << " " << ieta_max << std::endl;

    std::cout << " cellsInZone : phi ! reg " << descr->calo_phi_min() << " " << descr->calo_phi_max()
              << " zone " << zone_phi_min << " " << zone_phi_max
              << " channels " << iphi_min << " " << iphi_max << std::endl;
    */
   

    int reg_EtaMin = m_cell_id->eta_min (regId);
    int reg_EtaMax = m_cell_id->eta_max (regId);
    int reg_PhiMin = m_cell_id->phi_min (regId);
    if (reg_PhiMin<0) reg_PhiMin=0;      // catch case where it is not defined for Tile
    int reg_PhiMax = m_cell_id->phi_max (regId);
  

    if ( iphi_min <= iphi_max && zone_phi_min <= zone_phi_max ) {

      // The easy case : just make list a bit larger to avoid rounding effects

      for ( int ieta = ieta_min-1 ; ieta <= ieta_max+1; ieta++ )
        for ( int iphi = iphi_min-1 ; iphi <= iphi_max+1; iphi++ )
          {
            if (ieta>=reg_EtaMin && ieta <=reg_EtaMax && iphi>=reg_PhiMin && iphi<=reg_PhiMax) {
              cellId = m_cell_id->cell_id(regId, ieta, iphi);
              caloCellHash = m_cell_id->calo_cell_hash(cellId);
              elt = get_element (caloCellHash);
              //std::cout << " case 1 : for ieta = " << ieta << "  iphi = " << iphi << std::endl;
              if (elt
                && ( (elt->eta()+elt->deta()/2.) > zone_min + 0.00001 )
                && ( (elt->eta()-elt->deta()/2.) + 0.00001 < zone_max )
                && ( (elt->phi()+elt->dphi()/2.) > zone_phi_min + 0.00001  )
                && ( (elt->phi()-elt->dphi()/2.) + 0.00001 < zone_phi_max  )  )
              {
                cell_list.push_back(caloCellHash);
                //std::cout << " elt in ! eta= " << elt->eta() << " phi= " << elt->phi() << " " << ieta << " " << iphi << std::endl;
              }
              //else
                //std::cout << " elt out ! eta= " << elt->eta() << " phi= " << elt->phi() << std::endl;
            }
          }
    }
    else if ( iphi_min <= iphi_max && zone_phi_min > zone_phi_max ) {

      // we are around the -M_PI -> +M_PI discontinuity  , no zone_phi_max cut

      for ( int ieta = ieta_min-1 ; ieta <= ieta_max+1; ieta++ )
        for ( int iphi = iphi_min-1 ; iphi <= iphi_max+1; iphi++ )
          {
            if (ieta>=reg_EtaMin && ieta <=reg_EtaMax && iphi>=reg_PhiMin && iphi<=reg_PhiMax) {
              cellId = m_cell_id->cell_id(regId, ieta, iphi);
              caloCellHash = m_cell_id->calo_cell_hash(cellId);
              elt = get_element (caloCellHash);
              //std::cout << " case 2 : for ieta = " << ieta << "  iphi = " << iphi << std::endl;
              if (elt
                && ( (elt->eta()+elt->deta()/2.) > zone_min + 0.00001 )
                && ( (elt->eta()-elt->deta()/2.) + 0.00001 < zone_max )
                && ( ( (elt->phi()+elt->dphi()/2.) > zone_phi_min + 0.00001 )
                     || ( (elt->phi()-elt->dphi()/2.) + 0.00001 < zone_phi_max ) )
                )
              {
                cell_list.push_back(caloCellHash);
                //std::cout << " elt in ! eta= " << elt->eta() << " phi= " << elt->phi()  << " " << ieta << " " << iphi << std::endl;
              }
              //else
                //std::cout << " elt out ! eta= " << elt->eta() << " phi= " << elt->phi() << std::endl;
            }
          }
    }
    else {

      // iphi_min > iphi_max => we are around phi = 0

      int nchmax = m_cell_id->phi_max(regId);
      // number of cells = nchmax + 1 ==> 0 to nchmax included

      for ( int ieta = ieta_min-1 ; ieta <= ieta_max+1; ieta++ ) {

        // first cells from min up to 2*M_PI
        for ( int iphi = iphi_min-1 ; iphi <= nchmax; iphi++ )
          {
            if (ieta>=reg_EtaMin && ieta <=reg_EtaMax && iphi>=reg_PhiMin && iphi<=reg_PhiMax) {
              cellId = m_cell_id->cell_id(regId, ieta, iphi);
              caloCellHash = m_cell_id->calo_cell_hash(cellId);
              elt = get_element (caloCellHash);
              //std::cout << " case 3 : for ieta = " << ieta << "  iphi = " << iphi << std::endl;
              if (elt
                && ( (elt->eta()+elt->deta()/2.) > zone_min + 0.00001 )
                && ( (elt->eta()-elt->deta()/2.) + 0.00001 < zone_max )
                && ( (elt->phi()+elt->dphi()/2.) > zone_phi_min + 0.00001  )
                && ( (elt->phi()-elt->dphi()/2.) + 0.00001 < zone_phi_max  )  )
              {
                 cell_list.push_back(caloCellHash);
                //std::cout << " elt in ! eta= " << elt->eta() << " phi= " << elt->phi()  << " " << ieta << " " << iphi << std::endl;
              }
              //else
              //std::cout << " elt out ! eta= " << elt->eta() << " phi= " << elt->phi() << std::endl;
            }
          }
        // now cells from 0 to min
        for ( int iphi = 0 ; iphi <= iphi_max+1; iphi++ )
          {
            if (ieta>=reg_EtaMin && ieta <=reg_EtaMax && iphi>=reg_PhiMin && iphi<=reg_PhiMax) {
              cellId = m_cell_id->cell_id(regId, ieta, iphi);
              caloCellHash = m_cell_id->calo_cell_hash(cellId);
              elt = get_element (caloCellHash);
              //std::cout << " case 4 : for ieta = " << ieta << "  iphi = " << iphi << std::endl;
              if (elt
                && ( (elt->eta()+elt->deta()/2.) > zone_min + 0.00001 )
                && ( (elt->eta()-elt->deta()/2.) + 0.00001 < zone_max )
                && ( (elt->phi()+elt->dphi()/2.) > zone_phi_min + 0.00001  )
                && ( (elt->phi()-elt->dphi()/2.) + 0.00001 < zone_phi_max  )  )
              {
                cell_list.push_back(caloCellHash);
                //std::cout << " elt in ! eta= " << elt->eta() << " phi= " << elt->phi()  << " " << ieta << " " << iphi << std::endl;
              }
              //else
              //std::cout << " elt out ! eta= " << elt->eta() << " phi= " << elt->phi() << std::endl;
            }
          }
      }
    }
  }    // if else FCAL
}

bool CaloDetDescrManager_Base::is_in (double eta, double phi, CaloCell_ID::CaloSample sample, double& etadist )
const
{
  // note that this code does not work in the FCal as eta indices depend
  // on eta and phi

  // I'm looking for the distance between the eta direction and the closest edge.
  // all in absolute value --> the sign will indicate if it is in or out.

  bool result = false;
  etadist = 9999999.;
  double aeta = std::abs(eta);

  // Tile first
  // Treat Tile Barrel and End Cap in different ways
  // -- Barrel
  if(sample == CaloCell_ID::TileBar0 ||
     sample == CaloCell_ID::TileBar1 ||
     sample == CaloCell_ID::TileBar2)
  {
    for(unsigned int i=0; i<m_tile_descr_vec.size(); i++)
    {
      const CaloDetDescriptor* reg = m_tile_descr_vec[i];

      if(reg && reg->getSampling(0) == sample)
      {
	etadist = std::abs(aeta - reg->calo_eta_max());
	if(aeta <= reg->calo_eta_max()) result = true;
	break;
      }
    }
  }
  // End Cap
  else if(sample == CaloCell_ID::TileExt0 ||
	  sample == CaloCell_ID::TileExt1 ||
	  sample == CaloCell_ID::TileExt2 ||    
	  sample == CaloCell_ID::TileGap1 ||
	  sample == CaloCell_ID::TileGap2 ||
	  sample == CaloCell_ID::TileGap3)
  {
    for(unsigned int i=0; i<m_tile_descr_vec.size(); i++)
    {
      const CaloDetDescriptor* reg = m_tile_descr_vec[i];

      if(reg && reg->getSampling(0) == sample)
      {
	double edgemin = std::abs(aeta - reg->calo_eta_min());
	double edgemax = std::abs(aeta - reg->calo_eta_max());
	if(edgemin < etadist) etadist = edgemin;
	if(edgemax < etadist) etadist = edgemax;

	if(aeta >= reg->calo_eta_min() &&
	   aeta <= reg->calo_eta_max()) result = true;
	break;
      }
    }
  }
  else {
    for ( unsigned int i=0; i<m_descr_vec.size(); i++ ) {
      const CaloDetDescriptor* reg = m_descr_vec[i];
      if ( reg ) {
        if ( reg->getSampling(0) == sample ) {
          if ( reg->eta_channel(eta) >=0 && reg->phi_channel(phi) >=0) result = true;
          double edgemin = std::abs( aeta - reg->reg_min() );
          double edgemax = std::abs( aeta - reg->reg_max() );
          if ( edgemin < etadist ) etadist = edgemin;
          if ( edgemax < etadist ) etadist = edgemax;
        }
      }
    }
  }

  if (result) etadist = etadist*(-1.);

  return result;
}

void CaloDetDescrManager_Base::build_sample (const CaloCell_ID::SUBCALO subCalo,
                                             const bool barrel,
                                             const int sampling_or_module,
                                             CaloCell_ID::CaloSample& sample) const
{
  if ( subCalo == CaloCell_ID::LAREM ) {
    if ( barrel ) {
      if (sampling_or_module == 0) sample = CaloCell_ID::PreSamplerB;
      if (sampling_or_module == 1) sample = CaloCell_ID::EMB1;
      if (sampling_or_module == 2) sample = CaloCell_ID::EMB2;
      if (sampling_or_module == 3) sample = CaloCell_ID::EMB3;
    } else {
      if (sampling_or_module == 0) sample = CaloCell_ID::PreSamplerE;
      if (sampling_or_module == 1) sample = CaloCell_ID::EME1;
      if (sampling_or_module == 2) sample = CaloCell_ID::EME2;
      if (sampling_or_module == 3) sample = CaloCell_ID::EME3;
    }
    return;
  }
  else if  ( subCalo == CaloCell_ID::LARHEC ) {
    if (sampling_or_module == 0) sample = CaloCell_ID::HEC0;
    if (sampling_or_module == 1) sample = CaloCell_ID::HEC1;
    if (sampling_or_module == 2) sample = CaloCell_ID::HEC2;
    if (sampling_or_module == 3) sample = CaloCell_ID::HEC3;
  }
  else if  ( subCalo == CaloCell_ID::LARFCAL ) {
    if (sampling_or_module == 0) sample = CaloCell_ID::FCAL0;
    if (sampling_or_module == 1) sample = CaloCell_ID::FCAL1;
    if (sampling_or_module == 2) sample = CaloCell_ID::FCAL2;
  }
  else if  ( subCalo == CaloCell_ID::TILE ) {
    // For Tiles sampling_or_module has a different meaning :
    // assume the client meant the entrance of the Tiles and ignore the gap
    if ( barrel )
      sample = CaloCell_ID::TileBar0;
    else
      sample = CaloCell_ID::TileExt0;
  }
  else
    sample = CaloCell_ID::Unknown;

}

void CaloDetDescrManager_Base::decode_sample (CaloCell_ID::SUBCALO& subCalo,
                                              bool& barrel,
                                              int& sampling_or_module,
                                              const CaloCell_ID::CaloSample sample) const
{
  if ( sample == CaloCell_ID::PreSamplerB) {
    subCalo = CaloCell_ID::LAREM;
    barrel = true;
    sampling_or_module = 0;
    return;
  }
  else if ( sample == CaloCell_ID::EMB1) {
    subCalo = CaloCell_ID::LAREM;
    barrel = true;
    sampling_or_module = 1;
    return;
  }
  else if ( sample == CaloCell_ID::EMB2) {
    subCalo = CaloCell_ID::LAREM;
    barrel = true;
    sampling_or_module = 2;
    return;
  }
  else if ( sample == CaloCell_ID::EMB3) {
    subCalo = CaloCell_ID::LAREM;
    barrel = true;
    sampling_or_module = 3;
    return;
  }
  else if ( sample == CaloCell_ID::PreSamplerE) {
    subCalo = CaloCell_ID::LAREM;
    barrel = false;
    sampling_or_module = 0;
    return;
  }
  else if ( sample == CaloCell_ID::EME1) {
    subCalo = CaloCell_ID::LAREM;
    barrel = false;
    sampling_or_module = 1;
    return;
  }
  else if ( sample == CaloCell_ID::EME2) {
    subCalo = CaloCell_ID::LAREM;
    barrel = false;
    sampling_or_module = 2;
    return;
  }
  else if ( sample == CaloCell_ID::EME3) {
    subCalo = CaloCell_ID::LAREM;
    barrel = false;
    sampling_or_module = 3;
    return;
  }
  else if ( sample == CaloCell_ID::HEC0) {
    subCalo = CaloCell_ID::LARHEC;
    barrel = false;
    sampling_or_module = 0;
    return;
  }
  else if ( sample == CaloCell_ID::HEC1) {
    subCalo = CaloCell_ID::LARHEC;
    barrel = false;
    sampling_or_module = 1;
    return;
  }
  else if ( sample == CaloCell_ID::HEC2) {
    subCalo = CaloCell_ID::LARHEC;
    barrel = false;
    sampling_or_module = 2;
    return;
  }
  else if ( sample == CaloCell_ID::HEC3) {
    subCalo = CaloCell_ID::LARHEC;
    barrel = false;
    sampling_or_module = 3;
    return;
  }
  else if ( sample == CaloCell_ID::FCAL0) {
    subCalo = CaloCell_ID::LARFCAL;
    barrel = false;
    sampling_or_module = 0;
    return;
  }
  else if ( sample == CaloCell_ID::FCAL1) {
    subCalo = CaloCell_ID::LARFCAL;
    barrel = false;
    sampling_or_module = 1;
    return;
  }
  else if ( sample == CaloCell_ID::FCAL2) {
    subCalo = CaloCell_ID::LARFCAL;
    barrel = false;
    sampling_or_module = 2;
    return;
  }
  else if ( sample == CaloCell_ID::TileBar0 ||
            sample == CaloCell_ID::TileBar1 ||
            sample == CaloCell_ID::TileBar2 ||
            sample == CaloCell_ID::TileGap1 ||
            sample == CaloCell_ID::TileGap2 ||
            sample == CaloCell_ID::TileGap3 ) {
    subCalo = CaloCell_ID::TILE;
    barrel = true;
    sampling_or_module = 0;
    return;
  }
  else if ( sample == CaloCell_ID::TileExt0 ||
            sample == CaloCell_ID::TileExt1 ||
            sample == CaloCell_ID::TileExt2 ) {
    subCalo = CaloCell_ID::TILE;
    barrel = false;
    sampling_or_module = 0;
    return;
  }
   else {
    subCalo = CaloCell_ID::NOT_VALID;
    barrel = true;
    sampling_or_module = 0;
  }

}


CaloDetDescrManager* CaloDetDescrManager::s_instance = 0;

const CaloDetDescrManager* CaloDetDescrManager::instance()
{
  if(s_instance==0)
  {
    // The following code can be removed once all usage of the singleton has 
    // been removed and replaced by accessing this object via the transient 
    // detector store. Use the instance that's in the transient detector 
    // store if it exists. Otherwise create a new instance, register it in 
    // the store and return that. First, locate the StoreGate instance that's 
    // managing the transient detector store.
    
    StoreGateSvc* detStore = 0;
    IMessageSvc*  msgSvc;
    const CaloDetDescrManager* theMgr;
 
    ISvcLocator* svcLoc = Gaudi::svcLocator();
    StatusCode status = svcLoc->service( "MessageSvc", msgSvc);
    
    if(status.isSuccess()) 
    {
      MsgStream log(msgSvc, "CaloDetDescrManager");
      status = svcLoc->service("DetectorStore",detStore);

      if(status.isSuccess()) 
      {
	// Test whether the instance already exists in the transient 
	// detector store
	if(detStore->contains<CaloDetDescrManager>("CaloMgr")) 
	{
	  // The instance already exists - retrieve it and save it locally.
	  status = detStore->retrieve(theMgr);
	  s_instance = const_cast<CaloDetDescrManager*>(theMgr);
	}
      } 
      else {
	log << MSG::ERROR << "Could not locate DetectorStore" << endmsg;
      }
    }
    else
    {
      std::cerr << "CaloDetDescrManager: Could not locate the MessageSvc!!!\n";
    }
  }
  return s_instance;
}


CaloSuperCellDetDescrManager* CaloSuperCellDetDescrManager::s_instance = 0;

const CaloSuperCellDetDescrManager* CaloSuperCellDetDescrManager::instance()
{
  if(s_instance==0)
  {
    // The following code can be removed once all usage of the singleton has 
    // been removed and replaced by accessing this object via the transient 
    // detector store. Use the instance that's in the transient detector 
    // store if it exists. Otherwise create a new instance, register it in 
    // the store and return that. First, locate the StoreGate instance that's 
    // managing the transient detector store.
    
    StoreGateSvc* detStore = 0;
    IMessageSvc*  msgSvc;
    const CaloSuperCellDetDescrManager* theMgr;
 
    ISvcLocator* svcLoc = Gaudi::svcLocator();
    StatusCode status = svcLoc->service( "MessageSvc", msgSvc);
    
    if(status.isSuccess()) 
    {
      MsgStream log(msgSvc, "CaloSuperCellDetDescrManager");
      status = svcLoc->service("DetectorStore",detStore);

      if(status.isSuccess()) 
      {
	// Test whether the instance already exists in the transient 
	// detector store
	if(detStore->contains<CaloSuperCellDetDescrManager>("CaloSuperCellMgr")) 
	{
	  // The instance already exists - retrieve it and save it locally.
	  status = detStore->retrieve(theMgr);
	  s_instance = const_cast<CaloSuperCellDetDescrManager*>(theMgr);
	}
      } 
      else {
	log << MSG::ERROR << "Could not locate DetectorStore" << endmsg;
      }
    }
    else
    {
      std::cerr << "CaloSuperCellDetDescrManager: Could not locate the MessageSvc!!!\n";
    }
  }
  return s_instance;
}


const CaloCell_ID* CaloDetDescrManager::getCaloCell_ID() const
{
  return static_cast<const CaloCell_ID*>
    (CaloDetDescrManager_Base::getCaloCell_ID());
}

void CaloDetDescrManager::set_helper(const CaloCell_ID*  idHelper)
{
  CaloDetDescrManager_Base::set_helper (idHelper);
}


void CaloDetDescrManager::set_helper(const CaloIdManager* idHelper)
{
  CaloDetDescrManager_Base::set_helper (idHelper);
}


const CaloCell_SuperCell_ID* CaloSuperCellDetDescrManager::getCaloCell_ID() const
{
  return static_cast<const CaloCell_SuperCell_ID*>
    (CaloDetDescrManager_Base::getCaloCell_ID());
}

void CaloSuperCellDetDescrManager::set_helper(const CaloCell_SuperCell_ID*  idHelper)
{
  CaloDetDescrManager_Base::set_helper (idHelper);
}


void CaloSuperCellDetDescrManager::set_helper(const CaloIdManager* idHelper)
{
  CaloDetDescrManager_Base::set_helper (idHelper);
}



CaloSuperCellDetDescrManager::~CaloSuperCellDetDescrManager()
{
  // For offline case, the manager does not own the tile elements.
  // But for the supercell case, we do.
  // So delete them here.

  for (const CaloDetDescriptor* d : tile_descriptors_range()) {
    delete d;
  }

  for (const CaloDetDescrElement* d : element_range (CaloCell_ID::TILE)) {
    delete d;
  }
}
