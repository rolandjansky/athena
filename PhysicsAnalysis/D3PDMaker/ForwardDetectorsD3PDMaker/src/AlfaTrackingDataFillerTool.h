/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  * @file ForwardDetectorsD3PDMaker/FillerTool.h
  * @author Libor Nozka
  * @date December 2011
  * @brief Block filler tool for Alfa information.
  */
#ifndef FORWARDDETECTORSD3PDMAKER_ALFATRACKINGDATAFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_ALFATRACKINGDATAFILLERTOOL_H

#include "AlfaCommonHdr.h"

enum eRecType { ERC_LOCUNCORRECTED, ERC_LOCCORRECTED, ERC_GLOBAL };

namespace D3PD
{

   class AlfaTrackingDataFillerTool : public AlfaDataEventFillerBase
   {

   public:
      /**
       * @brief Standard Gaudi tool constructor.
       * @param type   The name of the tool type.
       * @param name   The tool name.
       * @param parent The tool's Gaudi parent.
       */
	  AlfaTrackingDataFillerTool( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

      virtual StatusCode initialize();
      virtual StatusCode book();
	  virtual StatusCode fill( const ALFA_CLinkEvent& DataEvent);

  private:
	  void ClearData(const int nMaxTrackCnt, eRecType eType);

  private:

	//LocRecEvCollection & LocRecODEvCollection
	vector<float>* m_pvecXDetCS;
	vector<float>* m_pvecYDetCS;
	vector<int>* m_pvecDetectorPartID;
	int* m_pnMaxTrackCnt;
	vector<float>* m_pvecOverU;
	vector<float>* m_pvecOverV;
	vector<float>* m_pvecOverY;
	vector<int>* m_pvecNumU;
	vector<int>* m_pvecNumV;
	vector<int>* m_pvecNumY;
	vector<int>* m_pvecMDFibSel;
	vector<int>* m_pvecODFibSel;

	//LocRecCorrEvCollection & LocRecCorrODEvCollection
	vector<float>* m_pvecXLhcCS;
	vector<float>* m_pvecYLhcCS;
	vector<float>* m_pvecZLhcCS;
	vector<float>* m_pvecXRPotCS;
	vector<float>* m_pvecYRPotCS;
	vector<float>* m_pvecXStatCS;
	vector<float>* m_pvecYStatCS;
	vector<float>* m_pvecXBeamCS;
	vector<float>* m_pvecYBeamCS;

	//GloRecEvCollection
// 	int* m_pnMaxGloTrackCnt;
// 	vector<int>* m_pvecGloArm;
// 	vector<float>* m_pvecGloXLhcCS;
// 	vector<float>* m_pvecGloYLhcCS;
// 	vector<float>* m_pvecGloXSlopeLhcCS;
// 	vector<float>* m_pvecGloYSlopeLhcCS;


   }; // class AlfaTrackingDataFillerTool

} // namespace D3PD

#endif // FORWARDDETECTORSD3PDMAKER_ALFATRACKINGDATAFILLERTOOL_H
