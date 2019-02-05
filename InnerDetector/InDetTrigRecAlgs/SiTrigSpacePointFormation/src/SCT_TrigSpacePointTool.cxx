/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_TrigSpacePointTool.h"

// For processing clusters
#include "InDetReadoutGeometry/SiLocalPosition.h" 
#include "InDetReadoutGeometry/SiDetectorDesign.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h" 
#include "InDetReadoutGeometry/SiDetectorElementCollection.h" 
#include "SiSpacePointFormation/SiElementPropertiesTable.h"

// Space point Classes,
#include "TrkSpacePoint/SpacePoint.h" 
#include "TrkSpacePoint/SpacePointCollection.h" 
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 
#include "InDetIdentifier/SCT_ID.h"

#include "EventPrimitives/EventPrimitives.h"


namespace InDet {


// Constructor with parameters:
SCT_TrigSpacePointTool::SCT_TrigSpacePointTool(const std::string &type,
					       const std::string &name,
					       const IInterface *parent) :
  AthAlgTool(type,name,parent),
  m_overlapLimitOpposite(2.8),// overlap limit for opposite-neighbour.
  m_overlapLimitPhi(5.64),      // overlap limit for phi-neighbours.
  m_overlapLimitEtaMin(1.68),   // low overlap limit for eta-neighbours.
  m_overlapLimitEtaMax(3.0),   // high overlap limit for eta-neighbours.
  m_spacePointsOverlapName("OverlapSpacePoints"),
  m_SiSpacePointMakerToolName("InDet::SiSpacePointMakerTool"),
  m_idHelper(0),
  m_Sct_clcontainer{nullptr},
  m_SiSpacePointMakerTool(0),
  m_allClusters(false),       // process all clusters without limits.
  m_overlap(false),           // process overlaps of SCT wafers.
  m_overrideBS(false),
  m_xVertex(0.),
  m_yVertex(0.),
  m_zVertex(0.),
  m_spacepointoverlapCollection(0)
{
  declareInterface<InDet::ITrigSCT_SpacePointTool>( this );

  declareProperty("OverlapLimitOpposite", m_overlapLimitOpposite);// overlap limit for 
                                              // opposite-neighbour.
  declareProperty("OverlapLimitPhi", m_overlapLimitPhi);// overlap limit for phi-neighbours.
  declareProperty("OverlapLimitEtaMin", m_overlapLimitEtaMin); // low overlap limit for
                                             // eta-neighbours.
  declareProperty("OverlapLimitEtaMax", m_overlapLimitEtaMax);// high overlap limit for 
                                           // eta-neighbours.
  declareProperty("SpacePointsOverlapName", m_spacePointsOverlapName);
  declareProperty("SiSpacePointMakerToolName", m_SiSpacePointMakerToolName);
 

 declareProperty("ProcessOverlaps", m_overlap);// process overlaps of SCT wafers.
 declareProperty("AllClusters",  m_allClusters); // process all clusters without limits
 declareProperty("OverrideBeamSpot", m_overrideBS);
 declareProperty("VertexX", m_xVertex);
 declareProperty("VertexY", m_yVertex);
 declareProperty("VertexZ", m_zVertex);
}

//--------------------------------------------------------------------------
StatusCode SCT_TrigSpacePointTool::initialize()  {
  ATH_CHECK( AthAlgTool::initialize() );
  ATH_CHECK( detStore()->retrieve(m_idHelper, "SCT_ID") );

  // Make a table of neighbours and widths of side 1 SCT wafers
  
  ATH_CHECK( toolSvc()->retrieveTool(m_SiSpacePointMakerToolName, 
                                     m_SiSpacePointMakerTool, this) );

  if (!m_overrideBS){
    ATH_CHECK( m_beamSpotKey.initialize() );
    ATH_MSG_INFO( "Retrieved beam spot Cond Key " << m_beamSpotKey );
  }

  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------------
StatusCode SCT_TrigSpacePointTool::finalize() {
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}


//--------------------------------------------------------------------------

void SCT_TrigSpacePointTool::
addSCT_SpacePoints(const SCT_ClusterCollection* clusCollection, 
                   const SCT_ClusterContainer* clusterContainer,
                   const SiElementPropertiesTable* properties,
                   const InDetDD::SiDetectorElementCollection* elements,
                   SpacePointCollection* spacepointCollection,
                   SpacePointOverlapCollection* overlapColl) {

  m_Sct_clcontainer = clusterContainer;
  m_spacepointoverlapCollection = overlapColl;

  if(!m_Sct_clcontainer || m_Sct_clcontainer->size() <1){
    msg(MSG::ERROR) << " Received an invalid cluster container " 
	<< endmsg;
    return;
  }

  if(!clusCollection){
    msg(MSG::ERROR) << " Received an invalid cluster collection " 
	<< endmsg;
    return;
  }
  if(clusCollection->size()==0){
    msg(MSG::ERROR) << " Received empty cluster collection" << endmsg;
    return;
  }

  // Do nothing unless this is a side 1 detector (strips of const phi).
  IdentifierHash thisHash(clusCollection->identifyHash());
  Identifier thisID(clusCollection->identify());

  // if it is not the stereo side
  const InDetDD::SiDetectorElement *element = 
    elements->getDetectorElement(thisHash);

  if (element && !(element->isStereo())){
    //if (m_idHelper->side(thisID)==1) {
    // Space points are created from clusters for all possibly 
    // overlapping pairs in the given range if m_overlap=true. 
    // Otherwise just the opposite pair are processed. 
    // Pick up the five neighbours of detector, and check ranges 
    // for which overlap is possible. 
    // "check1" is used for opposite and eta overlaps.
    // check2 for phi overlaps

    const std::vector<IdentifierHash>* 
      others(properties->neighbours(thisHash));
    if (others==0 || others->empty() ) return;
    std::vector<IdentifierHash>::const_iterator otherHash = others->begin();
    
    bool doOverlapColl = false;
    // check opposite wafer
    checkForSCT_Points(clusCollection, *otherHash, 
                       elements,
		       -m_overlapLimitOpposite, +m_overlapLimitOpposite,
		       spacepointCollection,doOverlapColl);
    

    if (!m_overlap || !m_spacepointoverlapCollection) return; 

    //
    //    m_spacepointoverlapCollection->clear();

    
    // check phi overlaps
    doOverlapColl = true;
    ++otherHash;
    if (otherHash == others->end() ) return;
    float hwidth(properties->halfWidth(thisHash));
    // half-width of wafer
    
    checkForSCT_Points(clusCollection, *otherHash, 
                       elements,
		       -hwidth, -hwidth+m_overlapLimitPhi, 
		       +hwidth-m_overlapLimitPhi, +hwidth);
    ++otherHash;
    if (otherHash == others->end() )  return;
    checkForSCT_Points(clusCollection, *otherHash, 
                       elements,
		       +hwidth-m_overlapLimitPhi, +hwidth,
		       -hwidth, -hwidth+m_overlapLimitPhi);
    
    // if barrel, check the eta overlaps too.
    // In this case, action depends on whether layer is even or odd
    // Also, whether we look at "lower in eta", which comes first,
    // or "higher".
    ++otherHash;
    if (otherHash == others->end() )  return;
    if (m_idHelper->barrel_ec(thisID)==0)
      {	
	if (m_idHelper->layer_disk(thisID)==0 || 
	    m_idHelper->layer_disk(thisID)==2)
	  {
	    checkForSCT_Points(clusCollection, *otherHash, 
                               elements,
			       +m_overlapLimitEtaMin, 
			       +m_overlapLimitEtaMax,
			       spacepointCollection,doOverlapColl);
	    ++otherHash;
	    if (otherHash == others->end() )  return;
	    
	    checkForSCT_Points(clusCollection, *otherHash, 
                               elements,
			       -m_overlapLimitEtaMax, 
			       -m_overlapLimitEtaMin,
			       spacepointCollection,doOverlapColl);
	  }else{
	    checkForSCT_Points(clusCollection, *otherHash, 
                               elements,
			       -m_overlapLimitEtaMax, 
			       -m_overlapLimitEtaMin,
			       spacepointCollection,doOverlapColl);
	    ++otherHash;
	    if (otherHash == others->end() )  return;
	     
	    checkForSCT_Points(clusCollection, *otherHash, 
                               elements,
			       +m_overlapLimitEtaMin,
			       +m_overlapLimitEtaMax,
			       spacepointCollection,doOverlapColl);
	  }
      }
  }

}


//--------------------------------------------------------------------------

void SCT_TrigSpacePointTool::
checkForSCT_Points(const SCT_ClusterCollection* clusters1,
		   const IdentifierHash& id2,
                   const InDetDD::SiDetectorElementCollection* elements,
                   double min, double max,
		   SpacePointCollection* spacepointCollection, 
		   bool overlapColl) {

  // Get the cluster collections for these two detectors.
  // Require that (xPhi2 - xPhi1) lie in the range specified.
  // Used for opposite and eta modules
  
  SCT_ClusterContainer::const_iterator
    it(m_Sct_clcontainer->indexFind(id2));
  if (it==m_Sct_clcontainer->end()) return;

  const SCT_ClusterCollection * clusters2 = *it;
 
  Amg::Vector3D beampos;
  if (!m_overrideBS){
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle(m_beamSpotKey);
    beampos = beamSpotHandle->beamPos();
  } else {
    Amg::Vector3D ovtx(m_xVertex,m_yVertex,m_zVertex); 
    beampos = ovtx;
  }

  if (!overlapColl) {
    m_SiSpacePointMakerTool->
      fillSCT_SpacePointCollection(clusters1, clusters2,min, max, 
				   m_allClusters, beampos, elements,
				   spacepointCollection);
  }

  else {
    m_SiSpacePointMakerTool->
      fillSCT_SpacePointEtaOverlapCollection(clusters1, clusters2, min, max, 
					     m_allClusters, beampos, 
					     elements,
					     m_spacepointoverlapCollection);
  }
}

//--------------------------------------------------------------------------
void SCT_TrigSpacePointTool::
  checkForSCT_Points(const SCT_ClusterCollection* clusters1,
		     const IdentifierHash& id2,
                     const InDetDD::SiDetectorElementCollection* elements,
		     double min1, double max1, double min2, double max2){

  // get the cluster collections for these two detectors. Clus1 must lie
  // within min1 and max1 and clus between min2 and max2. Used for phi
  // overlaps.


  Amg::Vector3D beampos;
  if (!m_overrideBS){
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle(m_beamSpotKey);
    beampos = beamSpotHandle->beamPos();
  }else {
    Amg::Vector3D ovtx(m_xVertex,m_yVertex,m_zVertex); 
    beampos = ovtx;
  }

  SCT_ClusterContainer::const_iterator
    it(m_Sct_clcontainer->indexFind(id2));
  if (it==m_Sct_clcontainer->end()) return;

  const SCT_ClusterCollection * clusters2 (&(**it));

  m_SiSpacePointMakerTool->
    fillSCT_SpacePointPhiOverlapCollection(clusters1, clusters2, min1, max1, 
					   min2, max2, m_allClusters, beampos,
					   elements,
					   m_spacepointoverlapCollection);

}


}

