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
	std::vector<float>* m_pvecXDetCS;
	std::vector<float>* m_pvecYDetCS;
	std::vector<int>* m_pvecDetectorPartID;
	int* m_pnMaxTrackCnt;
	std::vector<float>* m_pvecOverU;
	std::vector<float>* m_pvecOverV;
	std::vector<float>* m_pvecOverY;
	std::vector<int>* m_pvecNumU;
	std::vector<int>* m_pvecNumV;
	std::vector<int>* m_pvecNumY;
	std::vector<int>* m_pvecMDFibSel;
	std::vector<int>* m_pvecODFibSel;

	//LocRecCorrEvCollection & LocRecCorrODEvCollection
	std::vector<float>* m_pvecXLhcCS;
	std::vector<float>* m_pvecYLhcCS;
	std::vector<float>* m_pvecZLhcCS;
	std::vector<float>* m_pvecXRPotCS;
	std::vector<float>* m_pvecYRPotCS;
	std::vector<float>* m_pvecXStatCS;
	std::vector<float>* m_pvecYStatCS;
	std::vector<float>* m_pvecXBeamCS;
	std::vector<float>* m_pvecYBeamCS;

	//GloRecEvCollection
// 	int* m_pnMaxGloTrackCnt;
// 	std::vector<int>* m_pvecGloArm;
// 	std::vector<float>* m_pvecGloXLhcCS;
// 	std::vector<float>* m_pvecGloYLhcCS;
// 	std::vector<float>* m_pvecGloXSlopeLhcCS;
// 	std::vector<float>* m_pvecGloYSlopeLhcCS;


   }; // class AlfaTrackingDataFillerTool

} // namespace D3PD

#endif // FORWARDDETECTORSD3PDMAKER_ALFATRACKINGDATAFILLERTOOL_H
