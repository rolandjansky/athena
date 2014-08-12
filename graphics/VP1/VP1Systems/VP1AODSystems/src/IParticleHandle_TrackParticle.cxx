/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleHandle_TrackParticle         //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

// Local
#include "IParticleHandle_TrackParticle.h"
#include "IParticleCollHandle_TrackParticle.h"
#include "AODSysCommonData.h"
#include "TrackParticleCollectionSettingsButton.h"

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
class IParticleHandle_TrackParticle::Imp {
public:
  IParticleHandle_TrackParticle * theclass;
  const xAOD::TrackParticle * trackparticle;
  SoSeparator * sep; // everything hangs from this.
  SoLineSet * line;//This represents the line(s) representing the trackparticle. Can be interpolated.
  QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parametersAndPositions; // cache 
};

//____________________________________________________________________
IParticleHandle_TrackParticle::IParticleHandle_TrackParticle(IParticleCollHandleBase* ch, const xAOD::TrackParticle *tp)
  : IParticleHandleBase(ch), d(new Imp)
{ 
  d->theclass = this;
  d->trackparticle = tp;
  d->sep=0;
  d->line=0;
}

//____________________________________________________________________
IParticleHandle_TrackParticle::~IParticleHandle_TrackParticle()
{
  if (d->line) d->line->unref();
  if (d->sep) d->sep->unref();
  delete d;
}

bool IParticleHandle_TrackParticle::has3DObjects(){
  return 0!=d->sep;
}
void IParticleHandle_TrackParticle::clear3DObjects(){
  VP1Msg::messageVerbose("IParticleHandle_TrackParticle::clear3DObjects()");
  
  if (d->line) {
    d->line->unref();
    d->line=0;
  }
  if (d->sep) {
    d->sep->unref();
    d->sep=0;
  }
}

SoNode* IParticleHandle_TrackParticle::nodes(){
  // VP1Msg::messageVerbose("IParticleHandle_TrackParticle::nodes with d->sep="+VP1Msg::str(d->sep));

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

  const Trk::Perigee& peri = d->trackparticle->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...

  // std::cout<<peri.position()<<std::endl;
  // std::cout<<p4.X()<<", "<<p4.Y()<<", "<<p4.Z()<<std::endl;

  const IParticleCollHandle_TrackParticle* collHandleTrk = dynamic_cast<const IParticleCollHandle_TrackParticle*>(collHandle());
  // FIXME - this ugly...

  // Points:
  SoPointSet* points = 0;
  SoVertexProperty * vertices2 = 0;

  if (collHandleTrk->collSettingsButton().showParameters()){ 
    // std::cout<<"showing parameters... have "<<d->trackparticle->numberOfParameters()+1<<" to draw"<<std::endl;

        // Okay. so let's build them...
    points = new SoPointSet;
    vertices2 = new SoVertexProperty;
  }
  float x,y,z;      
  unsigned int iMinusOne=0;

  for (unsigned int i=0; i<d->trackparticle->numberOfParameters()+1 ; ++i){
      // std::cout<<"i:"<<i<<"/"<<d->trackparticle->numberOfParameters()+1<<std::endl;
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
      x = d->trackparticle->parameterX(iMinusOne);
      y = d->trackparticle->parameterY(iMinusOne);
      z = d->trackparticle->parameterZ(iMinusOne);  
      momenta.push_back(Amg::Vector3D(d->trackparticle->parameterPX(iMinusOne),
        d->trackparticle->parameterPY(iMinusOne),
        d->trackparticle->parameterPZ(iMinusOne)));      
    }
    positions.push_back(Amg::Vector3D(x,y,z));

    if (points) {
      if (collHandleTrk->collSettingsButton().colourParametersByType()) {
        // Okay, so we treat each point separately & add materials as we go...
        if (0==i) { // Going to assume first is beamline. 
          d->sep->addChild(collHandleTrk->collSettingsButton().parameterMaterial(xAOD::BeamLine));          
        } else {
          d->sep->addChild(collHandleTrk->collSettingsButton().parameterMaterial(d->trackparticle->parameterPosition(i-1)));
        }
        vertices2->vertex.set1Value(0,x,y,z);
        points->numPoints=1;
        points->vertexProperty = vertices2;
        d->sep->addChild(points);
        points = new SoPointSet; // reset for next point
        vertices2 = new SoVertexProperty;
      } else { // Not colouring by type
        vertices2->vertex.set1Value(iver++,x,y,z);
      } 
    } // if points
  } // end of loop.

  if (points) {
    if (!collHandleTrk->collSettingsButton().colourParametersByType()){
      // should have accumulated a bunch of points by now!
      points->numPoints=iver;
      points->vertexProperty = vertices2;
      d->sep->addChild(static_cast<IParticleCollHandle_TrackParticle*>(collHandle())->collSettingsButton().defaultParameterMaterial());
      d->sep->addChild(points);
    } else {
      // Avoid mem-leaks
      points->unref();
      vertices2->unref();
    }
  }

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
QStringList IParticleHandle_TrackParticle::clicked() const
{
  QStringList l;
  l << "TrackParticle:";
  l << IParticleHandleBase::baseInfo();
  return l;
}

//____________________________________________________________________
Amg::Vector3D IParticleHandle_TrackParticle::momentum() const
{
  const Trk::Perigee& p = d->trackparticle->perigeeParameters();
  return p.momentum();
}

//____________________________________________________________________
const xAOD::IParticle& IParticleHandle_TrackParticle::iParticle() const
{
  return *(d->trackparticle);
}

//____________________________________________________________________
double IParticleHandle_TrackParticle::charge() const
{
  return d->trackparticle->charge();
}

//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::summaryValue(xAOD::SummaryType type) const
{ 
  uint8_t num = 0;
  if (d->trackparticle->summaryValue(num,type)){
    return num;
  }
  // else...
  VP1Msg::message("IParticleHandle_TrackParticle::getSummaryValue - unable to retrieve the requested enum: "+VP1Msg::str(type));
  return 999999;
}
\
//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::getNPixelHits() const
{ 
  return summaryValue(xAOD::numberOfPixelHits);
}

//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::getNSCTHits() const
{ 
  return summaryValue(xAOD::numberOfSCTHits);
}
//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::getNTRTHits() const
{ 
  return summaryValue(xAOD::numberOfTRTHits);
}
//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::getNMuonPrecisionLayers() const
{ 
  return summaryValue(xAOD::numberOfPrecisionLayers);
}
//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::getNMuonPrecisionHoleLayers() const
{ 
  return summaryValue(xAOD::numberOfPrecisionHoleLayers);
}
//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::getNMuonPhiLayers() const
{ 
  return summaryValue(xAOD::numberOfPhiLayers);

}//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::getNMuonPhiHoleLayers() const
{ 
  return summaryValue(xAOD::numberOfPhiHoleLayers);
}

QString IParticleHandle_TrackParticle::shortInfo() const
{
  QString l("|P|="+VP1Msg::str(momentum().mag()/CLHEP::GeV)+" [GeV], ");
  l+= "Pix["+QString::number(getNPixelHits())+"], SCT["+QString::number(getNSCTHits())+"], TRT["+QString::number(getNTRTHits())
   +"], Muon prec. layers/holes ["+QString::number(getNMuonPrecisionLayers())+"/"+QString::number(getNMuonPrecisionHoleLayers())+"]";
  return l;
}

void IParticleHandle_TrackParticle::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) 
{
  IParticleHandleBase::fillObjectBrowser(listOfItems); // Obligatory!

  QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());
  TSOSitem->setText(0, QString("Def. Parameters " ) );
  QString dParameters("(");
  dParameters+=QString::number(d->trackparticle->d0());
  dParameters+=", ";     
  dParameters+=QString::number(d->trackparticle->z0());
  dParameters+=", ";     
  dParameters+=QString::number(d->trackparticle->phi0());
  dParameters+=", ";     
  dParameters+=QString::number(d->trackparticle->theta());
  dParameters+=", ";     
  dParameters+=QString::number(d->trackparticle->qOverP());
  dParameters+=")";     
  TSOSitem->setText(1, dParameters );
  
  for (unsigned int i=0; i<d->trackparticle->numberOfParameters() ; ++i){
    QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());
    TSOSitem->setText(0, QString("Parameter "+QString::number( i+1 ) ) );
    QString pos(", Position = (");  
    pos+=QString::number(d->trackparticle->parameterX(i));
    pos+=", ";    
    pos+=QString::number(d->trackparticle->parameterY(i));
    pos+=", ";
    pos+=QString::number(d->trackparticle->parameterZ(i));
    pos+=")";
    
    switch (d->trackparticle->parameterPosition(i)){
      case xAOD::BeamLine: 
        TSOSitem->setText(1, QString("BeamLine" )+pos );
        break;
      case xAOD::FirstMeasurement:
        TSOSitem->setText(1, QString("FirstMeasurement")+pos );
        break;
      case xAOD::LastMeasurement: 
        TSOSitem->setText(1, QString("LastMeasurement" )+pos );
        break;
      case xAOD::CalorimeterEntrance:
        TSOSitem->setText(1, QString("CalorimeterEntrance")+pos );
        break;
      case xAOD::CalorimeterExit: 
        TSOSitem->setText(1, QString("CalorimeterExit" )+pos );
        break;
      case xAOD::MuonSpectrometerEntrance:
        TSOSitem->setText(1, QString("MuonSpectrometerEntrance")+pos );
        break;
      default:
        TSOSitem->setText(1, QString("Undefined")+pos );
    }
  }
  
  // TODO - add more.
}

const QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> >& IParticleHandle_TrackParticle::parametersAndPositions() const {
  // 1st position
  if (!d->parametersAndPositions.isEmpty()) 
    return d->parametersAndPositions;
  
  typedef std::pair<xAOD::ParameterPosition, Amg::Vector3D> paramAndPos;
  
  const Trk::Perigee& peri = d->trackparticle->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
  d->parametersAndPositions.append(paramAndPos(xAOD::BeamLine, Amg::Vector3D(peri.position().x(),peri.position().y(),peri.position().z()) ) );

  float x,y,z;
  for (unsigned int i=0; i<d->trackparticle->numberOfParameters() ; ++i){
    x = d->trackparticle->parameterX(i);
    y = d->trackparticle->parameterY(i);
    z = d->trackparticle->parameterZ(i);
  
    d->parametersAndPositions.append(paramAndPos(d->trackparticle->parameterPosition(i), Amg::Vector3D(x,y,z)));
  }
  return d->parametersAndPositions;
}
