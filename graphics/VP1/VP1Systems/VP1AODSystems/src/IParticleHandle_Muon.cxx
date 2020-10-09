/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>

#ifdef BUILDVP1LIGHT
  #include "CLHEP/Units/SystemOfUnits.h"
#endif // BUILDVP1LIGHT

// System of units
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::cm'
#else
  #include "GaudiKernel/SystemOfUnits.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::cm'
#endif
//AOD

//____________________________________________________________________
class IParticleHandle_Muon::Imp {
public:
  IParticleHandle_Muon * theclass;
  const xAOD::Muon * muon;
  IParticleCollHandle_Muon* collHandle;
  
  SoSeparator * sep; // everything hangs from this.
  SoLineSet * line;//This represents the line(s) representing the trackparticle. Can be interpolated.
  QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parametersAndPositions; // cache 
};

//____________________________________________________________________
IParticleHandle_Muon::IParticleHandle_Muon(IParticleCollHandleBase* ch, const xAOD::Muon *tp)
  : IParticleHandleBase(ch), m_d(new Imp)
{ 
  m_d->theclass = this;
  m_d->muon = tp;
  m_d->collHandle = dynamic_cast<IParticleCollHandle_Muon*>(ch);
  m_d->sep=0;
  m_d->line=0;
}

//____________________________________________________________________
IParticleHandle_Muon::~IParticleHandle_Muon()
{
  if (m_d->line) m_d->line->unref();
  if (m_d->sep) m_d->sep->unref();
  delete m_d;
}

bool IParticleHandle_Muon::has3DObjects(){
  return 0!=m_d->sep;
}
void IParticleHandle_Muon::clear3DObjects(){
  VP1Msg::messageVerbose("IParticleHandle_Muon::clear3DObjects()");
  
  if (m_d->line) {
    m_d->line->unref();
    m_d->line=0;
  }
  if (m_d->sep) {
    m_d->sep->unref();
    m_d->sep=0;
  }
}

// SoNode* IParticleHandle_Muon::nodes(){
//   // VP1Msg::messageVerbose("IParticleHandle_Muon::nodes with m_d->sep="+VP1Msg::str(m_d->sep));
//
//   if (m_d->sep)
//     return m_d->sep; // FIXME - do we need to check if anything need to be redrawn?
//   if (!m_d->sep) {
//     m_d->sep = new SoSeparator();
//     m_d->sep->ref();
//   }
//
//   int iver(0);
//   unsigned npointsused(0);
//
//   // Loop through the TP and find the parameters & create points & get momenta for line interpolation
//   std::vector<Amg::Vector3D> positions;
//   std::vector<Amg::Vector3D> momenta;
//
//   const Trk::Perigee& peri = m_d->muon->primaryTrackParticle()->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
//
//   // std::cout<<peri.position()<<std::endl;
//   // std::cout<<p4.X()<<", "<<p4.Y()<<", "<<p4.Z()<<std::endl;
//
//   const IParticleCollHandle_Muon* collHandleTrk = dynamic_cast<const IParticleCollHandle_Muon*>(collHandle());
//   // FIXME - this ugly...
//
//   float x,y,z;
//   unsigned int iMinusOne=0;
//
//   for (unsigned int i=0; i<m_d->muon->primaryTrackParticle()->numberOfParameters()+1 ; ++i){
//     // std::cout<<"i:"<<i<<"/"<<m_d->muon->primaryTrackParticle()->numberOfParameters()+1<<std::endl;
//     if (0==i) {
//       // 1st position - set 'first' to false a few lines further down...
//       x = peri.position().x();
//       y = peri.position().y();
//       z = peri.position().z();
//       momenta.push_back(Amg::Vector3D(momentum().x(),
//       momentum().y(),
//       momentum().z()));
//     } else {
//       // Other positions
//       iMinusOne=i-1;
//       x = m_d->muon->primaryTrackParticle()->parameterX(iMinusOne);
//       y = m_d->muon->primaryTrackParticle()->parameterY(iMinusOne);
//       z = m_d->muon->primaryTrackParticle()->parameterZ(iMinusOne);
//       momenta.push_back(Amg::Vector3D(m_d->muon->primaryTrackParticle()->parameterPX(iMinusOne),
//       m_d->muon->primaryTrackParticle()->parameterPY(iMinusOne),
//       m_d->muon->primaryTrackParticle()->parameterPZ(iMinusOne)));
//     }
//     positions.push_back(Amg::Vector3D(x,y,z));
//   } // end of loop.
//
//   // Create new lineset etc
//   m_d->line = new SoLineSet();
//   m_d->line->ref();
//   SoVertexProperty * vertices = new SoVertexProperty();
//   iver=0;
//   // For the moment, lets just draw the lines.
//   // std::cout<<"About to loop over "<<positions.size()<<" points to make line"<<std::endl;
//
//   for (unsigned int i=1; i<positions.size(); ++i){
//     // std::cout<<"i: "<<i<<" at ("<<positions[i].x()<<","<<positions[i].y()<<","<<positions[i].z()<<")"<<std::endl;
//     unsigned int maxCount=10;
//     for (unsigned int count=0; count<maxCount;++count){
//       float fraction=count/(float)maxCount;
//       Amg::Vector3D point1(positions[i-1]+fraction*momenta[i-1]);
//       Amg::Vector3D point2(positions[i]-(1.0-fraction)*momenta[i]);
//       Amg::Vector3D tangent = point1+fraction*(point2-point1);
//       vertices->vertex.set1Value(iver++,tangent.x(),tangent.y(),tangent.z());
//       npointsused++;
//     }
//   }
//
//   // Add to SoLine set
//   m_d->line->numVertices.set1Value(0,npointsused);
//   m_d->line->vertexProperty = vertices;
//   // std::cout<<"About to add "<<collHandleTrk->material()<<std::endl;
//   m_d->sep->addChild(collHandleTrk->material());
//   // ^^ FIXME - should rearrange so we don't need to reset material
//   m_d->sep->addChild(m_d->line);
//
//   // points->vertexProperty = vertices2;
//
//   return m_d->sep;
// }

SoNode* IParticleHandle_Muon::nodes(){
  VP1Msg::messageVerbose("IParticleHandle_Muon::nodes with d->sep="+VP1Msg::str(m_d->sep));

  if (m_d->sep) {
    VP1Msg::messageVerbose("IParticleHandle_Muon::nodes() - already have sep so not doing anything.");
    return m_d->sep; 
  }
  if (!m_d->sep) {
    m_d->sep = new SoSeparator();
    m_d->sep->ref();
  }
  
  addLine_FromTrackParticle();
  addParameterShapes();
  
  return m_d->sep;
}

void IParticleHandle_Muon::addParameterShapes(){
  // Okay. so let's make the containers to hold them...
  SoPointSet* parameterPoints = new SoPointSet;
  SoVertexProperty* vertices2 = new SoVertexProperty;

  int i = 0;
  QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parAndPoses = parametersAndPositions();
  
  std::pair<xAOD::ParameterPosition, Amg::Vector3D> parpos;
  foreach (parpos, parAndPoses ) {
    Amg::Vector3D pos = parpos.second;
    vertices2->vertex.set1Value(i++,pos.x(),pos.y(),pos.z());
  } 
  
  parameterPoints->numPoints=i;
  parameterPoints->vertexProperty = vertices2;
  
  // m_d->sep->addChild(m_d->collHandle->collSettingsButton().defaultParameterMaterial());
  m_d->sep->addChild(parameterPoints);
  
}

void IParticleHandle_Muon::addLine_FromTrackParticle(){

  std::vector<Amg::Vector3D> positions, momenta;

  const xAOD::TrackParticle* trackparticle = m_d->muon->primaryTrackParticle();
  if (!trackparticle) {
    VP1Msg::message("ERROR : no primary track particle associated with this Muon. Can't visualise it.");
    return;
  }
#if defined BUILDVP1LIGHT
  positions.push_back(position());
  momenta.  push_back(momentum());
#else
    const Trk::Perigee& peri = trackparticle->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
 
    positions.push_back(Amg::Vector3D(peri.position().x(),peri.position().y(),peri.position().z()));
    momenta.  push_back(Amg::Vector3D(peri.momentum().x(),peri.momentum().y(),peri.momentum().z()));
    VP1Msg::messageVerbose("IParticleHandle_Muon::addLine_FromTrackParticle - has "+QString::number(trackparticle->numberOfParameters())+" extra parameters.");
  #endif // BUILDVP1LIGHT
  for (unsigned int i=0; i<trackparticle->numberOfParameters() ; ++i){
    // std::cout<<"i:"<<i<<"/"<<m_d->trackparticle->numberOfParameters()+1<<std::endl;

    positions.push_back(Amg::Vector3D(trackparticle->parameterX(i),
                                      trackparticle->parameterY(i),
                                      trackparticle->parameterZ(i)));      
    momenta.push_back(Amg::Vector3D(  trackparticle->parameterPX(i),
                                      trackparticle->parameterPY(i),
                                      trackparticle->parameterPZ(i)));      
  } // end of loop.

  if ( positions.size()<2 ) {
    VP1Msg::messageVerbose("IParticleHandle_Muon::addLine_FromTrackParticle - WARNING - not enough points to make a line.");
    return;
  }

  fillLineFromSplineFit(positions, momenta);

  // std::cout<<"About to add "<<collHandleTrk->material()<<std::endl;
  m_d->sep->addChild(m_d->collHandle->material()); 
  // ^^ FIXME - should rearrange so we don't need to reset material

  m_d->sep->addChild(m_d->line);
}

void IParticleHandle_Muon::linear(Amg::Vector3D& output, Amg::Vector3D& a, Amg::Vector3D& b, float t){
  float x = a.x() + (b.x()-a.x())*t;
  float y = a.y() + (b.y()-a.y())*t;
  float z = a.z() + (b.z()-a.z())*t;
  output = Amg::Vector3D(x,y,z);
}

void IParticleHandle_Muon::bezier(Amg::Vector3D& output, Amg::Vector3D& a, Amg::Vector3D& b, Amg::Vector3D& c, Amg::Vector3D& d, float t){
  // DeCasteljau subdivision
  Amg::Vector3D ab,bc,cd,abbc,bccd;
  linear(ab, a,b,t);           // point between a and b (green)
  linear(bc, b,c,t);           // point between b and c (green)
  linear(cd, c,d,t);           // point between c and d (green)
  linear(abbc, ab,bc,t);       // point between ab and bc (blue)
  linear(bccd, bc,cd,t);       // point between bc and cd (blue)
  linear(output, abbc,bccd,t);   // point on the bezier-curve (black)
}

void IParticleHandle_Muon::fillLineFromSplineFit(const std::vector<Amg::Vector3D>& positions, const std::vector<Amg::Vector3D>&momenta){
  int iver(0);
  unsigned int npointsused(0);
  // Create new lineset etc
  m_d->line = new SoLineSet();
  m_d->line->ref();
  
  // For the moment, lets just draw the lines.
  // std::cout<<"About to loop over "<<positions.size()<<" points to make line"<<std::endl;
  SoVertexProperty * vertices = new SoVertexProperty();

  for (unsigned int i=1; i<positions.size(); ++i){
    // std::cout<<"i: "<<i<<" at ("<<positions[i].x()<<","<<positions[i].y()<<","<<positions[i].z()<<")"<<" \tmom = ("<<momenta[i].x()<<","<<momenta[i].y()<<","<<momenta[i].z()<<")"<<std::endl;
    // unsigned int maxCount=m_d->collHandle->collSettingsButton().numOfStepsForInterpolation();
    unsigned int maxCount=4;
    float scale = ( (positions[i]-positions[i-1]).mag() )/2.0;
    Amg::Vector3D p0(positions[i-1]);
    Amg::Vector3D p1(positions[i-1]+(scale * momenta[i-1].unit() ) );
    Amg::Vector3D p2(positions[i]  -(scale * momenta[i].unit() ) );
    Amg::Vector3D p3(positions[i]);
    // std::cout<<"p0: "<<p0<<"\np1: "<<p1<<"\np2: "<<p2<<"\np3: "<<p3<<std::endl;
    
    for (unsigned int count=0; count<maxCount;++count){
      float t=count/(float)maxCount;
      Amg::Vector3D pos;
      bezier(pos, p0, p1, p2, p3, t);
      // std::cout<<"i: "<<count<<" \tat ("<<pos.x()<<","<<pos.y()<<","<<pos.z()<<")"<<" \tmomdelta = ("<<momdelta.x()<<","<<momdelta.y()<<","<<momdelta.z()<<")"<<std::endl;
      
      // pos = pos + momdelta;
      vertices->vertex.set1Value(iver++,pos.x(),pos.y(),pos.z());
      
      npointsused++;        
    }
  }
  // Add to SoLine set
  m_d->line->numVertices.set1Value(0,npointsused);
  m_d->line->vertexProperty = vertices;
  
}

//____________________________________________________________________
QStringList IParticleHandle_Muon::clicked() const
{
  QStringList l;
  l << "Muon:";
  l << IParticleHandleBase::baseInfo();
  switch (m_d->muon->muonType()){
    case  xAOD::Muon::Combined: 
      l << "Type = Combined";
      break;
    case  xAOD::Muon::MuonStandAlone: 
      l << "Type = MuonStandAlone";
      break;
    case  xAOD::Muon::SegmentTagged: 
      l << "Type = SegmentTagged";
      break;
    case  xAOD::Muon::CaloTagged: 
      l << "Type = CaloTagged";
      break;
    case  xAOD::Muon::SiliconAssociatedForwardMuon: 
      l << "Type = SiliconAssociatedForwardMuon";
      break;
    default:
       l << "Type = Unknown";
  }
  
  switch (m_d->muon->quality()){
    case xAOD::Muon::Tight: 
      l << "Quality = Tight";
      break;
    case  xAOD::Muon::Medium: 
      l << "Quality = Medium";
      break;
    case  xAOD::Muon::Loose: 
      l << "Quality = Loose";
      break;
    case  xAOD::Muon::VeryLoose: 
      l << "Quality = VeryLoose";
      break;
    default:
       l << "Quality = Unknown";
  }
  
  return l;
}

//____________________________________________________________________
#if defined BUILDVP1LIGHT
  Amg::Vector3D IParticleHandle_Muon::momentum() const
  {
    if( !(m_d->collHandle->hasPrimaryTrackParticleInfo()) ) {
        m_d->collHandle->printMsgNoTrackParticle();
        return Amg::Vector3D();
    }
    double phi = m_d->muon->primaryTrackParticle()->phi0();
    double theta = m_d->muon->primaryTrackParticle()->theta();

    // decide the sign of the charge
    double qop = m_d->muon->primaryTrackParticle()->qOverP();
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
  Amg::Vector3D IParticleHandle_Muon::momentum() const
  {
    if( !(m_d->collHandle->hasPrimaryTrackParticleInfo()) ) {
        m_d->collHandle->printMsgNoTrackParticle();
        return Amg::Vector3D();
    }
    //return Amg::Vector3D(); // RMB test 
    const Trk::Perigee& p = m_d->muon->primaryTrackParticle()->perigeeParameters();
    return p.momentum();
  }
#endif // 

//____________________________________________________________________
// TODO: probably, we can move this function to the common base class for all trackparticles
#if defined BUILDVP1LIGHT
  Amg::Vector3D IParticleHandle_Muon::position() const
  {
    if( !(m_d->collHandle->hasPrimaryTrackParticleInfo()) ) {
        m_d->collHandle->printMsgNoTrackParticle();
        return Amg::Vector3D();
    }
    double d0 = m_d->muon->primaryTrackParticle()->d0();
    double z0 = m_d->muon->primaryTrackParticle()->z0();
    double phi = m_d->muon->primaryTrackParticle()->phi0();

    // glopos[Amg::x] = - locpos[Trk::d0]*sin(phi);
    // glopos[Amg::y] =   locpos[Trk::d0]*cos(phi);
    // glopos[Amg::z] =   locpos[Trk::z0];


    const Amg::Vector3D pos = Amg::Vector3D(- d0*sin(phi), d0*cos(phi), z0);

    return pos;
  }
#else
    Amg::Vector3D IParticleHandle_Muon::position() const
  {
    if( !(m_d->collHandle->hasPrimaryTrackParticleInfo()) ) {
        m_d->collHandle->printMsgNoTrackParticle();
        return Amg::Vector3D();
    }
    const Trk::Perigee& p = m_d->muon->primaryTrackParticle()->perigeeParameters();
    return p.position(); // TODO: check what this returns
  }
#endif

//____________________________________________________________________
const xAOD::IParticle& IParticleHandle_Muon::iParticle() const
{
    if( !(m_d->collHandle->hasPrimaryTrackParticleInfo()) ) {
        m_d->collHandle->printMsgNoTrackParticle();
    }
  return *(m_d->muon->primaryTrackParticle());
}

//____________________________________________________________________
double IParticleHandle_Muon::charge() const
{
    if( !(m_d->collHandle->hasPrimaryTrackParticleInfo()) ) {
        m_d->collHandle->printMsgNoTrackParticle();
        return 0.0;
    }
  return m_d->muon->primaryTrackParticle()->charge();
}

//____________________________________________________________________
unsigned int IParticleHandle_Muon::quality() const
{
    if( !(m_d->collHandle->hasPrimaryTrackParticleInfo()) ) {
        m_d->collHandle->printMsgNoTrackParticle();
        return 0;
    }
  return static_cast<unsigned int>(m_d->muon->quality());
}

const xAOD::Muon& IParticleHandle_Muon::muon() const 
{
  return *(m_d->muon);
}

QString IParticleHandle_Muon::muonTypeString() const {
  switch (m_d->muon->muonType()) {
    case xAOD::Muon::Combined:
    return QString("Combined");
    case xAOD::Muon::MuonStandAlone:
    return QString("MuonStandAlone");
    case xAOD::Muon::SegmentTagged:
    return QString("SegmentTagged");
    case xAOD::Muon::CaloTagged:
    return QString("CaloTagged");
    case xAOD::Muon::SiliconAssociatedForwardMuon:
    return QString("SiliconAssociatedForwardMuon");
    default:
    return QString("Unknown Muon type");
  }
}

QString IParticleHandle_Muon::qualityString() const{
  switch (m_d->muon->quality()) {
    case xAOD::Muon::Tight:
    return QString("Tight");
    case xAOD::Muon::Medium:
    return QString("Medium");
    case xAOD::Muon::Loose:
    return QString("Loose");
    case xAOD::Muon::VeryLoose:
    return QString("VeryLoose");
    default:
    return QString("Unknown Muon quality");
  }
}

// //____________________________________________________________________
// unsigned IParticleHandle_Muon::summaryValue(xAOD::SummaryType type) const
// { 
//   uint8_t num = 0;
//   if (m_d->muon->summaryValue(num,type)){
//     return num;
//   }
//   // else...
//   VP1Msg::message("IParticleHandle_Muon::getSummaryValue - unable to retrieve the requested enum: "+VP1Msg::str(type));
//   return 999999;
// }


QString IParticleHandle_Muon::shortInfo() const
{
  
  QString l("");
  l+= "|P|=";
  l+= VP1Msg::str(momentum().mag()/SYSTEM_OF_UNITS::GeV)+" [GeV], ";
  l+= muonTypeString() + ", ";
  l+= qualityString();
  return l;
}

void IParticleHandle_Muon::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) 
{
  IParticleHandleBase::fillObjectBrowser(listOfItems); // Obligatory!

    if( !(m_d->collHandle->hasPrimaryTrackParticleInfo()) ) {
        m_d->collHandle->printMsgNoTrackParticle();
        return;
    }
  

  QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());
  TSOSitem->setText(0, QString("Def. Parameters [d0,z0,phi0,theta,qOverP]" ) );
  QString dParameters("(");
  dParameters+=QString::number(m_d->muon->primaryTrackParticle()->d0());
  dParameters+=", ";
  dParameters+=QString::number(m_d->muon->primaryTrackParticle()->z0());
  dParameters+=", ";
  dParameters+=QString::number(m_d->muon->primaryTrackParticle()->phi0());
  dParameters+=", ";
  dParameters+=QString::number(m_d->muon->primaryTrackParticle()->theta());
  dParameters+=", ";
  dParameters+=QString::number(m_d->muon->primaryTrackParticle()->qOverP());
  dParameters+=")";
  TSOSitem->setText(1, dParameters );
  
  for (unsigned int i=0; i<m_d->muon->primaryTrackParticle()->numberOfParameters() ; ++i){
    QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());
    TSOSitem->setText(0, QString("Parameter "+QString::number( i+1 ) ) );
    QString pos(", Position = (");
    pos+=QString::number(m_d->muon->primaryTrackParticle()->parameterX(i));
    pos+=", ";
    pos+=QString::number(m_d->muon->primaryTrackParticle()->parameterY(i));
    pos+=", ";
    pos+=QString::number(m_d->muon->primaryTrackParticle()->parameterZ(i));
    pos+=")";

    switch (m_d->muon->primaryTrackParticle()->parameterPosition(i)){
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

const QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> >& IParticleHandle_Muon::parametersAndPositions() const {
  // 1st position
  if (!m_d->parametersAndPositions.isEmpty()) 
    return m_d->parametersAndPositions;
  
  typedef std::pair<xAOD::ParameterPosition, Amg::Vector3D> paramAndPos;
  
  #if defined BUILDVP1LIGHT
    m_d->parametersAndPositions.append(paramAndPos(xAOD::BeamLine, position() ) );
  #else
    if( !(m_d->collHandle->hasPrimaryTrackParticleInfo()) ) {
        m_d->collHandle->printMsgNoTrackParticle();
        return m_d->parametersAndPositions;
    }
    const Trk::Perigee& peri = m_d->muon->primaryTrackParticle()->perigeeParameters (); // FIXME - I'd quite like not to use anything which requires Athena ...
    m_d->parametersAndPositions.append(paramAndPos(xAOD::BeamLine, Amg::Vector3D(peri.position().x(),peri.position().y(),peri.position().z()) ) );
  #endif // BUILDVP1LIGHT

  float x,y,z;
  for (unsigned int i=0; i<m_d->muon->primaryTrackParticle()->numberOfParameters() ; ++i){
    x = m_d->muon->primaryTrackParticle()->parameterX(i);
    y = m_d->muon->primaryTrackParticle()->parameterY(i);
    z = m_d->muon->primaryTrackParticle()->parameterZ(i);
  
    m_d->parametersAndPositions.append(paramAndPos(m_d->muon->primaryTrackParticle()->parameterPosition(i), Amg::Vector3D(x,y,z)));
  }
  return m_d->parametersAndPositions;
}
