/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "FastCaloSim/ParticleEnergyShape.h"

#include "FastCaloSimAthenaPool/FastShowerInfo.h"
#include <exception>
#include <stdexcept> // out_of_range exception
#include <iostream>


ParticleEnergyShape::ParticleEnergyShape():Ecal(0),dist_in(0),dist000(0),dist_rec(0),E(0),fcal_tot(0),fcal_tot_uncor(0)
{
  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
    E_layer[sample]=0;
    fcal_layer[sample]=0;
  }  
}

void ParticleEnergyShape::CopyDebugInfo( FastShowerInfo* fsi )
{
	if(!fsi)
	{
		std::cout << "ParticleEnergyShape \tERROR \tSetDebugInfo called with NULL pointer for FastShowerInfo object" << std::endl;
		return;
	}
		
	fsi->m_shape_Ecal = (float)this->Ecal;
	fsi->m_shape_dist_in = (float)this->dist_in;
	fsi->m_shape_dist000 = (float)this->dist000;
	fsi->m_shape_dist_rec = (float)this->dist_rec;
	fsi->m_shape_E = (float)this->E;
	fsi->m_shape_fcal_tot = (float)this->fcal_tot;
	fsi->m_shape_fcal_tot_uncor = (float)this->fcal_tot_uncor;
	
	for( int i = CaloCell_ID_FCS::FirstSample; i < CaloCell_ID_FCS::MaxSample; ++i)
	{
		try
		{
			fsi->m_shape_fcal_layer.at( i ) = (float)this->fcal_layer[i];
			fsi->m_shape_E_layer.at( i ) = (float)this->E_layer[i];
		}
		catch(std::out_of_range& e) 
		{
			std::cerr << e.what( ) << " in ParticleEnergyShape::CopyDebugInfo. Please check your index (" << i << ") and if FastShowerInfo::Initialize has correctly been called and all vecors have been initialized. \n";
		}
	}
}
