/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloGeometry_h
#define CaloGeometry_h

#include <TMath.h>

#include <vector>
#include <map>
#include <iostream>

#include "ISF_FastCaloSimParametrization/ICaloGeometry.h"
#include "ISF_FastCaloSimParametrization/MeanAndRMS.h"
#include "ISF_FastCaloSimParametrization/FSmap.h"

class CaloDetDescrElement;
class TCanvas;
class TGraphErrors;

typedef std::map< Identifier , const CaloDetDescrElement* > t_cellmap;
typedef std::map< double , const CaloDetDescrElement* > t_eta_cellmap;
   
class CaloGeometryLookup {
public:
   CaloGeometryLookup(int ind=0);
   virtual ~CaloGeometryLookup();
   
   bool IsCompatible(const CaloDetDescrElement* cell);
   void add(const CaloDetDescrElement* cell);
   t_cellmap::size_type size() const {return m_cells.size();};
   int index() const {return m_index;};
   void set_index(int ind) {m_index=ind;};
   void post_process();
   bool has_overlap(CaloGeometryLookup* ref);
   void merge_into_ref(CaloGeometryLookup* ref);
   //void CalculateTransformation();
   
   float mineta() const {return m_mineta;};
   float maxeta() const {return m_maxeta;};
   float minphi() const {return m_minphi;};
   float maxphi() const {return m_maxphi;};
   
   float mineta_raw() const {return m_mineta_raw;};
   float maxeta_raw() const {return m_maxeta_raw;};
   float minphi_raw() const {return m_minphi_raw;};
   float maxphi_raw() const {return m_maxphi_raw;};
   
   float minx() const {return m_mineta;};
   float maxx() const {return m_maxeta;};
   float miny() const {return m_minphi;};
   float maxy() const {return m_maxphi;};
   
   float minx_raw() const {return m_mineta_raw;};
   float maxx_raw() const {return m_maxeta_raw;};
   float miny_raw() const {return m_minphi_raw;};
   float maxy_raw() const {return m_maxphi_raw;};
   
   const MeanAndRMS& deta() {return m_deta;};
   const MeanAndRMS& dphi() {return m_dphi;};
   float mindeta() {return m_mindeta;};
   float mindphi() {return m_mindphi;};
   const MeanAndRMS& dx() {return m_deta;};
   const MeanAndRMS& dy() {return m_dphi;};
   float mindx() {return m_mindeta;};
   float mindy() {return m_mindphi;};

   const MeanAndRMS& eta_correction() {return m_eta_correction;};
   const MeanAndRMS& phi_correction() {return m_phi_correction;};
   const MeanAndRMS& x_correction() {return m_eta_correction;};
   const MeanAndRMS& y_correction() {return m_phi_correction;};
   
   int cell_grid_eta() const {return m_cell_grid_eta;};
   int cell_grid_phi() const {return m_cell_grid_phi;};
   void set_xy_grid_adjustment_factor(float factor) {m_xy_grid_adjustment_factor=factor;};

   virtual const CaloDetDescrElement* getDDE(float eta,float phi,float* distance=0,int* steps=0);

protected:
   float neta_double() {return (maxeta_raw()-mineta_raw())/deta().mean();};
   float nphi_double() {return (maxphi_raw()-minphi_raw())/dphi().mean();};
   Int_t neta() {return TMath::Nint( neta_double() );};
   Int_t nphi() {return TMath::Nint( nphi_double() );};

   //FCal is not sufficiently regular to use submaps with regular mapping
   float nx_double() {return (maxx_raw()-minx_raw())/mindx();};
   float ny_double() {return (maxy_raw()-miny_raw())/mindy();};
   Int_t nx() {return TMath::Nint(TMath::Ceil( nx_double() ));};
   Int_t ny() {return TMath::Nint(TMath::Ceil( ny_double() ));};
   
   float m_xy_grid_adjustment_factor;

   int raw_eta_position_to_index(float eta_raw) const {return TMath::Floor((eta_raw-mineta_raw())/m_deta_double);};
   int raw_phi_position_to_index(float phi_raw) const {return TMath::Floor((phi_raw-minphi_raw())/m_dphi_double);};
   bool index_range_adjust(int& ieta,int& iphi);
   float calculate_distance_eta_phi(const CaloDetDescrElement* DDE,float eta,float phi,float& dist_eta0,float& dist_phi0);

   int m_index;
   t_cellmap m_cells;
   std::vector< std::vector< const CaloDetDescrElement* > > m_cell_grid;
   int m_cell_grid_eta,m_cell_grid_phi;
   float m_mineta,m_maxeta,m_minphi,m_maxphi;
   float m_mineta_raw,m_maxeta_raw,m_minphi_raw,m_maxphi_raw;
   float m_mineta_correction,m_maxeta_correction,m_minphi_correction,m_maxphi_correction;
   
   MeanAndRMS m_deta,m_dphi,m_eta_correction,m_phi_correction;
   float m_mindeta,m_mindphi;
   float m_deta_double,m_dphi_double;
};

class CaloGeometry : virtual public ICaloGeometry {
public :
   static const int MAX_SAMPLING;
   
   static Identifier m_debug_identify;
   static bool m_debug;
   
   CaloGeometry();
   virtual ~CaloGeometry();
   
   virtual bool PostProcessGeometry();

   virtual void Validate();

   virtual const CaloDetDescrElement* getDDE(Identifier identify);
   
   virtual const CaloDetDescrElement* getDDE(int sampling,float eta,float phi,float* distance=0,int* steps=0);
   
   double deta(int sample,double eta) const;
   void   minmaxeta(int sample,double eta,double& mineta,double& maxeta) const;
   double rzmid(int sample,double eta) const;
   double rzent(int sample,double eta) const;
   double rzext(int sample,double eta) const;
   double rmid(int sample,double eta) const;
   double rent(int sample,double eta) const;
   double rext(int sample,double eta) const;
   double zmid(int sample,double eta) const;
   double zent(int sample,double eta) const;
   double zext(int sample,double eta) const;
   //enum SUBPOS { SUBPOS_MID = 0, SUBPOS_ENT = 1, SUBPOS_EXT = 2}; //MID=middle, ENT=entrance, EXT=exit of cal layer
   double rpos(int sample,double eta,int subpos=SUBPOS_MID) const;
   double zpos(int sample,double eta,int subpos=SUBPOS_MID) const;
   double rzpos(int sample,double eta,int subpos=SUBPOS_MID) const;
   bool   isCaloBarrel(int sample) const {return m_isCaloBarrel[sample];};
   static std::string SamplingName(int sample);
   
   TGraphErrors* GetGraph(unsigned int sample) const {return m_graph_layers[sample];};
   void SetDoGraphs(bool dographs=true) {m_dographs=dographs;};
   bool DoGraphs() const {return m_dographs;};

   TCanvas* DrawGeoForPhi0();
   
protected:
   virtual void addcell(const CaloDetDescrElement* cell);
   
   virtual void post_process(int layer);

   virtual void PrintMapInfo(int i, int j);
   
   virtual void InitRZmaps();
   
   t_cellmap m_cells;
   std::vector< t_cellmap > m_cells_in_sampling;
   std::vector< t_eta_cellmap > m_cells_in_sampling_for_phi0;
   std::vector< std::vector< CaloGeometryLookup* > > m_cells_in_regions;

   std::vector< bool > m_isCaloBarrel;
   std::vector< double > m_min_eta_sample[2]; //[side][calosample]
   std::vector< double > m_max_eta_sample[2]; //[side][calosample]
   std::vector< FSmap< double , double > > m_rmid_map[2]; //[side][calosample]
   std::vector< FSmap< double , double > > m_zmid_map[2]; //[side][calosample]
   std::vector< FSmap< double , double > > m_rent_map[2]; //[side][calosample]
   std::vector< FSmap< double , double > > m_zent_map[2]; //[side][calosample]
   std::vector< FSmap< double , double > > m_rext_map[2]; //[side][calosample]
   std::vector< FSmap< double , double > > m_zext_map[2]; //[side][calosample]
   
   bool m_dographs;
   std::vector< TGraphErrors* > m_graph_layers;
   
/*
   double  m_min_eta_sample[2][MAX_SAMPLING]; //[side][calosample]
   double  m_max_eta_sample[2][MAX_SAMPLING]; //[side][calosample]
   FSmap< double , double > m_rmid_map[2][MAX_SAMPLING]; //[side][calosample]
   FSmap< double , double > m_zmid_map[2][MAX_SAMPLING]; //[side][calosample]
   FSmap< double , double > m_rent_map[2][MAX_SAMPLING]; //[side][calosample]
   FSmap< double , double > m_zent_map[2][MAX_SAMPLING]; //[side][calosample]
   FSmap< double , double > m_rext_map[2][MAX_SAMPLING]; //[side][calosample]
   FSmap< double , double > m_zext_map[2][MAX_SAMPLING]; //[side][calosample]
*/
};

#if defined(__MAKECINT__)
#ifndef CaloGeometryFromFile_h
#pragma link C++ class CaloGeometryLookup;
#pragma link C++ class CaloGeometry;
#pragma link C++ class ICaloGeometry;
#endif
#endif

#endif


