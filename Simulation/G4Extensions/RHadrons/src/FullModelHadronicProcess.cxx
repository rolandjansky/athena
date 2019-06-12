/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FullModelHadronicProcess.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessHelper.hh"
#include "G4ParticleTable.hh"
#include "FullModelReactionDynamics.hh"
#include "G4HadReentrentException.hh"
#include "CustomPDGParser.h"
#include "CustomParticle.h"


// Macro to help with overzealous compiler warnings
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif


FullModelHadronicProcess::FullModelHadronicProcess(const G4String& processName) :
  G4VDiscreteProcess(processName,fUserDefined),m_theParticle(0),m_newParticle(0),m_toyModel(false),m_cache(0)
{
  // Instantiating helper class
  m_theHelper = G4ProcessHelper::Instance();

}


G4bool FullModelHadronicProcess::IsApplicable(const G4ParticleDefinition& aP)
{

  return m_theHelper->ApplicabilityTester(aP);

}

G4double FullModelHadronicProcess::GetMicroscopicCrossSection(const G4DynamicParticle *aParticle,
                                                              const G4Element *anElement,
                                                              G4double UNUSED(aTemp))
{
  // Get the cross section for this particle/element combination from the ProcessHelper
  G4double InclXsec = m_theHelper->GetInclusiveCrossSection(aParticle,anElement);
  //  G4cout<<"Returned cross section from helper was: "<<InclXsec/CLHEP::millibarn<<" millibarn"<<G4endl;

  // Need to provide Set-methods for these in time
  G4double HighestEnergyLimit = 10 * CLHEP::TeV  ;
  G4double LowestEnergyLimit = 1 * CLHEP::eV;

  G4double ParticleEnergy = aParticle->GetKineticEnergy();


  if (ParticleEnergy > HighestEnergyLimit || ParticleEnergy < LowestEnergyLimit){
    return 0;
  } else {
    //    G4cout << "Microscopic Cross Section: "<<InclXsec / CLHEP::millibarn<<" millibarn"<<G4endl;
    return InclXsec;
  }

}

G4double FullModelHadronicProcess::GetMeanFreePath(const G4Track& aTrack, G4double, G4ForceCondition*)
{
  G4Material *aMaterial = aTrack.GetMaterial();
  const G4DynamicParticle *aParticle = aTrack.GetDynamicParticle();
  G4double sigma = 0.0;

  G4int nElements = aMaterial->GetNumberOfElements();

  const G4double *theAtomicNumDensityVector =
    aMaterial->GetAtomicNumDensityVector();
  G4double aTemp = aMaterial->GetTemperature();

  for( G4int i=0; i<nElements; ++i )
    {
      G4double xSection =
        GetMicroscopicCrossSection( aParticle, (*aMaterial->GetElementVector())[i], aTemp);
      sigma += theAtomicNumDensityVector[i] * xSection;
    }

  return 1.0/sigma;

}


G4VParticleChange* FullModelHadronicProcess::PostStepDoIt(const G4Track& aTrack,
                                                          const G4Step&  UNUSED(aStep))
{
  // A little setting up
  aParticleChange.Initialize(aTrack);
  //  G4DynamicParticle* OrgPart = const_cast<G4DynamicParticle*>(aTrack.GetDynamicParticle());
  G4DynamicParticle* IncidentRhadron = const_cast<G4DynamicParticle*>(aTrack.GetDynamicParticle());
  CustomParticle* CustomIncident = static_cast<CustomParticle*>(IncidentRhadron->GetDefinition());
  const G4ThreeVector aPosition = aTrack.GetPosition();
  const G4int theIncidentPDG = IncidentRhadron->GetDefinition()->GetPDGEncoding();
  G4ParticleTable* theParticleTable = G4ParticleTable::GetParticleTable();
  std::vector<G4ParticleDefinition*> theParticleDefinitions;
  G4bool IncidentSurvives = false;
  G4bool TargetSurvives = false;
  G4Nucleus targetNucleus(aTrack.GetMaterial());
  G4ParticleDefinition* outgoingRhadron=0;
  G4ParticleDefinition* outgoingCloud=0;
  G4ParticleDefinition* outgoingTarget=0;


  G4ThreeVector p_0 = IncidentRhadron->GetMomentum();

  //  static int n=0;

  G4double e_kin_0 = IncidentRhadron->GetKineticEnergy();

  G4DynamicParticle* cloudParticle = new G4DynamicParticle();

  cloudParticle->SetDefinition(CustomIncident->GetCloud());

  if(cloudParticle->GetDefinition() == 0)
    {
      std::cout << "FullModelHadronicProcess::PostStepDoIt  Definition of particle cloud not available!!" << std::endl;
    }
  double scale=cloudParticle->GetDefinition()->GetPDGMass()/IncidentRhadron->GetDefinition()->GetPDGMass();

  G4LorentzVector cloudMomentum;
  cloudMomentum.setVectM(IncidentRhadron->GetMomentum()*scale,cloudParticle->GetDefinition()->GetPDGMass());
  G4LorentzVector gluinoMomentum;

  gluinoMomentum.setVectM(IncidentRhadron->GetMomentum()*(1.-scale),CustomIncident->GetSpectator()->GetPDGMass());

  G4LorentzVector FullRhadron4Momentum = IncidentRhadron->Get4Momentum();
  G4LorentzVector Cloud4Momentum = cloudMomentum;

  cloudParticle->Set4Momentum(cloudMomentum);

  G4DynamicParticle* OrgPart = cloudParticle;

  G4double ek = OrgPart->GetKineticEnergy();
  G4double amas = OrgPart->GetDefinition()->GetPDGMass();
  G4double tkin = targetNucleus.Cinema( ek );
  ek += tkin;
  OrgPart->SetKineticEnergy( ek );
  G4double et = ek + amas;
  G4double p = std::sqrt( std::abs((et-amas)*(et+amas)) );
  G4double pp = OrgPart->GetMomentum().mag();
  if( pp > 0.0 )
    {
      G4ThreeVector momentum = OrgPart->GetMomentum();
      OrgPart->SetMomentum( momentum * (p/pp) );
    }

  // calculate black track energies
  if(ek>0.) {tkin = targetNucleus.EvaporationEffects( ek ); ek -= tkin;}
  if(ek+gluinoMomentum.e()-gluinoMomentum.m()<=0.1*CLHEP::MeV||ek<=0.) {
    G4cout<<"Stopping particle...:"<<G4endl;
    G4cout<<"ek: "<<ek/CLHEP::MeV<<" MeV"<<G4endl;
    aParticleChange.ProposeTrackStatus( fStopButAlive );
    //    h4->Fill(aPosition.x()/CLHEP::cm);
    return &aParticleChange;
  }
  OrgPart->SetKineticEnergy( ek );
  et = ek + amas;
  p = std::sqrt( std::abs((et-amas)*(et+amas)) );
  pp = OrgPart->GetMomentum().mag();

  if( pp > 0.0 )
    {
      G4ThreeVector momentum = OrgPart->GetMomentum();
      OrgPart->SetMomentum( momentum * (p/pp) );
    }



  //Get the final state particles

  G4ParticleDefinition* aTarget;
  ReactionProduct rp = m_theHelper->GetFinalState(aTrack,aTarget);

  //FIXME: Due to force2to2 always being set to false the while loop
  //below can never be called, so have commented it out.

  // G4bool force2to2 = false;
  // //  G4cout<<"Trying to get final state..."<<G4endl;
  // while(rp.size()!=2 && force2to2){
  //   rp = m_theHelper->GetFinalState(aTrack,aTarget);
  // }

  G4int NumberOfSecondaries = rp.size();
  //  G4cout<<"Multiplicity of selected final state: "<<rp.size()<<G4endl;

  //Getting CMS transforms. Boosting is done at histogram filling
  G4LorentzVector Target4Momentum;
  Target4Momentum.setVectM(CLHEP::Hep3Vector(0.,0.,0.),aTarget->GetPDGMass());
  //  Target4Momentum.setVectM(0.,targetNucleus.GetN()*CLHEP::GeV);
  G4LorentzVector psum_full,psum_cloud;
  psum_full = FullRhadron4Momentum + Target4Momentum;
  psum_cloud = Cloud4Momentum + Target4Momentum;
  G4ThreeVector trafo_full_cms = (-1)*psum_full.boostVector();
  G4ThreeVector trafo_cloud_cms = (-1)*psum_cloud.boostVector();

  // OK Let's make some particles :-)
  // We're not using them for anything yet, I know, but let's make sure the machinery is there

  for(ReactionProduct::iterator it  = rp.begin();
      it != rp.end();
      it++)
    {
      G4ParticleDefinition* tempDef = theParticleTable->FindParticle(*it);
      CustomParticle* tempCust = dynamic_cast<CustomParticle*>(tempDef);
      if (tempDef==aTarget) TargetSurvives = true;

      if (tempCust!=0)
        {
          outgoingRhadron = tempDef;
          outgoingCloud=tempCust->GetCloud();
          if(outgoingCloud == 0)
            {
              std::cout << "FullModelHadronicProcess::PostStepDoIt  Definition of outgoing particle cloud for " << tempDef->GetParticleName() << " not available!!" << std::endl;
            }
        }

      if (tempDef==G4Proton::Proton()||tempDef==G4Neutron::Neutron()) outgoingTarget = tempDef;
      if (tempCust==0&&rp.size()==2) outgoingTarget = tempDef;
      if (tempDef->GetPDGEncoding()==theIncidentPDG) {
        IncidentSurvives = true;
      } else {
        theParticleDefinitions.push_back(tempDef);
      }
    }

  if (NULL==outgoingRhadron) {
    //FIXME: What should be done in the case that outgoingRhadron is
    //still 0 at this point? Throwing an exception for now to prevent
    //a null-pointer being dereferenced later on in the code.
    G4ExceptionDescription description;
    description << "PostStepDoIt: outgoingRHadron is null";
    G4Exception("FullModelHadronicProcess", "NoOutGoingRHadron", FatalException, description);
  }

  if (outgoingTarget==0) outgoingTarget = theParticleTable->FindParticle(rp[1]);

  // If the incident particle survives it is not a "secondary", although
  // it would probably be easier to fStopAndKill the track every time.
  if(IncidentSurvives) NumberOfSecondaries--;
  aParticleChange.SetNumberOfSecondaries(NumberOfSecondaries);


  // ADAPTED FROM G4LEPionMinusInelastic::ApplyYourself
  // It is bloody ugly, but such is the way of cut 'n' paste

  // Set up the incident
  const G4HadProjectile* originalIncident = new G4HadProjectile(*OrgPart);//This is where rotation to z-axis is done (Aarrggh!)


  //Maybe I need to calculate trafo from R-hadron... Bollocks! Labframe does not move. CMS does.
  G4HadProjectile* org2 = new G4HadProjectile(*OrgPart);
  G4LorentzRotation trans = org2->GetTrafoToLab();
  delete org2;

  G4DynamicParticle *originalTarget = new G4DynamicParticle;
  originalTarget->SetDefinition(aTarget);

  G4ReactionProduct targetParticle(aTarget);


  G4ReactionProduct currentParticle(const_cast<G4ParticleDefinition *>(originalIncident->GetDefinition() ) );
  currentParticle.SetMomentum( originalIncident->Get4Momentum().vect() );
  currentParticle.SetKineticEnergy( originalIncident->GetKineticEnergy() );


  G4ReactionProduct modifiedOriginal = currentParticle;

  currentParticle.SetSide( 1 ); // incident always goes in forward hemisphere
  targetParticle.SetSide( -1 );  // target always goes in backward hemisphere
  G4bool incidentHasChanged = false;
  if (!IncidentSurvives) incidentHasChanged = true; //I wonder if I am supposed to do this...
  G4bool targetHasChanged = false;
  if (!TargetSurvives) targetHasChanged = true; //Ditto here
  G4bool quasiElastic = false;
  if (rp.size()==2) quasiElastic = true; //Oh well...
  G4FastVector<G4ReactionProduct,GHADLISTSIZE> vec;  // vec will contain the secondary particles
  G4int vecLen = 0;
  vec.Initialize( 0 );



  // I hope my understanding of "secondary" is correct here
  // I think that it entails only what is not a surviving incident of target

  for (G4int i = 0; i!=NumberOfSecondaries;i++){
    if(theParticleDefinitions[i]!=aTarget
       && theParticleDefinitions[i]!=originalIncident->GetDefinition()
       && theParticleDefinitions[i]!=outgoingRhadron
       && theParticleDefinitions[i]!=outgoingTarget)
      {
        G4ReactionProduct* pa = new G4ReactionProduct;
        pa->SetDefinition( theParticleDefinitions[i] );
        (G4UniformRand() < 0.5) ? pa->SetSide( -1 ) : pa->SetSide( 1 );
        vec.SetElement( vecLen++, pa );
      }
  }

  //  if (incidentHasChanged) currentParticle.SetDefinitionAndUpdateE( outgoingRhadron );

  if (incidentHasChanged) currentParticle.SetDefinitionAndUpdateE( outgoingCloud );
  if (incidentHasChanged) modifiedOriginal.SetDefinition( outgoingCloud );//Is this correct? It solves the "free energy" checking in ReactionDynamics
  if (targetHasChanged) targetParticle.SetDefinitionAndUpdateE( outgoingTarget );


  CalculateMomenta( vec, vecLen,
                    originalIncident, originalTarget, modifiedOriginal,
                    targetNucleus, currentParticle, targetParticle,
                    incidentHasChanged, targetHasChanged, quasiElastic );


  G4String cPname = currentParticle.GetDefinition()->GetParticleName();


  aParticleChange.SetNumberOfSecondaries(vecLen+NumberOfSecondaries);
  G4double e_kin;
  G4LorentzVector cloud_p4_new; //Cloud 4-momentum in lab after collision

  cloud_p4_new.setVectM(currentParticle.GetMomentum(),currentParticle.GetMass());
  cloud_p4_new *= trans;

  G4LorentzVector cloud_p4_old_full = Cloud4Momentum; //quark system in CMS BEFORE collision
  cloud_p4_old_full.boost(trafo_full_cms);
  G4LorentzVector cloud_p4_old_cloud = Cloud4Momentum; //quark system in cloud CMS BEFORE collision
  cloud_p4_old_cloud.boost(trafo_cloud_cms);
  G4LorentzVector cloud_p4_full = cloud_p4_new; //quark system in CMS AFTER collision
  cloud_p4_full.boost(trafo_full_cms);
  G4LorentzVector cloud_p4_cloud = cloud_p4_new; //quark system in cloud CMS AFTER collision
  cloud_p4_cloud.boost(trafo_cloud_cms);

  G4LorentzVector p_g_cms = gluinoMomentum; //gluino in CMS BEFORE collision
  p_g_cms.boost(trafo_full_cms);

  G4LorentzVector p4_new;
  p4_new.setVectM( cloud_p4_new.v() + gluinoMomentum.v(), outgoingRhadron->GetPDGMass() );

  G4ThreeVector p_new;
  p_new = p4_new.vect();

  aParticleChange.ProposeLocalEnergyDeposit((p4_new-cloud_p4_new-gluinoMomentum).m());

  if( incidentHasChanged )
    {
      G4DynamicParticle* p0 = new G4DynamicParticle;
      p0->SetDefinition( outgoingRhadron );
      p0->SetMomentum( p_new );

      // May need to run SetDefinitionAndUpdateE here...
      G4Track* Track0 = new G4Track(p0,
                                    aTrack.GetGlobalTime(),
                                    aPosition);
      aParticleChange.AddSecondary(Track0);

      // Because of the above calculations, and the use of mass, there's going to be a lot of squaring and
      //  square rooting to get the total energy.  For that reason, we should allow a little buffer before
      //  we freak out over energy non-conservation...
      if(p0->GetKineticEnergy()>e_kin_0+1.e-9) { // Allow 1 meV of energy non-conservation in an interaction
        G4cout<<"ALAAAAARM!!! (incident changed from "
              <<IncidentRhadron->GetDefinition()->GetParticleName()
              <<" to "<<p0->GetDefinition()->GetParticleName()<<")"<<G4endl;
        G4cout<<"Energy loss: "<<(e_kin_0-p0->GetKineticEnergy())/CLHEP::GeV<<" GeV (should be positive)"<<G4endl;
        //Turns out problem is only in 2 -> 3 (Won't fix 2 -> 2 energy deposition)
        if(rp.size()!=3) G4cout<<"DOUBLE ALAAAAARM!!!"<<G4endl;
      } /*else {
          G4cout<<"NO ALAAAAARM!!!"<<G4endl;
          }*/
      if(std::abs(p0->GetKineticEnergy()-e_kin_0)>100*CLHEP::GeV) {
        G4cout<<"Diff. too big"<<G4endl;
      }

      aParticleChange.ProposeTrackStatus( fStopAndKill );
    }
  else
    {

      G4double p = p_new.mag();
      if( p > DBL_MIN )
        aParticleChange.ProposeMomentumDirection( p_new.x()/p, p_new.y()/p, p_new.z()/p );
      else
        aParticleChange.ProposeMomentumDirection( 1.0, 0.0, 0.0 );

      G4double aE = sqrt(p*p+(outgoingRhadron->GetPDGMass()*outgoingRhadron->GetPDGMass()) );
      e_kin = aE - outgoingRhadron->GetPDGMass();

      if(e_kin>e_kin_0) {
        G4cout<<"ALAAAAARM!!!"<<G4endl;
        G4cout<<"Energy loss: "<<(e_kin_0-e_kin)/CLHEP::GeV<<" GeV (should be positive)"<<G4endl;
        if(rp.size()!=3) G4cout<<"DOUBLE ALAAAAARM!!!"<<G4endl;
      }
      if(std::abs(e_kin-e_kin_0)>100*CLHEP::GeV) {
        G4cout<<"Diff. too big"<<G4endl;
      }

      if (std::fabs(aE)<.1*CLHEP::eV) aE=.1*CLHEP::eV;
      aParticleChange.ProposeEnergy( aE- outgoingRhadron->GetPDGMass() ); //I do not know if this is correct...
      if(std::abs(e_kin-e_kin_0)>100*CLHEP::GeV) {
        G4cout<<"Diff. too big"<<G4endl;
      }

    }

  if( targetParticle.GetMass() > 0.0 )  // targetParticle can be eliminated in TwoBody
    {
      G4DynamicParticle *p1 = new G4DynamicParticle;
      p1->SetDefinition( targetParticle.GetDefinition() );
      //      G4cout<<"Target secondary: "<<targetParticle.GetDefinition()->GetParticleName()<<G4endl;
      G4ThreeVector momentum = targetParticle.GetMomentum();
      momentum = momentum.rotate(m_cache,m_what);
      p1->SetMomentum( momentum );
      p1->SetMomentum( (trans*p1->Get4Momentum()).vect());
      G4Track* Track1 = new G4Track(p1,
                                    aTrack.GetGlobalTime(),
                                    aPosition);
      aParticleChange.AddSecondary(Track1);
    }
  G4DynamicParticle *pa;


  for(int i=0; i<vecLen; ++i )
    {
      pa = new G4DynamicParticle();
      pa->SetDefinition( vec[i]->GetDefinition() );
      pa->SetMomentum( vec[i]->GetMomentum() );
      pa->Set4Momentum(trans*(pa->Get4Momentum()));
      G4ThreeVector pvec = pa->GetMomentum();
      G4Track* Trackn = new G4Track(pa,
                                    aTrack.GetGlobalTime(),
                                    aPosition);
      aParticleChange.AddSecondary(Trackn);
      delete vec[i];
    }

  delete originalIncident;
  delete originalTarget;

  //clear interaction length
  ClearNumberOfInteractionLengthLeft();


  return &aParticleChange;

}


void FullModelHadronicProcess::CalculateMomenta(
                                                G4FastVector<G4ReactionProduct,GHADLISTSIZE> &vec,
                                                G4int &vecLen,
                                                const G4HadProjectile *originalIncident,   // the original incident particle
                                                const G4DynamicParticle *originalTarget,
                                                G4ReactionProduct &modifiedOriginal,   // Fermi motion and evap. effects included
                                                G4Nucleus &targetNucleus,
                                                G4ReactionProduct &currentParticle,
                                                G4ReactionProduct &targetParticle,
                                                G4bool &incidentHasChanged,
                                                G4bool &targetHasChanged,
                                                G4bool quasiElastic )
{
  FullModelReactionDynamics theReactionDynamics;

  m_cache = 0;
  m_what = originalIncident->Get4Momentum().vect();

  //Commented out like in casqmesmin.F where CALL STPAIR is commented out
  /*
    theReactionDynamics.ProduceStrangeParticlePairs( vec, vecLen,
    modifiedOriginal, originalTarget,
    currentParticle, targetParticle,
    incidentHasChanged, targetHasChanged );
  */

  if( quasiElastic )
    {
      //      G4cout<<"We are calling TwoBody..."<<G4endl;
      theReactionDynamics.TwoBody( vec, vecLen,
                                   modifiedOriginal, originalTarget,
                                   currentParticle, targetParticle,
                                   targetNucleus, targetHasChanged );

      return;
    }

  //If ProduceStrangeParticlePairs is commented out, let's cut this one as well
  G4ReactionProduct leadingStrangeParticle;
  G4bool leadFlag = MarkLeadingStrangeParticle( currentParticle,
                                                targetParticle,
                                                leadingStrangeParticle );



  //
  // Note: the number of secondaries can be reduced in GenerateXandPt and TwoCluster
  //
  G4bool finishedGenXPt = false;
  G4bool annihilation = false;
  if( originalIncident->GetDefinition()->GetPDGEncoding() < 0 &&
      currentParticle.GetMass() == 0.0 && targetParticle.GetMass() == 0.0 )
    {
      // original was an anti-particle and annihilation has taken place
      annihilation = true;
      G4double ekcor = 1.0;
      G4double ek = originalIncident->GetKineticEnergy();
      G4double ekOrg = ek;

      const G4double tarmas = originalTarget->GetDefinition()->GetPDGMass();
      if( ek > 1.0*CLHEP::GeV )ekcor = 1./(ek/CLHEP::GeV);
      const G4double atomicWeight = targetNucleus.AtomicMass(targetNucleus.GetA_asInt(),targetNucleus.GetZ_asInt()); //targetNucleus.GetN();
      ek = 2*tarmas + ek*(1.+ekcor/atomicWeight);

      G4double tkin = targetNucleus.Cinema( ek );
      ek += tkin;
      ekOrg += tkin;
      modifiedOriginal.SetKineticEnergy( ekOrg );
    }

  const G4double twsup[] = { 1.0, 0.7, 0.5, 0.3, 0.2, 0.1 };
  G4double rand1 = G4UniformRand();
  G4double rand2 = G4UniformRand();
  if (annihilation || vecLen >= 6 ||
      (modifiedOriginal.GetKineticEnergy()/CLHEP::GeV >= 1.0 &&
       ((rand1 < 0.5 &&
         (originalIncident->GetDefinition() == G4KaonPlus::KaonPlus() ||
          originalIncident->GetDefinition() == G4KaonMinus::KaonMinus() ||
          originalIncident->GetDefinition() == G4KaonZeroLong::KaonZeroLong() ||
          originalIncident->GetDefinition() == G4KaonZeroShort::KaonZeroShort()))
        || rand2 > twsup[vecLen])))
    finishedGenXPt =
      theReactionDynamics.GenerateXandPt(vec, vecLen,
                                         modifiedOriginal, originalIncident,
                                         currentParticle, targetParticle,
                                         targetNucleus, incidentHasChanged,
                                         targetHasChanged, leadFlag,
                                         leadingStrangeParticle);
  if( finishedGenXPt )
    {
      Rotate(vec, vecLen);
      return;
    }

  G4bool finishedTwoClu = false;
  if( modifiedOriginal.GetTotalMomentum()/CLHEP::MeV < 1.0 )
    {

      for(G4int i=0; i<vecLen; i++) delete vec[i];
      vecLen = 0;
    }
  else
    {

      theReactionDynamics.SuppressChargedPions( vec, vecLen,
                                                modifiedOriginal, currentParticle,
                                                targetNucleus,
                                                incidentHasChanged );

      try
        {
          finishedTwoClu = theReactionDynamics.TwoCluster( vec, vecLen,
                                                           modifiedOriginal, originalIncident,
                                                           currentParticle, targetParticle,
                                                           targetNucleus, incidentHasChanged,
                                                           targetHasChanged, leadFlag,
                                                           leadingStrangeParticle );
        }
      catch(G4HadReentrentException& aC)
        {
          aC.Report(G4cout);
          throw G4HadReentrentException(__FILE__, __LINE__, "Failing to calculate momenta");
        }
    }
  if( finishedTwoClu )
    {
      Rotate(vec, vecLen);
      return;
    }

  //
  // PNBlackTrackEnergy is the kinetic energy available for
  //   proton/neutron black track particles [was enp(1) in fortran code]
  // DTABlackTrackEnergy is the kinetic energy available for
  //   deuteron/triton/alpha particles      [was enp(3) in fortran code]
  //const G4double pnCutOff = 0.1;
  //const G4double dtaCutOff = 0.1;
  //if( (targetNucleus.GetN() >= 1.5)
  //    && !(incidentHasChanged || targetHasChanged)
  //    && (targetNucleus.GetPNBlackTrackEnergy()/CLHEP::MeV <= pnCutOff)
  //    && (targetNucleus.GetDTABlackTrackEnergy()/CLHEP::MeV <= dtaCutOff) )
  //{
  // the atomic weight of the target nucleus is >= 1.5            AND
  //   neither the incident nor the target particles have changed  AND
  //     there is no kinetic energy available for either proton/neutron
  //     or for deuteron/triton/alpha black track particles
  // For diffraction scattering on heavy nuclei use elastic routines instead
  //G4cerr << "*** Error in G4InelasticInteraction::CalculateMomenta" << G4endl;
  //G4cerr << "*** the elastic scattering would be better here ***" <<G4endl;
  //}
  theReactionDynamics.TwoBody( vec, vecLen,
                               modifiedOriginal, originalTarget,
                               currentParticle, targetParticle,
                               targetNucleus, targetHasChanged );
}


G4bool FullModelHadronicProcess::MarkLeadingStrangeParticle(
                                                            const G4ReactionProduct &currentParticle,
                                                            const G4ReactionProduct &targetParticle,
                                                            G4ReactionProduct &leadParticle )
{
  // the following was in GenerateXandPt and TwoCluster
  // add a parameter to the GenerateXandPt function telling it about the strange particle
  //
  // assumes that the original particle was a strange particle
  //
  G4bool lead = false;
  if( (currentParticle.GetMass() >= G4KaonPlus::KaonPlus()->GetPDGMass()) &&
      (currentParticle.GetDefinition() != G4Proton::Proton()) &&
      (currentParticle.GetDefinition() != G4Neutron::Neutron()) )
    {
      lead = true;
      leadParticle = currentParticle;              //  set lead to the incident particle
    }
  else if( (targetParticle.GetMass() >= G4KaonPlus::KaonPlus()->GetPDGMass()) &&
           (targetParticle.GetDefinition() != G4Proton::Proton()) &&
           (targetParticle.GetDefinition() != G4Neutron::Neutron()) )
    {
      lead = true;
      leadParticle = targetParticle;              //   set lead to the target particle
    }
  return lead;
}

void FullModelHadronicProcess::Rotate(G4FastVector<G4ReactionProduct,GHADLISTSIZE> &vec, G4int &vecLen)
{
  G4double rotation = 2.*CLHEP::pi*G4UniformRand();
  m_cache = rotation;
  G4int i;
  for( i=0; i<vecLen; ++i )
    {
      G4ThreeVector momentum = vec[i]->GetMomentum();
      momentum = momentum.rotate(rotation, m_what);
      vec[i]->SetMomentum(momentum);
    }
}

const G4DynamicParticle* FullModelHadronicProcess::FindRhadron(G4ParticleChange* aParticleChange)
{
  G4int nsec = aParticleChange->GetNumberOfSecondaries();
  if (nsec==0) return 0;
  int i = 0;
  G4bool found = false;
  while (i!=nsec && !found){
    //    G4cout<<"Checking "<<aParticleChange->GetSecondary(i)->GetDynamicParticle()->GetDefinition()->GetParticleName()<<G4endl;
    //    if (aParticleChange->GetSecondary(i)->GetDynamicParticle()->GetDefinition()->GetParticleType()=="rhadron") found = true;
    if (dynamic_cast<CustomParticle*>(aParticleChange->GetSecondary(i)->GetDynamicParticle()->GetDefinition())!=0) found = true;
    i++;
  }
  i--;
  if(found) return aParticleChange->GetSecondary(i)->GetDynamicParticle();
  return 0;
}
