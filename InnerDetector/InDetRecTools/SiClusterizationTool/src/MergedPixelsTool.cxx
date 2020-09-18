/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MergedPixelsTool.cxx
// TheSiSimpleClusteringAlg adapted to implement cluster merging
// input from RDOs. 
//
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/MergedPixelsTool.h"

#include "Identifier/IdentifierHash.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetIdentifier/PixelID.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include <unordered_set>

using CLHEP::micrometer;

namespace InDet {
  
  // Constructor with parameters:
  MergedPixelsTool::MergedPixelsTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent) :
                                     PixelClusteringToolBase(type,name,parent),
                                     m_IBLParameterSvc("IBLParameterSvc",name)
    {
      declareInterface<IPixelClusteringTool>(this);
    }
  
  StatusCode  MergedPixelsTool::initialize(){
    if (m_IBLParameterSvc.retrieve().isFailure()) { 
      ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc"); 
    } else { 
      m_IBLParameterSvc->setBoolParameters(m_IBLAbsent,"IBLAbsent");
    }  
    ATH_CHECK(m_pixelDetEleCollKey.initialize());
    return PixelClusteringToolBase::initialize();
  }


  StatusCode MergedPixelsTool::finalize()
  {
   
    ATH_MSG_DEBUG("------------------- Clusterization Statistics ------------------------");
    ATH_MSG_DEBUG("-- # Processed Pixel Clusters     : " << m_processedClusters);
    ATH_MSG_DEBUG("----------------------------------------------------------------------");
    return StatusCode::SUCCESS;
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
  
  PixelCluster* MergedPixelsTool::makeCluster (const std::vector<Identifier>& group,
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
      const Identifier elementID = element->identify();
      const InDetDD::PixelModuleDesign* design
          (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
      if (not design){
        ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of MergedPixelsTool.");
        return nullptr;
      }
      int rowMin = int(2*(design->width()/design->phiPitch()))+1;
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
  
      // Flag to tag clusters with any ganged pixel
      bool hasGanged = false;  
      for (; rdosBegin!= rdosEnd; ++rdosBegin) {
        // compute cluster lvl1
        if ( (*lvl1) < lvl1min ) lvl1min=(*lvl1);
        lvl1++;
        // process identifier
        const Identifier rId =  *rdosBegin;
        const int row = pixelID.phi_index(rId);
        const int col = pixelID.eta_index(rId);
        // flag if cluster contains at least a ganged pixel
        hasGanged = hasGanged || isGanged(rId, element, gangedID);
        DVid.push_back(rId);
        InDetDD::SiLocalPosition siLocalPosition
        (design->positionFromColumnRow(col,row)); 
        sumOfPositions += siLocalPosition;
        
        // check overflow for IBL
        int realtot = *tot;
        
        if (row == rowMin) qRowMin += realtot;
        if (row < rowMin) { 
          rowMin = row; 
          qRowMin = realtot;
        }
        
        if (row == rowMax) qRowMax += realtot;
        if (row > rowMax) {
          rowMax = row;
          qRowMax = realtot;
        }
        
        if (col == colMin) qColMin += realtot;
        if (col < colMin) {
          colMin = col;
          qColMin = realtot;
        }
        
        if (col == colMax) qColMax += realtot;
        if (col > colMax) {
          colMax = col;
          qColMax = realtot;
        }
        tot++;
      }
      
      const int numberOfPixels = group.size();
      InDetDD::SiLocalPosition centroid(sumOfPositions/numberOfPixels);
      const Identifier id = element->identifierOfPosition(centroid);
  
      const int colWidth = colMax-colMin+1;
      const int rowWidth = rowMax-rowMin+1;
  
      // Compute eta for charge interpolation correction (if required)
      // Two pixels may have tot=0 (very rarely, hopefully)
      float etaRow = -1;
      float etaCol = -1;
      if(qRowMin+qRowMax > 0) etaRow = qRowMax/float(qRowMin+qRowMax);
      if(qColMin+qColMax > 0) etaCol = qColMax/float(qColMin+qColMax);
  
      double etaWidth = design->widthFromColumnRange(colMin, colMax);
      double phiWidth = design->widthFromRowRange(rowMin, rowMax);
      SiWidth siWidth(Amg::Vector2D(rowWidth,colWidth), Amg::Vector2D(phiWidth,etaWidth) );
  
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
        const float sensorThickness = element->thickness();
        Amg::Vector3D globalPos = element->globalPosition(centroid);
        InDetDD::SiLocalPosition totCorrection(0,0,0);
        if(pixelID.is_barrel(elementID)) {
          deltax = 30*micrometer*(sensorThickness/(250*micrometer));
          deltay = sensorThickness*std::abs(globalPos.z())/globalPos.perp();
          if(deltay > (design->etaPitch()) ) deltay = design->etaPitch();
        } else {
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
        if (m_printw) {
          ATH_MSG_ERROR("Detected position strategy = 10, this is an obsolete setting for CTB analysis and is not supported anymore since Athena 15.4.0"); 
          ATH_MSG_ERROR("...reverting to default setting: position strategy = 0");
          m_printw=false;
        }
      }
      // Endcap SR1 Cosmics
      if(m_posStrategy == 20 && !hasGanged  && etaRow>0 && etaCol > 0){
        ATH_MSG_VERBOSE("Endcap cosmics simulation");
        const double deltax = 35*micrometer;
        const double deltay = 35*micrometer;
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
      } else {
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
  

  //-----------------------------------------------------------------------
  // Runs for every pixel module (with non-empty RDO collection...). 
  // It clusters together the RDOs with a pixell cell side in common
  // using connected component analysis based on four-cell 
  // or eight-cell (if m_addCorners == true) connectivity
  PixelClusterCollection*  MergedPixelsTool::clusterize(const InDetRawDataCollection<PixelRDORawData> &collection,
                                                        const PixelID& pixelID) const {
    
    // Size of RDO's collection:
    const unsigned int RDO_size = collection.size();
    if ( RDO_size==0) {
        // Empty RDO collection
        ATH_MSG_DEBUG (" areNeighbours - problems ");
        return nullptr;
    }
    
    // Get Identifier and IdentifierHash for these RDOs
    const Identifier elementID = collection.identify();
    const IdentifierHash idHash = collection.identifyHash();
    
    // If module is bad, do not create a cluster collection
    if (m_useModuleMap && !(m_summaryTool->isGood(idHash))) 
      return nullptr;
    
    // Get detector info.
    // Find detector element for these RDOs

    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
    const InDetDD::SiDetectorElementCollection* pixelDetEle(*pixelDetEleHandle);
    if (not pixelDetEleHandle.isValid() or pixelDetEle==nullptr) {
      ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " is not available.");
      return nullptr;
    }
    const InDetDD::SiDetectorElement* element = pixelDetEle->getDetectorElement(idHash);
    
    const Trk::RectangleBounds *mybounds=dynamic_cast<const Trk::RectangleBounds *>(&element->surface().bounds());
    if (not mybounds) {
      ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of MergedPixelsTool.cxx.");
      return nullptr;
    }
    
    // loop on the rdo collection and save the relevant quantities for each fired pixel
    // rowcolID contains: number of connected pixels, phi/eta pixel indices, tot, lvl1, rdo identifier
    std::vector<rowcolID> collectionID;
    std::unordered_set<Identifier> setOfIdentifiers{};
    for(const auto & rdo : collection) {
      const Identifier rdoID= rdo->identify();
      if (m_usePixelMap and !(m_summaryTool->isGood(idHash,rdoID))) continue;
      //check for duplication:
      //add to set of existing identifiers. If it fails (.second = false) then skip it.
      if (not setOfIdentifiers.insert(rdoID).second)   continue;
      // note that we don't understand, for now, why these duplicated RDO's occur
      //
      const int lvl1= rdo->getLVL1A();      
      if (m_checkDuplicatedRDO and checkDuplication(pixelID, rdoID, lvl1, collectionID)) continue;
      
      const int tot = rdo->getToT();
      
      rowcolID   RCI(-1, pixelID.phi_index(rdoID), pixelID.eta_index(rdoID), tot, lvl1, rdoID); 
      collectionID.push_back(RCI);
      // check if this is a ganged pixel    
      Identifier gangedID;
      const bool ganged = isGanged(rdoID, element, gangedID);  
      
      if (not ganged) continue;
          
      // if it is a ganged pixel, add its ganged RDO id to the collection
      rowcolID   RCI_ganged(-1, pixelID.phi_index(gangedID), pixelID.eta_index(gangedID), tot, lvl1, gangedID);
      collectionID.push_back(RCI_ganged);      
    }
    
    // Sort pixels in ascending columns order
    // 
    if(collectionID.empty()) return 0;    
    if(collectionID.size() > 1) std::sort(collectionID.begin(),collectionID.end(),pixel_less);
    
    // initialize the networks
    //
    std::vector<network> connections(collectionID.size());

    // Network production
    //
    int collectionSize = collectionID.size();    
    // the maximum number of elements to save can be either 2 or 3
    // m_addCorners == true requires saving the bottom (or top), the side and the corner connection for each pixel,
    // otherwise you only save bottom (or top) and the side
    int maxElements = m_addCorners ? 3 : 2;

    // start looping on the pixels
    // for each pixel you build the network connection accordingly to the 4- or 8-cells connections
    for(int currentPixel = 0; currentPixel!=collectionSize-1; ++currentPixel) {
      int NB  = 0;
      int row = collectionID.at(currentPixel).ROW;
      int col = collectionID.at(currentPixel).COL; 
      //
      auto & currentConnection = connections.at(currentPixel);
      for(int otherPixel = currentPixel+1; otherPixel!=collectionSize; ++otherPixel) {
        auto & otherConnection = connections.at(otherPixel);
        int deltaCol = std::abs(collectionID.at(otherPixel).COL - col);
        int deltaRow = std::abs(collectionID.at(otherPixel).ROW - row);
        // break if you are too far way in columns, as these ones will be taken in the next iterations
        if( deltaCol > 1) {
          break;
        }
        // if you need the corners, you jump the next rows, as these ones will be taken in the next iterations
        if ( m_addCorners and deltaRow > 1 ) {
          continue;
        }
        
        // Two default cases are considered:
        // 1) top/bottom connection (deltaCol=1 and deltaRow=0)
        // 2) side connection (deltaCol=0 and deltaRow=1)
        // In both cases the satisfied condition is:
        // deltaRow+deltaCol = 1
        // 
        // As an optional case (true by defaul) we save also add a corner connection:
        // 3) corner connection (deltaCol=1 and deltaRow=1)
        
        // this builds the single pixel connection and breaks if the max number of elements is reached:        
        if( (deltaCol+deltaRow) == 1 or (m_addCorners and deltaCol == 1 and deltaRow == 1) ) {
          try{
            currentConnection.CON.at(currentConnection.NC++) = otherPixel;
            otherConnection.CON.at(otherConnection.NC++) = currentPixel ;
          } catch (const std::out_of_range & ){
            throw std::runtime_error("attempt to access connection array beyond its size in MergedPixelsTool::clusterize");
          }
          if(++NB==maxElements) {
            break;
          }
        }
      }
    }

    // Pixels clusterization
    //
    // Once the connections are built, the pixel clusterisation can start grouping together pixels
    int Ncluster = 0;
    for(int currentPixel=0; currentPixel!=collectionSize; ++currentPixel) {
      if(collectionID.at(currentPixel).NCL < 0) {
        collectionID.at(currentPixel).NCL = Ncluster;
        addClusterNumber(currentPixel,Ncluster,connections,collectionID);
        ++Ncluster;
      }
    }

    // Clusters sort in Ncluster order
    //
    if(--collectionSize > 1) {
      for(int i(1); i<collectionSize; ++i ) {
        rowcolID U  = collectionID.at(i+1);
        
        int j(i);
        while(collectionID.at(j).NCL > U.NCL) {
          collectionID.at(j+1)=collectionID.at(j); 
          --j;
        }
        collectionID.at(j+1)=U;
      }
    }

    // Make a new pixel cluster collection
    //
    PixelClusterCollection  *clusterCollection = new PixelClusterCollection(idHash);
    clusterCollection->setIdentifier(elementID);
    clusterCollection->reserve(Ncluster);

    std::vector<Identifier> DVid = {collectionID.at(0).ID };
    std::vector<int>        Totg = {collectionID.at(0).TOT};
    std::vector<int>        Lvl1 = {collectionID.at(0).LVL1};
    
    int clusterNumber = 0;
    int NCL0          = 0;
    
    DVid.reserve(collectionID.back().NCL);
    Totg.reserve(collectionID.back().NCL);
    Lvl1.reserve(collectionID.back().NCL);

    ++collectionSize;    
    for(int i=1; i<=collectionSize; ++i) {

      if(i!=collectionSize and collectionID.at(i).NCL==NCL0) {
        DVid.push_back(collectionID.at(i).ID );
        Totg.push_back(collectionID.at(i).TOT);
        Lvl1.push_back(collectionID.at(i).LVL1);
      
      } else {
        
        // Cluster production
        ++m_processedClusters;
        PixelCluster* cluster = makeCluster(DVid,
                                            Totg,
                                            Lvl1,
                                            element,
                                            pixelID, 
                                            ++clusterNumber);
        
        // no merging has been done;
        if (cluster) { 
          // statistics output
          cluster->setHashAndIndex(clusterCollection->identifyHash(), clusterCollection->size());
          clusterCollection->push_back(cluster);
        }      
        
        
        // Preparation for next cluster
        if (i!=collectionSize) {
          NCL0   = collectionID.at(i).NCL                     ;
          DVid.clear(); DVid = {collectionID.at(i).ID };
          Totg.clear(); Totg = {collectionID.at(i).TOT};
          Lvl1.clear(); Lvl1 = {collectionID.at(i).LVL1};
        }
      }
    }

    return clusterCollection;
  }
  
  void MergedPixelsTool::addClusterNumber(const int& r, 
                                          const int& Ncluster,
                                          const std::vector<network>& connections,                                         
                                          std::vector<rowcolID>& collectionID) const {
    for(int i=0; i!=connections.at(r).NC; ++i) {  
      const int k = connections.at(r).CON.at(i);
      if(collectionID.at(k).NCL < 0) {
        collectionID.at(k).NCL = Ncluster;
        addClusterNumber(k, Ncluster, connections, collectionID);
      }
    }
  }
  
  
  bool MergedPixelsTool::checkDuplication(const PixelID& pixelID,
                                          const Identifier& rdoID, 
                                          const int& lvl1, 
                                          std::vector<rowcolID>& collectionID) const {
    
    // check if duplicates are found. If that is the case, update the LVL1 
    auto isDuplicate=[pixelID,rdoID](const rowcolID& rc) -> bool {
      return std::make_tuple(pixelID.phi_index(rdoID), pixelID.eta_index(rdoID)) == std::make_tuple(pixelID.phi_index(rc.ID), pixelID.eta_index(rc.ID));
    }; 
    
    const auto pDuplicate = std::find_if(collectionID.begin(), collectionID.end(),isDuplicate); 
    const bool foundDuplicate{pDuplicate != collectionID.end()}; 
    if (foundDuplicate) pDuplicate->LVL1 = std::max(pDuplicate->LVL1, lvl1); 
    return foundDuplicate;
  }                                            
}
//----------------------------------------------------------------------------
