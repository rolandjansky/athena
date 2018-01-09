/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloDetDescriptor.h
 *
 * @brief Definition of CaloDetDescriptor
 *
 * $Id: CaloDetDescriptor.h,v 1.37 2009-04-22 19:54:16 ssnyder Exp $
 */

#ifndef CALODETDESCR_CALODETDESCRIPTOR_H
#define CALODETDESCR_CALODETDESCRIPTOR_H

#include "CaloIdentifier/CaloCell_ID.h"
#include "Identifier/Identifiable.h"
#include "GeoPrimitives/GeoPrimitives.h"

class Identifier;
class AtlasDetectorID;

/**  @class CaloDetDescriptor

     @brief This is a base class for LAr and Tile Descriptors
     The primary goal is to speed up loops  over all the calo cells and reduce the
     size of the CaloDetDescrElements by caching information they have in common.

     There is 1 Descriptor per region of homogeneous grannularity.

  1) Phi convention :

     The calorimeter convention is to start cell counting at
     phi=0 and increment up to 2pi
     ==> the old 0 to 2pi convention is kept internally to Descriptors,
     and official Atlas one (pi,pi) applied to the CaloDetDescrElements and
     all methods concerning channels.

  2) Data members :

     All the m_calo_xxx numbers are "ideal", i.e. correspond to a
     HepGeom::Transform3D(0.,0.,0.,0.,0.,0.) and are NOT recalculated
     when a different one is set.

     WARNING : eta is always>0 and one has to multiply it by calo_sign()

  3) Coordinate systems : we (unfortunately) need 3 on them !!!

        cylindric for EMB:   eta,phi,cylinder-radius
	ec_cylindric for EMEC and HEC:   eta, phi, z
	cartesian for FCAL:  x,y,z

     Coordinates are kept coherent:
     set_cylindric re-computes the cartesian coordinates and vice-versa.

*/
class CaloDetDescriptor : public Identifiable
{
 public:
  /** @brief Constructor
      @param id {IN] region identifier
      @param helper [IN] Atlas Detector ID helper
      @param calo_helper [IN] Calo Cell ID helper
      @param sample [IN] sample
      @param layer [IN] layer
   */
  CaloDetDescriptor(const Identifier& id, 
		    const AtlasDetectorID* helper,
		    const CaloCell_Base_ID* calo_helper,
		    CaloCell_ID::CaloSample sample=CaloCell_ID::Unknown, 
		    int layer=CaloCell_ID::NOT_VALID);

  /** @brief virtual detructor
   */
  virtual ~CaloDetDescriptor();

  /** @brief print the contents
   */
  virtual void print() const;

  /** @brief set number of calo depths
      @param n_calo_depth [IN} number of depths
   */
  void set_n_calo_depth(int n_calo_depth);

  /** @brief set vector of in depths
      @param calo_depth [IN] vector of in depths
   */
  void set_depth_in(std::vector<double>& calo_depth);

  /** @brief set vector of out depths
      @param calo_depth [IN] vector of out depths
   */
  void set_depth_out(std::vector<double>& calo_depth);

  /** @brief descriptor belongs to EM calorimeter
   */
  bool is_lar_em() const;                    
  /** @brief descriptor belongs to EM barrel
   */
  bool is_lar_em_barrel() const;             
  /** @brief descriptor belongs to EM end cap
   */
  bool is_lar_em_endcap() const;             
  /** @brief descriptor belongs to the inner wheel of EM end cap
   */
  bool is_lar_em_endcap_inner() const;       
  /** @brief descriptor belongs to the outer wheel of EM end cap
   */
  bool is_lar_em_endcap_outer() const;       
  /** @brief descriptor belongs to HEC
   */
  bool is_lar_hec() const;                   
  /** @brief descriptor belongs to FCAL
   */
  bool is_lar_fcal() const;                  
  /** @brief descriptor belongs to Tile
   */
  bool is_tile() const;                      
  
  /** @brief get sampling
      @param onecalo_hash [IN] hash identifier
      if the descriptor contains more than one sampling, add offset calculated from hash id.
   */
  CaloCell_ID::CaloSample getSampling(IdentifierHash onecalo_hash) const;

  /** @brief get sampling
      returns the cached CaloSample, if the descriptor contains more than one sampling, it only returns the first sampling. 
   */
  CaloCell_ID::CaloSample getSampling() const
    { return m_calo_sampl ; 
    }
  /** @brief get subcalo
   */
  CaloCell_ID::SUBCALO getSubCalo() const;

  /** @brief get region identifier
   */
  Identifier identify() const override final;
  /** @brief get hash identifier for the region
   */
  IdentifierHash identifyHash() const  override final;
  /** @brief get calo hash
   */
  IdentifierHash calo_hash() const;
  /** @brief get subcalo hash
   */
  IdentifierHash subcalo_hash() const;       

  /** @brief get Calo Cell ID helper
   */
  const CaloCell_Base_ID* get_calo_helper() const;   
  
  /** @brief get layer
   */
  int layer() const;                         
  
  /** @brief minimal hash identifier for calo cells in the region
   */
  IdentifierHash caloCellMin() const;


  /** @brief 'ideal' geometry: calo sign
   */
  int    calo_sign() const;                  

  /** @brief 'ideal' geometry: z minimal
   */
  double calo_z_min() const;                 
  /** @brief 'ideal' geometry: z maximal
   */
  double calo_z_max() const;
  /** @brief 'ideal' geometry: eta minimal
   */
  double calo_eta_min() const;               
  /** @brief 'ideal' geometry: eta maximal
   */
  double calo_eta_max() const;
  /** @brief 'ideal' geometry: phi minimal
   */
  double calo_phi_min() const;               
  /** @brief 'ideal' geometry: phi maximal
   */
  double calo_phi_max() const;               
  /** @brief 'ideal' geometry: r minimal
   */
  double calo_r_min() const;                 
  /** @brief 'ideal' geometry: r maximal
   */
  double calo_r_max() const;                 

  /** @brief number of calo depths
   */
  int  n_calo_depth() const;
  /** @brief get vector of in depths
      @param calo_depth [OUT] vector of in depths
   */
  void get_depth_in(std::vector<double>& calo_depth) const;
  /** @brief get vector of out depths
      @param calo_depth [OUT] vector of out depths
   */
  void get_depth_out(std::vector<double>& calo_depth) const;

  /** @brief delta eta
   */
  double  deta() const;                   
  /** @brief delta phi
   */
  double  dphi() const;                   
   /** @brief eta granularity
   */
  int     n_eta() const;                   
  /** @brief phi granularity
   */
  int     n_phi() const;                  

  // ------- Return parameters needed for Trk::Surface --------

  /** this will be the closest radius of this region */
  /** @brief this will be the closest radius of this region (for Trk::Surface)
   */
  bool get_cylinder_surface (Amg::Transform3D* htrans,
			     double& radius, 
			     double& hphi, 
			     double& hlength, 
			     double& depth) const;

  /** @brief (for Trk::Surface)
   */
  bool get_disk_surface (Amg::Transform3D* htrans,
			 double& z,
			 double& rmin, 
			 double& rmax, 
			 double& hphisec, 
			 double& depth) const; 

  /** @brief check overlap with a given zone in eta and phi (for LAr only)
      @param eta_min [IN] lower eta boundary of the zone
      @param eta_max [IN] higher eta boundary of the zone
      @param phi_min [IN] lower phi boundary of the zone
      @param phi_max [IN] higher phi boundary of the zone
   */
  bool is_in (double eta_min,                        
	      double eta_max,
	      double phi_min, 
	      double phi_max) const;

  /** @brief channel number for a given eta/phi (if outside returns -1). Should not be used for FCAL
   */
  int eta_channel(double eta) const;
  /** @brief channel number for a given eta/phi (if outside returns -1)  Should not be used for FCAL
   */
  int phi_channel(double phi) const;

  /** @brief channel number for a given eta/phi raw (if outside returns -1). Should not be used for FCAL
   */
  int eta_channel_raw(double eta) const;
  /** @brief channel number for a given eta/phi raw (if outside returns -1)  Should not be used for FCAL
   */
  int phi_channel_raw(double phi) const;


  /** @brief eta min and max once misaligned
   */
  double reg_min() const;
  /** @brief eta min and max once misaligned
   */
  double reg_max() const;
  /** @brief minimal Eta for LAr region
   */
  double lar_eta_min() const;
  /** @brief minimal Phi for LAr region
   */
  double lar_phi_min() const;

  /** @brief Set Eta Min
   */
  void setCaloEtaMin(double eta_min);
  /** @brief Set Eta Max
   */
  void setCaloEtaMax(double eta_max);
  /** @brief Set Phi Min
   */
  void setCaloPhiMin(double phi_min);
  /** @brief Set Phi Max
   */
  void setCaloPhiMax(double phi_max);
  /** @brief Set Z Min
   */
  void setCaloZMin(double z_min);
  /** @brief Set Z Max
   */
  void setCaloZMax(double z_max);
  /** @brief Set R Min
   */
  void setCaloRMin(double r_min);
  /** @brief Set R Max
   */
  void setCaloRMax(double r_max);

  /** @brief Set LAr Region Eta Min
   */
  void setLArRegMin(double reg_min);
  /** @brief Set LAr Region Eta Max
   */
  void setLArRegMax(double reg_max);
  /** @brief Set LAr Region Phi Min
   */
  void setLArPhiMin(double phi_min);
  /** @brief Set LAr Region Phi Max
   */
  void setLArEtaMin(double eta_min);

  /** @brief Set Phi granularity
   */
  void set_eta_phi_granularity(int neta, 
			       double deta,
			       int nphi, 
			       double dphi);

  /** @brief Get the current transformation matrix.
   */
  const Amg::Transform3D& transform() const;

  /** @brief Set the transformation matrix.
   */
  void set_transform (const Amg::Transform3D& transform);

 private:
  /** @brief Atlas Detector ID helper
   */
  const AtlasDetectorID* m_helper;
  /** @brief Calo Cell ID helper
   */
  const CaloCell_Base_ID* m_calo_helper;
  /** @brief Region identifier
   */
  Identifier m_id;

  /** @brief descriptor belongs to EM calorimeter
   */
  bool m_is_lar_em; 
  /** @brief descriptor belongs to EM barrel
   */
  bool m_is_lar_em_barrel; 
  /** @brief descriptor belongs to EM end cap
   */
  bool m_is_lar_em_endcap; 
   /** @brief descriptor belongs to the inner wheel of EM end cap
   */
  bool m_is_lar_em_endcap_inner; 
  /** @brief descriptor belongs to the outer wheel of EM end cap
   */
  bool m_is_lar_em_endcap_outer; 
  /** @brief descriptor belongs to HEC
   */
  bool m_is_lar_hec;
  /** @brief descriptor belongs to FCAL
   */
  bool m_is_lar_fcal; 
   /** @brief descriptor belongs to Tile
   */
 bool m_is_tile; 

  /** @brief calo sign
   */
  int m_calo_sign;
  /** @brief layer
   */
  int m_layer;
  
  /** @brief Calo Sample
   */
  CaloCell_ID::CaloSample m_calo_sampl;

  /** @brief Subcalo
   */
  CaloCell_ID::SUBCALO m_calo_num;

  /** @brief minimal hash id of region cells
   */
  IdentifierHash m_calocell_min;

  /** @brief delta eta (LAr)
   */
  double m_lar_deta;
  /** @brief delta phi (LAr)
   */
  double m_lar_dphi;
  /** @brief eta granularity (LAr)
   */
  int    m_lar_n_eta;
  /** @brief phi granularity (LAr)
   */
  int    m_lar_n_phi;

 protected: 
  // values to be defined in the derived classes

  // 'ideal' geometry
  /** @brief minimal Eta (to be defined in the derived classes)
   */
  double m_calo_eta_min;
  /** @brief maximal Eta (to be defined in the derived classes)
   */
  double m_calo_eta_max;
  /** @brief minimal Phi (to be defined in the derived classes)
   */
  double m_calo_phi_min;
  /** @brief maximal Phi (to be defined in the derived classes)
   */
  double m_calo_phi_max;
  /** @brief minimal R (to be defined in the derived classes)
   */
  double m_calo_r_min;
  /** @brief maximal R (to be defined in the derived classes)
   */
  double m_calo_r_max;
  /** @brief minimal Z (to be defined in the derived classes)
   */
  double m_calo_z_min;
  /** @brief maximal Z (to be defined in the derived classes)
   */
  double m_calo_z_max;

  /** @brief number of depths
   */
  int m_n_calo_depth;
  /** @brief in depths
   */
  std::vector <double> m_calo_depth_in;
  /** @brief out depths
   */
  std::vector <double> m_calo_depth_out;

  /** @brief misalignment transformation
   */
  Amg::Transform3D m_transform;

  /** @brief minimal Abs Eta for LAr region
   */
  double m_lar_reg_min;
  /** @brief maximal Abs Eta for LAr region
   */
  double m_lar_reg_max;
  /** @brief minimal Phi for LAr region
   */
  double m_lar_phi_min;
  /** @brief minimal Eta for LAr region
   */
  double m_lar_eta_min;
  /** @brief minimal Eta channel number
   */
  int m_etachan_min;
};

inline bool CaloDetDescriptor::is_lar_em() const
{   
  return m_is_lar_em; 
}

inline bool CaloDetDescriptor::is_lar_em_barrel() const
{   
  return m_is_lar_em_barrel; 
}

inline bool CaloDetDescriptor::is_lar_em_endcap() const
{   
  return m_is_lar_em_endcap; 
}

inline bool CaloDetDescriptor::is_lar_em_endcap_inner() const
{   
  return m_is_lar_em_endcap_inner; 
}

inline bool CaloDetDescriptor::is_lar_em_endcap_outer() const
{   
  return m_is_lar_em_endcap_outer; 
}

inline bool CaloDetDescriptor::is_lar_hec() const
{   
  return m_is_lar_hec; 
}

inline bool CaloDetDescriptor::is_lar_fcal() const
{   
  return m_is_lar_fcal; 
}

inline bool CaloDetDescriptor::is_tile() const
{   
  return m_is_tile; 
}

inline Identifier CaloDetDescriptor::identify() const 
{ 
  return m_id; 
}

inline IdentifierHash CaloDetDescriptor::identifyHash() const
{ 
  int num;
  return m_calo_helper->subcalo_region_hash(m_id, num);
}

inline IdentifierHash CaloDetDescriptor::calo_hash() const
{ 
  return m_calo_helper->calo_region_hash(m_id); 
}

inline IdentifierHash CaloDetDescriptor::subcalo_hash() const
{ 
  int num;
  return m_calo_helper->subcalo_region_hash(m_id, num);
}

inline const CaloCell_Base_ID* CaloDetDescriptor::get_calo_helper() const  
{ 
  return m_calo_helper;
}

inline int CaloDetDescriptor::layer() const
{ 
  return m_layer; 
}

inline CaloCell_Base_ID::SUBCALO CaloDetDescriptor::getSubCalo()  const
{
  return m_calo_num;
}

inline IdentifierHash CaloDetDescriptor::caloCellMin() const
{
  return m_calocell_min;
}

inline int CaloDetDescriptor::calo_sign() const
{ 
  return m_calo_sign; 
}

inline double CaloDetDescriptor::calo_z_min() const
{ 
  return m_calo_z_min; 
}

inline double CaloDetDescriptor::calo_z_max() const
{ 
  return m_calo_z_max; 
}

inline double CaloDetDescriptor::calo_eta_min() const
{ 
  return m_calo_eta_min; 
}

inline double CaloDetDescriptor::calo_eta_max() const
{ 
  return m_calo_eta_max; 
}

inline double CaloDetDescriptor::calo_phi_min() const
{ 
  return m_calo_phi_min; 
}

inline double CaloDetDescriptor::calo_phi_max() const
{ 
  return m_calo_phi_max; 
}

inline double CaloDetDescriptor::calo_r_min() const
{ 
  return m_calo_r_min; 
}

inline double CaloDetDescriptor::calo_r_max() const
{ 
  return m_calo_r_max; 
}

inline double CaloDetDescriptor::deta() const
{ 
  return m_lar_deta;
}

inline double CaloDetDescriptor::dphi() const
{ 
  return m_lar_dphi;
}

inline int CaloDetDescriptor::n_eta() const
{ 
  return m_lar_n_eta;
}

inline int CaloDetDescriptor::n_phi() const
{ 
  return m_lar_n_phi;
}

inline double CaloDetDescriptor::reg_min() const
{  
  return m_lar_reg_min; 
}

inline double CaloDetDescriptor::reg_max() const
{  
  return m_lar_reg_max; 
}

inline double CaloDetDescriptor::lar_eta_min() const
{  
  return m_lar_eta_min; 
}

inline double CaloDetDescriptor::lar_phi_min() const
{  
  return m_lar_phi_min; 
}

inline void CaloDetDescriptor::setCaloEtaMin(double eta_min)
{
  m_calo_eta_min = eta_min;
}

inline void CaloDetDescriptor::setCaloEtaMax(double eta_max)
{
  m_calo_eta_max = eta_max;
}

inline void CaloDetDescriptor::setCaloPhiMin(double phi_min)
{
  m_calo_phi_min = phi_min;
}

inline void CaloDetDescriptor::setCaloPhiMax(double phi_max)
{
  m_calo_phi_max = phi_max;
}

inline void CaloDetDescriptor::setCaloZMin(double z_min)
{
  m_calo_z_min = z_min;
}

inline void CaloDetDescriptor::setCaloZMax(double z_max)
{
  m_calo_z_max = z_max;
}

inline void CaloDetDescriptor::setCaloRMin(double r_min)
{
  m_calo_r_min = r_min;
}

inline void CaloDetDescriptor::setCaloRMax(double r_max)
{
  m_calo_r_max = r_max;
}

inline void CaloDetDescriptor::setLArRegMin(double reg_min)
{
  m_lar_reg_min = reg_min;
}

inline void CaloDetDescriptor::setLArRegMax(double reg_max)
{
  m_lar_reg_max = reg_max;
}

inline void CaloDetDescriptor::setLArPhiMin(double phi_min)
{
  m_lar_phi_min = phi_min;
}

inline void CaloDetDescriptor::setLArEtaMin(double eta_min)
{
  m_lar_eta_min = eta_min;
}

inline
const Amg::Transform3D& CaloDetDescriptor::transform() const
{
  return m_transform;
}


inline
void CaloDetDescriptor::set_transform (const Amg::Transform3D& transform)
{
  m_transform = transform;
}


#endif
