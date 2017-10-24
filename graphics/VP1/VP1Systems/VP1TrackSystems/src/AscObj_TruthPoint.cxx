/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AscObj_TruthPoint                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/AscObj_TruthPoint.h"
#include "VP1TrackSystems/SimHitHandleBase.h"
#include "VP1Base/VP1Msg.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// Eigen migration
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/GlobalMomentum.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/AmgStringHelpers.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include <string> // for C++11 to_string features


//____________________________________________________________________
class AscObj_TruthPoint::Imp {
public:
  Imp(const HepMC::GenVertex * v, const HepMC::GenParticle * p)
    : genVertex(v), genParticle(p), simhit(0) {}
  Imp(SimHitHandleBase*s)
    : genVertex(0), genParticle(0), simhit(s) {}
  const HepMC::GenVertex * genVertex;
  const HepMC::GenParticle * genParticle;
  SimHitHandleBase * simhit;
};


//____________________________________________________________________
AscObj_TruthPoint::AscObj_TruthPoint(TrackHandleBase*th, const HepMC::GenVertex * v, const HepMC::GenParticle * p)
  : AssociatedObjectHandleBase(th), m_d(new Imp(v,p))
{
}

//____________________________________________________________________
AscObj_TruthPoint::AscObj_TruthPoint(TrackHandleBase*th, SimHitHandleBase*s)
  : AssociatedObjectHandleBase(th), m_d(new Imp(s))
{
}

//____________________________________________________________________
AscObj_TruthPoint::~AscObj_TruthPoint()
{
  delete m_d;
}

//____________________________________________________________________
void AscObj_TruthPoint::buildShapes(SoSeparator*&shape_simple, SoSeparator*&shape_detailed)
{
  VP1Msg::message("AscObj_TruthPoint::buildShapes.");

  // Eigen migration
//  Trk::GlobalPosition p1;//point
//  Trk::GlobalMomentum u;//mom direction
  Amg::Vector3D p1;//point
  Amg::Vector3D u;//mom direction


  if (m_d->simhit) {
    p1 = m_d->simhit->posStart();
    u = m_d->simhit->momentumDirection();
  }
  else
  {
	  // Eigen migration
//    p1 = Trk::GlobalPosition(m_d->genVertex->point3d().x(),m_d->genVertex->point3d().y(),m_d->genVertex->point3d().z());
//    u = Trk::GlobalMomentum(m_d->genParticle->momentum().px(),m_d->genParticle->momentum().py(),m_d->genParticle->momentum().pz()).unit();
    p1 = Amg::Vector3D(m_d->genVertex->point3d().x(),m_d->genVertex->point3d().y(),m_d->genVertex->point3d().z());
    u = Amg::Vector3D(m_d->genParticle->momentum().px(),m_d->genParticle->momentum().py(),m_d->genParticle->momentum().pz()).unit();
  }

  // Eigen migration
//  Trk::GlobalPosition p2 = p1+5*CLHEP::cm*u;
  Amg::Vector3D p2 = p1+5*CLHEP::cm*u;

  SoLineSet * line = new SoLineSet();
  SoVertexProperty * vertices = new SoVertexProperty();;
  vertices->vertex.set1Value(0,p1.x(),p1.y(),p1.z());
  vertices->vertex.set1Value(1,p2.x(),p2.y(),p2.z());
  line->numVertices.set1Value(0,2);
  line->vertexProperty = vertices;

  shape_simple = new SoSeparator;
  shape_simple->addChild(line);
  shape_detailed = shape_simple;

//   //Fixme: Uncomment these two lines:
//   shape_detailed = new SoSeparator;
//   shape_detailed->addChild(line);
}

//____________________________________________________________________
QStringList AscObj_TruthPoint::clicked()
{
	VP1Msg::messageVerbose("AscObj_TruthPoint::clicked()");

  QStringList l;

  l << "  ==> Truth point";
  if (m_d->simhit) {
    l << "Sim Hit ("+m_d->simhit->type()+")";
////    l << "  Position: "+VP1Msg::str( m_d->simhit->posStart() ) ;
//    l << "  Direction: "+VP1Msg::str( m_d->simhit->momentumDirection() );
//    l << "  Momentum: "+VP1Msg::str(m_d->simhit->momentum()/CLHEP::GeV)+" GeV"+(m_d->simhit->actualMomentum()==m_d->simhit->momentum()?"":" (fudged)");
    l << "  Position: " + QString::fromStdString(Amg::AsString(m_d->simhit->posStart()));
    l << "  Direction: " + QString::fromStdString(Amg::AsString(m_d->simhit->momentumDirection()));
    l << "  Momentum: " + VP1Msg::str(m_d->simhit->momentum()/CLHEP::GeV)+" GeV"+(m_d->simhit->actualMomentum()==m_d->simhit->momentum()?"":" (fudged)");
  } else {
    if (!m_d->genVertex||!m_d->genParticle) {
      l << "ERROR";
      return l;
    }

//    Trk::GlobalPosition p(m_d->genVertex->point3d().x(),m_d->genVertex->point3d().y(),m_d->genVertex->point3d().z());
//    Trk::GlobalMomentum mom(m_d->genParticle->momentum().px(),m_d->genParticle->momentum().py(),m_d->genParticle->momentum().pz());
    Amg::Vector3D p(m_d->genVertex->point3d().x(),m_d->genVertex->point3d().y(),m_d->genVertex->point3d().z());
    Amg::Vector3D mom(m_d->genParticle->momentum().px(),m_d->genParticle->momentum().py(),m_d->genParticle->momentum().pz());

    l << "Gen Particle vertex";
//    l << "  Position: "+VP1Msg::str(p);
//    l << "  Direction: "+ VP1Msg::str(mom.unit());
    l << "  Position: "+ QString::fromStdString(Amg::AsString(p));
    l << "  Direction: "+ QString::fromStdString( Amg::AsString(mom.unit()) );
    l << "  Momentum: "+ VP1Msg::str(mom.mag()/CLHEP::GeV)+" GeV";
  }

  return l;
}

//____________________________________________________________________
int AscObj_TruthPoint::regionIndex() const
{
  return 0;
}

//____________________________________________________________________
double AscObj_TruthPoint::lodCrossOverValue() const
{
  return 100;
}
