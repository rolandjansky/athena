/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandle_SCT.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1PRDSystems/PRDCollHandle_SCT.h"
#include "VP1HEPVis/nodes/SoTransparency.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "GeoPrimitives/GeoPrimitives.h"


//____________________________________________________________________
PRDHandle_SCT::PRDHandle_SCT(PRDCollHandle_SCT*collhandle, const InDet::SCT_Cluster*clus)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_cluster(clus)
{
    SoTransparency::initClass();
}

//____________________________________________________________________
void PRDHandle_SCT::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{
  double striplength = m_cluster->detectorElement()->etaPitch();

  //SoSeparator * errSimple  = new SoSeparator;
  SoSeparator * errDetailed  = new SoSeparator;

  //errSimple->addChild(common()->nodeManager()->getShapeNode_Strip(striplength));
  shape_simple=common()->nodeManager()->getShapeNode_Strip(striplength);

  const Amg::Vector2D& localpos = m_cluster->localPosition();
  const Amg::Vector2D& localposHIT = m_cluster->detectorElement()->localPositionOfCell( m_cluster->identify() );
  SoTranslation * localtrans0 = new SoTranslation;
  localtrans0->translation.setValue(localposHIT[Trk::locX]-localpos[Trk::locX],localposHIT[Trk::locY]-localpos[Trk::locY],0);

  const std::vector<Identifier> rdolist = m_cluster->rdoList();
  if (rdolist.size() == 1 || !collHandle()->drawRDOs())
  {
    errDetailed->addChild(localtrans0);
    errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
					     m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() ),//strip width
					     m_cluster->detectorElement()->thickness()*3.0));
    //strip thickness - scaled up by factor of 3 (looks better)
    //Fixme: Should we drop this upscaling of thickness?

    //Translate back so errDetailed is left sane (eg. when drawing errors later)
    SoTranslation * localtransBack = new SoTranslation;
    localtransBack->translation.setValue(-(localtrans0->translation.getValue()));
    errDetailed->addChild(localtransBack);
  } else
  {
    SoSeparator * rdos = new SoSeparator;
    rdos->addChild(localtrans0);

    rdos->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
					     m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() ),//strip width
					     m_cluster->detectorElement()->thickness()*3.0));
    //strip thickness - scaled up by factor of 3 (looks better)
    //Fixme: Should we drop this upscaling of thickness?

    SoTransparency * transparent = new SoTransparency;
    transparent->transparency.setValue(0.5);
    rdos->addChild( transparent );
    Amg::Vector2D localposOLD = localposHIT;
    std::vector<Identifier>::const_iterator st = rdolist.begin();
    std::vector<Identifier>::const_iterator en = rdolist.end();
    for( std::vector<Identifier>::const_iterator it = st; it!=en; it++)
    {
      if (*it == m_cluster->identify() )
        continue;

      const Amg::Vector2D& localposRDO = m_cluster->detectorElement()->localPositionOfCell(*it);

      SoTranslation * localtrans = new SoTranslation;
      localtrans->translation.setValue(localposRDO[Trk::locX]-localposOLD[Trk::locX],localposRDO[Trk::locY]-localposOLD[Trk::locY],0);
      rdos->addChild(localtrans);

      rdos->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
					     m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() ),//strip width
					     m_cluster->detectorElement()->thickness()*3.0));

      localposOLD = localposRDO;
    }
    errDetailed->addChild(rdos);
  }

//  shape_simple = errSimple;
  shape_detailed = errDetailed;
}

//____________________________________________________________________
int PRDHandle_SCT::regionIndex()
{
  //instead of identifier juggling, we simply discretize by center coordinate.
  static const double l=100.0;//CLHEP::mm
  return       static_cast<int>(m_cluster->detectorElement()->center().z()/l)
         +1000*static_cast<int>(m_cluster->detectorElement()->center().y()/l)
      +1000000*static_cast<int>(m_cluster->detectorElement()->center().x()/l);
  //Fixme: Use identifiers instead for more intuitive regions.
}
