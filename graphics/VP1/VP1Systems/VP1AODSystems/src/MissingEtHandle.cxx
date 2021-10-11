/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class MissingEtHandle               //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

// Local
#include "MissingEtHandle.h"
#include "AODSysCommonData.h"

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
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoGroup.h>

#include <Inventor/SoPath.h>

#include <SoDebug.h> // it's stored at /afs/cern.ch/sw/lcg/external/coin3d/3.1.3p2/x86_64-slc6-gcc47-opt/include/SoDebug.h
#include "MissingEtCollectionSettingsButton.h"
#include "MissingEtCollHandle.h"

// System of units
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::cm'
#else
  #include "GaudiKernel/SystemOfUnits.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::cm'
#endif

//double MissingEtHandle::thickness = 1.0;
//double MissingEtHandle::scale     = 1.0;
//bool   MissingEtHandle::shape     = true;


//____________________________________________________________________
class MissingEtHandle::Imp {
public:
  // constructor and default values
  //Imp () : theclass(0), controller(0), scale(1.0), bTaggingTagger("MV1"),bTaggingCut(0.98),randomColours(false),bTaggingSwitch(0),bTaggingTexture(0), bTaggingMaterial(0) {} // NOTE: parameters from the old JetCollection
  //Imp () : theclass(0), scale(1.0) {} // TODO: implement the parameters above, from the old JetCollection

  MissingEtHandle * theclass = nullptr; // the Jet class
  const MissingEtCollHandle* theCollHandle = nullptr; // the CollHandle instance
  const MissingEtCollectionSettingsButton* theCollSettingsButton = nullptr;

  const xAOD::MissingET * m_met = nullptr; // the met object

  SoSeparator * sep = nullptr; // everything hangs from this.
  SoGroup * base = nullptr ;//This gathers the whole dashed line visualizing MissingEt.
  SoCone * cone = nullptr ;//This gathers the whole dashed line visualizing MissingEt.
  //	SoMaterial * m_randomMat; // random material for jets


  //QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parametersAndPositions; // cache // TODO: see if useful for jets

  //Settings:
  //	bool considerTransverseEnergies = true; // TODO: update with button connection "E/Et" (see VP1JetCollection.cxx)
  //	double coneRPar = -1; // FIXME: add calculation of coneRPar, like in the old VP1 Jet
  double scale = 10.0; // default
  double length = 1.0; // default
  double thickness = 1.0; // default
  bool shape = true; // default
  double maxR = 0.0;


  //	SoLineSet * line;//This represents the line(s) representing the trackparticle. Can be interpolated.
  //	SoPointSet * points;//This represents the points(s) representing the trackparticle's parameters.


  // Getters
  QString name() const { return QString::fromStdString( m_met->name() ); }
  double mpx() const { return m_met->mpx(); }
  double mpy() const { return m_met->mpy(); }
  double met() const { return m_met->met(); }
  double phi() const { /*VP1Msg::messageVerbose("phi: " + QString::number(m_met->phi()) );*/ return m_met->phi(); }
  double sumet() const { return m_met->sumet(); }


  //	double eta() const { /*VP1Msg::messageVerbose("eta: " + QString::number(m_met->eta()) );*/ return m_met->eta(); }
  //	double energyForLengthAndCuts() const { return considerTransverseEnergies ? transverseEnergy() : energy(); }

  //Create/manipulate 3D objects:
  //	void createShapeFromJetParameters(const MissingEtCollHandle* collHandleJet, const double& coneR, const double& eta, const double& phi, const double& energy, const SbVec3f& origin);
  void createShapeFromJetParameters();

  void updateConeHeightParameters(SoCone*, SoTranslation*, const double& energy) const;
  void updateConeHeightParameters() const;

  // Update random material
  //	void rerandomiseMaterial();


};

//____________________________________________________________________
MissingEtHandle::MissingEtHandle(AODCollHandleBase* ch, const xAOD::MissingET *met)
  : AODHandleBase(ch), m_d(new Imp)
{ 
  VP1Msg::messageVerbose("MissingEtHandle::MissingEtHandle() - constructor");
  m_d->theclass = this;
  m_d->m_met = met;

  m_d->sep = 0;
  m_d->cone = 0;
  //	m_d->m_randomMat = 0;

  //	m_d->theCollHandle = dynamic_cast<const MissingEtCollHandle*>(collHandle());
  m_d->theCollHandle = 0;
}

//____________________________________________________________________
MissingEtHandle::~MissingEtHandle()
{
  //	VP1Msg::messageVerbose("MissingEtHandle::~MissingEtHandle() - destructor");
  //	if (m_d->m_randomMat) m_d->m_randomMat->unref();
  if (m_d->cone) m_d->cone->unref();
  if (m_d->base) m_d->base->unref();
  if (m_d->sep) m_d->sep->unref();
  delete m_d;
}

//____________________________________________________________________
SoMaterial * MissingEtHandle::determineMaterial() {
  // By default we use the collection material.
  // std::cout<<"VertexHandle::determineMaterial() - collHandle()->material()"<<collHandle()->material()<<std::endl;
  return collHandle()->material();
}

// Setter
//____________________________________________________________________
void MissingEtHandle::setScale( const double& sc) { m_d->scale = sc; }

////____________________________________________________________________
//void MissingEtHandle::rerandomiseMaterial() {m_d->rerandomiseMaterial(); }

//____________________________________________________________________
bool MissingEtHandle::has3DObjects()
{
  //	VP1Msg::messageVerbose("MissingEtHandle::has3DObjects()");
  return 0 != m_d->sep;
}


//____________________________________________________________________
void MissingEtHandle::clear3DObjects(){
  //	VP1Msg::messageVerbose("MissingEtHandle::clear3DObjects()");

  //	if (m_d->m_randomMat) {
  //		m_d->m_randomMat->unref();
  //		m_d->m_randomMat = 0;
  //	}
  if (m_d->cone) {
    m_d->cone->unref();
    m_d->cone = 0;
  }
  if (m_d->sep) {
    m_d->sep->unref();
    m_d->sep = 0;
  }

}



/*
* This is the method which makes the 3D objects from the measurements
*/
//____________________________________________________________________
SoNode* MissingEtHandle::nodes(){

  VP1Msg::messageVerbose("MissingEtHandle::nodes()");

  if (m_d->sep) {
    VP1Msg::messageVerbose("d->sep already defined (" + VP1Msg::str(m_d->sep) + "). Returning d->sep.");
    return m_d->sep; // FIXME - do we need to check if anything need to be redrawn?
  }
  if (!m_d->sep) {
    VP1Msg::messageVerbose("d->sep not defined. Creating shapes and a new d->sep.");
    m_d->sep = new SoSeparator();
    m_d->sep->ref();
  }

  m_d->theCollHandle = dynamic_cast<const MissingEtCollHandle*>(collHandle());
  m_d->theCollSettingsButton = &(m_d->theCollHandle->collSettingsButton());


//  SbVec3f origin(0.,0.,0.);
  /* TODO: ask if origin info is present in xAOD, like in the old Jet class
  if ( m_d->m_met->origin() ) {
  origin.setValue(m_d->m_met->origin()->position().x(),
  m_d->m_met->origin()->position().y(),
  m_d->m_met->origin()->position().z());
  }
  */

  VP1Msg::messageVerbose("creating the shapes");


  /*
  * Here the 3D shapes are created
  */
  m_d->createShapeFromJetParameters();

  return m_d->sep;
}

//____________________________________________________________________
void MissingEtHandle::updateHeight() {
  m_d->updateConeHeightParameters();
}


//____________________________________________________________________
//void MissingEtHandle::Imp::createShapeFromJetParameters(const MissingEtCollHandle* collHandleJet, const double& inputconeR, const double& eta,
//		const double& phi, const double& energy, const SbVec3f& origin)
void MissingEtHandle::Imp::createShapeFromJetParameters()
{
  VP1Msg::messageVerbose("MissingEtHandle::Imp::createShapeFromJetParameters()");

  //double length = m_met->met() * (200.0 * SYSTEM_OF_UNITS::cm /(100.0 * SYSTEM_OF_UNITS::GeV ));//Fixme: Make scale factor (and thickness) adjustable.

  // Set length of MET: 200 cm for 100 GeV
  double length = m_met->met() * (200.0 * SYSTEM_OF_UNITS::cm /(100.0 * SYSTEM_OF_UNITS::GeV ));
  this->length = length;

  // Set scale factor for MET length, from user's settings in the GUI
  this->scale = theCollSettingsButton->metLength();

  // Set thickness for MET line, from user's settings in the GUI
  this->thickness = theCollSettingsButton->metThickness();


  sep = new SoSeparator();
  sep->ref();

  //Dash line
  SoGroup * dash  = new SoGroup();

  SoRotationXYZ *r = new SoRotationXYZ();
  r->axis=SoRotationXYZ::Z;
  r->angle=-M_PI/2+ m_met->phi();
  dash->addChild(r);

  SoTranslation *t= new SoTranslation();
  t->translation.setValue(0, 0, 0);
  dash->addChild(t);

  //Define line width
  SoDrawStyle *drawStyle = new SoDrawStyle;
  drawStyle->style.setValue(SoDrawStyle::LINES);
  drawStyle->lineWidth.setValue(1. * thickness);
  drawStyle->linePattern.setValue(0xFF00); // 16-bits pattern for dashed line
  dash->addChild(drawStyle);

  //Define line connection
  SoCoordinate3 *coords = new SoCoordinate3;
  SbVec3f* vert = new SbVec3f[2];
  vert[0] = SbVec3f(0.0, 0.0, 0.0);
  vert[1] = SbVec3f(0.0, length * scale, 0.0);
  coords->point.setValues(0, 2, vert);
  delete [] vert;
  dash->addChild(coords);

  SoLineSet *lineSet = new SoLineSet ;
  lineSet->numVertices.set1Value(0, 2) ;
  dash->addChild(lineSet);

  dash->ref();

  base  = new SoGroup();
  base->addChild(dash);
  base->ref();


  sep->addChild(base);  // starts along y-axis // DO NOT MOVE THIS: ITS POSITION IS USED BY "updateConeHeightParameters(SoSeparator* sep,const double& energy)"

  //return sep;
}

//_____________________________________________________________________________________
void MissingEtHandle::Imp::updateConeHeightParameters(SoCone*cone, SoTranslation* trans, const double& energy) const
{
  VP1Msg::messageVerbose("MissingEtHandle::Imp::updateConeHeightParameters()");

  //	double h(scale * energy);
  double h(scale);

  if (maxR > 0.0001) { // maxR > 0. is not univocally defined, because maxR is a floating point number
    VP1Msg::messageVerbose("maxR > 0.0001 ==> setting h = min(h,maxR)");
    h = std::min(h,maxR);
  }

  //h = 1500.0; // only for debug

  cone->height = h;
  cone->bottomRadius = 50.; //tan(coneR()) * h;
  trans->translation = SbVec3f(0,-0.5*h,0);

  //debug
  SbString strHeight, strRadius;
  (cone->height).get(strHeight);
  (cone->bottomRadius).get(strRadius);
  std::cout << "input - energy: " << energy << " - scale: " << scale << " - maxR: " << maxR << " - h: " << h << " --- updated cone - height: " << strHeight.getString() << " - bottom radius: " << strRadius.getString() << std::endl;

  // you can also use the 'writeField()' method, direct to std output
  //SoDebug::writeField(&(cone->height));

  //	QString text = "updated cone - height: " + QString::number(cone->height) + " - bottom radius: " + QString::number(cone->bottomRadius) " - translation: " + trans->translation;
  //	VP1Msg::messageVerbose(text);
}


//_____________________________________________________________________________________
void MissingEtHandle::Imp::updateConeHeightParameters() const {


  if (!sep) {
    VP1Msg::messageVerbose("sep not defined. Returning.");
    return;
  }

  //NB: The translation is the SIXTH child and the cone is the LAST child.
  if (sep->getNumChildren()<6) {
    VP1Msg::messageVerbose("getNumChildren() < 6!!!");
    return;
  }

  //	const double energyJet = energyForLengthAndCuts();
  //
  //	SoNode * sixthChild = sep->getChild(5);
  //	if (sixthChild->getTypeId()!=SoTranslation::getClassTypeId()) {
  //		return;
  //	}
  //	SoNode * lastChild = sep->getChild(sep->getNumChildren()-1);
  //	if (lastChild->getTypeId()!=SoCone::getClassTypeId()) {
  //		return;
  //	}
  //	updateConeHeightParameters(static_cast<SoCone*>(lastChild),static_cast<SoTranslation*>(sixthChild), energyJet);
}


///
/// This gives the complete information about the object, shown in the main Message Box
///
//____________________________________________________________________
QStringList MissingEtHandle::clicked() const
{
	VP1Msg::messageDebug("MissingEtHandle::clicked()");

	QStringList l;
	l << "--MissingEt: ";
	//l << AODHandleBase::baseInfo();

	// info and parameters,
	// they go in the "Information" column in the Browser window
	// see: http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODMissingET/xAODMissingET/versions/MissingET_v1.h
	//
	l +="   - Name: " + m_d->name();
	l +="   - MissingEt: " + QString::number(m_d->met() / SYSTEM_OF_UNITS::GeV) +" [GeV]";
	l +="   - Phi: " + QString::number(m_d->phi());
	l +="   - SumEt: " + QString::number(m_d->sumet() / SYSTEM_OF_UNITS::GeV) +" [GeV]";
	l +="   - mpx: " + QString::number(m_d->mpx() / SYSTEM_OF_UNITS::GeV) +" [GeV]" ;
	l +="   - mpy: " + QString::number(m_d->mpy() / SYSTEM_OF_UNITS::GeV) +" [GeV]";

	return l;
}

/*
//____________________________________________________________________
Amg::Vector3D MissingEtHandle::momentum() const
{
const Trk::Perigee& p = m_d->trackparticle->perigeeParameters();
return p.momentum();
}
*/

////____________________________________________________________________
//const xAOD::IParticle& MissingEtHandle::iParticle() const
//{
//	return *(m_d->m_met);
//}


////____________________________________________________________________
//double MissingEtHandle::charge() const
//{
//	//return m_d->trackparticle->charge(); // TODO: check in Jet interface if a "charge" is defined
//	return 0; // FIXME: dummy value now
//}



/*
* TODO: If Jet class has something like SummaryType like TrackParticle has, implement this method. Otherwise, remove it!
*
*/
/*
//____________________________________________________________________
unsigned MissingEtHandle::summaryValue(xAOD::SummaryType type) const
{ 
uint8_t num = 0;
if (m_d->trackparticle->summaryValue(num,type)){
return num;
}
// else...
VP1Msg::message("MissingEtHandle::getSummaryValue - unable to retrieve the requested enum: "+VP1Msg::str(type));
return 999999;
}
*/


///
/// This gives the very short summary for the object's properties, shown in the 'Information' field in the Browser, beside the item number (e.g. 'MissingEt0')
///
//____________________________________________________________________
QString MissingEtHandle::shortInfo() const
{
	VP1Msg::messageDebug("MissingEtHandle::shortInfo()");

  QString l;

  l += m_d->name();
  l += ", met: " + QString::number(m_d->met() / SYSTEM_OF_UNITS::GeV) +" [GeV]";
  l += ", phi: " + QString::number(m_d->phi());

  return l;
}


///
/// This gives the list of object's properties, shown in the 'Information' field in the Browser, once the user clicked on one particular item (e.g. 'MissingEt0')
///
//____________________________________________________________________
void MissingEtHandle::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems)
{
  AODHandleBase::fillObjectBrowser(listOfItems); // Obligatory!

  QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());

  // Jet "Object" title, in the Browser window
  TSOSitem->setText(0, QString("Info: " ) );

  QString dParameters("(");

  // info and parameters,
  // they go in the "Information" column in the Browser window
  dParameters+="name: ";
  dParameters+=m_d->name();
  dParameters+=", met: ";
  dParameters+=QString::number(m_d->met());
  dParameters+=", phi: ";
  dParameters+=QString::number(m_d->phi());
  dParameters+=", sumet: ";
  dParameters+=QString::number(m_d->sumet());

  dParameters+="";

  dParameters+=")";

  dParameters += " [more info in the main Message Box]";

  TSOSitem->setText(1, dParameters );

  /*
  * TODO: check jets parameters
  */
  /*
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
  */


  // TODO - add more.
}




//____________________________________________________________________
double MissingEtHandle::phi() const {
  /*VP1Msg::messageVerbose("phi: " + QString::number(m_met->phi()) );*/
  return m_d->phi();
}


//____________________________________________________________________
double MissingEtHandle::met() const {
  /*VP1Msg::messageVerbose("eta: " + QString::number(m_met->eta()) );*/
  return m_d->met();
}


////____________________________________________________________________
//double MissingEtHandle::energy() const { return m_d->energy(); }
//
//
////____________________________________________________________________
//double MissingEtHandle::energyForCuts() const { return m_d->energyForLengthAndCuts(); }
//
//
////____________________________________________________________________
//double MissingEtHandle::transverseEnergy() const { return m_d->transverseEnergy(); } //sin(2*atan(exp(-fabs(eta()))))*energy();


////____________________________________________________________________
//void MissingEtHandle::Imp::rerandomiseMaterial()
//{
//	VP1Msg::messageVerbose("MissingEtHandle::Imp::rerandomiseMaterial()");
//
//	//Fixme: share this code with other systems!!
//	if ( !m_randomMat ) { //We will anyway rerandomize it when we need it
//		VP1Msg::messageVerbose("'m_randomMat not set. Returning.");
//		return;
//	}
//
//	double r2 = 0.3*0.3;
//	unsigned i(0);
//	double r,g,b;
//	bool ok;
//	while (true) {
//		r = (rand() / static_cast<double>(RAND_MAX));
//		g = (rand() / static_cast<double>(RAND_MAX));
//		b = (rand() / static_cast<double>(RAND_MAX));
//		ok = true;
//		//For now we make sure that we avoid black and red. This should be updated from bgd and highlight col automatically! (fixme).
//		// -> and we should probably also make sure that tracks close in (eta,phi) are well separated in colour-space.
//		if ( (r-1.0)*(r-1.0)+g*g+b*b < r2*0.5 )//avoid red (distance)
//			ok = false;
//		else if ( r*r/(r*r+g*g+b*b) > 0.8 )//avoid red (angle)
//			ok = false;
//		else if ( r*r+g*g+b*b < r2*2.0 )//avoid black
//			ok = false;
//		if (ok)
//			break;
//		++i;
//		if (i>50 ) {
//			r2 *= 0.99;//To avoid problem in case we add too many forbidden spheres.
//			if (i>1000) {
//				//Just a safety
//				VP1Msg::messageVerbose("MissingEtHandle::Imp::rerandomiseMaterial() - Warning: Random colour could"
//						" not be selected such as to satisfy all separation criterias");
//				break;
//			}
//		}
//	}
//
//	VP1MaterialButton::setMaterialParameters(m_randomMat,r, g, b, 0.1, 0.5 );
//}

/*
//____________________________________________________________________
//void MissingEtHandle::Imp::updateMaterial()
void MissingEtHandle::updateMaterial(bool isRandomColors)
{
  VP1Msg::messageVerbose("MissingEtHandle::Imp::updateMaterial()");

  // check if we have 3D objects; if not, return
  if ( m_d->sep == 0 )
    return;

  //	if (!isRandomColors && !m_d->m_randomMat)
  //		return;//m_randomMat can never have been attached
  //
  //	if (isRandomColors && !m_d->m_randomMat) {
  //		m_d->m_randomMat = new SoMaterial;
  //		m_d->m_randomMat->ref();
  //		rerandomiseMaterial();
  //	}


  //	int i = m_d->sep->findChild(m_d->m_randomMat);
  //
  //	if ( (i>=0) == isRandomColors ) {
  //		VP1Msg::messageVerbose("(i>=0)==isRandomColors. Returning.");
  //		return;
  //	}
  //
  //	if (!isRandomColors )
  //		m_d->sep->removeChild(m_d->m_randomMat);
  //	else
  //		m_d->sep->insertChild(m_d->m_randomMat, m_d->sep->getNumChildren()-1);
}
*/
