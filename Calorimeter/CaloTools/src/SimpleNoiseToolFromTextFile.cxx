/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/Units.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"
#include "CaloTools/SimpleNoiseToolFromTextFile.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

using Athena::Units::MeV;
using CLHEP::RandGauss;

SimpleNoiseToolFromTextFile::SimpleNoiseToolFromTextFile(const std::string& type,
							 const std::string& name,
							 const IInterface* parent)
  : AthAlgTool(type,name,parent)
    , m_caloDDM(nullptr)
    , m_caloIDH(nullptr)
    , m_cellNoiseFileName("")
    , m_cellNoiseUnits(MeV)
    , m_cellNoiseDefault(100.*MeV)
    , m_cellNoiseDefaultWarning(true)
{ 
  declareInterface<ICalorimeterNoiseTool>(this);

  declareProperty("CellNoiseFileName",       m_cellNoiseFileName="/afs/cern.ch/user/h/hectbmon/public/tb/aug02/noise/H6_2002NoiseMuons.dat");
  declareProperty("CellNoiseUnits",          m_cellNoiseUnits=MeV);
  declareProperty("CellNoiseDefault",        m_cellNoiseDefault=100*MeV);
  declareProperty("CellNoiseDefaultWarning", m_cellNoiseDefaultWarning=true);
}

SimpleNoiseToolFromTextFile::~SimpleNoiseToolFromTextFile()
{ }

StatusCode SimpleNoiseToolFromTextFile::initialize()
{
  const IGeoModelSvc *geoModel=nullptr;
  ATH_CHECK( service("GeoModelSvc", geoModel) );

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    ATH_CHECK( detStore()->regFcn(&IGeoModelSvc::geoInit,
                                  geoModel,
                                  &SimpleNoiseToolFromTextFile::geoInit,this) );
  }
  return StatusCode::SUCCESS;
}

StatusCode
SimpleNoiseToolFromTextFile::geoInit(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO( "SimpleNoiseToolFromTextFile initialize()" );
  
  // access calo detector managers
  m_caloDDM = CaloDetDescrManager::instance(); 

  // access calo identifier helper
  m_caloIDH = m_caloDDM->getCaloCell_ID();
  
  // open noise file
  std::ifstream fStream(m_cellNoiseFileName.c_str());
  
  // exit if the file cannot be opened
  if (!fStream.is_open()) {
    ATH_MSG_ERROR( "Failed to open file: " << m_cellNoiseFileName );
    return StatusCode::FAILURE;
  }
  
  // read the cell noise from file
  std::string line;
  std::istringstream inStream; 
  while (getline(fStream, line)) {  // read line into line
    if (line[0] != '#') {  // skip line if starting with #
      inStream.clear();
      inStream.str(line);  // use line as input stream
      int offlineID;       // this is theID.get_compact()
      float cellNoise;     // noise units are specified by a the jobOption CellNoiseUnits
      if (inStream >> offlineID >> cellNoise) {  // read in offlineID and cellNoise
        // sync noise units
        cellNoise *= m_cellNoiseUnits;
        // make the Identifier
        Identifier theID(offlineID);
        // fill the noise map
        m_cellNoise[theID] = cellNoise;
	
        ATH_MSG_DEBUG( "Cell " << theID.get_compact()
               << std::setbase(16) << std::setiosflags(std::ios_base::showbase) 
               << " : "    << theID.get_compact()  << std::setbase(10)
               << " : "    << std::setw(17) << m_caloIDH->show_to_string(theID)
               << " : noise = " << cellNoise/MeV << " MeV" 
                       );
      } else {
        ATH_MSG_ERROR("bad format in cell noise file " << m_cellNoiseFileName );
        return StatusCode::FAILURE;
      }
    }
  }
  
  fStream.close();
  
  ATH_MSG_INFO( "SimpleNoiseToolFromTextFile: cell noise values read from file " 
                << m_cellNoiseFileName  );
  
  return StatusCode::SUCCESS;
}

float SimpleNoiseToolFromTextFile::getNoise(const CaloCell* aCell, CalorimeterNoiseType /*type*/)
{  
  Identifier tmp=aCell->ID();
  return getNoiseHelper(tmp);
}

float SimpleNoiseToolFromTextFile::getNoise(const CaloDetDescrElement* caloDDE, CalorimeterNoiseType /*type*/)
{
  Identifier tmp=caloDDE->identify();
  return getNoiseHelper(tmp);
}

float SimpleNoiseToolFromTextFile::getNoiseHelper(Identifier id)
{
  std::map<Identifier, float>::iterator itr = m_cellNoise.find(id);
  if (itr != m_cellNoise.end()) {
    // cell is in the map
    // return the noise
    return itr->second;
  } else {
    // cell not in the map
    // report this if requested
    if (m_cellNoiseDefaultWarning) 
      ATH_MSG_WARNING( "SimpleNoiseToolFromTextFile: default noise " 
             << m_cellNoiseDefault/MeV
             << " MeV for cell " 
             << id.get_compact()
             << std::setbase(16) << std::setiosflags(std::ios_base::showbase) 
             << " : "    << id.get_compact()  << std::setbase(10)
             << " : "    << std::setw(17) << m_caloIDH->show_to_string(id)
                       );
    // return the default noise value set by jobOption
    return m_cellNoiseDefault;
  }
}


/////////////////////////////////////////////////////////////////////////

bool SimpleNoiseToolFromTextFile::isEOverNSigma(const CaloCell* caloCell, float sigmaCut ,
                     CalorimeterNoiseSymmetryHandling symmetryHandling,
                     CalorimeterNoiseType type)
{
  float noise_cut = fabs(sigmaCut*this->getEffectiveSigma(caloCell,  symmetryHandling,type));
  float energy = fabs(caloCell->e());
  return (energy>noise_cut);

}

////////////////////////////////////////////////////////////////////

float SimpleNoiseToolFromTextFile::getRandomisedE(const CaloCell* caloCell , CLHEP::HepRandomEngine* engine, CalorimeterNoiseType type)
{
  double rms = this->getNoise(caloCell,type);
  return RandGauss::shoot(engine,0.,rms);
}

////////////////////////////////////////////////////////////////////

float SimpleNoiseToolFromTextFile::getEffectiveSigma(const CaloCell* caloCell,
                     CalorimeterNoiseSymmetryHandling /*symmetryHandling*/,
                     CalorimeterNoiseType type )
{
     // simple implementation for gaussian noise
     return this->getNoise(caloCell,type);
}

