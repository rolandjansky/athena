/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4ProcessHelper.hh"
#include "CustomParticle.h"
#include "CustomPDGParser.h"
#include "G4ParticleTable.hh"
#include "G4DecayTable.hh"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

G4ProcessHelper::G4ProcessHelper()
  : theTarget(0)
  , theRmesoncloud(0)
  , theRbaryoncloud(0)
  , theReactionMap(0)
{
  particleTable = G4ParticleTable::GetParticleTable();
  theProton = particleTable->FindParticle("proton");
  theNeutron = particleTable->FindParticle("neutron");

  G4String line;

  // I opted for string based read-in, as it would make physics debugging easier later on

  std::ifstream process_stream ("ProcessList.txt");
  while(getline(process_stream,line)){
    std::vector<G4String> tokens;

    //Getting a line
    ReadAndParse(line,tokens,"#");

    //Important info
    G4String incident = tokens[0];
    G4cout<<"Incident: "<<incident<<G4endl;
    G4ParticleDefinition* incidentDef = particleTable->FindParticle(incident);
    G4int incidentPDG = incidentDef->GetPDGEncoding();
    known_particles[incidentDef]=true;

    G4String target = tokens[1];
    //    G4cout<<"Target: "<<target<<G4endl;

    // Making a ReactionProduct
    ReactionProduct prod;
    for (unsigned int i = 2; i != tokens.size();i++){
      G4String part = tokens[i];
      if (particleTable->contains(part))
        {
          prod.push_back(particleTable->FindParticle(part)->GetPDGEncoding());
        } else {
        G4cout<<"Particle: "<<part<<" is unknown."<<G4endl;
        G4Exception("G4ProcessHelper", "UnkownParticle", FatalException,
                    "Initialization: The reaction product list contained an unknown particle");
      }
    }
    if (target == "proton"){
      pReactionMap[incidentPDG].push_back(prod);
    } else if (target == "neutron") {
      nReactionMap[incidentPDG].push_back(prod);
    } else {
      G4Exception("G4ProcessHelper", "IllegalTarget", FatalException,
                  "Initialization: The reaction product list contained an illegal target particle");
    }
  }

  process_stream.close();

  parameters["Resonant"]=0.;
  parameters["ResonanceEnergy"]=0.;
  parameters["XsecMultiplier"]=1.;
  parameters["Gamma"]=0.;
  parameters["Amplitude"]=0.;
  parameters["ReggeSuppression"]=0.;
  parameters["HadronLifeTime"]=0.;
  parameters["ReggeModel"]=0.;
  parameters["Mixing"]=0.;
  parameters["DoDecays"]=0;

  std::ifstream physics_stream ("PhysicsConfiguration.txt");
  char** endptr=0;
  while(getline(physics_stream,line))
    {
      std::vector<G4String> tokens;
      //Getting a line
      ReadAndParse(line,tokens,"=");
      G4String key = tokens[0];
      G4double val = strtod(tokens[1],endptr);
      parameters[key]=val;
    }
  physics_stream.close();

  resonant = false;
  reggemodel = false;
  if(parameters["Resonant"]!=0.) resonant=true;
  ek_0 = parameters["ResonanceEnergy"]*CLHEP::GeV;
  gamma = parameters["Gamma"]*CLHEP::GeV;
  amplitude = parameters["Amplitude"]*CLHEP::millibarn;
  xsecmultiplier = parameters["XsecMultiplier"];
  suppressionfactor = parameters["ReggeSuppression"];
  hadronlifetime = parameters["HadronLifeTime"];
  mixing = parameters["Mixing"];
  if(parameters["ReggeModel"]!=0.) reggemodel=true;
  doDecays=parameters["DoDecays"];

  G4cout<<"Read in physics parameters:"<<G4endl;
  G4cout<<"Resonant = "<< resonant <<G4endl;
  G4cout<<"ResonanceEnergy = "<<ek_0/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout<<"XsecMultiplier = "<<xsecmultiplier<<G4endl;
  G4cout<<"Gamma = "<<gamma/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout<<"Amplitude = "<<amplitude/CLHEP::millibarn<<" millibarn"<<G4endl;
  G4cout<<"ReggeSuppression = "<<100*suppressionfactor<<" %"<<G4endl;
  G4cout<<"HadronLifeTime = "<<hadronlifetime; if (doDecays) G4cout<<" ns"<<G4endl; else G4cout<<" s"<<G4endl;
  G4cout<<"ReggeModel = "<< reggemodel <<G4endl;
  G4cout<<"Mixing = "<< mixing*100 <<" %"<<G4endl;
  G4cout<<"DoDecays = "<< doDecays << G4endl;

  checkfraction = 0;
  n_22 = 0;
  n_23 = 0;

  G4ParticleTable::G4PTblDicIterator* theParticleIterator;
  theParticleIterator = particleTable->GetIterator();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    CustomParticle* particle = dynamic_cast<CustomParticle*>(theParticleIterator->value());
    std::string name = theParticleIterator->value()->GetParticleName();
    G4DecayTable* table = theParticleIterator->value()->GetDecayTable();
    if(particle!=0&&table!=0&&name.find("cloud")>name.size()) {
      particle->SetPDGLifeTime(hadronlifetime*CLHEP::s);
      particle->SetPDGStable(false);
      G4cout<<"Lifetime of: "<<name<<" set to: "<<particle->GetPDGLifeTime()/CLHEP::s<<" s."<<G4endl;
      G4cout<<"Stable: "<<particle->GetPDGStable()<<G4endl;
    }
    if (particle && doDecays==1){
      // Make them decay immediately!!
      particle->SetPDGStable(false);
      particle->SetPDGLifeTime(hadronlifetime*CLHEP::ns);
      G4cout<<"Forcing a decay for "<<name<<G4endl;
      G4cout<<"Lifetime of: "<<name<<" set to: "<<particle->GetPDGLifeTime()/CLHEP::ns<<" ns."<<G4endl;
      G4cout<<"Stable: "<<particle->GetPDGStable()<<G4endl;
    }

    G4cout << "Done with particle " << name << G4endl;
  }
  theParticleIterator->reset();

  return;
}

G4ProcessHelper* G4ProcessHelper::pinstance = 0;

G4ProcessHelper* G4ProcessHelper::Instance()
{
  if (pinstance == 0)
    {
      pinstance = new G4ProcessHelper();
    }
  return pinstance;
}

G4bool G4ProcessHelper::ApplicabilityTester(const G4ParticleDefinition& aPart){
  const G4ParticleDefinition* aP = &aPart;
  if (known_particles[aP]) return true;
  return false;
}

G4double G4ProcessHelper::GetInclusiveCrossSection(const G4DynamicParticle *aParticle,
                                                   const G4Element *anElement){

  //We really do need a dedicated class to handle the cross sections. They might not always be constant


  //Disassemble the PDG-code

  G4int thePDGCode = aParticle->GetDefinition()->GetPDGEncoding();
  double boost = (aParticle->GetKineticEnergy()+aParticle->GetMass())/aParticle->GetMass();
  //  G4cout<<"thePDGCode: "<<thePDGCode<<G4endl;
  G4double theXsec = 0;
  G4String name = aParticle->GetDefinition()->GetParticleName();

  if(!reggemodel){
      //Flat cross section
      if(CustomPDGParser::s_isRGlueball(thePDGCode)) {
        theXsec = 24 * CLHEP::millibarn;
      } else {
        std::vector<G4int> nq=CustomPDGParser::s_containedQuarks(thePDGCode);
        for (std::vector<G4int>::iterator it = nq.begin();
             it != nq.end();
             it++)
          {
            // 12 mb taken from asymptotic pion-nucleon scattering cross sections
            if (*it == 1 || *it == 2) theXsec += 12 * CLHEP::millibarn;
            // 6 mb taken from asymptotic kaon-nucleon scattering cross sections
            // No data for D or B, so setting to behave like a kaon
            if (*it == 3 || *it == 4 || *it == 5) theXsec += 6 * CLHEP::millibarn;
          }
      }
  } else {
    // From Eur. Phys. J. C (2010) 66: 493-501
    // DOI 10.1140/epjc/s10052-010-1262-1
    double R = Regge(boost);
    double P = Pom(boost);
    if(thePDGCode>0)
      {
        if(CustomPDGParser::s_isMesonino(thePDGCode)) theXsec=(P+R)*CLHEP::millibarn;
        if(CustomPDGParser::s_isSbaryon(thePDGCode)) theXsec=2*P*CLHEP::millibarn;
        if(CustomPDGParser::s_isRMeson(thePDGCode)||CustomPDGParser::s_isRGlueball(thePDGCode)) theXsec=(R+2*P)*CLHEP::millibarn;
        if(CustomPDGParser::s_isRBaryon(thePDGCode)) theXsec=3*P*CLHEP::millibarn;
      }
    else
      {
        if(CustomPDGParser::s_isMesonino(thePDGCode)) theXsec=P*CLHEP::millibarn;
        if(CustomPDGParser::s_isSbaryon(thePDGCode)) theXsec=(2*(P+R)+30/sqrt(boost))*CLHEP::millibarn;
        if(CustomPDGParser::s_isRMeson(thePDGCode)||CustomPDGParser::s_isRGlueball(thePDGCode)) theXsec=(R+2*P)*CLHEP::millibarn;
        if(CustomPDGParser::s_isRBaryon(thePDGCode)) theXsec=3*P*CLHEP::millibarn;
      }
  }



  //Adding resonance

  if(resonant)
    {
    // Described in Section 5.1 of http://r-hadrons.web.cern.ch/r-hadrons/download/mackeprang_thesis.pdf
    // mentioned but dismissed in Section 3.3 of https://arxiv.org/pdf/hep-ex/0404001.pdf
      double e_0 = ek_0 + aParticle->GetDefinition()->GetPDGMass(); //Now total energy

      e_0 = sqrt(aParticle->GetDefinition()->GetPDGMass()*aParticle->GetDefinition()->GetPDGMass()
                 + theProton->GetPDGMass()*theProton->GetPDGMass()
                 + 2.*e_0*theProton->GetPDGMass());
      double sqrts=sqrt(aParticle->GetDefinition()->GetPDGMass()*aParticle->GetDefinition()->GetPDGMass()
                        + theProton->GetPDGMass()*theProton->GetPDGMass() + 2*aParticle->GetTotalEnergy()*theProton->GetPDGMass());

      double res_result = amplitude*(gamma*gamma/4.)/((sqrts-e_0)*(sqrts-e_0)+(gamma*gamma/4.));//Non-relativistic Breit Wigner

      theXsec += res_result;
    }


  return theXsec * pow(anElement->GetN(),0.7)*1.25 * xsecmultiplier;// * 0.523598775598299;

}

ReactionProduct G4ProcessHelper::GetFinalState(const G4Track& aTrack, G4ParticleDefinition*& aTarget){

  const G4DynamicParticle* aDynamicParticle = aTrack.GetDynamicParticle();

  //-----------------------------------------------
  // Choose n / p as target
  // and get ReactionProductList pointer
  //-----------------------------------------------

  G4Material* aMaterial = aTrack.GetMaterial();
  const G4ElementVector* theElementVector = aMaterial->GetElementVector() ;
  const G4double* NbOfAtomsPerVolume = aMaterial->GetVecNbOfAtomsPerVolume();

  G4double NumberOfProtons=0;
  G4double NumberOfNucleons=0;

  for ( size_t elm=0 ; elm < aMaterial->GetNumberOfElements() ; elm++ )
    {
      //Summing number of protons per unit volume
      NumberOfProtons += NbOfAtomsPerVolume[elm]*(*theElementVector)[elm]->GetZ();
      //Summing nucleons (not neutrons)
      NumberOfNucleons += NbOfAtomsPerVolume[elm]*(*theElementVector)[elm]->GetN();
    }

  if(CLHEP::RandFlat::shoot()<NumberOfProtons/NumberOfNucleons){
      theReactionMap = &pReactionMap;
      theTarget = theProton;
  } else {
    theReactionMap = &nReactionMap;
    theTarget = theNeutron;
  }

  aTarget = theTarget;

  G4int theIncidentPDG = aDynamicParticle->GetDefinition()->GetPDGEncoding();

  if(reggemodel
     &&CustomPDGParser::s_isMesonino(theIncidentPDG)
     &&CLHEP::RandFlat::shoot()*mixing>0.5
     &&aDynamicParticle->GetDefinition()->GetPDGCharge()==0.
     )
    {
      //      G4cout<<"Oscillating..."<<G4endl;
      theIncidentPDG *= -1;
    }


  bool baryonise=false;

  if(reggemodel
     &&CLHEP::RandFlat::shoot()>0.9
     &&(
        (CustomPDGParser::s_isMesonino(theIncidentPDG)&&theIncidentPDG>0)
        ||
        CustomPDGParser::s_isRMeson(theIncidentPDG)
        )
     )
    baryonise=true;


  //Making a pointer directly to the ReactionProductList we are looking at. Makes life easier :-)
  ReactionProductList*  aReactionProductList = &((*theReactionMap)[theIncidentPDG]);

  //-----------------------------------------------
  // Count processes
  // kinematic check
  // compute number of 2 -> 2 and 2 -> 3 processes
  //-----------------------------------------------

  G4int N22 = 0; //Number of 2 -> 2 processes
  G4int N23 = 0; //Number of 2 -> 3 processes. Couldn't think of more informative names

  //This is the list to be populated
  ReactionProductList theReactionProductList;
  std::vector<bool> theChargeChangeList;

  for (ReactionProductList::iterator prod_it = aReactionProductList->begin();
       prod_it != aReactionProductList->end();
       prod_it++){
    G4int secondaries = prod_it->size();
    // If the reaction is not possible we will not consider it
    /*    if(ReactionIsPossible(*prod_it,aDynamicParticle)
          &&(
          !baryonise||(baryonise&&ReactionGivesBaryon(*prod_it))
          ))*/
    if(ReactionIsPossible(*prod_it,aDynamicParticle)
       &&(
          (baryonise&&ReactionGivesBaryon(*prod_it))
          ||
          (!baryonise&&!ReactionGivesBaryon(*prod_it))
          ||
          (CustomPDGParser::s_isSbaryon(theIncidentPDG))
          ||
          (CustomPDGParser::s_isRBaryon(theIncidentPDG))
          ||!reggemodel
          )
       )
      {
        // The reaction is possible. Let's store and count it
        theReactionProductList.push_back(*prod_it);
        if (secondaries == 2){
          N22++;
        } else if (secondaries ==3) {
          N23++;
        } else {
          G4cerr << "ReactionProduct has unsupported number of secondaries: "<<secondaries<<G4endl;
        }
      } /*else {
          G4cout<<"There was an impossible process"<<G4endl;
          }*/
  }
  //  G4cout<<"The size of the ReactionProductList is: "<<theReactionProductList.size()<<G4endl;

  if (theReactionProductList.size()==0) G4Exception("G4ProcessHelper", "NoProcessPossible", FatalException,
                                                    "GetFinalState: No process could be selected from the given list.");

  // For the Regge model no phase space considerations. We pick a process at random
  if(reggemodel)
    {
      int n_rps = theReactionProductList.size();
      int select = (int)(CLHEP::RandFlat::shoot()*n_rps);
      //      G4cout<<"Possible: "<<n_rps<<", chosen: "<<select<<G4endl;
      return theReactionProductList[select];
    }

  // Fill a probability map. Remember total probability
  // 2->2 is 0.15*1/n_22 2->3 uses phase space
  G4double p22 = 0.15;
  G4double p23 = 1-p22; // :-)

  std::vector<G4double> Probabilities;
  std::vector<G4bool> TwotoThreeFlag;

  G4double CumulatedProbability = 0;

  // To each ReactionProduct we assign a cumulated probability and a flag
  // discerning between 2 -> 2 and 2 -> 3
  for (unsigned int i = 0; i != theReactionProductList.size(); i++){
    if (theReactionProductList[i].size() == 2) {
      if(0==N22) { throw std::runtime_error("G4ProcessHelper::GetFinalState: N22 is zero!"); }
      CumulatedProbability += p22/N22;
      TwotoThreeFlag.push_back(false);
    } else {
      if(0==N23) { throw std::runtime_error("G4ProcessHelper::GetFinalState: N23 is zero!"); }
      CumulatedProbability += p23/N23;
      TwotoThreeFlag.push_back(true);
    }
    Probabilities.push_back(CumulatedProbability);
    //    G4cout<<"Pushing back cumulated probability: "<<CumulatedProbability<<G4endl;
  }

  //Renormalising probabilities
  //  G4cout<<"Probs: ";
  for (std::vector<G4double>::iterator it = Probabilities.begin();
       it != Probabilities.end();
       it++)
    {
      *it /= CumulatedProbability;
      //      G4cout<<*it<<" ";
    }
  //  G4cout<<G4endl;

  // Choosing ReactionProduct

  G4bool selected = false;
  G4int tries = 0;
  //  ReactionProductList::iterator prod_it;

  //Keep looping over the list until we have a choice, or until we have tried 100 times
  unsigned int i=0;
  while(!selected && tries < 100){
    i=0;
    G4double dice = CLHEP::RandFlat::shoot();
    //    G4cout<<"What's the dice?"<<dice<<G4endl;
    while(dice>Probabilities[i] && i<theReactionProductList.size()){
      //      G4cout<<"i: "<<i<<G4endl;
      i++;
    }

    //    G4cout<<"Chosen i: "<<i<<G4endl;

    if(!TwotoThreeFlag[i]) {
      // 2 -> 2 processes are chosen immediately
      selected = true;
    } else {
      // 2 -> 3 processes require a phase space lookup
      if (PhaseSpace(theReactionProductList[i],aDynamicParticle)>CLHEP::RandFlat::shoot()) selected = true;
    }
    //    double suppressionfactor=0.5;
    if(selected&&particleTable->FindParticle(theReactionProductList[i][0])->GetPDGCharge()!=aDynamicParticle->GetDefinition()->GetPDGCharge())
      {
        /*
          G4cout<<"Incoming particle "<<aDynamicParticle->GetDefinition()->GetParticleName()
          <<" has charge "<<aDynamicParticle->GetDefinition()->GetPDGCharge()<<G4endl;
          G4cout<<"Suggested particle "<<particleTable->FindParticle(theReactionProductList[i][0])->GetParticleName()
          <<" has charge "<<particleTable->FindParticle(theReactionProductList[i][0])->GetPDGCharge()<<G4endl;
        */
        if(CLHEP::RandFlat::shoot()<suppressionfactor) selected = false;
      }
    tries++;
    //    G4cout<<"Tries: "<<tries<<G4endl;
  }
  if(tries>=100) G4cerr<<"Could not select process!!!!"<<G4endl;

  //  G4cout<<"So far so good"<<G4endl;
  //  G4cout<<"Sec's: "<<theReactionProductList[i].size()<<G4endl;

  //Updating checkfraction:
  if (theReactionProductList[i].size()==2) {
    n_22++;
  } else {
    n_23++;
  }

  checkfraction = (1.0*n_22)/(n_22+n_23);
  //  G4cout<<"n_22: "<<n_22<<" n_23: "<<n_23<<" Checkfraction: "<<checkfraction<<G4endl;
  //  G4cout <<"Biig number: "<<n_22+n_23<<G4endl;
  //Return the chosen ReactionProduct
  return theReactionProductList[i];
}

G4double G4ProcessHelper::ReactionProductMass(const ReactionProduct& aReaction,const G4DynamicParticle* aDynamicParticle) const {
  // Incident energy:
  G4double E_incident = aDynamicParticle->GetTotalEnergy();
  //G4cout<<"Total energy: "<<E_incident<<" Kinetic: "<<aDynamicParticle->GetKineticEnergy()<<G4endl;
  // sqrt(s)= sqrt(m_1^2 + m_2^2 + 2 E_1 m_2)
  G4double m_1 = aDynamicParticle->GetDefinition()->GetPDGMass();
  G4double m_2 = theTarget->GetPDGMass();
  //G4cout<<"M_R: "<<m_1/CLHEP::GeV<<" GeV, M_np: "<<m_2/CLHEP::GeV<<" GeV"<<G4endl;
  G4double sqrts = sqrt(m_1*m_1 + m_2*(m_2 + 2 * E_incident));
  //G4cout<<"sqrt(s) = "<<sqrts/CLHEP::GeV<<" GeV"<<G4endl;
  // Sum of rest masses after reaction:
  G4double M_after = 0;
  for (ReactionProduct::const_iterator r_it = aReaction.begin(); r_it !=aReaction.end(); r_it++){
    //G4cout<<"Mass contrib: "<<(particleTable->FindParticle(*r_it)->GetPDGMass())/CLHEP::MeV<<" MeV"<<G4endl;
    M_after += particleTable->FindParticle(*r_it)->GetPDGMass();
  }
  //G4cout<<"Intending to return this ReactionProductMass: "<<(sqrts - M_after)/CLHEP::MeV<<" MeV"<<G4endl;
  return sqrts - M_after;
}

G4bool G4ProcessHelper::ReactionIsPossible(const ReactionProduct& aReaction,const G4DynamicParticle* aDynamicParticle) const{
  if (ReactionProductMass(aReaction,aDynamicParticle)>0) return true;
  return false;
}

G4bool G4ProcessHelper::ReactionGivesBaryon(const ReactionProduct& aReaction) const{
  for (ReactionProduct::const_iterator it = aReaction.begin();it!=aReaction.end();it++)
    if(CustomPDGParser::s_isSbaryon(*it)||CustomPDGParser::s_isRBaryon(*it)) return true;
  return false;
}

G4double G4ProcessHelper::PhaseSpace(const ReactionProduct& aReaction,const G4DynamicParticle* aDynamicParticle) const{
  G4double qValue = ReactionProductMass(aReaction,aDynamicParticle);
  // Eq 4 of https://arxiv.org/pdf/hep-ex/0404001.pdf
  G4double phi = sqrt(1+qValue/(2*0.139*CLHEP::GeV))*pow(qValue/(1.1*CLHEP::GeV),3./2.);
  return (phi/(1+phi));
}

void G4ProcessHelper::ReadAndParse(const G4String& str,
                                   std::vector<G4String>& tokens,
                                   const G4String& delimiters) const
{
  // Skip delimiters at beginning.
  G4String::size_type lastPos = str.find_first_not_of(delimiters, 0);
  if (lastPos==G4String::npos) return;

  // Find first "non-delimiter".
  G4String::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (G4String::npos != pos || G4String::npos != lastPos)
    {
      //Skipping leading / trailing whitespaces
      G4String temp = str.substr(lastPos, pos - lastPos);
      while(temp.c_str()[0] == ' ') temp.erase(0,1);
      while(temp[temp.size()-1] == ' ') temp.erase(temp.size()-1,1);

      // Found a token, add it to the vector.
      tokens.push_back(temp);

      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      if (lastPos==G4String::npos) continue;

      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}

double G4ProcessHelper::Regge(const double boost) const
{
  // https://link.springer.com/content/pdf/10.1140%2Fepjc%2Fs10052-010-1262-1.pdf Eq 1
  // Originally from https://arxiv.org/pdf/0710.3930.pdf
  double a=2.165635078566177;
  double b=0.1467453738547229;
  double c=-0.9607903711871166;
  return 1.5*exp(a+b/boost+c*log(boost));
}


double G4ProcessHelper::Pom(const double boost) const
{
  // https://link.springer.com/content/pdf/10.1140%2Fepjc%2Fs10052-010-1262-1.pdf Eq 2
  // Originally from https://arxiv.org/pdf/0710.3930.pdf
  double a=4.138224000651535;
  double b=1.50377557581421;
  double c=-0.05449742257808247;
  double d=0.0008221235048211401;
  return a + b*sqrt(boost) + c*boost + d*pow(boost,1.5);
}
