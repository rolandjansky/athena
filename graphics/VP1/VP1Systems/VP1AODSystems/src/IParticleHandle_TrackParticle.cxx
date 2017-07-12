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
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>

//Athena
#include "TrkExInterfaces/IExtrapolationEngine.h"

//____________________________________________________________________
class IParticleHandle_TrackParticle::Imp {
public:
  IParticleHandle_TrackParticle * theclass;
  const xAOD::TrackParticle * trackparticle;
  IParticleCollHandle_TrackParticle* collHandle;
  
  SoSeparator * sep; // everything hangs from this.
  SoLineSet * line;//This represents the line(s) representing the trackparticle. Can be interpolated.
  QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parametersAndPositions; // cache - do we really need this?
  QList< Amg::Vector3D > positionsToWrite; // FIXME - this is just a quick hack so we can easily dump the points used in the lines to JSON 
  
};

//____________________________________________________________________
IParticleHandle_TrackParticle::IParticleHandle_TrackParticle(IParticleCollHandleBase* ch, const xAOD::TrackParticle *tp)
  : IParticleHandleBase(ch), d(new Imp)
{ 
  d->theclass = this;
  d->trackparticle = tp;
  d->collHandle = dynamic_cast<IParticleCollHandle_TrackParticle*>(ch);
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
  
  // Wipe caches
  d->positionsToWrite.clear();
  d->parametersAndPositions.clear();
}

SoNode* IParticleHandle_TrackParticle::nodes(){
  VP1Msg::messageVerbose("IParticleHandle_TrackParticle::nodes with d->sep="+VP1Msg::str(d->sep));

  if (d->sep) {
    VP1Msg::messageVerbose("IParticleHandle_TrackParticle::nodes() - already have sep so not doing anything.");
    return d->sep; 
  }
  if (!d->sep) {
    d->sep = new SoSeparator();
    d->sep->ref();
  }

  bool fromTrack=d->collHandle->collSettingsButton().useExistingParameters();
  
  if (fromTrack) 
    addLine_FromTrackParticle();
  else
    addLine_Extrapolated();

  if (d->collHandle->collSettingsButton().showParameters()) 
      addParameterShapes();  

  return d->sep;
}

void IParticleHandle_TrackParticle::addParameterShapes(){
  // Points:
  SoPointSet* parameterPoints = 0;
  SoVertexProperty * vertices2 = 0;

  // std::cout<<"showing parameters... have "<<d->trackparticle->numberOfParameters()+1<<" to draw"<<std::endl;
  
      // Okay. so let's make the containers to hold them...
  parameterPoints = new SoPointSet;
  vertices2 = new SoVertexProperty;

  int i = 0;
  bool colourByParamType = d->collHandle->collSettingsButton().colourParametersByType();
  QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parAndPoses = parametersAndPositions();
  
  std::pair<xAOD::ParameterPosition, Amg::Vector3D> parpos;
  foreach (parpos, parAndPoses ) {
    Amg::Vector3D pos = parpos.second;

    if (colourByParamType) {
      // Okay, so we treat each point separately & add materials as we go...
      d->sep->addChild(d->collHandle->collSettingsButton().parameterMaterial(parpos.first));
      vertices2->vertex.set1Value(0,pos.x(),pos.y(),pos.z());
      parameterPoints->numPoints=1;
      parameterPoints->vertexProperty = vertices2;
      d->sep->addChild(parameterPoints);
      parameterPoints = new SoPointSet; // reset for next point
      vertices2 = new SoVertexProperty;
    } else { // Not colouring by type
      vertices2->vertex.set1Value(i++,pos.x(),pos.y(),pos.z());
    } 
  }
 
  if (colourByParamType) {
    // In this case, we don't need the last Nodes created in the loop.
    parameterPoints->unref();
    vertices2->unref();
  
  } else {  
    parameterPoints->numPoints=i;
    parameterPoints->vertexProperty = vertices2;
    d->sep->addChild(static_cast<IParticleCollHandle_TrackParticle*>(collHandle())->collSettingsButton().defaultParameterMaterial());
    d->sep->addChild(parameterPoints);
  }
}

void IParticleHandle_TrackParticle::addLine_FromTrackParticle(){

  std::vector<Amg::Vector3D> positions, momenta;


  const Trk::Perigee& peri = d->trackparticle->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
 
  positions.push_back(Amg::Vector3D(peri.position().x(),peri.position().y(),peri.position().z()));
  momenta.  push_back(Amg::Vector3D(peri.momentum().x(),peri.momentum().y(),peri.momentum().z()));
  // std::cout<<"i:"<<0<<"/"<<d->trackparticle->numberOfParameters()+1<<": ("<<peri.position().x()<<","<<peri.position().y()<<","<<peri.position().z()<<")"<<std::endl;
  
  VP1Msg::messageVerbose("IParticleHandle_TrackParticle::addLine_FromTrackParticle - has "+QString::number(d->trackparticle->numberOfParameters())+" extra parameters.");
   for (unsigned int i=0; i<d->trackparticle->numberOfParameters() ; ++i){
    // std::cout<<"i:"<<i+1<<"/"<<d->trackparticle->numberOfParameters()+1<<": ("<<d->trackparticle->parameterX(i)<<","<<d->trackparticle->parameterY(i)<<","<<d->trackparticle->parameterZ(i)<<")"<<std::endl;

    positions.push_back(Amg::Vector3D(d->trackparticle->parameterX(i),
                                      d->trackparticle->parameterY(i),
                                      d->trackparticle->parameterZ(i)));      
    momenta.push_back(Amg::Vector3D(  d->trackparticle->parameterPX(i),
                                      d->trackparticle->parameterPY(i),
                                      d->trackparticle->parameterPZ(i)));      
  } // end of loop.

  // if ( positions.size()<2 ) VP1Msg::messageVerbose("IParticleHandle_TrackParticle::addLine_FromTrackParticle - WARNING - not enough points to make a line.");

  fillLineFromSplineFit(positions, momenta);

  // std::cout<<"About to add "<<collHandleTrk->material()<<std::endl;
  d->sep->addChild(d->collHandle->material()); 
  // ^^ FIXME - should rearrange so we don't need to reset material

  d->sep->addChild(d->line);
}

void IParticleHandle_TrackParticle::linear(Amg::Vector3D& output, Amg::Vector3D& a, Amg::Vector3D& b, float t){
  float x = a.x() + (b.x()-a.x())*t;
  float y = a.y() + (b.y()-a.y())*t;
  float z = a.z() + (b.z()-a.z())*t;
  output = Amg::Vector3D(x,y,z);
}

void IParticleHandle_TrackParticle::bezier(Amg::Vector3D& output, Amg::Vector3D& a, Amg::Vector3D& b, Amg::Vector3D& c, Amg::Vector3D& d, float t){
  // DeCasteljau subdivision
  Amg::Vector3D ab,bc,cd,abbc,bccd;
  linear(ab, a,b,t);           // point between a and b (green)
  linear(bc, b,c,t);           // point between b and c (green)
  linear(cd, c,d,t);           // point between c and d (green)
  linear(abbc, ab,bc,t);       // point between ab and bc (blue)
  linear(bccd, bc,cd,t);       // point between bc and cd (blue)
  linear(output, abbc,bccd,t);   // point on the bezier-curve (black)
}

void IParticleHandle_TrackParticle::fillLineFromSplineFit(const std::vector<Amg::Vector3D>& positions, const std::vector<Amg::Vector3D>&momenta){
  int iver(0);
  unsigned int npointsused(0);
  // Create new lineset etc
  d->line = new SoLineSet();
  d->line->ref();
  
  // For the moment, lets just draw the lines.
  SoVertexProperty * vertices = new SoVertexProperty();
  
  //  std::cout<<"About to loop over "<<positions.size()<<" points to make line"<<std::endl;
  // for (unsigned int i=0; i<positions.size(); ++i){
  //   std::cout<<"i: "<<i<<" at ("<<positions[i].x()<<","<<positions[i].y()<<","<<positions[i].z()<<")"<<" \tmom = ("<<momenta[i].x()<<","<<momenta[i].y()<<","<<momenta[i].z()<<")"<<std::endl;
  // }

  // std::cout<<"Now do interpolations."<<std::endl;

  bool useExistingParams = d->collHandle->collSettingsButton().useExistingParameters();

  for (unsigned int i=1; i<positions.size(); ++i){
    // std::cout<<"i: "<<i<<" at ("<<positions[i].x()<<","<<positions[i].y()<<","<<positions[i].z()<<")"<<" \tmom = ("<<momenta[i].x()<<","<<momenta[i].y()<<","<<momenta[i].z()<<")"<<std::endl;
    
    if (useExistingParams){ 
      float scale = ( (positions[i]-positions[i-1]).mag() )/3.0; // want to split curve into three parts
      Amg::Vector3D p0(positions[i-1]); // first position
      Amg::Vector3D p1(positions[i-1]+(scale * momenta[i-1].unit() ) ); // move 1/3 or the way towards next point, but following momentum direction
      Amg::Vector3D p2(positions[i]  -(scale * momenta[i].unit() ) ); // back 1/3 from second point, but following momentum direction
      Amg::Vector3D p3(positions[i]);
      // std::cout<<"p0: ("<<p0[0]<<", "<<p0[1]<<", "<<p0[2]<<") "<<std::endl;
      // std::cout<<"p1: ("<<p1[0]<<", "<<p1[1]<<", "<<p1[2]<<") "<<std::endl;
      // std::cout<<"p2: ("<<p2[0]<<", "<<p2[1]<<", "<<p2[2]<<") "<<std::endl;
      // std::cout<<"p3: ("<<p3[0]<<", "<<p3[1]<<", "<<p3[2]<<") "<<std::endl;
    
      unsigned int maxCount=d->collHandle->collSettingsButton().numOfStepsForInterpolation();
      for (unsigned int count=1; count<=maxCount;++count){
        float t=static_cast<float>(count)/static_cast<float>(maxCount);
        Amg::Vector3D pos;
        bezier(pos, p0, p1, p2, p3, t);
        // std::cout<<"j: "<<count<<" \tat ("<<pos.x()<<","<<pos.y()<<","<<pos.z()<<")"<<std::endl;
      
        // pos = pos + momdelta;
        vertices->vertex.set1Value(iver++,pos.x(),pos.y(),pos.z());
        d->positionsToWrite.append(pos);
      
        npointsused++;        
      }
    } else {
      vertices->vertex.set1Value(iver++,positions[i].x(),positions[i].y(),positions[i].z());
      d->positionsToWrite.append(positions[i]);
      npointsused++;        
    }
  }
  
  // Add final point.
  
  
    
  // Add to SoLine set
  d->line->numVertices.set1Value(0,npointsused);
  d->line->vertexProperty = vertices;
  
}

void IParticleHandle_TrackParticle::addLine_Extrapolated(){
  VP1Msg::messageVerbose("IParticleHandle_TrackParticle::addLine_Extrapolated().");
  const Trk::Perigee& peri = d->trackparticle->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
  
  Trk::CurvilinearParameters startParameters(peri.position(),peri.momentum(),peri.charge());
  Trk::ExtrapolationCell<Trk::TrackParameters> ecc(startParameters);
  // ecc.addConfigurationMode(Trk::ExtrapolationMode::StopAtBoundary);
  ecc.addConfigurationMode(Trk::ExtrapolationMode::CollectPassive);
  ecc.addConfigurationMode(Trk::ExtrapolationMode::CollectSensitive);
  
  // we want to extrapolate outwards only for muon tracks
  if (getNMuonPrecisionLayers()==0 &&  getNMuonPhiLayers()==0 )
    ecc.addConfigurationMode(Trk::ExtrapolationMode::StopAtBoundary);
  
  // call the extrapolation engine
  Trk::IExtrapolationEngine * engine = collHandle()->common()->extrapolator();
  if (!engine) {
     VP1Msg::messageVerbose("No extrapolator engine available. Aborting.");
     return;
  }
  
  std::vector<Amg::Vector3D> positions; 
  std::vector<Amg::Vector3D> momenta;
  
  // Add start positions
  positions.push_back(Amg::Vector3D(peri.position().x(),peri.position().y(),peri.position().z()));
  momenta.  push_back(Amg::Vector3D(peri.momentum().x(),peri.momentum().y(),peri.momentum().z()));
  
  Trk::ExtrapolationCode eCode = engine->extrapolate(ecc);
  
  // end the parameters if there
  if (eCode.isSuccess()){
    // loop over the collected information
    for (auto& es : ecc.extrapolationSteps){
      
      // continue if we have parameters
      const Trk::TrackParameters* parameters = es.parameters;
      if (parameters){
        // take what you  need -> position etc

        // finally delete the parameters
        Amg::Vector3D pos = parameters->position();
        Amg::Vector3D mom = parameters->momentum();
        positions.push_back(pos);
        momenta.push_back(mom);
        
        delete parameters;
      }
    }//
    
    Amg::Vector3D pos = ecc.endParameters->position();
    Amg::Vector3D mom = ecc.endParameters->momentum();
    positions.push_back(pos);
    momenta.push_back(mom);
    // and delete the final parameter
    delete ecc.endParameters;
  } else {
    VP1Msg::messageVerbose("IParticleHandle_TrackParticle::addLine_Extrapolated() - extrapolation failed.");
  }
  fillLineFromSplineFit(positions, momenta);
  // std::cout<<"About to add "<<collHandleTrk->material()<<std::endl;
  d->sep->addChild(d->collHandle->material()); 
  // ^^ FIXME - should rearrange so we don't need to reset material


  d->sep->addChild(d->line);
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
  VP1Msg::messageVerbose("TrackParticle parametersAndPositions() - returning this many positions : "+VP1Msg::str(d->parametersAndPositions.size()) );
  return d->parametersAndPositions;
}

void IParticleHandle_TrackParticle::dumpToJSON( std::ofstream& str) const {
  str << "\"chi2\":"<<d->trackparticle->chiSquared () <<", ";
  str << "\"dof\":" <<d->trackparticle->numberDoF () <<", ";
  str << "\"dparams\": [" 
      << d->trackparticle->d0() <<", "
      << d->trackparticle->z0() <<", "
      << d->trackparticle->phi0() <<", "
      << d->trackparticle->theta() <<", "
      << d->trackparticle->qOverP()
      << "], \"pos\": [";
  bool first = true;
  for (auto pos: d->positionsToWrite){
    if (first){
      first = false;
    } else {
      str <<",";
    }
    str << "["<<pos.x()<<","<<pos.y()<<","<<pos.z()<<"]";
  }
  str << "] ";
}
