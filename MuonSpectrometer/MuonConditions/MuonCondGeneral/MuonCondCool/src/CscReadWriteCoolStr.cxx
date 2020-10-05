/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CscReadWriteCoolStr.cxx - simple example of algorithm demonstrating 
// read/write of CondStrFile
// author lampen@physics.arizona.edu
// Update: Apr 10, 2007. No longer reads - Caleb Parnell-Lampen <lampen@physics.arizona.edu>

#include "CscReadWriteCoolStr.h"

#include "GaudiKernel/ISvcLocator.h"
#include "MuonCondData/CscCondParType.h"
#include "AthenaKernel/errorcheck.h"

#include <fstream>
#include <bitset>
#include <sstream>

namespace MuonCalib {

  CscReadWriteCoolStr::CscReadWriteCoolStr(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    p_detstore(nullptr),
    m_cscCoolStrSvc("MuonCalib::CscCoolStrSvc",name),
    m_write(false),
    m_read(false),
    m_ofile("output.cal"),
    m_outFileType("04-00"),
    m_condDataContainer(nullptr)
  {
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
    ATH_MSG_DEBUG("CscReadWriteCoolStr::initialize() called");
    ATH_CHECK(service("DetectorStore",p_detstore));
    ATH_CHECK(m_cscCoolStrSvc.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode CscReadWriteCoolStr::execute() {
    return StatusCode::SUCCESS;
  }

  StatusCode CscReadWriteCoolStr::finalize() {
    ATH_MSG_DEBUG("Finalizing CscReadWriteCoolStr.");
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
      ATH_MSG_DEBUG("Creating file");
      return makeFile();
    }
    return StatusCode::SUCCESS;
  }

  StatusCode CscReadWriteCoolStr::readFiles() {
    StatusCode sc = StatusCode::SUCCESS;
    ATH_MSG_DEBUG("About to insert files");
    std::vector<std::string>::const_iterator fItr = m_ifiles.begin();
    std::vector<std::string>::const_iterator fEnd = m_ifiles.end();
    for(;fItr != fEnd; fItr++)
    {
      std::ifstream ifile(fItr->c_str());
      if(ifile.is_open() && ifile.good())
      {
        if(!procInputStream(/*dynamic_cast<istream*>(ifile)*/ifile).isSuccess())
        {
          sc = StatusCode::RECOVERABLE;
          ATH_MSG_ERROR("Failed processing " << *fItr);
        }
      }
      else
      {
        sc = StatusCode::RECOVERABLE;
        ATH_MSG_ERROR("Failed opening " << *fItr);
      }
    }

    ATH_MSG_DEBUG("Finished reading files");

    return sc;
  }

  StatusCode CscReadWriteCoolStr::procInputStream(std::istream & input )
  {
    if(!input.good())
      return StatusCode::RECOVERABLE;

    bool recognizedVersion = false;
    //   bool failedAny = false;

    std::string junk;

    //Get the header
    std::string fileVersion;
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
          ATH_MSG_FATAL("Never reached end of header. Went through " << i << "words.");
          return StatusCode::FAILURE;
        }
      }

      //Loop over each parameter, then over each value stored in that parameter
      while(input.good()) {
        int nBits = 0;
        int shiftBits  = 0;


        std::string tag;
        input >> tag;

        if( tag == "<END_FILE>")
          break;
        else if ( tag != "<NEW_PAR>"){
          ATH_MSG_ERROR("Don't recognize tag " << tag << " in this context. ");
          return StatusCode::RECOVERABLE;
        }
        std::string nextWord;
        input >> nextWord;
        if(nextWord == "<BITS>") {
          input >> nBits;
          if(nBits > 64){
            ATH_MSG_ERROR(" bad nBits " << nBits);
            return StatusCode::RECOVERABLE;
          }
          input >> shiftBits;
          std::string end_bits;
          input >> end_bits;
          ATH_MSG_DEBUG("We've got nbits: " << nBits
            << " and a shift of  " << shiftBits);
          if(end_bits != "<END_BITS>"){
            ATH_MSG_ERROR("Expected <END_BITS> tag after nBits and shiftBits info");
          }
          input >> nextWord;
        }
        else
          ATH_MSG_DEBUG("No bit settings for this parameter");

        std::string parName = nextWord;

        std::string cat, dataType;

        StatusCode catSc, dataTypeSc, sizeSc;

        //use cool string sercie to get details about this parameter
        catSc = m_cscCoolStrSvc->getParCat(parName, cat);
        dataTypeSc = m_cscCoolStrSvc->getParDataType(parName, dataType);

        if(!catSc.isSuccess() || !dataTypeSc.isSuccess() || !sizeSc.isSuccess())
        {
          ATH_MSG_ERROR("Failed at retrieving info for " << parName  
            <<". Check job options to ensure you're adding it.");
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
          ATH_MSG_ERROR("Don't recognize dataType " << dataType << " when reading input file ");
      }//end parameter loop

    }
    else if(fileVersion == "<HEADER>" ) 
    {
      input >> fileVersion;

      //fileversion 04-01 allows for ASM2 based material
      if(fileVersion == "04-00" || fileVersion == "04-01"){
        ATH_MSG_DEBUG("Reading file version 4");
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
            ATH_MSG_FATAL("Never reached end of header. Went through " << i << "words.");
            return StatusCode::FAILURE;
          }
        }

        ATH_MSG_DEBUG("About to loop after finding " << junk);

        //Loop over each parameter, then over each value stored in that parameter
        while(input.good()) {
          ATH_MSG_DEBUG(" loop");
          int nBits = 0;
          int shiftBits  = 0;


          std::string tag;
          input >> tag;

          if( tag == "<END_FILE>")
            break;
          else if ( tag != "<PARAMETER>"){
            ATH_MSG_ERROR("Don't recognize tag " << tag << " in this context. ");
            return StatusCode::RECOVERABLE;
          }

          std::string nextWord;
          input >> nextWord;
          std::string parName = nextWord;
          if(parName == "<BITS>" || parName == "<DATA>"){
            ATH_MSG_ERROR("First entry insidet a parameter must be parameter name, not " 
              << parName);
            return StatusCode::RECOVERABLE;
          }

          input >> nextWord;

          if(nextWord == "<BITS>") {
            input >> nBits;
            input >> shiftBits;
            std::string end_bits;
            input >> end_bits;
            ATH_MSG_DEBUG("We've got nbits: " << nBits
              << " and a shift of  " << shiftBits);
            if(end_bits != "</BITS>"){
              ATH_MSG_ERROR("Expected </BITS> tag after nBits and shiftBits info");
            }
            input >> nextWord;
          }
          else
            ATH_MSG_DEBUG("No bit settings for this parameter");

          if(nextWord == "<DATA>"){

            std::string cat, dataType;

            StatusCode catSc, dataTypeSc, sizeSc;

            //use cool string sercie to get details about this parameter
            catSc = m_cscCoolStrSvc->getParCat(parName, cat);
            dataTypeSc = m_cscCoolStrSvc->getParDataType(parName, dataType);

            if(!catSc.isSuccess() || !dataTypeSc.isSuccess() || !sizeSc.isSuccess())
            {
              ATH_MSG_ERROR("Failed at retrieving info for " << parName  
                <<". Check job options to ensure you're adding it.");
	      //        failedAny = true;
              continue;
            }

            //Process parameter. Need to know datatype for proper string to data conversion.
            //Add parameters to 
            ATH_MSG_DEBUG("Processing data for parameter " << parName);

            if(dataType == "uint32_t")
              CHECK( procParameter4<uint32_t>(input, parName, cat, nBits, shiftBits));
            else if(dataType == "int")
              CHECK( procParameter4<int>(input, parName, cat, nBits, shiftBits));
            else if(dataType == "float")
              CHECK( procParameter4<float>(input, parName, cat, nBits, shiftBits));
            else if(dataType == "bool")
              CHECK( procParameter4<bool>(input, parName, cat, nBits, shiftBits));
            else
              ATH_MSG_ERROR("Don't recognize dataType " << dataType << " when reading input file ");
            input >> nextWord;
          }
          else {
            ATH_MSG_ERROR("No data found for parameter " << parName);
            return StatusCode::RECOVERABLE;
          }
          if(nextWord != "</PARAMETER>"){
            ATH_MSG_WARNING("Expected parameter info to end with token " 
              << "</PARAMETER>" << ". Instead have " << nextWord);
          }
        }//end parameter loop

      }//end if 04-00
    }//end version test

    if(!recognizedVersion){
      ATH_MSG_FATAL("Didn't recognize input format version. If 04-00 make sure you've put <HEADER> (all caps) at front of file!");
      ATH_MSG_FATAL("Read file version as " << fileVersion);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS; 

  } 

  StatusCode CscReadWriteCoolStr::writeToCool() {
    //Send to be added to database
    ATH_MSG_DEBUG("About to merge and submit data to cool");
    return m_cscCoolStrSvc->mergeAndSubmitCondDataContainer(m_condDataContainer);
  }

  StatusCode CscReadWriteCoolStr::makeFile() {
    ATH_MSG_DEBUG("Writing data from database to file " << m_ofile);

    std::ofstream outFile(m_ofile.c_str());
    
    std::string dataType ="";

    IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();

    if(m_outFileType =="04-00"){
      outFile << "<HEADER>\n" << m_outFileType << "\n</HEADER>\n";

    }
    else{
      outFile << m_outFileType << " <END_HEADER>\n";
    }

    std::vector<std::string>::const_iterator parNameItr = m_outParameters.begin();
    std::vector<std::string>::const_iterator parNameEnd = m_outParameters.end();
    for(;parNameItr != parNameEnd; parNameItr++) {

      ATH_MSG_DEBUG("Storing " << *parNameItr);

      unsigned int numIndx = 0;
      if(!m_cscCoolStrSvc->getParNumHashes(*parNameItr,numIndx).isSuccess()) {
        ATH_MSG_ERROR("Failed getting num hashes for " 
          << *parNameItr);
        return StatusCode::RECOVERABLE;
      }




      std::string cat;
      if(!m_cscCoolStrSvc->getParCat(*parNameItr,cat).isSuccess() ){
        ATH_MSG_ERROR("Failed getting category for " << *parNameItr);
      }

      ATH_MSG_DEBUG("Category is " << cat << " and maximum index is " <<numIndx-1 
       );

      if(m_forceChanCat)
      {
        ATH_MSG_WARNING(*parNameItr << " of category " << cat << " is being forced to be read as category CHANNEL");
        cat = "CHANNEL";
      }

      if(!m_cscCoolStrSvc->getParDataType(*parNameItr, dataType)){
        ATH_MSG_ERROR("Failed getting data type for " << *parNameItr);
        return StatusCode::RECOVERABLE;
      }

      ATH_MSG_DEBUG("datatype is " << dataType);
      if(m_outFileType == "04-00" || m_outFileType == "04-01"){
        outFile << "<PARAMETER>\n" << *parNameItr << "\n<DATA>\n";
        if(cat == "CHANNEL"){
          for(unsigned int indxItr =0; indxItr < numIndx; indxItr++)
          {
            //make sure the database has this value in a way that won't
            //cause any errors
            if(!m_cscCoolStrSvc->checkIndex(*parNameItr, indxItr)) {
              ATH_MSG_VERBOSE("nothing at index " << indxItr);
              continue;
            }

            Identifier chanId;
            m_idHelperSvc->cscIdHelper().get_id((IdentifierHash)indxItr, chanId, &channelContext);
            int eta = m_idHelperSvc->cscIdHelper().stationEta(chanId);
            int phi = m_idHelperSvc->cscIdHelper().stationPhi(chanId);
            int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(chanId);
            int strip = m_idHelperSvc->cscIdHelper().strip(chanId);
            int size = m_idHelperSvc->cscIdHelper().stationName(chanId);//1 for large, 0 for not
            int layer = m_idHelperSvc->cscIdHelper().wireLayer(chanId);

            int sector = (phi*2 - size + 50)*eta;


            std::string data =  RetrieveDataAsString( *parNameItr, indxItr, dataType);
            if(data == "" ){
              ATH_MSG_ERROR("Failed to retrieve data!");
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
              Identifier chanId = m_idHelperSvc->cscIdHelper().channelID(stationName, stationEta, stationPhi, 
                  chamberLayer, layerSince, measuresPhi, stripSince);
              IdentifierHash hash;
              m_idHelperSvc->cscIdHelper().get_channel_hash(chanId, hash);
              std::string data = RetrieveDataAsString(*parNameItr, (int)hash, dataType);

              //update file
              outFile << sector << " ASM" << asmNum << " " << data << "\n";
            }
          }

        }
        else{
          ATH_MSG_FATAL("CSC COOL parameter category \"" << cat << "\" is not supported");
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
            ATH_MSG_VERBOSE("nothing at index " << indxItr);
            continue;
          }

          std::string stringId;
          if(m_outFileType == "03-00"|| m_outFileType== "03-01") {
            if(!m_cscCoolStrSvc->indexToStringId(indxItr,cat,stringId)) {
              ATH_MSG_ERROR("Failed getting string Id from indxItr " 
                << indxItr << " for parameter " << *parNameItr);
              return StatusCode::RECOVERABLE;
            }
          }
          else {


            std::stringstream ss;
            Identifier chanId;
            m_idHelperSvc->cscIdHelper().get_id((IdentifierHash)indxItr, chanId, &channelContext);

            ss << indxItr << " " << m_idHelperSvc->cscIdHelper().show_to_string(chanId);

            stringId = ss.str();

          }



          ATH_MSG_DEBUG("index "  << indxItr << " converted to string " << stringId);

          outFile << stringId << " ";

          std::string data =  RetrieveDataAsString( *parNameItr, indxItr, dataType) ;
          if(data == ""){
            ATH_MSG_ERROR("Failed to to retrieve data!");
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
    std::bitset<32> newBits(refDatum);
    std::bitset<32> inputBits(inputDatum);

    ATH_MSG_DEBUG("Merging bits from input " <<  std::hex << inputDatum << " (with offset of " << bitShift << ") and reference " << std::hex << refDatum << std::dec);

    if(bitShift + nBits > 32) {
      ATH_MSG_ERROR("Requesting a bit beyond 32 during bit merging. Probably a bug.");
      return StatusCode::FAILURE;
    }


    for(int i = 0; i < nBits; i++){
      newBits.set(i+bitShift, inputBits[i]);
    }

    newDatum = newBits.to_ulong();
    ATH_MSG_DEBUG("Merged version is " << newDatum);
    return StatusCode::SUCCESS;
  }

  StatusCode CscReadWriteCoolStr::mergeBits(const int & , const int & , const int & , 
      const int &, const int &){
    ATH_MSG_ERROR("Tried to merge bits of an int");

    return StatusCode::FAILURE;
  }

  StatusCode CscReadWriteCoolStr::mergeBits(const float & , const float & , const float & , 
      const int &, const int &) {
    ATH_MSG_ERROR("Tried to merge bits of a float");
    return StatusCode::FAILURE;
  }

  std::string CscReadWriteCoolStr::RetrieveDataAsString( std::string coolKey, int hash, std::string dataType){
    std::stringstream stream;
    if(dataType == "uint32_t")
    {
      uint32_t datum;
      if(!m_cscCoolStrSvc->getParameter(datum,coolKey,hash)){
        ATH_MSG_ERROR("Failed to retrieve data for key " << coolKey
          << " data type " << dataType);
      }
      stream << datum;
    }
    else if(dataType == "int")
    {
      int datum;
      if(!m_cscCoolStrSvc->getParameter(datum,coolKey,hash)){
        ATH_MSG_ERROR("Failed to retrieve data for key " << coolKey
          << " data type " << dataType);
      }
      else 
        stream << datum;
    }
    else if(dataType == "float")
    {
      //std::cout << "Getting float datatype" << std::endl;
      float datum;
      if(!m_cscCoolStrSvc->getParameter(datum,coolKey,hash)){
        ATH_MSG_ERROR("Failed to retrieve data for key " << coolKey
          << " data type " << dataType);
      }
      else
        stream << datum;
    }
    else if(dataType == "bool")
    {
      //std::cout << "Getting bool datatype" << std::endl;
      bool datum;
      if(!m_cscCoolStrSvc->getParameter(datum,coolKey,hash)){
        ATH_MSG_ERROR("Failed to retrieve data for key " << coolKey
          << " data type " << dataType);
      }
      else
        stream << datum;
    }
    else
    {
      ATH_MSG_ERROR("don't know datatype " << dataType);
    }
    return stream.str();

  }
}//end namespace
