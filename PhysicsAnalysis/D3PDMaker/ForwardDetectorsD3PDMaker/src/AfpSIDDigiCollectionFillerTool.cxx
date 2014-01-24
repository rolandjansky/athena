/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Local include(s):
#include "AfpSIDDigiCollectionFillerTool.h"

namespace D3PD {

    AfpSIDDigiCollectionFillerTool::AfpSIDDigiCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
        : AfpSIDDigiCollectionFillerBase( type, name, parent )
    {
        ATH_MSG_INFO( "AfpSIDDigiCollectionFillerTool---> Constructor" );

        //items per hit pixels
        m_pnTotPixelsCnt=NULL;
        m_pvecPixStationIDs=NULL;
        m_pvecPixLayerIDs=NULL;
        m_pvecPixRow=NULL;
        m_pvecPixColumn=NULL;
        m_pvecPixEAmp=NULL;

        //items per hit layers
        m_pnTotLayersCnt=NULL;
        m_pvecLarStationIDs=NULL;
        m_pvecLarLayerIDs=NULL;
        m_pvecLarHitsPerLayer=NULL;
        m_pvecLarEAmpPerLayer=NULL;

        //items per hit stations
        m_pnTotStationsCnt=NULL;
        m_pvecStaStationIDs=NULL;
        m_pvecStaHitsPerStation=NULL;
        m_pvecStaEAmpPerStation=NULL;
    }

    StatusCode AfpSIDDigiCollectionFillerTool::initialize()
    {
        // Initialize the base class:
        CHECK( AfpSIDDigiCollectionFillerBase::initialize() );
        ATH_MSG_INFO( "AfpSIDDigiCollectionFillerTool---> Initialized" );

        return StatusCode::SUCCESS;
    }

    StatusCode AfpSIDDigiCollectionFillerTool::book()
    {
        ATH_MSG_DEBUG( "AfpSIDDigiCollectionFillerTool---> Booking Histograms" );

        //items per hit pixels
        CHECK(addVariable("pixel_tot_num",m_pnTotPixelsCnt));
        CHECK(addVariable("pixel_stID",m_pvecPixStationIDs));
        CHECK(addVariable("pixel_layerID",m_pvecPixLayerIDs));
        CHECK(addVariable("pixel_row",m_pvecPixRow));
        CHECK(addVariable("pixel_col",m_pvecPixColumn));
        CHECK(addVariable("pixel_E",m_pvecPixEAmp));

        //items per hit layers
        CHECK(addVariable("layer_tot_num",m_pnTotLayersCnt));
        CHECK(addVariable("layer_stID",m_pvecLarStationIDs));
        CHECK(addVariable("layer_layerID",m_pvecLarLayerIDs));
        CHECK(addVariable("layer_npix",m_pvecLarHitsPerLayer));
        CHECK(addVariable("layer_Etot",m_pvecLarEAmpPerLayer));

        //items per hit stations
        CHECK(addVariable("stat_tot_num",m_pnTotStationsCnt));
        CHECK(addVariable("stat_stID",m_pvecStaStationIDs));
        CHECK(addVariable("stat_npix",m_pvecStaHitsPerStation));
        CHECK(addVariable("stat_Etot",m_pvecStaEAmpPerStation));

        return StatusCode::SUCCESS;
    }

    StatusCode AfpSIDDigiCollectionFillerTool::fill(const AFP_SiDigiCollection& DataCollection)
    {
        int i,j,k;
        int arrHitsPerLayer[AFP_STATIONSCNT*AFP_SID_LAYERSCNT];
        int arrHitsPerStation[AFP_STATIONSCNT];
        float arrEAmpPerLayer[AFP_STATIONSCNT*AFP_SID_LAYERSCNT];
        float arrEAmpPerStation[AFP_STATIONSCNT];
        AFP_SiDigiCollection::const_iterator iterColl;

        ClearData(DataCollection);

        memset(&arrHitsPerLayer[0],0,sizeof(arrHitsPerLayer));
        memset(&arrHitsPerStation[0],0,sizeof(arrHitsPerStation));
        memset(&arrEAmpPerLayer[0],0,sizeof(arrEAmpPerLayer));
        memset(&arrEAmpPerStation[0],0,sizeof(arrEAmpPerStation));

        *m_pnTotPixelsCnt=GetNumOfItems(DataCollection,ESCL_PIXEL);
        *m_pnTotLayersCnt=GetNumOfItems(DataCollection,ESCL_LAYER);
        *m_pnTotStationsCnt=GetNumOfItems(DataCollection,ESCL_STATION);

        for(i=0,iterColl=DataCollection.begin();iterColl!=DataCollection.end();i++,iterColl++)
        {
            //items per hit pixels
            (*m_pvecPixStationIDs)[i]=iterColl->m_nStationID;
            (*m_pvecPixLayerIDs)[i]=iterColl->m_nDetectorID;
            (*m_pvecPixRow)[i]=iterColl->m_nPixelRow;
            (*m_pvecPixColumn)[i]=iterColl->m_nPixelCol;
            (*m_pvecPixEAmp)[i]=iterColl->m_fADC;

            //items per hit layers
            if(iterColl->m_nStationID>=0 && iterColl->m_nStationID<AFP_STATIONSCNT &&
               iterColl->m_nDetectorID>=0 && iterColl->m_nDetectorID<AFP_SID_LAYERSCNT)
            {
                arrHitsPerLayer[AFP_SID_LAYERSCNT*(iterColl->m_nStationID)+iterColl->m_nDetectorID]++;
                arrEAmpPerLayer[AFP_SID_LAYERSCNT*(iterColl->m_nStationID)+iterColl->m_nDetectorID]+=iterColl->m_fADC;
            }

            //items per hit stations
            if(iterColl->m_nStationID>=0 && iterColl->m_nStationID<AFP_STATIONSCNT)
            {
                arrHitsPerStation[iterColl->m_nStationID]++;
                arrEAmpPerStation[iterColl->m_nStationID]+=iterColl->m_fADC;
            }
        }

        //items per hit layers
        k=0;
        for(i=0;i<AFP_STATIONSCNT;i++)
        {
            for(j=0;j<AFP_SID_LAYERSCNT;j++)
            {
                if(arrHitsPerLayer[i*AFP_SID_LAYERSCNT+j]>0)
                {
                    (*m_pvecLarStationIDs)[k]=i;
                    (*m_pvecLarLayerIDs)[k]=j;
                    (*m_pvecLarHitsPerLayer)[k]=arrHitsPerLayer[i*AFP_SID_LAYERSCNT+j];
                    (*m_pvecLarEAmpPerLayer)[k]=arrEAmpPerLayer[i*AFP_SID_LAYERSCNT+j];
                    k++;
                }
            }
        }
        if(k!=(*m_pnTotLayersCnt))
        {
            CHECK(StatusCode::FAILURE);
        }

        //items per hit layers
        k=0;
        for(i=0;i<AFP_STATIONSCNT;i++)
        {
            if(arrHitsPerStation[i]>0)
            {
                (*m_pvecStaStationIDs)[k]=i;
                (*m_pvecStaHitsPerStation)[k]=arrHitsPerStation[i];
                (*m_pvecStaEAmpPerStation)[k]=arrEAmpPerStation[i];
                k++;
            }
        }
        if(k!=(*m_pnTotStationsCnt))
        {
            CHECK(StatusCode::FAILURE);
        }

        return StatusCode::SUCCESS;
    }

    void AfpSIDDigiCollectionFillerTool::ClearData(const AFP_SiDigiCollection& DataCollection)
    {
        int nItemsCnt;

        //items per hit pixels
        nItemsCnt=GetNumOfItems(DataCollection,ESCL_PIXEL);
        *m_pnTotPixelsCnt=-1;
        m_pvecPixStationIDs->resize(nItemsCnt);
        fill_n(m_pvecPixStationIDs->begin(),m_pvecPixStationIDs->size(),-1);
        m_pvecPixLayerIDs->resize(nItemsCnt);
        fill_n(m_pvecPixLayerIDs->begin(),m_pvecPixLayerIDs->size(),-1);
        m_pvecPixRow->resize(nItemsCnt);
        fill_n(m_pvecPixRow->begin(),m_pvecPixRow->size(),-1);
        m_pvecPixColumn->resize(nItemsCnt);
        fill_n(m_pvecPixColumn->begin(),m_pvecPixColumn->size(),-1);
        m_pvecPixEAmp->resize(nItemsCnt);
        fill_n(m_pvecPixEAmp->begin(),m_pvecPixEAmp->size(),-1.0);

        //items per hit layers
        nItemsCnt=GetNumOfItems(DataCollection,ESCL_LAYER);
        *m_pnTotLayersCnt=-1;
        m_pvecLarStationIDs->resize(nItemsCnt);
        fill_n(m_pvecLarStationIDs->begin(),m_pvecLarStationIDs->size(),-1);
        m_pvecLarLayerIDs->resize(nItemsCnt);
        fill_n(m_pvecLarLayerIDs->begin(),m_pvecLarLayerIDs->size(),-1);
        m_pvecLarHitsPerLayer->resize(nItemsCnt);
        fill_n(m_pvecLarHitsPerLayer->begin(),m_pvecLarHitsPerLayer->size(),-1);
        m_pvecLarEAmpPerLayer->resize(nItemsCnt);
        fill_n(m_pvecLarEAmpPerLayer->begin(),m_pvecLarEAmpPerLayer->size(),-1.0);

        //items per hit stations
        nItemsCnt=GetNumOfItems(DataCollection,ESCL_STATION);
        *m_pnTotStationsCnt=-1;
        m_pvecStaStationIDs->resize(nItemsCnt);
        fill_n(m_pvecStaStationIDs->begin(),m_pvecStaStationIDs->size(),-1);
        m_pvecStaHitsPerStation->resize(nItemsCnt);
        fill_n(m_pvecStaHitsPerStation->begin(),m_pvecStaHitsPerStation->size(),-1);
        m_pvecStaEAmpPerStation->resize(nItemsCnt);
        fill_n(m_pvecStaEAmpPerStation->begin(),m_pvecStaEAmpPerStation->size(),-1);
    }

    int AfpSIDDigiCollectionFillerTool::GetNumOfItems(const AFP_SiDigiCollection& DataCollection, const enum eSIDCountingLevel eCntLevel)
    {
        int i,nItemCnt=0;
        AFP_SiDigiCollection::const_iterator iterColl;
        int arrHitsPerLayer[AFP_STATIONSCNT*AFP_SID_LAYERSCNT];
        int arrHitsPerStation[AFP_STATIONSCNT];

        switch(eCntLevel)
        {
        case ESCL_PIXEL:
            nItemCnt=DataCollection.size();
            break;
        case ESCL_LAYER:
            memset(&arrHitsPerLayer[0],0,sizeof(arrHitsPerLayer));
            for(iterColl=DataCollection.begin();iterColl!=DataCollection.end();iterColl++)
            {
                if(iterColl->m_nStationID>=0 && iterColl->m_nStationID<AFP_STATIONSCNT &&
                   iterColl->m_nDetectorID>=0 && iterColl->m_nDetectorID<AFP_SID_LAYERSCNT)
                {
                    arrHitsPerLayer[AFP_SID_LAYERSCNT*(iterColl->m_nStationID)+iterColl->m_nDetectorID]++;
                }
            }

            for(i=0;i<AFP_STATIONSCNT*AFP_SID_LAYERSCNT;i++)
            {
                if(arrHitsPerLayer[i]>0) nItemCnt++;
            }
            break;
        case ESCL_STATION:
            memset(&arrHitsPerStation[0],0,sizeof(arrHitsPerStation));
            for(iterColl=DataCollection.begin();iterColl!=DataCollection.end();iterColl++)
            {
                if(iterColl->m_nStationID>=0 && iterColl->m_nStationID<AFP_STATIONSCNT)
                {
                    arrHitsPerStation[iterColl->m_nStationID]++;
                }
            }

            for(i=0;i<AFP_STATIONSCNT;i++)
            {
                if(arrHitsPerStation[i]>0) nItemCnt++;
            }
            break;
        default:
            nItemCnt=0;
            break;

        }

        return nItemCnt;
    }


} // namespace D3PD

