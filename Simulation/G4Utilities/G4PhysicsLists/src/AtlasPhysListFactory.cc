#include "G4PhysicsLists/AtlasPhysListFactory.h"

//PhysicsList
#include "QGSP_BERT_MuBias.hh"
// Hadronic Physics Variations
#include "FTFP_BERT_ATL_chipsXS.hh"
#include "FTFP_BERT_ATL_noDiffraction.hh"
#include "QGSP_FTFP_BERT_ATL.hh"
#include "FTFP_BERT_BIC_ATL.hh"
#include "FTFP_BERT_ATL_HP.hh"
#include "FTFP_BERT_ATL_newElastic.hh"
#include "FTFP_BERT_ATL_rescattering.hh"
#include "FTFP_BERP_ATL.hh"

#include <algorithm>

AtlasPhysListFactory::AtlasPhysListFactory() 
  : m_defName("FTFP_BERT_MuBias"),m_verbose(0)
{
  m_listnames = {
    "QGSP_BERT_MuBias",
    "FTFP_BERT_ATL_newElastic", //(newer elastic cross-sections and final-state models)
    "FTFP_BERT_ATL_noDiffraction", //(both projectile and target direction are switched off)
    "QGSP_FTFP_BERT_ATL", //(QGS model used above 25 GeV, transition betwen FTF and QGS between 12 and 25 GeV)
    "FTFP_BERT_BIC_ATL", //(binary cascade model as much as possible, Bertini cascade model for the rest)
    "FTFP_BERT_ATL_rescattering", //(re-scattering of the final-state produced by FTF is simulated with the binary cascade model)
    "FTFP_BERP_ATL", // (office Geant4 Preco model is used instead of Bertini)
    "FTFP_BERT_ATL_chipsXS", //(different inelastic hadron-nucleus cros-sections)
    "FTFP_BERT_ATL_HP" //(transportation of neutrons below 20 MeV is more accurate)
  };
}

G4VModularPhysicsList* 
AtlasPhysListFactory::ReferencePhysList()
{
  // instantiate PhysList by environment variable "PHYSLIST"
  G4String name = "";
  char* path = getenv("PHYSLIST");
  if (path != nullptr) {
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
  G4VModularPhysicsList* p = nullptr;
  if(name == "QGSP_BERT_MuBias")           {p = new QGSP_BERT_MuBias(m_verbose);}
  else if (name=="FTFP_BERT_ATL_newElastic")    {p = new FTFP_BERT_ATL_newElastic(m_verbose);}
  else if (name=="FTFP_BERT_ATL_noDiffraction") {p = new FTFP_BERT_ATL_noDiffraction(m_verbose);}
  else if (name=="QGSP_FTFP_BERT_ATL")          {p = new QGSP_FTFP_BERT_ATL(m_verbose);}
  else if (name=="FTFP_BERT_BIC_ATL")           {p = new FTFP_BERT_BIC_ATL(m_verbose);}
  else if (name=="FTFP_BERT_ATL_rescattering")  {p = new FTFP_BERT_ATL_rescattering(m_verbose);}
  else if (name=="FTFP_BERP_ATL")               {p = new FTFP_BERP_ATL(m_verbose);}
  else if (name=="FTFP_BERT_ATL_chipsXS")       {p = new FTFP_BERT_ATL_chipsXS(m_verbose);}
  else if (name=="FTFP_BERT_ATL_HP")            {p = new FTFP_BERT_ATL_HP(m_verbose);}
  else {
    G4cout << "### AtlasPhysListFactory WARNING: "
	   << "PhysicsList " << name << " is not known"
	   << G4endl;
  }
  if(p != nullptr) {
    G4cout << "<<< ATLAS Reference Physics List " << name
	    << " is built" << G4endl;
    p->SetVerboseLevel(0);
  }
  G4cout << G4endl;
  return p;
}
  
G4bool AtlasPhysListFactory::IsReferencePhysList(const G4String& name)
{
  return std::find(m_listnames.begin(), m_listnames.end(), name) != m_listnames.end();
}

const std::vector<G4String>& 
AtlasPhysListFactory::AvailablePhysLists() const
{
  return m_listnames;
}

