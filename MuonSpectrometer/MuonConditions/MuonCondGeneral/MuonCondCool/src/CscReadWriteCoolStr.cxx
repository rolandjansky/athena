/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscReadWriteCoolStr.cxx - simple example of algorithm demonstrating 
// read/write of CondStrFile
// author lampen@physics.arizona.edu
// Update: Apr 10, 2007. No longer reads - Caleb Parnell-Lampen <lampen@physics.arizona.edu>
#include "GaudiKernel/ISvcLocator.h"

#include "MuonCondData/CscCondParType.h"
//#include "MuonCondData/CscCalibData.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "string.h"

#include "CscReadWriteCoolStr.h"

#include <fstream>
#include <bitset>
#include <sstream>

#include "AthenaKernel/errorcheck.h"

using namespace std;
namespace MuonCalib {

  CscReadWriteCoolStr::CscReadWriteCoolStr(const std::string& name, 
      ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
                                 m_log(msgSvc(),name),
                                 p_detstore(0),
                                 m_cscCoolStrSvc("MuonCalib::CscCoolStrSvc",name),
                                 m_done(false),
                                 m_write(false),
                                 m_read(false),
                                 m_ofile("output.cal"),
                                 m_outFileType("04-00"),
                                 m_condDataContainer(NULL),
                                 m_cscId(0)
  {
    // declare properties

    declareProperty("Write",m_write);
    declareProperty("iFiles",m_ifiles);

    declareProperty("Read",m_read);
    declareProperty("oFile",m_ofile);
    declareProperty("OutFileType",m_outFileType);
    declareProperty("OutParameters",m_outParameters);
    declareProperty("ForceReadAsChannelCategory", m_forceChanCat= false);
    
    m_condDataContainer = new CscCondDataContainer() ;
  }


  CscReadWriteCoolStr::~CscReadWriteCoolStr() {
    delete m_condDataContainer;
  }

  StatusCode CscReadWriteCoolStr::initialize()
  {
    m_log << MSG::DEBUG << "CscReadWriteCoolStr::initialize() called" << endmsg;

    if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
      m_log << MSG::FATAL << "Detector store not found" << endmsg;
      return StatusCode::FAILURE;
    }
    if (StatusCode::SUCCESS!=m_cscCoolStrSvc.retrieve()) {
      m_log << MSG::FATAL << "Cannot get CscCoolStrSvc" << endmsg;
      return StatusCode::FAILURE;
    }
    
    StatusCode sc = p_detstore->retrieve(m_cscId,"CSCIDHELPER");
    if(sc.isFailure())
    {
      m_log << MSG::FATAL << "Cannot retrieve CscIdHelper from detector store" << endmsg;
      return sc;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode CscReadWriteCoolStr::execute() {
    return StatusCode::SUCCESS;
  }

  StatusCode CscReadWriteCoolStr::finalize() {
    m_log << MSG::INFO << "Finalizing CscReadWriteCoolStr." << endmsg;
    static int numFinal =0;
    if(numFinal>0)
      return StatusCode::SUCCESS;
    numFinal++;
    if (m_write) {
      if(readFiles() != StatusCode::SUCCESS)
        return StatusCode::RECOVERABLE;
      return writeToCool(); 
    }
    if(m_read)
    {
      m_log <<MSG::DEBUG<< "Creating file" << endmsg;
      return makeFile();
    }
    return StatusCode::SUCCESS;
  }

  StatusCode CscReadWriteCoolStr::readFiles() {

    StatusCode sc = StatusCode::SUCCESS;
    m_log <<MSG::INFO << "About to insert files" << endmsg;
    vector<string>::const_iterator fItr = m_ifiles.begin();
    vector<string>::const_iterator fEnd = m_ifiles.end();
    for(;fItr != fEnd; fItr++)
    {
      // ifstream * ifile = new ifstream(fItr->c_str());
      ifstream ifile(fItr->c_str());
      if(ifile.is_open() && ifile.good())
      {
        if(!procInputStream(/*dynamic_cast<istream*>(ifile)*/ifile).isSuccess())
        {
          sc = StatusCode::RECOVERABLE;
          m_log << MSG::ERROR << "Failed processing " << *fItr << endmsg;
        }
      }
      else
      {
        sc = StatusCode::RECOVERABLE;
        m_log << MSG::ERROR << "Failed opening " << *fItr << endmsg;
      }
    }

    m_log <<MSG::INFO << "Finished reading files" << endmsg;

    return sc;
  }

  StatusCode CscReadWriteCoolStr::procInputStream(istream & input )
  {
    if(!input.good())
      return StatusCode::RECOVERABLE;

    bool recognizedVersion = false;
    //   bool failedAny = false;

    string junk;

    //Get the header
    string fileVersion;
    input >> fileVersion; 

    if(fileVersion == "03-00" || fileVersion == "03-01") //-01 just means we can have bitsets
    {
      recognizedVersion = true;
      input >> junk;
      //Skip rest of header until "<END_HEADER>" is found
      //All other words can be comments from user, although it is recomended to start comments with
      //<COMMENTS>
      for(int i =1; i <= 1000000; i++)//for loop just prevents infinite loop in case of bad file
      {
        if(junk == "<END_HEADER>")
          break;

        //Prevent infinite loop
        if (i== 1000000 || !input.good()) {
          m_log << MSG::FATAL << "Never reached end of header. Went through " << i << "words."<< endmsg;
          return StatusCode::FAILURE;
        }
      }

      //Loop over each parameter, then over each value stored in that parameter
      while(input.good()) {
        int nBits = 0;
        int shiftBits  = 0;


        string tag;
        input >> tag;

        if( tag == "<END_FILE>")
          break;
        else if ( tag != "<NEW_PAR>"){
          m_log << MSG::ERROR << "Don't recognize tag " << tag << " in this context. " 
            << endmsg;
          return StatusCode::RECOVERABLE;
        }
        string nextWord;
        input >> nextWord;
        if(nextWord == "<BITS>") {
          input >> nBits;
          if(nBits > 64){
            m_log << MSG::ERROR << " bad nBits " << nBits << endmsg;
            return StatusCode::RECOVERABLE;
          }
          input >> shiftBits;
          string end_bits;
          input >> end_bits;
          m_log << MSG::DEBUG << "We've got nbits: " << nBits
            << " and a shift of  " << shiftBits << endmsg;
          if(end_bits != "<END_BITS>"){
            m_log << MSG::ERROR << "Expected <END_BITS> tag after nBits and shiftBits info" << endmsg;
          }
          input >> nextWord;
        }
        else
          m_log << MSG::DEBUG << "No bit settings for this parameter" << endmsg;

        string parName = nextWord;

        string cat, dataType;

        StatusCode catSc, dataTypeSc, sizeSc;

        //use cool string sercie to get details about this parameter
        catSc = m_cscCoolStrSvc->getParCat(parName, cat);
        dataTypeSc = m_cscCoolStrSvc->getParDataType(parName, dataType);

        if(!catSc.isSuccess() || !dataTypeSc.isSuccess() || !sizeSc.isSuccess())
        {
          m_log << MSG::ERROR << "Failed at retrieving info for " << parName  
            <<". Check job options to ensure you're adding it." << endmsg;
	  //          failedAny = true;
          continue;
        }

        //Process parameter. Need to know datatype for proper string to data conversion.
        //Add parameters to 
        
        if(dataType == "uint32_t")
          CHECK( procParameter3<uint32_t>(input, parName, cat, nBits, shiftBits));
        else if(dataType == "int")
          CHECK( procParameter3<int>(input, parName, cat, nBits, shiftBits) );
        else if(dataType == "float")
          CHECK( procParameter3<float>(input, parName, cat, nBits, shiftBits) );
        else if(dataType == "bool")
          CHECK( procParameter3<bool>(input, parName, cat, nBits, shiftBits) );
        else
          m_log << MSG::ERROR << "Don't recognize dataType " << dataType << " when reading input file " << endmsg;
      }//end parameter loop

    }
    else if(fileVersion == "<HEADER>" ) 
    {
      input >> fileVersion;

      //fileversion 04-01 allows for ASM2 based material
      if(fileVersion == "04-00" || fileVersion == "04-01"){
        m_log << MSG::DEBUG << "Reading file version 4" << endmsg;
        //Version 04-00 attempts to be more xml like, although it is still a bit of a poor man's
        //xml. Items must be in a particular order, and there must be spaces between tags and 
        //their content
        recognizedVersion = true;

        input >> junk;
        //Skip rest of header until "</END_HEADER>" is found
        //All other words can be comments from user, although it is recomended to start comments with
        //<COMMENTS>
        for(int i =1; i <= 1000000; i++)//for loop just prevents infinite loop in case of bad file
        {
          if(junk == "</HEADER>")
            break;

          //Prevent infinite loop
          if (i== 1000000 || !input.good()) {
            m_log << MSG::FATAL << "Never reached end of header. Went through " << i << "words."<< endmsg;
            return StatusCode::FAILURE;
          }
        }

        m_log << MSG::DEBUG << "About to loop after finding " << junk << endmsg;

        //Loop over each parameter, then over each value stored in that parameter
        while(input.good()) {
          m_log << MSG::DEBUG << " loop" <<endmsg;
          int nBits = 0;
          int shiftBits  = 0;


          string tag;
          input >> tag;

          if( tag == "<END_FILE>")
            break;
          else if ( tag != "<PARAMETER>"){
            m_log << MSG::ERROR << "Don't recognize tag " << tag << " in this context. " 
              << endmsg;
            return StatusCode::RECOVERABLE;
          }

          string nextWord;
          input >> nextWord;
          string parName = nextWord;
          if(parName == "<BITS>" || parName == "<DATA>"){
            m_log << MSG::ERROR << "First entry insidet a parameter must be parameter name, not " 
              << parName << endmsg;
            return StatusCode::RECOVERABLE;
          }

          input >> nextWord;

          if(nextWord == "<BITS>") {
            input >> nBits;
            input >> shiftBits;
            string end_bits;
            input >> end_bits;
            m_log << MSG::DEBUG << "We've got nbits: " << nBits
              << " and a shift of  " << shiftBits << endmsg;
            if(end_bits != "</BITS>"){
              m_log << MSG::ERROR << "Expected </BITS> tag after nBits and shiftBits info" << endmsg;
            }
            input >> nextWord;
          }
          else
            m_log << MSG::DEBUG << "No bit settings for this parameter" << endmsg;

          if(nextWord == "<DATA>"){

            string cat, dataType;

            StatusCode catSc, dataTypeSc, sizeSc;

            //use cool string sercie to get details about this parameter
            catSc = m_cscCoolStrSvc->getParCat(parName, cat);
            dataTypeSc = m_cscCoolStrSvc->getParDataType(parName, dataType);

            if(!catSc.isSuccess() || !dataTypeSc.isSuccess() || !sizeSc.isSuccess())
            {
              m_log << MSG::ERROR << "Failed at retrieving info for " << parName  
                <<". Check job options to ensure you're adding it." << endmsg;
	      //        failedAny = true;
              continue;
            }

            //Process parameter. Need to know datatype for proper string to data conversion.
            //Add parameters to 
            m_log << MSG::INFO << "Processing data for parameter " << parName << endmsg;

            if(dataType == "uint32_t")
              CHECK( procParameter4<uint32_t>(input, parName, cat, nBits, shiftBits));
            else if(dataType == "int")
              CHECK( procParameter4<int>(input, parName, cat, nBits, shiftBits));
            else if(dataType == "float")
              CHECK( procParameter4<float>(input, parName, cat, nBits, shiftBits));
            else if(dataType == "bool")
              CHECK( procParameter4<bool>(input, parName, cat, nBits, shiftBits));
            else
              m_log << MSG::ERROR << "Don't recognize dataType " << dataType << " when reading input file " << endmsg;
            input >> nextWord;
          }
          else {
            m_log << MSG::ERROR << "No data found for parameter " << parName << endmsg;
            return StatusCode::RECOVERABLE;
          }
          if(nextWord != "</PARAMETER>"){
            m_log << MSG::WARNING << "Expected parameter info to end with token " 
              << "</PARAMETER>" << ". Instead have " << nextWord << endmsg;
          }
        }//end parameter loop

      }//end if 04-00
    }//end version test

    if(!recognizedVersion){
      m_log << MSG::FATAL << "Didn't recognize input format version. If 04-00 make sure you've put <HEADER> (all caps) at front of file!" << endmsg;
      m_log << MSG::FATAL << "Read file version as " << fileVersion << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS; 

  } 

  StatusCode CscReadWriteCoolStr::writeToCool() {
    //Send to be added to database
    m_log << MSG::INFO << "About to merge and submit data to cool" << endmsg;
    return m_cscCoolStrSvc->mergeAndSubmitCondDataContainer(m_condDataContainer);
  }

  StatusCode CscReadWriteCoolStr::makeFile() {
    m_log << MSG::INFO << "Writing data from database to file " << m_ofile << endmsg;

    ofstream outFile(m_ofile.c_str());
    
    string dataType ="";

    IdContext channelContext = m_cscId->channel_context();

    if(m_outFileType =="04-00"){
      outFile << "<HEADER>\n" << m_outFileType << "\n</HEADER>\n";

    }
    else{
      outFile << m_outFileType << " <END_HEADER>\n";
    }

    vector<string>::const_iterator parNameItr = m_outParameters.begin();
    vector<string>::const_iterator parNameEnd = m_outParameters.end();
    for(;parNameItr != parNameEnd; parNameItr++) {

      m_log << MSG::DEBUG << "Storing " << *parNameItr << endmsg;

      unsigned int numIndx = 0;
      if(!m_cscCoolStrSvc->getParNumHashes(*parNameItr,numIndx).isSuccess()) {
        m_log << MSG::ERROR << "Failed getting num hashes for " 
          << *parNameItr << endmsg;
        return StatusCode::RECOVERABLE;
      }




      string cat;
      if(!m_cscCoolStrSvc->getParCat(*parNameItr,cat).isSuccess() ){
        m_log << MSG::ERROR << "Failed getting category for " << *parNameItr
          << endmsg;
      }

      m_log << MSG::DEBUG << "Category is " << cat << " and maximum index is " <<numIndx-1 
        << endmsg;

      if(m_forceChanCat)
      {
        m_log << MSG::WARNING << *parNameItr << " of category " << cat << " is being forced to be read as category CHANNEL"<< endmsg;
        cat = "CHANNEL";
      }

      if(!m_cscCoolStrSvc->getParDataType(*parNameItr, dataType)){
        m_log <<MSG::ERROR << "Failed getting data type for " << *parNameItr
          << endmsg;
        return StatusCode::RECOVERABLE;
      }

      m_log << MSG::INFO << "datatype is " << dataType << endmsg;
      if(m_outFileType == "04-00" || m_outFileType == "04-01"){
        outFile << "<PARAMETER>\n" << *parNameItr << "\n<DATA>\n";
        if(cat == "CHANNEL"){
          for(unsigned int indxItr =0; indxItr < numIndx; indxItr++)
          {
            //make sure the database has this value in a way that won't
            //cause any errors
            if(!m_cscCoolStrSvc->checkIndex(*parNameItr, indxItr)) {
              m_log << MSG::VERBOSE << "nothing at index " << indxItr << endmsg;
              continue;
            }

            Identifier chanId;
            m_cscId->get_id((IdentifierHash)indxItr, chanId, &channelContext);
            int eta = m_cscId->stationEta(chanId);
            int phi = m_cscId->stationPhi(chanId);
            int measuresPhi = m_cscId->measuresPhi(chanId);
            int strip = m_cscId->strip(chanId);
            int size = m_cscId->stationName(chanId);//1 for large, 0 for not
            int layer = m_cscId->wireLayer(chanId);

            int sector = (phi*2 - size + 50)*eta;


            std::string data =  RetrieveDataAsString( *parNameItr, indxItr, dataType);
            if(data == "" ){
              m_log << MSG::ERROR << "Failed to retrieve data!" << endmsg;
              return StatusCode::RECOVERABLE;
            }

            //update file
            outFile << sector << " "
              << layer  << " "
              << (measuresPhi ? 'Y' : 'X') << " "
              << strip << " "
              << data
              << "\n";
          }
        }
        else if(cat == "ASM"){
          //loop over all sectors
          for(int sector = -16 ; sector <= 16; sector++){

            //there is no sector 0
            if(sector == 0)
              continue;

            //Get details chamber id
            int stationName = (abs(sector) % 2 ? 51 : 50);
            int stationEta = (sector > 0 ? 1 : -1);
            int stationPhi = (abs(sector)+1)/2;
            static const int chamberLayer = 2;

            //loop over each asm for the sector
            for(int asmNum = 1; asmNum <=5 ; asmNum++){
              //Find range of strip numbers this corresponds to
              int stripSince(0), stripUntil(0); //stripSince is inclusive, stripUntil is exclusive
              int measuresPhi(0);
              int layerSince(0), layerUntil(0);//layerSince is inclusive, layerUntil is exclusive


              //Retreive data for a single strip in scope, at layer Since and stripSince. All values should be same, so its fine
              CHECK(m_cscCoolStrSvc->getAsmScope(asmNum, measuresPhi, layerSince, layerUntil, stripSince, stripUntil));
              Identifier chanId = m_cscId->channelID(stationName, stationEta, stationPhi, 
                  chamberLayer, layerSince, measuresPhi, stripSince);
              IdentifierHash hash;
              m_cscId->get_channel_hash(chanId, hash);
              std::string data = RetrieveDataAsString(*parNameItr, (int)hash, dataType);

              //update file
              outFile << sector << " ASM" << asmNum << " " << data << "\n";
            }
          }

        }
        else{
          m_log << MSG::FATAL << "CSC COOL parameter category \"" << cat << "\" is not supported" << endmsg;
          return StatusCode::FAILURE;
        }
        outFile << "</DATA>\n</PARAMETER>\n";
      }
      else{
        outFile <<"<NEW_PAR> " << *parNameItr << "\n";
        for(unsigned int indxItr =0; indxItr < numIndx; indxItr++)
        {
          //make sure the database has this value in a way that won't
          //cause any errors
          if(!m_cscCoolStrSvc->checkIndex(*parNameItr, indxItr)) {
            m_log << MSG::VERBOSE << "nothing at index " << indxItr << endmsg;
            continue;
          }

          string stringId;
          if(m_outFileType == "03-00"|| m_outFileType== "03-01") {
            if(!m_cscCoolStrSvc->indexToStringId(indxItr,cat,stringId)) {
              m_log << MSG::ERROR << "Failed getting string Id from indxItr " 
                << indxItr << " for parameter " << *parNameItr << endmsg;
              return StatusCode::RECOVERABLE;
            }
          }
          else {


            stringstream ss;
            Identifier chanId;
            m_cscId->get_id((IdentifierHash)indxItr, chanId, &channelContext);

            ss << indxItr << " " << m_cscId->show_to_string(chanId);

            stringId = ss.str();

          }



          m_log << MSG::DEBUG << "index "  << indxItr << " converted to string " << stringId << endmsg;

          outFile << stringId << " ";

          std::string data =  RetrieveDataAsString( *parNameItr, indxItr, dataType) ;
          if(data == ""){
            m_log << MSG::ERROR << "Failed to to retrieve data!" << endmsg;
            return StatusCode::RECOVERABLE;
          }
          outFile << data;


          outFile << "\n";
        }//end index loop
        outFile << "<END_PAR>\n";
      }//end if-else file type
    }//end parameer loop
    outFile << "<END_FILE>";
    outFile.close();
    return StatusCode::SUCCESS;
  }

  StatusCode CscReadWriteCoolStr::mergeBits(const uint32_t & inputDatum, 
      const uint32_t & refDatum, uint32_t & newDatum, 
      const int & nBits, const int & bitShift) {
    bitset<32> newBits(refDatum);
    bitset<32> inputBits(inputDatum);

    m_log << MSG::DEBUG << "Merging bits from input " <<  hex << inputDatum << " (with offset of " << bitShift << ") and reference " << hex << refDatum << dec << endmsg;

    if(bitShift + nBits > 32) {
      m_log << MSG::ERROR << "Requesting a bit beyond 32 during bit merging. Probably a bug."
        << endmsg;
      return StatusCode::FAILURE;
    }


    for(int i = 0; i < nBits; i++){
      newBits.set(i+bitShift, inputBits[i]);
    }

    newDatum = newBits.to_ulong();
    m_log << MSG::DEBUG << "Merged version is " << newDatum << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode CscReadWriteCoolStr::mergeBits(const int & , const int & , const int & , 
      const int &, const int &){
    m_log << MSG::ERROR << "Tried to merge bits of an int" << endmsg;

    return StatusCode::FAILURE;
  }

  StatusCode CscReadWriteCoolStr::mergeBits(const float & , const float & , const float & , 
      const int &, const int &) {
    m_log << MSG::ERROR << "Tried to merge bits of a float" << endmsg;
    return StatusCode::FAILURE;
  }

  std::string CscReadWriteCoolStr::RetrieveDataAsString( std::string coolKey, int hash, std::string dataType){
    std::stringstream stream;
    if(dataType == "uint32_t")
    {
      uint32_t datum;
      if(!m_cscCoolStrSvc->getParameter(datum,coolKey,hash)){
        m_log << MSG::ERROR << "Failed to retrieve data for key " << coolKey
          << " data type " << dataType << endmsg;
      }
      stream << datum;
    }
    else if(dataType == "int")
    {
      int datum;
      if(!m_cscCoolStrSvc->getParameter(datum,coolKey,hash)){
        m_log << MSG::ERROR << "Failed to retrieve data for key " << coolKey
          << " data type " << dataType << endmsg;
      }
      else 
        stream << datum;
    }
    else if(dataType == "float")
    {
      //std::cout << "Getting float datatype" << std::endl;
      float datum;
      if(!m_cscCoolStrSvc->getParameter(datum,coolKey,hash)){
        m_log << MSG::ERROR << "Failed to retrieve data for key " << coolKey
          << " data type " << dataType << endmsg;
      }
      else
        stream << datum;
    }
    else if(dataType == "bool")
    {
      //std::cout << "Getting bool datatype" << std::endl;
      bool datum;
      if(!m_cscCoolStrSvc->getParameter(datum,coolKey,hash)){
        m_log << MSG::ERROR << "Failed to retrieve data for key " << coolKey
          << " data type " << dataType << endmsg;
      }
      else
        stream << datum;
    }
    else
    {
      m_log << MSG::ERROR << "don't know datatype " << dataType << endmsg;
    }
    return stream.str();

  }
}//end namespace
