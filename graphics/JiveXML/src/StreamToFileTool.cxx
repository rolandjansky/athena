/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <JiveXML/StreamToFileTool.h>

#include <fstream>
#include <iomanip>

namespace JiveXML {

  /**
   * Constructor 
   * - setup message service
   * - initialize properties
   */
  StreamToFileTool::StreamToFileTool( const std::string& type , const std::string& name, const IInterface* p):
    AthAlgTool(type,name,p){
    
    //Provide interface
    declareInterface<IStreamTool>(this);

    /// The file name prefix
    declareProperty("FileNamePrefix", m_FileNamePrefix = "JiveXML");
    /// The file name suffix
    declareProperty("FileNameSuffix", m_FileNameSuffix = ".xml");
  }

  /**
   * Intialize - called once at the beginning
   */
  StatusCode StreamToFileTool::initialize(){

    return StatusCode::SUCCESS;
  }

  /**
   * Finalize - called once at the end
   */
  StatusCode StreamToFileTool::finalize(){
    return StatusCode::SUCCESS;
  }

  /**
   * Stream one event into a file
   * @param EventNumber the event number
   * @param RunNumber the run number
   * @param EventBuffer the string holding the complete event
   */
   StatusCode StreamToFileTool::StreamEvent( const unsigned long EventNumber, const unsigned int RunNumber, const std::ostringstream* EventBuffer ) { 
   
     /// Get a pointer to a new file
     std::ofstream* outFile;
     StatusCode sc = NewFile(EventNumber,RunNumber,outFile);
     if (sc.isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not open file for event " 
           << EventNumber << " from run " << RunNumber << endreq;
       return sc;
     }

     /// Stream this event into the file 
     (*outFile) << EventBuffer->str();
     outFile->flush();
     /// Check wether we could write the event
     if (!outFile->good()){
       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not write event " 
           << EventNumber << " from run " << RunNumber << endreq;
       return StatusCode::FAILURE;
     }
     
     /// Finally close the file
     sc = CloseFile(outFile);
     return sc;

   }

  /**
   * Generate a file name from the run and event number
   * @param EventNumber the event number
   * @param RunNumber the run number
   */
   std::string StreamToFileTool::MakeFileName( const unsigned long EventNumber, const unsigned int RunNumber) const {

     //Generate a the return string with file prefix
     std::ostringstream name;
     
     //Assemble file name
     name << m_FileNamePrefix << std::setfill('0');
     name << "_" << std::setw(5) << RunNumber;
     name << "_" << std::setw(5) << EventNumber;
     name << m_FileNameSuffix; 
     return name.str();
   }

   /** 
    * Open a new file
    */
   StatusCode StreamToFileTool::NewFile( const unsigned long EventNumber, const unsigned int RunNumber, std::ofstream *& outputFile) const {
      
      // Generate the file name
      std::string filename = MakeFileName(EventNumber,RunNumber);

      // create a new output stream
      outputFile = new std::ofstream(filename.c_str());

      // check if it worked
      if ( !(outputFile->good()) ){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to create output file with name " << filename << endreq;
        return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
    }

   /**
    * Close the file 
    */   
   StatusCode StreamToFileTool::CloseFile( std::ofstream *& outputFile ) const {

    //Try to close the file
    outputFile->close();
    
    //See if it worked
    if (!outputFile->good()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to close file" << endreq;
    }

    //In any case delete object
    delete outputFile;

    return StatusCode::SUCCESS;
  }

} //namespace
