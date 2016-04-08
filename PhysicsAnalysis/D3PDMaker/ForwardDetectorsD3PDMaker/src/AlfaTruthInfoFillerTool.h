/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  * @file ForwardDetectorsD3PDMaker/AlfaTruthInfoFillerTool.h
  * @author Libor Nozka
  * @date March 2013
  * @brief Block filler tool for Alfa information.
  */
#ifndef FORWARDDETECTORSD3PDMAKER_ALFATRUTHINFOFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_ALFATRUTHINFOFILLERTOOL_H

#include "AlfaCommonHdr.h"

namespace D3PD
{

	class AlfaTruthInfoFillerTool : public AlfaTruthInfoFillerBase
	{

	public:
		/**
	   * @brief Standard Gaudi tool constructor.
	   * @param type   The name of the tool type.
	   * @param name   The tool name.
	   * @param parent The tool's Gaudi parent.
	   */
		AlfaTruthInfoFillerTool( const std::string& type, const std::string& name, const IInterface* parent );

		virtual StatusCode initialize();
		virtual StatusCode book();
		virtual StatusCode fill(const McEventCollection& EventCollection);

	private:
		void ClearData(const int nPileUpCnt);

	private:
		//vector<float>* m_pvecVtx_g;
		vector<float>* m_pvecVtx_g_x;
		vector<float>* m_pvecVtx_g_y;
		vector<float>* m_pvecVtx_g_z;

		//vector<float>* m_pvecVtx_a;
		vector<float>* m_pvecVtx_a_x;
		vector<float>* m_pvecVtx_a_y;
		vector<float>* m_pvecVtx_a_z;

		//vector<float>* m_pvecVtx_c;
		vector<float>* m_pvecVtx_c_x;
		vector<float>* m_pvecVtx_c_y;
		vector<float>* m_pvecVtx_c_z;

		//vector<float>* m_pvecP_a;
		vector<float>* m_pvecPa_pt;
		vector<float>* m_pvecPa_phi;
		vector<float>* m_pvecPa_pz;
		vector<float>* m_pvecPa_m;

		//vector<float>* m_pvecP_c;
		vector<float>* m_pvecPc_pt;
		vector<float>* m_pvecPc_phi;
		vector<float>* m_pvecPc_pz;
		vector<float>* m_pvecPc_m;

		//vector<float>* m_pvecPBeam1_i;
		vector<float>* m_pvecPBeam1_i_pt;
		vector<float>* m_pvecPBeam1_i_phi;
		vector<float>* m_pvecPBeam1_i_pz;
		vector<float>* m_pvecPBeam1_i_E;

		//vector<float>* m_pvecPBeam1_f;
		vector<float>* m_pvecPBeam1_f_pt;
		vector<float>* m_pvecPBeam1_f_phi;
		vector<float>* m_pvecPBeam1_f_pz;
		vector<float>* m_pvecPBeam1_f_E;

		//vector<float>* m_pvecPBeam2_i;
		vector<float>* m_pvecPBeam2_i_pt;
		vector<float>* m_pvecPBeam2_i_phi;
		vector<float>* m_pvecPBeam2_i_pz;
		vector<float>* m_pvecPBeam2_i_E;

		//vector<float>* m_pvecPBeam2_f;
		vector<float>* m_pvecPBeam2_f_pt;
		vector<float>* m_pvecPBeam2_f_phi;
		vector<float>* m_pvecPBeam2_f_pz;
		vector<float>* m_pvecPBeam2_f_E;

	}; // class AlfaTruthInfoFillerTool

} // namespace D3PD

#endif //FORWARDDETECTORSD3PDMAKER_ALFATRUTHINFOFILLERTOOL_H
