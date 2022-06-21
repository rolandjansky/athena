/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTCALOSIM_CELLINFOCONTAINER_H
#define FASTCALOSIM_CELLINFOCONTAINER_H

#include "FastCaloSim/FSmap.h"
#include "FastCaloSim/FastCaloSim_CaloCell_ID.h"

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

#include <vector>
#include <map>

class CaloDetDescrElement;

class cellinfo_map
{
 public:
  typedef std::pair<const CaloDetDescrElement*,float> cellinfo;
  
  class cellinfo_vec 
  {
  public:
    typedef std::vector<cellinfo> vec;
    
    cellinfo_vec() = default;

    cellinfo& operator[](unsigned int i) {return m_cellinfo_vec[i];}
    const cellinfo& operator[](unsigned int i) const {return m_cellinfo_vec[i];}
    void resize(unsigned int n) {m_cellinfo_vec.resize(n);}
    unsigned int size() const {return m_cellinfo_vec.size();}
    void push_back(const cellinfo & ele,int layernr) {m_cellinfo_vec.push_back(ele);m_vol+=ele.second;m_subvol[layernr]+=ele.second;};
    double vol() const {return m_vol;};
    double vol(int i) const {return m_subvol[i];};

    vec m_cellinfo_vec;
    double m_vol{0};
    double m_subvol[4] = {0};
  };

  typedef std::vector< std::vector<cellinfo_vec> > map_type;
  
  cellinfo_map(int p_neta=0, int p_nphi=0)
    : m_eta_start(-5)
    , m_eta_end(5)
    , m_phi_start(-M_PI)
    , m_phi_end(M_PI)
    , m_neta(p_neta)
    , m_nphi(p_nphi)
    , m_leta(1)
    , m_lphi(1) 
  {
    m_deta = m_neta>0 ? (m_eta_end-m_eta_start)/m_neta : 0;
    m_dphi = m_nphi>0 ? (m_phi_end-m_phi_start)/m_nphi : 0;

    m_map.resize(m_neta);
    for(auto& vect : m_map) vect.resize(m_nphi);
  }

  cellinfo_map(double p_eta_start
	       , double p_eta_end
	       , double p_phi_start
	       , double p_phi_end
	       , int p_neta=100
	       , int p_nphi=64
	       , int p_leta=1
	       , int p_lphi=1)
    : m_eta_start(p_eta_start)
    , m_eta_end(p_eta_end)
    , m_phi_start(p_phi_start)
    , m_phi_end(p_phi_end)
    , m_neta(p_neta)
    , m_nphi(p_nphi)
    , m_leta(p_leta)
    , m_lphi(p_lphi) 
  {
    m_deta = m_neta>0 ? (m_eta_end-m_eta_start)/m_neta : 0;
    m_dphi = m_nphi>0 ? (m_phi_end-m_phi_start)/m_nphi : 0;

    m_map.resize(m_neta);
    for(auto& vect : m_map) vect.resize(m_nphi);
  }

  void init(double p_eta_start=-5.0
	    , double p_eta_end=+5.0
	    , double p_phi_start=-M_PI
	    , double p_phi_end=-M_PI
	    , int p_neta=100
	    , int p_nphi=64
	    , int p_leta=1
	    , int p_lphi=1) 
  {
    m_eta_start=p_eta_start;
    m_eta_end=p_eta_end;
    m_phi_start=p_phi_start;
    m_phi_end=p_phi_end;
    m_neta=p_neta;
    m_nphi=p_nphi;
    m_leta=p_leta;
    m_lphi=p_lphi;
    if(m_neta>0) m_deta=(m_eta_end-m_eta_start)/m_neta;
    if(m_nphi>0) m_dphi=(m_phi_end-m_phi_start)/m_nphi;
    
    m_map.resize(m_neta);
    for(auto& vect : m_map) vect.resize(m_nphi);
  }

  double eta_start() const {return m_eta_start;}
  double eta_end() const {return m_eta_end;}
  double phi_start() const {return m_eta_start;}
  double phi_end() const {return m_eta_end;}
  double deta() const {return m_deta;}
  double dphi() const {return m_dphi;}
  unsigned int neta() const {return m_neta;}
  unsigned int nphi() const {return m_nphi;}
  unsigned int leta() const {return m_leta;}
  unsigned int lphi() const {return m_lphi;}

  double index_to_eta_center(const int i) const {return m_eta_start+(i+0.5)*m_deta;}
  double index_to_phi_center(const int i) const {return m_phi_start+(i+0.5)*m_dphi;}

  int    eta_to_index(const double eta) const {return (int)floor( (eta-m_eta_start)/m_deta );}
  int    phi_to_index_cont(const double phi) const {
    return (int)floor( (phi-m_phi_start)/m_dphi );
  }
  int    phi_to_index(const double phi) const {
    int iphi=phi_to_index_cont(phi);
    if(iphi<0) iphi+=m_nphi;
    if(iphi>=(int)m_nphi) iphi-=m_nphi;
    return iphi;
  }

  void eta_range(const int ieta,double& eta1,double& eta2) { eta1=m_eta_start+ieta*m_deta; eta2=eta1+m_leta*m_deta; }
  void phi_range(const int iphi,double& phi1,double& phi2) { phi1=m_phi_start+iphi*m_dphi; phi2=phi1+m_lphi*m_dphi; }

  cellinfo_vec& vec(int ieta,int iphi) {return m_map[ieta][iphi];}
  const cellinfo_vec& vec(int ieta,int iphi) const {return m_map[ieta][iphi];}

  const std::string& name() const {return m_name;}
  void setname(const std::string& name) {m_name=name;}

 private:
  double m_eta_start;
  double m_eta_end;
  double m_phi_start;
  double m_phi_end;
  unsigned int m_neta;
  unsigned int m_nphi;
  unsigned int m_leta;
  unsigned int m_lphi;
  double m_deta;
  double m_dphi;

  std::string m_name;

  map_type m_map;
};

class CellInfoContainer
{
 public:
  CellInfoContainer() = default;
  ~CellInfoContainer() = default;

  friend class CellInfoContainerCondAlg;

  inline const cellinfo_map& getCellistMap(int sample) const {return m_celllist_maps[sample];}
  inline const cellinfo_map& getEmCellistMap() const {return m_em_celllist_map;}
  inline const cellinfo_map& getEmMap() const {return m_em_map;}
  inline const cellinfo_map& getEmFineMap() const {return m_em_fine_map;}
  inline const cellinfo_map& getHadMap() const {return m_had_map;}

  inline double getPhi0Em()  const {return m_phi0_em;}
  inline double getPhi0Had() const {return m_phi0_had;}

  inline double getMinEtaSample(unsigned side, unsigned calosample) const {return m_min_eta_sample[side][calosample];}
  inline double getMaxEtaSample(unsigned side, unsigned calosample) const {return m_max_eta_sample[side][calosample];}

  inline const FSmap< double , double >& getRmidMap(unsigned side, unsigned calosample) const {return m_rmid_map[side][calosample];}
  inline const FSmap< double , double >& getZmidMap(unsigned side, unsigned calosample) const {return m_zmid_map[side][calosample];}
  inline const FSmap< double , double >& getRentMap(unsigned side, unsigned calosample) const {return m_rent_map[side][calosample];}
  inline const FSmap< double , double >& getZentMap(unsigned side, unsigned calosample) const {return m_zent_map[side][calosample];}

 protected:
  inline cellinfo_map& getCellistMap(int sample) {return m_celllist_maps[sample];}
  inline cellinfo_map& getEmCellistMap() {return m_em_celllist_map;}
  inline cellinfo_map& getEmMap() {return m_em_map;}
  inline cellinfo_map& getEmFineMap() {return m_em_fine_map;}
  inline cellinfo_map& getHadMap() {return m_had_map;}

  inline double& getPhi0Em()  {return m_phi0_em;}
  inline double& getPhi0Had() {return m_phi0_had;}

  inline double& getMinEtaSample(unsigned side, unsigned calosample) {return m_min_eta_sample[side][calosample];}
  inline double& getMaxEtaSample(unsigned side, unsigned calosample) {return m_max_eta_sample[side][calosample];}

  inline FSmap< double , double >& getRmidMap(unsigned side, unsigned calosample) {return m_rmid_map[side][calosample];}
  inline FSmap< double , double >& getZmidMap(unsigned side, unsigned calosample) {return m_zmid_map[side][calosample];}
  inline FSmap< double , double >& getRentMap(unsigned side, unsigned calosample) {return m_rent_map[side][calosample];}
  inline FSmap< double , double >& getZentMap(unsigned side, unsigned calosample) {return m_zent_map[side][calosample];}

 private:
  cellinfo_map m_celllist_maps[CaloCell_ID_FCS::MaxSample];
  cellinfo_map m_em_celllist_map;
  cellinfo_map m_em_map;
  cellinfo_map m_em_fine_map;
  cellinfo_map m_had_map;

  double m_phi0_em{-1000.};
  double m_phi0_had{-1000.};

  double m_min_eta_sample[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  double m_max_eta_sample[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]

  FSmap< double , double > m_rmid_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_zmid_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_rent_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_zent_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]

};

CLASS_DEF( CellInfoContainer , 1248013772 , 1 );
CONDCONT_DEF( CellInfoContainer , 1163536718 );

#endif
