/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HitCreatorSilicon.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasToolsID/HitCreatorSilicon.h"

// ISF
#include "ISF_Event/ISFParticle.h"
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"

// Tracking
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/DefinedParameter.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkExUtils/LineIntersection2D.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
// InDet
// (0) InDet(DD)
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDiodesParameters.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetSimEvent/SiHit.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandLandau.h"
#include <TMath.h>
#include <TF1.h>
#include <vector>


double langaufun_fast(double* x, double* par);
//================ Constructor =================================================
iFatras::HitCreatorSilicon::HitCreatorSilicon(const std::string& t, const std::string& n, const IInterface*  p ) :
  base_class(t,n,p),
  m_incidentSvc("IncidentSvc", n),
  m_hitColl(0),
  m_collectionName("PixelHits"),
  m_randomSvc("AtRndmGenSvc", n),
  m_randomEngineName("FatrasRnd"),
  m_randomEngine(0),
  m_siIdHelperName("PixelID"),
  m_pixIdHelper(0),
  m_sctIdHelper(0),
  m_condSummarySvc("PixelConditionsSummarySvc", n),
  m_useConditionsSvc(true),
  m_siPathToCharge(500.),
  m_fastEnergyDepositionModel(true)
{
    // The Hit Collection Name
    declareProperty("CollectionName",               m_collectionName = "PixelHits");
    // Random number svc 
    declareProperty("RandomNumberService",          m_randomSvc,        "Random number generator");
    declareProperty("RandomStreamName",             m_randomEngineName, "Name of the random number stream");
    // The Pixel / SCT ID helper
    declareProperty("IdHelperName",                 m_siIdHelperName);
    // For the SiHit creation
    declareProperty("PathToChargeConversion",       m_siPathToCharge);
    // Tools & Services
    declareProperty("UseConditionsSvc",             m_useConditionsSvc);
    declareProperty("ConditionsSvc",                m_condSummarySvc);
    // Services
    declareProperty("IncidentService",              m_incidentSvc);   
    declareProperty("FastEnergyDepositionModel",    m_fastEnergyDepositionModel);   
}

//================ Destructor =================================================

iFatras::HitCreatorSilicon::~HitCreatorSilicon()
{}

//================ Initialisation =================================================
StatusCode iFatras::HitCreatorSilicon::initialize()
{
    // Random number service
    if ( m_randomSvc.retrieve().isFailure() ) {
        ATH_MSG_ERROR( "[ --- ] Could not retrieve " << m_randomSvc );
        return StatusCode::FAILURE;
    }
    //Get own engine with own seeds:
    m_randomEngine = m_randomSvc->GetEngine(m_randomEngineName);
    if (!m_randomEngine) {
        ATH_MSG_ERROR( "[ --- ] Could not get random engine '" << m_randomEngineName << "'" );
        return StatusCode::FAILURE;
    }
       
    if ( m_useConditionsSvc && m_condSummarySvc.retrieve().isFailure()){
        ATH_MSG_ERROR( "[ --- ] Could not Retrieve '" << m_condSummarySvc << "'" );
        return StatusCode::FAILURE;        
    } else 
        ATH_MSG_VERBOSE( "[ sihit ] Successfully retireved " << m_condSummarySvc );

    // Get the Pixel Identifier-helper:
    if (m_siIdHelperName == "PixelID" && detStore()->retrieve(m_pixIdHelper, m_siIdHelperName).isFailure()) {
      ATH_MSG_ERROR( "[ --- ] Could not get PixelID helper");
      return StatusCode::FAILURE;
    } else if ( m_siIdHelperName == "SCT_ID" && detStore()->retrieve(m_sctIdHelper, m_siIdHelperName).isFailure() )   {
        ATH_MSG_ERROR( "[ --- ] Could not get SCT_ID helper");
        return StatusCode::FAILURE;
    }

    // Athena/Gaudi framework
    if (m_incidentSvc.retrieve().isFailure()){
        ATH_MSG_WARNING("[ sihit ] Could not retrieve " << m_incidentSvc << ". Exiting.");
        return StatusCode::FAILURE;
    }
    // register to the incident service: BeginEvent for TrackCollection
    m_incidentSvc->addListener( this, IncidentType::BeginEvent);
    m_dEdX_function = new TF1 ("fitfunc2", langaufun_fast, 0.,10.,4);
    ATH_MSG_INFO( "[ sihit ]  initialize() successful." );
    return StatusCode::SUCCESS;
}


StatusCode iFatras::HitCreatorSilicon::finalize()
{    delete m_dEdX_function;
    ATH_MSG_INFO( "[ sihit ]  finalize() successful " );
    return StatusCode::SUCCESS;
}


void iFatras::HitCreatorSilicon::handle( const Incident& inc ) {
  // check the incident type 
  if ( inc.type() == IncidentType::BeginEvent ){
    // check if the hit collection already contains:
    // (a)     if yes ... try to retrieve it
    if ( evtStore()->contains<SiHitCollection>(m_collectionName) ){
        if ( (evtStore()->retrieve(m_hitColl , m_collectionName)).isFailure() )
            ATH_MSG_ERROR( "[ --- ] Unable to retrieve SiHitCollection " << m_collectionName);
    // (b)     if no ... try to create it      
    } else {
        m_hitColl = new SiHitCollection( m_collectionName);
        if ( (evtStore()->record(m_hitColl, m_collectionName, true)).isFailure() ) {
            ATH_MSG_ERROR( "[ --- ] Unable to record SiHitCollection " << m_collectionName);
            delete m_hitColl; m_hitColl=0;
        }
    }
  }
  return;
}

double langaufun_fast(double* x, double* par){
	//Fit parameters:
	//par[0]=Width (scale) parameter of Landau density
	//par[1]=Most Probable (MP, location) parameter of Landau density
	//par[2]=Total area (integral -inf to inf, normalization constant)
	//par[3]=Width (sigma) of convoluted Gaussian function
	//
	//In the Landau distribution (represented by the CERNLIB approximation), 
	//the maximum is located at x=-0.22278298 with the location parameter=0.
	//This shift is corrected within this function, so that the actual
	//maximum is identical to the MP parameter.
	
	// Numeric constants
	float invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
	float mpshift  = -0.22278298;       // Landau maximum location
	// Gauss lookup table - let's be honest, these are always the same values...
	// This table is specific to the following np value and needs adaptation in case np is changed
	double gauss_lut[50] = {0.998750780887374456,0.988813043727165275,0.96923323447634413,0.940588065326561695,0.903707082721058597,
		0.859632757966350525,0.809571661213986715,0.754839601989007347,0.696804761374882342,0.636831621583786367,
		0.576229102522380576,0.516205688098894111,0.457833361771614267,0.402021370154990454,0.349500576034800337,
		0.300817976590658676,0.256340161499255759,0.216265166829887306,0.180639843651333204,0.149381761360416171,
		0.122303465302261424,0.0991372321836489212,0.0795595087182276867,0.0632127172421080297,0.0497248676032363973,
		0.0387257750604656018,0.0298595590948963728,0.0227941808836123437,0.0172274759940137939,0.0128906873307075114,
		0.00954965878387800497,0.00700416504525899417,0.00508606923101270064,0.00365649704823364612,0.0026025848245772071,
		0.0018340111405293852,0.00127954549250140965,0.000883826306935049958,0.000604414925679283501,0.000409223053150731654,
		0.000274310255595171487,0.000182046138317709796,0.000119612883581024366,7.78093008945889215e-05,5.01120454198365631e-05,
		3.19527960443799922e-05,2.01712861314266379e-05,1.26071051770485227e-05,7.8010709105552431e-06,4.77914424437207415e-06};
	// Control constants
	float np = 100.0;      // number of convolution steps
	float sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
	
	// Variables
	float xx;
	float mpc;
	float fland;
	float sum = 0.0;
	float xlow,xupp;
	float step;
	int i;
	
	// MP shift correction
	mpc = par[1] - mpshift * par[0]; 
	
	// Range of convolution integral
	xlow = x[0] - sc * par[3];
	xupp = x[0] + sc * par[3];
	
	step = (xupp-xlow) / np;
	
	// Convolution integral of Landau and Gaussian by sum
	for(i=0; i < np/2; i++) {
		xx = x[0] +(((float) i)-.5) * step; //x[0] - (((float) i)+.5) * step;
		fland = TMath::Landau(xx,mpc,par[0]) / par[0];
		sum += fland * gauss_lut[i];
		
		xx = x[0] -(((float) i)-.5) * step; // x[0] + (((float) i)+.5) * step;
		fland = TMath::Landau(xx,mpc,par[0]) / par[0];
		sum += fland * gauss_lut[i];
	}
	
	return (par[2] * step * sum * invsq2pi / par[3]);
}

double iFatras::HitCreatorSilicon::energyDeposit_fast(const ISF::ISFParticle& isp, bool& isPix, bool& isSCT ) const {

  int pdg_id= isp.pdgCode(); 
  Amg::Vector3D mom = isp.momentum();
  double Momentum = sqrt((mom.x()*mom.x()) +( mom.y()*mom.y())+(mom.z()*mom.z()));
  double randLand = CLHEP::RandLandau::shoot(m_randomEngine);
  double energyLoss=0;
	if(abs(pdg_id)==11){ // electrons 
   		energyLoss = (randLand*0.032)+0.32;
  	}
  	else{ // kaons, protons, pions, myons
   	double rest_mass= isp.mass();
     	double para0 = -1.12409e-02; 
  	double para1 = 1.42786e-11;
        // ST : TEMPORARY remove beta dependence ( fixed to 1 GeV )
        Momentum = 1000.;
        // ST
  	double beta = Momentum/ sqrt((Momentum*Momentum)+(rest_mass*rest_mass)); 
  	double MPV = (para0/pow(beta,2))*(log(para1*(pow(beta,2)/(1-pow(beta,2))))-pow(beta,2));
        
    	if( Momentum< 1000){ //momentum < 1GeV
      		if(abs(pdg_id)==211){  // Pionen
	  		if(isSCT){
	    			energyLoss = (randLand*0.023)+(MPV+0.008);
	  		}
	  		if(isPix){
	    			energyLoss = (randLand*0.0225)+(MPV+0.008);
	  		}
      		}
      		else if(abs(pdg_id)==321){ //Kaonen
	  		if(isSCT){
	    			energyLoss = (randLand*0.048)+(MPV+0.01);
	  		}
	  		if(isPix){
	    			energyLoss = (randLand*0.0465)+(MPV+0.01);
	  		}
      		}
      		else if(abs(pdg_id)==13){ // Muonen
	  		if(isSCT){
	    			energyLoss = (randLand*0.025)+(MPV+0.011);
	  		}
	  		if(isPix){	
    				energyLoss = (randLand*0.048)+(MPV+0.011);
	  		}
      		}
      		else if(abs(pdg_id)==2212){ // Protonen
	  		if(isSCT){
	    			energyLoss = (randLand*0.0458)+(MPV+0.008);
	  		}
	  		if(isPix){
	    			energyLoss = (randLand*0.0465)+(MPV+0.008);
	  		}
      		}
     
    }//end momentum < 1GeV
    else if(Momentum >= 1000){
    	if(isSCT){	
	 energyLoss = (randLand*0.025)+(MPV+0.013);
       }
       if(isPix){	   
		energyLoss = (randLand*0.025)+(MPV+0.014);
       }
    }//end Momentum >= 1GeV
  } // End kaons, protons, pions, myons 
  
  return energyLoss;
}

double iFatras::HitCreatorSilicon::energyDeposit_exact(const ISF::ISFParticle& isp, bool& isPix, bool& isSCT ) const {
   int pdg_id= isp.pdgCode(); 
   Amg::Vector3D mom = isp.momentum();
   double Momentum = sqrt((mom.x()*mom.x()) +( mom.y()*mom.y())+(mom.z()*mom.z()));
   m_dEdX_function->SetNpx(4);
   double xmin = 0;
   double xmax = 0.;
   if(abs(pdg_id)==11){ // electrons
    m_dEdX_function->SetParameters(0.009807, 0.3 ,453.7,0.03733);
    xmax = 1.;
   }
   else{ //  Kaonen, Protonen, Pionen, Myonen
    
   double rest_mass= isp.mass();
   double para0 = -1.12409e-02;
   double para1 = 1.42786e-11;
   double beta = Momentum/ sqrt((Momentum*Momentum)+(rest_mass*rest_mass));
   double MPV = (para0/pow(beta,2))*(log(para1*(pow(beta,2)/(1-pow(beta,2))))-pow(beta,2));
     if(Momentum < 1000){ // momentum < 1GeV 
       if(abs(pdg_id)==211){  // Pionen
	   if(isSCT){
	      m_dEdX_function->SetParameters(0.0250, MPV ,453.7,0.0109);
	   }
	   if(isPix){
	     m_dEdX_function->SetParameters(0.0205, MPV ,453.7,0.0219);
	   }
       }
       else if(abs(pdg_id)==321){ //Kaonen
	   if(isSCT){
	     m_dEdX_function->SetParameters(0.0458, MPV ,453.7,0.0064);
	   }
	   if(isPix){
	     m_dEdX_function->SetParameters(0.0465, MPV,453.7,0.000);
	   }
       }
       else if(abs(pdg_id)==13){ // Muonen
	   if(isSCT){
	     m_dEdX_function->SetParameters(0.0140, MPV,453.7,0.0245);
	   }
	   if(isPix){
	     m_dEdX_function->SetParameters(0.0064, MPV ,453.7,0.0480);
	   }
       }
       else if(abs(pdg_id)==2212){ // Protonen
	   if(isSCT){
	     m_dEdX_function->SetParameters(0.0458, MPV ,453.7,0.0064);
	   }
	   if(isPix){
	     m_dEdX_function->SetParameters(0.0465, MPV ,453.7,0.000);
	   }
       }
      
     }
     else if(Momentum>= 1000){
       if(isSCT){
	 m_dEdX_function->SetParameters(0.0135, MPV ,453.7,0.0245);
       }
       if(isPix){
	 m_dEdX_function->SetParameters(0.0102, MPV ,453.7,0.0297);
       }
     }
    xmax = MPV*10.;
   } //  Parametrisierung fuer Kaonen, Protonen, Pionen, Myonen ENDE
 
   double ymin = 0;
   double ymax = m_dEdX_function -> GetMaximum(xmin, xmax);
   double energyLoss = 0;

   for (int count=0; count <1000; count++) {
     double ry = CLHEP::RandFlat::shoot(m_randomEngine)*(ymax-ymin) + ymin;
     double rx = CLHEP::RandFlat::shoot(m_randomEngine)*(xmax-xmin) + xmin;
     double y = m_dEdX_function->Eval(rx);
     if ( ry <= y ){ energyLoss = rx; break;}
   }

 

return energyLoss;
}


void iFatras::HitCreatorSilicon::createSimHit(const ISF::ISFParticle& isp, const Trk::TrackParameters& pars, double time ) const {
 
  // get surface and DetElement base
   const Trk::Surface &hitSurface = pars.associatedSurface();
   const Trk::TrkDetElementBase* detElementBase = hitSurface.associatedDetectorElement();  
   
   // the detector element cast -> needed 
   const InDetDD::SiDetectorElement* SiDetElement = dynamic_cast<const InDetDD::SiDetectorElement*>((detElementBase));

   //Adding new lines for custom detector simulation
   const iFatras::PlanarDetElement* PlanardetElement = dynamic_cast<const iFatras::PlanarDetElement*>((detElementBase));

   // return triggered if no siDetElement or no current particle link to the stack
   if ( !SiDetElement && !PlanardetElement){
     ATH_MSG_WARNING("[ sihit ] No Silicon & Planar Detector element available. Ignore this one.");
     return;
   }
   
   if( SiDetElement )
     return createSimHit( isp, pars, time, SiDetElement, 1);
   else return createSimHit( isp, pars, time, PlanardetElement, 0);
   
}

template<typename ELEMENT>
void iFatras::HitCreatorSilicon::createSimHit(const ISF::ISFParticle& isp, const Trk::TrackParameters& pars, double time, ELEMENT hitSiDetElement, bool isSiDetElement) const {
  	
  if (!isSiDetElement)
    ATH_MSG_VERBOSE("[ sihit ] --> Running with PlanarDetElement");
   
  // get surface and DetElement base
  const Trk::Surface &hitSurface = pars.associatedSurface();
     
  // get the identifier and hash identifier
  Identifier hitId         = hitSurface.associatedDetectorElementIdentifier();   
  IdentifierHash hitIdHash = hitSiDetElement->identifyHash();
  // check conditions of the intersection
  if ( m_useConditionsSvc ) {
    bool isActive = m_condSummarySvc->isActive(hitIdHash, hitId);                   // active = "element returns data"
    bool isGood   = isActive ? m_condSummarySvc->isGood(hitIdHash, hitId) : false;  // good   = "data are reliable"
    if (!isActive) 
      ATH_MSG_VERBOSE("[ sihit ]  ID " << hitId << ", hash " << hitIdHash << " is not active. ");
    else if (!isGood)               
      ATH_MSG_VERBOSE("[ sihit ]  ID " << hitId << ", hash " << hitIdHash << " is active but not good. ");
    else                            
      ATH_MSG_VERBOSE("[ sihit ]  ID " << hitId << ", hash " << hitIdHash << " is active and good.");
    if (!isActive || !isGood) return;
  }

 // intersection
   const Amg::Vector2D& intersection = pars.localPosition();
   double interX = intersection.x();
   double interY = intersection.y();
   // thickness of the module
   double thickness = hitSiDetElement->thickness();
   // get the momentum direction into the local frame
   Amg::Vector3D particleDir = pars.momentum().unit();
   const Amg::Transform3D& sTransform = hitSurface.transform();
   Amg::Vector3D localDirection = sTransform.inverse().linear() * particleDir;   
   localDirection *= thickness/cos(localDirection.theta());
   // moving direction
   int movingDirection = localDirection.z() > 0. ? 1 : -1;
   // get he local distance of the intersection in x,y
   double distX = localDirection.x();
   double distY = localDirection.y();
   // local entries in x,y
   double localEntryX = interX-0.5*distX;
   double localEntryY = interY-0.5*distY;
   double localExitX  = interX+0.5*distX;
   double localExitY  = interY+0.5*distY;
   double   energyDeposit=0;   
   //!< @TODO : fix edge effects 
   const Amg::Transform3D &hitTransform = Amg::CLHEPTransformToEigen( hitSiDetElement->transformHit().inverse() );
   // transform into the hit frame
   Amg::Vector3D localEntry(hitTransform*(sTransform*Amg::Vector3D(localEntryX,localEntryY,-0.5*movingDirection*thickness)));
   Amg::Vector3D localExit(hitTransform*(sTransform*Amg::Vector3D(localExitX,localExitY,0.5*movingDirection*thickness))); 

   bool isPix = hitSiDetElement->isPixel();
   bool isSCT = hitSiDetElement->isSCT(); 

   // Landau approximation
   double dEdX = m_fastEnergyDepositionModel ?
                    energyDeposit_fast(isp,isPix,isSCT)
                  : energyDeposit_exact(isp,isPix,isSCT);

   energyDeposit = dEdX * (localExit - localEntry).mag();
 
   //!< barcode & time from current stack particle
   //  double   energyDeposit = m_siPathToCharge*localDirection.mag()*CLHEP::RandLandau::shoot(m_randomEngine);
   int    barcode       = isp.barcode();
   
   // create the silicon hit
   const HepGeom::Point3D<double> localEntryHep( localEntry.x(), localEntry.y(), localEntry.z() );
   const HepGeom::Point3D<double> localExitHep( localExit.x(), localExit.y(), localExit.z() );
 
   SiHit siHit(localEntryHep,
	       localExitHep,
	       energyDeposit,
	       time,
	       barcode,
	       m_pixIdHelper ? 0 : 1,
	       m_pixIdHelper ? m_pixIdHelper->barrel_ec(hitId)  : m_sctIdHelper->barrel_ec(hitId),
	       m_pixIdHelper ? m_pixIdHelper->layer_disk(hitId) : m_sctIdHelper->layer_disk(hitId),
	       m_pixIdHelper ? m_pixIdHelper->eta_module(hitId) : m_sctIdHelper->eta_module(hitId),
	       m_pixIdHelper ? m_pixIdHelper->phi_module(hitId) : m_sctIdHelper->phi_module(hitId),
	       m_pixIdHelper ? 0 : m_sctIdHelper->side(hitId)); 
   
   if ( isSiDetElement )
     ATH_MSG_VERBOSE("[ sihit ] Adding an SiHit SiDetElement to the SiHitCollection.");
   else
     ATH_MSG_VERBOSE("[ sihit ] Adding an SiHit PlanarDetElement to the SiHitCollection.");
   
   m_hitColl->Insert(siHit);
  
}
