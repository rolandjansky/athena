/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          DumpTriggerObjects.cxx  -  description
                             -------------------
    begin                : Mon Apr 23 2001
    copyright            : (C) 2000 by moyse
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/

// Utilities
#include <cmath>
#include <iomanip>
#include <vector>

// This algorithm includes
#include "TrigT1CaloSim/DumpTriggerObjects.h"

#include "TrigT1CaloSim/JetTrigger.h"
//#include "TrigT1Calo/JetElement.h"

#include "TrigT1Interfaces/CoordinateRange.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "TrigT1CaloEvent/JetElement_ClassDEF.h"

namespace LVL1{

//--------------------------------
// Constructors and destructors
//--------------------------------

DumpTriggerObjects::DumpTriggerObjects
  (const std::string& name, ISvcLocator* pSvcLocator)
    : Algorithm(name, pSvcLocator), 
      m_storedTTs(0),
      jetElements(0),
      m_storeGate("StoreGateSvc", name),
      EMData(0), HData(0),
      PhiMax(0), EtaMax(0)
{
    
  m_ttContainer = 0; m_jetElementContainer = 0;
  m_jetElementLocation = TrigT1CaloDefs::JetElementLocation ;
  m_triggerTowerLocation = TrigT1CaloDefs::TriggerTowerLocation;
  declareProperty("JetElementLocation", m_jetElementLocation);
  declareProperty("TriggerTowerLocation", m_triggerTowerLocation);
  declareProperty("EventStore",m_storeGate,"StoreGate Service");

    m_oFile = new std::ofstream("Lvl1TestVector.dat");
        

}

// Destructor
DumpTriggerObjects::~DumpTriggerObjects()
{
    MsgStream log(messageService(), name());
    log << MSG::INFO << "Destructor called" << endreq;
    
    m_oFile->close();
    delete m_ttContainer;
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode DumpTriggerObjects::initialize()
{

  // We must here instantiate items which can only be made after
  // any job options have been set
  MsgStream log(messageService(), name()) ;

  // This isn't good code - very poor error checking!
  StatusCode sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName() 
        << endreq;
  } else {
    log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName() 
        << endreq;
  }
  
  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise()
//---------------------------------

StatusCode DumpTriggerObjects::finalize()
{
    MsgStream log(messageService(), name()) ;
    log << MSG::INFO << "Finalizing" << endreq;
    return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode DumpTriggerObjects::execute( )
{
  // make a message logging stream

    MsgStream log(messageService(), name()) ;
    log << MSG::DEBUG << "Executing" << endreq;

    //Create a map to hold the towers
    m_jetElementContainer = new std::map<int, JetElement*>; 
    
    //Create a map to hold the towers         
    m_ttContainer = new std::map<int, TriggerTower*>;
  
    loadTriggerTowers();

    delete m_jetElementContainer;
    delete m_ttContainer;

    m_jetElementContainer = 0;
    m_ttContainer = 0;
  
    return StatusCode::SUCCESS;
}


/**
 *  Creates two files of data - JemInputMain_Athena.txt and HighestTriggerTowers.data - that contain
 *  print outs of the block with the highest amount of energy per event.  The basic method creates 
 *  a two dimensional array based on the values EtaMax and PhiMax and then places the appropriate
 *  energy values within this array.  Finally a separate method iterates over the array, breaks it up
 *  into smaller blocks (4x8 for the JetElement data, 4x4 for the TriggerTowers) and prints out the 
 *  block with the most energy.
 *
 *  NOTE:  All output files are APPENDED and NOT OVERWRITTEN.  The user should take care to remove 
 *  any unwanted files between subsequent runs.
 *
 * @RETURNs		Status code
 */

StatusCode DumpTriggerObjects::loadTriggerTowers()
{
    MsgStream log(messageService(), name());
    // Find trigger towers in TES
    StatusCode sc1 = m_storeGate->retrieve(m_storedTTs, m_triggerTowerLocation);

    // Find jet elements in TES
    StatusCode sc2 = m_storeGate->retrieve(jetElements, m_jetElementLocation);

    if((sc1 == StatusCode::FAILURE))
    {
        log << MSG::DEBUG
            << " No TriggerTowers found in TES at "
            << m_triggerTowerLocation
            << endreq ;
        return StatusCode::SUCCESS;
    }
    
    if((sc2 == StatusCode::FAILURE))
    {
        log << MSG::DEBUG
            << " No JetElements found in TES at "
            << m_jetElementLocation
            << endreq ;
        return StatusCode::SUCCESS;
    }

    
    //std::ofstream oFile("TriggerTowers.dat" );
    

    *m_oFile << "Event ???" << std::endl;
    *m_oFile << "TriggerTowers" << std::endl;

    EMData = 0;
    HData = 0;
    
    PhiMax = 64;
    EtaMax = 60;
    try{
        EMData = InitialiseArray();
        HData = InitialiseArray();
    }catch(char *str){
        log << MSG::ERROR
	    << str
	    << endreq;
	EMData = 0;
	HData =0;
    }

    if(EMData && HData)
    {
        TTLoadArray();
	TTHighestOutput();
        EMData = FinaliseArray(EMData);
        HData = FinaliseArray(HData);
    }else{
       log << MSG::ERROR
	   << "Trigger Tower Arrays not initialised."
	   << endreq;
    }
   
    *m_oFile << std::endl;
    *m_oFile << "JetElements" << std::endl;

    
    PhiMax = 32;
    EtaMax = 24;
    
    try{
        EMData = InitialiseArray();
        HData = InitialiseArray();
    }catch(char *str){
        log << MSG::ERROR
	    << str
	    << endreq;
	EMData = 0;
	HData = 0;
    }
    
    if(EMData && HData)
    {
        JELoadArray();
        JEHighestOutput();
        EMData = FinaliseArray(EMData);       
        HData = FinaliseArray(HData);       
    }else{
       log << MSG::ERROR
	   << "Jet Array not initialised."
	   << endreq;
    }
   
   
    return StatusCode::SUCCESS;
}

/**
 *  Prints out complete array data in JetElement format of blocks of 4x8 to the file 
 *  JetElementOutputd.dat.  Note that output file is appended and not overwritten.
 *
 *  (NOT CALLED - USED FOR CONVENIENCE/DEBUGGING ONLY)
 */
void DumpTriggerObjects::JEDumpOutput()
{
    MsgStream log(messageService(), name()) ;
    log << MSG::INFO << "Dumping Full Jet Element Output to file: JetElementOutput.dat" << endreq;

    std::ofstream *output = new std::ofstream("JetElementOuput.dat", std::ios::app|std::ios::out);
    for(int i = 0; i < EtaMax; i+=4){
        for(int j = 0; j < PhiMax; j+=8){
	    printJEUnit(output, i, j);
        }
    }
    output->close();
}

/**
 *  Calculates the corresponding bin for a given phi angle.  Bins range from 0 to (PhiMax)
 *
 * @PARAM	phi	Phi angle
 * @RETURN		The bin number
 */
int DumpTriggerObjects::Phi2Bin(double phi)
{
    if(phi >= 2*M_PI || phi < 0){
        MsgStream log(messageService(), name()) ;
    	log << MSG::ERROR << "Phi out of range.  Setting phi to 0." << endreq;
	phi = 0;
    }
    return int(PhiMax*phi/(2*M_PI));
}

/**
 *  Calculates the corresponding bin number for a given eta angle.  Note: Method uses a very
 *  simple conversion method to calculate the Bin number.  A  more complicated method taking into
 *  account the granularity of the detector may be found in TTEta2Bin(double eta).
 *
 * @PARAM	eta	Eta value
 * @RETURN		The bin number
 */
int DumpTriggerObjects::JEEta2Bin(double eta)
{
    if(eta >= 5 || eta < -5){
        MsgStream log(messageService(), name());
        log << MSG::ERROR << "Eta out of range.  Setting eta to 0." << endreq;
	eta = -5;
    }
    return int(EtaMax*(eta+5)/10);
}

/**
 *  Calculates the corresponding bin number for a given eta angle.  Note:  this method takes into
 *  account the effect of the endcap.  It also assumes that the Eta bins range from 0 to 59 inclusive.
 *
 * @PARAM	eta	Eta Value
 * @RETURN		The bin number
 */
int DumpTriggerObjects::TTEta2Bin(double eta)
{
    double ETABarrel = 2.5;
    double ETAForward = 3.1;
    double ETAEndcap = 3.2;
    
    if(fabs(eta) > ETAEndcap)
    {
        if(eta <= 0)
        {
            return 0;
        }else
        {
            return 59;
        }
    }else if(fabs(eta) > ETAForward)
    {
        if(eta <= 0)
        {
            return 1;
        }else
        {
            return 58;
        }
    }else if(fabs(eta) > ETABarrel)
    {
        if(eta <= 0)
        {     
            return (int)(((eta+ETABarrel)*5)+4);
        }else
        {  
            return (int)(((eta-ETABarrel)*5)+55);
        }
    }else
    {
        return (int)(((eta+ETABarrel)*10)+5);
    }
}

/**
 *  Places Jet data from jetElements Map onto the Electromagnetic and hadronic arrays. 
 *  Method ignores absolute eta values of more than 2.5.
 *
 */
void DumpTriggerObjects::JELoadArray()
{
    JECollection::const_iterator ij;
    for(ij = jetElements->begin(); ij < jetElements->end(); ++ij)
    {
        if(fabs((*ij)->eta()) < 2.5)
	{
            int jetElementPhiBin = Phi2Bin((*ij)->phi());
	    int jetElementEtaBin = JEEta2Bin((*ij)->eta());
 
            MsgStream log(messageService(), name());   
            log << MSG::VERBOSE
	        << "Dump: JE has coords ("
	        << (*ij)->phi()<<", "
      	        << (*ij)->eta()
                << " and energy : " 
	        << (*ij)->energy()
	        << endreq;
				     
            EMData[jetElementPhiBin][jetElementEtaBin] += (*ij)->emEnergy();
            HData[jetElementPhiBin][jetElementEtaBin] += (*ij)->hadEnergy();
	}
    }
}

/**
 *  Places TriggerTower data from the m_storedTTs Map onto Electromagnetic and 
 *  hadronic arrays.  Method ignores absolute eta values of more than 2.5.
 *
 */
void DumpTriggerObjects::TTLoadArray()
{
    TTCollection::const_iterator it;
    for(it = m_storedTTs->begin(); it < m_storedTTs->end(); ++it)
    {
        if(fabs((*it)->eta()) < 2.5)
	{
            int ttElementPhiBin = Phi2Bin((*it)->phi());
  	    int ttElementEtaBin = TTEta2Bin((*it)->eta());
	
	    MsgStream log(messageService(), name());
	    log << MSG::VERBOSE 
	        << "Dump: TT has coords (" 
	        << (*it)->phi() << ", " 
	        << (*it)->eta() << " and energies : " 
	        << (*it)->emEnergy() << ", " 
	        << (*it)->hadEnergy() << " (Em,Had)" 
	        << endreq;
	     
	    EMData[ttElementPhiBin][ttElementEtaBin] += (*it)->emEnergy();
	    HData[ttElementPhiBin][ttElementEtaBin] += (*it)->hadEnergy();
	}
    }
}

/**
 *  Prints out complete array data in TriggerTower format of two blocks of 4x4 - 
 *  Electromagnetic data first, then Hadronic - to the file TriggerTowerOutput.dat.
 *  Note that output file is appended, not overwritten.
 *  
 *  (NOT CALLED - USED FOR CONVENIENCE/DEBUGGING ONLY)
 */
void DumpTriggerObjects::TTDumpOutput()
{
    MsgStream log(messageService(), name()) ;
    log << MSG::INFO << "Dumping Trigger Tower full output to file: TriggerTowerOutput.dat" << endreq;

    std::ofstream *output = new std::ofstream("TriggerTowerOutput.dat", std::ios::app|std::ios::out);
    for(int i = 0; i < EtaMax; i+=4)
    {
        for(int j = 0; j < PhiMax; j+=4)
	{
	    printTTUnit(output, i, j);
	    *output << std::endl;
	}
    }
    output->close();
}

/**
 *  Initialises a two dimensional array based on the current values of eta and phi.  All initial
 *  array elements are set to 0.  If for whatever reason the array cannot be initialised an
 *  exception is thrown.
 *
 * @THROWS	char *	Pointer to exception message.
 * @RETURN		Pointer to initialised array.
 */
int **DumpTriggerObjects::InitialiseArray()
{
    int **data = 0;
    data = new int *[PhiMax];
    if(data == 0){
        throw "Memory Fault: Could not allocate memory to data array";
    }
    for(int i = 0; i < PhiMax; i++){
        data[i] = 0;
	data[i] = new int[EtaMax];
	if(data[i] ==0){
            throw "Memory Fault: Could not allocate memory to data array";
	}
	for(int j = 0; j < EtaMax; j++){
	    data[i][j] = 0;
	}
    }
    return data;
}

/**
 *  Destroys an array.
 *
 * @PARAM	data	Pointer to the array to be destroyed.
 * @RETURNS		null pointer.
 */
int **DumpTriggerObjects::FinaliseArray(int **data)
{
    MsgStream log(messageService(), name());
    log << MSG::DEBUG << "Clearing data array." << endreq;

    for(int i = 0; i < PhiMax; i++){
        delete [] data[i];
    }
    delete [] data;
    data = 0;
    return data;
}

/**
 *  Prints out the highest TriggerTower 4x8 block (both EM and Hadronic blocks) based on total
 *  sum of energy deposited to the file HighestTriggerTowers.dat.  Note that this method DOES NOT 
 *  search for a region of interest - it simply prints the block with the highest energy.  Also 
 *  note that the file "HighestTriggerTowers.dat" is not overwritten, it is appended.
 *
 */
void DumpTriggerObjects::TTHighestOutput()
{
    MsgStream log(messageService(), name());
    log << MSG::INFO << "Dumping Highest TT values per group to file: HighestTriggerTowers.dat" << endreq;

    std::ofstream *output = new std::ofstream("HighestTriggerTowers.dat", std::ios::out|std::ios::app);
    int highestEnergy = 0;
    int energyTot = 0;
    int phiIndex = -1;
    int etaIndex = -1;
    for(int i = 0; i < EtaMax; i+=4)
    {
        for(int j = 0; j < PhiMax; j+=4)
	{
	    energyTot = 0;
	    for(int k = 0; k < 4; k++)
	    {
	        for(int l = 0; l < 4; l++)
		{
		    energyTot += (EMData[j+k][i+l]+HData[j+k][i+l]);
		}
            }
	    if(energyTot > highestEnergy)
	    {
	        highestEnergy = energyTot;
		phiIndex = j;
		etaIndex = i;
	    }
	}
    }
    if (etaIndex >= 0 && phiIndex >= 0) {
      printTTUnit(output, etaIndex, phiIndex);
      *output << std::endl;
    }
    output->close();
}

/**
 *  Prints out the highest JetElement block of 4x8 based on sum of total energy (em and had) 
 *  to the file JemInputMain_Athena.txt.
 *
 */
void DumpTriggerObjects::JEHighestOutput()
{
    MsgStream log(messageService(), name()) ;
    log << MSG::INFO << "Dumping highest JE values per group to file: JemInputMain_Athena.txt" << endreq;

    std::ofstream *output = new std::ofstream("JemInputMain_Athena.txt", std::ios::out|std::ios::app);
    int highestEnergy = 0;
    int energyTot = 0;
    int etaIndex = -1;
    int phiIndex = -1;
    
    for(int i = 0; i < EtaMax; i+=4)
    {
        for(int j = 0; j < PhiMax; j+=8)
	{ 
	    energyTot = 0;
	    for(int k = 0; k < 8; k++)
	    {
	        for(int l = 0; l < 4; l++)
	        {
	            energyTot += EMData[j+k][i+l];
   	            energyTot += HData[j+k][i+l];
	        }
	    }
	    if(energyTot > highestEnergy)
	    {
	        highestEnergy = energyTot;
		etaIndex = i;
		phiIndex = j;
	    }
	}
    }
    if (etaIndex >= 0 && phiIndex >= 0) {
      printJEUnit(output, etaIndex, phiIndex);    
    }
    output->close();
}

/**
 *  Prints out data in TriggerTower format - ie a block of 4x4.  Prints electromagnetic block
 *  followed by hadronic.
 *
 * @PARAM	output	Pointer to output stream (the file to which data is printed).
 *  		eta	Index of first eta value to start printing from.
 *  		phi	Index of first phi value to start printing from.
 */
void DumpTriggerObjects::printTTUnit(std::ofstream *output, int eta, int phi)
{
    for(int i = phi; i < phi+4; i++)
    {
        for(int j = eta; j < eta+4; j++)
	{
	    *output << std::setw(4) << EMData[i][j];
	}
	*output << std::endl;
    }

    for(int i = phi; i < phi+4; i++)
    {
        for(int j = eta; j < eta+4; j++)
	{
	    *output << std::setw(4) << HData[i][j];
	}
	*output << std::endl;
    }
}

/**
 *  Prints out data in JetElement format - ie a block of 4x8 followed by 4 separate figures
 *  (total of 68 integers printed).
 *
 * @PARAM	output	Pointer to output stream (the file to which data is printed).
 *  		eta	Index of first eta value to start printing from.
 *  		phi	Index of first phi value to start printing from.
 */
void DumpTriggerObjects::printJEUnit(std::ofstream *output, int eta, int phi)
{
    for(int i = phi; i < phi+8; i++)
    {
        for(int j = eta; j < eta + 4; j++)
        {
            *output << std::setw(4) << EMData[i][j];
	    *output << std::setw(4) << HData[i][j];
	    *output << std::endl;
        }
       *output << std::endl;
    }
   *output << std::setw(4) << 0
           << std::setw(4) << 0
           << std::setw(4) << 0
           << std::setw(4) << 0
           << std::endl << std::endl;
}

/**
 *  Prints out the entire array to a file.  Useful for debugging/convenience.  Note that 
 *  files are not overwritten, they are appended.
 *
 *  @PARAM	data	Pointer to data array to be printed
 *		str	The name of the file to be created.
 */
void DumpTriggerObjects::DumpArray(int **data, const char *str){
    std::ofstream *output = new std::ofstream(str, std::ios::app|std::ios::out);
    for(int i = 0; i < PhiMax; i++){
        for(int j = 0; j < EtaMax; j++){
	    *output << std::setw(4) << data[i][j];
	}
	*output << std::endl;
   }
}

}
