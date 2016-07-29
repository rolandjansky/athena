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
		//std::vector<float>* m_pvecVtx_g;
		std::vector<float>* m_pvecVtx_g_x;
		std::vector<float>* m_pvecVtx_g_y;
		std::vector<float>* m_pvecVtx_g_z;

		//std::vector<float>* m_pvecVtx_a;
		std::vector<float>* m_pvecVtx_a_x;
		std::vector<float>* m_pvecVtx_a_y;
		std::vector<float>* m_pvecVtx_a_z;

		//std::vector<float>* m_pvecVtx_c;
		std::vector<float>* m_pvecVtx_c_x;
		std::vector<float>* m_pvecVtx_c_y;
		std::vector<float>* m_pvecVtx_c_z;

		//std::vector<float>* m_pvecP_a;
		std::vector<float>* m_pvecPa_pt;
		std::vector<float>* m_pvecPa_phi;
		std::vector<float>* m_pvecPa_pz;
		std::vector<float>* m_pvecPa_m;

		//std::vector<float>* m_pvecP_c;
		std::vector<float>* m_pvecPc_pt;
		std::vector<float>* m_pvecPc_phi;
		std::vector<float>* m_pvecPc_pz;
		std::vector<float>* m_pvecPc_m;

		//std::vector<float>* m_pvecPBeam1_i;
		std::vector<float>* m_pvecPBeam1_i_pt;
		std::vector<float>* m_pvecPBeam1_i_phi;
		std::vector<float>* m_pvecPBeam1_i_pz;
		std::vector<float>* m_pvecPBeam1_i_E;

		//std::vector<float>* m_pvecPBeam1_f;
		std::vector<float>* m_pvecPBeam1_f_pt;
		std::vector<float>* m_pvecPBeam1_f_phi;
		std::vector<float>* m_pvecPBeam1_f_pz;
		std::vector<float>* m_pvecPBeam1_f_E;

		//std::vector<float>* m_pvecPBeam2_i;
		std::vector<float>* m_pvecPBeam2_i_pt;
		std::vector<float>* m_pvecPBeam2_i_phi;
		std::vector<float>* m_pvecPBeam2_i_pz;
		std::vector<float>* m_pvecPBeam2_i_E;

		//std::vector<float>* m_pvecPBeam2_f;
		std::vector<float>* m_pvecPBeam2_f_pt;
		std::vector<float>* m_pvecPBeam2_f_phi;
		std::vector<float>* m_pvecPBeam2_f_pz;
		std::vector<float>* m_pvecPBeam2_f_E;

	}; // class AlfaTruthInfoFillerTool

} // namespace D3PD

#endif //FORWARDDETECTORSD3PDMAKER_ALFATRUTHINFOFILLERTOOL_H
