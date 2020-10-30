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

#ifndef BUILDVP1LIGHT
	#include "TrkExInterfaces/IExtrapolationEngine.h"
#endif

// System of units
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::cm'
#else
  #include "GaudiKernel/SystemOfUnits.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::cm'
#endif


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
  : IParticleHandleBase(ch), m_d(new Imp)
{ 
  m_d->theclass = this;
  m_d->trackparticle = tp;
  m_d->collHandle = dynamic_cast<IParticleCollHandle_TrackParticle*>(ch);
  m_d->sep=0;
  m_d->line=0;
}

//____________________________________________________________________
IParticleHandle_TrackParticle::~IParticleHandle_TrackParticle()
{
  if (m_d->line) m_d->line->unref();
  if (m_d->sep) m_d->sep->unref();
  delete m_d;
}

bool IParticleHandle_TrackParticle::has3DObjects(){
  return 0!=m_d->sep;
}
void IParticleHandle_TrackParticle::clear3DObjects(){
  VP1Msg::messageVerbose("IParticleHandle_TrackParticle::clear3DObjects()");
  
  if (m_d->line) {
    m_d->line->unref();
    m_d->line=0;
  }
  if (m_d->sep) {
    m_d->sep->unref();
    m_d->sep=0;
  }
  
  // Wipe caches
  m_d->positionsToWrite.clear();
  m_d->parametersAndPositions.clear();
}

SoNode* IParticleHandle_TrackParticle::nodes(){
  VP1Msg::messageVerbose("IParticleHandle_TrackParticle::nodes with d->sep="+VP1Msg::str(m_d->sep));

  if (m_d->sep) {
    VP1Msg::messageVerbose("IParticleHandle_TrackParticle::nodes() - already have sep so not doing anything.");
    return m_d->sep; 
  }
  if (!m_d->sep) {
    m_d->sep = new SoSeparator();
    m_d->sep->ref();
  }

  bool fromTrack=m_d->collHandle->collSettingsButton().useExistingParameters();
  
  if (fromTrack) {
    addLine_FromTrackParticle();
  } else {
    #ifndef BUILDVP1LIGHT
      addLine_Extrapolated();
    #endif // BUILDVP1LIGHT
  }

  if (m_d->collHandle->collSettingsButton().showParameters()) 
      addParameterShapes();  

  return m_d->sep;
}

void IParticleHandle_TrackParticle::addParameterShapes(){
  // Points:
  SoPointSet* parameterPoints = 0;
  SoVertexProperty * vertices2 = 0;

  // std::cout<<"showing parameters... have "<<m_d->trackparticle->numberOfParameters()+1<<" to draw"<<std::endl;
  
      // Okay. so let's make the containers to hold them...
  parameterPoints = new SoPointSet;
  vertices2 = new SoVertexProperty;

  int i = 0;
  bool colourByParamType = m_d->collHandle->collSettingsButton().colourParametersByType();
  QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parAndPoses = parametersAndPositions();
  
  std::pair<xAOD::ParameterPosition, Amg::Vector3D> parpos;
  foreach (parpos, parAndPoses ) {
    Amg::Vector3D pos = parpos.second;

    if (colourByParamType) {
      // Okay, so we treat each point separately & add materials as we go...
      m_d->sep->addChild(m_d->collHandle->collSettingsButton().parameterMaterial(parpos.first));
      vertices2->vertex.set1Value(0,pos.x(),pos.y(),pos.z());
      parameterPoints->numPoints=1;
      parameterPoints->vertexProperty = vertices2;
      m_d->sep->addChild(parameterPoints);
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
    m_d->sep->addChild(static_cast<IParticleCollHandle_TrackParticle*>(collHandle())->collSettingsButton().defaultParameterMaterial());
    m_d->sep->addChild(parameterPoints);
  }
}

void IParticleHandle_TrackParticle::addLine_FromTrackParticle(){

  std::vector<Amg::Vector3D> positions, momenta;

#if defined BUILDVP1LIGHT
  positions.push_back(position());
  momenta.  push_back(momentum());
#else
  const Trk::Perigee& peri = m_d->trackparticle->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
 
  positions.push_back(Amg::Vector3D(peri.position().x(),peri.position().y(),peri.position().z()));
  momenta.  push_back(Amg::Vector3D(peri.momentum().x(),peri.momentum().y(),peri.momentum().z()));
  // std::cout<<"i:"<<0<<"/"<<m_d->trackparticle->numberOfParameters()+1<<": ("<<peri.position().x()<<","<<peri.position().y()<<","<<peri.position().z()<<")"<<std::endl;
#endif // BUILDVP1LIGHT


  VP1Msg::messageVerbose("IParticleHandle_TrackParticle::addLine_FromTrackParticle - has "+QString::number(m_d->trackparticle->numberOfParameters())+" extra parameters.");
   for (unsigned int i=0; i<m_d->trackparticle->numberOfParameters() ; ++i){
    // std::cout<<"i:"<<i+1<<"/"<<m_d->trackparticle->numberOfParameters()+1<<": ("<<m_d->trackparticle->parameterX(i)<<","<<m_d->trackparticle->parameterY(i)<<","<<m_d->trackparticle->parameterZ(i)<<")"<<std::endl;

    positions.push_back(Amg::Vector3D(m_d->trackparticle->parameterX(i),
                                      m_d->trackparticle->parameterY(i),
                                      m_d->trackparticle->parameterZ(i)));      
    momenta.push_back(Amg::Vector3D(  m_d->trackparticle->parameterPX(i),
                                      m_d->trackparticle->parameterPY(i),
                                      m_d->trackparticle->parameterPZ(i)));      

  } // end of loop.

  // if ( positions.size()<2 ) VP1Msg::messageVerbose("IParticleHandle_TrackParticle::addLine_FromTrackParticle - WARNING - not enough points to make a line.");

  fillLineFromSplineFit(positions, momenta);

  // std::cout<<"About to add "<<collHandleTrk->material()<<std::endl;
  m_d->sep->addChild(m_d->collHandle->material()); 
  // ^^ FIXME - should rearrange so we don't need to reset material

  m_d->sep->addChild(m_d->line);
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
  m_d->line = new SoLineSet();
  m_d->line->ref();
  
  // For the moment, lets just draw the lines.
  SoVertexProperty * vertices = new SoVertexProperty();
  
  //  std::cout<<"About to loop over "<<positions.size()<<" points to make line"<<std::endl;
  // for (unsigned int i=0; i<positions.size(); ++i){
  //   std::cout<<"i: "<<i<<" at ("<<positions[i].x()<<","<<positions[i].y()<<","<<positions[i].z()<<")"<<" \tmom = ("<<momenta[i].x()<<","<<momenta[i].y()<<","<<momenta[i].z()<<")"<<std::endl;
  // }

  // std::cout<<"Now do interpolations."<<std::endl;

  bool useExistingParams = m_d->collHandle->collSettingsButton().useExistingParameters();

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
    
      unsigned int maxCount=m_d->collHandle->collSettingsButton().numOfStepsForInterpolation();
      for (unsigned int count=1; count<=maxCount;++count){
        float t=static_cast<float>(count)/static_cast<float>(maxCount);
        Amg::Vector3D pos;
        bezier(pos, p0, p1, p2, p3, t);
        // std::cout<<"j: "<<count<<" \tat ("<<pos.x()<<","<<pos.y()<<","<<pos.z()<<")"<<std::endl;
      
        // pos = pos + momdelta;
        vertices->vertex.set1Value(iver++,pos.x(),pos.y(),pos.z());
        m_d->positionsToWrite.append(pos);
      
        npointsused++;        
      }
    } else {
      vertices->vertex.set1Value(iver++,positions[i].x(),positions[i].y(),positions[i].z());
      m_d->positionsToWrite.append(positions[i]);
      npointsused++;        
    }
  }
  
  // Add final point.
  
  
    
  // Add to SoLine set
  m_d->line->numVertices.set1Value(0,npointsused);
  m_d->line->vertexProperty = vertices;
  
}

#ifndef BUILDVP1LIGHT
void IParticleHandle_TrackParticle::addLine_Extrapolated(){
  VP1Msg::messageVerbose("IParticleHandle_TrackParticle::addLine_Extrapolated().");

  const Trk::Perigee& peri = m_d->trackparticle->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
  
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
  m_d->sep->addChild(m_d->collHandle->material()); 
  // ^^ FIXME - should rearrange so we don't need to reset material


  m_d->sep->addChild(m_d->line);
}
#endif // BUILDVP1LIGHT

//____________________________________________________________________
QStringList IParticleHandle_TrackParticle::clicked() const
{
  QStringList l;
  l << "TrackParticle:";
  l << IParticleHandleBase::baseInfo();
  return l;
}

//____________________________________________________________________
#if defined BUILDVP1LIGHT
  Amg::Vector3D IParticleHandle_TrackParticle::momentum() const
  {
    double phi = m_d->trackparticle->phi0();
    double theta = m_d->trackparticle->theta();

    // decide the sign of the charge
    double qop = m_d->trackparticle->qOverP();
    // if(qop < 0.) 
    //   m_chargeDef->setCharge(-1.);
    // else
    //   m_chargeDef->setCharge(1.);
    static constexpr double INVALID_P(10e9);
    static constexpr double INVALID_QOP(10e-9);

    // check qoverp is physical
    double p = 0.;
    if(qop != 0.)
      p = fabs(1./qop);
    else
    {
      // qop is unphysical. No momentum measurement.
      p = INVALID_P;
      qop = INVALID_QOP;
    }

    const Amg::Vector3D mom = Amg::Vector3D(p*cos(phi)*sin(theta), p*sin(phi)*sin(theta), p*cos(theta));

    return mom;
  }
#else
  Amg::Vector3D IParticleHandle_TrackParticle::momentum() const
  {
    const Trk::Perigee& p = m_d->trackparticle->perigeeParameters();
    return p.momentum();
  }
#endif // BUILDVP1LIGHT

//____________________________________________________________________
// TODO: probably, we can move this function to the common base class for all trackparticles
#if defined BUILDVP1LIGHT
  Amg::Vector3D IParticleHandle_TrackParticle::position() const
  {
    double d0 = m_d->trackparticle->d0();
    double z0 = m_d->trackparticle->z0();
    double phi = m_d->trackparticle->phi0();

    // glopos[Amg::x] = - locpos[Trk::d0]*sin(phi);
    // glopos[Amg::y] =   locpos[Trk::d0]*cos(phi);
    // glopos[Amg::z] =   locpos[Trk::z0];


    const Amg::Vector3D pos = Amg::Vector3D(- d0*sin(phi), d0*cos(phi), z0);

    return pos;
  }
#else
    Amg::Vector3D IParticleHandle_TrackParticle::position() const
  {
    const Trk::Perigee& p = m_d->trackparticle->perigeeParameters();
    return p.position(); // TODO: check what this returns
  }
#endif


//____________________________________________________________________
const xAOD::IParticle& IParticleHandle_TrackParticle::iParticle() const
{
  return *(m_d->trackparticle);
}

//____________________________________________________________________
double IParticleHandle_TrackParticle::charge() const
{
  return m_d->trackparticle->charge();
}

//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::summaryValue(xAOD::SummaryType type) const
{
  uint8_t num = 0;
  if (m_d->trackparticle->summaryValue(num,type)){
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
}
//____________________________________________________________________
unsigned IParticleHandle_TrackParticle::getNMuonPhiHoleLayers() const
{ 
  return summaryValue(xAOD::numberOfPhiHoleLayers);
}

QString IParticleHandle_TrackParticle::shortInfo() const
{
  QString l("");
  //l+= "|P|="+VP1Msg::str(momentum().mag()/SYSTEM_OF_UNITS::GeV)+" [GeV], ";
  l+= "Pix["+QString::number(getNPixelHits())+"], ";
  l+= "SCT["+QString::number(getNSCTHits())+"], " ;
  l+= "TRT["+QString::number(getNTRTHits())+"], ";
  l+= "Muon prec. layers/holes [";
  l+= QString::number(getNMuonPrecisionLayers())+"/"+QString::number(getNMuonPrecisionHoleLayers())+"]";
  return l;
}

void IParticleHandle_TrackParticle::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) 
{
  IParticleHandleBase::fillObjectBrowser(listOfItems); // Obligatory!

  QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());
  TSOSitem->setText(0, QString("Def. Parameters " ) );
  QString dParameters("(");
  dParameters+=QString::number(m_d->trackparticle->d0());
  dParameters+=", ";     
  dParameters+=QString::number(m_d->trackparticle->z0());
  dParameters+=", ";     
  dParameters+=QString::number(m_d->trackparticle->phi0());
  dParameters+=", ";     
  dParameters+=QString::number(m_d->trackparticle->theta());
  dParameters+=", ";     
  dParameters+=QString::number(m_d->trackparticle->qOverP());
  dParameters+=")";     
  TSOSitem->setText(1, dParameters );
  
  for (unsigned int i=0; i<m_d->trackparticle->numberOfParameters() ; ++i){
    QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());
    TSOSitem->setText(0, QString("Parameter "+QString::number( i+1 ) ) );
    QString pos(", Position = (");  
    pos+=QString::number(m_d->trackparticle->parameterX(i));
    pos+=", ";    
    pos+=QString::number(m_d->trackparticle->parameterY(i));
    pos+=", ";
    pos+=QString::number(m_d->trackparticle->parameterZ(i));
    pos+=")";
    
    switch (m_d->trackparticle->parameterPosition(i)){
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
  if (!m_d->parametersAndPositions.isEmpty()) 
    return m_d->parametersAndPositions;
  
  typedef std::pair<xAOD::ParameterPosition, Amg::Vector3D> paramAndPos;
  
#if defined BUILDVP1LIGHT
  m_d->parametersAndPositions.append(paramAndPos(xAOD::BeamLine, position() ) );
#else
  const Trk::Perigee& peri = m_d->trackparticle->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
  m_d->parametersAndPositions.append(paramAndPos(xAOD::BeamLine, Amg::Vector3D(peri.position().x(),peri.position().y(),peri.position().z()) ) );
#endif // BUILDVP1LIGHT

  float x,y,z;
  for (unsigned int i=0; i<m_d->trackparticle->numberOfParameters() ; ++i){
    x = m_d->trackparticle->parameterX(i);
    y = m_d->trackparticle->parameterY(i);
    z = m_d->trackparticle->parameterZ(i);
  
    m_d->parametersAndPositions.append(paramAndPos(m_d->trackparticle->parameterPosition(i), Amg::Vector3D(x,y,z)));
  }
  VP1Msg::messageVerbose("TrackParticle parametersAndPositions() - returning this many positions : "+VP1Msg::str(m_d->parametersAndPositions.size()) );
  return m_d->parametersAndPositions;
}

void IParticleHandle_TrackParticle::dumpToJSON( std::ofstream& str) const {
  str << "\"chi2\":"<<m_d->trackparticle->chiSquared () <<", ";
  str << "\"dof\":" <<m_d->trackparticle->numberDoF () <<", ";
  str << "\"dparams\": [" 
      << m_d->trackparticle->d0() <<", "
      << m_d->trackparticle->z0() <<", "
      << m_d->trackparticle->phi0() <<", "
      << m_d->trackparticle->theta() <<", "
      << m_d->trackparticle->qOverP()
      << "], \"pos\": [";
  bool first = true;
  for (auto pos: m_d->positionsToWrite){
    if (first){
      first = false;
    } else {
      str <<",";
    }
    str << "["<<pos.x()<<","<<pos.y()<<","<<pos.z()<<"]";
  }
  str << "] ";
}

