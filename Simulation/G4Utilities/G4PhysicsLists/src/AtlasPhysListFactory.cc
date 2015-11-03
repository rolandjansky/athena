#include "G4PhysicsLists/AtlasPhysListFactory.h"

//PhysicsList
#include "QGSP_BERT_MuBias.hh"
//#include "QGSP_BERT_95NoMsc.hh"
//#include "ElVariations/QGSP_BERT_Msc95.hh"
//#include "ElVariations/QGSP_BERT_Brem95.hh"
//#include "ElVariations/QGSP_BERT_95.hh"
//#include "ElVariations/QGSP_BERT_Conv95.hh"
//#include "ElVariations/QGSP_BERT_Msc92.hh"

AtlasPhysListFactory::AtlasPhysListFactory() 
  : defName("QGSP_BERT_MuBias"),verbose(1)
{
  nlists = 1;
  G4String ss[1] = {
    "QGSP_BERT_MuBias"};
  for(size_t i=0; i<nlists; ++i) {
    listnames.push_back(ss[i]);
  }
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
    name = defName;
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
  if(0 <++  verbose) {
    G4cout << "AtlasPhysListFactory::GetReferencePhysList <" << name
	   << G4endl;
  }
  G4VModularPhysicsList* p = 0;
  if(name == "QGSP_BERT_MuBias")           {p = new QGSP_BERT_MuBias(verbose);}
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

  for(size_t i=0; i<nlists; ++i) {
    if(name == listnames[i]) {
      res = true;
      break;
    }
  }
  return res;
}

const std::vector<G4String>& 
AtlasPhysListFactory::AvailablePhysLists() const
{
  return listnames;
}



