/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AlfaTrackingDataFillerTool.cxx 470583 2011-11-25 10:33:45Z krasznaa $
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
#include "AlfaTrackingDataFillerTool.h"
#include "ALFA_GloRecEv/ALFA_GloRecEvCollection.h"

namespace D3PD {

    /**
	* @brief Standard Gaudi tool constructor.
	* @param type The name of the tool type.
	* @param name The tool name.
	* @param parent The tool's Gaudi parent.
	*/
    AlfaTrackingDataFillerTool::AlfaTrackingDataFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
        : AlfaDataEventFillerBase( type, name, parent )
    {
        ATH_MSG_INFO( "AlfaTrackingDataFillerTool---> Constructor" );
        //declareProperty( "CalibrateEnergy",      m_calibrateEnergy = false );

        //LocRecEvCollection & LocRecODEvCollection
        m_pvecXDetCS=NULL;
        m_pvecYDetCS=NULL;
        m_pvecDetectorPartID=NULL;
        m_pnMaxTrackCnt=NULL;
        m_pvecOverU=NULL;
        m_pvecOverV=NULL;
        m_pvecOverY=NULL;
        m_pvecNumU=NULL;
        m_pvecNumV=NULL;
        m_pvecNumY=NULL;
        m_pvecMDFibSel=NULL;
        m_pvecODFibSel=NULL;

        //LocRecCorrEvCollection & LocRecCorrODEvCollection
        m_pvecXLhcCS=NULL;
        m_pvecYLhcCS=NULL;
        m_pvecZLhcCS=NULL;
        m_pvecXRPotCS=NULL;
        m_pvecYRPotCS=NULL;
        m_pvecXStatCS=NULL;
        m_pvecYStatCS=NULL;
        m_pvecXBeamCS=NULL;
        m_pvecYBeamCS=NULL;

        //GloRecEvCollection
        // 		m_pnMaxGloTrackCnt=NULL;
        // 		m_pvecGloArm=NULL;
        // 		m_pvecGloXLhcCS=NULL;
        // 		m_pvecGloYLhcCS=NULL;
        // 		m_pvecGloXSlopeLhcCS=NULL;
        // 		m_pvecGloYSlopeLhcCS=NULL;
    }

    /**
	* Initialize: informs the type container to D3PDMaker
	*             retrieves the mapping database
	*             prepares the coefficients for the calibration
	*
	* @return Success or failure (will stop the processing)
	*/
    StatusCode AlfaTrackingDataFillerTool::initialize()
    {
        // Initialize the base class:
        CHECK( AlfaDataEventFillerBase::initialize() );

        ATH_MSG_INFO( "AlfaTrackingDataFillerTool---> Initialized" );

        return StatusCode::SUCCESS;
    }

    StatusCode AlfaTrackingDataFillerTool::book()
    {
        ATH_MSG_DEBUG( "AlfaTrackingDataFillerTool---> Booking Histograms" );

        //LocRecEvCollection & LocRecODEvCollection
        CHECK( addVariable ("x_Det", m_pvecXDetCS));
        CHECK( addVariable ("y_Det", m_pvecYDetCS));
        CHECK( addVariable ("Detector", m_pvecDetectorPartID));
        CHECK( addVariable ("NumTrack", m_pnMaxTrackCnt));
        CHECK( addVariable ("OverU", m_pvecOverU));
        CHECK( addVariable ("OverV", m_pvecOverV));
        CHECK( addVariable ("OverY", m_pvecOverY));
        CHECK( addVariable ("NU", m_pvecNumU));
        CHECK( addVariable ("NV", m_pvecNumV));
        CHECK( addVariable ("NY", m_pvecNumY));
        CHECK( addVariable ("Fib_SelMD", m_pvecMDFibSel));
        CHECK( addVariable ("Fib_SelOD", m_pvecODFibSel));

        //LocRecCorrEvCollection & LocRecCorrODEvCollection
        CHECK( addVariable ("x_LHC", m_pvecXLhcCS));
        CHECK( addVariable ("y_LHC", m_pvecYLhcCS));
        CHECK( addVariable ("z_LHC", m_pvecZLhcCS));
        CHECK( addVariable ("x_Pot", m_pvecXRPotCS));
        CHECK( addVariable ("y_Pot", m_pvecYRPotCS));
        CHECK( addVariable ("x_Stat", m_pvecXStatCS));
        CHECK( addVariable ("y_Stat", m_pvecYStatCS));
        CHECK( addVariable ("x_Beam", m_pvecXBeamCS));
        CHECK( addVariable ("y_Beam", m_pvecYBeamCS));

        //GloRecEvCollection
        // 		CHECK( addVariable ("GloNumTrack", m_pnMaxGloTrackCnt));
        // 		CHECK( addVariable ("GloArm", m_pvecGloArm));
        // 		CHECK( addVariable ("GloX", m_pvecGloXLhcCS));
        // 		CHECK( addVariable ("GloY", m_pvecGloYLhcCS));
        // 		CHECK( addVariable ("GloXSlope", m_pvecGloXSlopeLhcCS));
        // 		CHECK( addVariable ("GloYSlope", m_pvecGloYSlopeLhcCS));

        return StatusCode::SUCCESS;
    }

    StatusCode AlfaTrackingDataFillerTool::fill( const ALFA_CLinkEvent& DataEvent )
    {
        unsigned int i;
        int nPotID, nSideID, nODSign, nMaxTrackCnt=1; // default value (PHamal)
        int arrTrackCntPerRPot[RPOTSCNT];
        vector<int> vecFiberSel;

        //LocRecEvCollection & LocRecODEvCollection
        const ALFA_LocRecEvCollection* pLocRecEvColl=(const ALFA_LocRecEvCollection*)DataEvent.GetLinkedObject(EDVT_LOCRECEVCOLLECTION);
        const ALFA_LocRecODEvCollection* pLocRecODEvColl=(const ALFA_LocRecODEvCollection*)DataEvent.GetLinkedObject(EDVT_LOCRECODEVCOLLECTION);
        ALFA_LocRecEvCollection::const_iterator iterLocRec;
        ALFA_LocRecODEvCollection::const_iterator iterLocRecOD;

        if(pLocRecEvColl!=NULL && pLocRecODEvColl!=NULL){
            // resolve max track count from LocRecEvCollection
            memset(&arrTrackCntPerRPot[0],0,sizeof(arrTrackCntPerRPot));
            for(iterLocRec=pLocRecEvColl->begin();iterLocRec!=pLocRecEvColl->end();iterLocRec++){
                nPotID=(*iterLocRec)->getPotNum();
                arrTrackCntPerRPot[nPotID]++;
            }
            for(iterLocRecOD=pLocRecODEvColl->begin();iterLocRecOD!=pLocRecODEvColl->end();iterLocRecOD++){
                nPotID=(*iterLocRecOD)->getPotNum();
                arrTrackCntPerRPot[nPotID]++;
            }
            for(i=0;i<RPOTSCNT;i++){
                if(arrTrackCntPerRPot[i]>nMaxTrackCnt) nMaxTrackCnt=arrTrackCntPerRPot[i];
            }

            memset(&arrTrackCntPerRPot[0],0,sizeof(arrTrackCntPerRPot));
            ClearData(nMaxTrackCnt,ERC_LOCUNCORRECTED);

            //fill data - LocRecEvCollection
            vecFiberSel.clear();
            for(iterLocRec=pLocRecEvColl->begin();iterLocRec!=pLocRecEvColl->end();iterLocRec++){
                nPotID=(*iterLocRec)->getPotNum();

                (*m_pvecDetectorPartID)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=1;
                (*m_pvecXDetCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getXposition();
                (*m_pvecYDetCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getYposition();

                (*m_pvecOverU)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getOverU();
                (*m_pvecOverV)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getOverV();
                (*m_pvecNumU)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getNumU();
                (*m_pvecNumV)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getNumV();

                vecFiberSel=(*iterLocRec)->getFibSel();
                for(i=0;i<vecFiberSel.size();i++){
                    (*m_pvecMDFibSel)[nPotID*nMaxTrackCnt*MDLAYERSCNT*MDPLATESCNT+arrTrackCntPerRPot[nPotID]*MDLAYERSCNT*MDPLATESCNT+i]=vecFiberSel[i];
                }

                arrTrackCntPerRPot[nPotID]++;
            }

            //fill data - LocRecODEvCollection
            vecFiberSel.clear();
            for(iterLocRecOD=pLocRecODEvColl->begin();iterLocRecOD!=pLocRecODEvColl->end();iterLocRecOD++){
                nPotID=(*iterLocRecOD)->getPotNum();
                nSideID=(*iterLocRecOD)->getSide();

                nODSign=(nSideID==0)? -1:1;
                (*m_pvecDetectorPartID)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(nSideID==0)? 3:2;
                (*m_pvecXDetCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
                (*m_pvecYDetCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecOD)->getYposition();
                (*m_pvecOverY)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecOD)->getOverY();
                (*m_pvecNumY)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecOD)->getNumY();

                vecFiberSel=(*iterLocRecOD)->getFibSel();
                for(i=0;i<vecFiberSel.size();i++){
                    (*m_pvecODFibSel)[nPotID*nMaxTrackCnt*ODPLATESCNT+arrTrackCntPerRPot[nPotID]*ODPLATESCNT+i]=vecFiberSel[i];
                }

                arrTrackCntPerRPot[nPotID]++;
            }
        }
        else{
            msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_LOCREC <<"' or '"<<EVCOLLNAME_LOCRECOD<<"' collection"<<endreq;
            //			return StatusCode::FAILURE;
        }

        //LocRecCorrEvCollection && LocRecCorrODEvCollection
        const ALFA_LocRecCorrEvCollection* pLocRecCorrEvColl=(const ALFA_LocRecCorrEvCollection*)DataEvent.GetLinkedObject(EDVT_LOCRECCORREVCOLLECTION);
        const ALFA_LocRecCorrODEvCollection* pLocRecCorrODEvColl=(const ALFA_LocRecCorrODEvCollection*)DataEvent.GetLinkedObject(EDVT_LOCRECCORRODEVCOLLECTION);
        ALFA_LocRecCorrEvCollection::const_iterator iterLocRecCorr;
        ALFA_LocRecCorrODEvCollection::const_iterator iterLocRecCorrOD;

        if(pLocRecCorrEvColl!=NULL && pLocRecCorrODEvColl!=NULL){
            memset(&arrTrackCntPerRPot[0],0,sizeof(arrTrackCntPerRPot));
            ClearData(nMaxTrackCnt,ERC_LOCCORRECTED);

            //fill data - LocRecCorrEvCollection - ONLY DetCS for now (TODO rest)
            for(iterLocRecCorr=pLocRecCorrEvColl->begin();iterLocRecCorr!=pLocRecCorrEvColl->end();iterLocRecCorr++){
                nPotID=(*iterLocRecCorr)->getPotNum();

                (*m_pvecXLhcCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getXpositionLHC();
                (*m_pvecYLhcCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getYpositionLHC();
                (*m_pvecZLhcCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getZpositionLHC();

                (*m_pvecXRPotCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getXpositionPot();
                (*m_pvecYRPotCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getYpositionPot();

                (*m_pvecXStatCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getXpositionStat();
                (*m_pvecYStatCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getYpositionStat();

                (*m_pvecXBeamCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getXpositionBeam();
                (*m_pvecYBeamCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getYpositionBeam();

                arrTrackCntPerRPot[nPotID]++;
            }

            //fill data - LocRecCorrODEvCollection - ONLY DetCS for now (TODO rest)
            for(iterLocRecCorrOD=pLocRecCorrODEvColl->begin();iterLocRecCorrOD!=pLocRecCorrODEvColl->end();iterLocRecCorrOD++){
                nPotID=(*iterLocRecCorrOD)->getPotNum();
                nSideID=(*iterLocRecCorrOD)->getSide();

                nODSign=(nSideID==0)? -1:1;
                (*m_pvecXLhcCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
                (*m_pvecYLhcCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getYpositionLHC();
                (*m_pvecZLhcCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getZpositionLHC();

                (*m_pvecXRPotCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
                (*m_pvecYRPotCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getYpositionPot();

                (*m_pvecXStatCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
                (*m_pvecYStatCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getYpositionStat();

                (*m_pvecXBeamCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
                (*m_pvecYBeamCS)[nPotID*nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getYpositionBeam();

                arrTrackCntPerRPot[nPotID]++;
            }
        }
        else{
            msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_LOCREC <<"' or '"<<EVCOLLNAME_LOCRECOD<<"' collection"<<endreq;
            //			return StatusCode::FAILURE;
        }

        //GloRecEvCollection
        // 		const ALFA_GloRecEvCollection* pGloRecEvColl=(const ALFA_GloRecEvCollection*)DataEvent.GetLinkedObject(EDVT_GLORECEVCOLLECTION);
        // 		ALFA_GloRecEvCollection::const_iterator iterGloRec;
        //
        // 		if(pGloRecEvColl!=NULL){
        // 			// resolve max track count
        // 			nMaxTrackCnt=pGloRecEvColl->size();
        // 			ClearData(nMaxTrackCnt,ERC_GLOBAL);
        //
        // 			//fill data - GloRecEvCollection
        // 			for(i=0,iterGloRec=pGloRecEvColl->begin();iterGloRec!=pGloRecEvColl->end();i++,iterGloRec++){
        // 				(*m_pvecGloArm)[i]=(*iterGloRec)->getArmNum();
        // 				(*m_pvecGloXLhcCS)[i]=(*iterGloRec)->getXposition();
        // 				(*m_pvecGloYLhcCS)[i]=(*iterGloRec)->getYposition();
        // 				(*m_pvecGloXSlopeLhcCS)[i]=(*iterGloRec)->getXslope();
        // 				(*m_pvecGloYSlopeLhcCS)[i]=(*iterGloRec)->getYslope();
        // 			}
        // 		}
        // 		else{
        // 			msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_GLOREC <<"' collection"<<endreq;
        // 			//return StatusCode::FAILURE;
        // 		}

        return StatusCode::SUCCESS;
    }

    void AlfaTrackingDataFillerTool::ClearData(const int nMaxTrackCnt, eRecType eType)
    {
        //LocRecEvCollection & LocRecEvODCollection
        if(eType==ERC_LOCUNCORRECTED){
            *m_pnMaxTrackCnt=nMaxTrackCnt;

            m_pvecXDetCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecXDetCS->begin(),m_pvecXDetCS->size(),-9999);
            m_pvecYDetCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecYDetCS->begin(),m_pvecYDetCS->size(),-9999);
            m_pvecDetectorPartID->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecDetectorPartID->begin(),m_pvecDetectorPartID->size(),0);
            m_pvecOverU->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecOverU->begin(),m_pvecOverU->size(),-9999);
            m_pvecOverV->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecOverV->begin(),m_pvecOverV->size(),-9999);
            m_pvecOverY->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecOverY->begin(),m_pvecOverY->size(),-9999);
            m_pvecNumU->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecNumU->begin(),m_pvecNumU->size(),-9999);
            m_pvecNumV->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecNumV->begin(),m_pvecNumV->size(),-9999);
            m_pvecNumY->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecNumY->begin(),m_pvecNumY->size(),-9999);
            m_pvecMDFibSel->resize(RPOTSCNT*nMaxTrackCnt*MDLAYERSCNT*MDPLATESCNT);
            fill_n(m_pvecMDFibSel->begin(),m_pvecMDFibSel->size(),-9999);
            m_pvecODFibSel->resize(RPOTSCNT*nMaxTrackCnt*ODPLATESCNT);
            fill_n(m_pvecODFibSel->begin(),m_pvecODFibSel->size(),-9999);
        }
        else if(eType==ERC_LOCCORRECTED){
            *m_pnMaxTrackCnt=nMaxTrackCnt;

            m_pvecXLhcCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecXLhcCS->begin(),m_pvecXLhcCS->size(),-9999);
            m_pvecYLhcCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecYLhcCS->begin(),m_pvecYLhcCS->size(),-9999);
            m_pvecZLhcCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecZLhcCS->begin(),m_pvecZLhcCS->size(),-9999);
            m_pvecXRPotCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecXRPotCS->begin(),m_pvecXRPotCS->size(),-9999);
            m_pvecYRPotCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecYRPotCS->begin(),m_pvecYRPotCS->size(),-9999);
            m_pvecXStatCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecXStatCS->begin(),m_pvecXStatCS->size(),-9999);
            m_pvecYStatCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecYStatCS->begin(),m_pvecYStatCS->size(),-9999);
            m_pvecXBeamCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecXBeamCS->begin(),m_pvecXBeamCS->size(),-9999);
            m_pvecYBeamCS->resize(RPOTSCNT*nMaxTrackCnt);
            fill_n(m_pvecYBeamCS->begin(),m_pvecYBeamCS->size(),-9999);
        }
        /*		else if(eType==ERC_GLOBAL){
			*m_pnMaxGloTrackCnt=nMaxTrackCnt;

			m_pvecGloArm->resize(nMaxTrackCnt);
			fill_n(m_pvecGloArm->begin(),m_pvecGloArm->size(),-1);
			m_pvecGloXLhcCS->resize(nMaxTrackCnt);
			fill_n(m_pvecGloXLhcCS->begin(),m_pvecGloXLhcCS->size(),-9999);
			m_pvecGloYLhcCS->resize(nMaxTrackCnt);
			fill_n(m_pvecGloYLhcCS->begin(),m_pvecGloYLhcCS->size(),-9999);

			m_pvecGloXSlopeLhcCS->resize(nMaxTrackCnt);
			fill_n(m_pvecGloXSlopeLhcCS->begin(),m_pvecGloXSlopeLhcCS->size(),-9999);
			m_pvecGloYSlopeLhcCS->resize(nMaxTrackCnt);
			fill_n(m_pvecGloYSlopeLhcCS->begin(),m_pvecGloYSlopeLhcCS->size(),-9999);
		}*/
    }


} // namespace D3PD
