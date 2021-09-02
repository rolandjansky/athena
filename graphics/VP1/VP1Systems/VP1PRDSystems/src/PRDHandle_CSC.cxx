/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandle_CSC.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/VP1DetInfo.h"

#include "VP1HEPVis/nodes/SoTransparency.h"

#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>

#include <sstream>

//____________________________________________________________________
PRDHandle_CSC::PRDHandle_CSC(PRDCollHandle_CSC*collhandle, const Muon::CscPrepData*csc)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_csc(csc)
{
    SoTransparency::initClass();
}

//____________________________________________________________________
void PRDHandle_CSC::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{
  const CscIdHelper * idhelper = VP1DetInfo::cscIDHelper();
  if (!idhelper)
    return;

  Identifier id = m_csc->identify();

  SoSeparator * errDetailed  = new SoSeparator;

  double striplength = m_csc->detectorElement()->stripLength( id );
  if (static_cast<PRDCollHandle_CSC*>(collHandle())->project())
    striplength += 300.0;//Fixme: Rough extension for now
  shape_simple = common()->nodeManager()->getShapeNode_Strip(striplength);

  // Translation from objects local position to the strip position
  const Amg::Vector2D& localpos = m_csc->localPosition();
  const Amg::Vector3D& globalposHIT = m_csc->detectorElement()->stripPos( id );
  // get local position on wire plane, here we have to use a tolarance as the wire plane is located 2.5 CLHEP::mm
  // from the strip plane
  double tolerance = 3.;
  std::optional<Amg::Vector2D> localposHIT = m_csc->detectorElement()->surface( id ).Trk::Surface::globalToLocal(globalposHIT,tolerance); 

  if( !localposHIT )
  {
    localposHIT.emplace();
    localposHIT->setZero();
    VP1Msg::message("Warning: Local wire position is NULL");
  }
  SoTranslation * localtrans0 = new SoTranslation;
  localtrans0->translation.setValue((*localposHIT)[Trk::locX]-localpos[Trk::locX],(*localposHIT)[Trk::locY]-localpos[Trk::locY],0);

  const double maxCharge = 1e7;
  //Thickness of highlighed strip
  //FIXME: the maxCharge/3 is just a quick fix to get a nice presentation
  //       maybe this should go like sqrt(charge/maxCharge) so that it
  //       is clear for small charges.
  //const double stripHeight = (1 + 4*sqrt(m_csc->charge()/maxCharge))*CLHEP::mm; // [1,5]
  //
  //       The minimum is chosen equal to the neighbouring strips, such
  //       that a zero charge will give the same stirp height
  const double stripHeight = (1 + 4*m_csc->charge()/(maxCharge/3))*CLHEP::mm; // [1,5]
  //Thickness of additional strips (from rdoList())
  const double stripHeightRDOs = 1*CLHEP::mm;

  const std::vector<Identifier> rdolist = m_csc->rdoList();
  if (rdolist.size() == 1 || !collHandle()->drawRDOs())
  {
    errDetailed->addChild(localtrans0);
    errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
						  m_csc->detectorElement()->cathodeReadoutPitch(idhelper->chamberLayer( id ),
							idhelper->measuresPhi( id )), stripHeight));

    //Translate back so errDetailed is left sane (eg. when drawing errors later)
    SoTranslation * localtransBack = new SoTranslation;
    localtransBack->translation.setValue(-(localtrans0->translation.getValue()));
    errDetailed->addChild(localtransBack);
  } else
  {
    SoSeparator * rdos = new SoSeparator;
    rdos->addChild(localtrans0);

    rdos->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
						  m_csc->detectorElement()->cathodeReadoutPitch(idhelper->chamberLayer( id ),
							idhelper->measuresPhi( id )), stripHeight));

    SoTransparency * transparent = new SoTransparency;
    transparent->transparency.setValue(0.5);
    rdos->addChild( transparent );
    Amg::Vector2D localposOLD = *localposHIT;
    std::vector<Identifier>::const_iterator st = rdolist.begin();
    std::vector<Identifier>::const_iterator en = rdolist.end();
    for( std::vector<Identifier>::const_iterator it = st; it!=en; it++)
    {
      if (*it == id )
        continue;
      const Amg::Vector3D& globalposRDO = m_csc->detectorElement()->stripPos( *it );

      //      const Amg::Vector2D * localposRDO = m_csc->detectorElement()->surface( *it ).globalToLocal(globalposRDO,tolerance);
      std::optional<Amg::Vector2D>  localposRDO = m_csc->detectorElement()->surface( *it ).Trk::Surface::globalToLocal(globalposRDO,tolerance);

      if (!localposRDO)
      {
        VP1Msg::message("Warning: Local wire position is NULL");
        continue;
      }

      SoTranslation * localtrans = new SoTranslation;
      localtrans->translation.setValue((*localposRDO)[Trk::locX]-localposOLD[Trk::locX],(*localposRDO)[Trk::locY]-localposOLD[Trk::locY],0);
      rdos->addChild(localtrans);
      double striplengthRDO = m_csc->detectorElement()->stripLength( *it );

      rdos->addChild(common()->nodeManager()->getShapeNode_Strip(striplengthRDO,
                     m_csc->detectorElement()->cathodeReadoutPitch(idhelper->chamberLayer( *it ),
                     idhelper->measuresPhi( *it )), stripHeightRDOs));

      localposOLD = *localposRDO;
    }
    errDetailed->addChild(rdos);
  }
  
  SoMaterial * mat = new SoMaterial;
  mat->diffuseColor.setValue(1.0,0,0);
  errDetailed->addChild(mat);
  errDetailed->addChild(common()->nodeManager()->getShapeNode_Cross(10));

  shape_detailed = errDetailed;
}

//____________________________________________________________________
int PRDHandle_CSC::regionIndex()
{
  //Unique for each station.
  const MuonGM::MuonStation* station = m_csc->detectorElement()->parentMuonStation();
  return station->getPhiIndex()-99999*station->getEtaIndex();//hopefully unique.
}

//____________________________________________________________________
QStringList PRDHandle_CSC::clicked() const
{

  if (!m_csc)
    return QStringList("Null PRD");
  const CscIdHelper * idhelper = VP1DetInfo::cscIDHelper();
  if (!idhelper)
    return QStringList("Null PRD");

  Identifier id = m_csc->identify();
  std::ostringstream os;
  os << "CscPrepData with Identifier ["<<id.get_compact()  ;
  os << "] = [" << (idhelper->print_to_string(id)).c_str()  ;
  os << "]\n at global position = [" << m_csc->globalPosition()<<"], local position = ["<<m_csc->localPosition()<<"].";
  os << ". Charge="<<m_csc->charge()<<", time="<< m_csc->time()<<", status="<<Muon::toString(m_csc->status());
  os << ". RDO list= [";
  for( unsigned int i=0;i<m_csc->rdoList().size();++i) os << m_csc->rdoList()[i] << " , ";
  os << "].";
  // os << *(m_csc);
  return QString(os.str().c_str()).split("\n");
}


