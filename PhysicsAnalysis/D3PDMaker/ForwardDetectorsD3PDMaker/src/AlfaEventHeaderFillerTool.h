/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  * @file ForwardDetectorsD3PDMaker/FillerTool.h
  * @author Libor Nozka
  * @date December 2011
  * @brief Block filler tool for Alfa information.
  */
#ifndef FORWARDDETECTORSD3PDMAKER_ALFAEVENTHEADERFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_ALFAEVENTHEADERFILLERTOOL_H

#include "AlfaCommonHdr.h"

namespace D3PD
{

    class AlfaEventHeaderFillerTool : public AlfaDataEventFillerBase
    {

    public:
        /**
       * @brief Standard Gaudi tool constructor.
       * @param type   The name of the tool type.
       * @param name   The tool name.
       * @param parent The tool's Gaudi parent.
       */
        AlfaEventHeaderFillerTool( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );

        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill( const ALFA_CLinkEvent& DataEvent);

    private:
        void ClearData();

    private:
        int m_nDataType;

        //DCS IDs
        unsigned long long* m_pullDCSBlmID;
        unsigned long long* m_pullDCSHVChannelID;
        unsigned long long* m_pullDCSLocalMonitoringID;
        unsigned long long* m_pullDCSMovementID;
        unsigned long long* m_pullDCSRadMonID;
        unsigned long long* m_pullDCSTriggerRatesID;
        unsigned long long* m_pullDCSFEConfigurationID;
        unsigned long long* m_pullDCSTriggerSettingsID;

        //RawDataContainer
        vector<int>* m_pvecScaler;
        int* m_pnBCId;
        int* m_pnTimeStamp;
        int* m_pnTimeStamp_ns;
        vector<int>* m_pvecTrigPat;

        //DigitCollection
        vector<int>* m_pvecMDFiberHits;
        vector<int>* m_pvecMDMultiplicity;

        //ODDigitCollection
        vector<int>* m_pvecODFiberHitsPos;
        vector<int>* m_pvecODFiberHitsNeg;
        vector<int>* m_pvecODMultiplicityPos;
        vector<int>* m_pvecODMultiplicityNeg;

    }; // class AlfaEventHeaderFillerTool

} // namespace D3PD

#endif // FORWARDDETECTORSD3PDMAKER_ALFAEVENTHEADERFILLERTOOL_H
