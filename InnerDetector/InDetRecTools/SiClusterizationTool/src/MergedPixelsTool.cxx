/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MergedPixelsTool.cxx
// TheSiSimpleClusteringAlg adapted to implement cluster merging
// input from RDOs. 
//
// Changes 
// 29/1/2011 Andi Salzburger & Giacinto Piacquadio (Pixel Clusterization TF)
//           Added support for splitting & use of more refined clustering methods
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/MergedPixelsTool.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Incident.h"
#include "Identifier/IdentifierHash.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetPrepRawData/PixelClusterParts.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterParts.h"
#include "InDetPrepRawData/PixelClusterSplitProb.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitter.h"
#include "InDetIdentifier/PixelID.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "PixelGeoModel/IBLParameterSvc.h"
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"


using CLHEP::micrometer;


namespace InDet {

// Constructor with parameters:
    MergedPixelsTool::MergedPixelsTool(const std::string &type,
        const std::string &name,
        const IInterface *parent) :
    PixelClusteringToolBase(type,name,parent),
    m_incidentSvc("IncidentSvc", name),
    m_IBLParameterSvc("IBLParameterSvc",name),
    m_emulateSplitter(false),
    m_minSplitSize(1),
    m_maxSplitSize(1000),
    m_minSplitProbability(0.),
    m_splitProbTool(""),  
    m_clusterSplitter(""),
    m_doIBLSplitting(false),
    m_IBLAbsent(true),
    m_splitClusterMap(0),
    m_splitClusterMapName("SplitClusterAmbiguityMap"),
    m_processedClusters(0),
    m_modifiedOrigClusters(0),   
    m_splitOrigClusters(0),   
    m_splitProdClusters(0),   
    m_largeClusters(0),
    m_overflowIBLToT(0),
    m_pixofflinecalibSvc("PixelOfflineCalibSvc", name)
    //m_detStore("DetectorStore", name),
    //m_idHelper(0)
    {
      declareInterface<IPixelClusteringTool>(this);
      /// for cluster splitting
      declareProperty("EmulateSplitting",            m_emulateSplitter);
      declareProperty("MinimalSplitSize",            m_minSplitSize);
      declareProperty("MaximalSplitSize",            m_maxSplitSize);
      declareProperty("MinimalSplitProbability",     m_minSplitProbability);
      declareProperty("SplitProbTool",               m_splitProbTool);
      declareProperty("ClusterSplitter",             m_clusterSplitter);
      declareProperty("DoIBLSplitting",		     m_doIBLSplitting);
      declareProperty("SplitClusterAmbiguityMap",    m_splitClusterMapName);
    }
  
//---------------------------------------------------------------------------
// virtual destructor
    MergedPixelsTool::~MergedPixelsTool(){}

    StatusCode  MergedPixelsTool::initialize(){

      if (m_IBLParameterSvc.retrieve().isFailure()) { 
	ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc"); 
      } 
      else { 
	m_clusterSplitter.setTypeAndName(m_IBLParameterSvc->setStringParameters(m_clusterSplitter.typeAndName(),"clusterSplitter")); 
	m_splitProbTool.setTypeAndName(m_IBLParameterSvc->setStringParameters(m_splitProbTool.typeAndName(),"clusterSplitter")); 
	m_IBLParameterSvc->setBoolParameters(m_IBLAbsent,"IBLAbsent");
      }  


    //// Get an Identifier helper object
    //sc = m_detStore.retrieve();
    //if(!sc.isSuccess()){
      //  ATH_MSG_FATAL("Unable to retrieve detector store");
        //return StatusCode::FAILURE;
    //}
    //else {
      //  msg(MSG::DEBUG) << "DetectorStore service found" << endreq;
    //}
 
    //// Get the PixelID Helper
    //if (m_detStore->retrieve(m_idHelper, "PixelID").isFailure()) {
      //  msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
        //return StatusCode::FAILURE;
    //}
    
// for the cluster splitting
        if (!m_splitProbTool.empty() && m_splitProbTool.retrieve().isFailure()) {
            ATH_MSG_FATAL( "Could not retrieve the split probability tool " << m_splitProbTool << "'.");
            return StatusCode::FAILURE;
        }      
        if (!m_clusterSplitter.empty() && m_clusterSplitter.retrieve().isFailure()) {
            ATH_MSG_FATAL( "Could not retrieve the cluster splitter '" << m_clusterSplitter << "'.");
            return StatusCode::FAILURE;
        }

        if (m_incidentSvc.retrieve().isFailure()){
            ATH_MSG_WARNING("Can not retrieve " << m_incidentSvc << ". Exiting.");
            return StatusCode::FAILURE;
        }
        // register to the incident service: 
        // (a) BeginEvent needed for the recording of the Split ambiguity map (always need to be there for merging)
        m_incidentSvc->addListener( this, IncidentType::BeginEvent);
        // (b) EndEvent needed for memory cleanup
        m_incidentSvc->addListener( this, IncidentType::EndEvent);

        if (m_pixofflinecalibSvc.retrieve().isFailure()){
            ATH_MSG_ERROR("Could not retrieve " << m_pixofflinecalibSvc);
            return StatusCode::FAILURE;
        }
	 
	return PixelClusteringToolBase::initialize();
    }


    StatusCode MergedPixelsTool::finalize()
    {
     
      ATH_MSG_DEBUG("------------------- Clusterization Statistics ------------------------");
      ATH_MSG_DEBUG("-- # Processed Pixel Clusters     : " << m_processedClusters);
      if(m_processedClusters){
	ATH_MSG_DEBUG("-- # Clusters modified  (%)       : " << m_modifiedOrigClusters << " (" << double(m_modifiedOrigClusters)/double(m_processedClusters) << ")" );
	ATH_MSG_DEBUG("-- # Clusters split into more (%) : " << m_splitOrigClusters << " (" << double(m_splitOrigClusters)/double(m_processedClusters) << ")" );
	ATH_MSG_DEBUG("-- # Split Clusters created       : " << m_splitProdClusters);
	ATH_MSG_DEBUG("-- # Large Pixel Clusters (%)     : " << m_largeClusters << " (" << double(m_largeClusters)/double(m_processedClusters) << ")" );
      }
      return StatusCode::SUCCESS;
    }

//-----------------------------------------------------------------------
 // Called by the PixelPrepRawDataFormation algorithm once for every pixel 
  // module (with non-empty RDO collection...). 
  // It clusters together the RDOs with a pixell cell side in common.

    PixelClusterCollection* MergedPixelsTool::clusterize(
        const InDetRawDataCollection<PixelRDORawData> &collection,
        const InDetDD::SiDetectorManager& manager,
        const PixelID& pixelID) const
    {
    // Get the messaging service, print where you are

        Identifier elementID = collection.identify();

    // Get hash Identifier for these RDOs
        IdentifierHash idHash = collection.identifyHash();

    // Size of RDO's collection:
        unsigned int RDO_size = collection.size();
        if ( RDO_size==0)
        {
    // Empty RDO collection
            ATH_MSG_DEBUG (" areNeighbours - problems ");
            return 0;
        }

        int clusterNumber = 0;

    // If module is bad, do not create a cluster collection
        if (m_useModuleMap &&  !(m_summarySvc->isGood(idHash))) 
            return 0;


        typedef InDetRawDataCollection<PixelRDORawData> RDO_Collection;

        RDO_Collection::const_iterator firstRDO(collection.begin());
        RDO_Collection::const_iterator lastRDO(collection.end());

     // Get detector info.
    // Find detector element for these RDOs

        InDetDD::SiDetectorElement* element = manager.getDetectorElement(elementID);

        const Trk::RectangleBounds *mybounds=dynamic_cast<const Trk::RectangleBounds *>(&element->surface().bounds());
				if (not mybounds){
  				ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of MergedPixelsTool.cxx.");
  				return nullptr;
  			}
    // RDOs will be accumulated as a vector of Identifier, which will
    // be added to a vector of groups.

        RDO_GroupVector rdoGroups;
        rdoGroups.reserve(RDO_size);

    // A similar structure is created for TOT information
        TOT_GroupVector totGroups;
        totGroups.reserve(RDO_size);
        TOT_GroupVector lvl1Groups;
        lvl1Groups.reserve(RDO_size);

    // loop on all RDOs
        for(RDO_Collection::const_iterator nextRDO=collection.begin() ; 
        nextRDO!=collection.end() ; ++nextRDO) 
        {
            Identifier rdoID= (*nextRDO)->identify();
    // if a pixel is not skip it in clusterization
            if ( m_usePixelMap && !(m_summarySvc->isGood(idHash,rdoID)) ) continue;
            int tot = (*nextRDO)->getToT();
            int lvl1= (*nextRDO)->getLVL1A();
    // check if this is a ganged pixel    
            Identifier gangedID;
    //	bool ganged= isGanged(rdoID, elementID, pixelID, gangedID);
            bool ganged= isGanged(rdoID, element, gangedID);  
            if(ganged){
                ATH_MSG_VERBOSE("Ganged Pixel, row = " << pixelID.phi_index(rdoID) 
                    << "Ganged row = " << pixelID.phi_index(gangedID));   
            }
            else
                ATH_MSG_VERBOSE("Not ganged Pixel, row = " << pixelID.phi_index(rdoID)); 
            
    // loop on all existing RDO groups, until you find that the RDO 
    // is a neighbour of the group.
            bool found= false;

            RDO_GroupVector::iterator firstGroup = rdoGroups.begin();
            RDO_GroupVector::iterator lastGroup  = rdoGroups.end();
            TOT_GroupVector::iterator totGroup = totGroups.begin();    
            TOT_GroupVector::iterator lvl1Group  = lvl1Groups.begin();

            while( !found && firstGroup!= lastGroup)
            {
        // Check if RDO is neighbour of the cluster
                if (areNeighbours(**firstGroup, rdoID, element, pixelID)) 
                {

          // if RDO is a duplicate of one in the cluster, do not add it. 
          // Instead the method isDuplicated check wether the new
          // one has a larger LVL1 - if so it does replace the old 
          // lvl1 with the new one.  
                    if(!isDuplicated(**firstGroup, **lvl1Group, rdoID, lvl1, pixelID)){
                        (*firstGroup)->push_back(rdoID);
                        (*totGroup)->push_back(tot);
                        (*lvl1Group)->push_back(lvl1);
                  // see if it is a neighbour to  any other groups
                        checkForMerge(rdoID, firstGroup, lastGroup, 
                            totGroup, lvl1Group, element, pixelID); 
                    }
                    else{
                        ATH_MSG_VERBOSE("duplicate found");
                    }
                    found = true; 
                }
                ++firstGroup;
                ++totGroup;
                ++lvl1Group;
            }

    // if RDO is isolated, create new cluster. 

            if(!found)
            {
                RDO_Vector* newGroup= new RDO_Vector;
                rdoGroups.push_back(newGroup);
                newGroup->push_back(rdoID);
                TOT_Vector* newtotGroup = new TOT_Vector;
                totGroups.push_back(newtotGroup);
                newtotGroup->push_back(tot);
                TOT_Vector* newlvl1Group = new TOT_Vector;
                lvl1Groups.push_back(newlvl1Group);
                newlvl1Group->push_back(lvl1);
            }

    // Repeat for ganged pixel if necessary
            if (! ganged) continue;

            ATH_MSG_VERBOSE("Ganged pixel, row = " << pixelID.phi_index(gangedID));	
            found= false;

            firstGroup = rdoGroups.begin();
            lastGroup  = rdoGroups.end();
            totGroup   = totGroups.begin();
            lvl1Group  = lvl1Groups.begin();

            while( !found && firstGroup!= lastGroup)
            {
        // if  neighbour of the cluster, add it to the cluster

                if ( areNeighbours(**firstGroup, gangedID, element, pixelID) ) 
                {
                    ATH_MSG_VERBOSE("Ganged pixel is neighbour of a cluster");	

               // if RDO is a duplicate of one in the cluster, do not add it. 
           // Instead the method isDuplicated check wether the new
          // one has a larger LVL1 - if so it does replace the old 
          // lvl1 with the new one.  
                    if(!isDuplicated(**firstGroup, **lvl1Group, gangedID, lvl1, pixelID)){

                        (*firstGroup)->push_back(gangedID);
                        (*totGroup)->push_back(tot);
                        (*lvl1Group)->push_back(lvl1);
                        checkForMerge(gangedID, firstGroup, lastGroup, 
                            totGroup, lvl1Group, element, pixelID);
                    }
                    else{
                        ATH_MSG_VERBOSE("duplicate found");
                    }
                    found = true;

                }
                ++firstGroup;
                ++totGroup;
                ++lvl1Group;
            }

    // if  isolated, create new cluster. 

            if(!found)
            {
                ATH_MSG_VERBOSE("New cluster with ganged pixel");	
                RDO_Vector* newGroup= new RDO_Vector;
                rdoGroups.push_back(newGroup);
                newGroup->push_back(gangedID);
                TOT_Vector* newtotGroup = new TOT_Vector;
                totGroups.push_back(newtotGroup);
                newtotGroup->push_back(tot);
                TOT_Vector* newlvl1Group = new TOT_Vector;
                lvl1Groups.push_back(newlvl1Group);
                newlvl1Group->push_back(lvl1);
            }

        }

        if(totGroups.size() != rdoGroups.size())
            ATH_MSG_ERROR("Mismatch between RDO identifier and TOT info!");
    // We now have groups of contiguous RDOs. Make clusters
    // Make a new empty cluster collection
        PixelClusterCollection  *clusterCollection =  
            new PixelClusterCollection(idHash);
        clusterCollection->setIdentifier(elementID);
        clusterCollection->reserve(rdoGroups.size());

        std::vector<TOT_Vector *>::iterator totgroup = totGroups.begin();
        std::vector<TOT_Vector *>::iterator lvl1group= lvl1Groups.begin();

    // LOOP over the RDO-groups to be clustered ----------------------------------------------------------------
    // Logics in splitting is: the splitter is only activated clusters within min/max split size,
    //  single pixel clusters are not split into more clusters, but can be modified if included by split size req.
    //  additionally:
    // (1) split size > m_minSplitSize : just follow output of the splitter
    // (2) split size <= m_minSplitSize && no split done by the splitter : 
    //      take position update of splitter if available (profit from NN), flag as split 
    // (3) split size <= m_minSplitSize && split done : fallback to pseudo-emulation mode 
    //      (i.e. do not allow splitting, but write split probabilities, do not flag as split)
    // (E) if configured to run in emulation mode, flag as split, save split information, but never split
    //
    // (MAIN CLUSTERIZATION LOOP after connected component finding)
    
        for(std::vector<RDO_Vector *>::iterator group = rdoGroups.begin() ; group!= rdoGroups.end() ; ++group) 
        {

               // the split probabilities
               // writing the split boolean is done the following way
               // - if in emulation mode: always write the output of the splitter for validation
               // - if in pseudo-emulation mode (1-pixel clusters): set boolean to false, but keep split probs
               bool   clusterModified    = false;
               bool   clusterSplit       = false;
               double clusterSplitP1     = 0.;
               double clusterSplitP2     = 0.;
               bool singlePixelSplitCase = 0;
    // the size of the inition rdo group
               size_t groupSize = (**group).size();
    // If cluster is empty, i.e. it has been merged with another, 
    // do not attempt to make cluster.
            if ( groupSize > 0 )
            {
                ++m_processedClusters;
                // create the original cluster - split & split probs are by default false and 0
                PixelCluster* cluster = makeCluster(**group,
                                                    **totgroup,
                                                    **lvl1group,
                                                    element,
                                                    pixelID, 
                                                    ++clusterNumber);
    // check for splitting
           if ( groupSize >= m_minSplitSize && groupSize <= m_maxSplitSize ) {

               // prepare for the return value of the pixel cluster 
               std::vector<InDet::PixelClusterParts> splitClusterParts;
               if ( !m_splitProbTool.empty() && (m_doIBLSplitting || m_IBLAbsent || !element->isBlayer())){                   
                   InDet::PixelClusterSplitProb splitProbObj = m_splitProbTool->splitProbability(*cluster);
                   clusterSplitP1 = splitProbObj.splitProbability(2);
                   clusterSplitP2 = splitProbObj.splitProbability(3);
                   ATH_MSG_VERBOSE( "Obtained split prob object with split prob: " << splitProbObj.splitProbability());
                   if ( splitProbObj.splitProbability() >  m_minSplitProbability ) {
                       ATH_MSG_VERBOSE( "Trying to split cluster ... ");
                       splitClusterParts = m_clusterSplitter->splitCluster(*cluster,splitProbObj);
                   }
               } else if ( !m_clusterSplitter.empty() && (m_doIBLSplitting || m_IBLAbsent || !element->isBlayer())) 
                    splitClusterParts = m_clusterSplitter->splitCluster(*cluster);
              // check if splitting worked
              clusterModified      = !m_emulateSplitter && splitClusterParts.size() > 0;
              clusterSplit         = (splitClusterParts.size() > 1);
              // exclusion: do not allow 
              singlePixelSplitCase = (groupSize==1) && clusterSplit;
              
              // CASE A: perform the actual split & create new clusters 
              if (clusterModified && !singlePixelSplitCase){
                 if ( splitClusterParts.size() > 1)
                     ATH_MSG_VERBOSE( "--> Cluster with " << groupSize << " pixels is split into " << splitClusterParts.size() << " parts.");
                 else 
                     ATH_MSG_VERBOSE( "--> Cluster is not actually split, but eventually modified, filling isSplit as: " << (clusterSplit || groupSize == 1));
                  
                 std::vector<InDet::PixelCluster*> splitClusters;
                 // statistics output                
                 if (clusterSplit){
	                 ++m_splitOrigClusters;
                     m_splitProdClusters += splitClusterParts.size();                                
                  } else 
                     ++m_modifiedOrigClusters;

                  ATH_MSG_VERBOSE( "--> Non-zero cluster split size. Try to use new clusterization...");
                  // iterate and make clusters
                  std::vector<InDet::PixelClusterParts>::iterator splitClusterPartsIter    = splitClusterParts.begin();
                  std::vector<InDet::PixelClusterParts>::iterator splitClusterPartsIterEnd = splitClusterParts.end();
                  // use internal clustering if no position is estimated by clustersplitter
                  if ( !(*splitClusterPartsIter).localPosition()){

                    ATH_MSG_VERBOSE( "--> Position estimate from new clusterization not available... Use old clusterization");
                    for ( ; splitClusterPartsIter != splitClusterPartsIterEnd; ++splitClusterPartsIter ){
                        // make a new cluster, use standard clusterization to have a consistent clustering used
                        PixelCluster* splitCluster =  makeCluster((*splitClusterPartsIter).identifierGroup(),
                                                                  (*splitClusterPartsIter).totGroup(),
                                                                  (*splitClusterPartsIter).lvl1Group(),
                                                                  element,
                                                                  pixelID,
                                                                  ++clusterNumber,
                                                                  (clusterSplit || groupSize == 1),
                                                                  clusterSplitP1,
                                                                  clusterSplitP2);
			            splitCluster->setHashAndIndex(clusterCollection->identifyHash(), clusterCollection->size());
                        /** end new stuff */
                        clusterCollection->push_back(splitCluster);
                        // @TODO: fill these clusters into an ambiguity map
                        splitClusters.push_back(splitCluster);
                    }
                 } else {

                    std::vector<InDet::PixelClusterParts>::iterator splitClusterPartsIter    = splitClusterParts.begin();
                    std::vector<InDet::PixelClusterParts>::iterator splitClusterPartsIterEnd = splitClusterParts.end();

                    ATH_MSG_VERBOSE( "--> Processing new splitCluster with n. " << splitClusterParts.size() << " subClusters. ");

                    for ( size_t iclus = 0 ; splitClusterPartsIter != splitClusterPartsIterEnd; ++splitClusterPartsIter, ++iclus ){
                      
                      const Amg::Vector2D& position=(*(*splitClusterPartsIter).localPosition());
                      const Amg::MatrixX&   error=(*(*splitClusterPartsIter).errorMatrix());
                      const std::vector<int>&   totGroup=(*splitClusterPartsIter).totGroup();
                      const std::vector<int>&   lvl1Group=(*splitClusterPartsIter).lvl1Group();
                      const std::vector<Identifier>& identifierGroup=(*splitClusterPartsIter).identifierGroup();

                       ATH_MSG_VERBOSE( "--> New Cluster :" << iclus << " - Position: " << position << " error: " << error );
                       
                       InDetDD::SiLocalPosition subPos(position);
                       if (std::abs(subPos.xPhi())>mybounds->halflengthPhi()) {
                         double newxphi= (subPos.xPhi()>0) ? mybounds->halflengthPhi()-0.001 : -mybounds->halflengthPhi()+0.001;
                         subPos.xPhi(newxphi);
                       }
                       if (std::abs(subPos.xEta())>mybounds->halflengthEta()) {
                         double newxeta= (subPos.xEta()>0) ? mybounds->halflengthEta()-0.001 : -mybounds->halflengthEta()+0.001;
                         subPos.xEta(newxeta);
                       }
                       
                       const Identifier idSubCluster = element->identifierOfPosition(subPos);
                       
                       
                       std::vector<Identifier>::const_iterator rdosBegin = identifierGroup.begin();
                       std::vector<Identifier>::const_iterator rdosEnd = identifierGroup.end();
                       
                       int rowMin = int(2*(element->width()/element->phiPitch()))+1;
                       int rowMax = 0;
                       int colMin = int(2*(element->length()/element->etaPitch()))+1;;
                       int colMax = 0;
                       
                       for (; rdosBegin!= rdosEnd; ++rdosBegin)
                       {
                         Identifier rId =  *rdosBegin;
                         int row = pixelID.phi_index(rId);
                         int col = pixelID.eta_index(rId);
                         
                         if (row < rowMin){ 
                           rowMin = row; 
                         }
                         if (row > rowMax){
                           rowMax = row;
                         }
                         if (col < colMin){
                           colMin = col;
                         }
                         if (col > colMax){
                           colMax = col;
                         }
                       }
                       const InDetDD::PixelModuleDesign* design
                           (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
                       if (not design){
  										 		ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of MergedPixelsTool.cxx.");
  												return nullptr;
  											}
                       int colWidth = colMax-colMin+1;
                       int rowWidth = rowMax-rowMin+1;
                       double etaWidth = design->widthFromColumnRange(colMin, colMax);
                       double phiWidth = design->widthFromRowRange(rowMin, rowMax);
                       SiWidth siWidth(Amg::Vector2D(rowWidth,colWidth), Amg::Vector2D(phiWidth,etaWidth) );
                       
                       // create the new cluster
                       PixelCluster* splitCluster = new PixelCluster( 
                           idSubCluster,
                           position,
                           identifierGroup,
                           lvl1Group[0], 
                           totGroup,
                           cluster->chargeList(),
                           siWidth,
                           element,
                           new Amg::MatrixX(error),
                           cluster->omegax(),cluster->omegay(),
                           (clusterSplit || groupSize == 1),
                           clusterSplitP1,
                           clusterSplitP2
                           );
                       
                        splitCluster->setHashAndIndex(clusterCollection->identifyHash(), clusterCollection->size());
                        clusterCollection->push_back(splitCluster);
                        splitClusters.push_back(splitCluster);

                    }//end iteration on split clusters
                  } //end if no content in split clusters

                  // fill the clusters into the ambiguity map
                  std::vector<InDet::PixelCluster*>::iterator iterOne = splitClusters.begin();
                  std::vector<InDet::PixelCluster*>::iterator endOne = splitClusters.end();
                  for ( ; iterOne != endOne; ++iterOne){
                      std::vector<InDet::PixelCluster*>::iterator iterTwo = splitClusters.begin();
                      std::vector<InDet::PixelCluster*>::iterator endTwo = splitClusters.end();
                      for ( ; iterTwo != endTwo; ++iterTwo){
                          if (iterTwo != iterOne) 
                              (m_splitClusterMap)->insert(std::make_pair((*iterOne),(*iterTwo)));
                      }
                  }
                  // delete the original cluster
                  delete cluster; cluster = 0;
              }//end if split clusters size is 0
              else if (m_emulateSplitter || singlePixelSplitCase){
                  // create a new cluser with updated split information
                  if (singlePixelSplitCase)
                      ATH_MSG_VERBOSE( "--> Cluster is a single pixel cluser - no split performed, only fill split information - do not flag as split.");
                  else 
                      ATH_MSG_VERBOSE( "--> Emulation mode: no split performed, only fill split information - flagged split.");
                  
                  PixelCluster* emulatedCluster = new PixelCluster( 
                         cluster->identify(),
                         cluster->localPosition(),
                         cluster->rdoList(),
                         cluster->LVL1A(), 
                         cluster->totList(),
                         cluster->chargeList(),
                         cluster->width(),
                         element,
                         new Amg::MatrixX(cluster->localCovariance()),
                         cluster->omegax(),cluster->omegay(),
                         (m_emulateSplitter ? clusterSplit : false),
                         clusterSplitP1,
                         clusterSplitP2
                         );
                     
                      emulatedCluster->setHashAndIndex(clusterCollection->identifyHash(), clusterCollection->size());
                      clusterCollection->push_back(emulatedCluster);
                      //splitClusters.push_back(splitCluster); // check if we want it there or not
                  
                  // delete the original cluster
                  delete cluster; cluster = 0;
              } else {
                ATH_MSG_VERBOSE( "ZERO cluster split size is. Not replacing old cluster...");
              }
              
                
           } 
          // no merging has been done;
          if (cluster){ 
            // statistics output
            if ((**group).size() >= m_maxSplitSize ) ++m_largeClusters;
               
            /** new: store hash id and index in the cluster:
            * hash can be obtained from collection
            * index is just the size of the coll before the push back
            this is needed for later to make the EL to IDC valid in the 
            RIO_OnTrack objects set method might be temporary, this tool 
            (MergedPixelsTool could be friend of the cluster objects)
            */
            cluster->setHashAndIndex(clusterCollection->identifyHash(), 
            clusterCollection->size());
            /** end new stuff */
            clusterCollection->push_back(cluster);
          }
        (**group).clear();
    }
    delete *group;     // now copied into cluster
    *group = 0;
    delete *totgroup;  // won't be used any more (cluster class has not 
                      // TOT info at the moment).	
    *totgroup=0;
    totgroup++;
    delete *lvl1group; // now used into cluster
    *lvl1group=0;
    lvl1group++;
}
return clusterCollection;
}

//-----------------------------------------------------------------------
  // Once the lists of RDOs which makes up the clusters have been found by the
  // clusterize() method, this method is called for each of these lists.
  // The method computes the local position of the cluster, and create 
  // a "cluster object" with all the required information (including charge 
  // interpolation variables Omegax and Omegay, and whether the cluster 
  // contains ganged pixels) 
  // This method calls the ClusterMakerTool to compute global position and 
  // position errors.
  // Input parameters are the list of RDOs identifier of the would-be 
  // cluster, the list of TOT values, the module the cluster belongs to,
  // the pixel helper tool and the number of RDOS of the cluster.

PixelCluster* MergedPixelsTool::makeCluster
    (const std::vector<Identifier>& group,
    const std::vector<int>& totgroup,
    const std::vector<int>& lvl1group,
    const InDetDD::SiDetectorElement* element,
    const PixelID& pixelID,
    int& clusterNumber,
    bool split,
    double splitProb1,
    double splitProb2) const
{
    ATH_MSG_VERBOSE("makeCluster called, number " << clusterNumber);

    Identifier gangedID;
    Identifier elementID = element->identify();
    const InDetDD::PixelModuleDesign* design
        (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
		if (not design){
			ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of MergedPixelsTool.");
			return nullptr;
		}
    int rowMin = int(2*(design->width()/design->phiPitch()))+1;;
    int rowMax = 0;
    int colMin = int(2*(design->length()/design->etaPitch()))+1;
    int colMax = 0;
    int qRowMin = 0;  int qRowMax = 0;
    int qColMin = 0;  int qColMax = 0;
    int lvl1min = 15; // lvl1 of a cluster is the minimum of the lvl1 of the hits

    InDetDD::SiLocalPosition sumOfPositions(0,0,0);
    std::vector<Identifier> DVid;
    DVid.reserve( group.size() );
    std::vector<Identifier>::const_iterator rdosBegin = group.begin();
    std::vector<Identifier>::const_iterator rdosEnd = group.end();
    std::vector<int>::const_iterator tot = totgroup.begin();    
    std::vector<int>::const_iterator lvl1= lvl1group.begin();    

    if(m_IBLParameterSvc->containsIBL()) m_overflowIBLToT = m_pixofflinecalibSvc->getIBLToToverflow();

    // Flag to tag clusters with any ganged pixel
    bool hasGanged = false;  
    for (; rdosBegin!= rdosEnd; ++rdosBegin)
    {
        // compute cluster lvl1
        if ( (*lvl1) < lvl1min ) lvl1min=(*lvl1);
        lvl1++;
        // process identifier
        Identifier rId =  *rdosBegin;
        int row = pixelID.phi_index(rId);
        int col = pixelID.eta_index(rId);
        // flag if cluster contains at least a ganged pixel
        hasGanged = hasGanged || isGanged(rId, element, gangedID);

        DVid.push_back(rId);
        InDetDD::SiLocalPosition siLocalPosition
            (design->positionFromColumnRow(col,row)); 
        sumOfPositions += siLocalPosition;


        // check overflow for IBL
        int realtot = *tot;
        if( m_IBLParameterSvc->containsIBL() && pixelID.barrel_ec(rId) == 0 && pixelID.layer_disk(rId) == 0 ) {
          if (*tot >= m_overflowIBLToT ) realtot = m_overflowIBLToT;
          ATH_MSG_DEBUG("barrel_ec = " << pixelID.barrel_ec(rId) << " layer_disque = " <<  pixelID.layer_disk(rId) << " ToT = " << *tot << " Real ToT = " << realtot);
        }
	   
        if (row == rowMin) qRowMin += realtot;
        if (row < rowMin){ 
            rowMin = row; 
            qRowMin = realtot;
        }

        if (row == rowMax) qRowMax += realtot;
        if (row > rowMax){
            rowMax = row;
            qRowMax = realtot;
        }

        if (col == colMin) qColMin += realtot;
        if (col < colMin){
            colMin = col;
            qColMin = realtot;
        }

        if (col == colMax) qColMax += realtot;
        if (col > colMax){
            colMax = col;
            qColMax = realtot;
        }
        tot++;
    } 
    int numberOfPixels = group.size();
    InDetDD::SiLocalPosition centroid(sumOfPositions/numberOfPixels);
    const Identifier id = element->identifierOfPosition(centroid);

    int colWidth = colMax-colMin+1;
    int rowWidth = rowMax-rowMin+1;

    // Compute eta for charge interpolation correction (if required)
    // Two pixels may have tot=0 (very rarely, hopefully)
    float etaRow = -1;
    float etaCol = -1;
    if(qRowMin+qRowMax > 0) etaRow = qRowMax/float(qRowMin+qRowMax);
    if(qColMin+qColMax > 0) etaCol = qColMax/float(qColMin+qColMax);

    double etaWidth = design->widthFromColumnRange(colMin, colMax);
    double phiWidth = design->widthFromRowRange(rowMin, rowMax);
    SiWidth siWidth(Amg::Vector2D(rowWidth,colWidth),
		    Amg::Vector2D(phiWidth,etaWidth) );

    // Charge interpolation. Very rough guess (one can do better with 
    // candidate track information later) TL
    if(m_posStrategy == 1 && !hasGanged && etaRow>0 && etaCol > 0){
      // width of the region of charge sharing
      // For disks assume normal incidence: delta is small, due to diffusion
      // of drifting charges in silicon
      // For barrel, assume 10 deg. incidence in Rphi, in z compute from 
      // pseudorapidity
      // this may be improved with better parameterization, but it is 
      // probably better to use candidate track information later in 
      // reconstruction. TL
      // Values are made dependent on the sensor thickness to accomodate 
      // different sensors layout. AA
        float deltax = 0;
        float deltay = 0;
        float sensorThickness = element->thickness();
        Amg::Vector3D globalPos = element->globalPosition(centroid);
        InDetDD::SiLocalPosition totCorrection(0,0,0);
        if(pixelID.is_barrel(elementID)){
            deltax = 30*micrometer*(sensorThickness/(250*micrometer));
            deltay = sensorThickness*fabs(globalPos.z())/globalPos.perp();
            if(deltay > (design->etaPitch()) ) deltay = design->etaPitch();
        }
        else{
            deltax = 10*micrometer*sqrt(sensorThickness/(250*micrometer));
            deltay = 10*micrometer*sqrt(sensorThickness/(250*micrometer));
        }
        InDetDD::SiLocalPosition pos1 = design->positionFromColumnRow(colMin,rowMin);
        InDetDD::SiLocalPosition pos2 = design->positionFromColumnRow(colMax,rowMin);
        InDetDD::SiLocalPosition pos3 = design->positionFromColumnRow(colMin,rowMax);
        InDetDD::SiLocalPosition pos4 = design->positionFromColumnRow(colMax,rowMax);
        centroid = 0.25*(pos1+pos2+pos3+pos4)+
        InDetDD::SiLocalPosition(deltay*(etaCol-0.5),deltax*(etaRow-0.5),0.);
        ATH_MSG_VERBOSE("Barrel cluster with global position r= " << globalPos.perp() << " and z = " << globalPos.z());
        ATH_MSG_VERBOSE("deltax = " << deltax << " deltay = " << deltay);

    }
    if(m_posStrategy == 10 && !hasGanged  && etaRow>0 && etaCol > 0){
    // recFlag == 10 (CTB simulation)
    // use parametrization studied on CTB data by I. Reisinger (Dortmund)
        static bool printw=true;
        if (printw) {
            ATH_MSG_ERROR("Detected position strategy = 10, this is an obsolete setting for CTB analysis and is not supported anymore since Athena 15.4.0"); 
            ATH_MSG_ERROR("...reverting to default setting: position strategy = 0");        printw=false;
        }
    } 
    // Endcap SR1 Cosmics
    if(m_posStrategy == 20 && !hasGanged  && etaRow>0 && etaCol > 0){

        ATH_MSG_VERBOSE("Endcap cosmics simulation");
        double deltax = 35*micrometer;
        double deltay = 35*micrometer;
        InDetDD::SiLocalPosition pos1 = 
            design->positionFromColumnRow(colMin,rowMin);
        InDetDD::SiLocalPosition pos2 = 
            design->positionFromColumnRow(colMax,rowMin);
        InDetDD::SiLocalPosition pos3 = 
            design->positionFromColumnRow(colMin,rowMax);
        InDetDD::SiLocalPosition pos4 = 
            design->positionFromColumnRow(colMax,rowMax);
        centroid = 0.25*(pos1+pos2+pos3+pos4)+
            InDetDD::SiLocalPosition(deltay*(etaCol-0.5),
            deltax*(etaRow-0.5),0.);
    }


    Amg::Vector2D position(centroid.xPhi(),centroid.xEta());

    ATH_MSG_VERBOSE("Cluster ID =" << id); 
    ATH_MSG_VERBOSE("Cluster width (eta x phi) = " << colWidth 
        << " x " << rowWidth);
    ATH_MSG_VERBOSE("Cluster width (eta x phi) = " << etaWidth 
        << " x " << phiWidth);
    ATH_MSG_VERBOSE("Cluster local position (eta,phi) = " 
        << (position)[0] << " " 
        << (position)[1]);

    if(!m_clusterMaker){
      PixelCluster* cluster = new PixelCluster(id,position,DVid,lvl1min,totgroup,siWidth,element,0);
        return cluster;
    }
    else{
        ATH_MSG_VERBOSE("Cluster omega old = " << etaRow <<  " " << etaCol);       

        PixelCluster* cluster = m_clusterMaker->pixelCluster(id,
            position,
            DVid,
            lvl1min,
            totgroup,
            siWidth,
            element,
            hasGanged,
            m_errorStrategy,
            pixelID,
            split,
            splitProb1,
            splitProb2);
        return cluster;
    }
}
//--------------------------------------------------------------------------
  // Determines if a pixel cell (whose identifier is the first argument) is 
  // a ganged pixel. If this is the case, the last argument assumes the 
  // value of the identifier of the cell it is ganged with. 
  // The second argument is the pixel module the hit belongs to.


bool MergedPixelsTool::isGanged(const Identifier& rdoID,
           //	     const Identifier& elementID,
           //            const PixelID& pixelID,
    const InDetDD::SiDetectorElement* element,  
    Identifier& gangedID) const
{
    // returns true for ganged pixels. If true returns Identifier of pixel

    InDetDD::SiCellId cellID = element->cellIdFromIdentifier (rdoID);
    if (element->numberOfConnectedCells (cellID) > 1) {
        InDetDD::SiCellId gangedCellID = element->connectedCell (cellID,1);
        gangedID = element->identifierFromCellId (gangedCellID);
        return true;
    } else {
        gangedID = Identifier();
        return false;
    }
} 


void MergedPixelsTool::handle(const Incident& inc) {
    
   
   
    // the cluster ambiguity map
    if ( inc.type() == IncidentType::BeginEvent ){
        // record the Split ambiguity map
        if (!m_splitClusterMap && !m_clusterSplitter.empty() && !m_emulateSplitter){
            m_splitClusterMap = new InDet::PixelGangedClusterAmbiguities;
            if ( evtStore()->record(m_splitClusterMap,m_splitClusterMapName).isFailure()){
                ATH_MSG_WARNING("Could not record split cluster ambiguity map."); 
                delete m_splitClusterMap; m_splitClusterMap = 0;
            } else
                ATH_MSG_VERBOSE("Cluster split ambiguity map recorded as '" << m_splitClusterMapName <<"'.");
        }
    }
   
   
     if ( inc.type() == IncidentType::EndEvent ){
         ATH_MSG_VERBOSE("'EndEvent' incident caught. Refreshing Cache.");
         m_splitClusterMap = 0;
     }     
}


//-----------------------------------------------------------------------
  // Checks if two RDO lists (would be clusters) whould be merged, 
  // which do happen if there is a pair of pixel cells belonging to the 
  // two different groups which have a side in common

void MergedPixelsTool::checkForMerge(const Identifier& id,
    MergedPixelsTool::RDO_GroupVector::iterator  baseGroup,
    MergedPixelsTool::RDO_GroupVector::iterator lastGroup,
    MergedPixelsTool::TOT_GroupVector::iterator totGroup,
    MergedPixelsTool::TOT_GroupVector::iterator lvl1Group,
    InDetDD::SiDetectorElement* element,
    const PixelID& pixelID) const
{
    // Look at each of groups that haven't already been checked to see if
    // RDO identifier is in any of them. If so, copy vector to base group and 
    // the vector that has been copied.

    typedef RDO_Vector::const_iterator idIterator;
    typedef TOT_Vector::const_iterator totIterator;
    MergedPixelsTool::RDO_GroupVector::const_iterator nextGroup(baseGroup+1);
    MergedPixelsTool::TOT_GroupVector::const_iterator nextTotGroup(totGroup+1);
    MergedPixelsTool::TOT_GroupVector::const_iterator nextLvl1Group(lvl1Group+1);

    for (; nextGroup!= lastGroup; ++nextGroup)
    {
        if (areNeighbours(**nextGroup, id, element, pixelID))
        {
      // merge the RDO identifier groups
            idIterator firstID = (*nextGroup)->begin();
            idIterator lastID = (*nextGroup)->end();
            for (; firstID != lastID; ++firstID)
            {

                (*baseGroup)->push_back(*firstID);
            }
            (*nextGroup)->clear();
        // merge the tot vectors
            totIterator firstTot = (*nextTotGroup)->begin();
            totIterator lastTot = (*nextTotGroup)->end();
            for (; firstTot != lastTot; ++firstTot)
            {
                (*totGroup)->push_back(*firstTot);
            }
            (*nextTotGroup)->clear();
        // merge the tot vectors
            totIterator firstLvl1 = (*nextLvl1Group)->begin();
            totIterator lastLvl1 = (*nextLvl1Group)->end();
            for (; firstLvl1 != lastLvl1; ++firstLvl1)
            {
                (*lvl1Group)->push_back(*firstLvl1);
            }
            (*nextLvl1Group)->clear();
        }
        nextTotGroup++;
        nextLvl1Group++;
    }
}

}

//----------------------------------------------------------------------------
