/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PunchThroughTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "PunchThroughTool.h"

// standard C++ libraries
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <numeric>


// standard C libraries
#include <math.h>

// Control
#include "DataModel/DataVector.h"

// Gaudi & StoreGate
#include "GaudiKernel/IPartPropSvc.h"

// HepMC
#include "HepMC/SimpleVector.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"
#include "HepPDT/ParticleDataTable.hh"

// CLHEP
#include "CLHEP/Random/RandFlat.h"

// ROOT
#include "TFile.h"
#include "TH2F.h"
#include "TAxis.h"
#include "TH1.h"
#include "TMath.h"
#include "TROOT.h"
#include "TKey.h"
#include "TClass.h"

// PathResolver
#include "PathResolver/PathResolver.h"

//ISF
#include "ISF_Event/ISFParticle.h"
#include "PDFcreator.h"
#include "PunchThroughParticle.h"
#include "ISF_Interfaces/IGeoIDSvc.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunctionInt32Histogram.h"



//Barcode
#include "BarcodeInterfaces/IBarcodeSvc.h"

//Geometry
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

//Amg
#include "GeoPrimitives/GeoPrimitivesHelpers.h"


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

ISF::PunchThroughTool::PunchThroughTool( const std::string& type,
                                         const std::string& name,
                                         const IInterface*  parent )
: base_class(type, name, parent),
  m_randomSvc("AtDSFMTGenSvc", name),
  m_punchThroughClassifier("ISF_PunchThroughClassifier"),
  m_pdgInitiators(),
  m_initiatorsMinEnergy(),
  m_initiatorsEtaRange(),
  m_punchThroughParticles(),
  m_doAntiParticles(),
  m_correlatedParticle(),
  m_minCorrEnergy(),
  m_fullCorrEnergy(),
  m_posAngleFactor(),
  m_momAngleFactor(),
  m_minEnergy(),
  m_maxNumParticles(),
  m_numParticlesFactor(),
  m_energyFactor(),
  m_particlePropSvc("PartPropSvc",name),
  m_geoIDSvc("ISF::GeoIDSvc",name),
  m_barcodeSvc("BarcodeSvc",name),
  m_envDefSvc("AtlasGeometry_EnvelopeDefSvc",name)
{
  // property definition and initialization - allows to set variables from python

  declareProperty( "PunchThroughClassifier",       m_punchThroughClassifier);
  declareProperty( "FilenameLookupTable",          m_filenameLookupTable   );
  declareProperty( "FilenameInverseCdf",           m_filenameInverseCDF  );
  declareProperty( "FilenameInversePca",           m_filenameInversePCA    );
  declareProperty( "PunchThroughInitiators",       m_pdgInitiators         );
  declareProperty( "InitiatorsMinEnergy",          m_initiatorsMinEnergy   );
  declareProperty( "InitiatorsEtaRange",           m_initiatorsEtaRange    );
  declareProperty( "PunchThroughParticles",        m_punchThroughParticles );
  declareProperty( "DoAntiParticles",              m_doAntiParticles       );
  declareProperty( "CorrelatedParticle",           m_correlatedParticle    );
  declareProperty( "MinCorrelationEnergy",         m_minCorrEnergy         );
  declareProperty( "FullCorrelationEnergy",        m_fullCorrEnergy        );
  declareProperty( "ScalePosDeflectionAngles",     m_posAngleFactor        );
  declareProperty( "ScaleMomDeflectionAngles",     m_momAngleFactor        );
  declareProperty( "MinEnergy",                    m_minEnergy             );
  declareProperty( "MaxNumParticles",              m_maxNumParticles       );
  declareProperty( "NumParticlesFactor",           m_numParticlesFactor    );
  declareProperty( "EnergyFactor",                 m_energyFactor          );
  declareProperty( "RandomNumberService",          m_randomSvc,            "Random number generator"         );
  declareProperty( "RandomStreamName",             m_randomEngineName,     "Name of the random number stream");
  declareProperty( "BarcodeSvc",                   m_barcodeSvc            );
  declareProperty( "EnvelopeDefSvc",               m_envDefSvc             );
  declareProperty( "BeamPipeRadius",               m_beamPipe              );
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

ISF::PunchThroughTool::~PunchThroughTool()
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

StatusCode ISF::PunchThroughTool::initialize()
{
  ATH_MSG_INFO( "initialize()" );

  if (m_punchThroughClassifier.retrieve().isFailure() )
  {
    ATH_MSG_ERROR (m_punchThroughClassifier.propertyName() << ": Failed to retrieve tool " << m_punchThroughClassifier.type());
    return StatusCode::FAILURE;
  }

  // resolving lookuptable file
  std::string resolvedFileName = PathResolverFindCalibFile (m_filenameLookupTable);
  if (resolvedFileName != "")
    {
      ATH_MSG_INFO( "[ punchthrough ] Parameterisation file found: " << resolvedFileName );
    }
  else
    {
      ATH_MSG_ERROR( "[ punchthrough ] Parameterisation file not found" );
      return StatusCode::FAILURE;
    }

  // open the LookupTable file
  m_fileLookupTable = new TFile( resolvedFileName.c_str(), "READ");
  if (!m_fileLookupTable)
    {
      ATH_MSG_WARNING("[ punchthrough ] unable to open the lookup-table for the punch-through simulation (file does not exist)");
      return StatusCode::FAILURE;
    }

  if (!m_fileLookupTable->IsOpen())
    {
      ATH_MSG_WARNING("[ punchthrough ] unable to open the lookup-table for the punch-through simulation (wrong or empty file?)");
      return StatusCode::FAILURE;
    }

    if (!initializeInverseCDF(PathResolverFindCalibFile(m_filenameInverseCDF)))
      {
        ATH_MSG_WARNING("[ punchthrough ] unable to open or read the inverse CDF config");
        return StatusCode::FAILURE;
      }



  if (!initializeInversePCA(PathResolverFindCalibFile(m_filenameInversePCA)))
    {
      ATH_MSG_WARNING("[ punchthrough ] unable to open or read the inverse PCA config");
      return StatusCode::FAILURE;
    }

  // retrieve the ParticleProperties handle
  if ( m_particlePropSvc.retrieve().isFailure() )
    {
      ATH_MSG_FATAL( "[ punchthrough ] Can not retrieve " << m_particlePropSvc << " ! Abort. " );
      return StatusCode::FAILURE;
    }

  // and the particle data table
  m_particleDataTable = m_particlePropSvc->PDT();
  if (m_particleDataTable==0)
    {
      ATH_MSG_FATAL( " [ punchthrough ] Could not get ParticleDataTable! Cannot associate pdg code with charge! Abort. " );
      return StatusCode::FAILURE;
    }

  // Random number service
  if ( m_randomSvc.retrieve().isFailure() )
    {
      ATH_MSG_ERROR( "[ punchthrough ] Could not retrieve " << m_randomSvc );
      return StatusCode::FAILURE;
    }

  // Get own engine with own seeds:
  m_randomEngine = m_randomSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine)
    {
      ATH_MSG_ERROR( "[ punchthrough ] Could not get random engine '" << m_randomEngineName << "'" );
      return StatusCode::FAILURE;
    }

  // Geometry identifier service
  if ( !m_geoIDSvc.empty() && m_geoIDSvc.retrieve().isFailure())
    {
      ATH_MSG_FATAL ( "[ punchthrough ] Could not retrieve GeometryIdentifier Service. Abort");
      return StatusCode::FAILURE;
    }

  //barcode service

  if (m_barcodeSvc.retrieve().isFailure() )
    {
      ATH_MSG_ERROR( "[ punchthrough ] Could not retrieve " << m_barcodeSvc );
      return StatusCode::FAILURE;
    }

  //envelope definition service
  if (m_envDefSvc.retrieve().isFailure() )
    {
      ATH_MSG_ERROR( "[ punchthrough ] Could not retrieve " << m_envDefSvc );
      return StatusCode::FAILURE;
    }

  //--------------------------------------------------------------------------------
  // register all the punch-through particles which will be simulated
  for ( unsigned int num = 0; num < m_punchThroughParticles.size(); num++ )
    {
      const int pdg = m_punchThroughParticles[num];
      // if no information is given on the creation of anti-particles -> do not simulate anti-particles
      const bool doAnti = ( num < m_doAntiParticles.size() ) ? m_doAntiParticles[num] : false;
      // if no information is given on the minimum energy -> take 50. MeV as default
      const double minEnergy = ( num < m_minEnergy.size() ) ? m_minEnergy[num] : 50.;
      // if no information is given on the maximum number of punch-through particles -> take -1 as default
      const int maxNum = ( num < m_minEnergy.size() ) ? m_maxNumParticles[num] : -1;
      // if no information is given on the scale factor for the number of particles -> take 1. as defaulft
      const double numFactor = ( num < m_numParticlesFactor.size() ) ? m_numParticlesFactor[num] : 1.;
      // if no information is given on the position angle factor -> take 1.
      const double posAngleFactor = ( num < m_posAngleFactor.size() ) ? m_posAngleFactor[num] : 1.;
      // if no information is given on the momentum angle factor -> take 1.
      const double momAngleFactor = ( num < m_momAngleFactor.size() ) ? m_momAngleFactor[num] : 1.;
      // if no information is given on the scale factor for the energy -> take 1. as default
      const double energyFactor = ( num < m_energyFactor.size() ) ? m_energyFactor[num] : 1.;

      // register the particle
      ATH_MSG_VERBOSE("[ punchthrough ] registering punch-through particle type with pdg = " << pdg );
      if ( registerParticle( pdg, doAnti, minEnergy, maxNum, numFactor, energyFactor, posAngleFactor, momAngleFactor )
           != StatusCode::SUCCESS)
        {
          ATH_MSG_ERROR("[ punchthrough ] unable to register punch-through particle type with pdg = " << pdg);
        }
    }

  // TODO: implement punch-through parameters for different m_pdgInitiators
  //       currently m_pdgInitiators is only used to filter out particles

  // check if more correlations were given than particle types were registered
  unsigned int numCorrelations = m_correlatedParticle.size();
  if ( numCorrelations > m_punchThroughParticles.size() )
    {
      ATH_MSG_WARNING("[ punchthrough ] more punch-through particle correlations are given, than punch-through particle types are registered (skipping the last ones)");
      numCorrelations = m_punchThroughParticles.size();
    }

  // now register correlation between particles
  for ( unsigned int num = 0; num < numCorrelations; num++ )
    {
      const int pdg1 = m_punchThroughParticles[num];
      const int pdg2 = m_correlatedParticle[num];
      const double fullCorrEnergy = ( num < m_fullCorrEnergy.size() ) ? m_fullCorrEnergy[num] : 0.;
      const double minCorrEnergy = ( num < m_minCorrEnergy.size() ) ?  m_minCorrEnergy[num] : 0.;

      // if correlatedParticle==0 is given -> no correlation
      if ( ! pdg2) continue;
      // register it
      if ( registerCorrelation(pdg1, pdg2, minCorrEnergy, fullCorrEnergy) != StatusCode::SUCCESS )
        {
          ATH_MSG_ERROR("[ punchthrough ] unable to register punch-through particle correlation for pdg1=" << pdg1 << " pdg2=" << pdg2 );
        }
    }

  // get the calo-MS border coordinates. Look at calo and MS geometry definitions, if same R and Z -> boundary surface

  RZPairVector* rzMS = &(m_envDefSvc->getMuonRZValues());
  RZPairVector* rzCalo = &(m_envDefSvc->getCaloRZValues());

  bool found1, found2;
  found1=false; found2=false;

  for ( size_t i=0; i<rzCalo->size();++i)
    {
      const double r_tempCalo = rzCalo->at(i).first;
      const double z_tempCalo = rzCalo->at(i).second;

      if (r_tempCalo> m_beamPipe)
        {
          for ( size_t j=0; j<rzMS->size();++j)
            {
              const double r_tempMS =rzMS->at(j).first;
              const double z_tempMS =rzMS->at(j).second;

              if (r_tempCalo==r_tempMS && z_tempCalo==z_tempMS && found1==false )
                {
                  m_R1=r_tempMS;
                  m_z1=std::fabs(z_tempMS);
                  found1=true;
                  continue;
                }
              else if (r_tempCalo==r_tempMS && z_tempCalo==z_tempMS && r_tempCalo!=m_R1 && std::fabs(z_tempCalo)!=m_z1)
                {
                  m_R2=r_tempMS;
                  m_z2=std::fabs(z_tempMS);
                  found2=true;
                }
            }

          if (found1==true && found2==true) break;
        }
    }

  //in case geometry description changes
  if (found1 == false) ATH_MSG_ERROR ("first coordinate of calo-MS border not found");
  if (found2 == false) ATH_MSG_ERROR ("second coordinate of calo-MS border not found; first one is: R1 ="<<m_R1<<" z1 ="<<m_z1);

  //now order the found values
  double r_temp, z_temp;
  if (m_R1>m_R2) { r_temp=m_R1; m_R1=m_R2; m_R2=r_temp; } //m_R1 - smaller one
  if (m_z1<m_z2) { z_temp=m_z1; m_z1=m_z2; m_z2=z_temp; } //m_z1 - bigger one

  if (m_R1==m_R2 || m_z1==m_z2) ATH_MSG_ERROR ("[punch-though] Bug in propagation calculation! R1="<<m_R1<<" R2 = "<<m_R2<<" z1="<<m_z1<<" z2= "<<m_z2 );
  else                  ATH_MSG_DEBUG ("calo-MS boundary coordinates: R1="<<m_R1<<" R2 = "<<m_R2<<" z1="<<m_z1<<" z2= "<<m_z2);

  ATH_MSG_INFO( "punchthrough initialization is successful" );
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

StatusCode ISF::PunchThroughTool::finalize()
{
  ATH_MSG_INFO( "[punchthrough] finalize() successful" );

  // close the file with the lookuptable
  m_fileLookupTable->Close();

  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

const ISF::ISFParticleVector* ISF::PunchThroughTool::computePunchThroughParticles(const ISF::ISFParticle &isfp, const TFCSSimulationState& simulstate ) const
{
  ATH_MSG_DEBUG( "[ punchthrough ] starting punch-through simulation");

  // reset the output particle collection
  m_isfpCont = new ISF::ISFParticleVector();

  // reset the parent GenEvent
  m_parentGenEvt = 0;

  // store the initial particle state locally
  m_initPs = &isfp;

  ATH_MSG_VERBOSE("[ punchthrough ] position of the input particle: r"<<m_initPs->position().perp()<<" z= "<<m_initPs->position().z() );

  //check if it points to the calorimeter - if not, don't simulate

  if ( m_geoIDSvc->identifyNextGeoID(*m_initPs) != AtlasDetDescr::fAtlasCalo)
    {
      ATH_MSG_VERBOSE ("[ GeoIDSvc ] input particle doesn't point to calorimeter"<< "Next GeoID: "<<m_geoIDSvc->identifyNextGeoID(*m_initPs) );
      return 0;
    }


  // check if the particle's pdg is registered as a punch-through-causing type
  {
    std::vector<int>::const_iterator pdgIt    = m_pdgInitiators.begin();
    std::vector<int>::const_iterator pdgItEnd = m_pdgInitiators.end();

    std::vector<int>::const_iterator minEnergyIt    = m_initiatorsMinEnergy.begin();
    // loop over all known punch-through initiators
    for ( ; pdgIt != pdgItEnd; ++pdgIt, ++minEnergyIt)
      {
        if (std::abs(m_initPs->pdgCode()) == *pdgIt){
          if(std::sqrt( m_initPs->momentum().mag2() + m_initPs->mass()*m_initPs->mass() ) < *minEnergyIt){
            ATH_MSG_DEBUG("[ punchthrough ] particle does not meet initiator min energy requirement. Dropping it in the calo.");
            return 0;
          }
          break;
        }
      }

    // particle will not cause punch-through -> bail out
    if (pdgIt == pdgItEnd)
      {
        ATH_MSG_DEBUG("[ punchthrough ] particle is not registered as punch-through initiator. Dropping it in the calo.");
        return 0;
      }
  }

  if(m_initPs->position().eta() < m_initiatorsEtaRange.at(0) || m_initPs->position().eta() > m_initiatorsEtaRange.at(1) ){
    ATH_MSG_DEBUG("[ punchthrough ] particle does not meet initiator eta range requirement. Dropping it in the calo.");
    return 0;
  }


  //Calculate probability of punch through using punchThroughClassifier
  double punchThroughProbability = m_punchThroughClassifier->computePunchThroughProbability(isfp, simulstate);

  //Draw random number to compare to probability
  double punchThroughClassifierRand = CLHEP::RandFlat::shoot(m_randomEngine);

  ATH_MSG_INFO("[ punchthrough ] punchThroughProbability output: " << punchThroughProbability << " RandFlat: " << punchThroughClassifierRand );

  //If probability < random number then don't simulate punch through
  if( punchThroughClassifierRand > punchThroughProbability){
      ATH_MSG_DEBUG("[ punchthrough ] particle not classified to create punch through. Dropping it in the calo.");
      return 0;
  }

  //if initial particle is on ID surface, points to the calorimeter, is a punch-through initiator, meets initiator min enery and eta range, and passes punchThroughClassifier

  // store initial particle mass
  const double mass = m_initPs->mass();
  //store its barcode
  m_primBC = m_initPs->barcode();
  // now store some parameters from the given particle locally
  // in this class
  //  -> the energy
  m_initEnergy = std::sqrt( m_initPs->momentum().mag2() + mass*mass );

  m_interpEnergy = interpolateEnergy(m_initEnergy);

  //  -> get geometrical properties
  //  -> the pseudorapidity eta
  m_initEta = m_initPs->position().eta();


  m_interpEta = interpolateEta(m_initEta);

  //  -> the angle theta
  m_initTheta = m_initPs->position().theta();
  //  -> the azimutal angle phi
  m_initPhi = m_initPs->position().phi();

  // this is the place where the magic is done:
  // test for each registered punch-through pdg if a punch-through
  // occures and create these particles
  // -> therefore loop over all registered pdg ids
  // to keep track of the correlated particles which were already simulated:
  // first int is pdg, second int is number of particles created
  std::map<int, int> corrPdgNumDone;


  int maxTries = 10;
  int nTries = 0;

  // loop over all particle pdgs
  while(m_isfpCont->size() < 1 && nTries < maxTries) { //ensure we always create at least one punch through particle, maxTries to catch very rare cases
      for (const auto& currentParticle : m_particles)
        {
            // the pdg that is currently treated
            int doPdg = currentParticle.first;
            // get the current particle's correlated pdg
            int corrPdg = currentParticle.second->getCorrelatedPdg();

            // if there is a correlated particle type to this one
            if (corrPdg)
            {
                // find out if the current pdg was already simulated
                std::map<int,int>::iterator pos = corrPdgNumDone.find(doPdg);
                // if the current pdg was not simulated yet, find out if
                // it's correlated one was simulated
                if ( pos == corrPdgNumDone.end() ) pos = corrPdgNumDone.find(corrPdg);

                // neither this nor the correlated particle type was simulated
                // so far:
                if ( pos == corrPdgNumDone.end() )
                {
                    // -> roll a dice if we create this particle or its correlated one
                    if ( CLHEP::RandFlat::shoot(m_randomEngine) > 0.5 ) doPdg = corrPdg;
                    // now create the particles with the given pdg and note how many
                    // particles of this pdg are created
                    corrPdgNumDone[doPdg] = getAllParticles(doPdg);
                }

                // one of the two correlated particle types was already simulated
                // 'pos' points to the already simulated one
                else
                {
                    // get the pdg of the already simulated particle and the number
                    // of these particles that were created
                    const int donePdg = pos->first;
                    const int doneNumPart = pos->second;
                    // set the pdg of the particle type that will be done
                    if (donePdg == doPdg) doPdg = corrPdg;

                    // now create the correlated particles
                    getCorrelatedParticles(doPdg, doneNumPart);
                    // note: no need to take note, that this particle type is now simulated,
                    // since this is the second of two correlated particles, which is
                    // simulated and we do not have correlations of more than two particles.
                }

                // if no correlation for this particle
                // -> directly create all particles with the current pdg
            }
            else  {
                getAllParticles(doPdg);
            }
        } // for-loop over all particle pdgs

        nTries++;
    }

  if (m_isfpCont->size() < 1)  ATH_MSG_DEBUG( "[ punchthrough ] no particles in ISFparticle vector , size: "<<m_isfpCont->size() );

  else if (m_isfpCont->size() > 0)  ATH_MSG_DEBUG( "[ punchthrough ] returning ISFparticle vector , size: "<<m_isfpCont->size() );

  for (ISF::ISFParticle *particle : *m_isfpCont) {
    ATH_MSG_DEBUG("codes of produced punch through particle: pdg = "<< particle->pdgCode());
    Amg::Vector3D position = particle->position();
    ATH_MSG_DEBUG("position of produced punch-through particle: x = "<< position.x() <<" y = "<< position.y() <<" z = "<< position.z());
    Amg::Vector3D momentum = particle->momentum();
    ATH_MSG_DEBUG("momentum of produced punch-through particle: px = "<< momentum.x() <<" py = "<< momentum.y() <<" pz = "<< momentum.z() <<" e = "<< particle->ekin() << " mass = " << particle->mass());
  }

  return m_isfpCont;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
int ISF::PunchThroughTool::getAllParticles(int pdg, int numParticles) const
{
  // first check if the ISF particle vector already exists
  if (!m_isfpCont)    m_isfpCont = new ISFParticleVector();

  // get the current particle
  PunchThroughParticle *p = m_particles[pdg];

  // if no number of particles (=-1) was handed over as an argument
  //  -> get the number of particles from the pdf
  if ( numParticles < 0 )
    {
      // prepare the function arguments for the PDFcreator class
      std::vector<int> parameters;

      parameters.push_back( std::round(m_interpEnergy) );
      parameters.push_back( std::round(m_interpEta*100) );
      // the maximum number of particles which should be produced
      // if no maximum number is given, this is -1
      int maxParticles = p->getMaxNumParticles();

      // get the right number of punch-through particles
      // and ensure that we do not create too many particles
      do
        {
          numParticles = floor( p->getNumParticlesPDF()->getRand(parameters) );

          // scale the number of particles if requested
          numParticles = lround( numParticles *= p->getNumParticlesFactor() );
        }
      while ( (maxParticles >= 0.) && (numParticles > maxParticles) );
    }

  ATH_MSG_VERBOSE("[ punchthrough ] adding " << numParticles << " punch-through particles with pdg " << pdg);

  // now create the exact number of particles which was just computed before
  double energyRest = m_initEnergy;
  double minEnergy = p->getMinEnergy();
  int numCreated = 0;

  for ( numCreated = 0; (numCreated < numParticles) && (energyRest > minEnergy); numCreated++ )
    {
      // create one particle which fullfills the right energy distribution
      ISF::ISFParticle *par = getOneParticle(pdg, energyRest);

      // if something went wrong
      if (!par)
        {
          ATH_MSG_ERROR("[ punchthrough ] something went wrong while creating punch-through particles");
          return StatusCode::FAILURE;
        }

      // get the energy of the particle which was just created
      const double restMass = m_particleDataTable->particle(std::abs(pdg))->mass();
      double curEnergy = std::sqrt(par->momentum().mag2() + restMass*restMass);

      // calculate the maximum energy to be available for all
      // following punch-through particles created
      energyRest -= curEnergy;

      // add this ISFparticle to the vector
      m_isfpCont->push_back( par );
    }

  // the number of particles which was created is numCreated
  return (numCreated);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

int ISF::PunchThroughTool::getCorrelatedParticles(int pdg, int corrParticles) const
{
  // get the PunchThroughParticle class
  PunchThroughParticle *p = m_particles[pdg];

  // (1.) decide if we do correlation or not
  double rand = CLHEP::RandFlat::shoot(m_randomEngine)
    *(p->getFullCorrelationEnergy()-p->getMinCorrelationEnergy())
    + p->getMinCorrelationEnergy();
  if ( m_initEnergy < rand )
    {
      // here we do not do correlation
      return getAllParticles(pdg);
    }

  // (2.) if this point is reached, we do correlation
  // decide which 2d correlation histogram to use
  double *histDomains = p->getCorrelationHistDomains();
  TH2F *hist2d = 0;
  // compute the center values of the lowE and highE
  // correlation histogram domains
  if ( m_initEnergy <  histDomains[1])
    {
      // initial energy lower than border between lowEnergy and highEnergy histogram domain
      //  --> choose lowEnergy correlation histogram
      hist2d = p->getCorrelationLowEHist();
    }
  else
    {
      double rand = CLHEP::RandFlat::shoot(m_randomEngine)*(histDomains[2]-histDomains[1])
        + histDomains[1];
      hist2d = ( m_initEnergy < rand) ? p->getCorrelationLowEHist()
        : p->getCorrelationHighEHist();
    }

  // get the correlation 2d histogram

  // now find out where on the x-axis the the bin for number of
  // correlated particles is
  Int_t xbin = hist2d->GetXaxis()->FindFixBin(corrParticles);
  int numParticles = 0;
  int maxParticles = p->getMaxNumParticles();
  // now the distribution along the y-axis is a PDF for the number
  // of 'pdg' particles
  do
    {
      double rand = CLHEP::RandFlat::shoot(m_randomEngine);
      double sum = 0.;
      for ( int ybin = 1; ybin <= hist2d->GetNbinsY(); ybin++ )
        {
          sum += hist2d->GetBinContent(xbin, ybin);
          // check if we choose the current bin or not
          if ( sum >= rand )
            {
              numParticles = ybin - 1;
              break;
            }
        }
      // scale the number of particles is requested
      numParticles = lround( numParticles * p->getNumParticlesFactor() );
    }
  while ( (maxParticles >= 0.) && (numParticles > maxParticles) );

  // finally create this exact number of particles
  return getAllParticles(pdg, numParticles);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

ISF::ISFParticle *ISF::PunchThroughTool::getOneParticle(int pdg, double maxEnergy) const
{

  // get a local copy of the needed punch-through particle class
  PunchThroughParticle *p = m_particles[pdg];

  // (1.) decide if we create a particle or an anti-particle
  int anti = 1;
  if ( p->getdoAnti() )
    {
      // get a random-value
      double rand = CLHEP::RandFlat::shoot(m_randomEngine);
      // 50/50 chance to be a particle or its anti-particle
      if (rand > 0.5) anti = -1;
    }

  // (2.) get the right punch-through distributions
  // prepare the function arguments for the PDFcreator class
  std::vector<int> parInitEnergyEta;
  parInitEnergyEta.push_back( std::round(m_interpEnergy) );
  parInitEnergyEta.push_back( std::round(m_interpEta*100) );

  //initialise variables to store punch through particle kinematics
  double energy = 0.;
  double deltaTheta = 0.;
  double deltaPhi = 0.;
  double momDeltaTheta = 0.;
  double momDeltaPhi = 0.;

  double principal_component_0 = 0.;
  double principal_component_1 = 0.;
  double principal_component_2 = 0.;
  double principal_component_3 = 0.;
  double principal_component_4 = 0.;
  std::vector<double> transformed_variables;

  while (energy < p->getMinEnergy()){

      principal_component_0 = p->getPCA0PDF()->getRand(parInitEnergyEta);
      principal_component_1 = p->getPCA1PDF()->getRand(parInitEnergyEta);
      principal_component_2 = p->getPCA2PDF()->getRand(parInitEnergyEta);
      principal_component_3 = p->getPCA3PDF()->getRand(parInitEnergyEta);
      principal_component_4 = p->getPCA4PDF()->getRand(parInitEnergyEta);

      ATH_MSG_DEBUG("Drawn punch through kinematics PCA components: PCA0 = "<< principal_component_0 <<" PCA1 = "<< principal_component_1 <<" PCA2 = "<< principal_component_2 <<" PCA3 = "<< principal_component_3 <<" PCA4 = "<< principal_component_4 );

      std::vector<double> principal_components;
      principal_components.push_back(principal_component_0);
      principal_components.push_back(principal_component_1);
      principal_components.push_back(principal_component_2);
      principal_components.push_back(principal_component_3);
      principal_components.push_back(principal_component_4);

      transformed_variables = inversePCA(principal_components);

      energy = inverseCdfTransform(transformed_variables.at(0), m_variable0_inverse_cdf);
      deltaTheta = inverseCdfTransform(transformed_variables.at(1), m_variable1_inverse_cdf);
      deltaPhi = inverseCdfTransform(transformed_variables.at(2), m_variable2_inverse_cdf);
      momDeltaTheta = inverseCdfTransform(transformed_variables.at(3), m_variable3_inverse_cdf);
      momDeltaPhi = inverseCdfTransform(transformed_variables.at(4), m_variable4_inverse_cdf);

      ATH_MSG_DEBUG("Transformed punch through kinematics: energy = "<< energy <<" deltaTheta = "<< deltaTheta <<" deltaPhi = "<< deltaPhi <<" momDeltaTheta = "<< momDeltaTheta <<" momDeltaPhi = "<< momDeltaPhi );

  }


  energy *= p->getEnergyFactor(); // scale the energy if requested

  // (2.2) get the particles delta theta relative to the incoming particle
  double theta = 0;
  // loop to keep theta within range [0,PI]
  do
    {
      // decide if delta positive/negative
      deltaTheta *=  ( CLHEP::RandFlat::shoot(m_randomEngine) > 0.5 ) ? 1. : -1.;
      // calculate the exact theta value of the later created
      // punch-through particle
      theta = m_initTheta + deltaTheta*p->getPosAngleFactor();

    }
  while ( (theta > M_PI) || (theta < 0.) );
  // (2.3) get the particle's delta phi relative to the incoming particle


  deltaPhi *=  ( CLHEP::RandFlat::shoot(m_randomEngine) > 0.5 ) ? 1. : -1.;

  // keep phi within range [-PI,PI]
  double phi = m_initPhi + deltaPhi*p->getPosAngleFactor();
  while ( std::fabs(phi) > 2*M_PI) phi /= 2.;
  while (phi >  M_PI) phi -= 2*M_PI;
  while (phi < -M_PI) phi += 2*M_PI;


  // (2.4) get the particle momentum delta theta, relative to its position
  //
  // loop to keep momTheta within range [0,PI]

  double momTheta = 0.;
  do
    {
      // decide if delta positive/negative
      momDeltaTheta *=  ( CLHEP::RandFlat::shoot(m_randomEngine) > 0.5 ) ? 1. : -1.;
      // calculate the exact momentum theta value of the later created
      // punch-through particle
      momTheta = theta + momDeltaTheta*p->getMomAngleFactor();
    }
  while ( (momTheta > M_PI) || (momTheta < 0.) );

  // (2.5) get the particle momentum delta phi, relative to its position

  momDeltaPhi *=  ( CLHEP::RandFlat::shoot(m_randomEngine) > 0.5 ) ? 1. : -1.;

  double momPhi = phi + momDeltaPhi*p->getMomAngleFactor();
  // keep momPhi within range [-PI,PI]
  while ( std::fabs(momPhi) > 2*M_PI) momPhi /= 2.;
  while (momPhi >  M_PI) momPhi -= 2*M_PI;
  while (momPhi < -M_PI) momPhi += 2*M_PI;


  //assign barcodes to the produced particles
  m_processCode = 0;
  m_secBC = m_barcodeSvc->newSecondary( m_primBC, m_processCode);

  // (**) finally create the punch-through particle as a ISFParticle

  ATH_MSG_DEBUG("createExitPs input parameters: doAnti? = "<< pdg*anti <<" energy = "<< energy <<" theta = "<< theta <<" phi = "<< phi <<" momTheta = "<< momTheta << " momPhi " << momPhi );

  ISF::ISFParticle *par = createExitPs( pdg*anti, energy, theta, phi, momTheta, momPhi);

  return par;
}


double ISF::PunchThroughTool::normal_cdf(double x) const{

    return  0.5 * TMath::Erfc(-x * m_SQRT_0p5);
}

std::vector<double> ISF::PunchThroughTool::dotProduct(const std::vector<std::vector<double>> &m, const std::vector<double> &v) const
{
    std::vector<double> result;
    for (auto& r : m){
        result.push_back(std::inner_product(v.begin(), v.end(), r.begin(), 0.0));
    }

    return result;
}

std::vector<double> ISF::PunchThroughTool::inversePCA(std::vector<double> &variables) const
{
    std::vector<double> transformed_variables = dotProduct(m_inverse_PCA_matrix, variables);

    std::transform (transformed_variables.begin(), transformed_variables.end(), m_PCA_means.begin(), transformed_variables.begin(), std::plus<double>()); // + means

    return transformed_variables;
}

StatusCode ISF::PunchThroughTool::initializeInversePCA(std::string inversePCAConfigFile){

    xmlDocPtr doc = xmlParseFile( inversePCAConfigFile.c_str() );

    ATH_MSG_INFO( "[ punchthrough ] Loading inversePCA: " << inversePCAConfigFile);

    for( xmlNodePtr nodeRoot = doc->children; nodeRoot != nullptr; nodeRoot = nodeRoot->next) {

        if (xmlStrEqual( nodeRoot->name, BAD_CAST "PCAinverse" )) {
            for( xmlNodePtr nodePCAinverse = nodeRoot->children; nodePCAinverse != nullptr; nodePCAinverse = nodePCAinverse->next ) {

                if (xmlStrEqual( nodePCAinverse->name, BAD_CAST "PCAmatrix" )) {

                    std::vector<double> PCA_matrix_row;

                    PCA_matrix_row.push_back( atof( (const char*) xmlGetProp( nodePCAinverse, BAD_CAST "comp_0" ) ) );
                    PCA_matrix_row.push_back( atof( (const char*) xmlGetProp( nodePCAinverse, BAD_CAST "comp_1" ) ) );
                    PCA_matrix_row.push_back( atof( (const char*) xmlGetProp( nodePCAinverse, BAD_CAST "comp_2" ) ) );
                    PCA_matrix_row.push_back( atof( (const char*) xmlGetProp( nodePCAinverse, BAD_CAST "comp_3" ) ) );

                    m_inverse_PCA_matrix.push_back(PCA_matrix_row);

                }

                else if (xmlStrEqual( nodePCAinverse->name, BAD_CAST "PCAmeans" )) {

                    m_PCA_means.push_back( atof( (const char*) xmlGetProp( nodePCAinverse, BAD_CAST "mean_0" ) ) );
                    m_PCA_means.push_back( atof( (const char*) xmlGetProp( nodePCAinverse, BAD_CAST "mean_1" ) ) );
                    m_PCA_means.push_back( atof( (const char*) xmlGetProp( nodePCAinverse, BAD_CAST "mean_2" ) ) );
                    m_PCA_means.push_back( atof( (const char*) xmlGetProp( nodePCAinverse, BAD_CAST "mean_3" ) ) );

                }
            }
        }
    }
    return StatusCode::SUCCESS;
}


StatusCode ISF::PunchThroughTool::initializeInverseCDF(std::string inverseCdfConfigFile){

    //parse xml that contains config for inverse CDF for each of punch through particle kinematics

    xmlDocPtr doc = xmlParseFile( inverseCdfConfigFile.c_str() );

    ATH_MSG_INFO( "[ punchthrough ] Loading inverse CDF: " << inverseCdfConfigFile);

    for( xmlNodePtr nodeRoot = doc->children; nodeRoot != nullptr; nodeRoot = nodeRoot->next) {

        if (xmlStrEqual( nodeRoot->name, BAD_CAST "CDFMappings" )) {
            for( xmlNodePtr nodeMappings = nodeRoot->children; nodeMappings != nullptr; nodeMappings = nodeMappings->next ) {

                if (xmlStrEqual( nodeMappings->name, BAD_CAST "variable0" )) {
                    m_variable0_inverse_cdf = getVariableCDFmappings(nodeMappings);
                }
                else if (xmlStrEqual( nodeMappings->name, BAD_CAST "variable1" )) {
                    m_variable1_inverse_cdf = getVariableCDFmappings(nodeMappings);
                }
                else if (xmlStrEqual( nodeMappings->name, BAD_CAST "variable2" )) {
                    m_variable2_inverse_cdf = getVariableCDFmappings(nodeMappings);
                }
                else if (xmlStrEqual( nodeMappings->name, BAD_CAST "variable3" )) {
                    m_variable3_inverse_cdf = getVariableCDFmappings(nodeMappings);
                }
                else if (xmlStrEqual( nodeMappings->name, BAD_CAST "variable4" )) {
                    m_variable4_inverse_cdf = getVariableCDFmappings(nodeMappings);
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

std::map<double, double> ISF::PunchThroughTool::getVariableCDFmappings(xmlNodePtr& nodeParent){

    std::map<double, double>  m_mappings;

    for( xmlNodePtr node = nodeParent->children; node != nullptr; node = node->next ) {
        //Get min and max values that we normalise values to
        if (xmlStrEqual( node->name, BAD_CAST "CDFmap" )) {
            double ref = atof( (const char*) xmlGetProp( node, BAD_CAST "ref" ) );
            double quant = atof( (const char*) xmlGetProp( node, BAD_CAST "quant" ) );

            m_mappings.insert(std::pair<double, double>(ref, quant) );

        }
    }

    return m_mappings;
}


double ISF::PunchThroughTool::inverseCdfTransform(double variable, std::map<double, double> inverse_cdf_map) const{

    double norm_cdf = normal_cdf(variable);

    auto upper = inverse_cdf_map.upper_bound(norm_cdf);
    auto lower = upper--;

    double m = (upper->second - lower->second)/(upper->first - lower->first);
    double c = lower->second - m * lower->first;
    double transformed = m * norm_cdf + c;

    return transformed;

}

double ISF::PunchThroughTool::interpolateEnergy(double &energy) const{

    ATH_MSG_DEBUG("[ punchthrough ] interpolating incoming energy: " << energy);

    std::string energyPointsString;
    for (auto element:m_energyPoints){
        energyPointsString += std::to_string(element) + " ";
    }

    ATH_MSG_DEBUG("[ punchthrough ] available energy points: " << energyPointsString);

    auto const upperEnergy = std::upper_bound(m_energyPoints.begin(), m_energyPoints.end(), energy);

    if(upperEnergy == m_etaPoints.end()){ //if no energy greater than input energy, choose greatest energy
        ATH_MSG_DEBUG("[ punchthrough ] incoming energy > largest energy point, returning greatest energy point: " << m_energyPoints.back());
        return m_energyPoints.back();
    }
    else if(upperEnergy == m_etaPoints.begin()){ //if smallest energy greater than input energy, choose smallest energy
        ATH_MSG_DEBUG("[ punchthrough ] incoming energy < smallest energy point, returning smallest energy point: " << *upperEnergy);
        return *upperEnergy;
    }

    ATH_MSG_DEBUG("[ punchthrough ] energy points upper_bound: "<< *upperEnergy);

    double randomShoot = CLHEP::RandFlat::shoot(m_randomEngine);

    ATH_MSG_DEBUG("[ punchthrough ] Shooting random number: "<< randomShoot);

    double midPoint = *std::prev(upperEnergy)*m_SQRT_2;

    if(energy <  midPoint){ //if energy smaller than mid point in log(energy)

        double distance = std::abs(energy - *std::prev(upperEnergy))/((midPoint) - *std::prev(upperEnergy));

        ATH_MSG_DEBUG( "[ punchthrough ] incoming energy is closest to prev(upper_bound) in log(energy), distance: " << distance );

        if(randomShoot < distance){
            ATH_MSG_DEBUG( "[ punchthrough ] randomShoot < distance, returning upper_bound " << *upperEnergy );
            return *upperEnergy;
        }
        ATH_MSG_DEBUG( "[ punchthrough ] randomShoot > distance, returning prev(upper_bound) " << *std::prev(upperEnergy) );

        return *std::prev(upperEnergy);
    }
    else if(energy >  midPoint){ //if energy greater than mid point in log(energy)

        double distance = std::abs(energy - *upperEnergy)/((*upperEnergy - midPoint));

        ATH_MSG_DEBUG( "[ punchthrough ] incoming energy is closest to upper_bound in log(energy), distance: " << distance );

        if(randomShoot < distance){
            ATH_MSG_DEBUG( "[ punchthrough ] randomShoot < distance, returning prev(upper_bound) " << *std::prev(upperEnergy) );
            return *std::prev(upperEnergy);
        }
        ATH_MSG_DEBUG( "[ punchthrough ] randomShoot > distance, returning upper_bound " << *upperEnergy );
        return *upperEnergy;
    }

    return *upperEnergy;
}

double ISF::PunchThroughTool::interpolateEta(double &eta) const{

    double absEta = std::abs(eta);

    ATH_MSG_DEBUG("[ punchthrough ] interpolating incoming abs(eta): " << absEta);

    std::string etaPointsString;
    for (auto element:m_etaPoints){
        etaPointsString += std::to_string(element) + " ";
    }

    ATH_MSG_DEBUG("[ punchthrough ] available eta points: " << etaPointsString);

    auto const upperEta = std::upper_bound(m_etaPoints.begin(), m_etaPoints.end(), absEta);

    if(upperEta == m_etaPoints.end()){
        ATH_MSG_DEBUG("[ punchthrough ] incoming abs(eta) > largest eta point, returning greatest eta point: " << m_etaPoints.back());
        return m_etaPoints.back();
    }


    ATH_MSG_DEBUG("[ punchthrough ] eta points upper_bound: "<< *upperEta);

    double randomShoot = CLHEP::RandFlat::shoot(m_randomEngine);

    ATH_MSG_DEBUG("[ punchthrough ] Shooting random number: "<< randomShoot);

    if(std::abs(absEta - *upperEta) <  std::abs(absEta - *std::prev(upperEta))){

        double distance = std::abs(absEta - *upperEta)/((*upperEta - *std::prev(upperEta))/2);

        ATH_MSG_DEBUG( "[ punchthrough ] abs(eta) is closer to eta points upper_bound, distance: " << distance );

        if(randomShoot > distance){
            ATH_MSG_DEBUG( "[ punchthrough ] randomShoot > distance, returning upper_bound " << *upperEta );
            return *upperEta;
        }

        ATH_MSG_DEBUG( "[ punchthrough ] randomShoot < distance, returning prev(upper_bound) " << *std::prev(upperEta) );

        return *std::prev(upperEta);
    }
    else if(std::abs(absEta - *std::prev(upperEta)) <  std::abs(absEta - *upperEta)){

        if(std::prev(upperEta) == m_etaPoints.begin()){
            ATH_MSG_DEBUG( "[ punchthrough ] prev of upper bound is begin, returning that: " << *std::prev(upperEta) );
            return *std::prev(upperEta);
        }

        double distance = std::abs(absEta - *std::prev(upperEta))/((*std::prev(upperEta) - *std::prev(std::prev(upperEta)))/2);

        ATH_MSG_DEBUG( "[ punchthrough ] abs(eta) is closer to eta points prev(upper_bound), distance: " << distance );

        if(randomShoot > distance){
            ATH_MSG_DEBUG( "[ punchthrough ] randomShoot > distance, returning prev(prev(upper_bound)) " << *std::prev(std::prev(upperEta)) );

            return *std::prev(std::prev(upperEta));
        }
        ATH_MSG_DEBUG( "[ punchthrough ] randomShoot < distance, returning prev(upper_bound) " << *std::prev(upperEta) );

        return *std::prev(upperEta);
    }

    return *std::prev(upperEta);
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

StatusCode
ISF::PunchThroughTool::registerParticle(int pdg, bool doAntiparticle,
                                        double minEnergy, int maxNumParticles, double numParticlesFactor,
                                        double energyFactor, double posAngleFactor, double momAngleFactor)
{
  // read in the data needed to construct the distributions for the number of punch-through particles

  // (1.) get the distribution function for the number of punch-through particles
  std::unique_ptr<ISF::PDFcreator> pdf_num(readLookuptablePDF(pdg, "FREQ_PDG"));
  if (!pdf_num ) return StatusCode::FAILURE; // return error if something went wrong

  // (2.) get the PDF for the punch-through energy
  std::unique_ptr<PDFcreator> pdf_pca0 (readLookuptablePDF(pdg, "PCA0_PDG"));
  if (!pdf_pca0)
    {
      return StatusCode::FAILURE; // return error if something went wrong
    }

  // (3.) get the PDF for the punch-through particles difference in
  //      theta compared to the incoming particle
  std::unique_ptr<PDFcreator> pdf_pca1 (readLookuptablePDF(pdg, "PCA1_PDG"));
  if (!pdf_pca1)
    {
      return StatusCode::FAILURE;
    }

  // (4.) get the PDF for the punch-through particles difference in
  //      phi compared to the incoming particle
  std::unique_ptr<PDFcreator> pdf_pca2 (readLookuptablePDF(pdg, "PCA2_PDG"));
  if (!pdf_pca2)
    {
      return StatusCode::FAILURE;
    }

  // (5.) get the PDF for the punch-through particle momentum delta theta angle
  std::unique_ptr<PDFcreator> pdf_pca3 (readLookuptablePDF(pdg, "PCA3_PDG"));
  if (!pdf_pca3)
    {
      return StatusCode::FAILURE;
    }

  // (6.) get the PDF for the punch-through particle momentum delta phi angle
  std::unique_ptr<PDFcreator> pdf_pca4 (readLookuptablePDF(pdg, "PCA4_PDG"));
  if (!pdf_pca4)
    {
      return StatusCode::FAILURE;
    }

  // (7.) now finally store all this in the right std::map
  PunchThroughParticle *particle = new PunchThroughParticle(pdg, doAntiparticle);
  particle->setNumParticlesPDF(std::move(pdf_num));
  particle->setPCA0PDF(std::move(pdf_pca0));
  particle->setPCA1PDF(std::move(pdf_pca1));
  particle->setPCA2PDF(std::move(pdf_pca2));
  particle->setPCA3PDF(std::move(pdf_pca3));
  particle->setPCA4PDF(std::move(pdf_pca4));

  // (8.) set some additional particle and simulation properties
  const double restMass = m_particleDataTable->particle(std::abs(pdg))->mass();
  minEnergy = ( minEnergy > restMass ) ? minEnergy : restMass;
  particle->setMinEnergy(minEnergy);
  particle->setMaxNumParticles(maxNumParticles);
  particle->setNumParticlesFactor(numParticlesFactor);
  particle->setEnergyFactor(energyFactor);
  particle->setPosAngleFactor(posAngleFactor);
  particle->setMomAngleFactor(momAngleFactor);

  // (9.) insert this PunchThroughParticle instance into the std::map class member
  m_particles[pdg] = particle;

  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

StatusCode ISF::PunchThroughTool::registerCorrelation(int pdgID1, int pdgID2,
                                                      double minCorrEnergy, double fullCorrEnergy)
{
  // find the given pdgs in the registered particle ids
  std::map<int, PunchThroughParticle*>::iterator location1 = m_particles.find(pdgID1);
  std::map<int, PunchThroughParticle*>::iterator location2 = m_particles.find(pdgID2);

  // if at least one of the given pdgs was not registered yet -> return an error
  if ( (location1 == m_particles.end()) || (location2 == m_particles.end()) )
    return StatusCode::FAILURE;

  // now look for the correlation histograms
  std::stringstream name;
  name << "NumExitCorrelations/x_PDG" << std::abs(pdgID1) << "__y_PDG" << std::abs(pdgID2) << "__lowE";
  TH2F *hist1_2_lowE = (TH2F*)m_fileLookupTable->Get(name.str().c_str());
  name.str("");
  name << "NumExitCorrelations/x_PDG" << std::abs(pdgID1) << "__y_PDG" << std::abs(pdgID2) << "__highE";
  TH2F *hist1_2_highE = (TH2F*)m_fileLookupTable->Get(name.str().c_str());
  name.str("");
  name << "NumExitCorrelations/x_PDG" << std::abs(pdgID2) << "__y_PDG" << std::abs(pdgID1) << "__lowE";
  TH2F *hist2_1_lowE = (TH2F*)m_fileLookupTable->Get(name.str().c_str());
  name.str("");
  name << "NumExitCorrelations/x_PDG" << std::abs(pdgID2) << "__y_PDG" << std::abs(pdgID1) << "__highE";
  TH2F *hist2_1_highE = (TH2F*)m_fileLookupTable->Get(name.str().c_str());
  // check if the histograms exist
  if ( (!hist1_2_lowE) || (!hist2_1_lowE) || (!hist1_2_highE) || (!hist2_1_highE) )
    {
      ATH_MSG_ERROR("[ punchthrough ] unable to retrieve the correlation data for PDG IDs " << pdgID1 <<  " and " << pdgID2);
      return StatusCode::FAILURE;
    }

  // TODO: if only one of the two histograms exists, create the other one
  //       by mirroring the data

  const double lowE = getFloatAfterPatternInStr( hist1_2_lowE->GetTitle(), "elow_");
  const double midE = getFloatAfterPatternInStr( hist1_2_lowE->GetTitle(), "ehigh_");
  //TODO: check if the same:
  // double midE = getFloatAfterPatternInStr( hist1_2_lowE->GetTitle(), "elow_");
  const double upperE = getFloatAfterPatternInStr( hist1_2_highE->GetTitle(), "ehigh_");
  // now store the correlation either way  id1->id2 and id2->id1
  m_particles[pdgID1]->setCorrelation(pdgID2, hist2_1_lowE, hist2_1_highE,
                                      minCorrEnergy, fullCorrEnergy,
                                      lowE, midE, upperE);

  m_particles[pdgID2]->setCorrelation(pdgID1, hist1_2_lowE, hist1_2_highE,
                                      minCorrEnergy, fullCorrEnergy,
                                      lowE, midE, upperE);
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *======================================================================*/

std::unique_ptr<ISF::PDFcreator> ISF::PunchThroughTool::readLookuptablePDF(int pdg, std::string folderName)
{

  // will hold the PDFcreator class which will be returned at the end
  // this will store the distributions for the punch through particles
  // (as map of energy & eta of the incoming particle)
  //PDFcreator *pdf = new PDFcreator(m_randomEngine);
  std::unique_ptr<ISF::PDFcreator> pdf = std::make_unique<ISF::PDFcreator>(m_randomEngine);

      //Get directory object
      std::stringstream dirName;
      dirName << folderName << pdg;
      pdf->setName(dirName.str().c_str());

      TDirectory * dir = (TDirectory*)m_fileLookupTable->Get(dirName.str().c_str());
      if(! dir)
      {
        ATH_MSG_ERROR( "[ punchthrough ] unable to retrieve directory object ("<< folderName << pdg << ")" );
        return 0;
      }



      //Get list of all objects in directory
      TIter keyList(dir->GetListOfKeys());
      TKey *key;

      while ((key = (TKey*)keyList())) {

        //Get histogram object from key and its name
        TH1* hist = nullptr;

        std::string histName;
        if(strcmp(key->GetClassName(), "TH1F") == 0){
          hist = (TH1*)key->ReadObj();
          histName = hist->GetName();
        }

        //extract energy and eta from hist name 6 and 1 to position delimeters correctly
        std::string strEnergy = histName.substr( histName.find_first_of("E") + 1, histName.find_first_of("_")-histName.find_first_of("E") - 1 );
        histName.erase(0, histName.find_first_of("_") + 1);
        std::string strEtaMin = histName.substr( histName.find("etaMin") + 6, histName.find_first_of("_") - histName.find("etaMin") - 6 );
        histName.erase(0, histName.find("_") + 1);
        std::string strEtaMax = histName.substr( histName.find("etaMax") + 6, histName.length());

        //create integers to store in map
        const int energy = std::stoi(strEnergy);
        const int etaMin = std::stoi(strEtaMin);

        //Add entry to pdf map
        pdf->addToEnergyEtaHist1DMap(energy, etaMin, hist);

        //create doubles to store energy and eta points for interpolation
        const double energyDbl = static_cast<double>(energy);
        const double etaDbl = static_cast<double>(etaMin)/100.;

        //create vectors to store the eta and energy points, this allows us to interpolate
        if (std::find(m_energyPoints.begin(), m_energyPoints.end(), energyDbl) == m_energyPoints.end()) {
            m_energyPoints.push_back(energyDbl);
        }
        if (std::find(m_etaPoints.begin(), m_etaPoints.end(), etaDbl) == m_etaPoints.end()) {
            m_etaPoints.push_back(etaDbl);
        }

      }



  return pdf;
}

/* =========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=========================================================================*/

ISF::ISFParticle* ISF::PunchThroughTool::createExitPs( int pdg,
                                                       double energy, double theta, double phi,double momTheta, double momPhi) const
{
  // the intersection point with Calo-MS surface

  const Amg::Vector3D pos = propagator(theta,phi);

  // set up the real punch-through particle at this position
  // set up the momentum vector of this particle as a GlobalMomentum
  // by using the given energy and mass of the particle and also using
  // the given theta and phi

  Amg::Vector3D mom;
  double mass = m_particleDataTable->particle(std::abs(pdg))->mass();
  Amg::setRThetaPhi( mom, std::sqrt(energy*energy - mass*mass), momTheta, momPhi);
  ATH_MSG_DEBUG("setRThetaPhi pre input parameters: energy = "<< energy <<" mass = "<< mass);
  ATH_MSG_DEBUG("setRThetaPhi input parameters: std::sqrt(energy*energy - mass*mass) = "<< std::sqrt(energy*energy - mass*mass) <<" momTheta = "<< momTheta <<" momPhi = "<< momPhi);


  double charge = m_particleDataTable->particle(std::abs(pdg))->charge();
  // since the PDT table only has abs(PID) values for the charge
  charge *= (pdg > 0.) ?  1. : -1.;

  const double pTime = 0;  /** @TODO: fix */

  ISF::ISFParticle* finalPar = new ISF::ISFParticle (pos, mom, mass, charge, pdg, pTime, *m_initPs, m_secBC);
  finalPar->setNextGeoID( AtlasDetDescr::fAtlasMS);

  // return the punch-through particle
  return finalPar;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/

double ISF::PunchThroughTool::getFloatAfterPatternInStr(const char *cstr, const char *cpattern)
{
  double num = 0.;

  const std::string str( cstr);
  const std::string pattern( cpattern);
  const size_t pos = str.find(cpattern);

  if ( pos == std::string::npos)
    {
      ATH_MSG_WARNING("[ punchthrough ] unable to retrieve floating point number from string");
      return -999999999999.;
    }

  std::istringstream iss( cstr+pos+pattern.length());
  iss >> std::dec >> num;

  return num;
}

Amg::Vector3D ISF::PunchThroughTool::propagator(double theta,double phi) const
{
  // phi, theta - direction of the punch-through particle coming into calo
  //particle propagates inside the calorimeter along the straight line
  //coordinates of this particles when exiting the calo (on calo-MS boundary)

  double x, y, z, r;

  // cylinders border angles
  const double theta1 = atan (m_R1/m_z1);
  const double theta2 = atan (m_R1/m_z2);
  const double theta3 = atan (m_R2/m_z2);
  //where is the particle

  if (theta >= 0 && theta < theta1)
    {
      z = m_z1;
      r = std::fabs (m_z1*tan(theta));
    }
  else if (theta >= theta1 && theta < theta2)
    {
      z = m_R1/tan(theta);
      r = m_R1;
    }
  else if (theta >= theta2 && theta < theta3)
    {
      z = m_z2;
      r = std::fabs(m_z2*tan(theta));;
    }
  else if (theta >= theta3 && theta < (TMath::Pi()-theta3) )
    {
      z = m_R2/tan(theta);
      r = m_R2;
    }
  else if (theta >= (TMath::Pi()-theta3) && theta < (TMath::Pi()-theta2) )
    {
      z = -m_z2;
      r = std::fabs(m_z2*tan(theta));
    }
  else if (theta >= (TMath::Pi()-theta2) && theta < (TMath::Pi()-theta1) )
    {
      z = m_R1/tan(theta);
      r = m_R1;
    }
  else if (theta >= (TMath::Pi()-theta1) && theta <= TMath::Pi() )
    {
      z = -m_z1;
      r = std::fabs(m_z1*tan(theta));
    }

  //parallel universe
  else
    {
      ATH_MSG_WARNING ( "Given theta angle is incorrect, setting particle position to (0, 0, 0)");
      x = 0.0; y = 0.0; z = 0.0; r = 0.0;
    }

  x = r*cos(phi);
  y = r*sin(phi);
  Amg::Vector3D pos(x, y, z);

  ATH_MSG_DEBUG("position of produced punch-through particle: x = "<< x <<" y = "<< y <<" z = "<< z<<" r = "<< pos.perp() <<"std::sqrt(x^2+y^2) = "<< std::sqrt(x*x+y*y) );
  ATH_MSG_DEBUG("GeoID thinks: Calo: "<< m_geoIDSvc->inside(pos, AtlasDetDescr::fAtlasCalo) <<" MS: "<< m_geoIDSvc->inside(pos,AtlasDetDescr::fAtlasMS));

  return pos;
}
