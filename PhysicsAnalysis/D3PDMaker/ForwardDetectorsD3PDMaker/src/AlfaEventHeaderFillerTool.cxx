/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AlfaEventHeaderFillerTool.cxx 470583 2011-11-25 10:33:45Z krasznaa $
/**
  * @file ForwardDetectorsD3PDMaker/FillerTool.h
  * @author Yujiao Chen
  * @author Marco Leite <leite@cern.ch>
  * @date August 2010
  * @brief Block filler tool for Alfa information.
  */

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Local include(s):
#include "AlfaEventHeaderFillerTool.h"

namespace D3PD {

    /**
	* @brief Standard Gaudi tool constructor.
	* @param type The name of the tool type.
	* @param name The tool name.
	* @param parent The tool's Gaudi parent.
	*/
    AlfaEventHeaderFillerTool::AlfaEventHeaderFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
        : AlfaDataEventFillerBase( type, name, parent )
    {
        ATH_MSG_INFO( "AlfaEventHeaderFillerTool---> Constructor" );
        //declareProperty( "CalibrateEnergy",      m_calibrateEnergy = false );

        //DCS IDs
        m_pullDCSBlmID=NULL;
        m_pullDCSHVChannelID=NULL;
        m_pullDCSLocalMonitoringID=NULL;
        m_pullDCSMovementID=NULL;
        m_pullDCSRadMonID=NULL;
        m_pullDCSTriggerRatesID=NULL;
        m_pullDCSFEConfigurationID=NULL;
        m_pullDCSTriggerSettingsID=NULL;

        //RawDataContainer
        m_pvecScaler=NULL;
        m_pnBCId=NULL;
        m_pnTimeStamp=NULL;
        m_pnTimeStamp_ns=NULL;
        m_pvecTrigPat=NULL;

        //DigitCollection
        m_pvecMDFiberHits=NULL;
        m_pvecMDMultiplicity=NULL;

        //ODDigitCollection
        m_pvecODFiberHitsPos=NULL;
        m_pvecODFiberHitsNeg=NULL;
        m_pvecODMultiplicityPos=NULL;
        m_pvecODMultiplicityNeg=NULL;

        // data type using in the local reconstruction
        // for the simulation data the value is 0, for the real data the value is 1. Unset value is -1
        declareProperty("DataType", m_nDataType=1, "data type using in the local reconstruction");
    }

    /**
	* Initialize: informs the type container to D3PDMaker
	*             retrieves the mapping database
	*             prepares the coefficients for the calibration
	*
	* @return Success or failure (will stop the processing)
	*/
    StatusCode AlfaEventHeaderFillerTool::initialize()
    {
        // Initialize the base class:
        CHECK( AlfaDataEventFillerBase::initialize() );

        ATH_MSG_INFO( "AlfaEventHeaderFillerTool---> Initialized" );

        //		std::cout << "m_iDataType = " << m_iDataType << std::endl;
        return StatusCode::SUCCESS;
    }

    StatusCode AlfaEventHeaderFillerTool::book()
    {
        ATH_MSG_DEBUG( "AlfaEventHeaderFillerTool---> Booking Histograms" );

        if (m_nDataType==1)
        {
            //DCS IDs
            CHECK( addVariable ("DCS_BLM", m_pullDCSBlmID));
            CHECK( addVariable ("DCS_HVCHANNEL", m_pullDCSHVChannelID));
            CHECK( addVariable ("DCS_LOCALMONITORING", m_pullDCSLocalMonitoringID));
            CHECK( addVariable ("DCS_MOVEMENT", m_pullDCSMovementID));
            CHECK( addVariable ("DCS_RADMON", m_pullDCSRadMonID));
            CHECK( addVariable ("DCS_TRIGGERRATES", m_pullDCSTriggerRatesID));
            CHECK( addVariable ("DCS_FECONFIGURATION", m_pullDCSFEConfigurationID));
            CHECK( addVariable ("DCS_TRIGGERSETTINGS", m_pullDCSTriggerSettingsID));

            //RawDataContainer
            CHECK( addVariable ("Scaler", m_pvecScaler));
            CHECK( addVariable ("BCId", m_pnBCId));
            CHECK( addVariable ("TimeStp", m_pnTimeStamp));
            CHECK( addVariable ("TimeStp_ns", m_pnTimeStamp_ns));
            CHECK( addVariable ("TrigPat", m_pvecTrigPat));
        }

        //DigitCollection
        CHECK( addVariable ("FiberHitsMD", m_pvecMDFiberHits));
        CHECK( addVariable ("MultiMD", m_pvecMDMultiplicity));

        //ODDigitCollection
        CHECK( addVariable ("FiberHitsODPos", m_pvecODFiberHitsPos));
        CHECK( addVariable ("FiberHitsODNeg", m_pvecODFiberHitsNeg));
        CHECK( addVariable ("MultiODPos", m_pvecODMultiplicityPos));
        CHECK( addVariable ("MultiODNeg", m_pvecODMultiplicityNeg));

        return StatusCode::SUCCESS;
    }

    StatusCode AlfaEventHeaderFillerTool::fill( const ALFA_CLinkEvent& DataEvent )
    {
        unsigned int i;
        int nPotID, nPlateID, nFiberID, nSideID;
        ClearData();

        if (m_nDataType==1)
        {
            //DCS IDs
            *m_pullDCSBlmID=DataEvent.GetDCSFolderID(EDCSI_BLM);
            *m_pullDCSHVChannelID=DataEvent.GetDCSFolderID(EDCSI_HVCHANNEL);
            *m_pullDCSLocalMonitoringID=DataEvent.GetDCSFolderID(EDCSI_LOCALMONITORING);
            *m_pullDCSMovementID=DataEvent.GetDCSFolderID(EDCSI_MOVEMENT);
            *m_pullDCSRadMonID=DataEvent.GetDCSFolderID(EDCSI_RADMON);
            *m_pullDCSTriggerRatesID=DataEvent.GetDCSFolderID(EDCSI_TRIGGERRATES);
            *m_pullDCSFEConfigurationID=DataEvent.GetDCSFolderID(EDCSI_FECONFIGURATION);
            *m_pullDCSTriggerSettingsID=DataEvent.GetDCSFolderID(EDCSI_TRIGGERSETTINGS);

            //RawDataContainer
            const ALFA_RawDataContainer* pRawDataColl=(const ALFA_RawDataContainer*)DataEvent.GetLinkedObject(EDVT_RAWDATAEVCOLLECTION);
            ALFA_RawDataContainer::const_iterator iterRawData;

            if(pRawDataColl!=NULL){
                *m_pnTimeStamp=pRawDataColl->GetTimeStamp();
                *m_pnTimeStamp_ns=pRawDataColl->GetTimeStampns();
                *m_pnBCId=pRawDataColl->GetBCId();

                vector<bool> vecRPPattern;
                for(iterRawData=pRawDataColl->begin();iterRawData!=pRawDataColl->end();iterRawData++){
                    nPotID=(*iterRawData)->GetMBId_POT();
                    (*m_pvecScaler)[nPotID-1]=(*iterRawData)->Get_scaler_POT();

                    vecRPPattern=(*iterRawData)->Get_pattern_POT();
                    for(i=0;i<vecRPPattern.size();i++){
                        if(i<RPOTSCNT*TRIGPATCNT) (*m_pvecTrigPat)[(nPotID-1)*TRIGPATCNT+i]=vecRPPattern[vecRPPattern.size()-(i+1)];
                    }
                }
            }
            else{
                msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_RAWDATA <<"' collection"<<endreq;
                //			return StatusCode::FAILURE;
            }
        }

        //DigitCollection
        const ALFA_DigitCollection* pDigitColl= (const ALFA_DigitCollection*)DataEvent.GetLinkedObject(EDVT_DIGITCOLLECTION);
        ALFA_DigitCollection::const_iterator iterDigit;

        if(pDigitColl!=NULL){
            for(iterDigit=pDigitColl->begin();iterDigit!=pDigitColl->end();iterDigit++){
                nPotID=(*iterDigit)->getStation(); //in range 0-7
                nPlateID=(*iterDigit)->getPlate(); //indexed from 0
                nFiberID=(*iterDigit)->getFiber(); //indexed from 0

                if(nPotID<RPOTSCNT && nPlateID<(MDLAYERSCNT*MDPLATESCNT) && nFiberID<MDFIBERSCNT){
                    (*m_pvecMDFiberHits)[(nPotID*MDLAYERSCNT*MDPLATESCNT*MDFIBERSCNT)+(nPlateID*MDFIBERSCNT)+nFiberID]=1;
                    (*m_pvecMDMultiplicity)[(nPotID*MDLAYERSCNT*MDPLATESCNT)+nPlateID]++;
                }
                else{
                    msg(MSG::ERROR) << "Index exceed array size for [RPotID, nPlateID, nFiberID]= ["<<nPotID<<", "<<nPlateID<<", "<<nFiberID<<"]"<<endreq;
                    return StatusCode::FAILURE;
                }
            }
        }
        else{
            msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_DIGIT <<"' collection"<<endreq;
            //			return StatusCode::FAILURE;
        }

        //ODDigitCollection
        const ALFA_ODDigitCollection* pODDigitColl= (const ALFA_ODDigitCollection*)DataEvent.GetLinkedObject(EDVT_ODDIGITCOLLECTION);
        ALFA_ODDigitCollection::const_iterator iterODDigit;

        if(pODDigitColl!=NULL){
            for(iterODDigit=pODDigitColl->begin();iterODDigit!=pODDigitColl->end();iterODDigit++){
                nPotID=(*iterODDigit)->getStation(); //in range 0-7
                nPlateID=(*iterODDigit)->getPlate(); //indexed from 0
                nSideID=(*iterODDigit)->getSide();   //indexed from 0
                nFiberID=(*iterODDigit)->getFiber(); //indexed from 0

                if(nPotID<RPOTSCNT && nPlateID<(ODPLATESCNT) && nFiberID<ODLAYERSCNT*ODFIBERSCNT){
                    if(nSideID==0){ //right side
                        (*m_pvecODFiberHitsNeg)[(nPotID*ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT)+(nPlateID*ODLAYERSCNT*ODFIBERSCNT)+nFiberID]=1;
                        (*m_pvecODMultiplicityNeg)[(nPotID*ODPLATESCNT)+nPlateID]++;
                    }
                    else{ //left side
                        (*m_pvecODFiberHitsPos)[(nPotID*ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT)+(nPlateID*ODLAYERSCNT*ODFIBERSCNT)+nFiberID]=1;
                        (*m_pvecODMultiplicityPos)[(nPotID*ODPLATESCNT)+nPlateID]++;
                    }
                }
                else{
                    msg(MSG::ERROR) << "Index exceed array size for [RPotID, nPlateID, nFiberID, nSideID]= ["<<nPotID<<", "<<nPlateID<<", "<<nFiberID<<", "<<nSideID<<"]"<<endreq;
                    return StatusCode::FAILURE;
                }
            }
        }
        else{
            msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_ODDIGIT <<"' collection"<<endreq;
            //			return StatusCode::FAILURE;
        }

        return StatusCode::SUCCESS;
    }

    void AlfaEventHeaderFillerTool::ClearData()
    {
        if (m_nDataType==1)
        {
            //DCS IDs
            *m_pullDCSBlmID=0;
            *m_pullDCSHVChannelID=0;
            *m_pullDCSLocalMonitoringID=0;
            *m_pullDCSMovementID=0;
            *m_pullDCSRadMonID=0;
            *m_pullDCSTriggerRatesID=0;
            *m_pullDCSFEConfigurationID=0;
            *m_pullDCSTriggerSettingsID=0;

            //RawDataContainer
            m_pvecScaler->resize(RPOTSCNT);
            fill_n(m_pvecScaler->begin(),m_pvecScaler->size(),-1);
            *m_pnBCId=-1;
            *m_pnTimeStamp=-1;
            *m_pnTimeStamp_ns=-1;
            m_pvecTrigPat->resize(RPOTSCNT*TRIGPATCNT);
            fill_n(m_pvecTrigPat->begin(),m_pvecTrigPat->size(),0);
        }

        //DigitCollection
        m_pvecMDFiberHits->resize(RPOTSCNT*MDLAYERSCNT*MDPLATESCNT*MDFIBERSCNT);
        fill_n(m_pvecMDFiberHits->begin(),m_pvecMDFiberHits->size(),0);
        m_pvecMDMultiplicity->resize(RPOTSCNT*MDLAYERSCNT*MDPLATESCNT);
        fill_n(m_pvecMDMultiplicity->begin(),m_pvecMDMultiplicity->size(),0);

        //ODDigitCollection
        m_pvecODFiberHitsPos->resize(RPOTSCNT*ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT);
        fill_n(m_pvecODFiberHitsPos->begin(),m_pvecODFiberHitsPos->size(),0);
        m_pvecODFiberHitsNeg->resize(RPOTSCNT*ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT);
        fill_n(m_pvecODFiberHitsNeg->begin(),m_pvecODFiberHitsNeg->size(),0);

        m_pvecODMultiplicityPos->resize(RPOTSCNT*ODPLATESCNT);
        fill_n(m_pvecODMultiplicityPos->begin(),m_pvecODMultiplicityPos->size(),0);
        m_pvecODMultiplicityNeg->resize(RPOTSCNT*ODPLATESCNT);
        fill_n(m_pvecODMultiplicityNeg->begin(),m_pvecODMultiplicityNeg->size(),0);

    }


} // namespace D3PD
