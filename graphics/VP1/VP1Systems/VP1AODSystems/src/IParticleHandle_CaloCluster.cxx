/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleHandle_CaloCluster               //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

// Local
#include "IParticleHandle_CaloCluster.h"
#include "IParticleCollHandle_CaloCluster.h"
#include "AODSysCommonData.h"
#include "VP1AODSystems/JetCollectionSettingsButton.h"

//VP1
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/SoTools.h"
#include "VP1HEPVis/nodes/SoGenericBox.h"

//SoCoin
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoMaterial.h>
//#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSwitch.h>
#include <SoDebug.h> // it's stored at /afs/cern.ch/sw/lcg/external/coin3d/3.1.3p2/x86_64-slc6-gcc47-opt/include/SoDebug.h

// System of units
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::cm'
#else
  #include "GaudiKernel/SystemOfUnits.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::cm'
#endif


//____________________________________________________________________
class IParticleHandle_CaloCluster::Imp {
public:

	IParticleHandle_CaloCluster * theclass = nullptr; // the Jet class
	const IParticleCollHandle_CaloCluster* theCollHandle = nullptr; // the CollHandle instance

	const xAOD::CaloCluster * m_cluster = nullptr; // the single jet

	SoSeparator * sep = nullptr; // everything hangs from this.
	SoGenericBox* m_genericBox = nullptr; //This is the box representing the calorimeter cluster.

	//Settings:
	bool considerTransverseEnergies = true; // TODO: update with button connection "E/Et" (see VP1JetCollection.cxx)

	// Getters
	const xAOD::CaloCluster* cluster() const { return m_cluster; }
	SoGenericBox * genericBox() const { return m_genericBox; }
	double phi() const { /*VP1Msg::messageVerbose("phi: " + QString::number(m_cluster->phi()) );*/ return m_cluster->phi(); }
	double eta() const { /*VP1Msg::messageVerbose("eta: " + QString::number(m_cluster->eta()) );*/ return m_cluster->eta(); }
	double et() const { VP1Msg::messageDebug("et()");return m_cluster->et(); }
	double e() const { VP1Msg::messageDebug("e()");return m_cluster->e(); }

	double energyForLengthAndCuts(const IParticleCollHandle_CaloCluster* coll_handle) { return coll_handle->isConsiderTransverseEnergy() ? et() : e(); } // TODO: is this used at all??
	double energyForLengthAndCuts() { VP1Msg::messageDebug("energyForLengthAndCuts()"); return theclass->isConsiderTransverseEnergies() ? et() : e();	}

	// Create/manipulate 3D objects
	void createShapeFromParameters(const IParticleCollHandle_CaloCluster* coll_handle);
	void updateShapePars(const IParticleCollHandle_CaloCluster* coll_handle);

};

//____________________________________________________________________
IParticleHandle_CaloCluster::IParticleHandle_CaloCluster(IParticleCollHandleBase* ch, const xAOD::CaloCluster *cluster)
: IParticleHandleBase(ch), m_d(new Imp)
{ 
	//	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::IParticleHandle_CaloCluster() - constructor");

	// set the Imp members
	m_d->theclass = this;
	m_d->m_cluster = cluster;
	m_d->sep = 0;
	m_d->m_genericBox = 0;
  
  //
  SoGenericBox::initClass();
}




//____________________________________________________________________
IParticleHandle_CaloCluster::~IParticleHandle_CaloCluster()
{
	//	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::~IParticleHandle_CaloCluster() - destructor");
	if (m_d->m_genericBox) m_d->m_genericBox->unref();
	if (m_d->sep) m_d->sep->unref();
	delete m_d;
}



//____________________________________________________________________
SoGenericBox* IParticleHandle_CaloCluster::genericBox() const
{
	return m_d->genericBox();
}


//____________________________________________________________________
bool IParticleHandle_CaloCluster::isConsiderTransverseEnergies() const
{
	const IParticleCollHandle_CaloCluster* collHandleJet = dynamic_cast<const IParticleCollHandle_CaloCluster*>(collHandle());
	if (not collHandleJet) return false;
	bool Et = collHandleJet->isConsiderTransverseEnergy();
	VP1Msg::messageDebug("IParticleHandle_CaloCluster::isConsiderTransverseEnergies() - " + QString::number(Et));
	return Et;
}

// Setter
////____________________________________________________________________
//void IParticleHandle_CaloCluster::setScale( const double& sc) { m_d->scale = sc; }

//____________________________________________________________________
//void IParticleHandle_CaloCluster::setMaxR(const double& maxR) { m_d->maxR = maxR * SYSTEM_OF_UNITS::m; }


//____________________________________________________________________
//void IParticleHandle_CaloCluster::rerandomiseMaterial() {m_d->rerandomiseMaterial(); }

//____________________________________________________________________
bool IParticleHandle_CaloCluster::has3DObjects()
{
	//	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::has3DObjects()");
	return 0 != m_d->sep;
}


//____________________________________________________________________
void IParticleHandle_CaloCluster::clear3DObjects(){
	//	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::clear3DObjects()");

	//	if (m_d->m_randomMat) {
	//		m_d->m_randomMat->unref();
	//		m_d->m_randomMat = 0;
	//	}
	if (m_d->m_genericBox) {
		m_d->m_genericBox->unref();
		m_d->m_genericBox = 0;
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
SoNode* IParticleHandle_CaloCluster::nodes(){

	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::nodes()");

	if (m_d->sep) {
		VP1Msg::messageVerbose("d->sep already defined (" + VP1Msg::str(m_d->sep) + "). Returning d->sep.");
		return m_d->sep; // FIXME - do we need to check if anything need to be redrawn?
	}
	if (!m_d->sep) {
		VP1Msg::messageVerbose("d->sep not defined. Creating shapes and a new d->sep.");
		m_d->sep = new SoSeparator();
		m_d->sep->ref();
	}

	const IParticleCollHandle_CaloCluster* coll_handle = dynamic_cast<const IParticleCollHandle_CaloCluster*>(collHandle());
  if (not coll_handle) return m_d->sep;
	VP1Msg::messageVerbose("creating the shapes");

	/*
	 * Build the 3D objects for the physics object, and update m_d->sep.
	 */
	//	m_d->createShapeFromJetParameters(collHandle, m_d->coneR(), m_d->eta(), m_d->phi(), m_d->energyForLengthAndCuts(), origin);
	//	m_d->createShapeFromParameters(coll_handle, origin);
	m_d->createShapeFromParameters(coll_handle);


	return m_d->sep;
}


////____________________________________________________________________
//void IParticleHandle_CaloCluster::updateHeight() {
//	m_d->updateConeHeightParameters();
//}

void IParticleHandle_CaloCluster::updateShape(IParticleCollHandle_CaloCluster* coll)
{
	VP1Msg::messageDebug("IParticleHandle_CaloCluster::updateShape()");
	m_d->updateShapePars(coll);
}


//____________________________________________________________________
void IParticleHandle_CaloCluster::Imp::updateShapePars(const IParticleCollHandle_CaloCluster* coll_handle)
{
	VP1Msg::messageDebug("IParticleHandle_CaloCluster::Imp::updateShapePars()");


	const double dEta = 0.05;//FIXME
	const double dPhi = 0.05;//FIXME
	const double theEta = eta();
	const double thePhi = phi();
	const double cellDepth = coll_handle->energyToLength( energyForLengthAndCuts(coll_handle) );
	if (fabs(theEta) < IParticleCollHandle_CaloCluster::calo_crack_eta) {
		m_genericBox->setParametersForBarrelEtaPhiCell( theEta-0.5*dEta, theEta+0.5*dEta,thePhi-0.5*dPhi, thePhi+0.5*dPhi,
				cellDepth, IParticleCollHandle_CaloCluster::calo_start_r );
	} else {
		m_genericBox->setParametersForEndCapEtaPhiCell( theEta-0.5*dEta, theEta+0.5*dEta,thePhi-0.5*dPhi, thePhi+0.5*dPhi,
				cellDepth, IParticleCollHandle_CaloCluster::calo_start_z );
	}
}

//____________________________________________________________________
//void IParticleHandle_CaloCluster::Imp::createShapeFromJetParameters(const IParticleCollHandle_CaloCluster* collHandle, const double& inputconeR, const double& eta,
//		const double& phi, const double& energy, const SbVec3f& origin)
//void IParticleHandle_CaloCluster::Imp::createShapeFromParameters(const IParticleCollHandle_CaloCluster* coll_handle, const SbVec3f& origin)
void IParticleHandle_CaloCluster::Imp::createShapeFromParameters(const IParticleCollHandle_CaloCluster* coll_handle)
{
	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::Imp::createShapeFromParameters()");

	if (!m_genericBox) {	
		SoGenericBox::initClass();
		m_genericBox = new SoGenericBox();
		m_genericBox->drawEdgeLines = coll_handle->showOutlines();
		m_genericBox->forceEdgeLinesInBaseColour = true;
		m_genericBox->ref();
	}

	//	m_attached = true;
	updateShapePars(coll_handle);
	//	m_d->sephelper->addNode(m_genericBox);


	//std::cout << "Using material: " << coll_handle->material() << std::endl; // ^^ FIXME - should rearrange so we don't need to reset material

	sep->addChild(coll_handle->material());
	sep->addChild(m_genericBox);






	//	sep = new SoSeparator();
	//	sep->ref();


	//	cone = new SoCone();
	//	cone->ref();

	//	// coneR IS the opening half-angle of the jet, in delta phi (easy) and in
	//	//  delta eta (trickier)
	//	// to try to get the jet extent right in eta, find theta corresponding
	//	//  to eta+R, eta-R, and take half of the difference:
	//	double thetaMax = 2.*atan(exp(-(eta+inputconeR)));
	//	double thetaMin = 2.*atan(exp(-(eta-inputconeR)));
	//	double deltaTheta = fabs(thetaMax-thetaMin);
	//	double etaScale = deltaTheta/(2.*inputconeR);
	//
	//	// Translate by half cone height and flip 180 deg so point is at IP:
	//	SoTranslation *translate = new SoTranslation();
	//
	//	SoRotationXYZ *flip = new SoRotationXYZ();
	//	flip->axis=SoRotationXYZ::Z;
	//	flip->angle=M_PI;
	//
	//	SoRotationXYZ *ytoz = new SoRotationXYZ();
	//	ytoz->axis=SoRotationXYZ::X;
	//	ytoz->angle=M_PI/2.;
	//
	//	// Cones should now be along Z-axis,point at IP.
	//	SoRotationXYZ *rotationPhi = new SoRotationXYZ();
	//	rotationPhi->axis=SoRotationXYZ::Z;
	//	rotationPhi->angle = phi+M_PI/2.; // starts from -y-axis in x-y plane
	//	// phi is measured from x-axis, so +M_PI/2
	//
	//	SoRotationXYZ *rotationEta = new SoRotationXYZ();
	//	double signEta = fabs(eta)/eta;
	//	double theta = 2.*atan(signEta*exp(-fabs(eta)));
	//	if (theta<0.)theta+=M_PI;
	//	rotationEta->axis=SoRotationXYZ::X;
	//	rotationEta->angle=theta;
	//
	//	//message("Eta: "+QString::number(eta)+" theta: "+QString::number(theta)+" phi: "+QString::number(phi));
	//
	//	updateConeHeightParameters(cone, translate, energy);
	//
	//	// play with Scale:
	//	SoScale* myScale = new SoScale();
	//	//message("Eta scaling factor is "+QString::number(etaScale));
	//	// maybe we need to squish along both axes...
	//	myScale->scaleFactor.setValue(etaScale,1.,etaScale);
	//
	//	// translate to origin vertex rather than (0,0,0)
	//	SoTranslation *transvertex = new SoTranslation();
	//	transvertex->translation = origin;
	//
	//	sep->addChild(transvertex);
	//	sep->addChild(rotationPhi);
	//	sep->addChild(rotationEta);//theta rotation done around x-axis
	//	sep->addChild(ytoz); // now it's along the z-axis
	//	sep->addChild(flip); // flip so tip at origin
	//	sep->addChild(translate); // back it up so base at origin // DO NOT MOVE THIS: ITS POSITION IS USED BY "updateConeHeightParameters(SoSeparator* sep,const double& energy)"
	//	sep->addChild(myScale); // squeeze jet according to eta
	//	//This is the point in the child sequence where we MAY add a random colour when appropriate.
	//	//Thus: The translation is the SIXTH child and the cone is the LAST child.
	//
	//	//	sep->addChild(collHandle->collSettingsButton().defaultParameterMaterial());
	//
	//	std::cout<<"About to add material: "<<collHandle->material()<<std::endl;
	//	sep->addChild(collHandle->material());
	//	// ^^ FIXME - should rearrange so we don't need to reset material
	//
	//	sep->addChild(cone);  // starts along y-axis // DO NOT MOVE THIS: ITS POSITION IS USED BY "updateConeHeightParameters(SoSeparator* sep,const double& energy)"

}

////_____________________________________________________________________________________
//void IParticleHandle_CaloCluster::Imp::updateConeHeightParameters(SoCone*cone, SoTranslation* trans, const double& energy) const
//{
//	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::Imp::updateConeHeightParameters()");
//
//	double h(scale * energy);
//
//	// FIXME: check this! when maxR is set, what should that do?? Here it's only used as "h", but maybe it should set the max length of the cone's bottom radius?? Check with Ed!
//	if (maxR > 0.0001) { // maxR > 0. is not univocally defined, because maxR is a floating point number
//		VP1Msg::messageVerbose("maxR > 0.0001 ==> setting h = min(h, maxR)");
//		h = std::min(h, maxR);
//	}
//
//	cone->height = h;
//	cone->bottomRadius = tan(coneR()) * h;
//	trans->translation = SbVec3f(0,-0.5*h, 0);
//
//
//
//	//debug
//	SbString strHeight, strRadius;
//	(cone->height).get(strHeight);
//	(cone->bottomRadius).get(strRadius);
//	std::cout << "input - energy: " << energy << " - scale: " << scale << " - maxR: " << maxR << " - h: " << h << " --- updated cone - height: " << strHeight.getString() << " - bottom radius: " << strRadius.getString() << std::endl;
//
//	// you can also use the 'writeField()' method, direct to std output
//	//SoDebug::writeField(&(cone->height));
//
//	//	QString text = "updated cone - height: " + QString::number(cone->height) + " - bottom radius: " + QString::number(cone->bottomRadius) " - translation: " + trans->translation;
//	//	VP1Msg::messageVerbose(text);
//}


////_____________________________________________________________________________________
//void IParticleHandle_CaloCluster::Imp::updateConeHeightParameters() const {
//
//
//	if (!sep) {
//		VP1Msg::messageVerbose("sep not defined. Returning.");
//		return;
//	}
//
//	//NB: The translation is the SIXTH child and the cone is the LAST child.
//	if (sep->getNumChildren()<6) {
//		VP1Msg::messageVerbose("getNumChildren() < 6!!! Returning...");
//		return;
//	}
//
//	const double energyJet = energyForLengthAndCuts();
//
//	SoNode * sixthChild = sep->getChild(5);
//	if (sixthChild->getTypeId()!=SoTranslation::getClassTypeId()) {
//		VP1Msg::messageVerbose("6th child is not a translation!!! Returning...");
//		return;
//	}
//	SoNode * lastChild = sep->getChild(sep->getNumChildren()-1);
//	if (lastChild->getTypeId()!=SoCone::getClassTypeId()) {
//		VP1Msg::messageVerbose("lastChild is not a cone!!! Returning...");
//		return;
//	}
//	updateConeHeightParameters(static_cast<SoCone*>(lastChild),static_cast<SoTranslation*>(sixthChild), energyJet);
//}


///
/// This gives the complete information about the object, shown in the main Message Box
///
//____________________________________________________________________
QStringList IParticleHandle_CaloCluster::clicked() const
{
	QStringList l;
	l << "--CaloCluster:";
	//l << IParticleHandleBase::baseInfo();

	// info and parameters,
	// they go in the "Information" column in the Browser window
	// see: http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODCaloEvent/xAODCaloEvent/versions/CaloCluster_v1.h
	//
	l +="    - pt: " + QString::number(m_d->m_cluster->pt() / SYSTEM_OF_UNITS::GeV) +" [GeV]";
	l +="    - et: " + QString::number(m_d->et() / SYSTEM_OF_UNITS::GeV) +" [GeV]";
	l +="    - eta: " + QString::number(m_d->eta());
	l +="    - phi: " + QString::number(m_d->phi());
	l +="    - m: " + QString::number(m_d->m_cluster->m() / SYSTEM_OF_UNITS::GeV) +" [GeV] (invariant mass of the particle)";
	l +="    - e: " + QString::number(m_d->m_cluster->e() / SYSTEM_OF_UNITS::GeV) +" [GeV] (total energy of the particle)";
	l +="    - rapidity: " + QString::number(m_d->m_cluster->rapidity());
	l +="    - type: " + QString::number(m_d->m_cluster->type());
	l +="    - ClusterSize: " + QString::number(m_d->m_cluster->clusterSize());
	l +="    - inBarrel: " + QString::number(m_d->m_cluster->inBarrel());
	l +="    - inEndcap: " + QString::number(m_d->m_cluster->inEndcap());

	return l;
}

/*
//____________________________________________________________________
Amg::Vector3D IParticleHandle_CaloCluster::momentum() const
{
  const Trk::Perigee& p = m_d->trackparticle->perigeeParameters();
  return p.momentum();
}
 */

//____________________________________________________________________
const xAOD::IParticle& IParticleHandle_CaloCluster::iParticle() const
{
	return *(m_d->m_cluster);
}


//____________________________________________________________________
double IParticleHandle_CaloCluster::charge() const
{
	//return m_d->trackparticle->charge(); // TODO: check in Jet interface if a "charge" quantity is defined
	return 0; // FIXME: dummy value now
}



/*
 * TODO: If Jet class has something like SummaryType like TrackParticle has, implement this method. Otherwise, remove it!
 *
 */
/*
//____________________________________________________________________
unsigned IParticleHandle_CaloCluster::summaryValue(xAOD::SummaryType type) const
{ 
	uint8_t num = 0;
	if (m_d->trackparticle->summaryValue(num,type)){
		return num;
	}
	// else...
  VP1Msg::message("IParticleHandle_CaloCluster::getSummaryValue - unable to retrieve the requested enum: "+VP1Msg::str(type));
	return 999999;
}
 */


///
/// This gives the very short summary for the object's properties, shown in the 'Information' field in the Browser, beside the item number (e.g. 'Jet0')
///
//____________________________________________________________________
QString IParticleHandle_CaloCluster::shortInfo() const
{
	QString dParameters("(");

	// info and parameters,
	// they go in the "Information" column in the Browser window
	dParameters +="pt: ";
	dParameters += QString::number(m_d->m_cluster->pt() / SYSTEM_OF_UNITS::GeV);

	dParameters +=", et: ";
	dParameters += QString::number(m_d->m_cluster->et() / SYSTEM_OF_UNITS::GeV);

	dParameters +=", eta: ";
	dParameters += QString::number(m_d->m_cluster->eta());

	dParameters +=", phi: ";
	dParameters += QString::number(m_d->m_cluster->phi());

	dParameters+="";

	dParameters+=")" ;

	return dParameters;
}


///
/// This gives the list of object's properties, shown in the 'Information' field in the Browser, once the user clicked on one particular item (e.g. 'Jet0')
///
//____________________________________________________________________
void IParticleHandle_CaloCluster::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems)
{
	IParticleHandleBase::fillObjectBrowser(listOfItems); // Obligatory!

	QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());

	// Jet "Object" title, in the Browser window
	TSOSitem->setText(0, QString("CaloCluster Parameters " ) );

	QString dParameters = shortInfo();
	dParameters += " [more info in the main Message Box]";

	TSOSitem->setText(1, dParameters );

}




//____________________________________________________________________
double IParticleHandle_CaloCluster::phi() const {
	/*VP1Msg::messageVerbose("phi: " + QString::number(m_cluster->phi()) );*/
	return m_d->phi();
}


//____________________________________________________________________
double IParticleHandle_CaloCluster::eta() const {
	/*VP1Msg::messageVerbose("eta: " + QString::number(m_cluster->eta()) );*/
	return m_d->eta();
}


//____________________________________________________________________
double IParticleHandle_CaloCluster::e() const { return m_d->e(); }


//____________________________________________________________________
double IParticleHandle_CaloCluster::energyForCuts() const {
	VP1Msg::messageDebug("IParticleHandle_CaloCluster::energyForCuts()");
	return m_d->energyForLengthAndCuts();
}


//____________________________________________________________________
double IParticleHandle_CaloCluster::et() const { return m_d->et(); }


////_____

//____________________________________________________________________
//void IParticleHandle_CaloCluster::Imp::updateMaterial()
//void IParticleHandle_CaloCluster::updateMaterial(bool isRandomColors)
void IParticleHandle_CaloCluster::updateMaterial()
{
	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::Imp::updateMaterial()");

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

	//	if ( (i>=0) == isRandomColors ) {
	//		VP1Msg::messageVerbose("(i>=0)==isRandomColors. Returning.");
	//		return;
	//	}

	//	if (!isRandomColors )
	//		m_d->sep->removeChild(m_d->m_randomMat);
	//	else
	//		m_d->sep->insertChild(m_d->m_randomMat, m_d->sep->getNumChildren()-1);
}


void IParticleHandle_CaloCluster::dumpToJSON( std::ofstream& str) const {
  str << "\"phi\":" <<m_d->phi() <<", ";
  str << "\"eta\":" <<m_d->eta() <<", ";
  str << "\"energy\":" <<m_d->e();
}

