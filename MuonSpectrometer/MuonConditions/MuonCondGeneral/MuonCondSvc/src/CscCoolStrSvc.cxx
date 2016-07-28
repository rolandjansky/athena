/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************************8
//******************************************************************************************
//CscCoolStrSvc.cxx
//******************************************************************************************
//Service designed to read in calibration files to the cool database. Can also read them
//back out again to check.
// author lampen@physics.arizona.edu

#include <sstream>
#include <vector>
#include <algorithm>//for transform
#include <cctype> //for toupper
#include <inttypes.h> 
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "MuonIdHelpers/CscIdHelper.h"

// temporary includes to access CLOBs
//#include "CoolKernel/ExtendedAttributeListSpecification.h"
//#include "CoolKernel/PredefinedStorageHints.h"

//Calibration data containers
#include "MuonCondData/CscCondDataCollection.h"
//#include "MuonCondData/CscCondDataCollectionBaseReader.h"
#include "MuonCondData/CscCondDataContainer.h"

#include "StoreGate/DataHandle.h"
#include "EventInfo/TagInfo.h"

// root class for string manipulation 
//#include "TString.h"

#include "CscCoolStrSvc.h"

using namespace std;
namespace MuonCalib {
  CscCoolStrSvc::CscCoolStrSvc(const string& name, ISvcLocator* svc) :
    AthService(name,svc),
    p_detstore(nullptr),
    m_log(msgSvc(),name), 
    m_cscId(nullptr),
    m_maxChamberHash(32), //retrieved later from cscIdHelper
    m_maxChanHash(61440), //retrieved later from cscIdHelper
    m_dbCache(0),
    m_rmsCondData(nullptr),
    m_slopeCondData(nullptr),
    m_noiseCondData(nullptr),
    m_f001CondData(nullptr),
    m_pedestalCondData(nullptr),
    m_t0PhaseCondData(nullptr),
    m_t0BaseCondData(nullptr),
    m_statusCondData(nullptr),
    m_debug(false),
    m_verbose(false),
    m_numFailedRequests(0),
    m_maxChamberCoolChannel(32),
    m_maxLayerHash(255),
    m_defaultChanStatusName("status"),
    m_accfloats(nullptr),
    m_accInts(nullptr),
    m_accUints(nullptr),
    m_accBools(nullptr),
    m_accVoid(nullptr)
  {
    //declare properties

    declareProperty("ParNames", m_parNameVec);          //Parameter name
    declareProperty("ParSGKeys", m_parSGKeyVec);        //Storegate key for retrieval
    declareProperty("ParFolders",m_parFolderVec);       //Not used here. Provided for access by outside algorithms, primarily for writing.
    declareProperty("ParDataTypes", m_parDataTypeVec);  //float, int, etc
    declareProperty("ParCats", m_parCatVec);            //categories for parameters
    declareProperty("ParDefaults", m_parDefaultVec);    //default value of parameters

    declareProperty("PreCache",m_preCache = false); //Set to true if initialized in CBNT_initialize()
    declareProperty("MaxFailedRequests", m_maxFailedRequests = 100);
    declareProperty("MaxChamberHash", m_maxChamberHash = 31);
    declareProperty("DefaultDatabaseReadVersion", m_defaultDatabaseReadVersion = "02-00");
    declareProperty("DefaultChanStatusName", m_defaultChanStatusName = "status");
    declareProperty("DoMerge", m_doMerge = true);
    declareProperty("DoCaching", m_doCaching = true);
    declareProperty("OnlineOfflineConvertPhiFlip", m_onlineOfflinePhiFlip = true);
    declareProperty("PhiSwapVersion1Strings", m_phiSwapVersion1Strings = true);

  }

  //-------------------------------------------------------------------
  CscCoolStrSvc::~CscCoolStrSvc() {
    if(m_dbCache != 0) m_dbCache->clear();
    delete m_dbCache;
  }

  const InterfaceID& CscCoolStrSvc::type() const
  {
    return CscICoolStrSvc::interfaceID();
  }

  StatusCode CscCoolStrSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
  {
    if (CscICoolStrSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface=(CscICoolStrSvc*)this;
    } else {
      return AthService::queryInterface(riid,ppvInterface);
    }
    return StatusCode::SUCCESS;
  }


  //-------------------------------------------------------------------
  StatusCode CscCoolStrSvc::initialize()
  {
    // service initialisation 
    m_log.setLevel(outputLevel());    //individual outputlevel not known before inialize
    m_debug = (m_log.level() <= MSG::DEBUG);
    m_verbose = (m_log.level() <= MSG::VERBOSE);

    m_log << MSG::INFO << "Initializing CscCoolStrSvc" <<endmsg;

    // get detector store, linked to cool database by other algorithms in your
    // jobOptions file.
    if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
      m_log << MSG::FATAL << "Detector store not found" << endmsg; 
      return StatusCode::FAILURE;
    }

    //Setup CscIdHelper
    StoreGateSvc* detStore= 0;
    StatusCode sc = serviceLocator()->service("DetectorStore",detStore);

    if(sc.isSuccess())
    {
      sc = detStore->retrieve(m_cscId,"CSCIDHELPER");
      if(sc.isFailure())
      {
        m_log << MSG::FATAL << "Cannot retrieve CscIdHelper from detector store" << endmsg;
        return sc;
      }
    }
    else
    {
      m_log << MSG::FATAL << "DetectorStore service not found " << endmsg;
      return StatusCode::FAILURE;
    } 

    m_moduleContext = m_cscId->module_context();
    m_channelContext = m_cscId->channel_context();

    m_rmsCondData = 0;
    m_slopeCondData = 0;
    m_noiseCondData = 0;
    m_f001CondData = 0;
    m_pedestalCondData = 0;
    m_t0PhaseCondData = 0;
    m_t0BaseCondData = 0;
    m_statusCondData = 0;


    //prepare layer hash array
    int hash = 0;

    for(int stationName  = 0; stationName < 2; stationName++)
    {
      for(int stationEta =0; stationEta <2; stationEta++)
      {
        for(int stationPhi = 0; stationPhi <8; stationPhi++)
        {
          for(int wireLayer = 0; wireLayer <4; wireLayer++)
          {
            for(int measuresPhi = 0; measuresPhi <2; measuresPhi++)
            {
              Identifier id = m_cscId->channelID(
                  stationName+1,
                  (stationEta? 1:-1),
                  stationPhi+1,
                  2,//only installed chamber layer
                  wireLayer+1,
                  measuresPhi,
                  1 //channel doesn't matter. We'll just use first
                  );
              unsigned int onlineId;
              if(!offlineToOnlineId(id, onlineId).isSuccess()) {
                m_log << MSG::ERROR << "Failed at geting online id!" << endmsg; 
                return StatusCode::RECOVERABLE;
              }
              m_onlineChannelIdsFromLayerHash.push_back(onlineId);

              m_layerHashes[stationName][stationEta][stationPhi][wireLayer][measuresPhi] = hash++;
            }
          }
        }
      }
    }//end prepare hash array

    *(const_cast<unsigned int*>(&m_maxLayerHash)) = hash -1; //-1 because hash overshoots in loop

    //prepare chamberCoolChannel. This is similar to the hash array in CscIdHelper.
    //Putting it here because cool channels are dependent on this hash, and it can't
    //ever change, even if idHelper's definition changes

    hash = 1;
    for(int stationName  = 0; stationName < 2; stationName++)
    {
      for(int stationEta =0; stationEta <2; stationEta++)
      {
        for(int stationPhi = 0; stationPhi <8; stationPhi++)
        {
              Identifier id = m_cscId->channelID(
                  stationName+1,
                  (stationEta? 1:-1),
                  stationPhi+1,
                  2,//only installed chamber layer 2
                  1,//wirelayer doesn't matter, we'll just use first
                  0,//measures phi doesn't matter, we'll just use precision
                  1 //channel doesn't matter. We'll just use first
                  );
              unsigned int onlineId;
              if(!offlineToOnlineId(id, onlineId).isSuccess()) {
                m_log << MSG::ERROR << "Failed at geting online id!" << endmsg; 
                return StatusCode::RECOVERABLE;
              }

          m_onlineChannelIdsFromChamberCoolChannel.push_back(onlineId);
          m_chamberCoolChannels[stationName][stationEta][stationPhi] = hash++;
        }
      }
    }
    *(const_cast<unsigned int*>(&m_maxChamberCoolChannel)) = hash - 1; //-1 because hash overshoots in loop
    //*(const_cast<unsigned int*>(&m_maxChamberHash)) = m_cscId->module_hash_max() - 1;
    *(const_cast<unsigned int*>(&m_maxChanHash)) = m_cscId->channel_hash_max() - 1;

    if(m_debug) m_log << MSG::DEBUG << "Maximum Chamber hash is " << m_maxChamberHash 
      << endmsg;
    if(m_debug) m_log << MSG::DEBUG << "Maximum Chamber COOL Channel is " 
      << m_maxChamberCoolChannel << endmsg;
    if(m_debug) m_log << MSG::DEBUG << "Maximum Layer hash is " << m_maxLayerHash 
      << endmsg;
    if(m_debug) m_log << MSG::DEBUG << "Maximum Channel hash is " << m_maxChanHash 
      << endmsg;


    ////////////
    // Loop over csc detector elements and add in the hash ids
    std::vector<Identifier> modules = m_cscId->idVector();

    //typedef std::vector<Identifier>::const_iterator csc_id;

    /*
       print out all strip hashes
    csc_id firstId = modules.begin();
    csc_id lastId  = modules.end();

    IdentifierHash moduleHashId;
    IdentifierHash channelHashId;

    for(; firstId != lastId; ++firstId) {
      Identifier id = *firstId;
      if (m_cscId->validElement(id)) {
        if(!m_cscId->get_hash(id, moduleHashId, &m_moduleContext)) {
          std::cout << "The CSC Chamber hash id is " << moduleHashId
            << " for " << m_cscId->show_to_string(id,&m_moduleContext) << std::endl;
          std::cout << "*****The strips in this Chamber ********************" << std::endl;
          std::vector<Identifier> vect;
          m_cscId->idChannels(id, vect);
          for (unsigned int i=0; i<vect.size(); ++i) {
            if(!m_cscId->get_hash(vect[i], channelHashId, &m_channelContext)) {
              std::cout << "The CSC strip hash id is " << channelHashId
                << " for " << m_cscId->show_to_string(vect[i],&m_channelContext) << std::endl;
            }
          } 
        }
        else {
          m_log << MSG::ERROR << "Unable to set csc hash id for det elem " 
            << "context begin_index = " << m_moduleContext.begin_index()
            << " context end_index  = " << m_moduleContext.end_index()
            << " the idenitifier is "
            << endmsg;
          id.show();
        }
      } else {
        m_log << MSG::ERROR << "Invalid CSC id " << endmsg;
        id.show();
      }
    }  
    */
    /////////

    //Prepare local cache.
    m_dbCache = new CscCondDataContainer();
    m_dbCache->clear(); 

    unsigned int numPars = m_parNameVec.size(); 
    if(numPars == 0) {
      m_log << MSG::WARNING << "No parameters requested for CscCoolStrSvc. No COOL CSC data will be available from this service." << endmsg;
    }

    //Ensure all vectors have exactly numPars
    if( m_parSGKeyVec.size() != numPars 
        || m_parFolderVec.size() != numPars
        || m_parDataTypeVec.size() != numPars
        || m_parCatVec.size() != numPars 
        || m_parDefaultVec.size() != numPars
      ) {
      m_log << MSG::FATAL << "Need identical number of entries in each parameter definition vector!"
        << "\nParNames:\t" << m_parNameVec.size() 
        << "\nParSGKeys:\t" << m_parSGKeyVec.size()
        << "\nParFolders:\t" << m_parFolderVec.size()
        << "\nParDataTypes:\t" << m_parDataTypeVec.size()
        << "\nParCats:\t" << m_parCatVec.size()
        << "\nParDefault:\t" << m_parDefaultVec.size()
        << endmsg;
      return StatusCode::FAILURE;
    }

    //bool noneFailed = true;
    //bool anySucceed = false;
    //Initialized each parameter
    for(unsigned int parItr = 0 ; parItr < numPars; parItr++)
    {

      std::string name = m_parNameVec[parItr];
      std::string sgKey = m_parSGKeyVec[parItr];
      std::string folder = m_parFolderVec[parItr] ;
      std::string dataType = m_parDataTypeVec[parItr]; 
      std::string category = m_parCatVec[parItr]; 
      std::string defaultVal = m_parDefaultVec[parItr] ;


      if(m_debug) m_log << MSG::DEBUG << "Entering new parameter." 
        << "\nName:\t" << name
          << "\nStoreGateKey:\t" << sgKey
          << "\nDataType:\t" << dataType
          << "\nCategory:\t" << category
          << "\nDefaultVal:\t" << defaultVal
          << "\nSgKey:\t" << sgKey 
          << "\nFolder:\t" << folder
          << endmsg;

      if(m_parNameMap.count(name)) {
        m_log << MSG::WARNING << "Multiple parameters with name " << name
          << ". This isn't allowed! Skipping extra entries." << endmsg;
        continue;
      }

      if(m_parSGKeyMap.count(sgKey)) {
        m_log << MSG::WARNING << "Multiple parameters with storegate key " << sgKey 
          << ". This isn't allowed! Skipping extra entries." << endmsg;
        continue;
      }

      //Need to store this as the proper data type.  
      CscCondDataCollectionBase * coll = NULL;


      //Now determine data type.
      //Data types when requested by getParameter must be EXACT, or seg
      //fault results.
      //Although we have interface for several types, until they're needed
      //we disable easily confused types. To reenable, need to uncomment here,
      //and remove casting in getParamter functions
      //if(dataType == "uint8_t") {
      //  coll = new CscCondDataCollection<uint8_t>();
      //}
      //else if(dataType == "uint16_t") {
      //  coll = new CscCondDataCollection<uint16_t>();
      // }
      if(dataType == "uint32_t") {
        coll = new CscCondDataCollection<uint32_t>();
      }
      else if(dataType == "int") { 
        coll = new CscCondDataCollection<int8_t>();
      }
      else if(dataType == "float") {
        coll = new CscCondDataCollection<float>();
      }
      else if(dataType == "bool") {
        coll = new CscCondDataCollection<bool>();
      }
      else {
        m_log << MSG::WARNING << "Don't recognize requested data type " 
          << dataType << endmsg;
      }

      //Store details of parameter for easy lookup later
      coll->setParName(name);
      coll->setParSGKey(sgKey);
      coll->setParFolder(folder);
      coll->setParCat(category);
      coll->setParDefault(defaultVal);

      //Store more details derived from parameter category 
      StatusCode extractStatus= initCscCondDataCollection(coll);
      if(!extractStatus.isSuccess()) {
        //noneFailed = false;
        m_log << MSG::WARNING << "Failed to register " << name << ". Continuing without..." << endmsg;
        continue;
      }



      //Store in container so it could (potentially) be persitified. Also will
      //handle cleanup for us.
      m_dbCache->push_back(coll);

      //Store the pointers in maps for easy lookup later
      m_parNameMap[name] = coll;
      m_parSGKeyMap[sgKey] = coll;
      
      if( name == "rms" ) m_rmsCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == "pslope" ) m_slopeCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == "noise" ) m_noiseCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == "f001" ) m_f001CondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == "ped" ) m_pedestalCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == "t0phase" ) m_t0PhaseCondData = dynamic_cast<CscCondDataCollection<bool>*>(coll);
      else if( name == "t0base" ) m_t0BaseCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == m_defaultChanStatusName ) {
        m_statusCondData = dynamic_cast<CscCondDataCollection<uint32_t>*>(coll);
        if( !m_statusCondData ) m_log << MSG::WARNING << "Wrong data type for status bit " << dataType << endmsg;
      }else  m_log << MSG::WARNING << "Data type not cached, a direct access should be provided " << name << endmsg;
      
      const DataHandle<CondAttrListCollection> & dataHandle = coll->atrcHandle();

      //Registering callback function. The callback funciton will now be called
      //whenever the parameter in question is altered. i.e. whenever it goes
      //into a new interval of validity.
      if(m_debug) m_log << MSG::DEBUG << "Registering " << name << " with storegate key " << sgKey << endmsg;

      if(m_doCaching)
        if(StatusCode::SUCCESS != p_detstore->regFcn(&CscCoolStrSvc::callback,this,
              dataHandle, sgKey, true)) {
          m_log << MSG::ERROR << "Failed to register parameter " << name << " with storeGate Key " << sgKey << endmsg;
        }

      //Precaching. We shouldn't need to do this, but some rare cases it seems the callbacks are not called in time for the begining of the run.
      if(m_preCache)
        cacheParameter(sgKey);

      //   anySucceed = true;

    }//end register parameter loop

    return StatusCode::SUCCESS;
  }//end initialize()


  //-------------------------------------------------------------------
  StatusCode CscCoolStrSvc::finalize()
  {
    if(m_debug) m_log << MSG::DEBUG << "in finalize()" << endmsg;
    return StatusCode::SUCCESS;
  }//end finalize()


  //-------------------------------------------------------------------
  //Here we define the hard coded categories. We define what their indices 
  //correspond to, as well as the cool channel range they cover. 
  //Not defined under a category is the type of data (eg float), which is a separate
  //type of information stored inside the CscCondDataCollectionBase class.
  //
  //To add a new category you must:
  //1. Add one here
  //2. Add to CscReadWriteStr algorithm
  //3. Increase size of bitset used in mergeCollections
  StatusCode  CscCoolStrSvc::initCscCondDataCollection(CscCondDataCollectionBase * coll) const {

    const std::string & cat = coll->getParCat();

    //Note: Index here refers to index of the CscCondParCollection DataVector
    //setNumCoolChan indicates the number of cool channels are reserved for 
    //that category of parameter
    //Note that cool channels start at 1, so "maxChamberCoolChannel" is also the
    //number of chamber cool channels

    //setSize sets the size of the collection's internal storage, indicating
    //the number of hashes needed for that category. i.e. the number of values
    //stored for the cateogry.

    //CSC wide parameter
    //Only a single flag for entirety of CSCs
    if(cat == "CSC") 
    {
      coll->setNumCoolChan(1);
      coll->setSize(1);
    }

    //One parameter per endcap
    //Each endcap is a cool channel
    else if(cat == "ENDCAP")
    {
      coll->setNumCoolChan(2);
      coll->setSize(2);
    } 

    //One parameter per chamber
    //Each chamber is a cool channel
    //Each index is a chamber hash (or module hash, as defined by cscIdHelper)
    else if(cat == "CHAMBER")
    {
      coll->setNumCoolChan(m_maxChamberCoolChannel);
      coll->setSize(m_maxChamberHash+1);
    }

    //One parameter per layer
    //Each chamber is a cool channel
    //Each index is a layerHash (as defined by this service)
    else if(cat == "LAYER")
    {
      coll->setNumCoolChan(m_maxChamberCoolChannel);
      coll->setSize(m_maxLayerHash+1);
    }

    //One parameter per channel
    //Each chamber is a cool channel
    //Each index is a channel hash (as defined in cscIdHelper)
    else if(cat == "CHANNEL")
    {
      coll->setNumCoolChan(m_maxChamberCoolChannel);
      coll->setSize(m_maxChanHash+1);
    }

    //One parameter per asm-chamber
    //Each chamber is a cool channel
    //Each index is a channel hash (as defined in cscIdHelper)
    else if(cat == "ASM")
    {
      coll->setNumCoolChan(m_maxChamberCoolChannel);
      coll->setSize(m_maxChanHash+1);
    }


    //Don't recognize category... 
    else
    {
      m_log << MSG::WARNING << "Don't recognize category " << cat << " as a parameter category. " << endmsg;
    }

    coll->reset(); //clears collection

    return StatusCode::SUCCESS; 
  }//end initCscCondDataCollection


  //-------------------------------------------------------------------
  //Checks that there is data for a particular index of a parameter
  int CscCoolStrSvc::checkIndex(const std::string & parName, unsigned int& index) const {

    map<string, CscCondDataCollectionBase*>::const_iterator itr =  
      m_parNameMap.find(parName);
    if(itr == m_parNameMap.end())
      return 0;

    //if there is data, return 1, otherwise 0
    const CscCondDataCollectionBase * coll = itr->second;
    if(!coll) 
      return 0;

    if(index >= coll->getSize()) {
      return 0;
    } 

    return coll->check(index);
  }

  bool CscCoolStrSvc::getVal( float& val, const CscCondDataCollection<float>& coll, unsigned int index) const {
    const CscCondData<float>* condData = coll[index];
    if( !condData ){
      m_log << MSG::WARNING << " No data available for hash " << index << endmsg;
      return false;
    }
    val = condData->getValue();
    return true;
  }
  bool CscCoolStrSvc::getVal( bool& val, const CscCondDataCollection<bool>& coll, unsigned int index) const {
    const CscCondData<bool>* condData = coll[index];
    if( !condData ){
      m_log << MSG::WARNING << " No data available for hash " << index << endmsg;
      return false;
    }
    val = condData->getValue();
    return true;
  }

  bool CscCoolStrSvc::getVal(uint32_t& val, const CscCondDataCollection<uint32_t>& coll, unsigned int index) const {
    const CscCondData<uint32_t>* condData = coll[index];
    if( !condData ){
      m_log << MSG::WARNING << " No data available for hash " << index << endmsg;
      return false;
    }
    val = condData->getValue();
    return true;
  }

  bool CscCoolStrSvc::getRMS( float& val, const unsigned int & index) const {
    if( !m_rmsCondData ) {
      m_log << MSG::WARNING << " No RMS data available" << endmsg;
      return false;
    }
    return getVal(val,*m_rmsCondData,index);
  }
  bool CscCoolStrSvc::getSlope( float& val, const unsigned int & index) const {
    if( !m_slopeCondData ) {
      m_log << MSG::WARNING << " No slope data available" << endmsg;
      return false;
    }
    return getVal(val,*m_slopeCondData,index);
  }
  bool CscCoolStrSvc::getF001( float& val, const unsigned int & index) const {
    if( !m_f001CondData ) {
      m_log << MSG::WARNING << " No f001 data available" << endmsg;
      return false;
    }
    return getVal(val,*m_f001CondData,index);
  }
  bool CscCoolStrSvc::getPedestal( float& val, const unsigned int & index) const {
    if( !m_pedestalCondData ) {
      m_log << MSG::WARNING << " No pedestal data available" << endmsg;
      return false;
    }
    return getVal(val,*m_pedestalCondData,index);
  }
  bool CscCoolStrSvc::getT0Base( float& val, const unsigned int & index) const {
    if( !m_t0BaseCondData ) {
      m_log << MSG::WARNING << " No t0base data available" << endmsg;
      return false;
    }
    return getVal(val,*m_t0BaseCondData,index);
  }
  bool CscCoolStrSvc::getT0Phase( bool& val, const unsigned int & index) const {
    if( !m_t0PhaseCondData ) {
      m_log << MSG::WARNING << " No t0phase data available" << endmsg;
      return false;
    }
    return getVal(val,*m_t0PhaseCondData,index);
  }
  bool CscCoolStrSvc::getNoise( float& val, const unsigned int & index) const {
    if( !m_noiseCondData ) {
      m_log << MSG::WARNING << " No noise data available" << endmsg;
      return false;
    }
    return getVal(val,*m_noiseCondData,index);
  }

  //Returns the status. This is only here for backwards compatibility
  bool CscCoolStrSvc::getStatus(uint32_t &val, const unsigned int &index) const  {
    if( !m_statusCondData ) {
      m_log << MSG::WARNING << " No status data available" << endmsg;
      return false;
    }
    return getVal(val,*m_statusCondData,index);
  }

  //-------------------------------------------------------------------
  StatusCode CscCoolStrSvc::getParameter(uint8_t &retData, const std::string &parName, const unsigned int & index) const 
  {
    //Only have uint32_t, so we switch to that
    uint32_t data;
    StatusCode sc;
    sc =  getParameterTemplated(data, parName,index);
    retData = data;
    
     m_log << MSG::WARNING << " The use of this function is very expensive, please use the direct call instead: parName " 
           << parName << endmsg;
    return sc;
  }

  StatusCode CscCoolStrSvc::getParameter(uint16_t &retData, const std::string &parName, const unsigned int & index) const 
  {
    //Only have uint8_t, so they probably meant that...
    uint32_t data;
    StatusCode sc;
    sc =  getParameterTemplated(data, parName,index);
    retData = data;
     m_log << MSG::WARNING << " The use of this function is very expensive, please use the direct call instead: parName " 
           << parName << endmsg;
    return sc;
  }

  StatusCode CscCoolStrSvc::getParameter(uint32_t &retData, const std::string &name, const unsigned int & index) const 
  {
    //We only have uint8_t parameters, so they probably meant that...
     // m_log << MSG::WARNING << " The use of this function is very expensive, please use the direct call instead: parName " 
     //       << parName << endmsg;
     StatusCode sc = getParameterTemplated(retData, name, index);
     if( name == m_defaultChanStatusName ) {
       uint32_t val;
       if( !getStatus( val, index) ) {
         m_log << MSG::WARNING << " Failed to retrieve data " << name << endmsg;
       }
       if( val != retData ){
         m_log << MSG::WARNING << " Bad conversion of rms " << retData << " --> " << val << " " << name << endmsg;
       }
     }else  m_log << MSG::WARNING << "Data type not cached, a direct access should be provided " << name << endmsg;
     return sc;
  }

  StatusCode CscCoolStrSvc::getParameter(int &retData, const std::string &parName, const unsigned int & index) const 
  {
     m_log << MSG::WARNING << " The use of this function is very expensive, please use the direct call instead: parName " 
           << parName << endmsg;
    return getParameterTemplated(retData, parName, index);
  }

  StatusCode CscCoolStrSvc::getParameter(float &retData, const std::string &name, const unsigned int & index) const 
  {
     // m_log << MSG::WARNING << " The use of this function is very expensive, please use the direct call instead: parName " 
     //       << parName << endmsg;
     StatusCode sc = getParameterTemplated(retData, name, index);
     if( name == "rms" ){
       float val;
       if( !getRMS( val, index) ) {
         m_log << MSG::WARNING << " Failed to retrieve data " << name << endmsg;
       }
       if( val != retData ){
         m_log << MSG::WARNING << " Bad conversion of rms " << retData << " --> " << val << " " << name << endmsg;
       }
     }else if( name == "pslope" ){
       float val;
       if( !getSlope( val, index) ) {
                  m_log << MSG::WARNING << " Failed to retrieve data " << name << endmsg;
       }
       if( val != retData ){
         m_log << MSG::WARNING << " Bad conversion of rms " << retData << " --> " << val << " " << name << endmsg;
       }       
     }else if( name == "noise" ){
       float val;
       if( !getNoise( val, index) ) {
                  m_log << MSG::WARNING << " Failed to retrieve data " << name << endmsg;
       }
       if( val != retData ){
         m_log << MSG::WARNING << " Bad conversion of rms " << retData << " --> " << val << " " << name << endmsg;
       }       
     }else if( name == "f001" ) {
       float val;
       if( !getF001( val, index) ) {
                  m_log << MSG::WARNING << " Failed to retrieve data " << name << endmsg;
       }
       if( val != retData ){
         m_log << MSG::WARNING << " Bad conversion of rms " << retData << " --> " << val << " " << name << endmsg;
       }       
     }else if( name == "ped" ) {
       float val;
       if( !getPedestal( val, index) ) {
                  m_log << MSG::WARNING << " Failed to retrieve data " << name << endmsg;
       }
       if( val != retData ){
         m_log << MSG::WARNING << " Bad conversion of rms " << retData << " --> " << val << " " << name << endmsg;
       }       
     }else if( name == "t0base" ){
       float val;
       if( !getT0Base( val, index) ) {
                  m_log << MSG::WARNING << " Failed to retrieve data " << name << endmsg;
       }
       if( val != retData ){
         m_log << MSG::WARNING << " Bad conversion of rms " << retData << " --> " << val << " " << name << endmsg;
       }       
     }
     return sc;
  }
  
  StatusCode CscCoolStrSvc::getParameter(bool &retData, const std::string &name, const unsigned int & index) const 
  {
    // m_log << MSG::WARNING << " The use of this function is very expensive, please use the direct call instead: parName " 
    //       << parName << endmsg;
    if( name == "t0phase" ) {
      bool val;
      if( !getT0Phase( val, index) ) {
        m_log << MSG::WARNING << " Failed to retrieve data " << name << endmsg;
      }
      if( val != retData ){
        m_log << MSG::WARNING << " Bad conversion of rms " << retData << " --> " << val << " " << name << endmsg;
      }
    }
    return getParameterTemplated(retData, name, index);
  }


  //-------------------------------------------------------------------
  //Returns the status. This is only here for backwards compatibility
  StatusCode CscCoolStrSvc::getStatus(uint8_t &status, const unsigned int & stripID) const
  {
    uint32_t theStatus(0);
    StatusCode sc;
    sc = getParameter(theStatus, m_defaultChanStatusName, stripID);
    status = static_cast<uint8_t>(theStatus);
    // m_log << MSG::WARNING << " The use of this function is very expensive, please use the direct call instead " 
    //       << endmsg;
    return sc;
  }


  //-------------------------------------------------------------------
  //Retrieve conditions string for a single chamber from the database.
  StatusCode CscCoolStrSvc::getCoolChannelString(const CondAttrListCollection * atrc, int coolChannel, std::string& data) const 
  { 
    CondAttrListCollection::ChanNum channum= coolChannel;
    CondAttrListCollection::const_iterator itr=atrc->chanAttrListPair(channum);

    if (itr!=atrc->end()) {
      const coral::AttributeList& atr=itr->second;
      data=atr["Data"].data<std::string>();
    }
    else {
      m_log << MSG::DEBUG << "Couldn't find a requested COOL channel number." 
        << coolChannel << endmsg;
      return StatusCode::RECOVERABLE;
    }
    return StatusCode::SUCCESS;
  }


  //-------------------------------------------------------------------
  /** Merge and then write to cool database*/ 
  StatusCode CscCoolStrSvc::mergeAndSubmitCondDataContainer(const CscCondDataContainer * newCont) {

    m_log <<MSG::INFO 
      << "Merging provided csc conditions data into the COOL data string for writing to database." 
      << endmsg;
    //CscCondDataContainer mergedContainer
    CscCondDataContainer::const_iterator newItr = newCont->begin();
    CscCondDataContainer::const_iterator endItr = newCont->end();
    for(; newItr != endItr ; newItr++) {
      if(!(*newItr))
      {
        m_log << MSG::ERROR << "Empty element in container with new data. Can't merge" 
          << endmsg;
        return StatusCode::RECOVERABLE;
      }

      const CscCondDataCollectionBase * newColl = *newItr;

      string parName = newColl->getParName();
      map<string, CscCondDataCollectionBase*>::const_iterator refItr =  
        m_parNameMap.find(parName);
      if(refItr == m_parNameMap.end())
      {
        m_log << MSG::ERROR << "Parameter " << parName << " not loaded. Can't merge. Check your JobOptions." << endmsg;
      }
      const CscCondDataCollectionBase *refColl = refItr->second;


      string dataType = refColl->getParDataType();

      if(dataType != newColl->getParDataType())
      {
        m_log << MSG::ERROR << "When merging parameter " << refColl->getParName()
          << " found that new data has type " << newColl->getParDataType() 
          << " and reference has type " << dataType << ". quiting merging..."
          << endmsg;
        return StatusCode::RECOVERABLE;
      }

      //now merge them 
      if(dataType == "uint32_t") {
        if(!mergeCollections<uint32_t>(newColl, refColl).isSuccess()){
          m_log << MSG::ERROR << "Failed merging " << parName << endmsg;
          return StatusCode::RECOVERABLE;
        }
      }
      if(dataType == "int") {
        if(!mergeCollections<int>(newColl, refColl).isSuccess()){
          m_log << MSG::ERROR << "Failed merging " << parName << endmsg;
          return StatusCode::RECOVERABLE;
        }
      }
      if(dataType == "float") {
        if(!mergeCollections<float>(newColl, refColl).isSuccess()){
          m_log << MSG::ERROR << "Failed merging " << parName << endmsg;
          return StatusCode::RECOVERABLE;
        }
      }
      if(dataType == "bool") {
        if(!mergeCollections<bool>(newColl, refColl).isSuccess()){
          m_log << MSG::ERROR << "Failed merging " << parName << endmsg;
          return StatusCode::RECOVERABLE;
        }
      }
    }//end collection loop
    return StatusCode::SUCCESS;
  }


  //-------------------------------------------------------------------
  /** callback functions called whenever a database folder goes out of date*/
  StatusCode CscCoolStrSvc::callback( IOVSVC_CALLBACK_ARGS_P(/*I*/,keys))
  { //IOVSVC_CALLBACK_ARGS is (int& idx, std::list<std::string>& keylist)
    list<string>::const_iterator keyItr = keys.begin();
    list<string>::const_iterator keyEnd = keys.end();
    for(; keyItr != keyEnd; keyItr++) {
      if(!cacheParameter(*keyItr).isSuccess()) {
        m_log << MSG::WARNING << "Failed at caching key " << (*keyItr) << endmsg;
      }
    } 
    return StatusCode::SUCCESS;
  }


  //-------------------------------------------------------------------
  //Extracts parameter from database, reads it into the database mirror;
  StatusCode CscCoolStrSvc::cacheParameter(const std::string & parKey)
  {
    if(m_debug) m_log << MSG::DEBUG << "Caching parameter " << parKey << endmsg;
    ///*****//
    std::map<std::string, CscCondDataCollectionBase*>::iterator collItr = m_parSGKeyMap.find(parKey);

    if(collItr == m_parSGKeyMap.end())
    {
      m_log << MSG::ERROR << "Failed caching " << parKey 
        << ". It doens't seem to be registered." << endmsg;
      return StatusCode::RECOVERABLE;
    }

    CscCondDataCollectionBase * coll = dynamic_cast<CscCondDataCollectionBase *>( collItr->second);

    coll->reset(); //Clear vector and set to size dictated by maxIndex

    if(!m_doCaching)
      return StatusCode::SUCCESS;

    const unsigned int & numCoolChannels = coll->getNumCoolChan();


    const CondAttrListCollection * atrc = &*(coll->atrcHandle());

    //now cycle through all chambers in database;
    unsigned int numCoolChannelsFound = 0;

    for(unsigned int coolItr=1; coolItr <= numCoolChannels;  coolItr++)
    {
      if(m_debug) m_log << MSG::DEBUG << "Attempting to retrieve cool channel " 
        << coolItr << " with key " << parKey <<  endmsg;
      //retrieve datastring from db			
      string dataStr;
      if (StatusCode::SUCCESS != getCoolChannelString(atrc, coolItr, dataStr))
      {
        if(m_debug) m_log << MSG::DEBUG << "Couldn't find cool channel " << coolItr << endmsg;
        continue;
      }

      numCoolChannelsFound = numCoolChannelsFound + 1;


      if(m_debug) m_log << MSG::VERBOSE  << "For cool channel " << coolItr << ", String is \n[" << dataStr << "]" << endmsg;
      //now decode string into numeric parameters.
      istringstream ss(dataStr);

      if(!ss.good()) 
      {
        m_log << MSG::WARNING << "Failed forming stringstream during caching of " << coll->getParName() << endmsg;
        continue;
      }


      string version;

      ss >>  version;

      if(version == "1" or atoi(version.c_str()) == 1)
      {
        if(!cacheVersion1(ss,coll).isSuccess()) {
          m_log << MSG::FATAL <<  "Failed caching from COOL string." << endmsg;
          return StatusCode::FAILURE;
        }
      }
      else if(version == "02-00") {
        if(!cacheVersion2(ss,coll).isSuccess()) {
          m_log << MSG::FATAL <<  "Failed caching from COOL string." << endmsg;
          return StatusCode::FAILURE;
        }
      }
      else 
      {
        //Old version was treated as an actual number rather than string. It was always
        //set to 1 or sometimes 1.00000, so we convert to integer here and check
        m_log << MSG::WARNING << "Don't recognize CSC COOL string version " << version <<
          " for parameter " << coll->getParName() << ". Will treat as default version " << m_defaultDatabaseReadVersion << endmsg;
        if(m_defaultDatabaseReadVersion == "1"){
          if(!cacheVersion1(ss,coll).isSuccess()) {
            m_log << MSG::FATAL <<  "Failed caching from COOL." << endmsg;
            return StatusCode::FAILURE;
          }
        }
        else if(m_defaultDatabaseReadVersion == "02-00")
          if(!cacheVersion1(ss,coll).isSuccess()) {
            m_log << MSG::FATAL <<  "Failed caching from COOL." << endmsg;
            return StatusCode::FAILURE;
          }
      }
    }//end cool channel loop
    if(!numCoolChannelsFound) {
      m_log << MSG::ERROR << "Found no COOL channels!" << endmsg;
      return StatusCode::RECOVERABLE;
    }
    if(numCoolChannelsFound < numCoolChannels) {
      m_log << MSG::WARNING << "Only could retrieve " << numCoolChannelsFound << " out of " << numCoolChannels << " cool channels. This should never happen for a normal dataset, and could be a serious problem." << endmsg;
    }

    return StatusCode::SUCCESS;
  }//end cache parameter


  //-------------------------------------------------------------------
  StatusCode CscCoolStrSvc::cacheVersion1(istringstream & ss, CscCondDataCollectionBase * const coll) {

    if(m_debug) m_log << MSG::DEBUG << "Caching " << coll->getParName()
      << " (category " << coll->getParCat() << "). Database string is version 1" 
        << endmsg;
    if(coll->getParCat() != "CHANNEL")
    {
      m_log << MSG::ERROR << coll->getParCat()
        << " is not a valid parameter category for version 1!" << endmsg;
      return StatusCode::RECOVERABLE;
    }

    unsigned int numUpdated = 0;
    string indexStr,valueStr;

    const int & numEntries = coll->getSize();
    //Loop over strips in chamber. We don't usually expect i to reach maxIndex
    //We just have this upper limit to prevent a bug causing an endless loop.
    for(int cnt = 0; cnt < numEntries; cnt++)  
    {	
      ss >> indexStr;

      //If we read END for the next value, we have reached end of database
      //string. This is the expected way we'll exit this loop
      if(indexStr == "END")
        break; //reached end of database string

      int index = atoi(indexStr.c_str());

      if(m_phiSwapVersion1Strings){
        Identifier chanId;
        m_cscId->get_id((IdentifierHash)index, chanId, &m_channelContext);
        int stationEta = m_cscId->stationEta(chanId);          // +1 Wheel A   -1 Wheel C
        int measuresPhi = m_cscId->measuresPhi(chanId); // 0 eta 1 phi
        if(stationEta > 0 && measuresPhi ==1){
          int stationName = m_cscId->stationName(chanId);  // CSL or CSS
          int stationPhi = m_cscId->stationPhi(chanId); // PhiSector from 1-8
          int chamberLayer = m_cscId->chamberLayer(chanId); // Either 1 or 2 (but always 2)
          //if( chamberLayer == 1 ) chamberLayer = 2 ;//m_log << MSG::WARNING << "Bad chamberLayer " << endmsg;
          int wireLayer = m_cscId->wireLayer(chanId);  // layer in chamber 1-4
          int strip = 49 - m_cscId->strip(chanId);

          Identifier newId = m_cscId->channelID(stationName,stationEta,stationPhi,chamberLayer, wireLayer,measuresPhi,strip);
          IdentifierHash hash ;
          m_cscId->get_channel_hash(newId, hash);

          if(m_verbose) 
            m_log << MSG::VERBOSE <<  "Swapped phi strip "  
            << m_cscId->show_to_string(chanId) << " (" << index 
            << ") to " << m_cscId->show_to_string(newId) << " (" << hash << ")" << endmsg;

          index = hash;
        }
        else if (m_verbose) m_log << MSG::VERBOSE << "Not swapping " << m_cscId->show_to_string(chanId) << endmsg;
      }

      if(m_verbose) m_log << MSG::VERBOSE << "[cache version 1] Recording index " << index << " for paramter " << coll->getParName() << endmsg;

      //Put next word in string into collection as a data entry at [index]
      if(!coll->recordFromSS(ss, index).isSuccess())
      {
        m_log << MSG::WARNING << "Failed caching to index " << index << " for parameter "  << coll->getParName() << " (data string version 1). Likely tried to recache to same index twice." << endmsg;
      }

      if(m_debug) numUpdated++;
    }//end index loop

    if(m_debug) m_log << MSG::DEBUG << "Number chans updated for this channel is " << numUpdated << endmsg;

    return(StatusCode::SUCCESS);
  }//end cache version 1


  //-------------------------------------------------------------------
  StatusCode CscCoolStrSvc::cacheVersion2(istringstream & ss, CscCondDataCollectionBase * const coll) {

    if(m_debug) m_log << MSG::DEBUG << "Caching " << coll->getParName()
                      << " (category " << coll->getParCat() << "). Database string is version 2" 
                      << endmsg;
    unsigned int numUpdated = 0;
    string indexStr,valueStr;

    //get past remainder of header info
    while(ss.good()) {
      ss >> valueStr;
      if(valueStr == "<BEGIN_DATA>")
        break;
    }

    const string & cat = coll->getParCat();
    const int & numEntries = coll->getSize();
    string str = "";

    if ( cat!="ASM" )
    {
      //Loop over strips in chamber. We don't usually expect cnt to reach numEntries
      //We just have this upper limit to prevent a bug causing an endless loop.
      for(int cnt = 0; cnt < numEntries; cnt++) {
        ss >> str;  

        //See if at end of data string 
        if(str == "<END_DATA>")
          break;

        //Except for the CSC category, str now is holding an id string.
        //In these cases, we will now map it to category, and then read out 
        //next value
        unsigned int index = 0;
        if(cat != "CSC") {
          if(!stringIdToIndex(str, cat, index).isSuccess()) {
            m_log << MSG::ERROR << "Failed converting string Id to index in cacheVersion2"
                  << endmsg;
            return StatusCode::RECOVERABLE;
          }
          ss >> str;
        }

        if(m_verbose) m_log << MSG::VERBOSE << "[cache version 2 (CHANNEL) ] Recording " 
          << str << " at index " << index << endmsg;

        //Now str has a value in it. We pass it to the collection.
        istringstream valueSS(str);
        if(!coll->recordFromSS(valueSS, index).isSuccess()) {
          m_log << MSG::WARNING << "Failed caching to index " << index << " for parameter" 
            << coll->getParName()
            << " (data string version 2). Likely tried to recache to same index twice."
            << " Likely a bug when the data was orginally put in COOL." << endmsg;
        }
        if(m_debug) numUpdated++;
        if (cnt==(numEntries-1)) {
          m_log << MSG::ERROR << "Something wrong with <END_DATA>"
                << " in COOL tag in cacheVersion2" << endmsg;
          return StatusCode::RECOVERABLE;
        }
      }  // for cnt
    }  // if cat!="ASM"

    else  // cat=="ASM"
    { 
      std::string       asmIDstr;
      std::string       valueStr;

      int asmNum      = 0;
      int stationEta  = 0;
      int stationPhi  = 0;
      int stationName = 0;
      int measuresPhi = 0;
      int chamberLayer= 2;  //  Did not learn about this in time to avoid hard coding it.
                            //  (chamberLayer1 was never built.)
      int layerSince  = 0;
      int layerUntil  = 0;
      int stripSince  = 0;
      int stripUntil  = 0;

      Identifier chanId;
      unsigned int index = 0;
      IdentifierHash hashIdentifier;

      for ( int cnt = 0; cnt < numEntries; cnt++ )  //  Only works for chamberLayer=2
      {	ss >> asmIDstr;  /*  asm cool tag id string which is
          ASM[#:1-5]_[StationEtaString:AorC][stationPhi:1-8]_[stationName:50-51]
          xxx   3   x                  5                6   x             x9      */
        if(m_verbose) m_log << MSG::VERBOSE << "ASM ID String: " << asmIDstr << endmsg;
        if  ( asmIDstr == "<END_DATA>" )  break;

        asmNum = atoi(asmIDstr.substr(3,1).c_str());

        if  ( asmIDstr[5] == 'A' )  stationEta =  1;
        else if( asmIDstr[5] == 'C')   stationEta = -1;   
        else{
          m_log << MSG::FATAL << "Bad ASMID String in CSC COOL database \"" << asmIDstr << "\" (wheel " << asmIDstr[5] << " doesn't exist" << endmsg;
          return StatusCode::FAILURE;
        }
         
        stationPhi = atoi(asmIDstr.substr(6,1).c_str());

        stationName = atoi(asmIDstr.substr(8,2).c_str());
        
        if(stationPhi < 1 || stationPhi > 8 || stationName <50 || stationName > 51){
          m_log << MSG::FATAL << "Bad ASMID String in CSC COOL database: \"" << asmIDstr << "\""<< endmsg;

          m_log << MSG::FATAL << "Read station phi: " << stationPhi << ", stationName " << stationName << endmsg;
          return StatusCode::FAILURE;
        }

        if ( !getAsmScope(asmNum, measuresPhi, layerSince, layerUntil, stripSince, stripUntil) )
        { m_log << MSG::FATAL << "Failure of getAsmScope in cacheVersion2." << endmsg;
          return StatusCode::FAILURE;
        }

        ss >> valueStr;  // get value to put in parameter vector
        //std::stringstream valueSS(ss);

        //  Now for given asmID, loop over strip and layer
        for ( int iStrip = stripSince; iStrip < stripUntil; iStrip++ )
        { for ( int iLayer = layerSince; iLayer < layerUntil; iLayer++ )
          { chanId = m_cscId->channelID(stationName, stationEta, stationPhi, chamberLayer,
              iLayer, measuresPhi, iStrip);
          m_cscId->get_channel_hash(chanId, hashIdentifier);
          index = (int)hashIdentifier;
          if ( m_verbose ) m_log << MSG::VERBOSE << "[cache version 2 (ASM)] Recording " 
            << valueStr << " at index " << index 
              << "\nstationName " << stationName
              <<"\nstationEta " << stationEta
              << "\nstationPhi " << stationPhi 
              << "\nchamberLayer " << chamberLayer
              << "\niLayer " << iLayer
              << "\nmeasuresPhi " << measuresPhi
              << "\niStrip " << iStrip 
              << endmsg;
          //Now valueStr has a value in it. We pass it to the collection.
          istringstream valueSS(valueStr);
          if ( !coll->recordFromSS(valueSS, index).isSuccess() )
          { m_log << MSG::WARNING << "Failed caching to index " << index 
            << " for parameter " << coll->getParName()
              << " (data string version 2)."
              << " Likely tried to recache to same index twice."
              << " Likely a bug when the data was orginally put in COOL." << endmsg;
          }
          if ( m_debug ) numUpdated++;
          if ( cnt==(numEntries-1) )
          { m_log << MSG::ERROR << "Something wrong with <END_DATA> in"
            << " COOL tag in cacheVersion2" << endmsg;
            return StatusCode::RECOVERABLE;
          } } } } }  //  cat=="ASM"

          if ( m_debug ) m_log << MSG::DEBUG << "Number updated is " << numUpdated << endmsg;
          return StatusCode::SUCCESS;
  }  //  end cacheVersion2


  //-------------------------------------------------------------------
  int CscCoolStrSvc::swapChamberLayerReturnHash(const Identifier & id) const
  {
    int stationName = m_cscId->stationName(id);
    int stationEta = m_cscId->stationEta(id); 
    int stationPhi = m_cscId->stationPhi(id); 
    int chamberLayer = m_cscId->chamberLayer(id) == 1 ? 2 : 1; //Swap chamber layer
    int measuresPhi = m_cscId->measuresPhi(id); 
    int wireLayer = m_cscId->wireLayer(id);
    int strip = m_cscId->strip(id);
    Identifier newId = m_cscId->channelID(stationName, stationEta, stationPhi, 
        chamberLayer, wireLayer, measuresPhi, strip);
    IdentifierHash hash;
    m_cscId->get_channel_hash(newId, hash);
    m_log << MSG::INFO << "swap chamber layer " << m_cscId->show_to_string(id) 
          << " to " << m_cscId->show_to_string(newId) << " (" << hash << ")" << endmsg;
    return (int)hash;
  }


  //-------------------------------------------------------------------
  bool CscCoolStrSvc::ignoreBadMultilayer() const
  {
    static bool haveChecked = false;
    static bool ignoreIt = false;

    StoreGateSvc* detStore= 0;
    StatusCode sc = serviceLocator()->service("DetectorStore",detStore);
    if(sc.isFailure())
    {
      m_log << MSG::WARNING << "Failed to retrieve detector store in ignoreBadMultilayer()" << endmsg;
      return false;
    }

    if(!haveChecked)
    {
      if(m_debug) m_log << "First time running ignoreBadMultilayer(). Checking geo tag."
        << endmsg;
      haveChecked = true;
      //All geometries before the "ATLAS-GEO-xx-xx-xx" series had a bug where the
      //wrong multilayer was set. As long as we are in ATLAS-GEO range, we should print
      //a warning message if someone tries to request info from the wrong multilayer
      const DataHandle<TagInfo> tagInfo;
      if(detStore->retrieve(tagInfo).isFailure()) {
        m_log << MSG::WARNING << "Could not retrieve tag info from TDS in ignore bad multilayer..." 
          << endmsg;;
        return false;
      }

      std::string detdescr = "";
      tagInfo->findTag("GeoAtlas", detdescr);
      if(m_debug) m_log << MSG::DEBUG << "DetDescr tag = " << detdescr << endmsg;
      if ( detdescr.find ("ATLAS-") != std::string::npos )
      {
        //Will print warning messages when bad multilayer requested in getParameter()
        ignoreIt = false;
      } 
      else
        ignoreIt = true;
    }
    return ignoreIt;
  }//end ignoreBadMultilayer


  //-------------------------------------------------------------------
  StatusCode CscCoolStrSvc::offlineToOnlineId(const Identifier & id, unsigned int &onlineId) const
  {

    onlineId = 0;
    //Phi,wireLayer,and strip all are offset by one between the two schemes.
    //Also, station name is 50 or 51 in Identifiers, but only 0 or 1 in 
    //the online id.
    int stationName  	((m_cscId->stationName(id) -50)&0x1 );		// 0001 0000 0000 0000 0000
    int phi =   		(m_cscId->stationPhi(id) - 1)&0x7  ;		    // 0000 1110 0000 0000 0000
    int eta = 		((m_cscId->stationEta(id) == 1) ? 1:0) &0x1;  // 0000 0001 0000 0000 0000
    int chamLay = 		(m_cscId->chamberLayer(id)-1) &0x1;		    // 0000 0000 1000 0000 0000
    int wireLay = 		(m_cscId->wireLayer(id)-1) &0x3;		      // 0000 0000 0110 0000 0000
    int measuresPhi = 	(m_cscId->measuresPhi(id) &0x1);		    // 0000 0000 0001 0000 0000
    int strip;     		                                          // 0000 0000 0000 1111 1111

    //Online and offline phi ids are flipped on A wheel
    if(m_onlineOfflinePhiFlip && measuresPhi && eta == 1){
      strip = (48 - (m_cscId->strip(id))) & 0xff;  
    }
    else {
      strip = (m_cscId->strip(id)-1) & 0xff;     		     
    }


    onlineId 	+= (stationName << 16);	// 0001 0000 0000 0000 0000
    onlineId        += (phi << 13) ;	// 0000 1110 0000 0000 0000
    onlineId	+= (eta <<12);  	      // 0000 0001 0000 0000 0000
    onlineId 	+= (chamLay <<11);	    // 0000 0000 1000 0000 0000
    onlineId	+= (wireLay << 9);	    // 0000 0000 0110 0000 0000
    onlineId	+= (measuresPhi << 8);	// 0000 0000 0001 0000 0000
    onlineId	+= strip ;     		      // 0000 0000 0000 1111 1111
    return StatusCode::SUCCESS;
  }


  //------------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::offlineToAsmId(const Identifier & id, std::string & AsmId,
      unsigned int & iChamber, unsigned int & iASM) const
  { 
    int stationEta  = m_cscId->stationEta(id);
    std::string stationEtaString  = (stationEta == 1 ? "A":"C");
    int stationPhi  = m_cscId->stationPhi(id);
    int stationName = m_cscId->stationName(id);
    int wireLayer   = m_cscId->wireLayer(id);
    int measuresPhi = m_cscId->measuresPhi(id);
    int strip       = m_cscId->strip(id);

    iChamber = getChamberCoolChannel(id) - 1; //0-31

    if (measuresPhi==1)  iASM=5;
    else if (strip<=96)
    { 
      if (wireLayer<=2)  iASM=1;
      else  iASM=2;
    }
    else
    { 
      if (wireLayer<=2)  iASM=3;
      else  iASM=4;
    }
    /* Never gonna reach this code
    else
    { 
      m_log << MSG::ERROR << "Could not assign iASM in CscCoolStrSvc::offlineToAsmId" << endmsg;
      return StatusCode::RECOVERABLE;
    }
    */

    std::stringstream ss;
    ss << "ASM" << iASM << "_" << stationEtaString << stationPhi << "_" << stationName;
    AsmId = ss.str();

    return StatusCode::SUCCESS;
  }

  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::offlineElementToOnlineId(const Identifier & id, unsigned int &onlineId) const
  {
    onlineId = 0;
    //Phi,wireLayer,and strip all are offset by one between the two schemes.
    //Also, station name is 50 or 51 in Identifiers, but only 0 or 1 in 
    //the online id.
    int stationName  	((m_cscId->stationName(id) -50)&0x1 );		// 0001 0000 0000 0000 0000
    int phi =   		(m_cscId->stationPhi(id) - 1)&0x7  ;		// 0000 1110 0000 0000 0000
    int eta = 		((m_cscId->stationEta(id) == 1) ? 1:0) &0x1;  	// 0000 0001 0000 0000 0000
    int chamLay = 		1;		// 0000 0000 1000 0000 0000
    int wireLay = 		0;		// 0000 0000 0110 0000 0000
    int measuresPhi = 0;		// 0000 0000 0001 0000 0000
    int strip = 		0;     	// 0000 0000 0000 1111 1111

    onlineId 	+= (stationName << 16);	// 0001 0000 0000 0000 0000
    onlineId        += (phi << 13) ;	// 0000 1110 0000 0000 0000
    onlineId	+= (eta <<12);  	      // 0000 0001 0000 0000 0000
    onlineId 	+= (chamLay <<11);	    // 0000 0000 1000 0000 0000
    onlineId	+= (wireLay << 9);	    // 0000 0000 0110 0000 0000
    onlineId	+= (measuresPhi << 8);	// 0000 0000 0001 0000 0000
    onlineId	+= strip ;     	      	// 0000 0000 0000 1111 1111
    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::onlineToOfflineIds(const unsigned int & onlineId, Identifier &elementId, Identifier &channelId) const
  {
    int stationName =       ((onlineId >> 16)&0x1) + 50;
    int phi =               ((onlineId >> 13)&0x7)+1;
    int eta =               ((((onlineId >> 12)&0x1) == 1) ? 1:-1);
    int chamLay =           ((onlineId>>11)&0x1) +1;
    int wireLay =           ((onlineId>>9)&0x3) +1;
    int measuresPhi =       ((onlineId >> 8)&0x1);
    int strip;

    //Online and offline phi ids are flipped on A wheel
    if(m_onlineOfflinePhiFlip && measuresPhi && eta == 1){
      strip = 48 - ((onlineId)&0xff) ; //equivalent: 49 -( onlineId&0xff +1)
    }
    else {
      strip = ((onlineId)&0xff) +1;
    }

    elementId = m_cscId->elementID(stationName,eta,phi);
    channelId = m_cscId->channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip);

    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::onlineToOfflineElementId(const unsigned int & onlineId, Identifier &elementId) const
  {
    int stationName =       ((onlineId >> 16)&0x1) + 50;
    int phi =               ((onlineId >> 13)&0x7)+1;
    int eta =               ((((onlineId >> 12)&0x1) == 1) ? 1:-1);

    elementId = m_cscId->elementID(stationName,eta,phi);

    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::onlineToOfflineChannelId(const unsigned int & onlineId, Identifier &chanId) const
  {
    int stationName =       ((onlineId >> 16)&0x1) + 50;
    int phi =               ((onlineId >> 13)&0x7)+1;
    int eta =               ((((onlineId >> 12)&0x1) == 1) ? 1:-1);
    int chamLay =           ((onlineId>>11)&0x1) +1;
    int wireLay =           ((onlineId>>9)&0x3) +1;
    int measuresPhi =       ((onlineId >> 8)&0x1);
    int strip;

    //Online and offline phi ids are flipped on A wheel
    if(m_onlineOfflinePhiFlip && measuresPhi && eta == 1){
      strip = 48 - ((onlineId)&0xff) ; //equivalent: 49 -( onlineId&0xff +1)
    }
    else {
      strip = ((onlineId)&0xff) +1;
    }

    chanId = m_cscId->channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip);

    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  unsigned int CscCoolStrSvc::getLayerHash( const Identifier & id) const 
  {
    unsigned int stationName = m_cscId->stationName(id);
    if(m_cscId->stationName(id) >= 50 ) stationName = stationName - 50;
    else {
      m_log <<MSG::ERROR << "stationName: " << stationName << " is not CSC - emergency stop." << endmsg;
      throw;
    }
    unsigned int stationEta = (m_cscId->stationEta(id) == 1 ? 1 :0);
    unsigned int stationPhi = m_cscId->stationPhi(id) -1; 
    unsigned int wireLayer = m_cscId->wireLayer(id) -1;
    unsigned int measuresPhi = m_cscId->measuresPhi(id);

    return m_layerHashes[stationName][stationEta][stationPhi][wireLayer][measuresPhi];
  }


  //-----------------------------------------------------------------------------------
  unsigned int CscCoolStrSvc::getChamberCoolChannel( const Identifier & id) const
  {

    unsigned int stationName = m_cscId->stationName(id) -50;
    unsigned int eta = (m_cscId->stationEta(id) == 1 ? 1 : 0);
    unsigned int phi = m_cscId->stationPhi(id) -1; 

    if(stationName > 1
        || phi > 7
        || eta > 1
      ) {
      m_log <<MSG::ERROR << "when creating chamber cool channel, inputs were:\nstationName: " 
        << stationName 
        << "\nPhi: " << phi
        << "\neta: " << eta
        << endmsg;
      return 1;
    }

    return m_chamberCoolChannels[stationName][eta][phi];
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::onlineIdToLayerHash(const unsigned int & onlineId, int & layerHash ) const
  {
    unsigned int stationName =      ((onlineId >> 16)&0x1);
    unsigned int phi =              ((onlineId >> 13)&0x7);
    unsigned int eta =              (((onlineId >> 12)&0x1));
    unsigned int wireLay =          ((onlineId>>9)&0x3);
    unsigned int measuresPhi =      ((onlineId >> 8)&0x1);

//    according to coverity this if never becomes true (defect 11296)
//    if(stationName > 1
//        || phi > 7
//        || eta > 1
//        || wireLay > 3
//        || measuresPhi > 1
//      )
//      return StatusCode::RECOVERABLE;

    layerHash = m_layerHashes[stationName][eta][phi][wireLay][measuresPhi];

    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::layerHashToOnlineId(const unsigned int & layerHash, unsigned int & onlineId) const {
    if(layerHash > m_onlineChannelIdsFromLayerHash.size())
    {
      m_log << MSG::ERROR << "Tried to lookup online id from layer hash " 
        << layerHash <<". Max is " << m_onlineChannelIdsFromLayerHash.size()<<  endmsg;
      return StatusCode::SUCCESS;
    }
    onlineId = m_onlineChannelIdsFromLayerHash[layerHash];

    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::onlineIdToCoolChamberChannel(const unsigned int & onlineId, unsigned int & chamCoolChan ) const
  {
    unsigned int stationName =      ((onlineId >> 16)&0x1);
    unsigned int phi =              ((onlineId >> 13)&0x7);
    unsigned int eta =              (((onlineId >> 12)&0x1));

    /* According to coverity 105392, this never becomes true
    if(stationName > 1
        || phi > 7
        || eta > 1
      ) {
      m_log <<MSG::ERROR << "when creating chamber cool channel, inputs were:\nstationName: " 
        << stationName 
        << "\nPhi: " << phi
        << "\neta: " << eta
        << endmsg;
      return StatusCode::RECOVERABLE;
    }
    */

    chamCoolChan = m_chamberCoolChannels[stationName][eta][phi];
    if(chamCoolChan < 1 || chamCoolChan > 32) {
      m_log << MSG::ERROR << "created chamber cool channel is " 
        << chamCoolChan << endmsg;
      return StatusCode::RECOVERABLE;
    }
    return StatusCode::SUCCESS;
  }

  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::coolChamberChannelToOnlineId(const unsigned int & chamCoolChan, unsigned int & onlineId ) const {

    if(chamCoolChan > m_onlineChannelIdsFromChamberCoolChannel.size()
        || chamCoolChan ==0 ) {
      m_log << "Requested online ID for chamber Cool Channel " << chamCoolChan 
        <<"which can't be more than " << m_onlineChannelIdsFromChamberCoolChannel.size();
    }
    onlineId = m_onlineChannelIdsFromChamberCoolChannel[chamCoolChan-1];
    return StatusCode::SUCCESS;
  }
  /*
     indexToCoolChan(const unsigned int & index, std::string & cat,  unsigned int & coolChan) {


     }
   */


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::getParCat(const std::string & parName, std::string & val) const{
    map<std::string, CscCondDataCollectionBase*>::const_iterator itr = m_parNameMap.find(parName);
    if(itr == m_parNameMap.end())
      return StatusCode::RECOVERABLE;

    val = itr->second->getParCat();
    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::getParDataType(const std::string & parName, std::string & val) const {
    map<std::string, CscCondDataCollectionBase*>::const_iterator itr = m_parNameMap.find(parName);
    if(itr == m_parNameMap.end())
      return StatusCode::RECOVERABLE;

    val = itr->second->getParDataType();
    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::getParFolder(const std::string & parName, std::string & val) const { 
    map<std::string, CscCondDataCollectionBase*>::const_iterator itr = m_parNameMap.find(parName);
    if(itr == m_parNameMap.end())
      return StatusCode::RECOVERABLE;

    val = itr->second->getParFolder();
    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::getParNumHashes(const std::string & parName, unsigned int & val) const { 
    map<std::string, CscCondDataCollectionBase*>::const_iterator itr = m_parNameMap.find(parName);
    if(itr == m_parNameMap.end())
      return StatusCode::RECOVERABLE;

    val = itr->second->getSize();
    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::stringIdToIndex(const std::string & idString, const std::string & cat, unsigned int & index) const
  {
    if(cat == "CSC") {
      index = 0;
      return StatusCode::SUCCESS;
    } 

    if(cat == "ENDCAP")
    {

      if(idString =="1")
        index = 1;
      else if(idString=="-1"|| idString =="0")
        index = 0;
      else
      {
        m_log << MSG::ERROR << "Unknown idString of " << idString 
          << " asked for ENDCAP cateogry." << endmsg;
        return StatusCode::RECOVERABLE;
      }

      return StatusCode::SUCCESS;
    }   

    istringstream ss(idString);
    unsigned int chanAddress;
    ss >> hex >> chanAddress;

    //remaining categories need offline identifiers
    Identifier chamberId;
    Identifier channelId;
    if(!onlineToOfflineIds(chanAddress, chamberId, channelId).isSuccess()){
      m_log << MSG::ERROR << "Cannon get offline Ids from online Id" << hex << chanAddress << dec << endmsg; 
    }

    if(cat == "CHAMBER")
    {
      IdentifierHash chamberHash;
      m_cscId->get_module_hash(chamberId,chamberHash);
      index = (unsigned int)chamberHash; 
    } 
    else if(cat == "LAYER")
    {
      index = getLayerHash(channelId);
    }
    else if(cat == "CHANNEL")
    {
      IdentifierHash chanHash;
      m_cscId->get_channel_hash(channelId, chanHash);
      index = (unsigned int)chanHash;
    }

    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::indexToStringId(const unsigned int & index, const std::string & cat,  std::string & idString) const {

    //There is no string id for the CSC category.
    if(cat == "CSC") {
      idString = "";
      return StatusCode::SUCCESS;
    } 
    if(cat == "ENDCAP") {
      if(index == 0)
        idString = "-1";
      if(index == 1)
        idString = "1";
      else {
        m_log << MSG::ERROR << "Requested index " << index 
          << " can't be converted to a string Id for the category " << cat << endmsg;
        return StatusCode::RECOVERABLE;
      }
    }

    //remaining categories need online identifiers
    unsigned int onlineId = 0;
    stringstream ss;
    if(cat == "CHAMBER")
    {

      Identifier chamberId;
      m_cscId->get_id(IdentifierHash(index), chamberId, &m_moduleContext);
      if(!offlineElementToOnlineId(chamberId, onlineId).isSuccess()) {
        m_log << MSG::ERROR 
          << "Failed converting chamber identifier to online id during stringId gen. " 
          << endmsg;
        return StatusCode::RECOVERABLE;
      }
    } 
    else if(cat == "LAYER")
    {
      unsigned int onlineId;
      if(!layerHashToOnlineId(index, onlineId)){
        m_log <<MSG::ERROR
          << "Failed at getting online id from layer hash during stringId gen" 
          << endmsg;
      }
    }
    else if(cat == "CHANNEL")
    {
      Identifier channelId;
      m_cscId->get_id(IdentifierHash(index), channelId, &m_channelContext);
      if(!offlineToOnlineId(channelId, onlineId).isSuccess()) {
        m_log << MSG::ERROR
          << "Failed converting chamber identifier to online id during stringId gen. "
          << endmsg;
        return StatusCode::RECOVERABLE;
      }
    }

    ss << hex << setfill('0') << setw(5) <<  onlineId << dec;
    idString = ss.str();

    return StatusCode::SUCCESS;
  }//end index to string id


  //-----------------------------------------------------------------------------------
  //give us a way to interact with this in a patch ;
  int CscCoolStrSvc::Access(std::vector<std::string>* /* strVec*/,std::vector<int>* /*intVec*/, std::vector<float>* /*floatVector*/) {
    return 0;
  }


  //-----------------------------------------------------------------------------------
  //Get details from ASM Number
  StatusCode CscCoolStrSvc::getAsmScope(int asmNum, int &measuresPhi,  int & layerSince, int & layerUntil, int & stripSince , int & stripUntil){
    if(asmNum == 1 ){
      stripSince = 1;  //inclusive
      stripUntil = 97; //exclusive
      layerSince = 1; //inclusive
      layerUntil = 3; //exclusive
      measuresPhi = 0;
    }
    else if(asmNum == 2){
      stripSince = 1;  //inclusive
      stripUntil = 97; //exclusive
      layerSince = 3; //inclusive
      layerUntil = 5; //exclusive
      measuresPhi = 0;
    }
    else if(asmNum == 3){
      stripSince = 97; 
      stripUntil = 193;
      layerSince = 1;
      layerUntil = 3;
      measuresPhi = 0;
    }
    else if( asmNum == 4){
      stripSince = 97; 
      stripUntil = 193;
      layerSince = 3;
      layerUntil = 5;
      measuresPhi = 0;
    }
    else if(asmNum == 5){
      stripSince = 1;
      stripUntil = 49;
      layerSince = 1;
      layerUntil = 5;
      measuresPhi = 1;
    }
    else{
      m_log << MSG::FATAL << "ASM  number  \"" << asmNum << "\" is invalid. It needs to end in a number from 1-5." << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

} //end MuonCalib namespace


