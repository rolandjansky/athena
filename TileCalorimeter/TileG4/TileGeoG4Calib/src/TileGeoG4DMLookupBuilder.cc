/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class building look-up tables for TileGeoG4CalibSD
//
// Author: Gia Khoriauli <gia.khoriauli@cern.ch>
//
// May, 2005
//
// DESCRIPTION:
//  The additional Look-up builder for Calibration Hit 
//  recording. Must be called after the 'ordinary' Tile 
//  Look-up builder has been already called
//
//************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelUtilities/GeoModelExperiment.h"

//'ordinary' Look-up & its Builder
#include "TileGeoG4SD/TileGeoG4LookupBuilder.hh"
#include "TileGeoG4SD/TileGeoG4Lookup.hh"

//Calibration Look-up & its Builder
#include "TileGeoG4Calib/TileGeoG4DMLookupBuilder.h"
#include "TileGeoG4Calib/TileGeoG4DMLookup.h"

//DB Manager
#include "TileDetDescr/TileDddbManager.h"
#include "TileGeoG4Calib/TileCalibDddbManager.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"


/////////// C O N S T R U C T O R 

TileGeoG4DMLookupBuilder::TileGeoG4DMLookupBuilder(TileGeoG4LookupBuilder* lookup_builder, 
						   IRDBAccessSvc* raccess, 
						   IGeoModelSvc* geo_svc,
                                                   StoreGateSvc* pDetStore,
                                                   IMessageSvc* msgSvc) : 
  rBMin(0),rBMax(0),zBarrMaxPos(0),zBarrMaxNeg(0),
  dzBarrMod(0),dzExtBarrMod(0),zLegngthITC(0),
  dzBarrPeriod(0),dzExtBarrPeriod(0),rGirdMin(0),
  dRFront(0),dZEnd(0),dZEndSh(0),rP1Min(0),rP2Min(0),
  rGapMax(0),rGapMin(0),rCrMax(0),rCrMin(0),
  m_dbManager(0),
  m_lookup_builder(lookup_builder),
  m_sectionMap(0),
  m_tdbManager(0),
  m_msgSvc(msgSvc),
  plateToCell(false)
{
  m_log = new MsgStream (msgSvc, "TileGeoG4DMLookupBuilder");

  const DataHandle<GeoModelExperiment> theExpt;
  StatusCode sc = pDetStore->retrieve( theExpt, "ATLAS" );
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Unable to retrieve GeoModelExperiment from DetectorStore" << endreq;
    abort();
  }

  m_theManager = dynamic_cast<const TileDetDescrManager*>(theExpt->getManager("Tile"));
  if(m_theManager)
    m_tdbManager = m_theManager->getDbManager();
  else
  {
    (*m_log) << MSG::ERROR << "Cannot get TileDetDescrManager" << endreq;
    abort();
  }

  std::string versionTag = (geo_svc->tileVersionOverride()).empty()? geo_svc->atlasVersion() : geo_svc->tileVersionOverride() ;
  std::string versionNode = (geo_svc->tileVersionOverride()).empty()? "ATLAS" : "TileCal";

  //m_dbManager = new TileCalibDddbManager(raccess,"ATLAS-00","ATLAS");
  m_dbManager = new TileCalibDddbManager(raccess, versionTag, versionNode, msgSvc);
}

TileGeoG4DMLookupBuilder::~TileGeoG4DMLookupBuilder()
{
  delete m_dbManager;
  delete m_log;
}

///////////////// B U I L D E R 

void TileGeoG4DMLookupBuilder::BuildLookup(bool is_tb)
{
  // initializations
  m_sectionMap = new TileGeoG4CalibSectionMap();
  
  // Building Section Look-up tables for Calibration Hits
  CreateGeoG4CalibSections(is_tb); 
}



//////////////////// G E T   S E C T I O N 

TileGeoG4CalibSection* TileGeoG4DMLookupBuilder::GetSection(TileCalibDddbManager::TileCalibSections key) const
{
  if (m_sectionMap && m_sectionMap->find(key)!=m_sectionMap->end())
    return m_sectionMap->operator[](key);
  else
    return 0;
}


//////// C E L L   R E S E T 

void TileGeoG4DMLookupBuilder::ResetCells()
{
  TileGeoG4CalibSectionMap::iterator first = m_sectionMap->begin();
  TileGeoG4CalibSectionMap::iterator last  = m_sectionMap->end();

  TileGeoG4CalibSample* _sample;
  TileGeoG4PlateCell*   _pl_cell;
  TileGeoG4GirderCell*  _gr_cell;

  unsigned int l_nSamp, l_nCell;
  unsigned int j;

  for(;first!=last;first++)
  {
    //Reset PlateCells
    for (l_nSamp = 0; l_nSamp < first->second->samples.size(); l_nSamp++)
    {
      _sample = first->second->samples[l_nSamp];
      for (l_nCell = 0; l_nCell < _sample->plateCells.size(); l_nCell++)
      {
        _pl_cell = _sample->plateCells[l_nCell];
        for (j = 0; j < _pl_cell->m_nPlateCellHit.size(); j++) 
	  _pl_cell->m_nPlateCellHit[j] = 0;
        for (j = 0; j < _pl_cell->m_nPlateCellHit_negative.size(); j++) 
	  _pl_cell->m_nPlateCellHit_negative[j] = 0;
      }
    }
    //Reset GirderCells
    for (l_nCell = 0; l_nCell < first->second->girderCells.size(); l_nCell++)
    {
      _gr_cell = first->second->girderCells[l_nCell];
      for (j = 0; j < _gr_cell->m_nGirderCellHit.size(); j++) 
	_gr_cell->m_nGirderCellHit[j] = 0;
      for (j = 0; j < _gr_cell->m_nGirderCellHit_negative.size(); j++) 
	_gr_cell->m_nGirderCellHit_negative[j] = 0;
    }  
  }
}


bool TileGeoG4DMLookupBuilder::GetPlateToCell()
{
return plateToCell;
}

//////////////// C R E A T E    S E C T I O N S

void TileGeoG4DMLookupBuilder::CreateGeoG4CalibSections(bool is_tb) 
{
  if(!m_dbManager || !m_sectionMap)
  {
    (*m_log) << MSG::ERROR
             << "CreateGeoG4CalibSections() - Initialization failed"
             << "  DbManager = " << m_dbManager << "  SectionMap = " << m_sectionMap << endreq;
    abort();
  }
  
  //_plateToCell parameter
  plateToCell = m_dbManager-> GetSwitchPlateToCell();
  //geometry

  m_tdbManager->SetCurrentEnvByType(1);
  rBMin=m_tdbManager->GetEnvRin() * CLHEP::cm;
  rBMax=m_tdbManager->GetEnvRout() * CLHEP::cm;
  zBarrMaxPos = (m_tdbManager->GetEnvZLength()/2.0) * CLHEP::cm;
  zBarrMaxNeg = ((-1)*m_tdbManager->GetEnvZLength()/2.0) * CLHEP::cm;

  for (int ii=m_tdbManager->GetNumberOfEnv()-1; ii>=0; --ii) {
    m_tdbManager->SetCurrentEnvByIndex(ii);
    int env_type = m_tdbManager->GetEnvType();
    if (env_type == 5) {
      zBarrMaxPos += m_tdbManager->GetEnvDZ() * CLHEP::cm;
      zLegngthITC = m_tdbManager->GetEnvZLength() * CLHEP::cm;
    }
    else if (env_type == 4) {
      zBarrMaxNeg += m_tdbManager->GetEnvDZ() * CLHEP::cm;
      zLegngthITC = m_tdbManager->GetEnvZLength() * CLHEP::cm;
    }
  }
  
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
  rGirdMin = m_tdbManager->TILBrmax() * CLHEP::cm;
  dRFront = m_tdbManager->TILBdrfront() * CLHEP::cm;
  dZEnd = m_tdbManager->TILBdzend() * CLHEP::cm;
  dzBarrMod = m_tdbManager->TILBdzmodul() * CLHEP::cm;
  dzBarrPeriod = (dzBarrMod-2*dZEnd+m_tdbManager->TILBdzmast() * CLHEP::cm)/m_tdbManager->TILBnperiod();
  
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);
  dzExtBarrMod = m_tdbManager->TILBdzmodul() * CLHEP::cm;
  dZEndSh = m_tdbManager->TILBdzend1() * CLHEP::cm;
  dzExtBarrPeriod = (dzExtBarrMod - dZEnd - dZEndSh)/m_tdbManager->TILBnperiod();
  
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
  rP1Min = m_tdbManager->TILBrmin() * CLHEP::cm;
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);  
  rP2Min = m_tdbManager->TILBrmin() * CLHEP::cm;
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3); 
  rGapMin = m_tdbManager->TILBrmin() * CLHEP::cm;
  rGapMax = m_tdbManager->TILBrmax() * CLHEP::cm;
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4); 
  rCrMin = m_tdbManager->TILBrmin() * CLHEP::cm;
  rCrMax = m_tdbManager->TILBrmax() * CLHEP::cm;
  //

  TileGeoG4CalibSection* _section;
  TileGeoG4CalibSample*  _sample;
  TileGeoG4CalibCell*    _cell;
  TileGeoG4PlateCell*    _pl_cell;
  TileGeoG4GirderCell*   _gr_cell;
  
  TileCalibDddbManager::TileCalibSections key;
  TileDddbManager::TileSections v_key;

  int j, counter, sampleNum;
  
  //Define number of Sections & Modules due to the selected detector geometry

  int nSections = (is_tb) ? 4 : 6;
  if (m_tdbManager->GetNumberOfEnv() == 1) nSections = 1;

  int nModules = (is_tb) ? 3 : 64;
  for (int ii=m_tdbManager->GetNumberOfEnv()-1; ii>=0; --ii) {
    m_tdbManager->SetCurrentEnvByIndex(ii);
    nModules = std::max(nModules,m_tdbManager->GetEnvNModules());
  }

  //Loop over the Tile sections
  for(counter=1; counter<=nSections; counter++)   
  {

    //Retrieve current Section from DB
    key = TileCalibDddbManager::TileCalibSections(counter);
    m_dbManager->SetCurrentSection(key);
    
    //Take a pointer to the "ordinary" Tile Section 
    //Needed to retrieve Cells from "ordinary" Look-up
    v_key = TileDddbManager::TileSections(counter);
    TileGeoG4Section* tile_section = m_lookup_builder->GetSection(v_key); 

    //Create & initialize Section
    _section = new TileGeoG4CalibSection(m_msgSvc);
    _section->section         = static_cast<int>(m_dbManager->GetCurrentSection());
    _section->nrOfPeriods     = static_cast<int>(m_dbManager->GetNumSectPeriods());
    _section->nrOfSamples     = static_cast<int>(m_dbManager->GetNumSectSamples());
    _section->nrOfCells       = static_cast<int>(m_dbManager->GetNumSectCells());
    _section->nrOfPlateCells  = static_cast<int>(m_dbManager->GetNumSectPlateCells());
    _section->nrOfGirderCells = static_cast<int>(m_dbManager->GetNumSectGirderCells());
    for(int i=0; i<4; i++) 
    {
      if((i==3)&&(plateToCell))
      {
	_section->sample_ZBound[i] = 9999.9 * CLHEP::cm;
      }
      else
      {
        _section->sample_ZBound[i] = static_cast<double>(m_dbManager->GetSampleZBound(i)) * CLHEP::cm;
      }
    }


    //------------------------------------------ C E L L ------------------------------------------

    m_dbManager->SetFirstDetCell(_section->section);

    //Make first Cell of the current Section
    _cell = new TileGeoG4CalibCell();
    _cell->sample = static_cast<int>(m_dbManager->GetCellSample());
    _cell->detector = static_cast<int>(m_dbManager->GetCellDetector());
    _cell->nrOfPeriodsInCell[0] = static_cast<int>(m_dbManager->GetNumOfPeriodsInCell(0));
    _cell->nrOfPeriodsInCell[1] = static_cast<int>(m_dbManager->GetNumOfPeriodsInCell(1));


    //Create Section first Sample & Store the first Cell of current Section
    _sample = new TileGeoG4CalibSample(); 
    _sample->cells.push_back(_cell);

    sampleNum = _cell->sample;


    //Loop over the other Section Cells in DB and store them into Samples
    while(m_dbManager->SetNextDetCell())  {

      //Create & initialize Cell
      _cell = new TileGeoG4CalibCell();
      _cell->sample = static_cast<int>(m_dbManager->GetCellSample());
      _cell->detector = static_cast<int>(m_dbManager->GetCellDetector());
      _cell->nrOfPeriodsInCell[0] = static_cast<int>(m_dbManager->GetNumOfPeriodsInCell(0));
      _cell->nrOfPeriodsInCell[1] = static_cast<int>(m_dbManager->GetNumOfPeriodsInCell(1));
     
      //Check the Cell is it from the next Sample or not
      if(sampleNum!=_cell->sample) {
	
	//Store the current Sample & Create the new one
	_section->samples.push_back(_sample);
	_sample = NULL;
	_sample = new TileGeoG4CalibSample();	

	sampleNum = _cell->sample;
      }
      
      //Store a Cell into Sample 
      _sample->cells.push_back(_cell);

    }
    //Don't forget store last Sample
    _section->samples.push_back(_sample);   


    //check cells2sample structure
    if (m_log->level()<=MSG::VERBOSE) {
      for(int samp=0;  samp<static_cast<int>(_section->samples.size());  samp++) {
    	for(int cell=0;  cell<static_cast<int>(_section->samples[samp]->cells.size());  cell++) {
	    (*m_log) << MSG::VERBOSE
                     <<"samp="<<samp<<"  cell_ind="<<cell
		     <<" :  nrOfPeriodsInCell[0]= "<<_section->samples[samp]->cells[cell]->nrOfPeriodsInCell[0]
		     <<" ,  nrOfPeriodsInCell[1] = "<<_section->samples[samp]->cells[cell]->nrOfPeriodsInCell[1]
                     <<" ,  detector = "<<_section->samples[samp]->cells[cell]->detector
	             <<" ,  sample = "<<_section->samples[samp]->cells[cell]->sample
		     <<endreq;
	}
      }
    }
    
    //Build a corespondence vector 
    //between Cell and Sample/Period
    if((key != TileCalibDddbManager::TILE_PLUG3) && (key != TileCalibDddbManager::TILE_PLUG4))
      _section->DMToCell(false, tile_section);
    else {
      _section->DMToCell(true, tile_section);
    }



    //----------------------------------- P L A T E   C E L L ------------------------------------

    
    //Clean up Samples plateCell vectors. ...for any case
    for(int samp=0;  samp<static_cast<int>(_section->samples.size());  samp++) 
    {    
      for(int pcell=0;  pcell<static_cast<int>(_section->samples[samp]->plateCells.size());  pcell++) 
	                                                             delete _section->samples[samp]->plateCells[pcell];
      _section->samples[samp]->plateCells.clear();
    }


    //Set current PlateCell
    if(m_dbManager->SetFirstDetPlateCell(_section->section)) {
      
      //Make first PlateCell of the current Section
      _pl_cell = new TileGeoG4PlateCell();
      _pl_cell->detector = static_cast<int>(m_dbManager->GetPlateCellDetector());
      _pl_cell->tower    = static_cast<int>(m_dbManager->GetPlateCellTower());
      _pl_cell->sample   = static_cast<int>(m_dbManager->GetPlateCellSample());
      _pl_cell->eta      = static_cast<double>(m_dbManager->GetPlateCellEta());
      _pl_cell->dEta     = static_cast<double>(m_dbManager->GetPlateCellDeta());
      _pl_cell->xBound   = static_cast<double>(m_dbManager->GetPlateCellXBound()) * CLHEP::cm;
      _pl_cell->zBound   = static_cast<double>(m_dbManager->GetPlateCellZBound()) * CLHEP::cm;
      _pl_cell->neighbor = static_cast<int>(m_dbManager->GetPlateCellNeighborPeriod());
      
      //Samples are already created
      _sample=_section->samples[0];   
      _sample->plateCells.push_back(_pl_cell);
      
      sampleNum=_pl_cell->sample;   
      
      //Sample iterator
      unsigned int n_samp=0;

      
      // iterate over the PlateCells & store them into Samples
      while(m_dbManager->SetNextDetPlateCell()) {
	
	//Create & initialize PlateCell
	_pl_cell = new TileGeoG4PlateCell();
	_pl_cell->detector = static_cast<int>(m_dbManager->GetPlateCellDetector());
	_pl_cell->tower    = static_cast<int>(m_dbManager->GetPlateCellTower());
	_pl_cell->sample   = static_cast<int>(m_dbManager->GetPlateCellSample());
	_pl_cell->eta      = static_cast<double>(m_dbManager->GetPlateCellEta());
	_pl_cell->dEta     = static_cast<double>(m_dbManager->GetPlateCellDeta());
	_pl_cell->xBound   = static_cast<double>(m_dbManager->GetPlateCellXBound()) * CLHEP::cm;
	_pl_cell->zBound   = static_cast<double>(m_dbManager->GetPlateCellZBound()) * CLHEP::cm;
	_pl_cell->neighbor = static_cast<int>(m_dbManager->GetPlateCellNeighborPeriod());

	//Check the PlateCell is it from the next Sample or not
	if(sampleNum!=_pl_cell->sample)
	{
	  //Take the next Sample. If it is necessary create more Samples
	  sampleNum = _pl_cell->sample;
	  if(++n_samp < _section->samples.size())
	  {
	    //The 'old' Sample should be used
	    //Protection against messages from Coverity tool ?
	    _sample = NULL; 
	    _sample = _section->samples[n_samp];
	  }
	  else
	  {
	    //If new Sample was already created then store it in Section
	    if(n_samp > _section->samples.size())   _section->samples.push_back(_sample);
	    //The new Sample is necessary.
	    //Protection against messages from Coverity tool ?
	    _sample = NULL;
	    _sample = new TileGeoG4CalibSample();
	  }
	}
	
	//Store a PlateCell into Sample 
	_sample->plateCells.push_back(_pl_cell);
      }
      
      //Don't forget store last Sample if it was created as a new Sample
      if(++n_samp > _section->samples.size())   _section->samples.push_back(_sample);
    }

	
    // check plateCells2sample structure
    if (m_log->level()<=MSG::VERBOSE) {
      for(int psamp=0;  psamp<static_cast<int>(_section->samples.size());  psamp++) {
        for(int pcell=0;  pcell<static_cast<int>(_section->samples[psamp]->plateCells.size());  pcell++) {
	    (*m_log) << MSG::VERBOSE
                     <<"psamp="<<psamp<<"  cell_ind="<<pcell
        	     <<" :  detector   = "<<_section->samples[psamp]->plateCells[pcell]->detector
            	     <<" ,  tower    = "<<_section->samples[psamp]->plateCells[pcell]->tower
            	     <<" ,  sample = "<<_section->samples[psamp]->plateCells[pcell]->sample
            	     <<endreq;
	}
      }
    }

    //---------------------------------- G I R D E R   C E L L -------------------------------------
    //
    // comment: there are the Barrel & Extended Fingers jointed to Girders
    
    //Clean up Section girderCell vector. ...for any case 
    for(int gcell=0; gcell<static_cast<int>(_section->girderCells.size()); gcell++) 
                                                                delete _section->girderCells[gcell];
    _section->girderCells.clear();
    

    if(m_dbManager->SetFirstDetGirderCell(_section->section)) {

      do { 

	//Create & initialize GirderCell
	_gr_cell = new TileGeoG4GirderCell();
	_gr_cell->detector = static_cast<int>(m_dbManager->GetGirderCellDetector());
	_gr_cell->tower    = static_cast<int>(m_dbManager->GetGirderCellTower());
	_gr_cell->sample   = static_cast<int>(m_dbManager->GetGirderCellSample());
	_gr_cell->eta      = static_cast<double>(m_dbManager->GetGirderCellEta());
	_gr_cell->dEta     = static_cast<double>(m_dbManager->GetGirderCellDeta());
	_gr_cell->xBound   = static_cast<double>(m_dbManager->GetGirderCellXBound()) * CLHEP::cm;
	
	_section->girderCells.push_back(_gr_cell);

      } while(m_dbManager->SetNextDetGirderCell()) ;

    }
    

    // check girder cells in the section
    if (m_log->level()<=MSG::VERBOSE) {
      for(int gcell=0;  gcell<static_cast<int>(_section->girderCells.size());  gcell++) {
        (*m_log) << MSG::VERBOSE
                 <<"cell_ind="<<gcell
                 <<" :  detector   = "<<_section->girderCells[gcell]->detector
                 <<" ,  tower    = "<<_section->girderCells[gcell]->tower
                 <<" ,  sample = "<<_section->girderCells[gcell]->sample
                 <<endreq;
      }
    }

     
    //---------------------- M O D U L E S --------------------

    // Initialize ModuleToCell values
    for (j = 0; j < nModules; j++)
    {
      _section->AddModuleToCell(false);
      if(key != TileCalibDddbManager::TILE_BARREL) {   
        _section->AddModuleToCell(true); 
      }	
    }
	

    //Store Section in map
    m_sectionMap->operator[](key) = _section;

  } //End of Section loop
}  //END

