/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "FastCaloSimAthenaPool/FastShowerInfo.h"
#include <iostream>

#include <exception>
#include <stdexcept> // out_of_range exception

// #include "FastCaloSim/ParticleEnergyShape.h"
// #include "FastCaloSim/ParticleEnergyParametrization.h"
// #include "FastCaloSim/TShape_Result.h"

FastShowerInfo::FastShowerInfo() :	
	m_part_pdgid(0), m_part_barcode(0),
	m_part_eta(0.), m_part_phi(0.), m_part_e(0.), m_part_pt(0.),
	m_calosurf_eta(0.), m_calosurf_phi(0.), m_calosurf_d(0.),
	m_shape_Ecal(0.), m_shape_dist_in(0.), m_shape_dist000(0.), m_shape_dist_rec(0.), 
	m_shape_E(-1.), // negative one, as process_particles returns, if E is negative
	m_shape_fcal_tot(0.), m_shape_fcal_tot_uncor(0.),
	m_pep_id(0), m_pep_e(0.), m_pep_eta(0.)
{
	//std::cout << "=======> FastShowerInfo: Constructor called";
}

void FastShowerInfo::Initialize(const int& cellids)
{
	m_shape_E_layer.resize( cellids, 0. );
	m_shape_fcal_layer.resize( cellids, 0. );
	m_letacalo.resize( cellids, 0. );
	m_lphicalo.resize( cellids, 0. );
	m_fcx.resize( cellids, 0. );
	m_fcy.resize( cellids, 0. );
	m_shaperes_calosample.resize( cellids, 0 );
	m_shaperes_id.resize( cellids, 0 );
	m_shaperes_e.resize( cellids, 0. );
	m_shaperes_eta.resize( cellids, 0. );
	m_layer_bestcell_eta.resize( cellids, 0. );
	m_layer_bestcell_phi.resize( cellids, 0. );
	m_layer_bestcell_hash.resize( cellids, 0 );
	m_layer_etot.resize( cellids, 0. );
	m_layer_etot2.resize( cellids, 0. );
	m_layer_etot3.resize( cellids, 0. );
	m_cell_subetot.resize( cellids, 0. );
	m_cell_id.resize( cellids, 0 );
	m_cell_edeposition_errorcorrected.resize( cellids, 0. );
	m_cell_edeposition_final.resize( cellids, 0. );
}


void FastShowerInfo::AddCellSubETot(const int& sample, const double& subetot, int caloid)
{
	try
	{
		m_cell_subetot.at(sample) =  (float)subetot;
		m_cell_id.at(sample) = caloid;
	} 
	catch(std::out_of_range& e) 
	{
    	std::cerr << e.what( ) << " in FastShowerInfo::AddCellSubETot. Please check your index (" << sample << ") and if Initialize has correctly been called and all vecors have been initialized. \n";
  	}
}

void FastShowerInfo::AddCellEErrorCorrected(const int& sample, const double& energy )
{
	try
	{
		m_cell_edeposition_errorcorrected.at(sample) =  (float)energy;
	} 
	catch(std::out_of_range& e) 
	{
    	std::cerr << e.what( ) << " in FastShowerInfo::AddCellEErrorCorrected. Please check your index (" << sample << ") and if Initialize has correctly been called and all vecors have been initialized. \n";
  	}
}

void FastShowerInfo::AddCellEFinal(const int& sample, const double& energy  /*= 0.*/ /* if detelement not found */ ) 
{
	try
	{
		m_cell_edeposition_final.at(sample) =  (float)energy;
	} 
	catch(std::out_of_range& e) 
	{
    	std::cerr << e.what( ) << " in FastShowerInfo::AddCellEFinal. Please check your index (" << sample << ") and if Initialize has correctly been called and all vecors have been initialized. \n";
  	}
}

void FastShowerInfo::SetLayerInfo( 	const int& sample, const double& elayertot, const double& elayertot2, const double& elayertot3, 
					double eta /*= 0.*/, // case, if ibestcell < 0
					double phi /*= 0.*/,
					unsigned int hash /*= 0*/
					)
{
	try
	{
		m_layer_etot.at(sample) = (float)elayertot;
		m_layer_etot2.at(sample) = (float)elayertot2;
		m_layer_etot3.at(sample) = (float)elayertot3;
		m_layer_bestcell_eta.at(sample) = (float)eta;
		m_layer_bestcell_phi.at(sample) = (float)phi;
		m_layer_bestcell_hash.at(sample) = (int) hash;
	} 
	catch(std::out_of_range& e) 
	{
    	std::cerr << e.what( ) << " in FastShowerInfo::SetLayerInfo. Please check your index (" << sample << ") and if Initialize has correctly been called and all vecors have been initialized. \n";
  	}
}

void FastShowerInfo::SetPtEtaPhiE(const double& pt, const double& eta, const double& phi, const double& e)
{
	m_part_pt = (float)pt;
	m_part_eta = (float)eta;
	m_part_phi = (float)phi;
	m_part_e = (float)e;
}

void FastShowerInfo::SetBarcodeAndPDGId(const int& barcode, const int& pdgid)
{
	m_part_barcode = barcode;
	m_part_pdgid = pdgid;
}

void FastShowerInfo::SetCaloSurface(const double& eta,const double& phi,const double& d)
{
	m_calosurf_eta = (float)eta;
	m_calosurf_phi = (float)phi;
	m_calosurf_d = (float)d;
}

// void FastShowerInfo::SetParticleEnergyShape(const ParticleEnergyShape& pes)
// {
// 	m_shape_Ecal = (float)pes.Ecal;
// 	m_shape_dist_in = (float)pes.dist_in;
// 	m_shape_dist000 = (float)pes.dist000;
// 	m_shape_dist_rec = (float)pes.dist_rec;
// 	m_shape_E = (float)pes.E;
// 	m_shape_fcal_tot = (float)pes.fcal_tot;
// 	m_shape_fcal_tot_uncor = (float)pes.fcal_tot_uncor;
// 	
// 	for( int i = CaloCell_ID::PreSamplerB; i < CaloCell_ID::Unknown; ++i)
// 	{
// 		m_shape_fcal_layer[i] = (float)pes.fcal_layer[i];
// 		m_shape_E_layer[i] = (float)pes.E_layer[i];
// 	}
// }

// void FastShowerInfo::SetParticleEnergyParam( const ParticleEnergyParametrization& pep )
// {
// 	m_pep_id = (int)pep.id();
// 	m_pep_e = (float)pep.E();
// 	m_pep_eta = (float)pep.eta();
// }

void FastShowerInfo::SetCaloInfo( const int& sample, const double& fcx, const double& fcy, const double& letacalo, const double& letaphi)
{
	try
	{
		m_letacalo.at(sample) = (float)letacalo;
		m_lphicalo.at(sample) = (float)letaphi;
		m_fcx.at(sample) = (float)fcx;
		m_fcy.at(sample) = (float)fcy;
	} 
	catch(std::out_of_range& e) 
	{
    	std::cerr << e.what( ) << " in FastShowerInfo::SetCaloInfo. Please check your index (" << sample << ") and if Initialize has correctly been called and all vecors have been initialized. \n";
  	}
}

// void FastShowerInfo::SetTShapeResult( const int& sample, const TShape_Result& shape )
// {
// 	m_shaperes_calosample[sample] = shape.calosample();
// 	m_shaperes_id[sample] = shape.id();
// 	m_shaperes_e[sample] = (float)shape.energy();
// 	m_shaperes_eta[sample] = (float)shape.eta();
// }
		
