/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTrackClusterAssociationTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 * Description: Associate tracks to caloclusters 
 *
 * Created in June 2013 
 */

#ifndef TILETRACKCLUSTERASSOCIATIONTOOL_H
#define TILETRACKCLUSTERASSOCIATIONTOOL_H

/// D3PD INCLUDES
#include "D3PDMakerUtils/MultiAssociationTool.h"

/// GENERIC INCLUDES
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

/// PHYSICS INCLUDES
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

/// TRACK INCLUDES
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"

/// CALORIMETER INCLUDES
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

/// TRACK TOOL INCLUDES
#include "ITrackTools.h"

/// C++ INCLUDES
#include <vector>
#include <string>

/// TRACK TOOL INCLUDES
#include "TrackTools.h"

// CLASS INITIALIATIONS AND NAMESPACES
class CaloCluster;


//typedef CaloClusterContainer CONTAINER;

// ADD TILEMUONMUCELLASSOCIATIONTOOL TO D3PD NAMESPACE
namespace D3PD{

//=======================================================================================
class TileTrackClusterAssociationTool:public MultiAssociationTool<Rec::TrackParticle,CaloCluster>{
//=======================================================================================
    public:
        typedef MultiAssociationTool<Rec::TrackParticle,CaloCluster> Base;

        TileTrackClusterAssociationTool(const std::string& type,const std::string& name, const IInterface* parent);
        virtual ~TileTrackClusterAssociationTool();

        StatusCode initialize();
        virtual StatusCode reset(const Rec::TrackParticle& p);
        virtual const CaloCluster* next();
	typedef std::vector<const CaloCluster*> ListOfClusters;
	typedef std::vector<ListOfClusters> ListOfListsOfClusters;


    private:
	float m_cutSizeOfCone;
        std::vector<const CaloCluster*> m_caloclusterContainer;
        std::string m_caloclusterContainerName;

        std::vector<const CaloCluster*>::const_iterator m_clusterItr;
        std::vector<const CaloCluster*>::const_iterator m_clusterEnd;
        std::vector<const CaloCluster*> m_listOfClusters; 
	CaloClusterContainer* m_outputCont;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;
        std::string m_standalone;
        ToolHandle<ITrackTools> m_inCalo;

}; // TILEMUONMUCELLASSOCIATIONTOOL

} // NAMESPACE D3PD

#endif	/* TILEMUONMUCELLASSOCIATIONTOOL_H */


