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




//____________________________________________________________________
class IParticleHandle_CaloCluster::Imp {
public:
	// constructor and default values
	//Imp () : theclass(0), controller(0), scale(1.0), bTaggingTagger("MV1"),bTaggingCut(0.98),randomColours(false),bTaggingSwitch(0),bTaggingTexture(0), bTaggingMaterial(0) {} // NOTE: parameters from the old JetCollection
	//Imp () : theclass(0), scale(1.0) {} // TODO: implement the parameters above, from the old JetCollection

	IParticleHandle_CaloCluster * theclass; // the Jet class
	const IParticleCollHandle_CaloCluster* theCollHandle; // the CollHandle instance

	const xAOD::CaloCluster * m_cluster; // the single jet
//	const CaloCluster* m_cluster;

	SoSeparator * sep; // everything hangs from this.
//	SoCone * cone;//This represents the cone representing the jet.
	SoGenericBox* m_genericBox; //This is the box representing the calorimeter cluster.

//	SoMaterial * m_randomMat; // random material for jets
//	SoSwitch   * m_bTagged; // switch for b-tagged jets
//	SoSwitch   * m_bTaggingCollSwitch; // switch for b-tagged jets


	double m_transverseEnergy;

	//QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parametersAndPositions; // cache // TODO: see if useful for jets

	//Settings:
	bool considerTransverseEnergies = true; // TODO: update with button connection "E/Et" (see VP1JetCollection.cxx)
//	double coneRPar = -1; // FIXME: add calculation of coneRPar, like in the old VP1 Jet
//	double scale = (10.0 * Gaudi::Units::m) / (100.0*Gaudi::Units::GeV); // the default scale of all jets: 10m/100GeV
//	double maxR = 0.0 * Gaudi::Units::m; // default value for maxR is 0.0

	//	SoLineSet * line;//This represents the line(s) representing the trackparticle. Can be interpolated.
	//	SoPointSet * points;//This represents the points(s) representing the trackparticle's parameters.



	// Getters
	const xAOD::CaloCluster* cluster() const { return m_cluster; }
	SoGenericBox * genericBox() const { return m_genericBox; }
	double phi() const { /*VP1Msg::messageVerbose("phi: " + QString::number(m_cluster->phi()) );*/ return m_cluster->phi(); }
	double eta() const { /*VP1Msg::messageVerbose("eta: " + QString::number(m_cluster->eta()) );*/ return m_cluster->eta(); }
	double energy() const { return m_cluster->e(); }
	double transverseEnergy() const { return m_transverseEnergy; }
//	double transverseEnergy() const { return sin(2*atan(exp(-fabs(eta()))))*energy(); }
//	double coneR() const { return coneRPar > 0 ? coneRPar : 0.4; }

	double energyForLengthAndCuts(const IParticleCollHandle_CaloCluster* coll_handle) { return coll_handle->isConsiderTransverseEnergy() ? transverseEnergy() : energy(); } // FIXME: is this def used at all??
	double energyForLengthAndCuts() { return theCollHandle->isConsiderTransverseEnergy() ? transverseEnergy() : energy(); }

//	double energyForLengthAndCuts() const { return considerTransverseEnergies ? transverseEnergy() : energy(); }

	//Create/manipulate 3D objects:
//	void createShapeFromJetParameters(const IParticleCollHandle_CaloCluster* collHandle, const double& coneR, const double& eta, const double& phi, const double& energy, const SbVec3f& origin);
//	void updateConeHeightParameters(SoCone*, SoTranslation*, const double& energy) const;
//	void updateConeHeightParameters() const;

	// Create/manipulate 3D objects
	void createShapeFromParameters(const IParticleCollHandle_CaloCluster* coll_handle);
	void updateShapePars(const IParticleCollHandle_CaloCluster* coll_handle);

};

//____________________________________________________________________
IParticleHandle_CaloCluster::IParticleHandle_CaloCluster(IParticleCollHandleBase* ch, const xAOD::CaloCluster *cluster)
: IParticleHandleBase(ch), d(new Imp)
{ 
	//	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::IParticleHandle_CaloCluster() - constructor");

	// set the Imp members
	d->theclass = this;
	d->m_cluster = cluster;
	d->sep = 0;
	d->m_genericBox = 0;
//	d->cone = 0;


	// calculate the transverse energy
	d->m_transverseEnergy = sin(2*atan(exp(-fabs( d->eta() )))) * d->energy();



//	d->m_randomMat = 0;
//	d->m_bTagged = 0;
//	d->m_bTaggingCollSwitch = new SoSwitch();

//	d->theCollHandle = dynamic_cast<const IParticleCollHandle_CaloCluster*>(collHandle());
}

//____________________________________________________________________
IParticleHandle_CaloCluster::~IParticleHandle_CaloCluster()
{
	//	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::~IParticleHandle_CaloCluster() - destructor");
//	if (d->m_randomMat) d->m_randomMat->unref();
//	if (d->cone) d->cone->unref();
	if (d->m_genericBox) d->m_genericBox->unref();
	if (d->sep) d->sep->unref();
	delete d;
}



//____________________________________________________________________
SoGenericBox* IParticleHandle_CaloCluster::genericBox() const
{
	return d->genericBox();
}


// Setter
////____________________________________________________________________
//void IParticleHandle_CaloCluster::setScale( const double& sc) { d->scale = sc; }

//____________________________________________________________________
//void IParticleHandle_CaloCluster::setMaxR(const double& maxR) { d->maxR = maxR * Gaudi::Units::m; }

//____________________________________________________________________
//void IParticleHandle_CaloCluster::rerandomiseMaterial() {d->rerandomiseMaterial(); }

//____________________________________________________________________
bool IParticleHandle_CaloCluster::has3DObjects()
{
	//	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::has3DObjects()");
	return 0 != d->sep;
}


//____________________________________________________________________
void IParticleHandle_CaloCluster::clear3DObjects(){
	//	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::clear3DObjects()");

//	if (d->m_randomMat) {
//		d->m_randomMat->unref();
//		d->m_randomMat = 0;
//	}
	if (d->m_genericBox) {
		d->m_genericBox->unref();
		d->m_genericBox = 0;
	}
	if (d->sep) {
		d->sep->unref();
		d->sep = 0;
	}

}



/*
 * This is the method which makes the 3D objects from the measurements
 */
//____________________________________________________________________
SoNode* IParticleHandle_CaloCluster::nodes(){

	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::nodes()");

	if (d->sep) {
		VP1Msg::messageVerbose("d->sep already defined (" + VP1Msg::str(d->sep) + "). Returning d->sep.");
		return d->sep; // FIXME - do we need to check if anything need to be redrawn?
	}
	if (!d->sep) {
		VP1Msg::messageVerbose("d->sep not defined. Creating shapes and a new d->sep.");
		d->sep = new SoSeparator();
		d->sep->ref();
	}

	const IParticleCollHandle_CaloCluster* coll_handle = dynamic_cast<const IParticleCollHandle_CaloCluster*>(collHandle());

	//SbVec3f origin(0.,0.,0.); // it seems it is not used here...
	/* TODO: ask if origin info is present in xAOD, like in the old Jet class
	if ( d->m_cluster->origin() ) {
		origin.setValue(d->m_cluster->origin()->position().x(),
				d->m_cluster->origin()->position().y(),
				d->m_cluster->origin()->position().z());
	}
	 */

	VP1Msg::messageVerbose("creating the shapes");


	/*
	 * Build the 3D objects for the physics object, and update d->sep.
	 */
//	d->createShapeFromJetParameters(collHandle, d->coneR(), d->eta(), d->phi(), d->energyForLengthAndCuts(), origin);
//	d->createShapeFromParameters(coll_handle, origin);
	d->createShapeFromParameters(coll_handle);


	return d->sep;
}


////____________________________________________________________________
//void IParticleHandle_CaloCluster::updateHeight() {
//	d->updateConeHeightParameters();
//}

void IParticleHandle_CaloCluster::updateShape(IParticleCollHandle_CaloCluster* coll)
{
	VP1Msg::messageDebug("IParticleHandle_CaloCluster::updateShape()");
	d->updateShapePars(coll);
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
			m_genericBox = new SoGenericBox();
			m_genericBox->drawEdgeLines = coll_handle->showOutlines();
			m_genericBox->forceEdgeLinesInBaseColour = true;
			m_genericBox->ref();
		}

	//	m_attached = true;
	updateShapePars(coll_handle);
	//	d->sephelper->addNode(m_genericBox);


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



//____________________________________________________________________
QStringList IParticleHandle_CaloCluster::clicked() const
{
	QStringList l;
	l << "--Jet:";
	l << IParticleHandleBase::baseInfo();
	return l;
}

/*
//____________________________________________________________________
Amg::Vector3D IParticleHandle_CaloCluster::momentum() const
{
  const Trk::Perigee& p = d->trackparticle->perigeeParameters();
  return p.momentum();
}
 */

//____________________________________________________________________
const xAOD::IParticle& IParticleHandle_CaloCluster::iParticle() const
{
	return *(d->m_cluster);
}


//____________________________________________________________________
double IParticleHandle_CaloCluster::charge() const
{
	//return d->trackparticle->charge(); // TODO: check in Jet interface if a "charge" quantity is defined
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
	if (d->trackparticle->summaryValue(num,type)){
		return num;
	}
	// else...
  VP1Msg::message("IParticleHandle_CaloCluster::getSummaryValue - unable to retrieve the requested enum: "+VP1Msg::str(type));
	return 999999;
}
 */



//____________________________________________________________________
QString IParticleHandle_CaloCluster::shortInfo() const
{
	/*
  QString l("|P|="+VP1Msg::str(momentum().mag()/Gaudi::Units::GeV)+" [GeV], ");
  l+= "Pix["+QString::number(getNPixelHits())+"], SCT["+QString::number(getNSCTHits())+"], TRT["+QString::number(getNTRTHits())
   +"], Muon prec. layers/holes ["+QString::number(getNMuonPrecisionLayers())+"/"+QString::number(getNMuonPrecisionHoleLayers())+"]";
	 */
	QString l("xAOD Jet test"); // TODO: implement a meaningful short_info for jets
	return l;
}



//____________________________________________________________________
void IParticleHandle_CaloCluster::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems)
{
	IParticleHandleBase::fillObjectBrowser(listOfItems); // Obligatory!

	QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());

	// Jet "Object" title, in the Browser window
	TSOSitem->setText(0, QString("Def. Parameters " ) );

	QString dParameters("(");

	/*
	 * TODO: check the Jets parameters and add them here
	 */
	/*
  dParameters+=QString::number(d->trackparticle->d0());
  dParameters+=", ";     
  dParameters+=QString::number(d->trackparticle->z0());
  dParameters+=", ";     
  dParameters+=QString::number(d->trackparticle->phi0());
  dParameters+=", ";     
  dParameters+=QString::number(d->trackparticle->theta());
  dParameters+=", ";     
  dParameters+=QString::number(d->trackparticle->qOverP());
	 */

	// jet info and parameters,
	// they go in the "Information" column in the Browser window
	dParameters+="jets parameters go here!";

	dParameters+=")";
	TSOSitem->setText(1, dParameters );

	/*
	 * TODO: check jets parameters
	 */
	/*
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
	 */


	// TODO - add more.
}




//____________________________________________________________________
double IParticleHandle_CaloCluster::phi() const {
	/*VP1Msg::messageVerbose("phi: " + QString::number(m_cluster->phi()) );*/
	return d->phi();
}


//____________________________________________________________________
double IParticleHandle_CaloCluster::eta() const {
	/*VP1Msg::messageVerbose("eta: " + QString::number(m_cluster->eta()) );*/
	return d->eta();
}


//____________________________________________________________________
double IParticleHandle_CaloCluster::energy() const { return d->energy(); }


//____________________________________________________________________
double IParticleHandle_CaloCluster::energyForCuts() const { return d->energyForLengthAndCuts(); }


//____________________________________________________________________
double IParticleHandle_CaloCluster::transverseEnergy() const { return d->transverseEnergy(); } //sin(2*atan(exp(-fabs(eta()))))*energy();


////_____

//____________________________________________________________________
//void IParticleHandle_CaloCluster::Imp::updateMaterial()
//void IParticleHandle_CaloCluster::updateMaterial(bool isRandomColors)
void IParticleHandle_CaloCluster::updateMaterial()
{
	VP1Msg::messageVerbose("IParticleHandle_CaloCluster::Imp::updateMaterial()");

	// check if we have 3D objects; if not, return
	if ( d->sep == 0 )
		return;

//	if (!isRandomColors && !d->m_randomMat)
//		return;//m_randomMat can never have been attached
//
//	if (isRandomColors && !d->m_randomMat) {
//		d->m_randomMat = new SoMaterial;
//		d->m_randomMat->ref();
//		rerandomiseMaterial();
//	}


//	int i = d->sep->findChild(d->m_randomMat);

//	if ( (i>=0) == isRandomColors ) {
//		VP1Msg::messageVerbose("(i>=0)==isRandomColors. Returning.");
//		return;
//	}

//	if (!isRandomColors )
//		d->sep->removeChild(d->m_randomMat);
//	else
//		d->sep->insertChild(d->m_randomMat, d->sep->getNumChildren()-1);
}




