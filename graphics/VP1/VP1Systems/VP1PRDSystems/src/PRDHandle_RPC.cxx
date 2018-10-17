/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandle_RPC.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Base/VP1Msg.h"
#include "VP1HEPVis/nodes/SoTransparency.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>

#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include <sstream>

//____________________________________________________________________
PRDHandle_RPC::PRDHandle_RPC(PRDCollHandle_RPC*collhandle, const Muon::RpcPrepData*rpc)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_rpc(rpc)
{
    SoTransparency::initClass();
}

//____________________________________________________________________
void PRDHandle_RPC::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{
  const RpcIdHelper * idhelper = VP1DetInfo::rpcIDHelper();
  if (!idhelper)
    return;

  Identifier id = m_rpc->identify();
  SoSeparator * errDetailed  = new SoSeparator;

  int measPhi = idhelper->measuresPhi( id );
  double striplength = m_rpc->detectorElement()->StripLength(measPhi);
  shape_simple = common()->nodeManager()->getShapeNode_Strip(striplength);


  // Translation from objects local position to the strip position
  const Amg::Vector2D& localpos = m_rpc->localPosition();
  const Amg::Vector3D& globalposHIT = m_rpc->detectorElement()->stripPos( id );
  // get local position on wire plane, here we have to use a tolarance as the wire plane is located 2.5 CLHEP::mm
  // from the strip plane
  double tolerance = 3.;
//  const Amg::Vector2D * localposHIT = m_rpc->detectorElement()->surface( id ).globalToLocal(globalposHIT,tolerance);
  const Amg::Vector2D * localposHIT = m_rpc->detectorElement()->surface( id ).Trk::Surface::globalToLocal(globalposHIT,tolerance); // TODO: this is a workaround because of missing function in Trak::PlaneSurface.h
  if( !localposHIT )
  {
    localposHIT = new Amg::Vector2D;
    VP1Msg::message("Warning: Local wire position is NULL");
  }
  SoTranslation * localtrans0 = new SoTranslation;
  localtrans0->translation.setValue((*localposHIT)[Trk::locX]-localpos[Trk::locX],(*localposHIT)[Trk::locY]-localpos[Trk::locY],0);

  bool project(static_cast<PRDCollHandle_RPC*>(collHandle())->project());

  const std::vector<Identifier> rdolist = m_rpc->rdoList();
  if (rdolist.size() == 1 || !collHandle()->drawRDOs())
  {
    errDetailed->addChild(localtrans0);
    errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(project ? striplength+0.2 : striplength,
							       m_rpc->detectorElement()->StripWidth(measPhi),//strip width
							       project ? 2*(6.85+0.1) : 0.8));//strip thickness - hardcoded to something (hopefully) reasonable.
    //Translate back so errDetailed is left sane (eg. when drawing errors later)
    SoTranslation * localtransBack = new SoTranslation;
    localtransBack->translation.setValue(-(localtrans0->translation.getValue()));
    errDetailed->addChild(localtransBack);
  } else {
    VP1Msg::message("Warning: RPC has additional elements in rdoList: THIS HAS NEVER BEEN TESTED");
    SoSeparator * rdos = new SoSeparator;
    rdos->addChild(localtrans0);

    rdos->addChild(common()->nodeManager()->getShapeNode_Strip(project ? striplength+0.2 : striplength,
							       m_rpc->detectorElement()->StripWidth(measPhi),//strip width
							       project ? 2*(6.85+0.1) : 0.8));//strip thickness - hardcoded to something (hopefully) reasonable.

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
      const Amg::Vector3D& globalposRDO = m_rpc->detectorElement()->stripPos( *it );
//      const Amg::Vector2D * localposRDO = m_rpc->detectorElement()->surface( *it ).globalToLocal(globalposRDO,tolerance);
      const Amg::Vector2D * localposRDO = m_rpc->detectorElement()->surface( *it ).Trk::Surface::globalToLocal(globalposRDO,tolerance); // TODO: this is a workaround because of missing function in Trak::PlaneSurface.h
      if (!localposRDO)
      {
        VP1Msg::message("Warning: Local wire position is NULL");
        continue;
      }

      SoTranslation * localtrans = new SoTranslation;
      localtrans->translation.setValue((*localposRDO)[Trk::locX]-localposOLD[Trk::locX],(*localposRDO)[Trk::locY]-localposOLD[Trk::locY],0);
      rdos->addChild(localtrans);

      rdos->addChild(common()->nodeManager()->getShapeNode_Strip(project ? striplength+0.2 : striplength,
							       m_rpc->detectorElement()->StripWidth(measPhi),//strip width
							       project ? 2*(6.85+0.1) : 0.8));

      localposOLD = *localposRDO;
      delete localposRDO;
    }
    errDetailed->addChild(rdos);
  }
  delete localposHIT;
  shape_detailed = errDetailed;
}

//____________________________________________________________________
int PRDHandle_RPC::regionIndex()
{
  //Unique for each station.
  const MuonGM::MuonStation* station = m_rpc->detectorElement()->parentMuonStation();
  return station->getPhiIndex()-99999*station->getEtaIndex();//hopefully unique.
}

//____________________________________________________________________
QStringList PRDHandle_RPC::clicked() const
{
    if (!m_rpc)
      return QStringList("Null PRD");
    const RpcIdHelper * idhelper = VP1DetInfo::rpcIDHelper();
    if (!idhelper)
      return QStringList("Null PRD");
    std::ostringstream os;
    Identifier id = m_rpc->identify();

    os << "RpcPrepData with Identifier ["<<id.get_compact()  ;
    os << "] = [" << (idhelper->print_to_string(id)).c_str()  ;
    os << "]\n at global position = [" << m_rpc->globalPosition()<<"], local position = ["<<m_rpc->localPosition()<<"].";
    os <<"\nTime: "<<m_rpc->time();
    os <<", Trigger info: "<<m_rpc->triggerInfo();
    os <<", Ambiguity Flag: 0x"<<std::hex<<m_rpc->ambiguityFlag()<<std::dec;
    os << ". Rdo list= [";
    for( unsigned int i=0;i<m_rpc->rdoList().size();++i) os << m_rpc->rdoList()[i] << " , ";
    os << "].";
    QStringList l(QString(os.str().c_str()));
    //    if (detailed) l << PRDHandleBase::clicked() ;

    return l;
}
