/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#define ARTRU          // Choice of TR generator

// class header
#include "TRTTransitionRadiation.h"

// package includes
#include "TRTRadiatorParameters.h"
#include "TRRegionXMLHandler.h"

// Athena includes
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"//Material Manager
#include "GeoModelInterfaces/StoredMaterialManager.h"//Material Manager
#include "GeoMaterial2G4/Geo2G4MaterialFactory.h" //Converting GeoMaterial -> G4Material
#include "PathResolver/PathResolver.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"//Detector Store

// Geant4 includes
#include "G4DynamicParticle.hh"
#include "G4Gamma.hh"
#include "G4Material.hh"
#include "G4EmProcessSubType.hh"// for fTransitionRadiation
#include "G4ProcessType.hh" // for fElectromagnetic

#include "Randomize.hh"

// CLHEP includes
#include "CLHEP/Random/RandPoisson.h"

// STL includes
#include <cmath>
#include <limits>

////////////////////////////////////////////////////////////////////////////
//
// Constructor, destructor

TRTTransitionRadiation::TRTTransitionRadiation( const G4String& processName, const std::string xmlfilename) :
  G4VDiscreteProcess(processName,fElectromagnetic),m_XMLhandler(NULL),m_xmlfilename(xmlfilename),
  m_MinEnergyTR(0.0),m_MaxEnergyTR(0.0),m_NumBins(0),m_WplasmaGas(0.0),
  m_WplasmaFoil(0.0),m_GammaMin(0.0),m_EkinMin(0.0),m_Ey(NULL),m_Sr(NULL),
  m_om(NULL),m_Omg(NULL),m_sigmaGas(NULL),m_sigmaFoil(NULL),
  m_msg("TRTTransitionRadiation")
{
  m_radiators.clear();
  m_XMLhandler = new TRRegionXMLHandler( this );

  SetProcessSubType(  fTransitionRadiation );

  ATH_MSG_DEBUG ( "##### Constructor TRTTransitionRadiation" );

  Initialize();

  ATH_MSG_DEBUG ( "##### Constructor TRTTransitionRadiation done" );

}

///////////////////////////////////////////////////////////////////////////

TRTTransitionRadiation::~TRTTransitionRadiation() {
  delete [] m_Ey;
  delete [] m_Sr;
  delete [] m_Omg;
  delete [] m_om;
  delete [] m_sigmaGas;
  delete [] m_sigmaFoil;
}

///////////////////////////////////////////////////////////////////////////
void TRTTransitionRadiation::AddRadiatorParameters(TRTRadiatorParameters p) {

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " New Radiator parameters being defined for TR process"
                    << endmsg;
    msg(MSG::DEBUG) << " Volume " << p.GetLogicalVolume()->GetName() << endmsg;
    msg(MSG::DEBUG) << " FoilThickness " << p.GetFoilThickness() << endmsg;
    msg(MSG::DEBUG) << " GasThickness  " << p.GetGasThickness() << endmsg;
  }

  m_radiators.push_back(p);

  return;
}


void TRTTransitionRadiation::Initialize() {
  ATH_MSG_DEBUG ( "Constructor TRTTransitionRadiation::Initialize()" );
  m_NumBins     = 100;         // Bins for TR generation (Nevski has 25 bins)
  m_MinEnergyTR =   2.0*CLHEP::keV;   // Minimum TR energy
  m_MaxEnergyTR =  50.0*CLHEP::keV;   // Maximum TR energy

  m_GammaMin = 500.;          // Min. value of Lorentz gamma for TR generation
  m_EkinMin  = (m_GammaMin-1.)*CLHEP::electron_mass_c2;

  //We get the materials from GeoMaterial manager and convert them to G4Materials
  //
  //NB: Maybe we should control this from TRT_GeoModel instead?
  //
  //    On a related note we should get rid of the xml files too -
  //    they are a loophole in the versioning.

  // Get material information from storegate.
  ISvcLocator *svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StoreGateSvc *detStore(NULL);
  if( StatusCode::SUCCESS != svcLocator->service( "DetectorStore", detStore ) ) {
    ATH_MSG_FATAL ( "Can not access Detector Store " );
    return;
  };
  DataHandle<StoredMaterialManager> sMaterialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(sMaterialManager, std::string("MATERIALS"))) {
    ATH_MSG_FATAL ( "Could not retrieve material manager from Detector Store" );
    return;
  };
  AbsMaterialManager *materialManager = &*sMaterialManager;//TK: figure out why &* (overloaded * ?)

  Geo2G4MaterialFactory geo2g4_material_fact;//Note - this is a very lightweight class!
  G4Material *g4mat_Gas(geo2g4_material_fact.Build(materialManager->getMaterial("trt::CO2")));
  G4Material *g4mat_FoilMaterial(geo2g4_material_fact.Build(materialManager->getMaterial("trt::CH2")));
  //Note: One might wonder if we now own g4mat_Gas and
  //g4mat_FoilMaterial, but since the elementfactory used inside
  //Geo2G4MaterialFactory is static, we must NOT delete them!!

  //   FadsMolecule FadsGas( "TRTCO2", 1.842*mg/cm3, 2 );
  //   FadsGas.AddElement( "C", 1 );
  //   FadsGas.AddElement( "O", 2 );
  //   g4mat_Gas = FadsGas.GetG4Material();
  //   FadsMolecule FadsFoilMaterial( "TRTCH2", 0.935*g/cm3, 2 );
  //   FadsFoilMaterial.AddElement( "C", 1 );
  //   FadsFoilMaterial.AddElement( "H", 2 );
  //   g4mat_FoilMaterial = FadsFoilMaterial.GetG4Material();


  G4double PlasmaCof(4.0*M_PI*CLHEP::fine_structure_const*CLHEP::hbarc*CLHEP::hbarc*CLHEP::hbarc/CLHEP::electron_mass_c2);

  // Plate and gas indicies
  m_WplasmaFoil = sqrt( PlasmaCof * g4mat_FoilMaterial->GetElectronDensity() );
  m_WplasmaGas  = sqrt( PlasmaCof * g4mat_Gas->GetElectronDensity() );

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Foil material : " << g4mat_FoilMaterial->GetName()
                    << "; plasma energy : " << m_WplasmaFoil/CLHEP::eV << " eV"
                    << endmsg;
    msg(MSG::DEBUG) << "Gas : " << g4mat_Gas->GetName()
                    << "; plasma energy : " << m_WplasmaGas/CLHEP::eV << " eV"
                    << G4endl;
  }

  m_Omg       = new G4double[m_NumBins];
  m_om        = new G4double[m_NumBins];
  m_Ey        = new G4double[m_NumBins];
  m_Sr        = new G4double[m_NumBins];
  m_sigmaGas  = new G4double[m_NumBins];
  m_sigmaFoil = new G4double[m_NumBins];

  for(G4int j(0); j<m_NumBins; ++j) {
    m_Omg[j] = log( m_MinEnergyTR ) +
      j*log( m_MaxEnergyTR/m_MinEnergyTR )/(m_NumBins-1);
    m_om [j] = exp( m_Omg[j] );
    m_sigmaGas[j]  = ComputePhotoAbsCof( g4mat_Gas, m_om[j] );
    m_sigmaFoil[j] = ComputePhotoAbsCof( g4mat_FoilMaterial, m_om[j] );
  }

  const std::string file=PathResolver::find_file(m_xmlfilename,"DATAPATH");
  if (!file.empty())
    {
      m_XMLhandler->Process(file);
    }
  else ATH_MSG_WARNING("File "<<m_xmlfilename<<" not found! Expect problems.");

  ATH_MSG_DEBUG ( "Constructor TRTTransitionRadiation::Initialize() DONE!" );
  return;

}

///////////////////////////////////////////////////////////////////////////////
//
// Returns condition for application of the model depending on particle type


G4bool TRTTransitionRadiation::IsApplicable(const G4ParticleDefinition& particle) {
  //return true if PDG Charge and PDG Mass are non-zero.
  return  ( fabs(particle.GetPDGCharge())>std::numeric_limits<double>::epsilon() && fabs(particle.GetPDGMass())>std::numeric_limits<double>::epsilon() ) ;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetContinuousStepLimit
//

G4double TRTTransitionRadiation::GetMeanFreePath(const G4Track& aTrack,
                                                 G4double,
                                                 G4ForceCondition* condition) {
  const G4DynamicParticle *aParticle(aTrack.GetDynamicParticle());
  if(fabs(aParticle->GetDefinition()->GetPDGCharge())< std::numeric_limits<double>::epsilon() ||
     fabs(aParticle->GetDefinition()->GetPDGMass())<std::numeric_limits<double>::epsilon()  ) {
    *condition = NotForced;
  } else {
    *condition = Forced;
  }
  return DBL_MAX;
}

///////////////////////////////////////////////////////////////////////////////
//
//
G4VParticleChange* TRTTransitionRadiation::PostStepDoIt( const G4Track& aTrack,
                                                         const G4Step& aStep ) {
  aParticleChange.Initialize(aTrack);

  // Obtain kinetic energy
  const G4DynamicParticle *aParticle(aTrack.GetDynamicParticle());
  G4double KineticEnergy(aParticle->GetKineticEnergy());

  // Kinetic energy too low for TR generation for any kind of particle
  if ( KineticEnergy < m_EkinMin )
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

  // Particle properties ...
  G4double ParticleMass(aParticle->GetDefinition()->GetPDGMass());
  G4double Gamma(1.0 + KineticEnergy/ParticleMass);

  if( Gamma < m_GammaMin ) {
    ATH_MSG_VERBOSE ( "Leaving PostStepDoIt(): Gamma < " << m_GammaMin );
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  G4double FoilThickness(0.);
  G4double GasThickness(0.);
  BEflag BEflag(BARREL); //enum?

  // Check whether particle is inside radiator
  std::vector<TRTRadiatorParameters>::const_iterator currentRadiator(m_radiators.begin());
  const std::vector<TRTRadiatorParameters>::const_iterator endOfRadiators(m_radiators.end());
  while(currentRadiator!=endOfRadiators) {
    if ( currentRadiator->GetLogicalVolume() ==
         aTrack.GetVolume()->GetLogicalVolume() ) {
      FoilThickness = currentRadiator->GetFoilThickness();
      GasThickness  = currentRadiator->GetGasThickness();
      BEflag        = currentRadiator->GetBEflag();
      break;
    }
    ++currentRadiator;
  }
  // Particle not inside radiator - return
  if ( currentRadiator == endOfRadiators ) {
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  ATH_MSG_VERBOSE ( "PostStepDoIt(): Inside radiator "
                    << currentRadiator->GetLogicalVolume()->GetName() );

  G4ThreeVector ParticleDirection(aParticle->GetMomentumDirection());

  if ( BEflag == ENDCAP ) {
    G4double costh(fabs(ParticleDirection[2]));
    FoilThickness /= costh;
    GasThickness  /= costh;
  }

  G4double StepLength(aStep.GetStepLength());
  G4int FoilsTraversed(static_cast<G4int>(StepLength/(FoilThickness+GasThickness)+0.5));

  // Steplength so short, that no foils are crossed.
  // Forget it...
  // Actually the treatment we do is only valid for "many" foils.
  // Should we put this requirement higher?
  if ( FoilsTraversed == 0 ) {
    ATH_MSG_VERBOSE ( "Leaving PostStepDoIt(): No foils traversed." );
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  G4double Gy(0.), Gmany(0.), Neff(0.);

  // Generate, in the language of Artru:
  //   1) G_y
  //   2) N_{eff}
  //   3) G_{many}
  // Divide by photon energy to get photon spectrum (m_Ey)

  for( G4int i(0); i<m_NumBins; ++i ) {

#ifdef ARTRU                                    // Formulas from Artru
    Gy = XEmitanArtru( m_om[i],
                       Gamma,
                       GasThickness,
                       FoilThickness );
    Neff = NeffArtru( m_sigmaGas[i],
                      m_sigmaFoil[i],
                      GasThickness,
                      FoilThickness,
                      FoilsTraversed );
#else
    Gy = XEmitanNevski( m_om[i],                   // Formulas from Cherry via
                        Gamma,                     // Pavel Nevski in G3
                        GasThickness,
                        FoilThickness );
    Neff = NeffNevski( m_sigmaGas[i],
                       m_sigmaFoil[i],
                       GasThickness,
                       FoilThickness,
                       FoilsTraversed );
#endif

    Gmany = Neff*Gy;                              // Energy spectrum
    m_Ey[i] = Gmany / m_om[i];                    // Photon spectrum

  }

  // Integrate spectrum
  G4double Sph(XInteg( m_Ey, m_Sr ));
  G4double Sph_org(Sph);

  if ( ( BEflag == BARREL ) && (Gamma > 0.0) ) {
    G4double tempLog10Gamma(log10(Gamma)); //Hardcoded numbers from TestBeam study (E.Klinkby). Better to move elsewhere.
    G4double pHT_dEdxData = -0.001 + 0.0005*tempLog10Gamma;
    G4double pHT_TRData   = 0.15/(1.0 + exp(-(tempLog10Gamma - 3.3)/0.27));
    G4double pHT_dEdxMC = -0.0031 + 0.00016*tempLog10Gamma;
    G4double pHT_TRMC   = 0.1289/(1.0 + exp(-(tempLog10Gamma - 3.01)/0.1253));

    G4double fudge = std::min(2.0,(pHT_dEdxData+pHT_TRData)/( pHT_dEdxMC+pHT_TRMC));

    Sph *= fudge;
  }

  // Now how many photons are generated?
  G4long NumberOfPhotonsGenerated(CLHEP::RandPoisson::shoot(Sph));

  //reset Sph to original value.
  Sph = Sph_org;

  ATH_MSG_VERBOSE ( ">>>> FoilsTraversed = " << FoilsTraversed
                    << ", Gamma = " << Gamma
                    << ", Sph = " << Sph
                    << ", Nph = " << NumberOfPhotonsGenerated );
  // No photons, return
  if ( NumberOfPhotonsGenerated <= 0 ) {
    ATH_MSG_VERBOSE ( "Leaving TRTTransitionRadiation::PostStepDoIt: "
                      << "No photons generated." );

    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  // Now, generate the photons
  aParticleChange.SetNumberOfSecondaries( NumberOfPhotonsGenerated );
  G4StepPoint *pPostStepPoint(aStep.GetPostStepPoint());
  G4ThreeVector x0(pPostStepPoint->GetPosition());

  G4double Esum(0.), XRannu(0.), Ephot(0.), Theta(0.), Phi(0.),
    X(0.), Y(0.), Z(0.);

  G4ThreeVector pos(0),TRPhotonDirection(0);

  for( G4int i(0); i<NumberOfPhotonsGenerated; ++i ) {
    XRannu = CLHEP::RandFlat::shoot();
    Ephot = exp( XFinter( XRannu*Sph, m_Sr, m_Omg ) );

    Esum += Ephot;

    pos  = pPostStepPoint->GetPosition();

    ATH_MSG_VERBOSE ( " Photon generated with energy : " << Ephot/CLHEP::keV
                      << " keV; position: ( " << pos.x()/CLHEP::cm << " cm, "
                      << pos.y()/CLHEP::cm << " cm, " << pos.z()/CLHEP::cm << " cm )" );

    // Angle w.r.t. electron direction (this is not correct but anyway...)
    Theta = fabs( CLHEP::RandGaussZiggurat::shoot( 0.0, M_PI/Gamma ) );
    if( Theta >= 0.1 ) Theta = 0.1;

    Phi = (2.*M_PI)*CLHEP::RandFlat::shoot();

    X = sin(Theta)*cos(Phi);
    Y = sin(Theta)*sin(Phi);
    Z = cos(Theta);

    TRPhotonDirection.set(X,Y,Z);
    TRPhotonDirection.rotateUz( ParticleDirection );
    TRPhotonDirection.unit();

    G4DynamicParticle*
      TRPhoton = new G4DynamicParticle( G4Gamma::Gamma(),
                                        TRPhotonDirection,
                                        Ephot );
    aParticleChange.AddSecondary( TRPhoton );

  }

  aParticleChange.ProposeEnergy( KineticEnergy-Esum );

  ATH_MSG_VERBOSE ( "Leaving TRTTransitionRadiation::PostStepDoIt; "
                    << "Number of TR photons generated: " << NumberOfPhotonsGenerated );

  return &aParticleChange;
}

inline G4double TRTTransitionRadiation::NeffNevski( const G4double & sigGas,
                                                    const G4double & sigFoil,
                                                    const G4double & GasThickness,
                                                    const G4double & FoilThickness,
                                                    const G4int &    FoilsTraversed ) const {
  if ( FoilsTraversed <= 0 )
    return 0.;

  G4double C1 = sigGas * GasThickness*FoilsTraversed;
  G4double C2 = sigFoil*FoilThickness*FoilsTraversed;

  G4double W1 = exp(-C1);
  G4double W2 = exp(-C2);

  return FoilsTraversed * (1-W1*W2)/(C1+C2);
}

inline G4double TRTTransitionRadiation::NeffArtru( const G4double & sigGas,
                                                   const G4double & sigFoil,
                                                   const G4double & GasThickness,
                                                   const G4double & FoilThickness,
                                                   const G4int &    FoilsTraversed ) const {
  if ( FoilsTraversed <= 0 )
    return 0.;

  G4double sigma = FoilThickness*sigFoil + GasThickness*sigGas;
  G4double xNF = FoilsTraversed;
  return (1.-exp(-xNF*sigma))/(1.-exp(-sigma));
}

////////////////////////////////////////////////////////////////////////
//
// Nevskis XEmitan-function
//
//inline G4double max(G4double a, G4double b) {return a>b?a:b;}

inline G4double TRTTransitionRadiation::XEmitanNevski( const G4double & PhotonEnergy,
                                                       const G4double & Gamma,
                                                       const G4double & Al1,
                                                       const G4double & Al2 ) const {
  //FIXME hardcoded numbers are not good.
  G4double eps = 0.05;

  G4double Al = Al1 + Al2;

  G4double CK = 1/(2*M_PI*CLHEP::hbarc);

  // Plasma frequencies:
  G4double om1 = m_WplasmaGas;
  G4double om2 = m_WplasmaFoil;

  G4double om12 = (om1*om1-om2*om2)/PhotonEnergy;
  G4double oms = (Al1*om1*om1+Al2*om2*om2)/Al;
  G4double Ao = 0.5*Al1*om12*CK;
  G4double Bo = 0.5*Al2*om12*CK;
  G4double ro = sqrt(oms)/Gamma;
  G4double co = 0.5*(PhotonEnergy/(Gamma*Gamma)+oms/PhotonEnergy);
  G4double cc = co*Al*CK;
  G4double cb = M_PI*Al2/Al;
  G4int i1 = static_cast<G4int>(1.0+(ro>co?ro:co)*Al*CK);//(G4int)(1.0+max(ro,co)*Al*CK);
  G4int i2 = static_cast<G4int>(Gamma*ro*Al*CK);

  G4double Sum = 0.0;
  G4double Smx = 0.0;
  G4double Sm1 = 0.0;
  G4double S = 0.0;
  G4double So = 0.0;

  G4double Sx = 0.0;

  for(G4int i(i1); i<=i2; ++i) {
    Sx = So;
    So = S;
    S = (static_cast<G4double>(i)-cc)*(sin(cb*(static_cast<G4double>(i)-Ao))/((static_cast<G4double>(i)-Ao)*(static_cast<G4double>(i)+Bo)))*
      (sin(cb*(static_cast<G4double>(i)-Ao))/((static_cast<G4double>(i)-Ao)*(static_cast<G4double>(i)+Bo)));
    Smx=Smx>S?Smx:S;//max(Smx,S);
    Sm1=Sm1>S?Sm1:S;//max(Sm1,S);
    Sum += S;

    if( (So<Sx)&&(So<=S) ) {
      if( Sm1<eps*Smx ) break;
      Sm1 = 0.0;
    }
  }

  return ( CLHEP::fine_structure_const*om12*om12*Al*Al*CK*CK*Sum/M_PI );
}

////////////////////////////////////////////////////////////////////////
//
// Same as above but for the Artru way of doing things
//
inline G4double TRTTransitionRadiation::XEmitanArtru( const G4double & omega,
                                                      const G4double & Gamma,
                                                      const G4double & d2,
                                                      const G4double & d1 ) const {
  G4double phimax = 4.*M_PI;
  G4double ginv2 = 1./(Gamma*Gamma);
  G4double tau = d2/d1;

  G4double mom = omega/CLHEP::hbarc;

  G4double xi1 = m_WplasmaFoil/omega;
  G4double xi2 = m_WplasmaGas/omega;

  G4double xi1sq = xi1*xi1;
  G4double xi2sq = xi2*xi2;

  G4double Z1 = 2./(mom*(ginv2+xi1sq));
  G4double Z2 = 2./(mom*(ginv2+xi2sq));

  G4double a = d1/Z2;
  G4double V = d1*(1./Z1-1./Z2);

  G4double p0   = mom*d1*(xi1sq-xi2sq)/(4.*M_PI);
  G4double pmin = p0 + a*(1.+tau)/(2.*M_PI);
  G4double ymax = tau*phimax;
  G4double pmax = p0 + ymax*(1.+tau)/(2.*M_PI);

  G4int ipmin = (int)pmin+1;
  G4int ipmax = (int)pmax;

  G4double sum=0.;
  G4double y0 = -0.5*mom*d1*(xi1sq-xi2sq)/(1+tau);
  G4double dy = (2.*M_PI)/(1+tau);
  G4double y;

  G4double term = 0.;
  G4double t1   = 0.;
  G4double t2   = 0.;
  G4double tx   = 0.;

  for ( G4int ip(ipmin); ip<=ipmax; ++ip ) {
    t2 = t1;
    t1 = term;

    y = y0 + ip*dy;
    term  = (1./y-1./(y+V))*sin(0.5*(y+V));
    term *= term;
    term *= y-a;
    sum  += term;

    //G4double theta2 = 2.*y*tau/(mom*d2) - ginv2 - xi2sq;
    //G4double theta  = sqrt(theta2);

    tx = (tx>term) ? tx : term;

    // Locate local maxima (previous term)

    if ( t2 > 0. && t1 > term && t1 > t2 ) {
      if ( t1 < 0.0001*sum ) break;
    }
  }

  return ( sum*4.*2.*CLHEP::fine_structure_const/(1.+tau));
}

////////////////////////////////////////////////////////////////////////
//
// Nevskis XInteg function
// Spectrum integration
//

inline G4double TRTTransitionRadiation::XInteg( const G4double * yy, G4double * ss ) const {
  G4double S(0.);
  ss[0] = 0.;

  for( G4int i(1); i<m_NumBins; ++i ) {
    S += 0.5*( m_Omg[i]-m_Omg[i-1] )*( yy[i-1]*m_om[i-1] + yy[i]*m_om[i] );
    ss[i] = S;
  }

  return S;
}

////////////////////////////////////////////////////////////////////////
//
// Nevskis XFINTER function

G4double TRTTransitionRadiation::XFinter( const G4double & X, const G4double* A, const G4double* F ) const {
  G4int K1(0);
  G4int K2(0);

  if( (K1>=m_NumBins) || ( X < A[K1] ) ) K1 = 0; //FIXME This line has no effect?
  if( (K2>=m_NumBins) || ( X > A[K2] ) ) K2 = m_NumBins-1;

  G4int K(0);
  while( K2-K1>1 ) {
    K = static_cast<G4int>((K1+K2)/2);
    if( A[K]<X ) {
      K1 = K;
    } else {
      K2 = K;
    }
  }

  G4double X1(A[K1]);
  G4double X2(A[K1+1]);

  G4double xfinter((F[K1]*(X-X2)+F[K1+1]*(X1-X))/(X1-X2));
  return xfinter;
}

////////////////////////////////////////////////////////////////////////
//
// Computes Sandia photo absorption cross section coefficients for Material

G4double TRTTransitionRadiation::ComputePhotoAbsCof( const G4Material* Material,
                                                     const G4double & PhotonEnergy ) const {

  const G4double *SandiaCof(Material->GetSandiaTable()->GetSandiaCofForMaterial(PhotonEnergy));

  const G4double Energy2(PhotonEnergy*PhotonEnergy);
  const G4double Energy3(PhotonEnergy*Energy2);
  const G4double Energy4(Energy2*Energy2);

  return ( SandiaCof[0]/PhotonEnergy +
           SandiaCof[1]/Energy2 +
           SandiaCof[2]/Energy3 +
           SandiaCof[3]/Energy4 );
}
