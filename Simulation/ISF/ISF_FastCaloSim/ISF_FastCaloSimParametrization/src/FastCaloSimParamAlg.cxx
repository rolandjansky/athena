/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ISF_FastCaloSimParametrization/FastCaloSimParamAlg.h"
    
  /**
   *   
   *
   */

// STL include(s):
#include <sstream>
#include <map>

// local include(s):
//#include "LArG4Code/EnergySpot.h"
//#include "LArG4ShowerLib/Shower.h"
//#include "LArG4ShowerLib/ShowerLibList.h"

#include "ISF_FastCaloSimEvent/FCS_StepInfo.h"
#include "ISF_FastCaloSimEvent/FCS_StepInfoCollection.h"
 
// athena includes
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "EventInfo/TagInfo.h"


#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"


// For MC Truth information:
#include "GeneratorObjects/McEventCollection.h"

// geant includes
#include "G4Version.hh"
#include "TFile.h"


using CLHEP::Hep3Vector;


// definition for helper struct
/*
class Dist { 
public:
  Dist(ISF_FCS_Parametrization::FCS_StepInfo* _h1, ISF_FCS_Parametrization::FCS_StepInfo* _h2) : h1(_h1), h2(_h2) {}
  ISF_FCS_Parametrization::FCS_StepInfo* h1;
  ISF_FCS_Parametrization::FCS_StepInfo* h2;
  ISF_FCS_Parametrization::FCS_StepInfo* merge ()
	{
	  ISF_FCS_Parametrization::FCS_StepInfo* newStep = new ISF_FCS_Parametrization::FCS_StepInfo(*h1,*h2);
		h1->setValid(false);
		h2->setValid(false);
		return newStep;
	}
};

class  stepInfoDistCompare{
public:
      enum CompareType {RHO,R,Z};
      stepInfoDistCompare(const CompareType _type) : type(_type) {}
      bool operator()( const ISF_FCS_Parametrization::FCS_StepInfo * first,  const ISF_FCS_Parametrization::FCS_StepInfo * second) const {
            return sortFunction(first, second, type );
      }
private:
      CompareType type;
      //returns true if first is closer to the origin. this should sort from closest to farest
  bool sortFunction ( const ISF_FCS_Parametrization::FCS_StepInfo* first,  const ISF_FCS_Parametrization::FCS_StepInfo* second, const CompareType _type) const {
    	  if (_type == RHO) {
			  if (first->position().mag2() < second->position().mag2()) return true;
			  return false;
    	  } else if (_type == R) {
    		  if (first->position().perp2() < second->position().perp2()) return true;
    		  return false;
    	  } else if (_type == Z) {
    		  if (first->position().z() < second->position().z()) return true;
    		  return false;
    	  }
    	  return true; //it's here just to avoid warnings
      }
};
*/

struct SortByE{
  bool operator() (const ISF_FCS_Parametrization::FCS_StepInfo& step1, const ISF_FCS_Parametrization::FCS_StepInfo& step2) { return (step1.energy() > step2.energy()); }
  bool operator() (const ISF_FCS_Parametrization::FCS_StepInfo* step1, const ISF_FCS_Parametrization::FCS_StepInfo* step2) { return (step1->energy() > step2->energy()); }
};

FastCaloSimParamAlg::FastCaloSimParamAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator), m_calo_dd_man(0)
{
  declareProperty("Clusterize", m_clusterize = true, "merge nearby hits");
  declareProperty("Truncate", m_truncate = 2,"truncate hits with t>1000ns (if >=2)"); 
  declareProperty("MaxDistance",   m_maxDistance = 50000.,
		  "max distance squared after which the hits will be truncated");
  declareProperty("MinEnergy",   m_minEnergy = .99,
		  "energy border, that truncation won't cross");
  declareProperty("MaxRadius",        m_maxRadius = 25., 
		  "maximal radius squared until two hits will be combined");
  declareProperty("MaxRadiusLAr",        m_maxRadiusLAr = 25.,
                  "maximal radius in LAr squared until two hits will be combined");
  declareProperty("MaxRadiusHEC",        m_maxRadiusHEC = 100.,
                  "maximal radius in HEC squared until two hits will be combined");
  declareProperty("MaxRadiusFCAL",        m_maxRadiusFCAL = 100.,
                  "maximal radius in FCAL squared until two hits will be combined");
  declareProperty("MaxRadiusTile",        m_maxRadiusTile = 100.,
                  "maximal radius in Tile squared until two hits will be combined");

  declareProperty("MaxTime", m_maxTime = 25., "max time difference to merge two hits (ns) ");
  declareProperty("MaxTimeLAr", m_maxTimeLAr = 25., "max time difference to merge two hits (ns) ");
  declareProperty("MaxTimeHEC", m_maxTimeHEC = 25., "max time difference to merge two hits (ns) ");
  declareProperty("MaxTimeFCAL", m_maxTimeFCAL = 25., "max time difference to merge two hits (ns) ");
  declareProperty("MaxTimeTile", m_maxTimeTile = 25., "max time difference to merge two hits (ns) ");


  declareProperty("ContainmentEnergy",        m_containmentEnergy = 0.95,
		  "energy fraction that will be inside containment borders");
  declareProperty("LibStructFiles",   m_lib_struct_files,
		  "List of files to read library structures from");
  declareProperty("EnergyFraction",   m_energyFraction = .02,
		  "the allowed amount of energy that can be deposited outside calorimeter region ");
}


StatusCode FastCaloSimParamAlg::initialize()
{
  StatusCode sc;

  msg(MSG::INFO) << "Initializing " << name()
      << " - package version " << PACKAGE_VERSION << endmsg;

  //
  // Connect to StoreGate:
  //
  sc = evtStore().retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't connect to StoreGateSvc" << endmsg;
    return sc;
  }

  //
  // connect to detector store for conditions
  //
  sc = detStore().retrieve();
  if ( sc.isFailure() )  {
    msg(MSG::ERROR) << "Could not find detector store!" << endmsg;
    return sc;
  }

  msg(MSG::INFO) << "FastCaloSimParamAlg " << this->name() << " initialized" << endmsg;
  m_calo_dd_man  = CaloDetDescrManager::instance(); 


  return StatusCode::SUCCESS;
}

StatusCode FastCaloSimParamAlg::execute()
{
  const ISF_FCS_Parametrization::FCS_StepInfoCollection* eventStepsES = getFCS_StepInfo();
  if (!eventStepsES) eventStepsES = new ISF_FCS_Parametrization::FCS_StepInfoCollection(); //create empty one if it doesn't exist!
  std::cout <<"ZH in FastCaloSimParamAlg execute: eventStep size: "<<eventStepsES->size()<<std::endl;

  const HepMC::GenParticle* theParticle = getParticleFromMC();
  if (theParticle == NULL) {
    msg(MSG::ERROR) << "Couldn't get truth particle" << endmsg;
    return StatusCode::FAILURE;
  }
  else
    {
      std::cout <<"ZH Particle: "<<theParticle->pdg_id()<<" rapidity: "<<theParticle->momentum().eta()<<" phi: "<<theParticle->momentum().phi()<<" E: "<<theParticle->momentum().e()<<std::endl;
    }
  ISF_FCS_Parametrization::FCS_StepInfoCollection* MergeeventSteps;
  MergeeventSteps = copyFCS_StepInfo(eventStepsES);
  //double etot = 0.;
  //for (ISF_FCS_Parametrization::FCS_StepInfoList::const_iterator iter = eventSteps->begin();iter != eventSteps->end();iter++) {
  //	  etot += (*iter)->dep();
  // }
  //std::cout << "ZH: Size of input shower: " << eventSteps->size() << std::endl;
  //Still might not need this...
  truncate(MergeeventSteps);
  if (m_clusterize)
    {
      clusterize(MergeeventSteps);
    }
  else
    {
      std::cout <<"ZH: FastCaloSimParamAlg: not merging nearby hits: "<<m_clusterize<<std::endl;
    }
  //clusterize(eventSteps);
  std::cout << "Size after clusterization: " << MergeeventSteps->size() << std::endl;
  StatusCode sc = evtStore()->record(MergeeventSteps,"ZHMergedEventSteps");
  if (sc.isFailure()) 
    {
      std::cout<<"Coudn't resave merged collection "<<std::endl;
      return 0;  
    }

  return StatusCode::SUCCESS;
}

const HepMC::GenParticle* FastCaloSimParamAlg::getParticleFromMC()
{
  const DataHandle<McEventCollection> mcEvent;
  StatusCode sc = evtStore()->retrieve(mcEvent,"TruthEvent");
  if (sc.isFailure()) return 0;

  // Return the first particle of the first event
  if (mcEvent)
    return ( * (* mcEvent->begin())->particles_begin());

  return NULL;
}

const ISF_FCS_Parametrization::FCS_StepInfoCollection* FastCaloSimParamAlg::getFCS_StepInfo()
{
  const ISF_FCS_Parametrization::FCS_StepInfoCollection* eventStepsES;
  StatusCode sc = evtStore()->retrieve(eventStepsES, "ZHEventSteps");
  if (sc.isFailure()) return NULL;
  
  return eventStepsES;
}

ISF_FCS_Parametrization::FCS_StepInfoCollection* FastCaloSimParamAlg::copyFCS_StepInfo(const ISF_FCS_Parametrization::FCS_StepInfoCollection* stepinfo)
{
  ISF_FCS_Parametrization::FCS_StepInfoCollection *rez = new ISF_FCS_Parametrization::FCS_StepInfoCollection();
  ISF_FCS_Parametrization::FCS_StepInfo *copy = NULL;
  
  for (ISF_FCS_Parametrization::FCS_StepInfoCollection::const_iterator iter = stepinfo->begin(); iter!=stepinfo->end(); ++iter) {
    copy = new ISF_FCS_Parametrization::FCS_StepInfo(*(*iter));
    rez->push_back(copy);
  }
  
  return rez;
}
/*
ISF_FCS_Parametrization::FCS_StepInfoList* FastCaloSimParamAlg::copyFCS_StepInfoZeroCleanup(const ISF_FCS_Parametrization::FCS_StepInfoCollection* stepinfo)
{
  
  ISF_FCS_Parametrization::FCS_StepInfoList *rez = new ISF_FCS_Parametrization::FCS_StepInfoList();
 
  const double dsame = m_maxRadius; // 25mm^2
  ISF_FCS_Parametrization::FCS_StepInfo *i1 = NULL;
  ISF_FCS_Parametrization::FCS_StepInfo *i2 = NULL;

  for (ISF_FCS_Parametrization::FCS_StepInfoCollection::const_iterator i = stepinfo->begin(); i!=stepinfo->end(); ++i) {
    if(i1 == NULL) {
      i1 = new ISF_FCS_Parametrization::FCS_StepInfo(*(*i));
      rez->push_back(i1);
    }
    else {
      i2 = new ISF_FCS_Parametrization::FCS_StepInfo(*(*i));
      // if distance below cut off, combined and erase
      if ( (i1)->diff2(*i2) < dsame && (i1->detector() == i2->detector())) {
	*i1 += *i2;
	delete i2;
      } else {
	rez->push_back(i2);
	i1 = i2;
      }
    }
  }
  
  return rez;
}
*/

void FastCaloSimParamAlg::clusterize(ISF_FCS_Parametrization::FCS_StepInfoCollection* stepinfo)
{
  std::cout <<"ZH FastCaloSimParamAlg initial clusterize size: "<<stepinfo->size()<<" will merge steps in the same cell which are less than dR and dT to each other"<<std::endl;          
  double dsame, tsame; 

  double total_energy1 = 0;
  for (ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator i1 = stepinfo->begin(); i1 != stepinfo->end(); ++i1)
    {
      total_energy1+=(*i1)->energy();
    }
  std::cout <<"ZH Check: total energy before clusterize "<<total_energy1<<std::endl;

  //Try this if it will be faster: split to cells first
  //ISF_FCS_Parametrization::FCS_StepInfoCollection *rez = new ISF_FCS_Parametrization::FCS_StepInfoCollection();
  ISF_FCS_Parametrization::FCS_StepInfo *copy = NULL;

  std::map<Identifier, ISF_FCS_Parametrization::FCS_StepInfoCollection*> FCS_SIC_cells;
  for (ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator it = stepinfo->begin(); it != stepinfo->end(); ++it)
    {
      if (FCS_SIC_cells.find((*it)->identify()) != FCS_SIC_cells.end())
	{
	  //std::cout <<"Cell: "<<(*it)->identify()<<" exists in map, including hit: "<<std::endl;
	  //this cell already exists
	  copy = NULL;
	  copy = new ISF_FCS_Parametrization::FCS_StepInfo(*(*it));
	  FCS_SIC_cells[(*it)->identify()]->push_back(copy);
	  //std::cout <<"Adding hit E: "<<(*it)->energy()<<" New size: "<<FCS_SIC_cells[(*it)->identify()]->size()<<std::endl;
	}
      else
	{
	  //new cell
	  ISF_FCS_Parametrization::FCS_StepInfoCollection* new_fcs_sic= new ISF_FCS_Parametrization::FCS_StepInfoCollection();
	  copy = NULL;
	  copy = new ISF_FCS_Parametrization::FCS_StepInfo(*(*it));
	  new_fcs_sic->push_back(copy);
	  FCS_SIC_cells.insert(std::pair<Identifier, ISF_FCS_Parametrization::FCS_StepInfoCollection*>((*it)->identify(),new_fcs_sic));
	  //std::cout <<"New cell: "<<(*it)->identify()<<" size: "<<FCS_SIC_cells[(*it)->identify()]->size()<<std::endl;
	}
    }
  
  std::cout <<"ZH FastCaloSimParamAlg Merging separately in each cell: Ncells: "<<FCS_SIC_cells.size()<<std::endl;
  //Then do merging for each cell
  //Int_t cellN = 0;
  for (std::map<Identifier, ISF_FCS_Parametrization::FCS_StepInfoCollection*>::iterator it = FCS_SIC_cells.begin(); it!= FCS_SIC_cells.end(); ++it)
    {
      //cellN++;
      //std::cout <<"Merging cell: "<<cellN<<" id: "<<it->first<<" with Nhits: "<<(it->second)->size()<<std::endl;
      //sort each cell hits by Energy?
      //std::cout <<"Test: "<<std::endl;
      //for (ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator itt = FCS_SIC_cells[it->first]->begin(); itt!= FCS_SIC_cells[it->first]->end(); ++itt)
      //	{
      //	  std::cout <<"In "<<it->first<<" E: "<<(*itt)->energy()<<std::endl;
      //	}

      std::sort(FCS_SIC_cells[it->first]->begin(), FCS_SIC_cells[it->first]->end(), SortByE()); 
      //      std::cout <<"Test: "<<std::endl;
      //for (ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator itt = FCS_SIC_cells[it->first]->begin(); itt!= FCS_SIC_cells[it->first]->end(); ++itt)
      //        {
      //	  std::cout <<"In "<<it->first<<" sorted E: "<<(*itt)->energy()<<std::endl;
      //        }

      std::sort(FCS_SIC_cells[it->first]->begin(), FCS_SIC_cells[it->first]->end(), SortByE());

      //Int_t hitN = 0;
      //ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator it1 = FCS_SIC_cells[it->first]->begin();
      ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator it1 = (it->second)->begin();
            
      //while (it1 != FCS_SIC_cells[it->first]->end())                                                                    
      while ( it1 != (it->second)->end() )
	{
	  //hitN++;
	  //std::cout <<"Merging cell: "<<cellN<<" id: "<<it->first<<" with Nhits: "<<(it->second)->size()<<std::endl;
	  if (! m_calo_dd_man->get_element(it->first))
	    {
	      //bad identifier                                                                                     
	      std::cout <<"Something wrong with identifier: "<<(*it1)->identify()<<std::endl;
	      break; //no need to merge these
	    }
	  else if ((it->second)->size()==1)
	    {
	      //std::cout <<"Just one hit in : "<<(*it1)->identify()<<std::endl;
	      break; //go to next iterator
	    }
	  else
	    {
	      ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator it2 = it1;//hm is this the problem?
	      while (it2 != (it->second)->end())
		{
		  if ( (it1 ==it2) ) //they shouldn't be anyway                                       
		    {
		      ++it2;
		    }
		  else
		    {
		      CaloCell_ID::CaloSample layer = m_calo_dd_man->get_element(it->first)->getSampling();
		      if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3)
			{
			  dsame = m_maxRadiusLAr;
			  tsame = m_maxTimeLAr;
			}
		      else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3)
			{
			  dsame = m_maxRadiusHEC;
			  tsame = m_maxTimeHEC;
			}
		      else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2)
			{
			  dsame = m_maxRadiusTile;
			  tsame = m_maxTimeTile;
			}
		      else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2)
			{
			  dsame = m_maxRadiusFCAL;
			  tsame = m_maxTimeFCAL;
			}
		      else
			{
			  dsame = m_maxRadius;
			  tsame = m_maxTime;
			}
		      
		      if (((*it1)->diff2(**it2) < dsame) && fabs((*it1)->time() - (*it2)->time()) < tsame ) 
			{
			  **it1 += **it2;
			  it2 = (it->second)->erase(it2);
			} 
		      else 
			{
			  ++it2;
			}
		    }
		}
	      ++it1;
	    }
	}
    }
  //merge them back into a single list
  std::cout <<"Copying back"<<std::endl;
  stepinfo->clear();
  for (std::map<Identifier, ISF_FCS_Parametrization::FCS_StepInfoCollection*>::iterator it = FCS_SIC_cells.begin(); it!= FCS_SIC_cells.end(); ++it)
    {
      for (ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator iit = FCS_SIC_cells[it->first]->begin(); iit != FCS_SIC_cells[it->first]->end(); ++iit)
	{
	  stepinfo->push_back(*iit);
	}
    }
  std::cout <<"ZH Check: "<<stepinfo->size()<<std::endl;
  double total_energy2 = 0;

  for (ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator i1 = stepinfo->begin(); i1 != stepinfo->end(); ++i1)  
    {
      total_energy2+=(*i1)->energy();
    }
  std::cout <<"ZH Check: total energy "<<total_energy2<<std::endl;

  /*
  ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator i1 = stepinfo->begin();

  while (i1 != stepinfo->end()) 
    {
      CaloDetDescrElement* element = m_calo_dd_man->get_element((*i1)->identify());
      if (!element)
	{
	  std::cout <<"Something wrong with identifier: "<<(*i1)->identify()<<" from: index "<<std::distance(stepinfo->begin(), i1)<<std::endl;
	  //              ++i1;                                                                                          
	  //	  break; //this should break while i2 and increment i1                                                           
	}

      ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator i2 = i1 + 1;
      while (i2 != stepinfo->end()) 
	{
	  // if distance && time are below cut off, combined and erase && make sure they're in the same detector ZH  
	  if ( (i1 !=i2) && ( (*i1)->identify() == (*i2)->identify() ) )
	    {
	      CaloDetDescrElement* element = m_calo_dd_man->get_element((*i1)->identify());
	      if (!element)
		{
		  std::cout <<"Something wrong with identifier: "<<(*i1)->identify()<<std::endl;
		  //		  ++i1;
		  break; //this should break while i2 and increment i1
		}
	      CaloCell_ID::CaloSample layer = element->getSampling();
	      //std::cout <<"Same cell: "<< layer<<" / "<<m_calo_dd_man->get_element((*i2)->identify())->getSampling()<<std::endl;
	      if (layer >= CaloCell_ID::EMB1 && layer <= CaloCell_ID::EME3)
		{
		  dsame = m_maxRadiusLAr;
		  tsame = m_maxTimeLAr;
		}
	      else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3)
		{
		  dsame = m_maxRadiusHEC;
		  tsame = m_maxTimeHEC;
		}
	      else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2)
		{
		  dsame = m_maxRadiusTile;
		  tsame = m_maxTimeTile;
		}
	      else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2)
		{
		  dsame = m_maxRadiusFCAL;
		  tsame = m_maxTimeFCAL;
		}
	      else
		{
		  dsame = m_maxRadius;
		  tsame = m_maxTime;
		}
	      
	      //std::cout <<"Comparing with : dR: "<<dsame<< ", dT: "<<tsame<<std::endl;
	      double dr = (*i1)->diff2(**i2);
	      //std::cout <<"I have: "<<dr<<" "<<std::endl;
	      if (dr>10000)
		{
		  //std::cout << (*i1)->position()<<" "<<(*i2)->position()<<std::endl;
		}
	      if ( (dr < dsame) && ( fabs( (*i1)->time() - (*i2)->time() ) < tsame ) ) 
		{
		  //std::cout <<"Deleting: Old E: "<<(*i1)->energy()<<" "<<(*i2)->energy()<<" at "<<(*i1)->position()<<" and "<<(*i2)->position()<<" from: index "<<std::distance(stepinfo->begin(), i1)<< " and "<<std::distance(stepinfo->begin(), i2)<<std::endl;
		  **i1 += **i2;
		  //std::cout <<"New E: "<<(*i1)->energy()<<" pos "<<(*i1)->position()<<std::endl;
		  i2 = stepinfo->erase(i2);
		} 
	      else 
		{
		  //std::cout <<"Not deleting: "<<(*i1)->diff2(**i2)<<" "<<(*i1)->time()<<" "<<(*i2)->time()<<std::endl;
		  ++i2;
		}
	      //std::cout <<"New stepinfo size: "<<stepinfo->size()<<std::endl;
	    }
	  else
	    {
	      ++i2; //no match
	    }
	}
      ++i1;
    }
  */
  std::cout <<"ZH FastCaloSimParamAlg after clusterize: "<<stepinfo->size()<<std::endl;
  // remove invalid
  for (ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator i = stepinfo->begin(); i != stepinfo->end();) {
    if ((*i)->valid()) {
      i++;
    } else {
      delete (*i);
      i = stepinfo->erase(i);
    }
  }
  
  //std::cout <<"ZH after clustering "<<stepinfo->size()<<std::endl;
}

void FastCaloSimParamAlg::truncate(ISF_FCS_Parametrization::FCS_StepInfoCollection* stepinfo)
{
  std::cout <<"ZH FastCaloSimParamAlg initial truncate size: "<<stepinfo->size()<<" settings: "<<m_truncate<<std::endl;
  if (m_truncate>0)
    {
      ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator i1 = stepinfo->begin();
      while (i1 != stepinfo->end())
	{
	  if ((m_truncate>=2)&&((*i1)->time()>1000))
	    {
	      i1 = stepinfo->erase(i1);
	    }
	  else
	    ++i1;
	}
      std::cout <<"ZH FastCaloSimParamAlg after truncate size: "<<stepinfo->size()<<std::endl;
    }

  /*
    double etot = 0;
    stepinfo->sort(stepInfoDistCompare(stepInfoDistCompare::RHO));
    
    for (ISF_FCS_Parametrization::FCS_StepInfoList::const_iterator i(stepinfo->begin()); i != stepinfo->end(); i++) {
      etot += (*i)->dep();
    }
    
    double minEnergy = m_minEnergy * etot;
    double rsum = 0.;
    ISF_FCS_Parametrization::FCS_StepInfoList::iterator iterCut;
    //we're continuing our cycle if  (we're not at the end yet)  && ( (the current hit is still inside maxDistance radius) || (the total energy still less then the threshold) )
    for (iterCut = stepinfo->begin(); (iterCut != stepinfo->end()) && (((*iterCut)->position().mag2() <  m_maxDistance) || (rsum < minEnergy));iterCut++) {
      rsum += (*iterCut)->dep();
    }
    
    if (iterCut == stepinfo->end()) {
      return;
    }
    
    stepinfo->erase(iterCut,stepinfo->end());
    
    if (rsum == 0) { //WTF?
      return;
    }
    
    for (iterCut = stepinfo->begin(); iterCut != stepinfo->end(); ++iterCut){
      (*iterCut)->setE((*iterCut)->dep() * etot/rsum);
    }
  */    
}

void FastCaloSimParamAlg::addingTagsToLibrary()
{
  // get tag info
  const TagInfo* tagInfo = NULL;
  StatusCode sc = detStore()->retrieve( tagInfo );
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "No TagInfo in DetectorStore" << endmsg;
    tagInfo = NULL;
  } else {
    msg(MSG::DEBUG) << "Retrieved TagInfo" << endmsg;
  } 

}


StatusCode FastCaloSimParamAlg::finalize()
{ 
  std::cout <<"ZH FastCaloSimParamAlg - that's all finishing..."<<std::endl;  

  return StatusCode::SUCCESS;
}


