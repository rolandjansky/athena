/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _FASTSHOWERINFO_H_
#define _FASTSHOWERINFO_H_

#include <vector>

// #include "FastCaloSim/FastCaloSim_CaloCell_ID.h"

// class ParticleEnergyShape;
// class ParticleEnergyParametrization;
// class TShape_Result;

/** 
*	@class FastShowerInfo
*	@brief Class to store information about the Particles and the used shower shapes and parametrizations for these.
*	Some ugly stuff is implemented here for several reasons:
*	There must be no dependency on the Package FastCaloSim! Circular dependencies are evil !
*	Therefore all vectors are first initialized when the method Initialize() is called. So when filling the vectors we have to be careful, as Initialize() 
*	might have not been called yet.
*	And the second consequence is, that this info has to be filled by the classes in FastCaloSim.
*/
class FastShowerInfo
{

	public:
		
		/** @brief Constructor. Don't forget to set default values for all objects! */
		FastShowerInfo();
		
		~FastShowerInfo(){};
		
	public:
	
	// as we need a default constructor without arguments. But on the other hand we need to know the size of the arrays to store the information for each layer in it
		void Initialize(const int& cellids);
		
	// some information about the truth particle, the parametrization has been processed for
		int 	m_part_pdgid; 
		int 	m_part_barcode;
		float 	m_part_eta; 
		float	m_part_phi;
		float	m_part_e;
		float	m_part_pt;
		
	// calo surface information
		float m_calosurf_eta;
		float m_calosurf_phi;
		float m_calosurf_d;
		
	// particle energy shapes
		float m_shape_Ecal;
		float m_shape_dist_in;
		float m_shape_dist000;
		float m_shape_dist_rec;	
		float m_shape_E;
// 		float m_shape_E_layer[CaloCell_ID::Unknown];
		std::vector<float> m_shape_E_layer;
		float m_shape_fcal_tot;
// 		float m_shape_fcal_layer[CaloCell_ID::Unknown];
		std::vector<float> m_shape_fcal_layer;
		float m_shape_fcal_tot_uncor;
		
	// ParticleEnergyParametrization
		int m_pep_id;
		float m_pep_e;
		float m_pep_eta;
	
	// some calo information (not documented)
		std::vector<float> m_letacalo;
		std::vector<float> m_lphicalo;
		std::vector<float> m_fcx;
		std::vector<float> m_fcy;
// 		float m_letacalo[CaloCell_ID::Unknown];
// 		float m_lphicalo[CaloCell_ID::Unknown];
// 		float m_fcx[CaloCell_ID::Unknown];
// 		float m_fcy[CaloCell_ID::Unknown];
		
	// TShapeResults
		std::vector<int> m_shaperes_calosample;
		std::vector<int> m_shaperes_id;
		std::vector<float> m_shaperes_e;
		std::vector<float> m_shaperes_eta;
// 		int m_shaperes_calosample[CaloCell_ID::Unknown];
// 		int m_shaperes_id[CaloCell_ID::Unknown];
// 		float m_shaperes_e[CaloCell_ID::Unknown];
// 		float m_shaperes_eta[CaloCell_ID::Unknown];
		
	// layer info
		std::vector<float> m_layer_etot;
		std::vector<float> m_layer_etot2;
		std::vector<float> m_layer_etot3;
		std::vector<float> m_layer_bestcell_eta;
		std::vector<float> m_layer_bestcell_phi;
		std::vector<int> m_layer_bestcell_hash;
// 		float m_layer_etot[CaloCell_ID::Unknown];
// 		float m_layer_etot2[CaloCell_ID::Unknown];
// 		float m_layer_etot3[CaloCell_ID::Unknown];
// 		float m_layer_bestcell_eta[CaloCell_ID::Unknown];
// 		float m_layer_bestcell_phi[CaloCell_ID::Unknown];
// 		int m_layer_bestcell_hash[CaloCell_ID::Unknown];
		
	// calo cells
		std::vector<int> m_cell_sample;
		std::vector<int> m_cell_id;
		std::vector<float> m_cell_subetot;
		std::vector<float> m_cell_edeposition_errorcorrected;
		std::vector<float> m_cell_edeposition_final;
		
	public:
		
		// All the methods to fill the variables
				
		void AddCellSubETot(const int& sample, const double& subetot, int caloid);
		
		void AddCellEErrorCorrected( const int& sample, const double& energy );
		
		void AddCellEFinal( const int& sample, const double& energy  = 0. /* if detelement not found */ );
		
		void SetLayerInfo( 	const int& sample, const double& elayertot, const double& elayertot2, const double& elayertot3, 
							double eta = 0., // case, if ibestcell < 0
							double phi = 0.,
							unsigned int hash = 0
						  );
		
		void SetPtEtaPhiE(const double& pt, const double& eta, const double& phi, const double& e);
	
		void SetBarcodeAndPDGId(const int& barcode, const int& pdgid);
		
		void SetCaloSurface(const double& eta,const double& phi,const double& d);
		
		void SetCaloInfo( const int& sample, const double& fcx, const double& fcy, const double& letacalo, const double& letaphi);
		
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(FastShowerInfo, 666777, 1)

#endif
