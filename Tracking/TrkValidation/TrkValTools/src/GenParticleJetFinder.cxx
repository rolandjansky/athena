/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// GenParticleJetFinder.cxx
//   Source file for class GenParticleJetFinder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkValTools/GenParticleJetFinder.h"
#include "TrkValEvent/GenParticleJet.h"

Trk::GenParticleJetFinder::GenParticleJetFinder(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool                    (type,name,parent),
    m_yijCut                   (0.003)
{
  declareInterface<IGenParticleJetFinder>(this);

  declareProperty("yCut",      m_yijCut);
}

///////////////////////////////
/// initialize
///////////////////////////////
StatusCode Trk::GenParticleJetFinder::initialize() {
    ATH_MSG_INFO ("initialise in " << name() );
    return StatusCode::SUCCESS;
}

///////////////////////////////
/// finalize
///////////////////////////////
StatusCode Trk::GenParticleJetFinder::finalize() {
  ATH_MSG_INFO ( "starting finalize() in " << name() );
  return StatusCode::SUCCESS;
}

///////////////////////////////
/// jetMCFinder
///////////////////////////////
std::vector< Trk::GenParticleJet >* Trk::GenParticleJetFinder::jetMCFinder(  std::vector <HepMC::ConstGenParticlePtr>&  GenStableCharged) const
{
  if (GenStableCharged.size() == 0) {
    ATH_MSG_INFO ("no selected charged particles!");
    return nullptr;
  }
  std::vector< Trk::GenParticleJet >* myJets 
    = new std::vector< Trk::GenParticleJet >;
  ATH_MSG_DEBUG ( "start jetMCFinder() with input GenParticle-vector "
          << "of size " << GenStableCharged.size() );

  myJets->clear();
  double totalEnergyFromTracks=0.;
  int partIterations=0;
  double yijmin=100.;
  bool combineParticles = false;

  std::vector<Trk::GenParticleJet>::iterator iAtMin, jAtMin;

  for( auto  i = GenStableCharged.begin() ; i < GenStableCharged.end();  ++i){
    totalEnergyFromTracks = totalEnergyFromTracks + (*i)->momentum().e();
    Trk::GenParticleJet tempPJ;
    tempPJ.addParticle( *i, int(i - GenStableCharged.begin()) );
    myJets->push_back(tempPJ);
  }

  int numParts = myJets->size();
  do {   
    combineParticles = false;
    yijmin=100.;
    for( std::vector<Trk::GenParticleJet>::iterator i = myJets->begin() ; i<myJets->end(); ++i) {

      for( std::vector<Trk::GenParticleJet>::iterator j = myJets->begin() ; j<myJets->end(); ++j) {

        if (i!=j) {

		  double yij=0;
		  double absI= sqrt((i->getMomentum()).dot(i->getMomentum()));
		  double absJ= sqrt(((*j).getMomentum()).dot((*j).getMomentum()));
		  double IdotJ=    (((*i).getMomentum()).dot((*j).getMomentum()));
		  if (fabs(absI*absJ)<0.000001 || fabs(totalEnergyFromTracks)<0.00001) {

            ATH_MSG_WARNING ("JetMCFinder: momenta / total energy is 0, "
                  << "can't continue calculations." );
            delete myJets;
            return nullptr;

          } else {
            yij=2*(i->getEnergy())*(j->getEnergy())/pow(totalEnergyFromTracks,2)*(1-(IdotJ/(absI*absJ)));
            if (yij<yijmin) {

			  combineParticles=true;
			  yijmin=yij;
			  iAtMin=i;
			  jAtMin=j;

			};
          };
		}
      }
	}
    partIterations++;
      
    /* If one pair of particles are found which have the required minimal */
    
    if (combineParticles==true) {

      ATH_MSG_VERBOSE ("combining: " << (*iAtMin).getNumParticles()
                 << " ("<<(*iAtMin).getIndicesInEvent().size()<<")"
                 << " and "<< (*jAtMin).getNumParticles()
                 << " ("<<(*jAtMin).getIndicesInEvent().size()<<")" );
	  if((*iAtMin).getNumParticles()>(*jAtMin).getNumParticles()) {

        auto partsTemp = (*jAtMin).getParticles();	  
        std::vector<int> indexTemp = (*jAtMin).getIndicesInEvent();
        //	  int partsTempSize = partsTemp.size();
        if (partsTemp.size()!=0) {
          
          std::vector<int>::iterator       ki=indexTemp.begin();
          for (auto k =partsTemp.begin(); k!=partsTemp.end(); ++k, ++ki) {
            (*iAtMin).addParticle(*k,*ki);
          }
		} else {

		  ATH_MSG_WARNING ("No particles in this jet, logic failed, stop finder." );
          delete myJets;
		  return nullptr;
		}
        myJets->erase(jAtMin);
      }
	  else {

        auto partsTemp = (*iAtMin).getParticles();	  
        std::vector<int> indexTemp = (*iAtMin).getIndicesInEvent();
        if (partsTemp.size()!=0) {

          std::vector<int>::iterator       ki=indexTemp.begin();
          for (auto k =partsTemp.begin(); k!=partsTemp.end(); ++k, ++ki) {
            //		  tempPartJet.addParticle(*k);
            (*jAtMin).addParticle(*k,*ki);
          }
		}
        else {
          ATH_MSG_WARNING ( "No particles in this jet, logic failed, stop finder." );
          delete myJets;
          return nullptr;
        }
        myJets->erase(iAtMin);
      }
    }
    int currentNumParts = 0;
    for(std::vector<Trk::GenParticleJet>::iterator k = myJets->begin(); k<myJets->end(); ++k)
      currentNumParts = currentNumParts + k->getNumParticles();

    if(numParts != currentNumParts){
      ATH_MSG_WARNING ( "Losing particles in jets!!" );
      delete myJets;
      return nullptr;
    }
    if(partIterations==999)
      ATH_MSG_INFO ( "JetMCFinder cut off because of too many (1000) iterations");
		  
  } while ((yijmin<m_yijCut && partIterations<1000)==true);
  return myJets;
}
