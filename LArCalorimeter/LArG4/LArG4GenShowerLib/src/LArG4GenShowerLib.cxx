/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *   $Id: LArG4GenShowerLib.cxx 770430 2016-08-26 20:49:25Z ssnyder $:
 *
 *   @short Implementation of shower library generation algorithm
 *
 *
 *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
 *  @author Sasha Glazov, DESY Hamburg, Germany
 * @version $Revision: 770430 $
 *
 */

// class header
#include "LArG4GenShowerLib/LArG4GenShowerLib.h"

// local include(s):
#include "LArG4GenShowerLib/StepInfo.h"
#include "LArG4GenShowerLib/StepInfoCollection.h"

// athena includes
#include "EventInfo/TagInfo.h"
#include "GeneratorObjects/McEventCollection.h"// For MC Truth information:
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "LArG4ShowerLib/ShowerEnergySpot.h"
#include "LArG4ShowerLib/Shower.h"
#include "LArG4ShowerLib/ShowerLibList.h"

// geant includes
#include "G4Version.hh"

// ROOT includes
#include "TFile.h"

// STL include(s):
#include <sstream>
#include <map>

using CLHEP::Hep3Vector;

// definition for helper struct
class Dist {
public:
  Dist(ShowerLib::StepInfo* the_h1, ShowerLib::StepInfo* the_h2) : h1(the_h1), h2(the_h2) {}
  ShowerLib::StepInfo* h1;
  ShowerLib::StepInfo* h2;
  ShowerLib::StepInfo* merge ()
  {
    ShowerLib::StepInfo* newStep = new ShowerLib::StepInfo(*h1,*h2);
    h1->setValid(false);
    h2->setValid(false);
    return newStep;
  }
};

class  stepInfoDistCompare{
public:
  enum CompareType {RHO,R,Z};
  stepInfoDistCompare(const CompareType type) : m_type(type) {}
  bool operator()( const ShowerLib::StepInfo * first,  const ShowerLib::StepInfo * second) const {
    return sortFunction(first, second, m_type );
  }
private:
  CompareType m_type;
  //returns true if first is closer to the origin. this should sort from closest to farest
  bool sortFunction ( const ShowerLib::StepInfo* first,  const ShowerLib::StepInfo* second, const CompareType type) const {
    if (type == RHO) {
      if (first->position().mag2() < second->position().mag2()) return true;
      return false;
    } else if (type == R) {
      if (first->position().perp2() < second->position().perp2()) return true;
      return false;
    } else if (type == Z) {
      if (first->position().z() < second->position().z()) return true;
      return false;
    }
    return true; //it's here just to avoid warnings
  }
};

LArG4GenShowerLib::LArG4GenShowerLib(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), m_stat_numshowers(0), m_stat_valid(0), m_stat_invalid(0), m_stat_nolib(0)
{
  declareProperty("MaxDistance",   m_maxDistance = 50000.,
                  "max distance squared after which the hits will be truncated");
  declareProperty("MinEnergy",   m_minEnergy = .99,
                  "energy border, that truncation won't cross");
  declareProperty("MaxRadius",        m_maxRadius = 25.,
                  "maximal radius squared until two hits will be combined");
  declareProperty("ContainmentEnergy",        m_containmentEnergy = 0.95,
                  "energy fraction that will be inside containment borders");
  declareProperty("LibStructFiles",   m_lib_struct_files,
                  "List of files to read library structures from");
  declareProperty("EnergyFraction",   m_energyFraction = .02,
                  "the allowed amount of energy that can be deposited outside calorimeter region ");
  declareProperty("PhysicsList", m_physicslist_name = "FTFP_BERT", "Geant4 PhysicsList used in the simulation");
}


StatusCode LArG4GenShowerLib::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name()
                 << " - package version " << PACKAGE_VERSION );

  ShowerLib::IShowerLib* library = NULL;

  std::vector< std::string >::const_iterator nameiter;

  ATH_MSG_DEBUG ( "Starting struct files iteration" );
  for (nameiter = m_lib_struct_files.value().begin(); nameiter != m_lib_struct_files.value().end(); nameiter++ ) {

    ATH_MSG_DEBUG ( "Struct file: " << (*nameiter) );

    library = ShowerLib::iterateStruct(*nameiter);

    if (library == NULL) {
      ATH_MSG_WARNING ( "Library structure file " << (*nameiter) << " doesn't describe a valid library" );
      continue;
    }

    std::stringstream location;
    location << library->detector() << "/" << library->particle_id();
    m_libraries[location.str()] = library;
    m_libraries_by_filename[(*nameiter)+".root"] = library;
    m_stat_lib_saved[library]=0;
    m_stat_lib_notsaved[library]=0;

    ATH_MSG_VERBOSE ( "Library named " << (*nameiter)+".root" << "is stored at the location " << location.str() );

  }

  if (m_libraries.empty()) {
    ATH_MSG_ERROR ( "No valid library structure files found. Further execution is pointless." );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO ( "LArG4GenShowerLib " << this->name() << " initialized" );

  return StatusCode::SUCCESS;
}

StatusCode LArG4GenShowerLib::execute()
{
  const ShowerLib::StepInfoCollection* eventStepsES = getStepInfo();

  const HepMC::GenParticle* theParticle = getParticleFromMC();
  if (theParticle == NULL) {
    ATH_MSG_ERROR ( "Couldn't get truth particle" );
    return StatusCode::FAILURE;
  }

  m_stat_numshowers += 1;

  double fraction = eventStepsES->invalid_energy/theParticle->momentum().e();
  if (fraction > m_energyFraction) {
    ATH_MSG_WARNING ( "Shower deposited too much energy outside the calorimeter region (" << (int)(fraction*100) << "%), ignoring" );
    m_stat_invalid += 1;
    return StatusCode::SUCCESS;
  }
  //otherwise shower is valid. even if it ultimately wont go to any library
  m_stat_valid += 1;

  ShowerLib::StepInfoList* eventSteps;

  if (eventStepsES->size()>500) {
    eventSteps = copyStepInfoZeroCleanup(eventStepsES);
  } else {
    eventSteps = copyStepInfo(eventStepsES);
  }

  double etot = 0.;
  for (ShowerLib::StepInfoList::const_iterator iter = eventSteps->begin();iter != eventSteps->end();iter++) {
    etot += (*iter)->dep();
  }

  std::stringstream location;

  location << eventStepsES->detector << "/" << theParticle->pdg_id();

  if (m_libraries.find(location.str()) == m_libraries.end()) {
    ATH_MSG_WARNING ( "No library structure for " << location.str() );

    for (ShowerLib::StepInfoList::iterator i(eventSteps->begin());i != eventSteps->end(); ++i) {
      delete (*i);
    }
    delete eventSteps;
    m_stat_nolib += 1;
    return StatusCode::SUCCESS; //not really an error, just lost time
  }

  Hep3Vector origin(theParticle->production_vertex()->position().x(),
                    theParticle->production_vertex()->position().y(),
                    theParticle->production_vertex()->position().z());

  // Also save direction vector. By default shower lib is created in
  // the direction of the input particle.
  Hep3Vector DirectionShower(theParticle->momentum().px(),
                             theParticle->momentum().py(),
                             theParticle->momentum().pz());
  DirectionShower /= DirectionShower.mag();
  Hep3Vector OrthoShower = DirectionShower.orthogonal();
  OrthoShower /= OrthoShower.mag();
  Hep3Vector CrossShower = DirectionShower.cross(OrthoShower);
  CrossShower /= CrossShower.mag();

  // Rotate the hit such that Z direction is along the shower direction
  for (ShowerLib::StepInfoList::iterator i(eventSteps->begin()); i != eventSteps->end(); ++i) {
    Hep3Vector HitDir = (*i)->position() - origin;

    (*i)->setX(HitDir*OrthoShower);
    (*i)->setY(HitDir*CrossShower);
    (*i)->setZ(HitDir*DirectionShower);
  }

  ATH_MSG_VERBOSE ( "Size of input shower: " << eventSteps->size() );

  clusterize(eventSteps);

  ATH_MSG_VERBOSE ( "Size after clusterization: " << eventSteps->size() );

  if (eventSteps->size() > 10) {
    truncate(eventSteps);
    ATH_MSG_VERBOSE ( "Size after truncation: " << eventSteps->size() );
  }

  double maxZ = 0, maxR = 0;
  double edep = 0.;

  double containmentEnergy = m_containmentEnergy * etot;

  // sort the hits by R, make the border where 95% is deposited
  eventSteps->sort(stepInfoDistCompare(stepInfoDistCompare::R));
  ShowerLib::StepInfoList::const_iterator iter = eventSteps->begin();
  for (;(iter != eventSteps->end()) && (edep < containmentEnergy);iter++) {
    edep += (*iter)->dep();
    maxR = (*iter)->position().r();
  }

  edep = 0.;
  // sort the hits by Z, make the border where 95% is deposited
  eventSteps->sort(stepInfoDistCompare(stepInfoDistCompare::Z));
  iter = eventSteps->begin();
  for (;(iter != eventSteps->end()) && (edep < containmentEnergy);iter++) {
    edep += (*iter)->dep();
    maxZ = (*iter)->position().z();
  }

  ShowerLib::Shower* shower = new ShowerLib::Shower();
  shower->setZSize(maxZ);
  shower->setRSize(maxR);

  for (ShowerLib::StepInfoList::iterator i(eventSteps->begin());i != eventSteps->end(); ++i) {

      shower->push_back(new ShowerLib::ShowerEnergySpot(G4ThreeVector((*i)->x(), (*i)->y(), (*i)->z()),(*i)->dep(),(*i)->time()));

    delete (*i);
  }

  delete eventSteps;

  if ((*m_libraries.find(location.str())).second->storeShower(theParticle, shower)) {
    m_stat_lib_saved[(*m_libraries.find(location.str())).second] += 1;
  } else {
    ATH_MSG_WARNING ( "Wasn't able to store shower (" << location.str() << ")" );
    m_stat_lib_notsaved[(*m_libraries.find(location.str())).second] += 1;
  }

  ATH_MSG_VERBOSE ( "Successfully finished" );

  return StatusCode::SUCCESS;
}

const HepMC::GenParticle* LArG4GenShowerLib::getParticleFromMC()
{
  const DataHandle<McEventCollection> mcEvent;
  if (evtStore()->retrieve(mcEvent,"BeamTruthEvent").isFailure()) return 0;

  // Return the first particle of the first event
  if (mcEvent)
    return ( * (* mcEvent->begin())->particles_begin());

  return NULL;
}

const ShowerLib::StepInfoCollection* LArG4GenShowerLib::getStepInfo()
{
  const ShowerLib::StepInfoCollection* eventStepsES;
  if (evtStore()->retrieve(eventStepsES, "EventSteps").isFailure()) return NULL;

  return eventStepsES;

}

ShowerLib::StepInfoList* LArG4GenShowerLib::copyStepInfo(const ShowerLib::StepInfoCollection* stepinfo)
{
  ShowerLib::StepInfoList *rez = new ShowerLib::StepInfoList();
  ShowerLib::StepInfo *copy = NULL;

  for (ShowerLib::StepInfoCollection::const_iterator iter = stepinfo->begin(); iter!=stepinfo->end(); ++iter) {
    copy = new ShowerLib::StepInfo(*(*iter));
    rez->push_back(copy);
  }

  return rez;
}

ShowerLib::StepInfoList* LArG4GenShowerLib::copyStepInfoZeroCleanup(const ShowerLib::StepInfoCollection* stepinfo)
{
  ShowerLib::StepInfoList *rez = new ShowerLib::StepInfoList();

  const double dsame = 1.; // 1mm^2
  ShowerLib::StepInfo *i1 = NULL;
  ShowerLib::StepInfo *i2 = NULL;

  for (ShowerLib::StepInfoCollection::const_iterator i = stepinfo->begin(); i!=stepinfo->end(); ++i) {
    if(i1 == NULL) {
      i1 = new ShowerLib::StepInfo(*(*i));
      rez->push_back(i1);
    }
    else {
      i2 = new ShowerLib::StepInfo(*(*i));
      // if distance below cut off, combined and erase
      if ( (i1)->diff2(*i2) < dsame) {
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

void LArG4GenShowerLib::clusterize(ShowerLib::StepInfoList* stepinfo)
{
  typedef std::multimap<double,Dist> distMap;

  distMap distances;

  //fill the map
  for (ShowerLib::StepInfoList::iterator i_h1(stepinfo->begin()); i_h1 != stepinfo->end(); i_h1++) {
    //iterate only the upper triangle of N*N matrix, since we do not want to create every distance twice
    for (ShowerLib::StepInfoList::reverse_iterator i_h2(stepinfo->rbegin()); (*i_h2) != (*i_h1); i_h2++) {
      distances.insert(distMap::value_type((*i_h1)->diff2(**i_h2),Dist(*i_h1, *i_h2)));
    }
  }

  const double rmin = m_maxRadius;

  int cursize = stepinfo->size();


  while (cursize > 1) {
    distMap::iterator iter = distances.begin();
    while ( !( (*iter).second.h1->valid() && (*iter).second.h2->valid() ) ) {
      distances.erase(iter++); //find the first (i.e. closest) valid pair of hits, removing invalid
    }

    if ((*iter).first >  rmin ) break; //if the closest already far enough - stop

    ShowerLib::StepInfo* mergedHit = (*iter).second.merge(); //merge two closest hits

    for (ShowerLib::StepInfoList::iterator i_h1(stepinfo->begin()); i_h1 != stepinfo->end(); i_h1++) {
      if ((*i_h1)->valid()){ //only for valid hits
        distances.insert(distMap::value_type((*i_h1)->diff2(*mergedHit),Dist(*i_h1, mergedHit))); //calculate and store distances
      }
    }

    stepinfo->push_back(mergedHit); //store the merged hit
    cursize--; //merging == -1 valid hit
  }

  // remove invalid
  for (ShowerLib::StepInfoList::iterator i = stepinfo->begin(); i != stepinfo->end();) {
    if ((*i)->valid()) {
      i++;
    } else {
      delete (*i);
      i = stepinfo->erase(i);
    }
  }

}

void LArG4GenShowerLib::truncate(ShowerLib::StepInfoList* stepinfo)
{
  double etot = 0;
  stepinfo->sort(stepInfoDistCompare(stepInfoDistCompare::RHO));

  for (ShowerLib::StepInfoList::const_iterator i(stepinfo->begin()); i != stepinfo->end(); i++) {
    etot += (*i)->dep();
  }

  double minEnergy = m_minEnergy * etot;
  double rsum = 0.;
  ShowerLib::StepInfoList::iterator iterCut;
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

  const double inv_rsum = 1. / rsum;
  for (iterCut = stepinfo->begin(); iterCut != stepinfo->end(); ++iterCut){
    (*iterCut)->setE((*iterCut)->dep() * etot*inv_rsum);
  }

}

void LArG4GenShowerLib::addingTagsToLibrary()
{
  // get tag info
  const TagInfo* tagInfo = NULL;
  if ( detStore()->retrieve( tagInfo ).isFailure() ) {
    ATH_MSG_ERROR ( "No TagInfo in DetectorStore" );
    tagInfo = NULL;
  } else {
    ATH_MSG_DEBUG ( "Retrieved TagInfo" );
  }

  libMap::iterator itr;
  for (itr = m_libraries.begin();itr != m_libraries.end();itr++){
    // release
    if (tagInfo) {
      std::string tag;
      tagInfo->findTag("AtlasRelease", tag);
      if (!tag.empty()) {
        (*itr).second->release(tag);
      }
    }

    // get geometry version
    ServiceHandle<IGeoModelSvc> geoModelSvc("GeoModelSvc", this->name());
    (*itr).second->geometry(geoModelSvc->atlasVersion());

    // get Physics list
    (*itr).second->physicsList(m_physicslist_name);

    // get geant4 version and strip off CVS Name tag
    std::string g4Version = G4Version;
    size_t pos = g4Version.find("$Name: ");
    if (pos != std::string::npos) {
      g4Version.erase(pos, 7);
    }
    pos = g4Version.find(" $");
    if (pos != std::string::npos) {
      g4Version.erase(pos, 2);
    }
    (*itr).second->geantVersion(g4Version);
  }
}


StatusCode LArG4GenShowerLib::finalize()
{

  // add condition tags
  addingTagsToLibrary();

  libMap::iterator itr;
  for (itr = m_libraries_by_filename.begin();itr != m_libraries_by_filename.end();itr++){
    ATH_MSG_DEBUG ( "Writing shower library to file " << (*itr).first );

    TFile libr((*itr).first.c_str(),"RECREATE");

    if (!(*itr).second->writeToROOT(&libr)) {
      ATH_MSG_ERROR ( "Wasn't able to write " << (*itr).first << ". Probably empty lib." );
    }

    libr.Close();
  }
  if (m_stat_numshowers> 0) {
    ATH_MSG_INFO ( "********Statistics for GenShowerLib********" );
    ATH_MSG_INFO ( "Total number of showers: " << m_stat_numshowers
                   << ", valid: "<< m_stat_valid << " (" << (m_stat_valid*100)/m_stat_numshowers << "%)"
                   << ", invalid: " << m_stat_invalid << " (" << (m_stat_invalid*100)/m_stat_numshowers << "%)" );
    for (itr = m_libraries.begin();itr != m_libraries.end();itr++){
      ATH_MSG_INFO ( "*******************************************" );
      std::stringstream ss((*itr).second->statistics());
      for(std::string line; std::getline(ss,line);)
        ATH_MSG_INFO ( line );
      ATH_MSG_INFO ( "Saved: " << m_stat_lib_saved[(*itr).second] << " Rejected: " << m_stat_lib_notsaved[(*itr).second] );
    }
    ATH_MSG_INFO ( "*******************************************" );
    ATH_MSG_INFO ( "Showers with no corresponding library: " << m_stat_nolib << " (" << (m_stat_nolib*100)/m_stat_numshowers << "%)" );
  }
  ATH_MSG_INFO ( "Finalized." );

  return StatusCode::SUCCESS;
}


void LArG4GenShowerLib::calculateMoments(const ShowerLib::StepInfoCollection& eventSteps,
                                         double& weights, double& xavfra, double& yavfra, double& ravfra)
{
  double escal(0.);
  double xav(0.), yav(0.), xav2(0.), yav2(0.);

  for (ShowerLib::StepInfoCollection::const_iterator i=eventSteps.begin();i<eventSteps.end(); ++i) {
    escal += (*i)->dep();
    xav   += (*i)->x()*(*i)->dep();
    yav   += (*i)->y()*(*i)->dep();
    xav2  += (*i)->x()*(*i)->x()*(*i)->dep();
    yav2  += (*i)->y()*(*i)->y()*(*i)->dep();
    //    std::cout << "TRUNC EXYZ"<<(*i)->x()<<" "<<(*i)->y()<<" "<<(*i)->z()
    //      <<" "<<(*i)->dep()<<" "<<(*i)->code<<std::endl;
  }

  // Center of gravity:
  const double inv_escal = 1. / escal;
  xavfra = xav*inv_escal;
  yavfra = yav*inv_escal;
  // Second momentum:
  ravfra = std::sqrt(std::abs((xav2*inv_escal-xavfra*xavfra) +
                              (yav2*inv_escal-yavfra*yavfra)));
  // energy is used as weights
  weights = escal;

  return;
}
