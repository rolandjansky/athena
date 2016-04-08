/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_AFPTRUTHINFOFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_AFPTRUTHINFOFILLERTOOL_H

#include "AfpCommonHdr.h"
#include "GeneratorObjects/McEventCollection.h"
#include "StoreGate/DataHandle.h"

namespace D3PD
{	
	typedef D3PD::BlockFillerTool<McEventCollection> AfpTruthInfoFillerBase;
	
	class AfpTruthInfoFillerTool : public AfpTruthInfoFillerBase
	{

	public:

		AfpTruthInfoFillerTool( const std::string& type, const std::string& name, const IInterface* parent );

		virtual StatusCode initialize();
		virtual StatusCode book();
		virtual StatusCode fill(const McEventCollection& EventCollection);

	private:
		void ClearData();

	private:
		
		
		//StoreGateSvc* p_detstore;
		
		//***	information about vertex and incoming protons for each collision, pileup considered
		//int* m_pnPrimaryIPs;
		
		vector<float>* m_pvecVtx_g_x;
		vector<float>* m_pvecVtx_g_y;
		vector<float>* m_pvecVtx_g_z;
		vector<float>* m_pvecVtx_g_t;

		vector<float>* m_pvecPBeam1_i_pt;
		vector<float>* m_pvecPBeam1_i_phi;
		vector<float>* m_pvecPBeam1_i_pz;
		vector<float>* m_pvecPBeam1_i_E;
		
		vector<float>* m_pvecPBeam2_i_pt;
		vector<float>* m_pvecPBeam2_i_phi;
		vector<float>* m_pvecPBeam2_i_pz;
		vector<float>* m_pvecPBeam2_i_E;

		//***   information about leading particles (protons and proton like particles)
		
		//int*   m_pnProtons_f;
		
		vector<float>* m_pvecParticle_f_pt;
		vector<float>* m_pvecParticle_f_phi;
		vector<float>* m_pvecParticle_f_pz;
		vector<float>* m_pvecParticle_f_m;
		vector<int>* m_pvecParticle_f_pdg;
		vector<int>* m_pvecParticle_f_barcode;

	}; // class AfpTruthInfoFillerTool

} // namespace D3PD

#endif // not FORWARDDETECTORSD3PDMAKER_AFPTRUTHINFOFILLERTOOL_H
