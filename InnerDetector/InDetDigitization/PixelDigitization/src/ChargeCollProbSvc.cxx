/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "src/ChargeCollProbSvc.h"
#include "src/IChargeCollProbSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ServiceHandle.h"
 
#include "GaudiKernel/PropertyMgr.h"

#include "PathResolver/PathResolver.h"
 
#include <fstream>
 
/**
 ** Constructor(s)
 **/
ChargeCollProbSvc::ChargeCollProbSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc)
    //  : AthService(name,svc),log(msgSvc(),name)
{
  declareProperty( "CCProbMapFileFEI3", m_cc_prob_file_fei3 = "PixelDigitization/3DFEI3-3E-problist-1um_v1.txt");  
  declareProperty( "CCProbMapFileFEI4", m_cc_prob_file_fei4 = "PixelDigitization/3DFEI4-2E-problist-1um_v0.txt");  
}
 
ChargeCollProbSvc::~ChargeCollProbSvc()
{
}
 
/**
 ** Initialize Service
 **/
StatusCode ChargeCollProbSvc::initialize()
{
  //MsgStream log(msgSvc(), name());
 
  StatusCode result = AthService::initialize();
  if (result.isFailure())
  {
   ATH_MSG_FATAL ( "Unable to initialize the service!" );
   return result;
  }
 
  // read Charge Collection Probability Maps
 
  if ( this->readProbMap(m_cc_prob_file_fei3).isFailure() || this->readProbMap(m_cc_prob_file_fei4).isFailure() ){
    ATH_MSG_ERROR ( "Charge Collection Prob Maps: error in reading txt files" );
    return StatusCode::FAILURE;
  }
 
  ATH_MSG_INFO ( "initialized service!" );
  return result;
 
}  
 
StatusCode ChargeCollProbSvc::finalize()
{
        return StatusCode::SUCCESS;
}
 
StatusCode ChargeCollProbSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_IChargeCollProbSvc == riid )    {
    *ppvInterface = (IChargeCollProbSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
 
// read the Charge Collection Prob Map from text file
StatusCode ChargeCollProbSvc::readProbMap(std::string fileE)
{
  std::string line;
  const std::string fileName = fileE;
  std::string inputFile=PathResolverFindCalibFile(fileName);
  if (inputFile==""){
      ATH_MSG_ERROR ( "Could not open input file!!!!!" );
      return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG (" opening file "<<inputFile);
  std::ifstream myfile(inputFile.c_str());
  if (myfile.is_open()){
    ATH_MSG_DEBUG (" opened file!");
      while (myfile.good())
        {
          while (std::getline(myfile,line))
            {
              std::istringstream sline(line);
              int xpos,ypos;
              double prob;
              sline>>xpos>>ypos>>prob;
              if (fileName.find("FEI4")!=std::string::npos){
                probMapFEI4.insert( std::make_pair( std::make_pair( xpos , ypos ) , prob ) );
                ATH_MSG_DEBUG ("FEI4 inside xpos  "<<xpos<<"   ypos  "<<ypos<<"    prob  "<<prob);
              }else if(fileName.find("FEI3")!=std::string::npos){
                probMapFEI3.insert( std::make_pair( std::make_pair( xpos , ypos ) , prob ) );
                ATH_MSG_DEBUG ("FEI3 inside xpos  "<<xpos<<"   ypos  "<<ypos<<"    prob  "<<prob);
              }else{
                ATH_MSG_ERROR ("Please check name of Charge Coll Prob Maps! (should contain FEI3 or FEI4) ");
                return StatusCode::FAILURE;
              }
            }      
        }
      myfile.close();
  }
  //
  return StatusCode::SUCCESS;
}
 
// -- Print out the Charge Collection Probability map (full map)
StatusCode ChargeCollProbSvc::printProbMap(std::string readout)
{
 
  if(readout == "FEI4"){
    for ( std::multimap<std::pair<int,int>, double >::iterator it = probMapFEI4.begin(); it != probMapFEI4.end(); ++it ) {
      ATH_MSG_DEBUG ("read full probMap  FEI4 --- bin x "<<it->first.first<<"   bin y  "<<it->first.second<<"    prob  "<<it->second);
    }
  }else if(readout == "FEI3"){
    for ( std::multimap<std::pair<int,int>, double >::iterator it = probMapFEI3.begin(); it != probMapFEI3.end(); ++it ) {
      ATH_MSG_DEBUG ("read full probMap  FEI3 --- bin x "<<it->first.first<<"   bin y  "<<it->first.second<<"    prob  "<<it->second);
    }
  }else{
    ATH_MSG_ERROR ("Error in printout Charge Coll Prob Maps! (readout should contain FEI3 or FEI4 strings) ");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
 
 
// -- Returns the Charge Collection Probability at a given point (bin_x,bin_y)
double ChargeCollProbSvc::getProbMapEntry( std::string readout, int binx, int biny ){
 
  std::pair <int, int> doublekey ( binx, biny);
  double echarge;
 
  if (readout == "FEI4"){
    std::multimap<  std::pair< int,int >, double >::const_iterator iter = probMapFEI4.find(doublekey);
    echarge = iter->second;
  }else if(readout == "FEI3"){
    std::multimap<  std::pair< int,int >, double >::const_iterator iter = probMapFEI3.find(doublekey);
    echarge = iter->second;
  }else{
    ATH_MSG_ERROR ("No Map Entry available for the requested readout");
    echarge = -1.;
  }
 
  return echarge;
}
