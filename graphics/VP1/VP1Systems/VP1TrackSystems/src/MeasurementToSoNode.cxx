/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1TrackSystems/MeasurementToSoNode.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include "VP1HEPVis/nodes/SoTubs.h"
#include "VP1HEPVis/nodes/SoGenericBox.h"
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoRotationXYZ.h>

#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Base/VP1Msg.h"



// Amg Eigen objects
#include "GeoPrimitives/GeoPrimitives.h"

//MeasurementBase
#include "TrkMeasurementBase/MeasurementBase.h"

//ROTs
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

//CompetingROTs
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

//Pseudo Measurements
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

//Segments
#include "TrkSegment/Segment.h"

// PRDs
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"

//IdHelpers
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"

Trk::MeasurementToSoNode::MeasurementToSoNode()
    :
    m_trtProjection(false),
    m_cscIdHelper(0),
    m_rpcIdHelper(0),
    m_tgcIdHelper(0),
    m_mdtIdHelper(0),
    m_stripThickness(0.8),
    m_dtDrawMode(Trk::MeasurementToSoNode::ShortLength),
    m_clusDrawMode(Trk::MeasurementToSoNode::SingleStrip),
    m_dtLength(10.0)
{
  SoTubs::initClass();
  SoGenericBox::initClass();
  // Have simple view on by default
  for (int i=0; i<TrkObjToString::Unknown ; i++) m_simpleView.push_back(true);
}

void
Trk::MeasurementToSoNode::initialize(   const CscIdHelper* cscIdHelper,
                                        const RpcIdHelper* rpcIdHelper,
                                        const TgcIdHelper* tgcIdHelper,
                                        const MdtIdHelper* mdtIdHelper)
{
  m_cscIdHelper=cscIdHelper;
  m_rpcIdHelper=rpcIdHelper;
  m_tgcIdHelper=tgcIdHelper;
  m_mdtIdHelper=mdtIdHelper;
}

std::pair<SoSeparator*,  std::vector<SoNode*> >
Trk::MeasurementToSoNode::createMeasSep(  const Trk::MeasurementBase& meas)
{

    VP1Msg::messageVerbose("Trk::MeasurementToSoNode::createMeasSep()");

    // Vector to hold all the nodes associated with this measurement
    std::vector<SoNode*> measurements;
    SoTransform*   theHitTransform=0;
    TrkObjToString::MeasurementType detType = m_objToType.type(&meas);
    // Find the correct type and call that method.

    switch (detType)
    {
        case TrkObjToString::Pixel:
        case TrkObjToString::SCT:
        case TrkObjToString::TRT:
        case TrkObjToString::CSC:
        case TrkObjToString::TGC:
        case TrkObjToString::RPC:
        case TrkObjToString::MDT:
        {
            const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(&meas);
            measurements = toSoNodes( *rot );
            theHitTransform=createTransform(*rot, detType);
            break;
        }
        case TrkObjToString::CompetingROT:
        {
            const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(&meas);
            measurements = toSoNodes(*crot);
            const Trk::RIO_OnTrack* mostProbROT =  &(crot->rioOnTrack( crot->indexOfMaxAssignProb ()));
            theHitTransform=createTransform( static_cast<const Trk::MeasurementBase&>(*mostProbROT), TrkObjToString::type( mostProbROT ));
            break;
        }
        case TrkObjToString::PseudoMeasurement:
        {
            const Trk::PseudoMeasurementOnTrack* pseudo = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(&meas) ;
            measurements = toSoNodes(*pseudo);
            theHitTransform=createTransform( *pseudo, detType);
            break;
        }
        case TrkObjToString::Segment:
        case TrkObjToString::Hole:
        case TrkObjToString::Unknown:
        {
            std::cerr<< "MeasurementToSoNode::createMeasSep: WARNING - Unknown detector type!"<<std::endl;
            std::cerr<<" Dumping MeasurementBase: "<<meas<<std::endl;
        }

    }
    // Add all the above to the separator
    SoSeparator *theHitMarkerSep = new SoSeparator;
    theHitMarkerSep->addChild(theHitTransform);

    std::vector<SoNode*>::const_iterator it, itEnd=measurements.end();
    for ( it = measurements.begin(); it!=itEnd; it++)
    {
        theHitMarkerSep->addChild(*it);
    }
    //std::cout<<"MeasurementToSoNode::createMeasSep - done"<<std::endl;

    return std::make_pair(theHitMarkerSep, measurements);
}

std::vector<SoNode*>
Trk::MeasurementToSoNode::toSoNodes(const Trk::RIO_OnTrack& rot)
{
    //std::cout<<"toSoNodes"<<std::endl;

  //TODO Implement modes properly
  std::vector<SoNode*> nodes ;
  switch (m_clusDrawMode)
  {
  case SingleStrip:
  case MultipleStrips:
  case Errors: 
  {
    double coord1 = rot.localParameters().get(Trk::loc1);
    double coord2 = 0.0;
    if (rot.localParameters().dimension()>1) coord2 = rot.localParameters().get(Trk::loc2);
//    nodes.push_back( toSoNode(rot.detectorElement(), rot.identify(), m_objToType.type(&rot), Trk::LocalPosition(coord1,coord2) ) );
    nodes.push_back( toSoNode(rot.detectorElement(), rot.identify(), m_objToType.type(&rot), Amg::Vector2D(coord1,coord2) ) );
    break;
  }
  default:
    break;
  }
  return nodes;
}

std::vector<SoNode*>
Trk::MeasurementToSoNode::toSoNodes(const Trk::CompetingRIOsOnTrack& crot)
{
    VP1Msg::messageVerbose("Trk::MeasurementToSoNode::CompetingRIOsOnTrack()");

    //
    // ++++++++++++++++++++++ TO DO! +++++++++++++++++++++++++
    //
    const Trk::RIO_OnTrack* mostProbROT =  &(crot.rioOnTrack( crot.indexOfMaxAssignProb ()));
    std::vector<SoNode*> nodes = toSoNodes( *mostProbROT ); // Temporary hack - should draw all competing ROTs and show them linked somehow.
    return nodes;
}

std::vector<SoNode*>
Trk::MeasurementToSoNode::toSoNodes(const Trk::PseudoMeasurementOnTrack& /*rot*/)
{
    //
    // ++++++++++++++++++++++ TO DO! +++++++++++++++++++++++++
    //
	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::PseudoMeasurementOnTrack()");

    std::vector<SoNode*> nodes ;
    nodes.push_back( m_nodeManager.getShapeNode_Strip(10.0, 10.0, 10.0) );
    return nodes;
}

SoNode*
Trk::MeasurementToSoNode::toSoNode(const Trk::TrkDetElementBase* baseDetEl, Identifier id, TrkObjToString::MeasurementType prdType, Amg::Vector2D locPos)
{
	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::toSoNode(Trk)");

  SoNode *theHitMarker = 0;
  switch (prdType)
  {
  case TrkObjToString::Pixel:
  case TrkObjToString::SCT:
    {
      const InDetDD::SiDetectorElement* detEl =
        dynamic_cast<const InDetDD::SiDetectorElement*>(baseDetEl);
      assert(detEl!=0); // by definition this should never fail
      theHitMarker = toSoNode(detEl, id, prdType, locPos);
      break;
    }
  case TrkObjToString::MDT:
  case TrkObjToString::TRT:
    {
      theHitMarker = toSoNodeDriftTube(baseDetEl, id, prdType, locPos[0]);
      break;
    }
  case TrkObjToString::CSC:
    {
      const MuonGM::CscReadoutElement* detEl =
        dynamic_cast<const MuonGM::CscReadoutElement*>(baseDetEl);
      assert(detEl!=0); // by definition this should never fail
      theHitMarker=toSoNode( detEl, id );
      break;
    }
  case TrkObjToString::RPC:
    {
      const MuonGM::RpcReadoutElement* detEl =
        dynamic_cast<const MuonGM::RpcReadoutElement*>(baseDetEl);
      assert(detEl!=0); // by definition this should never fail
      theHitMarker=toSoNode( detEl, id );
      break;
    }
  case TrkObjToString::TGC:
    {
      const MuonGM::TgcReadoutElement* detEl =
        dynamic_cast<const MuonGM::TgcReadoutElement*>(baseDetEl);
      assert(detEl!=0); // by definition this should never fail
      theHitMarker=toSoNode( detEl, id );
      break;
    }

  default:
    //message( "Specific Hit type could not be identified.");
    theHitMarker = m_nodeManager.getShapeNode_Strip(10.0, 10.0, 10.0);
    break;
  }
  return theHitMarker;
}


SoNode*
Trk::MeasurementToSoNode::toSoNode( const InDetDD::SiDetectorElement* detEl, Identifier /*id*/, TrkObjToString::MeasurementType detType, Amg::Vector2D& locPos)
{
	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::toSoNode(InDetDD)");

  SoNode *theHitMarker = 0;
  if ( isSimpleView(TrkObjToString::Pixel) && detType==TrkObjToString::Pixel)
  {
    // theHitMarker = createPointMarker(); // Simple Pixel
    theHitMarker = m_nodeManager.getShapeNode_Point(); // Simple Pixel
  }
  else
  {

    double stripLength=detEl->etaPitch() ;
    if (isSimpleView(TrkObjToString::SCT) && detType==TrkObjToString::SCT)
    {
      theHitMarker = createLineMarker(stripLength/2.0); // Simple SCT
    }
    else
    {
      double stripWidth=detEl->phiPitch( locPos );
      theHitMarker = m_nodeManager.getShapeNode_Strip(stripLength, stripWidth, m_stripThickness); // Detailed SCT/Pixel
    }
  }
  return theHitMarker;
}


SoNode*
Trk::MeasurementToSoNode::toSoNodeDriftTube(const Trk::TrkDetElementBase* detEl, Identifier id, TrkObjToString::MeasurementType detType, double radius)
{
	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::toSoNodeDriftTube(Trk)");

  //////std::cout<<"Drift tube : isSimpleView = "<<isSimpleView(type)<<std::endl;
  SoNode *theHitMarker = 0;
  const Trk::CylinderBounds* ccbo = dynamic_cast<const Trk::CylinderBounds*>(&(detEl->surface(id).bounds()));
  assert(ccbo!=0);

  
  double hlength = ccbo->halflengthZ(); // the halflength of the actual tube
  
  if (m_dtDrawMode==ShortLength && m_dtLength < hlength){
      hlength =  m_dtLength;
  }

  if ( m_trtProjection && detType==TrkObjToString::TRT ) {
    theHitMarker  = createProjectedDriftTubeMarker(hlength, true);
  } else if ( isSimpleView(detType) )
  {
    theHitMarker = createLineMarker(hlength, true);
  }
  else
  {
    //TKP1 if (detType==Trk::MeasurementToSoNode::TRT) radius = 1.999;//CLHEP::mm
    //TKP1 else if (fabs(radius)<5.0) radius = 5.0;
    theHitMarker    = createDriftTubeMarker(hlength, radius);
  }
  return theHitMarker;
}

SoNode*
Trk::MeasurementToSoNode::toSoNode(const MuonGM::CscReadoutElement* detEl, Identifier id)
{
	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::toSoNode(MuonGM::CscReadoutElement)");

  SoNode *theHitMarker = 0;
  int measPhi = m_cscIdHelper->measuresPhi( id );
  int chamberLayer = m_cscIdHelper->chamberLayer( id );
  double stripWidth=detEl->cathodeReadoutPitch(chamberLayer, measPhi) ;
  double stripLength=detEl->stripLength( id );
  if ( isSimpleView(TrkObjToString::CSC) )
  {
    theHitMarker = createLineMarker(stripLength/2.0);
  }
  else
  {
    theHitMarker = m_nodeManager.getShapeNode_Strip(stripLength, stripWidth, m_stripThickness);
  }
  return theHitMarker;
}

SoNode*
Trk::MeasurementToSoNode::toSoNode(const MuonGM::RpcReadoutElement* detEl, Identifier id)
{
	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::toSoNode(MuonGM::RpcReadoutElement)");

  SoNode *theHitMarker = 0;
  int measPhi = m_rpcIdHelper->measuresPhi( id );
  double stripWidth=detEl->StripWidth(measPhi) ;
  double stripLength=detEl->StripLength(measPhi) ;
  //////std::cout<<"RPC measPhi: "<< measPhi<<std::endl;

  if ( isSimpleView(TrkObjToString::RPC) )
  {
    theHitMarker = createLineMarker(stripLength/2.0);
  }
  else
  {
    theHitMarker = m_nodeManager.getShapeNode_Strip(stripLength, stripWidth, m_stripThickness);
  }
  return theHitMarker;
}

SoNode*
Trk::MeasurementToSoNode::toSoNode(const MuonGM::TgcReadoutElement* detEl, Identifier id)
{
	VP1Msg::messageVerbose("Trk::MeasurementToSoNode::toSoNode(MuonGM::TgcReadoutElement)");

  SoNode *theHitMarker = 0;
  int plane     = m_tgcIdHelper->gasGap( id );
  int strip     = m_tgcIdHelper->channel( id );
  int isStrip   = m_tgcIdHelper->isStrip( id );

  double striplength =0.0, stripWidth = 0.0;

  if (isStrip){
    striplength = detEl->stripLength(plane, strip);
    stripWidth  = detEl->stripWidth(plane, strip);
  } else {        
    striplength = detEl->gangShortWidth(plane, strip);
    stripWidth  = detEl->gangLength(plane, strip);
  }

  if ( isSimpleView(TrkObjToString::TGC) )
  {
    theHitMarker = createLineMarker(striplength/2.0);
  }
  else
  {
    theHitMarker = m_nodeManager.getShapeNode_Strip(striplength, stripWidth, m_stripThickness);
  }
  return theHitMarker;
}

SoNode*
Trk::MeasurementToSoNode::createLineMarker( double halfLength, bool isTube )
{
  //////std::cout<<"createLineMarker. halfLength="<<halfLength<<std::endl;
  SoVertexProperty * scatVtxProperty = new SoVertexProperty();
  if (isTube)
  {
    scatVtxProperty->vertex.set1Value(0, 0.0,0.0,-halfLength);
    scatVtxProperty->vertex.set1Value(1, 0.0,0.0, halfLength);
  }
  else
  {
    scatVtxProperty->vertex.set1Value(0, 0.0,-halfLength,0.0);
    scatVtxProperty->vertex.set1Value(1, 0.0,halfLength,0.0);
  }
  SoLineSet * line = new SoLineSet();
  line->numVertices = 2;
  line->vertexProperty = scatVtxProperty;
  return line;
}

SoNode*
Trk::MeasurementToSoNode::createDriftTubeMarker(double halfLength, double radius)
{
  //Fixme: Use SoCylinder instead!!!
  SoTubs* tubeSurface = new SoTubs();
//  (*lineSurface).pRMin = fabs(radius)-0.2;
  (*tubeSurface).pRMin = 0.0;

  (*tubeSurface).pRMax = std::max(float(fabs(radius)),0.05f);//min 50 micron (fixme: use line/point instead)
  (*tubeSurface).pDz   = halfLength;
  //////std::cout<<"createDriftTubeMarker: done"<<std::endl;
  return tubeSurface;
}


SoNode*
Trk::MeasurementToSoNode::createProjectedDriftTubeMarker(double strawLength, double /*radius*/)
{
  //Fix to put the hits from short barrel straws onto same z as the long barrel straws:
  if (strawLength<160.0)
    strawLength = 2*349.3150-strawLength;//Yeah, we hardcode a lot here... but dimensions of TRT barrel straws are not likely to change.

//   SoSeparator * sep = new SoSeparator;

//   SoDrawStyle * ds = new SoDrawStyle;
//   ds->pointSize.setValue(2.0f);
//   sep->addChild(ds);

  SoPointSet       * scatPointSet    = new SoPointSet;
  SoVertexProperty * scatVtxProperty = new SoVertexProperty;
  scatVtxProperty->vertex.set1Value(0,0.0f,0.0f,strawLength);
  scatPointSet->numPoints=1;
  scatPointSet->vertexProperty.setValue(scatVtxProperty);
  return scatPointSet;

//   sep->addChild(scatPointSet);
//   return sep;
}



SoTransform*
Trk::MeasurementToSoNode::createTransform(  const Trk::MeasurementBase& mb, TrkObjToString::MeasurementType detType)
{
  const Trk::Surface& theSurface = mb.associatedSurface();

  // We need to handle various cases:
  // -> full tubes - use the surface position (or equivalent, global position of straw)
  // -> short tubes/strips - use the global position of the ROT (but this is now done inside AscoObj_TSOS)
  // -> long strips - use the global position of centre of strip.

  SoTransform*   theHitTransform = VP1LinAlgUtils::toSoTransform(theSurface.transform());

  Amg::Vector3D theHitGPos ;
  // Amg::Vector3D theHitGPos = mb.globalPosition();
  if ( detType==TrkObjToString::MDT || detType==TrkObjToString::TRT )
     {
       // for tubes, should use position of center of tube (if drawing full tube), or position of ROT for short tubes
       if (m_dtDrawMode==FullLength) 
         theHitGPos = theSurface.center();
       else
         theHitGPos = mb.globalPosition();
     } else {
       // so, strips
       switch (detType) {
         case TrkObjToString::CSC:
         {
           const Muon::CscClusterOnTrack* csc = dynamic_cast<const Muon::CscClusterOnTrack* >(&mb);
            assert(csc);
            theHitGPos = csc->detectorElement()->stripPos( csc->identify() );;
            break;
          }
         case TrkObjToString::TGC:
         {
           const Muon::TgcClusterOnTrack* tgc = dynamic_cast<const Muon::TgcClusterOnTrack* >(&mb);
            assert(tgc && tgc->detectorElement());
            theHitGPos = tgc->detectorElement()->channelPos( tgc->identify() ); 
            break;
          }
         case TrkObjToString::RPC:
         {
           const Muon::RpcClusterOnTrack* rpc = dynamic_cast<const Muon::RpcClusterOnTrack* >(&mb);
            assert(rpc);
            theHitGPos = rpc->detectorElement()->stripPos( rpc->identify() );
            break;
          }
         case TrkObjToString::SCT: 
         case TrkObjToString::Pixel:
         
         {        // For strips we should use the centre of the strip - this is a bit of a hack - should use detector elements (add above)
           const Amg::Vector3D* tempHitGPos = theSurface.localToGlobal( mb.localParameters() );
           theHitGPos=*tempHitGPos;
           delete tempHitGPos;
           break;
         }
         case TrkObjToString::MDT:
         case TrkObjToString::TRT:
         default:
           // shouldn't ever get in here!
        	 // initializations to avoid warnings
        	 theHitGPos[0]=-999.;
        	 theHitGPos[1]=-999.;
        	 theHitGPos[2]=-999.;
           break;
       }
     }
  
  theHitTransform->translation.setValue(theHitGPos[0], theHitGPos[1], theHitGPos[2]);
  return theHitTransform;
}
