/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetDynamicNoiseAdjustment.cxx
//   Source file for ???????????????????????????????????
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Vakhtang.Kartvelishvili@cern.ch
///////////////////////////////////////////////////////////////////

// #include "GaudiKernel/ListItem.h"

#include "TrkDynamicNoiseAdjustor/InDetDynamicNoiseAdjustment.h"

#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkToolInterfaces/IUpdator.h"

//#include "TrkParameters/MeasuredTrackParameters.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include <cmath>

#include <ext/algorithm>

#include "TrkGeometry/Layer.h"
//#include "TrkGeometry/ReferenceMaterial.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Gaudi AlgTool control (constructor, initialise ...)                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
Trk::InDetDynamicNoiseAdjustment::InDetDynamicNoiseAdjustment
(const std::string& t,const std::string& n,const IInterface* p) :
  AthAlgTool (t,n,p),
  m_yminmax_generic (100.),
  m_yminmax_electron(100.),
  m_signifmin_generic (2.0),
  m_signifmin_electron(0.0),
  m_lambdaxmin_generic (2.0),
  m_lambdaxmin_electron(0.0),
  m_lambdaqop_generic (0.),
  m_lambdaqop_electron(0.),
  m_currentDnaStrategy(Trk::InDetDynamicNoiseAdjustment::unidentifiedStrategy),
  m_extrapolator(nullptr),
  m_updator(nullptr)
{
	// AlgTool stuff
	declareInterface<IDynamicNoiseAdjustor>( this );

    declareProperty("yminmax_generic", m_yminmax_generic);
    declareProperty("signifmin_generic", m_signifmin_generic);
    declareProperty("lambdaxmin_generic", m_lambdaxmin_generic);
    declareProperty("lambdaqop_generic", m_lambdaqop_generic);

    declareProperty("yminmax_electron", m_yminmax_electron);
    declareProperty("signifmin_electron", m_signifmin_electron);
    declareProperty("lambdaxmin_electron", m_lambdaxmin_electron);
    declareProperty("lambdaqop_electron", m_lambdaqop_electron);

}

// destructor
Trk::InDetDynamicNoiseAdjustment::~InDetDynamicNoiseAdjustment()
{}


// initialize
StatusCode Trk::InDetDynamicNoiseAdjustment::initialize()
{
  msg(MSG::INFO)
        << "DNA tuning: yminmax <GEN> = " << m_yminmax_generic
        << "(activity level of DNA, range 0.0 to 100)" 
        << "\nDNA tuning: signifmin <GEN> = " << m_signifmin_generic
        << "\nDNA tuning: lambdaxmin <GEN> = " << m_lambdaxmin_generic
        << "\n(par to tune the width of the pull, range -2.0 to 4.0)" 
        << "\nDNA tuning: lambdaqop <GEN> = " << m_lambdaqop_generic << endmsg;
  msg(MSG::INFO)
        << "DNA tuning: yminmax <ELE> = " << m_yminmax_electron
        << "(activity level of DNA, range 0.0 to 100)" 
        << "\nDNA tuning: signifmin <ELE> = " << m_signifmin_electron 
        << "\nDNA tuning: lambdaxmin <ELE> = " << m_lambdaxmin_electron
        << "\n(par to tune the width of the pull, range -2.0 to 4.0)" 
        << "\nDNA tuning: lambdaqop <ELE> = " << m_lambdaqop_electron << endmsg;

  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}


// finalize
StatusCode Trk::InDetDynamicNoiseAdjustment::finalize()
{
  ATH_MSG_INFO ("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// configure the InDetDynamicNoiseAdjustment:
// needs: Extrapolator       - configurable tool to wrap extrapolation
//        Updator            - defines the statistics for updating the estimator
//
////////////////////////////////////////////////////////////////////////////////
StatusCode Trk::InDetDynamicNoiseAdjustment::configureWithTools
(Trk::IExtrapolator* extrap,
 const Trk::IUpdator* updator)
{
	m_extrapolator = extrap;
	m_updator      = updator;

	// protection, if not confiured
	if (!m_extrapolator) {
      ATH_MSG_ERROR ("Extrapolator missing, need to configure with it !");
		return StatusCode::FAILURE;
	}

	// protection, if not confiured
	if (!m_updator) {
      ATH_MSG_ERROR ("Updator missing, need to configure with it !");
		return StatusCode::FAILURE;
	}

	ATH_MSG_INFO ("Configured with updator and extrapolator");
	return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// main method DNA_adjust with actual code for kalman noise adjustment        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
const Trk::DNA_MaterialEffects* Trk::InDetDynamicNoiseAdjustment::DNA_Adjust(
                const TrackParameters*& predPar,             // predicted into next layer
                const TrackParameters*& updatedPar,           // previously updated
                const MeasurementBase*  fittableMeasurement,
                const KalmanMatEffectsController& matEff,
                PropDirection direction,
                const Trk::DNA_MaterialEffects* /*forwardMEF*/) const{

//  if (m_outputlevel <=0) m_log << MSG::DEBUG << "VATO-7777 : entered "
//                               << "DNA_Adjust, dir = " << direction << endmsg;

  if ( predPar==nullptr || updatedPar == nullptr || fittableMeasurement == nullptr) {
    ATH_MSG_WARNING ("Inconsistent use: inputs are NULL pointers! " <<
                     predPar << ", " << updatedPar << ", " << fittableMeasurement);
    return nullptr;
  }
  const Trk::Surface&   surf = fittableMeasurement->associatedSurface();
  Trk::DNA_MaterialEffects* estimatedMatEffects = nullptr;
  double cut_yminmax   = matEff.aggressiveDNA() ? m_yminmax_electron    : m_yminmax_generic;
  double cut_signifmin = matEff.aggressiveDNA() ? m_signifmin_electron  : m_signifmin_generic;
  double lambdaxmin    = matEff.aggressiveDNA() ? m_lambdaxmin_electron : m_lambdaxmin_generic;
  double lambdaqop     = matEff.aggressiveDNA() ? m_lambdaqop_electron  : m_lambdaqop_generic;

  // Check if error matrix of start parameter and its associated layer material exist
  const Trk::Layer* lay = updatedPar ? (updatedPar->associatedSurface()).associatedLayer() : nullptr;
  

  m_currentDnaStrategy = unidentifiedStrategy;

  if(!updatedPar->covariance()) return nullptr;

  AmgSymMatrix(5) merr(*updatedPar->covariance());

  // define various parameters
  double rxymax, rxymin,qopmax, chi2threshold, sign;
  if(direction == Trk::alongMomentum){
     rxymax = 545.0;
//   rxymin = 170.0;
//   rxymax = 470.0;
     rxymin = 100.0;
//     rxymin = 250.0;
     //tantmin= 0.16;
     qopmax = 0.002; // 500MeV hard cut
     chi2threshold=  1.0;
//      chi2threshold=  0.5;
     sign = 1.0;
  } else {
     rxymax = 545.0;
//   rxymin =  40.0;
//   rxymax = 470.0;
//     rxymin = 100.0;
     rxymin =   0.0;
//     rxymin = 250.0;
     //tantmin= 0.16;
     qopmax = 0.002;
     chi2threshold=  1.0;
//     chi2threshold=  0.5;
     sign = -1.0;
  }

  // Set default values for estimated z and dynamic noise term
  double zest=1.0, dna=0.0;
  // currently not needed?:  
  double zcor=1.0;

  double pathlength = 0.00, m=0.00;

  double zinv1=1.0, zinv2=1.0;  
  double xmin=1.0, dx=1.0, xth=4.0, ymin=99.99;

  //   double xminf=1.0, dxf=1.0;
      
  // calculate various geometrical quantities, used later to decide
  // whether to next measurement is worth considering
  double rxy1 = sqrt(pow(updatedPar->position().x(),2)+pow(updatedPar->position().y(),2));
  double rxy2 = sqrt(pow(predPar->position().x(),2)+pow(predPar->position().y(),2));
  double z1 =updatedPar->position().z();
  double z2 =predPar->position().z();
  double cost12 = (rxy1*rxy2+z1*z2)/sqrt((rxy1*rxy1+z1*z1)*(rxy2*rxy2+z2*z2));
  double sint1 = std::sin(updatedPar->parameters()[Trk::theta]);
  double sint2 = std::sin(predPar->parameters()[Trk::theta]);
  double  dsint = sint1-sint2;
  double eta1=-std::log(std::tan((updatedPar->parameters()[Trk::theta])/2.0));
  double phi1= updatedPar->parameters()[Trk::phi];

  double sigmaQoverP=0.0;
  double qop0=0.0, sigmaqop0=0.0, chi20=0.0, dof0=1.0;
  double chi21=0.0;
  double chi22=0.0;
  double y0=1.0, y1=1.0, y2=1.0;
  

  ATH_MSG_DEBUG ("VATO-729 rxy1, z1, rxy2, z2, cost12, dsint, eta1, phi1   "<<
                 rxy1  <<"   "<< z1   <<"   "<< rxy2 << "   "<< z2 << "   "<<
                 cost12<<"   "<< dsint <<"   "<<eta1<<"   "<< phi1 <<"   ");
  //Step zero parameters and errors before extrapolation and before measurement update
  AmgVector(5) updatedParameters0 = (updatedPar->parameters());
  qop0 = updatedParameters0[Trk::qOverP];
  
  sigmaqop0 = Amg::error(merr,Trk::qOverP);
	          
  // Geometric cuts to exclude weird "tracks"
  if( cost12>0.98
      && std::fabs(dsint)<0.003
      && rxy1<rxymax && rxy1>rxymin
//    && rxy2<rxymax && rxy2>rxymin
      && sign*rxy1<sign*rxy2
      && sint1>0.0 
      && std::fabs(eta1)<2.7
//     && fabs(rxy1-rxy2)<200.0
//     && sign*fabs(z1)<sign*fabs(z2)
//     && rxy1>fabs(z1)*tantmin & rxy2>fabs(z2)*tantmin)
//     && sign*(rxy2-rxy1)>3.0 
      ){

// pre-update the extrapolated predPar to new measurement, calculate tentative Chi2
    const FitQualityOnSurface* fitQuality0
      = m_updator->predictedStateFitQuality(*predPar,
                                            fittableMeasurement->localParameters(),
                                            fittableMeasurement->localCovariance());
    if (!fitQuality0) {
      ATH_MSG_DEBUG ("chi2 calculation failed for state with measurement " << *fittableMeasurement);
      return nullptr;
    }
    chi20 = fitQuality0->chiSquared();
    dof0  = std::fabs(fitQuality0->numberDoF());
    delete fitQuality0;

    y0=1.0, y1=1.0, y2=1.0;
    zinv1=1.0; zinv2=1.0; zcor=1.0;

// Calculate the step for zest testing
    if (qop0 == 0.0 || chi20 < 0.0) {
      ATH_MSG_WARNING ( (qop0==0.0 ? "FPE, bad fit (qop0="
                                  : "invalid chi2 from updator (chi20=")
                        << (qop0==0.0 ? qop0 : chi20) <<"). Stop dna at this state");
      return nullptr;
    }
    double qoplimit, stepFactor = 1.0+3.0*sqrt(chi20)*sigmaqop0/std::fabs(qop0);
    if (direction == Trk::alongMomentum) {
      if (stepFactor>1.6) stepFactor=1.6;                      ///// needs tuning
      zinv1=stepFactor;
      zinv2=zinv1;
      qoplimit=qopmax/(zinv1*zinv2);
    } else {
      if (stepFactor>1.6) stepFactor=1.6;                      ///// needs tuning
      zinv1=1.0/stepFactor;
      zinv2=zinv1;
      qoplimit=qopmax;
    }

//if brem is suspected:
    if ( chi20 > chi2threshold*dof0 
         && std::fabs(qop0) < qoplimit 
         && std::fabs(qop0)*0.08 > sigmaqop0 )    // excludes undeveloped tracks
      {

        // Change momentum for step one
        AmgVector(5) updatedParameters1 = (updatedPar->parameters());
        updatedParameters1[Trk::qOverP] *= zinv1;

        // Prepare the same error on qop for step one
	    AmgSymMatrix(5)* updatedCovariance1 =
          new AmgSymMatrix(5)(merr);

        // Change updatedPar for step one
        const Trk::TrackParameters* clonePars1 
          = CREATE_PARAMETERS(*updatedPar, updatedParameters1, updatedCovariance1);
        delete updatedPar; updatedPar = clonePars1;
        // --- Extrapolate changed updatedPar and calculate chi2 for step one
        const Trk::TrackParameters* testPredPars = m_extrapolator->extrapolate( *updatedPar,  surf, direction, false, matEff.particleType() );
        if (testPredPars == nullptr) {
          ATH_MSG_WARNING ("Test extrapolation for DNA step 1 failed");
          return nullptr;
        }
        
        // --- Find out Chi2 and dof for step 1
        const FitQualityOnSurface* fitQuality1 = m_updator->predictedStateFitQuality
          ( *testPredPars,fittableMeasurement->localParameters(),
            fittableMeasurement->localCovariance() );
        if (fitQuality1 == nullptr) {
          ATH_MSG_WARNING ("fit quality calculation for DNA step 1 failed");
          return nullptr;
        }
        chi21 = fitQuality1->chiSquared();
        //dof1  = fabs(fitQuality1->numberDoF());
        delete testPredPars;
        delete fitQuality1;
	
        // --- Change momentum for step two
        AmgVector(5) updatedParameters2 = (updatedPar->parameters());
        updatedParameters2[Trk::qOverP] *= zinv2;

        // --- Prepare the same error on qop for step two
	AmgSymMatrix(5)* updatedCovariance2 = new AmgSymMatrix(5)(*(updatedPar->covariance()));
	
        // --- Change updatedPar for step two
	const Trk::TrackParameters* clonePars2 = 
              CREATE_PARAMETERS(*updatedPar, updatedParameters2, updatedCovariance2);
	delete updatedPar; updatedPar = clonePars2;
	
        // --- Extrapolate changed updatedPar and calculate chi2 for step two
        testPredPars = m_extrapolator->extrapolate( *updatedPar,  surf, direction, false, matEff.particleType());
        if (testPredPars == nullptr) {
          ATH_MSG_WARNING ("Test extrapolation for DNA step 2 failed");
          return nullptr;
        }
        const FitQualityOnSurface* fitQuality2 = m_updator->predictedStateFitQuality
              ( *testPredPars, fittableMeasurement->localParameters(),
				fittableMeasurement->localCovariance() );
        if (fitQuality2 == nullptr) {
          ATH_MSG_WARNING ("fit quality calculation for DNA step 2 failed");
          return nullptr;
        }

        chi22 = fitQuality2->chiSquared();
        //dof2  = fabs(fitQuality2->numberDoF());
        delete testPredPars;
        delete fitQuality2;

        // -- Have three points now, try calculate the minimum of Chi2:
        y0=chi20; y1=chi21; y2=chi22;

        double x0=1.0, x1, x2;
        double a=0., b=0., c=0., xmincan=1.0, dxcan=1.0;

        if(direction == Trk::alongMomentum){
          x1=zinv1;
          x2=zinv1*zinv2;
        } else {
          x1=1.0/zinv1;
          x2=1.0/(zinv1*zinv2);
        }

        if (x1!=x2 && x0!=x1 && x0!=x2) {
          a=((y2-y0)/(x2-x0)-(y1-y0)/(x1-x0))/(x2-x1);
          b=(y2-y0)/(x2-x0)-a*(x2+x0);
          c=y0-a*x0*x0-b*x0;
          if(a > 0.0) xmincan = -b/2./a;
          if(a > 0.0) dxcan   = 1.0/sqrt(a);
          ymin=a*xmincan*xmincan+b*xmincan+c;
        }
        ATH_MSG_DEBUG ("VATO-707:  rxy1,dof0,x0,x1,x2,y0,y1,y2,xmin+/-dx,ymin:   " <<
                       rxy1 <<" "<< dof0 <<"  |  "<< x0 <<" "<< x1 <<" "<< x2 <<"  |  "<<
                       y0 <<" "<< y1 <<" "<< y2 <<"  |  "<< xmincan <<"+/-"<<dxcan<<"  |  "<<ymin); 

        // --- If a good minimum is found:
        // FIXME query matEff.aggressiveDNA() to switch behaviour
        if(a>0. && (xmincan-x0)*(xmincan-xth)<0.0 && ymin<cut_yminmax) {
//           if(a>0. & (xmincan-x0)*(xmincan-xth)<0.0 ) {
          xmin=xmincan;
          dx  =dxcan;
        } else m_currentDnaStrategy = noGoodMinimum;

      } 
    else { // low chi2, low pt, or undeveloped track  

      zinv1  = 1.0; 
      zinv2  = 1.0;      
      m_currentDnaStrategy = undevelopedTrack;
 
    }     //endif --- a brem is suspected
  } else { // geometric cuts are not satisfied
      
    m_currentDnaStrategy = geomCutsNotSatisfied;
    
  }       //endif --- geometric cuts
                                                                     
//===========================================================================
// If a "good" minimum has been found here, then xmin > 1.0, else xmin = 1.0
// If a forwardMEF exists here, xminf > 1.0, else xminf = 1.0
// In any case, figure out what to do next:

  ATH_MSG_DEBUG ("VATO-708-0-0:  dir, rxy1, xmin, xminf: " << direction << "  "<< rxy1<<
                 "     " << xmin << " +/- " << dx <<",   ");   //<< xminf <<" +/- "<<dxf);
  
  double signif = 0.0;
  double denom  = 1.0;           // - sigmaqop0/fabs(qop0)*m_lambdaqop;

//    double xmincombined=1.0, dxcombined=1.0;
//    if ((xminf == 1.0 && xmin == 1.0)) {  // no good minimum, and nothing in mefot

  if (m_currentDnaStrategy != unidentifiedStrategy) {  // already known that no good minimum exists

    ATH_MSG_DEBUG ("VATO-708-1-1: dir, Rxy, eta, phi, pT, dp/p, chi20, dof0, xmin+/-dx, "<<
                   "ymin, m, dpdna/p: " << direction <<"   "<< rxy1 <<"   "<< eta1 <<"   "<<
                   phi1 <<"   " << sint1/qop0 << "   " << sigmaqop0/std::fabs(qop0) <<"   "<<chi20<<
                   "   "<<dof0<<"   " << xmin <<"   "<< dx <<"   "<< ymin <<"   "<< m <<"   "<<
                   sigmaQoverP/std::fabs(qop0));
    
  } else if (xmin > 1.0) { // good minimum was found, determine DNA strategy further

    if(lay){

      //bool doRefMat = true;
     

      // Estimate m (sometimes called c, pathlength in X0, corrected for theta, divided by ln2)
      // calculate the pathcorrection
      double pathcorrection = std::fabs(lay->surfaceRepresentation().pathCorrection(updatedPar->position(), updatedPar->momentum()));
      // prepare the material properties
      const Trk::MaterialProperties* mat = lay->fullUpdateMaterialProperties(*updatedPar);

      // Temporary fix to avoid crashes due to mat=0:
      double thick=0.0;
      if (mat) { thick = mat->thicknessInX0(); }
      else {
        ATH_MSG_DEBUG ("Problem with material: refMat=0, lay exists, but mat=0.    x,y,z = "<<
                       updatedPar->position().x() << "  " <<
                       updatedPar->position().y() << "  " <<
                       updatedPar->position().z());
      }

      // --- Calcluate the pathlength
      pathlength = pathcorrection * thick;
      m=pathlength/std::log(2.);


      // --- Calculate the median of B-H with that m: {approximation to 1-(0.5)**(1/m)}
      if(m>0.5) m=0.5;
      if(m>0.0 && m<=0.5) {
        double temp=45.*m*m, temp3=temp*temp*temp, temp4=temp*temp3;
        zcor = 1.0 - 0.02433*temp4/(1.0+temp3);
      }
       
//    zcor = zcor - 1.0*m;
       
      signif = (xmin-1.)/dx;
      denom = xmin - dx*lambdaxmin;           // - sigmaqop0/fabs(qop0)*m_lambdaqop;

//         zest=1.0; 
//         if(signif>cut_signifmin) zest=1.0/denom; 
       
         // dilerr can be used to dilute the argument of dna; dilcor weighs zest with B-H mean       
//         double dilerr=1.0, dilcor=1.0; 
      if (rxy1 < 250.0 && sign == -1) {       // special treatment for pixel hits
        m_currentDnaStrategy = backwardPixelTreatment;
//
//           dilerr = 0.1; dilcor = 0.9;         
//           dilerr = 0.5; dilcor = 1.0;         
      }
//         zcor = 1.0 - dilcor*(1.0-zcor) - (1.0-dilcor)*(1.0-pow(0.5,m));                  // reweight z correction
//         if(signif>cut_signifmin) zest=1.0 - (1.0 - 1.0/denom)*dilerr*(1.0-lambdaqop);  // dilute dna argument

//     zcor = zest;
       
           
    } // end of if( lay )

       // Calculate dna
//       dna=Trk::InDetDynamicNoiseAdjustment::calculateDna(xth, m, zest);
     
    if (m_currentDnaStrategy != backwardPixelTreatment)
      m_currentDnaStrategy = regularSuccessfulDNA;

  } // both dna and non-dna branches come here now
  else {
    ATH_MSG_INFO ("rupture in DNA logic, xmin doesn'f fullfil >1. xmin = " << xmin);
  }

  // --- now determine final noise adjustment depending on detected strategy

  switch (m_currentDnaStrategy) {
  case regularSuccessfulDNA:    // regular successful DNA activity 
       
    zest=1.0;
    if(signif>cut_signifmin) zest = 1.0/denom;
    dna = Trk::InDetDynamicNoiseAdjustment::calculateDna(xth, m, zest);
    break;

  case backwardPixelTreatment:  // special treatment for regular DNA activity in pixels in backward dir.
    zest = 1.0;
    if (matEff.aggressiveDNA()) { // now they are the same, near future test different tunings here
      //double dilerr = 0.1, dilcor = 0.9;        
      double dilerr = 1.0, dilcor = 1.0;        
      zcor = 1.0 - dilcor*(1.0-zcor) - (1.0-dilcor)*(1.0-std::pow(0.5,m));                  // reweight z correction
      if(signif>cut_signifmin) zest=1.0 - (1.0 - 1.0/denom)*dilerr*(1.0-lambdaqop);  // dilute dna argument
    } else {
      double dilerr = 0.1, dilcor = 0.9;        
      zcor = 1.0 - dilcor*(1.0-zcor) - (1.0-dilcor)*(1.0-std::pow(0.5,m));                  // reweight z correction
      if(signif>cut_signifmin) zest=1.0 - (1.0 - 1.0/denom)*dilerr*(1.0-lambdaqop);  // dilute dna argument
    }
    dna=Trk::InDetDynamicNoiseAdjustment::calculateDna(xth, m, zest);
    break;

  case noGoodMinimum:
  case geomCutsNotSatisfied:  // geometric cuts not satisfied (mostly TRT)
  case undevelopedTrack:      // low chi2, undeveloped tracks etc.
  case unidentifiedStrategy:  // should never happen!
    zcor = 1.0; dna  = 0.0; 
    // use matEff.aggressiveDNA() to separate strategies further

  }

  ATH_MSG_DEBUG ("VATO-803-3: chi20 dof0  pt  dpt eta  m *  dna zcor zinv1 zinv2  * Rxy lay casino : "
                 << chi20 <<"   "<< dof0 <<"  "<< std::fabs(sint1/qop0) <<"   "
                 << std::fabs(sigmaqop0/qop0) <<"   "<< eta1 <<"   "<< m <<"   *   "
                 << dna <<"   "<< zcor <<"   "  << zinv1<<"   "<< zinv2 << "   *   "
                 << rxy1 <<"   "<< lay <<"   "<< m_currentDnaStrategy <<"      cas ");

  // --- only if a correction is there re-do the extrapolation, else leave original parameters
  if (dna!= 0.00) {
    // pick up values for dna, zcor, zinv1, zinv2; do corrections to momentum and its error
    AmgVector(5) updatedParameters = (updatedPar->parameters());
    //       updatedParameters[Trk::qOverP] *= 1.0;    //should be z0 here!
    updatedParameters[Trk::qOverP] *= zcor/zinv1/zinv2;    //should be z0 here!
    // Calculate the DNA term to the error
      
    sigmaQoverP=dna*std::fabs(updatedParameters[Trk::qOverP]);

    AmgSymMatrix(5)* updatedCovariance =  new AmgSymMatrix(5)(merr);
    (*updatedCovariance)(Trk::qOverP,Trk::qOverP) += sigmaQoverP*sigmaQoverP;

    const Trk::TrackParameters* clonePars = (updatedPar->associatedSurface()).createTrackParameters(updatedParameters[0],
                                                                                                    updatedParameters[1],
                                                                                                    updatedParameters[2],
                                                                                                    updatedParameters[3],
                                                                                                    updatedParameters[4],
                                                                                                    updatedCovariance);
    delete updatedPar; updatedPar = clonePars;

    const Trk::TrackParameters* testPredPars = m_extrapolator->extrapolate(*updatedPar,  surf, direction, false, matEff.particleType());
    if (testPredPars == nullptr) {
      ATH_MSG_WARNING ("extrapolation for adjusted cov fails - should never happen!");
      return nullptr;
    }

    // now replace by DNA-corrected prediction (delete TPs from KF)
    delete predPar; predPar = testPredPars;

    // fill estimated material effects based on suspected brem
    estimatedMatEffects = 
      new DNA_MaterialEffects(xmin /*combined*/,
                              dx   /*combined*/,
                              chi20,
                              pathlength,
                              dna,
                              sigmaQoverP,
                              (direction == Trk::alongMomentum),
                              updatedPar->associatedSurface()); 
    // p is now corrected by DNA itself.
    estimatedMatEffects->setDeltaP(1.0/std::fabs(qop0)*(1.-zcor));

    ATH_MSG_DEBUG ("VATO-708-2-1: dir, Rxy, eta, phi, pT, dp/p, chi20, dof0, xmin+/-dx, ymin, m, dpdna/p: "
                   << direction <<"   "<< rxy1 <<"   "<< eta1 <<"   "<< phi1 <<"   "
                   << sint1/qop0 << "   " << sigmaqop0/std::fabs(qop0) <<"   "<<chi20<<"   "<<dof0<<"   " 
                   << xmin <<"   "<< dx <<"   "<< ymin <<"   "<< m <<"   "
                   << sigmaQoverP/std::fabs(qop0));
  }
  return estimatedMatEffects;
  //==============================================================================
}            //end of DNA_Adjust




////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// numerical approximation for probability-mapping Bethe-Heitle to Gauss      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
double Trk::InDetDynamicNoiseAdjustment::calculateDna(double xth, double c, double zest) const
{       double dna=0.0;
        double m = c; if(c>0.2) m=0.2;
	if(zest<1.0 && zest>1.0/xth && m>0.0 && m<=0.2) {
		double p1=0.23+m*1.5, p2=0.96-m*2.5, p3=0.325, p4=0.84-m*3.0;     //parameters for dna
		dna=(p1*std::pow((std::log(1./zest)),p2)*std::pow(zest,p3)*(1.+p4*zest)); //dna parametrization
//        dna=0.0; // Dirty and ugly way of running without dna
//          dna=2.0*dna; //temporary !!!!!!!
	}
	return dna;
}

