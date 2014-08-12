/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleHandle_Jet               //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

// Local
#include "IParticleHandle_Jet.h"
#include "IParticleCollHandle_Jet.h"
#include "AODSysCommonData.h"
#include "JetCollectionSettingsButton.h"

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
#include <Inventor/nodes/SoScale.h>
#include <SoDebug.h> // it's stored at /afs/cern.ch/sw/lcg/external/coin3d/3.1.3p2/x86_64-slc6-gcc47-opt/include/SoDebug.h





//____________________________________________________________________
class IParticleHandle_Jet::Imp {
public:
	// constructor and default values
	//Imp () : theclass(0), controller(0), scale(1.0), bTaggingTagger("MV1"),bTaggingCut(0.98),randomColours(false),bTaggingSwitch(0),bTaggingTexture(0), bTaggingMaterial(0) {} // NOTE: parameters from the old JetCollection
	//Imp () : theclass(0), scale(1.0) {} // TODO: implement the parameters above, from the old JetCollection

	IParticleHandle_Jet * theclass; // the Jet class
	const IParticleCollHandle_Jet* theCollHandle; // the CollHandle instance

	const xAOD::Jet * m_jet; // the single jet

	SoSeparator * sep; // everything hangs from this.
	SoCone * cone;//This represents the cone representing the jet.
	SoMaterial * m_randomMat; // random material for jets


	//QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parametersAndPositions; // cache // TODO: see if useful for jets

	//Settings:
	bool considerTransverseEnergies = true; // TODO: update with button connection "E/Et" (see VP1JetCollection.cxx)
	double coneRPar = -1; // FIXME: add calculation of coneRPar, like in the old VP1 Jet
	double scale = 1.0; // default
	double maxR;


	//	SoLineSet * line;//This represents the line(s) representing the trackparticle. Can be interpolated.
	//	SoPointSet * points;//This represents the points(s) representing the trackparticle's parameters.


	// Getters
	double phi() const { /*VP1Msg::messageVerbose("phi: " + QString::number(m_jet->phi()) );*/ return m_jet->phi(); }
	double eta() const { /*VP1Msg::messageVerbose("eta: " + QString::number(m_jet->eta()) );*/ return m_jet->eta(); }
	double energy() const { return m_jet->e(); }
	double transverseEnergy() const { return sin(2*atan(exp(-fabs(eta()))))*energy(); }
	double coneR() const { return coneRPar > 0 ? coneRPar : 0.4; }
	double energyForLengthAndCuts() const { return considerTransverseEnergies ? transverseEnergy() : energy(); }

	//Create/manipulate 3D objects:
	void createShapeFromJetParameters(const IParticleCollHandle_Jet* collHandleJet, const double& coneR, const double& eta, const double& phi, const double& energy, const SbVec3f& origin);
	void updateConeHeightParameters(SoCone*, SoTranslation*, const double& energy) const;
	void updateConeHeightParameters() const;

	// Update random material
	void rerandomiseMaterial();


};

//____________________________________________________________________
IParticleHandle_Jet::IParticleHandle_Jet(IParticleCollHandleBase* ch, const xAOD::Jet *jet)
: IParticleHandleBase(ch), d(new Imp)
{ 
	//	VP1Msg::messageVerbose("IParticleHandle_Jet::IParticleHandle_Jet() - constructor");
	d->theclass = this;
	d->m_jet = jet;

	d->sep = 0;
	d->cone = 0;
	d->m_randomMat = 0;

//	d->theCollHandle = dynamic_cast<const IParticleCollHandle_Jet*>(collHandle());
}

//____________________________________________________________________
IParticleHandle_Jet::~IParticleHandle_Jet()
{
	//	VP1Msg::messageVerbose("IParticleHandle_Jet::~IParticleHandle_Jet() - destructor");
	if (d->m_randomMat) d->m_randomMat->unref();
	if (d->cone) d->cone->unref();
	if (d->sep) d->sep->unref();
	delete d;
}




// Setter
//____________________________________________________________________
void IParticleHandle_Jet::setScale( const double& sc) { d->scale = sc; }

//____________________________________________________________________
void IParticleHandle_Jet::rerandomiseMaterial() {d->rerandomiseMaterial(); }

//____________________________________________________________________
bool IParticleHandle_Jet::has3DObjects()
{
	//	VP1Msg::messageVerbose("IParticleHandle_Jet::has3DObjects()");
	return 0 != d->sep;
}


//____________________________________________________________________
void IParticleHandle_Jet::clear3DObjects(){
	//	VP1Msg::messageVerbose("IParticleHandle_Jet::clear3DObjects()");

	if (d->m_randomMat) {
		d->m_randomMat->unref();
		d->m_randomMat = 0;
	}
	if (d->cone) {
		d->cone->unref();
		d->cone = 0;
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
SoNode* IParticleHandle_Jet::nodes(){

	VP1Msg::messageVerbose("IParticleHandle_Jet::nodes()");

	if (d->sep) {
		VP1Msg::messageVerbose("d->sep already defined (" + VP1Msg::str(d->sep) + "). Returning d->sep.");
		return d->sep; // FIXME - do we need to check if anything need to be redrawn?
	}
	if (!d->sep) {
		VP1Msg::messageVerbose("d->sep not defined. Creating shapes and a new d->sep.");
		d->sep = new SoSeparator();
		d->sep->ref();
	}

	const IParticleCollHandle_Jet* collHandleJet = dynamic_cast<const IParticleCollHandle_Jet*>(collHandle());

	SbVec3f origin(0.,0.,0.);
	/* TODO: ask if origin info is present in xAOD, like in the old Jet class
	if ( d->m_jet->origin() ) {
		origin.setValue(d->m_jet->origin()->position().x(),
				d->m_jet->origin()->position().y(),
				d->m_jet->origin()->position().z());
	}
	 */

	VP1Msg::messageVerbose("creating the shapes");


	/*
	 * Here the 3D cone (SoCone) for the jet has to be created
	 */
	d->createShapeFromJetParameters(collHandleJet, d->coneR(), d->eta(), d->phi(), d->energyForLengthAndCuts(), origin);

	return d->sep;
}

//____________________________________________________________________
void IParticleHandle_Jet::updateHeight() {
	d->updateConeHeightParameters();
}


//____________________________________________________________________
void IParticleHandle_Jet::Imp::createShapeFromJetParameters(const IParticleCollHandle_Jet* collHandleJet, const double& inputconeR, const double& eta,
		const double& phi, const double& energy, const SbVec3f& origin)
{
	VP1Msg::messageVerbose("IParticleHandle_Jet::Imp::createShapeFromJetParameters()");



	sep = new SoSeparator();
	sep->ref();

	cone = new SoCone();
	cone->ref();

	// coneR IS the opening half-angle of the jet, in delta phi (easy) and in
	//  delta eta (trickier)
	// to try to get the jet extent right in eta, find theta corresponding
	//  to eta+R, eta-R, and take half of the difference:
	double thetaMax = 2.*atan(exp(-(eta+inputconeR)));
	double thetaMin = 2.*atan(exp(-(eta-inputconeR)));
	double deltaTheta = fabs(thetaMax-thetaMin);
	double etaScale = deltaTheta/(2.*inputconeR);

	// Translate by half cone height and flip 180 deg so point is at IP:
	SoTranslation *translate = new SoTranslation();

	SoRotationXYZ *flip = new SoRotationXYZ();
	flip->axis=SoRotationXYZ::Z;
	flip->angle=M_PI;

	SoRotationXYZ *ytoz = new SoRotationXYZ();
	ytoz->axis=SoRotationXYZ::X;
	ytoz->angle=M_PI/2.;

	// Cones should now be along Z-axis,point at IP.
	SoRotationXYZ *rotationPhi = new SoRotationXYZ();
	rotationPhi->axis=SoRotationXYZ::Z;
	rotationPhi->angle = phi+M_PI/2.; // starts from -y-axis in x-y plane
	// phi is measured from x-axis, so +M_PI/2

	SoRotationXYZ *rotationEta = new SoRotationXYZ();
	double signEta = fabs(eta)/eta;
	double theta = 2.*atan(signEta*exp(-fabs(eta)));
	if (theta<0.)theta+=M_PI;
	rotationEta->axis=SoRotationXYZ::X;
	rotationEta->angle=theta;

	//message("Eta: "+QString::number(eta)+" theta: "+QString::number(theta)+" phi: "+QString::number(phi));

	updateConeHeightParameters(cone, translate, energy);

	// play with Scale:
	SoScale* myScale = new SoScale();
	//message("Eta scaling factor is "+QString::number(etaScale));
	// maybe we need to squish along both axes...
	myScale->scaleFactor.setValue(etaScale,1.,etaScale);

	// translate to origin vertex rather than (0,0,0)
	SoTranslation *transvertex = new SoTranslation();
	transvertex->translation = origin;

	sep->addChild(transvertex);
	sep->addChild(rotationPhi);
	sep->addChild(rotationEta);//theta rotation done around x-axis
	sep->addChild(ytoz); // now it's along the z-axis
	sep->addChild(flip); // flip so tip at origin
	sep->addChild(translate); // back it up so base at origin // DO NOT MOVE THIS: ITS POSITION IS USED BY "updateConeHeightParameters(SoSeparator* sep,const double& energy)"
	sep->addChild(myScale); // squeeze jet according to eta
	//This is the point in the child sequence where we MAY add a random colour when appropriate.
	//Thus: The translation is the SIXTH child and the cone is the LAST child.

	//	sep->addChild(collHandleJet->collSettingsButton().defaultParameterMaterial());

	std::cout<<"About to add material: "<<collHandleJet->material()<<std::endl;
	sep->addChild(collHandleJet->material());
	// ^^ FIXME - should rearrange so we don't need to reset material

	sep->addChild(cone);  // starts along y-axis // DO NOT MOVE THIS: ITS POSITION IS USED BY "updateConeHeightParameters(SoSeparator* sep,const double& energy)"

	//return sep;
}

//_____________________________________________________________________________________
void IParticleHandle_Jet::Imp::updateConeHeightParameters(SoCone*cone, SoTranslation* trans, const double& energy) const
{
	VP1Msg::messageVerbose("IParticleHandle_Jet::Imp::updateConeHeightParameters()");

	double h(scale * energy);

	if (maxR > 0.0001) { // maxR > 0. is not univocally defined, because maxR is a floating point number
		VP1Msg::messageVerbose("maxR > 0.0001 ==> setting h = min(h,maxR)");
		h = std::min(h,maxR);
	}

	//h = 1500.0; // only for debug

	cone->height = h;
	cone->bottomRadius = tan(coneR()) * h;
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
void IParticleHandle_Jet::Imp::updateConeHeightParameters() const {


	if (!sep) {
		VP1Msg::messageVerbose("sep not defined. Returning.");
		return;
	}

	//NB: The translation is the SIXTH child and the cone is the LAST child.
	if (sep->getNumChildren()<6) {
		VP1Msg::messageVerbose("getNumChildren() < 6!!!");
		return;
	}

	const double energyJet = energyForLengthAndCuts();

	SoNode * sixthChild = sep->getChild(5);
	if (sixthChild->getTypeId()!=SoTranslation::getClassTypeId()) {
		return;
	}
	SoNode * lastChild = sep->getChild(sep->getNumChildren()-1);
	if (lastChild->getTypeId()!=SoCone::getClassTypeId()) {
		return;
	}
	updateConeHeightParameters(static_cast<SoCone*>(lastChild),static_cast<SoTranslation*>(sixthChild), energyJet);
}



//____________________________________________________________________
QStringList IParticleHandle_Jet::clicked() const
{
	QStringList l;
	l << "--Jet:";
	l << IParticleHandleBase::baseInfo();
	return l;
}

/*
//____________________________________________________________________
Amg::Vector3D IParticleHandle_Jet::momentum() const
{
  const Trk::Perigee& p = d->trackparticle->perigeeParameters();
  return p.momentum();
}
 */

//____________________________________________________________________
const xAOD::IParticle& IParticleHandle_Jet::iParticle() const
{
	return *(d->m_jet);
}


//____________________________________________________________________
double IParticleHandle_Jet::charge() const
{
	//return d->trackparticle->charge(); // TODO: check in Jet interface if a "charge" is defined
	return 0; // FIXME: dummy value now
}



/*
 * TODO: If Jet class has something like SummaryType like TrackParticle has, implement this method. Otherwise, remove it!
 *
 */
/*
//____________________________________________________________________
unsigned IParticleHandle_Jet::summaryValue(xAOD::SummaryType type) const
{ 
	uint8_t num = 0;
	if (d->trackparticle->summaryValue(num,type)){
		return num;
	}
	// else...
  VP1Msg::message("IParticleHandle_Jet::getSummaryValue - unable to retrieve the requested enum: "+VP1Msg::str(type));
	return 999999;
}
 */



//____________________________________________________________________
QString IParticleHandle_Jet::shortInfo() const
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
void IParticleHandle_Jet::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) 
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
double IParticleHandle_Jet::phi() const {
	/*VP1Msg::messageVerbose("phi: " + QString::number(m_jet->phi()) );*/
	return d->phi();
}


//____________________________________________________________________
double IParticleHandle_Jet::eta() const {
	/*VP1Msg::messageVerbose("eta: " + QString::number(m_jet->eta()) );*/
	return d->eta();
}


//____________________________________________________________________
double IParticleHandle_Jet::energy() const { return d->energy(); }


//____________________________________________________________________
double IParticleHandle_Jet::energyForCuts() const { return d->energyForLengthAndCuts(); }


//____________________________________________________________________
double IParticleHandle_Jet::transverseEnergy() const { return d->transverseEnergy(); } //sin(2*atan(exp(-fabs(eta()))))*energy();


//____________________________________________________________________
void IParticleHandle_Jet::Imp::rerandomiseMaterial()
{
	VP1Msg::messageVerbose("IParticleHandle_Jet::Imp::rerandomiseMaterial()");

	//Fixme: share this code with other systems!!
	if ( !m_randomMat ) { //We will anyway rerandomize it when we need it
		VP1Msg::messageVerbose("'m_randomMat not set. Returning.");
		return;
	}

	double r2 = 0.3*0.3;
	unsigned i(0);
	double r,g,b;
	bool ok;
	while (true) {
		r = (rand() / static_cast<double>(RAND_MAX));
		g = (rand() / static_cast<double>(RAND_MAX));
		b = (rand() / static_cast<double>(RAND_MAX));
		ok = true;
		//For now we make sure that we avoid black and red. This should be updated from bgd and highlight col automatically! (fixme).
		// -> and we should probably also make sure that tracks close in (eta,phi) are well separated in colour-space.
		if ( (r-1.0)*(r-1.0)+g*g+b*b < r2*0.5 )//avoid red (distance)
			ok = false;
		else if ( r*r/(r*r+g*g+b*b) > 0.8 )//avoid red (angle)
			ok = false;
		else if ( r*r+g*g+b*b < r2*2.0 )//avoid black
			ok = false;
		if (ok)
			break;
		++i;
		if (i>50 ) {
			r2 *= 0.99;//To avoid problem in case we add too many forbidden spheres.
			if (i>1000) {
				//Just a safety
				VP1Msg::messageVerbose("IParticleHandle_Jet::Imp::rerandomiseMaterial() - Warning: Random colour could"
						" not be selected such as to satisfy all separation criterias");
				break;
			}
		}
	}

	VP1MaterialButton::setMaterialParameters(m_randomMat,r, g, b, 0.1, 0.5 );
}


//____________________________________________________________________
//void IParticleHandle_Jet::Imp::updateMaterial()
void IParticleHandle_Jet::updateMaterial(bool isRandomColors)
{
	VP1Msg::messageVerbose("IParticleHandle_Jet::Imp::updateMaterial()");

	// check if we have 3D objects; if not, return
	if ( d->sep == 0 )
		return;

	if (!isRandomColors && !d->m_randomMat)
		return;//m_randomMat can never have been attached

	if (isRandomColors && !d->m_randomMat) {
		d->m_randomMat = new SoMaterial;
		d->m_randomMat->ref();
		rerandomiseMaterial();
	}


	int i = d->sep->findChild(d->m_randomMat);

	if ( (i>=0) == isRandomColors ) {
		VP1Msg::messageVerbose("(i>=0)==isRandomColors. Returning.");
		return;
	}

	if (!isRandomColors )
		d->sep->removeChild(d->m_randomMat);
	else
		d->sep->insertChild(d->m_randomMat, d->sep->getNumChildren()-1);
}
