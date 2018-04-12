#include "G4PhysicsLists/AtlasPhysListFactory.h"

//PhysicsList
#include "QGSP_BERT_MuBias.hh"
#include "FTFP_BERT_ATL.hh"
// Hadronic Physics Variations
#include "FTFP_BERT_ATL_EMY.hh"
#include "FTFP_BERT_ATL_chipsXS.hh"
#include "FTFP_BERT_ATL_noDiffraction.hh"
#include "QGSP_FTFP_BERT_ATL.hh"
#include "FTFP_BERT_BIC_ATL.hh"
#include "FTFP_BERT_ATL_HP.hh"
#include "FTFP_BERT_ATL_newElastic.hh"
#include "FTFP_BERT_ATL_rescattering.hh"
#include "FTFP_BERP_ATL.hh"
#include "QGSP_BIC_HP.hh"
#include "G4EmStandardPhysics_option3.hh"

//#include "QGSP_BERT_95NoMsc.hh"
//#include "ElVariations/QGSP_BERT_Msc95.hh"
//#include "ElVariations/QGSP_BERT_Brem95.hh"
//#include "ElVariations/QGSP_BERT_95.hh"
//#include "ElVariations/QGSP_BERT_Conv95.hh"
//#include "ElVariations/QGSP_BERT_Msc92.hh"

AtlasPhysListFactory::AtlasPhysListFactory() 
  : m_defName("FTFP_BERT_MuBias"),m_verbose(0)
{
  m_listnames = {
    "QGSP_BERT_MuBias",
    "FTFP_BERT_ATL", //(nominal)
    "QGSP_BIC_HP_EMY", //(most reasonable single-alternative option
    "FTFP_BERT_ATL_EMY", //(electromagnetic physics)
    "FTFP_BERT_ATL_newElastic", //(newer elastic cross-sections and final-state models)
    "FTFP_BERT_ATL_noDiffraction", //(both projectile and target direction are switched off)
    "QGSP_FTFP_BERT_ATL", //(QGS model used above 25 GeV, transition betwen FTF and QGS between 12 and 25 GeV)
    "FTFP_BERT_BIC_ATL", //(binary cascade model as much as possible, Bertini cascade model for the rest)
    "FTFP_BERT_ATL_rescattering", //(re-scattering of the final-state produced by FTF is simulated with the binary cascade model)
    "FTFP_BERP_ATL", // (office Geant4 Preco model is used instead of Bertini)
    "FTFP_BERT_ATL_chipsXS", //(different inelastic hadron-nucleus cros-sections)
    "FTFP_BERT_ATL_HP" //(transportation of neutrons below 20 MeV is more accurate)
  };
  m_nlists = m_listnames.size();
}

AtlasPhysListFactory::~AtlasPhysListFactory()
{}

G4VModularPhysicsList* 
AtlasPhysListFactory::ReferencePhysList()
{
  // instantiate PhysList by environment variable "PHYSLIST"
  G4String name = "";
  char* path = getenv("PHYSLIST");
  if (path) {
    name = G4String(path);
  } else {
    name = m_defName;
    G4cout << "### AtlasPhysListFactory WARNING: "
	   << " environment variable PHYSLIST is not defined"
	   << G4endl
	   << "    Default Physics Lists " << name 
	   << " is instantiated" 
	   << G4endl;
  }
  return GetReferencePhysList(name);
}

G4VModularPhysicsList* 
AtlasPhysListFactory::GetReferencePhysList(const G4String& name)
{
  if(0 <= m_verbose) {
    G4cout << "AtlasPhysListFactory::GetReferencePhysList <" << name
	   << G4endl;
  }
  G4VModularPhysicsList* p = 0;
  if(name == "QGSP_BERT_MuBias")           {p = new QGSP_BERT_MuBias(m_verbose);}
  else if (name=="FTFP_BERT_ATL")          {p = new FTFP_BERT_ATL(m_verbose);}
  else if (name=="FTFP_BERT_ATL_EMY")           {p = new FTFP_BERT_ATL_EMY(m_verbose);}
  else if (name=="FTFP_BERT_ATL_newElastic")    {p = new FTFP_BERT_ATL_newElastic(m_verbose);}
  else if (name=="FTFP_BERT_ATL_noDiffraction") {p = new FTFP_BERT_ATL_noDiffraction(m_verbose);}
  else if (name=="QGSP_FTFP_BERT_ATL")          {p = new QGSP_FTFP_BERT_ATL(m_verbose);}
  else if (name=="FTFP_BERT_BIC_ATL")           {p = new FTFP_BERT_BIC_ATL(m_verbose);}
  else if (name=="FTFP_BERT_ATL_rescattering")  {p = new FTFP_BERT_ATL_rescattering(m_verbose);}
  else if (name=="FTFP_BERP_ATL")               {p = new FTFP_BERP_ATL(m_verbose);}
  else if (name=="FTFP_BERT_ATL_chipsXS")       {p = new FTFP_BERT_ATL_chipsXS(m_verbose);}
  else if (name=="FTFP_BERT_ATL_HP")            {p = new FTFP_BERT_ATL_HP(m_verbose);}
  else if (name=="QGSP_BIC_HP_EMY")             {p = new QGSP_BIC_HP(m_verbose); p->ReplacePhysics(new G4EmStandardPhysics_option3(m_verbose));}
  else {
    G4cout << "### AtlasPhysListFactory WARNING: "
	   << "PhysicsList " << name << " is not known"
	   << G4endl;
  }
  if(p) {
    G4cout << "<<< Reference Physics List " << name
	    << " is built" << G4endl;
    p->SetVerboseLevel(0);
  }
  G4cout << G4endl;
  return p;
}
  
G4bool AtlasPhysListFactory::IsReferencePhysList(const G4String& name)
{
  G4bool res = false;

  for(size_t i=0; i<m_nlists; ++i) {
    if(name == m_listnames[i]) {
      res = true;
      break;
    }
  }
  return res;
}

const std::vector<G4String>& 
AtlasPhysListFactory::AvailablePhysLists() const
{
  return m_listnames;
}



