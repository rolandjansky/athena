/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleHandle_Muon         //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

// Local
#include "IParticleHandle_Muon.h"
#include "IParticleCollHandle_Muon.h"
#include "AODSysCommonData.h"
#include "MuonCollectionSettingsButton.h"

//VP1
#include "VP1Base/VP1Msg.h"

//SoCoin
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
//____________________________________________________________________
class IParticleHandle_Muon::Imp {
public:
  IParticleHandle_Muon * theclass;
  const xAOD::Muon * muon;
  SoSeparator * sep; // everything hangs from this.
  SoLineSet * line;//This represents the line(s) representing the trackparticle. Can be interpolated.
  QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parametersAndPositions; // cache 
};

//____________________________________________________________________
IParticleHandle_Muon::IParticleHandle_Muon(IParticleCollHandleBase* ch, const xAOD::Muon *tp)
  : IParticleHandleBase(ch), d(new Imp)
{ 
  d->theclass = this;
  d->muon = tp;
  d->sep=0;
  d->line=0;
}

//____________________________________________________________________
IParticleHandle_Muon::~IParticleHandle_Muon()
{
  if (d->line) d->line->unref();
  if (d->sep) d->sep->unref();
  delete d;
}

bool IParticleHandle_Muon::has3DObjects(){
  return 0!=d->sep;
}
void IParticleHandle_Muon::clear3DObjects(){
  VP1Msg::messageVerbose("IParticleHandle_Muon::clear3DObjects()");
  
  if (d->line) {
    d->line->unref();
    d->line=0;
  }
  if (d->sep) {
    d->sep->unref();
    d->sep=0;
  }
}

SoNode* IParticleHandle_Muon::nodes(){
  // VP1Msg::messageVerbose("IParticleHandle_Muon::nodes with d->sep="+VP1Msg::str(d->sep));

  if (d->sep) 
    return d->sep; // FIXME - do we need to check if anything need to be redrawn?
  if (!d->sep) {
    d->sep = new SoSeparator();
    d->sep->ref();
  }


  int iver(0);
  unsigned npointsused(0);

  // Loop through the TP and find the parameters & create points & get momenta for line interpolation
  std::vector<Amg::Vector3D> positions;
  std::vector<Amg::Vector3D> momenta;

  const Trk::Perigee& peri = d->muon->primaryTrackParticle()->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...

  // std::cout<<peri.position()<<std::endl;
  // std::cout<<p4.X()<<", "<<p4.Y()<<", "<<p4.Z()<<std::endl;

  const IParticleCollHandle_Muon* collHandleTrk = dynamic_cast<const IParticleCollHandle_Muon*>(collHandle());
  // FIXME - this ugly...

  float x,y,z;      
  unsigned int iMinusOne=0;

  for (unsigned int i=0; i<d->muon->primaryTrackParticle()->numberOfParameters()+1 ; ++i){
    // std::cout<<"i:"<<i<<"/"<<d->muon->primaryTrackParticle()->numberOfParameters()+1<<std::endl;
    if (0==i) {
      // 1st position - set 'first' to false a few lines further down...
      x = peri.position().x();
      y = peri.position().y();
      z = peri.position().z();
      momenta.push_back(Amg::Vector3D(momentum().x(),
      momentum().y(),
      momentum().z()));
    } else {
      // Other positions
      iMinusOne=i-1;
      x = d->muon->primaryTrackParticle()->parameterX(iMinusOne);
      y = d->muon->primaryTrackParticle()->parameterY(iMinusOne);
      z = d->muon->primaryTrackParticle()->parameterZ(iMinusOne);  
      momenta.push_back(Amg::Vector3D(d->muon->primaryTrackParticle()->parameterPX(iMinusOne),
      d->muon->primaryTrackParticle()->parameterPY(iMinusOne),
      d->muon->primaryTrackParticle()->parameterPZ(iMinusOne)));      
    }
    positions.push_back(Amg::Vector3D(x,y,z));
  } // end of loop.

  // Create new lineset etc
  d->line = new SoLineSet();
  d->line->ref();
  SoVertexProperty * vertices = new SoVertexProperty();
  iver=0;
  // For the moment, lets just draw the lines.
  // std::cout<<"About to loop over "<<positions.size()<<" points to make line"<<std::endl;

  for (unsigned int i=1; i<positions.size(); ++i){
    // std::cout<<"i: "<<i<<" at ("<<positions[i].x()<<","<<positions[i].y()<<","<<positions[i].z()<<")"<<std::endl;
    unsigned int maxCount=10;
    for (unsigned int count=0; count<maxCount;++count){
      float fraction=count/(float)maxCount;
      Amg::Vector3D point1(positions[i-1]+fraction*momenta[i-1]);
      Amg::Vector3D point2(positions[i]-(1.0-fraction)*momenta[i]);
      Amg::Vector3D tangent = point1+fraction*(point2-point1);
      vertices->vertex.set1Value(iver++,tangent.x(),tangent.y(),tangent.z());
      npointsused++;        
    }
  }

  // Add to SoLine set
  d->line->numVertices.set1Value(0,npointsused);
  d->line->vertexProperty = vertices;
  // std::cout<<"About to add "<<collHandleTrk->material()<<std::endl;
  d->sep->addChild(collHandleTrk->material()); 
  // ^^ FIXME - should rearrange so we don't need to reset material
  d->sep->addChild(d->line);

  // points->vertexProperty = vertices2;

  return d->sep;
}

//____________________________________________________________________
QStringList IParticleHandle_Muon::clicked() const
{
  QStringList l;
  l << "Muon:";
  l << IParticleHandleBase::baseInfo();
  return l;
}

//____________________________________________________________________
Amg::Vector3D IParticleHandle_Muon::momentum() const
{
  const Trk::Perigee& p = d->muon->primaryTrackParticle()->perigeeParameters();
  return p.momentum();
}

//____________________________________________________________________
const xAOD::IParticle& IParticleHandle_Muon::iParticle() const
{
  return *(d->muon->primaryTrackParticle());
}

//____________________________________________________________________
double IParticleHandle_Muon::charge() const
{
  return d->muon->primaryTrackParticle()->charge();
}

// //____________________________________________________________________
// unsigned IParticleHandle_Muon::summaryValue(xAOD::SummaryType type) const
// { 
//   uint8_t num = 0;
//   if (d->muon->summaryValue(num,type)){
//     return num;
//   }
//   // else...
//   VP1Msg::message("IParticleHandle_Muon::getSummaryValue - unable to retrieve the requested enum: "+VP1Msg::str(type));
//   return 999999;
// }


QString IParticleHandle_Muon::shortInfo() const
{
  QString l("|P|="+VP1Msg::str(momentum().mag()/CLHEP::GeV)+" [GeV], ");
  return l;
}

void IParticleHandle_Muon::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) 
{
  IParticleHandleBase::fillObjectBrowser(listOfItems); // Obligatory!

  // QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());
  // TSOSitem->setText(0, QString("Def. Parameters " ) );
  // QString dParameters("(");
  // dParameters+=QString::number(d->muon->primaryTrackParticle()->d0());
  // dParameters+=", ";     
  // dParameters+=QString::number(d->muon->primaryTrackParticle()->z0());
  // dParameters+=", ";     
  // dParameters+=QString::number(d->muon->primaryTrackParticle()->phi0());
  // dParameters+=", ";     
  // dParameters+=QString::number(d->muon->primaryTrackParticle()->theta());
  // dParameters+=", ";     
  // dParameters+=QString::number(d->muon->primaryTrackParticle()->qOverP());
  // dParameters+=")";     
  // TSOSitem->setText(1, dParameters );
  
  // for (unsigned int i=0; i<d->muon->primaryTrackParticle()->numberOfParameters() ; ++i){
  //   QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());
  //   TSOSitem->setText(0, QString("Parameter "+QString::number( i+1 ) ) );
  //   QString pos(", Position = (");  
  //   pos+=QString::number(d->muon->primaryTrackParticle()->parameterX(i));
  //   pos+=", ";    
  //   pos+=QString::number(d->muon->primaryTrackParticle()->parameterY(i));
  //   pos+=", ";
  //   pos+=QString::number(d->muon->primaryTrackParticle()->parameterZ(i));
  //   pos+=")";
  //   
  //   switch (d->muon->primaryTrackParticle()->parameterPosition(i)){
  //     case xAOD::BeamLine: 
  //       TSOSitem->setText(1, QString("BeamLine" )+pos );
  //       break;
  //     case xAOD::FirstMeasurement:
  //       TSOSitem->setText(1, QString("FirstMeasurement")+pos );
  //       break;
  //     case xAOD::LastMeasurement: 
  //       TSOSitem->setText(1, QString("LastMeasurement" )+pos );
  //       break;
  //     case xAOD::CalorimeterEntrance:
  //       TSOSitem->setText(1, QString("CalorimeterEntrance")+pos );
  //       break;
  //     case xAOD::CalorimeterExit: 
  //       TSOSitem->setText(1, QString("CalorimeterExit" )+pos );
  //       break;
  //     case xAOD::MuonSpectrometerEntrance:
  //       TSOSitem->setText(1, QString("MuonSpectrometerEntrance")+pos );
  //       break;
  //     default:
  //       TSOSitem->setText(1, QString("Undefined")+pos );
  //   }
  // }
  
  // TODO - add more.
}

const QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> >& IParticleHandle_Muon::parametersAndPositions() const {
  // 1st position
  if (!d->parametersAndPositions.isEmpty()) 
    return d->parametersAndPositions;
  
  typedef std::pair<xAOD::ParameterPosition, Amg::Vector3D> paramAndPos;
  
  const Trk::Perigee& peri = d->muon->primaryTrackParticle()->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
  d->parametersAndPositions.append(paramAndPos(xAOD::BeamLine, Amg::Vector3D(peri.position().x(),peri.position().y(),peri.position().z()) ) );

  float x,y,z;
  for (unsigned int i=0; i<d->muon->primaryTrackParticle()->numberOfParameters() ; ++i){
    x = d->muon->primaryTrackParticle()->parameterX(i);
    y = d->muon->primaryTrackParticle()->parameterY(i);
    z = d->muon->primaryTrackParticle()->parameterZ(i);
  
    d->parametersAndPositions.append(paramAndPos(d->muon->primaryTrackParticle()->parameterPosition(i), Amg::Vector3D(x,y,z)));
  }
  return d->parametersAndPositions;
}
