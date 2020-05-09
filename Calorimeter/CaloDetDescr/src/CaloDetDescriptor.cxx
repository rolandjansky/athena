/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "Identifier/Identifier.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include <cmath>
#include <iomanip>
#include <iostream>


CaloDetDescriptor::CaloDetDescriptor(const Identifier& id, 
				     const AtlasDetectorID* helper,
				     const CaloCell_Base_ID* calo_helper,
				     CaloCell_ID::CaloSample sample,
				     int layer)
  : m_calo_eta_min (0),
    m_calo_eta_max (0)
{
  // -- m_id, m_calo_helper
  m_id=id;  
  m_helper=helper;
  m_calo_helper=calo_helper;

  // --m_is_xxx
  m_is_lar_em = m_calo_helper->is_em(id); 
  m_is_lar_em_barrel = m_calo_helper->is_em_barrel(id); 
  m_is_lar_em_endcap = m_calo_helper->is_em_endcap(id); 
  m_is_lar_em_endcap_inner = m_calo_helper->is_em_endcap_inner(id); 
  m_is_lar_em_endcap_outer = m_calo_helper->is_em_endcap_outer(id); 
  m_is_lar_hec = m_calo_helper->is_hec(id);
  m_is_lar_fcal = m_calo_helper->is_fcal(id); 
  m_is_tile = m_calo_helper->is_tile(id); 

  // -- m_calo_sign
  if (m_is_tile)
    m_calo_sign = m_calo_helper->side(id);
  else {
    int posneg = m_calo_helper->pos_neg(id);
    if (posneg == 0) 
      m_calo_sign = 0;
    else
      m_calo_sign = (int)(posneg/std::abs(posneg));
  }

  // -- m_layer
  m_layer = (layer != CaloCell_ID::NOT_VALID) ? layer : m_calo_helper->sampling(id);

  // -- m_calo_sampl
  if(m_is_tile)
  {
    if(sample != CaloCell_ID::Unknown) 
    {
      m_calo_sampl = sample;
    } 
    else 
    {        
      const Tile_Base_ID* tileid = static_cast<const Tile_Base_ID*>(m_helper);
      if(tileid->is_tile_barrel(id))
	m_calo_sampl = CaloCell_ID::TileBar0;
      else if(tileid->is_tile_extbarrel(id))
	m_calo_sampl = CaloCell_ID::TileExt0;
      else if(tileid->is_tile_gap(id))
	m_calo_sampl = (CaloCell_ID::CaloSample) 
	  ((int)CaloCell_ID::TileGap1 - 1);
      else
	m_calo_sampl = CaloCell_ID::Unknown;
    }
  }
  else 
    m_calo_sampl = (CaloCell_ID::CaloSample) m_calo_helper->calo_sample(id);

  // -- m_calo_num
  m_calo_num = (CaloCell_ID::SUBCALO) m_calo_helper->sub_calo(id);

  // -- m_calocell_min
  IdentifierHash junk;
  m_calo_helper->calo_cell_hash_range(id,m_calocell_min,junk);

  // -- m_lar_deta, m_lar_n_eta (phi)
  m_lar_deta = (double) m_calo_helper->etaGranularity(id);
  m_lar_dphi = (double) m_calo_helper->phiGranularity(id);
  m_lar_n_eta = m_calo_helper->eta_max(id) - m_calo_helper->eta_min(id) + 1;
  m_lar_n_phi = m_calo_helper->phi_max(id) - m_calo_helper->phi_min(id) + 1;

  // To be initialized in the derived classes:
  m_calo_phi_min = 0.;
  m_calo_phi_max = 0.;
  m_calo_r_min = 0.;
  m_calo_r_max = 0.;
  m_calo_z_min = 0.;
  m_calo_z_max = 0.;

  m_lar_reg_min = 999999.;
  m_lar_reg_max = -999999.;
  m_lar_phi_min = 999999.;
  m_lar_eta_min = 0.;

  m_n_calo_depth = 0;
  m_calo_depth_in.resize(0);
  m_calo_depth_out.resize(0);

  // Eigen doesn't default-initialize things.
  m_transform.setIdentity();

  m_etachan_min = m_calo_helper->eta_min(m_id);
}

CaloDetDescriptor::~CaloDetDescriptor() 
{
}

void CaloDetDescriptor::print() const
{
  dump(std::cout);
}

void CaloDetDescriptor::dump(std::ostream& os) const
{
  os << " CaloDetDescriptor print: \n\n"; 

  os << m_calo_helper->print_to_string(m_id);    
  
  if(m_calo_helper->is_em_endcap(m_id))
    os << " LAr EM ENDCAP: " << (int) m_calo_sampl << " " <<  (int) m_calo_num << "\n";
  else if(m_calo_helper->is_em_barrel(m_id))
    os << " LAr EM BARREL: " << (int) m_calo_sampl << " " <<  (int) m_calo_num << "\n";
  else if(m_calo_helper->is_lar_hec(m_id))
    os << " LAr HEC: " << (int) m_calo_sampl << " " <<  (int) m_calo_num << "\n";
  else if(m_calo_helper->is_lar_fcal(m_id))
    os << " LAr Fcal:  "  << (int) m_calo_sampl << " " <<  (int) m_calo_num << "\n";
  else
  {
    if(!m_is_tile) os << " UNKNOW LAr decriptor !!!! \n";
  }

  os << "Ideal values before alignment :\n";
  os << "  eta_min   eta_max    deta    n_eta  phi_min   phi_max    dphi  n_phi    \n"; 
  os << std::setw(9) << std::setprecision(4) << calo_eta_min() << " " 
	    << std::setw(9) << std::setw(9) << std::setprecision(4) << calo_eta_max() << " "
            << std::setw(9) << std::setprecision(4) << deta()    << " " 
	    << std::setw(9) << std::setprecision(4) << n_eta()    << " " 
	    << std::setw(9) << std::setprecision(4) << calo_phi_min() << " " 
	    << std::setw(9) << std::setprecision(4) << calo_phi_max() << " " 
	    << std::setw(9) << std::setprecision(4) << dphi()    << " "
	    << std::setw(9) << std::setprecision(4) << n_phi() << "\n";
 
  os << " r_min  z_min   r_max  z_max  sign\n";  

  os << std::setprecision(4) << calo_r_min() << " " 
	    << std::setprecision(4) << calo_r_max() << " " 
	    << std::setprecision(4) << calo_z_min() << " " 
	    << std::setprecision(4) << calo_z_max() << " " 
	    << std::setprecision(1) << calo_sign() << "\n";

  Amg::RotationMatrix3D rot= m_transform.rotation ();

  //calculate euler angles
  double theta1, psi1, phi1;
  if (fabs(fabs(rot(2,1))-1) > 0.00001)
  {
      theta1 = -asin(rot(2,0));
      psi1 = atan2(rot(2,1)/cos(theta1), rot(2,2)/cos(theta1));
      phi1 = atan2(rot(1,0)/cos(theta1), rot(0,0)/cos(theta1));
  } else {
      phi1 = 0;
      if(rot(2,0) == -1){
          theta1 = M_PI*0.5;
          psi1 = atan2(rot(0,1),rot(0,2));
      } else {
          theta1 = -(M_PI * 0.5);
          psi1 = atan2(-rot(0,1), -rot(0,2));
      }
  }

  os << "\nEigenTransform phi,theta,psi,x,y,z: "
	    << phi1 << " "  << theta1 << " "
	    << psi1  << " "  << m_transform(0,3) << " "
	    << m_transform(1,3) << " "  << m_transform(2,3) << "\n\n";

    os << std::setprecision(4) << "in real world (can be <0)" 
	      << m_lar_reg_min << " " << m_lar_reg_max << " " 
	      << m_lar_phi_min << " " << m_lar_eta_min << "\n\n"; 
}

CaloCell_ID::CaloSample
CaloDetDescriptor::getSampling(IdentifierHash onecalo_hash) const
{
  if(m_layer==CaloCell_ID::NOT_VALID) 
  {
    const Tile_Base_ID* tileid = reinterpret_cast<const Tile_Base_ID*> (m_helper);
    Identifier id = tileid->cell_id (onecalo_hash);
    return (CaloCell_ID::CaloSample) (m_calo_sampl + tileid->sample(id));
  } 
  else 
  {
    return m_calo_sampl;
  }
}

bool CaloDetDescriptor::get_cylinder_surface (Amg::Transform3D* htrans,
                                              double& radius,
                                              double& hphi, 
                                              double& hlength, 
                                              double& depth) const
{
  // For cylinders : htrans is the displacement compared to a cylinder centered along the
  //                 z axis

  *htrans = m_transform;
  radius = m_calo_r_min;
  hlength = m_calo_z_max;
  hphi = 0.5*m_lar_dphi*m_lar_n_phi;
  depth = m_calo_r_max;

  if (m_is_tile) 
  { 
    // need to shift tile cylinders (half-barrels, ext.barrels and ITC)
    hlength = m_calo_z_max-m_calo_z_min;
    double z = m_calo_sign*(m_calo_z_min + m_calo_z_max)/2.;
    Amg::Translation3D vec(Amg::Vector3D(0.,0.,z));
    *htrans = (*htrans) * vec;
  }

  return true;
}

bool CaloDetDescriptor::get_disk_surface (Amg::Transform3D* htrans,
                                          double& z, 
                                          double& rmin, 
                                          double& rmax, 
                                          double& hphisec, 
                                          double& depth) const 
{
  // For disks : htrans is the position of the entrance surface, i.e. the alignment
  //             heptransform + a z shift to put the entrance at the right z. 

  *htrans = m_transform;
  z = m_calo_z_min*m_calo_sign;
  depth = m_calo_z_max - m_calo_z_min;

  Amg::Translation3D vec(0.,0.,z);
  *htrans = (*htrans) * vec;

  rmin = m_calo_r_min;
  rmax = m_calo_r_max;
  hphisec = 0.5*m_lar_dphi*m_lar_n_phi;
 
  return true;
}

bool CaloDetDescriptor::is_in(double eta_min, 
                              double eta_max,
                              double /*phi_min*/, 
                              double /*phi_max*/) const
{
  bool result = (eta_min  + 0.00001 <= m_lar_reg_max &&
		 eta_max >= m_lar_reg_min + 0.00001);
  return result;
}

int CaloDetDescriptor::eta_channel(double eta) const
{
  int result = -1; 
  //  if(m_calo_sign*eta<0) return result;
  if(eta < m_lar_reg_min || eta > m_lar_reg_max) return result;


  if(std::abs(eta) + 0.00001 >= m_lar_eta_min)
    result = (int)((std::abs(eta) - m_lar_eta_min)/m_lar_deta) + m_etachan_min;

  if(result>=(m_lar_n_eta+m_etachan_min)) result = m_lar_n_eta + m_etachan_min -1;

  return result;
}

int CaloDetDescriptor::phi_channel(double phi) const
{
  double phi_fixed = phi ;

  // -M_PI,M_PI convention :
  if (phi <= m_lar_phi_min) 
    phi_fixed = phi_fixed + 4*asin(1.);

  int chan = (int)((phi_fixed - m_lar_phi_min)/m_lar_dphi) + m_calo_helper->phi_min(m_id);

  if(chan >= m_lar_n_phi + m_calo_helper->phi_min(m_id)) chan = -1;
  
  return chan;
}

int CaloDetDescriptor::eta_channel_raw(double eta) const
{
  int result = -1;
  eta = eta*m_calo_sign;
  if(eta < m_calo_eta_min || eta > m_calo_eta_max) return result;


  if(std::abs(eta) + 0.00001 >= m_calo_eta_min)
    result = (int)((std::abs(eta) - m_calo_eta_min)/m_lar_deta);

  if(result>=(m_lar_n_eta+m_etachan_min)) result = m_lar_n_eta -1;

  return result;
}

int CaloDetDescriptor::phi_channel_raw(double phi) const
{
  double phi_fixed = phi ;
  
  // -M_PI,M_PI convention :
  if (phi <= m_calo_phi_min)
    phi_fixed = phi_fixed + 4*asin(1.);
    
  int chan = (int)((phi_fixed - m_calo_phi_min)/m_lar_dphi) + m_calo_helper->phi_min(m_id);
  
  if(chan >= m_lar_n_phi + m_calo_helper->phi_min(m_id)) chan = -1;

  return chan;
} 



int CaloDetDescriptor::n_calo_depth() const
{ return m_n_calo_depth; }

void CaloDetDescriptor::get_depth_in(std::vector<double>& calo_depth) const
{
  calo_depth.resize(m_n_calo_depth);
  for (int i=0; i<m_n_calo_depth; i++)
    calo_depth [i] = m_calo_depth_in[i];
}

void CaloDetDescriptor::get_depth_out(std::vector<double>& calo_depth) const
{
  calo_depth.resize(m_n_calo_depth);
  for (int i=0; i<m_n_calo_depth; i++)
    calo_depth [i] = m_calo_depth_out[i];
}


void CaloDetDescriptor::set_n_calo_depth(int n_calo_depth)
{
  m_n_calo_depth= n_calo_depth;
  m_calo_depth_in.resize(m_n_calo_depth);
  m_calo_depth_out.resize(m_n_calo_depth);
}

void CaloDetDescriptor::set_depth_in(std::vector<double>& calo_depth)
{
  for (int i=0; i<m_n_calo_depth; i++)
    m_calo_depth_in[i] = calo_depth[i];
}

void CaloDetDescriptor::set_depth_out(std::vector<double>& calo_depth)
{
  for (int i=0; i<m_n_calo_depth; i++)
    m_calo_depth_out[i] = calo_depth[i];
}

void CaloDetDescriptor::set_eta_phi_granularity(int neta, 
						double deta,
						int nphi, 
						double dphi)
{
  m_lar_deta = deta;
  m_lar_dphi = dphi;
  m_lar_n_eta = neta;
  m_lar_n_phi = nphi;
}
 
