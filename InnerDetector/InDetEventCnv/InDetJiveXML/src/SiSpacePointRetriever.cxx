/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetJiveXML/SiSpacePointRetriever.h"
#include "StoreGate/DataHandle.h"
#include "JiveXML/DataType.h"

#include "DataModel/DataVector.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandle.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"
#include "TrkPrepRawData/PrepRawData.h"

namespace JiveXML 
{

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  SiSpacePointRetriever::SiSpacePointRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("S3D"),
    m_geo("JiveXML::InDetGeoModelTool/InDetGeoModelTool",this){

    //Declare the interface
    declareInterface<IDataRetriever>(this);
    
    //And the properties
    declareProperty("PixelSpacePoints"     , m_PixelSPContainerName = std::string("PixelSpacePoints"));
    declareProperty("SCTSpacePoints"       , m_SCTSPContainerName = std::string("SCT_SpacePoints"));
    declareProperty("PRD_TruthPixel"       , m_PixelPRDTruthName = std::string("PRD_MultiTruthPixel")); 
    declareProperty("PRD_TruthSCT"         , m_SCTPRDTruthName = std::string("PRD_MultiTruthSCT"));   
  }

  //Namespace for the helper functions
  namespace SiSpacePointRetrieverHelpers {
    
    /**
     * Get the barcodes of associated truth particles for a SpacePoint ( we require both clusters, in case there are)
     * @param idFirst the SpacePoint identifier of first cluster
     * @param idSecond the SpacePoint identifier of second cluster (only for SCT)
     * @param truthColl the truth collection
     * @param barcodes the date vector to which to append the barcodes
     * @return number of associated truth particles
     */
    unsigned int getTruthBarcodes( const Identifier idFirst, const Identifier idSecond, const PRD_MultiTruthCollection* truthColl, DataVect& barcodes) {
      
      //Make life easier
      typedef PRD_MultiTruthCollection::const_iterator  PRDTruthIter;


      /**
       * NOTE: This function could be simplified if we could directly insert
       * into the DataVect, instead of barcodesCommon. At the moment, this
       * doesnt work as we are lacking operator< and operator== for DataType
       */

      //Sets of barcodes associated with first, second and both cluster
      std::set<int> barcodesFirst;
      std::set<int> barcodesSecond;
      std::set<int> barcodesCommon;

      //Get the set of particle barcodes associated with the first cluster identifier
      std::pair<PRDTruthIter,PRDTruthIter> equalRangeFirst = truthColl->equal_range(idFirst);
      for(PRDTruthIter TruthCollItr=equalRangeFirst.first; TruthCollItr!= equalRangeFirst.second; TruthCollItr++)
        barcodesFirst.insert(TruthCollItr->second.barcode());

      //Check if we have only have one valid cluster identifier
      if (! idSecond.is_valid()){

        //Make this our list of barcodes (swap is O(1))
        barcodesCommon.swap(barcodesFirst);

      //otherwise only store barcodes associated with both identifiers
      } else {

        //Get the set of particle barcodes associated with the second cluster identifier
        std::pair<PRDTruthIter,PRDTruthIter> equalRangeSecond = truthColl->equal_range(idSecond);
        for(PRDTruthIter TruthCollItr=equalRangeSecond.first; TruthCollItr!= equalRangeSecond.second; TruthCollItr++)
          barcodesSecond.insert(TruthCollItr->second.barcode());

        //Copy the list of particle barcodes that are associated with both clusters
        std::set_intersection(barcodesFirst.begin(), barcodesFirst.end(), barcodesSecond.begin(), barcodesSecond.end(), 
                              std::insert_iterator< std::set<int> >(barcodesCommon,barcodesCommon.begin()) );
      }

      //Finally add the list of barcodes to our DataVect
      std::set<int>::const_iterator barcodeItr = barcodesCommon.begin();
      for (; barcodeItr != barcodesCommon.end(); ++barcodeItr) barcodes.push_back(DataType(*barcodeItr)); 

      //return the number of added barcodes
      return barcodesCommon.size();
    }

  }//helpers namespace

  /**
   * Implementation of DataRetriever interface
   * @param FormatTool the tool that will create formated output from the DataMap
   **/
  StatusCode SiSpacePointRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() <<endmsg; 
   
    /**
     * Try to retrieve all the relevant collections first
     */
    SG::ReadHandle<PRD_MultiTruthCollection> PixelPRDTruthColl(m_PixelPRDTruthName);
    SG::ReadHandle<SpacePointContainer> PixelSPContainer(m_PixelSPContainerName);
    SG::ReadHandle<PRD_MultiTruthCollection> SCTPRDTruthColl(m_SCTPRDTruthName);
    SG::ReadHandle<SpacePointContainer> SCTSPContainer(m_SCTSPContainerName);

    //Try to retrieve all four from store gate
    if ( not PixelSPContainer.isValid() )
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Unable to retrieve SpacePoint container with name " << m_PixelSPContainerName.key() << endmsg;
    if ( not PixelPRDTruthColl.isValid() )
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Unable to retrieve PRD_MultiTruth collection with name " << m_PixelPRDTruthName.key() << endmsg;
    if ( not SCTSPContainer.isValid() )
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Unable to retrieve SpacePoint container with name " << m_SCTSPContainerName.key() << endmsg;
    if ( not SCTPRDTruthColl.isValid() )        
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Unable to retrieve PRD_MultiTruth collection with name " << m_SCTPRDTruthName.key() << endmsg;

    /**
     * Now make a list of SpacePoint - PRDTruth collection pairs to run over
     */
    typedef std::pair< const SpacePointContainer*, const PRD_MultiTruthCollection* > SpacePointTruthPair;
    std::vector<SpacePointTruthPair> SpacePointTruthPairList;

    //Add Pixel if there is a collection
    if (PixelSPContainer.isValid())
      SpacePointTruthPairList.push_back(SpacePointTruthPair(PixelSPContainer.cptr(),PixelPRDTruthColl.cptr()));
    
    //Add SCT if there is a collection
    if (SCTSPContainer.isValid())
      SpacePointTruthPairList.push_back(SpacePointTruthPair(SCTSPContainer.cptr(),SCTPRDTruthColl.cptr()));
    
    /**
     * Found out how much space we will need
     */
    int NSpacePoints = 0;
    //Loop over all SpacePoint - PRDTruth pairs
    std::vector<SpacePointTruthPair>::iterator SpacePointTruthPairItr = SpacePointTruthPairList.begin();
    for ( ; SpacePointTruthPairItr != SpacePointTruthPairList.end() ; ++SpacePointTruthPairItr ){

      //Get an iterator over the SpacePoint container itself  
      SpacePointContainer::const_iterator SpacePointCollItr = (*SpacePointTruthPairItr).first->begin();

      //Add up the size of the SpacePoint collections in the container
      for (; SpacePointCollItr!=  (*SpacePointTruthPairItr).first->end(); ++SpacePointCollItr)
        NSpacePoints += (**SpacePointCollItr).size();
    }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<  "Counted  " << NSpacePoints << " in total" << endmsg;

    /**
     * Declare all the data we want to retrieve
     */
    DataVect x; x.reserve(NSpacePoints);
    DataVect y; y.reserve(NSpacePoints);
    DataVect z; z.reserve(NSpacePoints);
    DataVect clusters; clusters.reserve(NSpacePoints*2);
    DataVect phiModule; phiModule.reserve(NSpacePoints);
    DataVect etaModule; etaModule.reserve(NSpacePoints);
    DataVect numBarcodes; numBarcodes.reserve(NSpacePoints);
    DataVect barcodes; barcodes.reserve(NSpacePoints); // Usually less then one per space point

    /**
     * Now fill in all the data
     */

    //Loop over all SpacePoint - PRDTruth pairs
    SpacePointTruthPairItr = SpacePointTruthPairList.begin();
    for ( ; SpacePointTruthPairItr != SpacePointTruthPairList.end(); ++SpacePointTruthPairItr ){

      // Loop over SpacePoint Collections in the SpacePoint container
      SpacePointContainer::const_iterator SpacePointCollItr=(*SpacePointTruthPairItr).first->begin();
      for (;SpacePointCollItr!=(*SpacePointTruthPairItr).first->end();++SpacePointCollItr){

        const SpacePointCollection* SpacePointColl=(*SpacePointCollItr);
        
        //Loop over SpacePoints themselves
        DataVector<Trk::SpacePoint>::const_iterator SpacePointItr = SpacePointColl->begin() ;
        for ( ; SpacePointItr != SpacePointColl->end(); ++SpacePointItr) {
          
          //Get the position of the space point
          const Trk::SpacePoint* SpacePoint = (*SpacePointItr);
	  Amg::Vector3D point = SpacePoint->globalPosition();
          
          //Store position in units of centimeters
          x.push_back(DataType(point.x() * CLHEP::mm/CLHEP::cm));
          y.push_back(DataType(point.y() * CLHEP::mm/CLHEP::cm));
          z.push_back(DataType(point.z() * CLHEP::mm/CLHEP::cm));
          
          //Get the cluster list for the Space point (first and second)
          const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*> clusterList = SpacePoint->clusterList();

          //Get the identifiers of the first and second cluster 
          Identifier idFirst = clusterList.first->identify();
          Identifier idSecond = (clusterList.second != NULL) ? clusterList.second->identify() : Identifier();

          //Get phi and eta of the module in detector coordinates of the first cluster
          phiModule.push_back(DataType(m_geo->SCTIDHelper()->phi_module(idFirst)));
          etaModule.push_back(DataType(m_geo->SCTIDHelper()->eta_module(idFirst)));

          // Store the cluster(s) identifier (pair)
          clusters.push_back(DataType(idFirst.get_compact()));   
          clusters.push_back((idSecond.is_valid()) ? DataType(idSecond.get_compact()) : DataType(-1));

          //Stop here if there is no truth
          const PRD_MultiTruthCollection* PRDTruthColl = (*SpacePointTruthPairItr).second;
          if ( PRDTruthColl == NULL ) continue ;

          // Finally get barcodes of associated truth particles
          numBarcodes.push_back(SiSpacePointRetrieverHelpers::getTruthBarcodes(idFirst, idSecond, PRDTruthColl, barcodes));

          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Found " << numBarcodes.back() << " common barcodes, now " 
                                                      << barcodes.size() << " in total" << endmsg;

        } // loop over SpacePoint collection
      } // loop over SpacePoint container
    } //loop over SpacePoint - TruthMap collection pairs

    //Now put together the DataMap
    DataMap dataMap;
    dataMap["x"] = x;
    dataMap["y"] = y;
    dataMap["z"] = z;
    dataMap["clusters multiple=\"2\""] = clusters;
    dataMap["phiModule"] = phiModule;
    dataMap["etaModule"] = etaModule;

    //Only store truth associations if we retrieved them
    if ( numBarcodes.size() > 0 ){
      //Add barcodes counter
      dataMap["numBarcodes"] = numBarcodes;
      // Compute the "multiple" and put the barcodes vector in the map.
      std::string bctag = "barcodes multiple=\""+DataType(barcodes.size()/double(numBarcodes.size())).toString()+"\"";
      dataMap[bctag] = barcodes;
    }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< x.size() << endmsg;

     //forward data to formating tool and return
    return FormatTool->AddToEvent(dataTypeName(), "", &dataMap);
  }

  StatusCode SiSpacePointRetriever::initialize() {
    // Read Handle Key
    ATH_CHECK(m_PixelSPContainerName.initialize());
    ATH_CHECK(m_SCTSPContainerName.initialize());
    ATH_CHECK(m_PixelPRDTruthName.initialize());
    ATH_CHECK(m_SCTPRDTruthName.initialize());

    return m_geo.retrieve();
  }
  
} //namespace

