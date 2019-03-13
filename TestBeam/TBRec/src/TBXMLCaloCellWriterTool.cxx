/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TBXMLWriterToolBase.h"
#include "TBXMLCaloCellWriterTool.h"
#include "TBXMLWriter.h"

#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "boost/io/ios_state.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>

#include <vector>
#include <string>
#include <sstream>
#include <map>

#include <cmath>


using CLHEP::deg;


unsigned int TBXMLCaloCellWriterTool::m_nCols = 2;

TBXMLCaloCellWriterTool::TBXMLCaloCellWriterTool(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
  : TBXMLWriterToolBase(type,name,parent)
    , m_etaMin(-5.0)
    , m_etaMax(5.0)
    , m_phiMin(0.*deg)
    , m_phiMax(360.*deg)
    , m_firstEvent(true)
    , m_idHelper(0)
{
  declareProperty("InputCellContainer", m_cellContainer);
  declareProperty("IncludedCalos",      m_includedCalos);
  declareProperty("IncludedSamplings",  m_includedSamplings);
  declareProperty("EtaMin",             m_etaMin);
  declareProperty("EtaMax",             m_etaMax);
  declareProperty("PhiMin",             m_phiMin);
  declareProperty("PhiMax",             m_phiMax);
  // save pointer to algorithm
  m_mother = dynamic_cast<const TBXMLWriter*>(parent);
}

TBXMLCaloCellWriterTool::~TBXMLCaloCellWriterTool()
{ }

//////////////////
// Event Writer //
//////////////////

StatusCode
TBXMLCaloCellWriterTool::writeEvent(std::ostream& outStream,
				    const std::string& /* entryTag */)
{
  // messaging
  MsgStream log(msgSvc(),name());

  ///////////////////////////////
  // On the Fly Initialization //
  ///////////////////////////////

  if ( m_firstEvent ) 
    {
      // check pointer
      if ( m_mother == 0 )
	{
	  log << MSG::ERROR
	      << "tool does not hang off the TBXMLWriter algorithm."
	      << endmsg;
	  return StatusCode::FAILURE;
	}

      // convert poperties
      StatusCode checkOut = this->convertProperties();
      if ( checkOut.isFailure() )
	{
	  log << MSG::ERROR
	      << "cannot convert properties correctly!"
	      << endmsg;
	  return StatusCode::FAILURE;
	}
      m_firstEvent = false;

      // print out configuration
      log << MSG::INFO
	  << "list of included calo samplings:"
	  << endmsg;
      for ( unsigned int i=0; i<m_caloSamplings.size(); i++ )
	{
	  log << MSG::INFO
	      << "Sampling \042"
	      << m_includedSamplings[i]
	      << "\042 with SubCalo index "
	      << (int)m_caloSamplings[i]
	      << endmsg;
	}
    }

  ////////////////////////
  // Check On Begin Run //
  ////////////////////////

  if ( m_mother->handleBeginRun() )
    {
      StatusCode checkOut =  this->writeRunFiles(m_mother->getFileDir(),
						 m_mother->getRunNumber());
      if ( checkOut.isFailure() )
	{
	  log << MSG::ERROR
	      << "cannot produce run XML files."
	      << endmsg;
	  return StatusCode::FAILURE;
	}
    }


  //////////////////////////////////
  // Retrieve Data from StoreGate //
  //////////////////////////////////

  const CaloCellContainer* theContainer = 0;
  ATH_CHECK( evtStore()->retrieve(theContainer,m_cellContainer) );

  //////////////////////////////////
  // Loop Individual Calorimeters //
  //////////////////////////////////

  for ( unsigned int i=0; i<m_caloIndices.size(); i++ )
    {
      // write container tag
      outStream << "<!-- TBXMLCaloCellWriterTool: begin write cell container "
		<< m_includedCalos[i] << " -->" << std::endl;

      std::vector<std::string> theCellAttrNames;
      theCellAttrNames.push_back("name");
      theCellAttrNames.push_back("ncols");
      theCellAttrNames.push_back("nrows");
      theCellAttrNames.push_back("idtype");
      theCellAttrNames.push_back("etype");
      std::vector<std::string> theCellAttrValues;
      theCellAttrValues.push_back(m_includedCalos[i]);
      std::ostringstream theCols; 
      theCols << m_nCols << std::ends;

      // get data size
      unsigned int dataSize = theContainer->nCellsCalo(m_caloIndices[i]);
      unsigned int nRows = 
	(unsigned int)ceil( (double)dataSize / (double)m_nCols );
      std::ostringstream theRows;
      theRows << nRows << std::ends;
      theCellAttrValues.push_back(theCols.str());
      theCellAttrValues.push_back(theRows.str());
      theCellAttrValues.push_back("uint");
      theCellAttrValues.push_back("double");
      this->openElement(outStream,"CaloCellContainer",
			theCellAttrNames,theCellAttrValues);

      // loop on cells
      CaloCellContainer::const_iterator firstCell = 
	theContainer->beginConstCalo(m_caloIndices[i]);
      CaloCellContainer::const_iterator lastCell =
	theContainer->endConstCalo(m_caloIndices[i]);

      // baseline index
      IdentifierHash baseIndex, lastIndex;
      m_idHelper->calo_cell_hash_range((int)m_caloIndices[i],
				       baseIndex, lastIndex);

      unsigned int theCtr = 0;
      bool isTerminated = false;
      for ( ; firstCell != lastCell; firstCell++ )
	{  
	  // check if cell in requested layer and range
	  double eta = (*firstCell)->eta();
	  double phi = (*firstCell)->phi();
          const CaloDetDescrElement * theCaloDDE= (*firstCell)->caloDDE();
	  CaloSampling::CaloSample theLayer;
          if (theCaloDDE) {
              theLayer = (CaloSampling::CaloSample) theCaloDDE->getSampling();
          }else{
              theLayer =  CaloSampling::Unknown;
          } 
	  if ( ( std::find(m_caloSamplings.begin(),
			   m_caloSamplings.end(),
			   theLayer) != m_caloSamplings.end() ) &&
	       ( ( eta > m_etaMin && eta < m_etaMax ) &&
		 ( phi > m_phiMin && phi < m_phiMax ) )
	       )
	    {  	      
	      // get calorimeter hash index
	      IdentifierHash theIndex = 
		(*firstCell)->caloDDE()->calo_hash();
	      // get eta, phi, region
	      int etaIndex = m_idHelper->eta((*firstCell)->ID());
	      int phiIndex = m_idHelper->phi((*firstCell)->ID());
	      int regIndex = m_idHelper->region((*firstCell)->ID());
	      // int subIndex = m_idHelper->sub_calo((*firstCell)->ID());
	      int lyrIndex = m_idHelper->sampling((*firstCell)->ID());
	      int subIndex = -1;
	      // get subIndex
	      if ( m_idHelper->is_em_barrel((*firstCell)->ID()) )
		{
		  subIndex = 0;
		}
	      else if ( m_idHelper->is_em_endcap((*firstCell)->ID()) )
		{
		  subIndex = 1;
		}
	      else if ( m_idHelper->is_tile_barrel((*firstCell)->ID()) )
		{
		  subIndex = 2;
		}
	      else if ( m_idHelper->is_tile_extbarrel((*firstCell)->ID()) )
		{
		  subIndex = 3;
		}
	      else if ( m_idHelper->is_hec((*firstCell)->ID()) )
		{
		  subIndex = 4;
		}
	      else if ( m_idHelper->is_fcal((*firstCell)->ID()) )
		{
		  subIndex = 5;
		}
	      // get signal
	      double theEnergy     = (*firstCell)->e();
	      if ( theCtr == 0 || ( theCtr % m_nCols ) == 0 )
		{
		  outStream << "     ";
		}
	      theIndex -= (int)baseIndex;
              boost::io::ios_base_all_saver streamsave (outStream);
	      outStream 
		<< std::setw(10) << std::setfill(' ') << theIndex << " "
		<< std::setw(3)  << std::setfill(' ') << subIndex << " "
		<< std::setw(3)  << std::setfill(' ') << lyrIndex << " "
		<< std::setw(3)  << std::setfill(' ') << regIndex << " "
		<< std::setw(3)  << std::setfill(' ') << etaIndex << " "
		<< std::setw(3)  << std::setfill(' ') << phiIndex << " "
		<< std::setw(10) << std::setprecision(5) << theEnergy;
	      theCtr++;
	      if ( (isTerminated = ( theCtr % m_nCols )) == 0 )
		{
		  outStream << std::endl;
		}
	    }
	}
      if ( ! isTerminated ) outStream << std::endl;
      this->closeElement(outStream);
      // write container tag
      outStream << "<!-- TBXMLCaloCellWriterTool: end write cell container "
		<< m_includedCalos[i] << " -->" << std::endl;
    }
  
  return StatusCode::SUCCESS;
}

/////////////////////
// Run File Writer //
/////////////////////

StatusCode
TBXMLCaloCellWriterTool::writeRunFiles(const std::string& fileDir,
				       unsigned int runNumber)
{

  //////////////////////
  // Write Dictionary //
  //////////////////////

  // construct directory and file name
  std::ostringstream thisFileName;
  thisFileName << fileDir << "/geom." 
	       << std::setw(6) << std::setfill('0')
	       << runNumber << ".xml" << std::ends;

  std::ofstream thisFileStream((thisFileName.str()).c_str());

  // Document type
  std::vector<std::string> theRunElements;
  theRunElements.push_back("FCalGeomTable*");
  this->openDictionary(thisFileStream,"FCalGeomTable",theRunElements);
  std::vector<std::string> theRunAttr;
  theRunAttr.push_back("name     CDATA                           #REQUIRED");
  this->addAttributes(thisFileStream,"FCalGeomTable",theRunAttr);

  // FCal geometry table
  std::vector<std::string> anyData;
  anyData.push_back("ANY");
  this->addDictElement(thisFileStream,"FCalGeomTable",anyData);
  std::vector<std::string> theGeomAttr;
  theGeomAttr.push_back("name    CDATA                           #REQUIRED");
  this->addAttributes(thisFileStream,"FCalGeomTable",theGeomAttr);

  // close dictionary
  this->closeDictionary(thisFileStream);

  ////////////////////////
  // Write Run Geometry //
  ////////////////////////

  const CaloDetDescrManager* caloDetMgr = nullptr;
  ATH_CHECK( detStore()->retrieve (caloDetMgr, "CaloMgr") );

  IdentifierHash firstIndex, lastIndex, safeIndex;
  m_idHelper->calo_cell_hash_range((int)CaloCell_ID::LARFCAL,
				   firstIndex, lastIndex);
  safeIndex = firstIndex;

  // open element 
  std::vector<std::string> theRunValues(theRunAttr.size());
  for ( unsigned int i=0; i<theRunAttr.size(); i++ )
    {
      if ( (theRunAttr[i]).find("name") != std::string::npos )
	{
	  theRunValues[i] = "FCalMod0";
	  theRunAttr[i] = 
	    theRunAttr[i].substr(0,theRunAttr[i].find_first_of(" "));
	}
    }
  this->openElement(thisFileStream,"FCalGeomTable",theRunAttr,theRunValues);
  theRunValues[0] = "FCalCells";
  this->openElement(thisFileStream,"FCalGeomTable",theRunAttr,theRunValues);
  thisFileStream << "<!-- 32-bit Id"
		 << "   Module#  "
		 << "   EtaIndex "
		 << "   PhiIndex "
		 << "   X [cm]   "
		 << "   Y [cm]   "
		 << "   Z [cm]   "
		 << "  TileSize  "
		 << "-->" << std::endl;

  // find big/small tile indicator -> to be replaced
  std::map<int,double> smallestDx;
  std::map<int,double> largestDx;
  for ( unsigned int iCtr = (unsigned int)safeIndex; 
	iCtr <= (unsigned int)lastIndex; iCtr++ )
    {
      IdentifierHash theIndex(iCtr);
      Identifier theId = m_idHelper->cell_id(theIndex);
      int theCalo      = m_idHelper->sub_calo(theId);
      const CaloDetDescrElement* theElement = 
	caloDetMgr->get_element(theIndex);
      double dx        = theElement->dx();
      // check tile size
      if ( smallestDx.find(theCalo) == smallestDx.end() )
	{
	  smallestDx[theCalo] = dx;
	  largestDx[theCalo]  = dx;
	}
      else
	{
	  if ( dx < smallestDx[theCalo] ) smallestDx[theCalo] = dx;
	  if ( dx > largestDx[theCalo] )  largestDx[theCalo]  = dx;
	}
    }

  // get geometry
  for (unsigned int iCtr=(unsigned int)firstIndex; 
       iCtr<=(unsigned int)lastIndex; iCtr++ )
    {
      IdentifierHash theIndex(iCtr);
      // get identifiers
      Identifier theId = m_idHelper->cell_id(theIndex);
      int theModule    = m_idHelper->sampling(theId);
      int theEta       = m_idHelper->eta(theId);
      int thePhi       = m_idHelper->phi(theId);
      int theCalo      = m_idHelper->sub_calo(theId);
      // get geometry
      const CaloDetDescrElement* theElement = 
	caloDetMgr->get_element(theIndex);
      double theX      = theElement->x();
      double theY      = theElement->y();
      double theZ      = theElement->z();
      // check tile size
      double dx        = theElement->dx();
      int tileSize     = dx > 1.1*smallestDx[theCalo] ? 1 : 0;

      // write out
      thisFileStream << "    "
		     << std::setw(10) 
		     << std::setfill(' ') << iCtr-(unsigned int)firstIndex
		     << "     "
		     << std::setw(2)  << std::setfill(' ') << theModule 
		     << "          "
		     << std::setw(2)  << std::setfill(' ') << theEta    
		     << "          "
		     << std::setw(2)  << std::setfill(' ') << thePhi    
		     << "   "
		     << std::setw(10) << std::setprecision(5) << theX << "  "
		     << std::setw(10) << std::setprecision(5) << theY << "  "
		     << std::setw(10) << std::setprecision(5) << theZ << "  "
		     << "       "
		     << std::setw(2)  << std::setfill(' ')    << tileSize 
		     << std::endl;
	}
  this->finalize(thisFileStream);	
  thisFileStream.close();

  return StatusCode::SUCCESS;
}

StatusCode
TBXMLCaloCellWriterTool::convertProperties()
{
  // messaging
  MsgStream log(msgSvc(),name());

  // get calo id helper
  ATH_CHECK( detStore()->retrieve (m_idHelper, "CaloCell_ID") );

  //////////////////////
  // Get Calo Indices //
  //////////////////////

  std::vector<std::string>::iterator firstCalo = m_includedCalos.begin();
  std::vector<std::string>::iterator lastCalo  = m_includedCalos.end();
  while ( firstCalo != lastCalo )
    {
      if ( *firstCalo == "LAREM" )
	{
	  m_caloIndices.push_back(CaloCell_ID::LAREM);
	  firstCalo++;
	}
      else if ( *firstCalo == "LARHEC" )
	{
	  m_caloIndices.push_back(CaloCell_ID::LARHEC);
	  firstCalo++;
	}
      else if ( *firstCalo == "LARFCAL" )
	{
	  m_caloIndices.push_back(CaloCell_ID::LARFCAL);
	  firstCalo++;
	}
      else if ( *firstCalo == "TILE" )
	{
	  m_caloIndices.push_back(CaloCell_ID::TILE);
	  firstCalo++;
	}
      else
	{
	  firstCalo = m_includedCalos.erase(firstCalo);
	}
    }

  ///////////////////////
  // Get CaloSamplings //
  ///////////////////////

  std::vector<std::string>::iterator firstSampl = m_includedSamplings.begin();
  std::vector<std::string>::iterator lastSampl  = m_includedSamplings.end();

  // nasty (no switch on string types...)
  for ( ; firstSampl != lastSampl; firstSampl++ )
    {
      // H8 calos
      if ( *firstSampl == "PreSamplerB" ) 
	m_caloSamplings.push_back(CaloSampling::PreSamplerB);
      if ( *firstSampl == "EMB0" )
	m_caloSamplings.push_back(CaloSampling::EMB1);
      if ( *firstSampl == "EMB1" )
	m_caloSamplings.push_back(CaloSampling::EMB2);
      if ( *firstSampl == "EMB2" )
	m_caloSamplings.push_back(CaloSampling::EMB3);
      if ( *firstSampl == "TileBar0" )
	m_caloSamplings.push_back(CaloSampling::TileBar0);
      if ( *firstSampl == "TileBar1" )
	m_caloSamplings.push_back(CaloSampling::TileBar0);
      if ( *firstSampl == "TileBar2" )
	m_caloSamplings.push_back(CaloSampling::TileBar0);
      if ( *firstSampl == "TileExt0" )
	m_caloSamplings.push_back(CaloSampling::TileExt0);
      if ( *firstSampl == "TileExt1" )
	m_caloSamplings.push_back(CaloSampling::TileExt1);
      if ( *firstSampl == "TileExt2" )
	m_caloSamplings.push_back(CaloSampling::TileExt2);

      // H6 calos
      if ( *firstSampl == "EME1" )
	m_caloSamplings.push_back(CaloSampling::EME1);
      if ( *firstSampl == "EME2" )
	m_caloSamplings.push_back(CaloSampling::EME2);
      if ( *firstSampl == "HEC0" )
	m_caloSamplings.push_back(CaloSampling::HEC0);
      if ( *firstSampl == "HEC1" )
	m_caloSamplings.push_back(CaloSampling::HEC1);
      if ( *firstSampl == "HEC2" )
	m_caloSamplings.push_back(CaloSampling::HEC2);
      if ( *firstSampl == "HEC3" )
	m_caloSamplings.push_back(CaloSampling::HEC3);
      if ( *firstSampl == "FCAL0" )
	m_caloSamplings.push_back(CaloSampling::FCAL0);
      if ( *firstSampl == "FCAL1" )
	m_caloSamplings.push_back(CaloSampling::FCAL1);
      if ( *firstSampl == "FCAL2" )
	m_caloSamplings.push_back(CaloSampling::FCAL2);
    }

  return m_caloSamplings.size() == 0 && m_caloIndices.size() == 0
    ? StatusCode::FAILURE
    : StatusCode::SUCCESS;
}      

