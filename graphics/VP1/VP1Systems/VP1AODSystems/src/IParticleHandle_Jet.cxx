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
#include "VP1AODSystems/JetCollectionSettingsButton.h"

//VP1
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/SoTools.h"

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
#include <Inventor/nodes/SoSwitch.h>
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
	SoSwitch   * m_bTagged; // switch for b-tagged jets
	SoSwitch   * m_bTaggingCollSwitch; // switch for b-tagged jets


	// vars to store b-tagging weights for different taggers
	double m_bTagWeightMV1;
	double m_bTagWeightMV2c20;
	double m_bTagWeightMV2c10;
	double m_JetFitterCombNN_pb;
	double m_JetFitterCombNN_pc;
	double m_JetFitterCombNN_pu;


	//QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> > parametersAndPositions; // cache // TODO: see if useful for jets

	//Settings:
	bool considerTransverseEnergies = true; // TODO: update with button connection "E/Et" (see VP1JetCollection.cxx)
	double coneRPar = -1; // FIXME: add calculation of coneRPar, like in the old VP1 Jet
	double scale = (10.0 * Gaudi::Units::m) / (100.0*Gaudi::Units::GeV); // the default scale of all jets: 10m/100GeV
	double maxR = 0.0 * Gaudi::Units::m; // default value for maxR is 0.0

	//	SoLineSet * line;//This represents the line(s) representing the trackparticle. Can be interpolated.
	//	SoPointSet * points;//This represents the points(s) representing the trackparticle's parameters.



	// Getters
	double pt() const { return m_jet->pt(); }
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
	d->m_bTagged = 0;
	d->m_bTaggingCollSwitch = new SoSwitch();

	// get b-tagging weights for different taggers
	d->m_bTagWeightMV1 = getBTaggingWeight("MV1");
	d->m_bTagWeightMV2c20 = getBTaggingWeight("MV2c20");
	d->m_bTagWeightMV2c10 = getBTaggingWeight("MV2c10");
	d->m_JetFitterCombNN_pb = getBTaggingWeight("JetFitterCombNN_pb");
	d->m_JetFitterCombNN_pc = getBTaggingWeight("JetFitterCombNN_pc");
	d->m_JetFitterCombNN_pu = getBTaggingWeight("JetFitterCombNN_pu");


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
void IParticleHandle_Jet::setMaxR(const double& maxR) { d->maxR = maxR * Gaudi::Units::m; }

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
  if (not collHandleJet) return d->sep; //would nullptr be better?
	SbVec3f origin(0.,0.,0.);
	/* TODO: ask if origin info is present in xAOD, like in the old Jet class
	if ( d->m_jet->origin() ) {
		origin.setValue(d->m_jet->origin()->position().x(),
				d->m_jet->origin()->position().y(),
				d->m_jet->origin()->position().z());
	}
	 */
  
  const xAOD::Vertex * vtx; 
  bool exists = d->m_jet->getAssociatedObject(xAOD::JetAttribute::OriginVertex, vtx);

  if( exists && vtx ) {
    origin.setValue(vtx->position().x(),vtx->position().y(),vtx->position().z());
  }
  

	VP1Msg::messageVerbose("creating the shapes");


	/*
	 * Here the 3D cone (SoCone) for the jet has to be created (and d->sep is updated)
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


	// rendering b-tagged jets
	SoNode * lastChild = sep->getChild( sep->getNumChildren()-1 );
	if (lastChild->getTypeId() == SoCone::getClassTypeId())
	{
		VP1Msg::messageVerbose("adding b-tagging switch");

		m_bTagged = new SoSwitch();
		m_bTagged->whichChild=SO_SWITCH_ALL;

		m_bTagged->addChild(m_bTaggingCollSwitch);

		// add the b-tagging switch before the SoCone node
		sep->insertChild(m_bTagged, sep->getNumChildren()-1);
	}


	//return sep;
}

//_____________________________________________________________________________________
void IParticleHandle_Jet::Imp::updateConeHeightParameters(SoCone*cone, SoTranslation* trans, const double& energy) const
{
	VP1Msg::messageVerbose("IParticleHandle_Jet::Imp::updateConeHeightParameters()");

	double h(scale * energy);

	// FIXME: check this! when maxR is set, what should that do?? Here it's only used as "h", but maybe it should set the max length of the cone's bottom radius?? Check with Ed!
	if (maxR > 0.0001) { // maxR > 0. is not univocally defined, because maxR is a floating point number
		VP1Msg::messageVerbose("maxR > 0.0001 ==> setting h = min(h, maxR)");
		h = std::min(h, maxR);
	}

	cone->height = h;
	cone->bottomRadius = tan(coneR()) * h;
	trans->translation = SbVec3f(0,-0.5*h, 0);



	//debug
	SbString strHeight, strRadius;
	(cone->height).get(strHeight);
	(cone->bottomRadius).get(strRadius);
  // std::cout << "input - energy: " << energy << " - scale: " << scale << " - maxR: " << maxR << " - h: " << h << " --- updated cone - height: " << strHeight.getString() << " - bottom radius: " << strRadius.getString() << std::endl;

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
		VP1Msg::messageVerbose("getNumChildren() < 6!!! Returning...");
		return;
	}

	const double energyJet = energyForLengthAndCuts();

	SoNode * sixthChild = sep->getChild(5);
	if (sixthChild->getTypeId()!=SoTranslation::getClassTypeId()) {
		VP1Msg::messageVerbose("6th child is not a translation!!! Returning...");
		return;
	}
	SoNode * lastChild = sep->getChild(sep->getNumChildren()-1);
	if (lastChild->getTypeId()!=SoCone::getClassTypeId()) {
		VP1Msg::messageVerbose("lastChild is not a cone!!! Returning...");
		return;
	}
	updateConeHeightParameters(static_cast<SoCone*>(lastChild),static_cast<SoTranslation*>(sixthChild), energyJet);
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


///
/// This gives the complete information about the object, shown in the main Message Box
///
//____________________________________________________________________
QStringList IParticleHandle_Jet::clicked() const
{
	QStringList l;

	l << "--Jet:";

	//l << IParticleHandleBase::baseInfo();

	l += "   - pt: " + QString::number(d->pt());
    l += "   - e: " + QString::number(d->energy());
	l += "   - eta: " + QString::number(d->eta());
	l += "   - phi: " + QString::number(d->phi());
	l += "   - m: " + QString::number(d->m_jet->m());
	l += "   - rapidity: " + QString::number(d->m_jet->rapidity());
	l += "   - type: " + QString::number(d->m_jet->type());
	l += "   - px: " + QString::number(d->m_jet->px());
	l += "   - py: " + QString::number(d->m_jet->py());
	l += "   - pz: " + QString::number(d->m_jet->pz());
	l += "   - numConstituents: " + QString::number(d->m_jet->numConstituents());

	l += "   - SizeParameter: " + QString::number(d->m_jet->getSizeParameter());
//	l += "   - SizeParameter: " + QString::number(int(d->m_jet->getSizeParameter()*10));

	xAOD::JetAlgorithmType::ID jetAlgID =  d->m_jet->getAlgorithmType();
	std::string algName = xAOD::JetAlgorithmType::algName(jetAlgID);
	l += "   - AlgorithmType: " + QString::fromStdString( algName );

	 xAOD::JetInput::Type jetAlgType = d->m_jet->getInputType();
	std::string inputType =  xAOD::JetInput::typeName(jetAlgType);
	l += "   - InputType: " + QString::fromStdString( inputType );


	l += "   - 'MV2c20' b-tagging weight: " + QString::number( d->m_bTagWeightMV2c20 );
	l += "   - 'MV2c10' b-tagging weight: " + QString::number( d->m_bTagWeightMV2c10 );
	l += "   - 'MV1' b-tagging weight: " + QString::number( d->m_bTagWeightMV1 );


	l << "------";

	return l;
}

///
/// This gives the very short summary for the object's properties, shown in the 'Information' field in the Browser, beside the item number (e.g. 'Jet0')
///
//____________________________________________________________________
QString IParticleHandle_Jet::shortInfo() const
{

	QString l;

    l += "pt: " + QString::number(d->pt());
	l += ", e: " + QString::number(d->energy());
	l += ", eta: " + QString::number(d->eta());
	l += ", phi: " + QString::number(d->phi());
	l += ", MV2c20: " + QString::number( d->m_bTagWeightMV2c20 );
	l += ", MV2c10: " + QString::number( d->m_bTagWeightMV2c10 );
	l += ", MV1: " + QString::number( d->m_bTagWeightMV1 );

	return l;
}


///
/// This gives the list of object's properties, shown in the 'Information' field in the Browser, once the user clicked on one particular item (e.g. 'Jet0')
///
//____________________________________________________________________
void IParticleHandle_Jet::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) 
{
	IParticleHandleBase::fillObjectBrowser(listOfItems); // Obligatory!

	QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());

	// Jet "Object" title, in the Browser window
	TSOSitem->setText(0, QString("Info:" ) );

	QString dParameters("(");

	// jet info and parameters,
	// they go in the "Information" column in the Browser window
	// see: http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODJet/xAODJet/versions/Jet_v1.h
	//

	dParameters+="pt: ";
	dParameters+=QString::number(d->pt());
	dParameters+=", e: ";
	dParameters+=QString::number(d->energy());
	dParameters+=", eta: ";
	dParameters+=QString::number(d->eta());
	dParameters+=", phi: ";
	dParameters+=QString::number(d->phi());
	dParameters+=", m: ";
	dParameters+=QString::number(d->m_jet->m());
	dParameters+=", rapidity: ";
	dParameters+=QString::number(d->m_jet->rapidity());

	dParameters+="";

	dParameters+=")";

	dParameters += " [more info in the main Message Box]";

	TSOSitem->setText(1, dParameters );


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

	// TODO: Move to HSV system, instead of RGB, and make random colors more contrasting


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


//____________________________________________________________________
void IParticleHandle_Jet::updateBTaggingSwitch(SoSwitch *bTaggingSwitch)
{
	VP1Msg::messageVerbose("IParticleHandle_Jet::updateBTaggingSwitch()");

	std::cout << "old switch: " << d->m_bTaggingCollSwitch << " - new: " << bTaggingSwitch << std::endl;

	// remove the old switch
	d->m_bTagged->removeChild(d->m_bTaggingCollSwitch);

	// updating the jet switch with the Coll switch
	d->m_bTaggingCollSwitch = 0;
	d->m_bTaggingCollSwitch = bTaggingSwitch;

	// add the new switch to the internal b-tagging switch
	d->m_bTagged->addChild( bTaggingSwitch );

}


//____________________________________________________________________
void IParticleHandle_Jet::updateBTagging(const std::string& bTaggingTagger, const double& bTaggingCut)
{
	VP1Msg::messageVerbose("IParticleHandle_Jet::updateBTagging()");

	// get the b-tagging "weight" (the discriminant)
	//double bTaggingWeight = 0.99; // dummy value for debug only!!!
	double bTaggingWeight = getBTaggingWeight(bTaggingTagger); // actual value

    std::cout << "B-TAG UPDATE - jet eta: " << d->eta() << ", phi: " << d->phi() << " - tagger: " << bTaggingTagger << " - cut: " << bTaggingCut << " - weight: " << bTaggingWeight;

	if (bTaggingWeight > bTaggingCut) {
		d->m_bTagged->whichChild = SO_SWITCH_ALL;
		std::cout << " ON" << std::endl;
	}
	else {
		d->m_bTagged->whichChild = SO_SWITCH_NONE;
		std::cout << " OFF" << std::endl;
	}

	// for debug: dumping the content of the whole node
	// SoTools::dumpNode(d->sep);
}

//____________________________________________________________________
double IParticleHandle_Jet::getBTaggingWeight(std::string tagger)
{
    double weight = 0.0;

	const xAOD::BTagging * myBTag = nullptr;
    myBTag = d->m_jet->btagging();

   if (myBTag == nullptr) {
    VP1Msg::messageWarningRed("It was not possible to access the pointer to b-tagging info, for the selected collection! Returning 'weight': 0.0"); //("It was not possible to access the tagger '"+ tagger +"' for the selected collection: " + d->m_jet->getInputType() + d->m_jet->getAlgorithmType() );
    return weight;
   }
    
	

	// TODO: add the other taggers

//    if (tagger == "MV1")
//		weight = myBTag->MV1_discriminant();

	/* these methods have been removed in xAODBTagging-00-00-35 (cfr. SVN changesets 797165 + 801102)
	else if (tagger == "JetFitterCombNN_pb")
		weight = myBTag->JetFitterCombNN_pb();
	else if (tagger == "JetFitterCombNN_pc")
		weight = myBTag->JetFitterCombNN_pc();
	else if (tagger == "JetFitterCombNN_pu")
		weight = myBTag->JetFitterCombNN_pu();
	*/

//    else if (tagger=="MV2c20")
    if (tagger=="MV2c20")
		/*const bool hasMv2c20 =*/ myBTag->MVx_discriminant("MV2c20", weight);
    else if (tagger=="MV2c10")
		/*const bool hasMv2c10 =*/ myBTag->MVx_discriminant("MV2c10", weight);
    else if (tagger=="MV2c00")
	    /*const bool hasMv2c00 =*/ myBTag->MVx_discriminant("MV2c00", weight);
	else
		VP1Msg::message("Tagger '" + QString::fromStdString(tagger) + "' not found! Returning weight=0.0 ...");

	return weight;



}

void IParticleHandle_Jet::dumpToJSON( std::ofstream& str) const {
  str << "\"coneR\":"<<d->coneR() <<", ";
  str << "\"phi\":" <<d->phi() <<", ";
  str << "\"eta\":" <<d->eta() <<", ";
  str << "\"energy\":" <<d->energyForLengthAndCuts();
}


