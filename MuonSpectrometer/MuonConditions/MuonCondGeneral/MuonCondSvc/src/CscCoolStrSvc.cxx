/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Service designed to read in calibration files to the cool database. Can also read them
//back out again to check.
// author lampen@physics.arizona.edu

#include "CscCoolStrSvc.h"

#include <sstream>
#include <vector>
#include <TString.h> // for Form
#include <algorithm>//for transform
#include <cctype> //for toupper
#include <inttypes.h> 
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "MuonCondData/CscCondDataCollection.h"
#include "MuonCondData/CscCondDataContainer.h"
#include "StoreGate/DataHandle.h"

namespace MuonCalib {
  CscCoolStrSvc::CscCoolStrSvc(const std::string& name, ISvcLocator* svc) :
    AthService(name,svc),
    p_detstore(nullptr),
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
    declareProperty("ReadPSlopeFromDatabase", m_pslopeFromDB = false);
    declareProperty("PSlope", m_pslope = m_DEFAULT_PSLOPE);
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
    ATH_MSG_DEBUG("Initializing CscCoolStrSvc");
    ATH_CHECK(service("DetectorStore",p_detstore));
    ATH_CHECK(m_idHelperSvc.retrieve());

    m_moduleContext = m_idHelperSvc->cscIdHelper().module_context();
    m_channelContext = m_idHelperSvc->cscIdHelper().channel_context();

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
              Identifier id = m_idHelperSvc->cscIdHelper().channelID(
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
                ATH_MSG_ERROR("Failed at geting online id!"); 
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
              Identifier id = m_idHelperSvc->cscIdHelper().channelID(
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
                ATH_MSG_ERROR("Failed at geting online id!"); 
                return StatusCode::RECOVERABLE;
              }

          m_onlineChannelIdsFromChamberCoolChannel.push_back(onlineId);
          m_chamberCoolChannels[stationName][stationEta][stationPhi] = hash++;
        }
      }
    }
    *(const_cast<unsigned int*>(&m_maxChamberCoolChannel)) = hash - 1; //-1 because hash overshoots in loop
    *(const_cast<unsigned int*>(&m_maxChanHash)) = m_idHelperSvc->cscIdHelper().channel_hash_max() - 1;

    if(msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("Maximum Chamber hash is " << m_maxChamberHash);
      ATH_MSG_DEBUG("Maximum Chamber COOL Channel is "<< m_maxChamberCoolChannel);
      ATH_MSG_DEBUG("Maximum Layer hash is " << m_maxLayerHash);
      ATH_MSG_DEBUG("Maximum Channel hash is " << m_maxChanHash);
    }


    ////////////
    // Loop over csc detector elements and add in the hash ids
    std::vector<Identifier> modules = m_idHelperSvc->cscIdHelper().idVector();

    //Prepare local cache.
    m_dbCache = new CscCondDataContainer();
    m_dbCache->clear(); 

    unsigned int numPars = m_parNameVec.size(); 
    if(numPars == 0) {
      ATH_MSG_WARNING("No parameters requested for CscCoolStrSvc. No COOL CSC data will be available from this service.");
    }

    //Ensure all vectors have exactly numPars
    if( m_parSGKeyVec.size() != numPars 
        || m_parFolderVec.size() != numPars
        || m_parDataTypeVec.size() != numPars
        || m_parCatVec.size() != numPars 
        || m_parDefaultVec.size() != numPars
      ) {
      ATH_MSG_FATAL("Need identical number of entries in each parameter definition vector!"
        << "\nParNames:\t" << m_parNameVec.size() 
        << "\nParSGKeys:\t" << m_parSGKeyVec.size()
        << "\nParFolders:\t" << m_parFolderVec.size()
        << "\nParDataTypes:\t" << m_parDataTypeVec.size()
        << "\nParCats:\t" << m_parCatVec.size()
        << "\nParDefault:\t" << m_parDefaultVec.size());
      return StatusCode::FAILURE;
    }

    if (m_pslopeFromDB) {
      ATH_MSG_WARNING("You have activated the retrieval of the pslope per CSC channel from the COOL database. "
            << "Please make sure that a correct PSLOPE database is in place which uses geometrical CSC hashes in hex format "
            << "as keys and different values of the pslopes for the different CSC channels as values, otherwise please run "
            << "with the ReadPSlopeFromDatabase property set to false");
    } else {
      if (!(m_pslope>0 && m_pslope<1)) {
        ATH_MSG_FATAL("The Pslope cannot be set to a value <=0 or >=1");
        return StatusCode::FAILURE;
      } else if (m_pslope != m_DEFAULT_PSLOPE) {
        ATH_MSG_WARNING("You have manually set the Pslope property (to " << m_pslope << "). Please check if this is really intended.");
      }
    }

    //Initialized each parameter
    for(unsigned int parItr = 0 ; parItr < numPars; parItr++)
    {

      std::string name = m_parNameVec[parItr];
      std::string sgKey = m_parSGKeyVec[parItr];
      std::string folder = m_parFolderVec[parItr] ;
      std::string dataType = m_parDataTypeVec[parItr]; 
      std::string category = m_parCatVec[parItr]; 
      std::string defaultVal = m_parDefaultVec[parItr] ;

      if (!m_pslopeFromDB) {
        // in this case, we do not need to read the pslopes from the database, thus, we do not need a CscCondDataCollection
        if (sgKey=="CSC_PSLOPE") continue;
      }

      if(msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Entering new parameter." 
        << "\nName:\t" << name
          << "\nStoreGateKey:\t" << sgKey
          << "\nDataType:\t" << dataType
          << "\nCategory:\t" << category
          << "\nDefaultVal:\t" << defaultVal
          << "\nSgKey:\t" << sgKey 
          << "\nFolder:\t" << folder
         );

      if(m_parNameMap.count(name)) {
        ATH_MSG_WARNING("Multiple parameters with name " << name
          << ". This isn't allowed! Skipping extra entries.");
        continue;
      }

      if(m_parSGKeyMap.count(sgKey)) {
        ATH_MSG_WARNING("Multiple parameters with storegate key " << sgKey 
          << ". This isn't allowed! Skipping extra entries.");
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
        ATH_MSG_WARNING("Don't recognize requested data type " 
          << dataType);
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
        ATH_MSG_WARNING("Failed to register " << name << ". Continuing without...");
        continue;
      }



      //Store in container so it could (potentially) be persitified. Also will
      //handle cleanup for us.
      m_dbCache->push_back(coll);

      //Store the pointers in maps for easy lookup later
      m_parNameMap[name] = coll;
      m_parSGKeyMap[sgKey] = coll;
      
      if( name == "rms" ) m_rmsCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( m_pslopeFromDB && name == "pslope" ) m_slopeCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == "noise" ) m_noiseCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == "f001" ) m_f001CondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == "ped" ) m_pedestalCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == "t0phase" ) m_t0PhaseCondData = dynamic_cast<CscCondDataCollection<bool>*>(coll);
      else if( name == "t0base" ) m_t0BaseCondData = dynamic_cast<CscCondDataCollection<float>*>(coll);
      else if( name == m_defaultChanStatusName ) {
        m_statusCondData = dynamic_cast<CscCondDataCollection<uint32_t>*>(coll);
        if( !m_statusCondData ) ATH_MSG_WARNING("Wrong data type for status bit " << dataType);
      }else  ATH_MSG_WARNING("Data type not cached, a direct access should be provided " << name);
      
      const DataHandle<CondAttrListCollection> & dataHandle = coll->atrcHandle();

      //Registering callback function. The callback funciton will now be called
      //whenever the parameter in question is altered. i.e. whenever it goes
      //into a new interval of validity.
      if(msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Registering " << name << " with storegate key " << sgKey);

      if(m_doCaching)
        if(StatusCode::SUCCESS != p_detstore->regFcn(&CscCoolStrSvc::callback,this,
              dataHandle, sgKey, true)) {
          ATH_MSG_ERROR("Failed to register parameter " << name << " with storeGate Key " << sgKey);
        }

      //Precaching. We shouldn't need to do this, but some rare cases it seems the callbacks are not called in time for the begining of the run.
      if(m_preCache)
        cacheParameter(sgKey).ignore();

      //   anySucceed = true;

    }//end register parameter loop

    return StatusCode::SUCCESS;
  }//end initialize()

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
      ATH_MSG_WARNING("Don't recognize category " << cat << " as a parameter category. ");
    }

    coll->reset(); //clears collection

    return StatusCode::SUCCESS; 
  }//end initCscCondDataCollection


  //-------------------------------------------------------------------
  //Checks that there is data for a particular index of a parameter
  int CscCoolStrSvc::checkIndex(const std::string & parName, unsigned int& index) const {

    std::map<std::string, CscCondDataCollectionBase*>::const_iterator itr =  
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
      ATH_MSG_WARNING(" No data available for hash " << index);
      return false;
    }
    val = condData->getValue();
    return true;
  }
  bool CscCoolStrSvc::getVal( bool& val, const CscCondDataCollection<bool>& coll, unsigned int index) const {
    const CscCondData<bool>* condData = coll[index];
    if( !condData ){
      ATH_MSG_WARNING(" No data available for hash " << index);
      return false;
    }
    val = condData->getValue();
    return true;
  }

  bool CscCoolStrSvc::getVal(uint32_t& val, const CscCondDataCollection<uint32_t>& coll, unsigned int index) const {
    const CscCondData<uint32_t>* condData = coll[index];
    if( !condData ){
      ATH_MSG_WARNING(" No data available for hash " << index);
      return false;
    }
    val = condData->getValue();
    return true;
  }

  bool CscCoolStrSvc::getRMS( float& val, const unsigned int & index) const {
    if( !m_rmsCondData ) {
      ATH_MSG_WARNING(" No RMS data available");
      return false;
    }
    return getVal(val,*m_rmsCondData,index);
  }
  bool CscCoolStrSvc::getSlope( float& val, const unsigned int & index) const {
    if (!m_pslopeFromDB) {
      val = m_pslope;
      return true;
    } else if( !m_slopeCondData ) {
      ATH_MSG_WARNING(" No slope data available");
      return false;
    }
    return getVal(val,*m_slopeCondData,index);
  }
  bool CscCoolStrSvc::getF001( float& val, const unsigned int & index) const {
    if( !m_f001CondData ) {
      ATH_MSG_WARNING(" No f001 data available");
      return false;
    }
    return getVal(val,*m_f001CondData,index);
  }
  bool CscCoolStrSvc::getPedestal( float& val, const unsigned int & index) const {
    if( !m_pedestalCondData ) {
      ATH_MSG_WARNING(" No pedestal data available");
      return false;
    }
    return getVal(val,*m_pedestalCondData,index);
  }
  bool CscCoolStrSvc::getT0Base( float& val, const unsigned int & index) const {
    if( !m_t0BaseCondData ) {
      ATH_MSG_WARNING(" No t0base data available");
      return false;
    }
    return getVal(val,*m_t0BaseCondData,index);
  }
  bool CscCoolStrSvc::getT0Phase( bool& val, const unsigned int & index) const {
    if( !m_t0PhaseCondData ) {
      ATH_MSG_WARNING(" No t0phase data available");
      return false;
    }
    return getVal(val,*m_t0PhaseCondData,index);
  }
  bool CscCoolStrSvc::getNoise( float& val, const unsigned int & index) const {
    if( !m_noiseCondData ) {
      ATH_MSG_WARNING(" No noise data available");
      return false;
    }
    return getVal(val,*m_noiseCondData,index);
  }

  //Returns the status. This is only here for backwards compatibility
  bool CscCoolStrSvc::getStatus(uint32_t &val, const unsigned int &index) const  {
    if( !m_statusCondData ) {
      ATH_MSG_WARNING(" No status data available");
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
    
     ATH_MSG_WARNING(" The use of this function is very expensive, please use the direct call instead: parName " 
           << parName);
    return sc;
  }

  StatusCode CscCoolStrSvc::getParameter(uint16_t &retData, const std::string &parName, const unsigned int & index) const 
  {
    //Only have uint8_t, so they probably meant that...
    uint32_t data;
    StatusCode sc;
    sc =  getParameterTemplated(data, parName,index);
    retData = data;
     ATH_MSG_WARNING(" The use of this function is very expensive, please use the direct call instead: parName " 
           << parName);
    return sc;
  }

  StatusCode CscCoolStrSvc::getParameter(uint32_t &retData, const std::string &name, const unsigned int & index) const 
  {
    //We only have uint8_t parameters, so they probably meant that...
     StatusCode sc = getParameterTemplated(retData, name, index);
     if( name == m_defaultChanStatusName ) {
       uint32_t val = 0;
       if( !getStatus( val, index) ) {
         ATH_MSG_WARNING(" Failed to retrieve data " << name);
       }
       if( val != retData ){
         ATH_MSG_WARNING(" Bad conversion of rms " << retData << " --> " << val << " " << name);
       }
     }else ATH_MSG_WARNING("Data type not cached, a direct access should be provided " << name);
     return sc;
  }

  StatusCode CscCoolStrSvc::getParameter(int &retData, const std::string &parName, const unsigned int & index) const 
  {
     ATH_MSG_WARNING(" The use of this function is very expensive, please use the direct call instead: parName " 
           << parName);
    return getParameterTemplated(retData, parName, index);
  }

  StatusCode CscCoolStrSvc::getParameter(float &retData, const std::string &name, const unsigned int & index) const 
  {
     StatusCode sc = getParameterTemplated(retData, name, index);
     if( name == "rms" ){
       float val = 0;
       if( !getRMS( val, index) ) {
         ATH_MSG_WARNING(" Failed to retrieve data " << name);
       }
       if( val != retData ){
         ATH_MSG_WARNING(" Bad conversion of rms " << retData << " --> " << val << " " << name);
       }
     }else if( name == "pslope" ){
       float val = 0;
       if( !getSlope( val, index) ) {
                  ATH_MSG_WARNING(" Failed to retrieve data " << name);
       }
       if( val != retData ){
         ATH_MSG_WARNING(" Bad conversion of rms " << retData << " --> " << val << " " << name);
       }       
     }else if( name == "noise" ){
       float val = 0;
       if( !getNoise( val, index) ) {
                  ATH_MSG_WARNING(" Failed to retrieve data " << name);
       }
       if( val != retData ){
         ATH_MSG_WARNING(" Bad conversion of rms " << retData << " --> " << val << " " << name);
       }       
     }else if( name == "f001" ) {
       float val = 0;
       if( !getF001( val, index) ) {
                  ATH_MSG_WARNING(" Failed to retrieve data " << name);
       }
       if( val != retData ){
         ATH_MSG_WARNING(" Bad conversion of rms " << retData << " --> " << val << " " << name);
       }       
     }else if( name == "ped" ) {
       float val = 0;
       if( !getPedestal( val, index) ) {
                  ATH_MSG_WARNING(" Failed to retrieve data " << name);
       }
       if( val != retData ){
         ATH_MSG_WARNING(" Bad conversion of rms " << retData << " --> " << val << " " << name);
       }       
     }else if( name == "t0base" ){
       float val = 0;
       if( !getT0Base( val, index) ) {
                  ATH_MSG_WARNING(" Failed to retrieve data " << name);
       }
       if( val != retData ){
         ATH_MSG_WARNING(" Bad conversion of rms " << retData << " --> " << val << " " << name);
       }       
     }
     return sc;
  }
  
  StatusCode CscCoolStrSvc::getParameter(bool &retData, const std::string &name, const unsigned int & index) const 
  {
    if( name == "t0phase" ) {
      bool val = false;
      if( !getT0Phase( val, index) ) {
        ATH_MSG_WARNING(" Failed to retrieve data " << name);
      }
      if( val != retData ){
        ATH_MSG_WARNING(" Bad conversion of rms " << retData << " --> " << val << " " << name);
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
      ATH_MSG_DEBUG("Couldn't find a requested COOL channel number." 
        << coolChannel);
      return StatusCode::RECOVERABLE;
    }
    return StatusCode::SUCCESS;
  }


  //-------------------------------------------------------------------
  /** Merge and then write to cool database*/ 
  StatusCode CscCoolStrSvc::mergeAndSubmitCondDataContainer(const CscCondDataContainer * newCont) {

    ATH_MSG_DEBUG("Merging provided csc conditions data into the COOL data string for writing to database.");
    //CscCondDataContainer mergedContainer
    CscCondDataContainer::const_iterator newItr = newCont->begin();
    CscCondDataContainer::const_iterator endItr = newCont->end();
    for(; newItr != endItr ; newItr++) {
      if(!(*newItr))
      {
        ATH_MSG_ERROR("Empty element in container with new data. Can't merge");
        return StatusCode::RECOVERABLE;
      }

      const CscCondDataCollectionBase * newColl = *newItr;

      std::string parName = newColl->getParName();
      std::map<std::string, CscCondDataCollectionBase*>::const_iterator refItr =  
        m_parNameMap.find(parName);
      if(refItr == m_parNameMap.end())
      {
        ATH_MSG_ERROR("Parameter " << parName << " not loaded. Can't merge. Check your JobOptions.");
      }
      const CscCondDataCollectionBase *refColl = refItr->second;


      std::string dataType = refColl->getParDataType();

      if(dataType != newColl->getParDataType())
      {
        ATH_MSG_ERROR("When merging parameter " << refColl->getParName()
          << " found that new data has type " << newColl->getParDataType() 
          << " and reference has type " << dataType << ". quiting merging...");
        return StatusCode::RECOVERABLE;
      }

      //now merge them 
      if(dataType == "uint32_t") {
        if(!mergeCollections<uint32_t>(newColl, refColl).isSuccess()){
          ATH_MSG_ERROR("Failed merging " << parName);
          return StatusCode::RECOVERABLE;
        }
      }
      if(dataType == "int") {
        if(!mergeCollections<int>(newColl, refColl).isSuccess()){
          ATH_MSG_ERROR("Failed merging " << parName);
          return StatusCode::RECOVERABLE;
        }
      }
      if(dataType == "float") {
        if(!mergeCollections<float>(newColl, refColl).isSuccess()){
          ATH_MSG_ERROR("Failed merging " << parName);
          return StatusCode::RECOVERABLE;
        }
      }
      if(dataType == "bool") {
        if(!mergeCollections<bool>(newColl, refColl).isSuccess()){
          ATH_MSG_ERROR("Failed merging " << parName);
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
    std::list<std::string>::const_iterator keyItr = keys.begin();
    std::list<std::string>::const_iterator keyEnd = keys.end();
    for(; keyItr != keyEnd; keyItr++) {
      if(!cacheParameter(*keyItr).isSuccess()) {
        ATH_MSG_WARNING("Failed at caching key " << (*keyItr));
      }
    } 
    return StatusCode::SUCCESS;
  }


  //-------------------------------------------------------------------
  //Extracts parameter from database, reads it into the database mirror;
  StatusCode CscCoolStrSvc::cacheParameter(const std::string & parKey)
  {
    if(msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Caching parameter " << parKey);
    ///*****//
    std::map<std::string, CscCondDataCollectionBase*>::iterator collItr = m_parSGKeyMap.find(parKey);

    if(collItr == m_parSGKeyMap.end())
    {
      ATH_MSG_ERROR("Failed caching " << parKey 
        << ". It doens't seem to be registered.");
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
      if(msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Attempting to retrieve cool channel " 
        << coolItr << " with key " << parKey);
      //retrieve datastring from db			
      std::string dataStr;
      if (StatusCode::SUCCESS != getCoolChannelString(atrc, coolItr, dataStr))
      {
        if(msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Couldn't find cool channel " << coolItr);
        continue;
      }

      numCoolChannelsFound = numCoolChannelsFound + 1;


      if(msgLvl(MSG::DEBUG)) ATH_MSG_VERBOSE("For cool channel " << coolItr << ", String is \n[" << dataStr << "]");
      //now decode string into numeric parameters.
      std::istringstream ss(dataStr);

      if(!ss.good()) 
      {
        ATH_MSG_WARNING("Failed forming stringstream during caching of " << coll->getParName());
        continue;
      }

      std::string version;
      ss >>  version;
      if(version == "02-00") {
        if(!cache(ss,coll).isSuccess()) {
          ATH_MSG_FATAL( "Failed caching from COOL string.");
          return StatusCode::FAILURE;
        }
      } else {
        ATH_MSG_FATAL( "Did not recognize CSC COOL string version " << version << ". Currently, only version 02-00 is supported. The keys of the database have to be geometrical CSC hashes in hex format.");
        return StatusCode::FAILURE;
      }

    }//end cool channel loop
    if(!numCoolChannelsFound) {
      ATH_MSG_ERROR("Found no COOL channels!");
      return StatusCode::RECOVERABLE;
    }
    if(numCoolChannelsFound < numCoolChannels) {
      ATH_MSG_WARNING("Only could retrieve " << numCoolChannelsFound << " out of " << numCoolChannels << " cool channels. This should never happen for a normal dataset, and could be a serious problem.");
    }

    return StatusCode::SUCCESS;
  }//end cache parameter

  //-------------------------------------------------------------------
  StatusCode CscCoolStrSvc::cache(std::istringstream& ss, CscCondDataCollectionBase* const coll) {

    if(msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Caching " << coll->getParName()
                      << " (category " << coll->getParCat() << "). Database string is version 2" 
                     );
    unsigned int numUpdated = 0;
    std::string indexStr,valueStr;

    //get past remainder of header info
    while(ss.good()) {
      ss >> valueStr;
      if(valueStr == "<BEGIN_DATA>")
        break;
    }

    const std::string & cat = coll->getParCat();
    const unsigned int & numEntries = coll->getSize();
    std::string str = "";

    if ( cat!="ASM" )
    {
      //Loop over strips in chamber. We don't usually expect cnt to reach numEntries
      //We just have this upper limit to prevent a bug causing an endless loop.
      for(unsigned int cnt = 0; cnt < numEntries; cnt++) {
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
            ATH_MSG_ERROR("Failed converting string Id to index");
            return StatusCode::RECOVERABLE;
          }
          ss >> str;
        }

        if(msgLvl(MSG::VERBOSE)) ATH_MSG_VERBOSE("[cache version 2 (CHANNEL) ] Recording " 
          << str << " at index " << index);

        if (index==UINT_MAX) continue;
        if (index>=numEntries) continue;

        //Now str has a value in it. We pass it to the collection.
        std::istringstream valueSS(str);
        if(!coll->recordFromSS(valueSS, index).isSuccess()) {
          ATH_MSG_WARNING("Failed caching to index " << index << " for parameter" 
            << coll->getParName()
            << ". Likely tried to recache to same index twice."
            << " Likely a bug when the data was orginally put in COOL.");
        }
        if(msgLvl(MSG::DEBUG)) numUpdated++;
        if (cnt==(numEntries-1)) {
          ATH_MSG_ERROR("Something wrong with <END_DATA>"
                << " in COOL tag in cacheVersion2");
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

      for (unsigned int cnt = 0; cnt < numEntries; cnt++ )  //  Only works for chamberLayer=2
      {	ss >> asmIDstr;  /*  asm cool tag id string which is
          ASM[#:1-5]_[StationEtaString:AorC][stationPhi:1-8]_[stationName:50-51]
          xxx   3   x                  5                6   x             x9      */
        if(msgLvl(MSG::VERBOSE)) ATH_MSG_VERBOSE("ASM ID String: " << asmIDstr);
        if  ( asmIDstr == "<END_DATA>" )  break;

        asmNum = atoi(asmIDstr.substr(3,1).c_str());

        if  ( asmIDstr[5] == 'A' )  stationEta =  1;
        else if( asmIDstr[5] == 'C')   stationEta = -1;   
        else{
          ATH_MSG_FATAL("Bad ASMID String in CSC COOL database \"" << asmIDstr << "\" (wheel " << asmIDstr[5] << " doesn't exist");
          return StatusCode::FAILURE;
        }
         
        stationPhi = atoi(asmIDstr.substr(6,1).c_str());

        stationName = atoi(asmIDstr.substr(8,2).c_str());
        
        if(stationPhi < 1 || stationPhi > 8 || stationName <50 || stationName > 51){
          ATH_MSG_FATAL("Bad ASMID String in CSC COOL database: \"" << asmIDstr << "\"");

          ATH_MSG_FATAL("Read station phi: " << stationPhi << ", stationName " << stationName);
          return StatusCode::FAILURE;
        }

        if ( !getAsmScope(asmNum, measuresPhi, layerSince, layerUntil, stripSince, stripUntil) )
        { ATH_MSG_FATAL("Failure of getAsmScope in cacheVersion2.");
          return StatusCode::FAILURE;
        }

        ss >> valueStr;  // get value to put in parameter vector

        //  Now for given asmID, loop over strip and layer
        for ( int iStrip = stripSince; iStrip < stripUntil; iStrip++ )
        { for ( int iLayer = layerSince; iLayer < layerUntil; iLayer++ )
          { 
          // The following call of channelID with check=true ensures that the identifier is checked to be physically valid.
          // This is currently required to be checked when running with layouts which do not contain all CSCs anymore, since the
          // CSCCool database contains still all CSCs. A clean fix would be to have a dedicated database for every layout.
          bool isValid = true;
          chanId = m_idHelperSvc->cscIdHelper().channelID(stationName, stationEta, stationPhi, chamberLayer, iLayer, measuresPhi, iStrip, true, &isValid);
          static bool conversionFailPrinted = false;
          if (!isValid) {
            if (!conversionFailPrinted) {
              ATH_MSG_WARNING("Failed to retrieve offline identifier from ASM cool string " << asmIDstr
                                    << ". This is likely due to the fact that the CSCCool database contains more entries than "
                                    << "the detector layout.");
              conversionFailPrinted = true;
            }
            continue;
          }
          if (m_idHelperSvc->cscIdHelper().get_channel_hash(chanId, hashIdentifier)) {
            ATH_MSG_WARNING("Failed to retrieve channel hash for identifier " << chanId.get_compact());
          }

          index = (int)hashIdentifier;
          if ( msgLvl(MSG::VERBOSE) ) ATH_MSG_VERBOSE("(ASM) Recording " 
            << valueStr << " at index " << index 
              << "\nstationName " << stationName
              <<"\nstationEta " << stationEta
              << "\nstationPhi " << stationPhi 
              << "\nchamberLayer " << chamberLayer
              << "\niLayer " << iLayer
              << "\nmeasuresPhi " << measuresPhi
              << "\niStrip " << iStrip 
             );

          if (index==UINT_MAX) continue;
          if (index>=numEntries) continue;

                    //Now valueStr has a value in it. We pass it to the collection.
          std::istringstream valueSS(valueStr);
          if ( !coll->recordFromSS(valueSS, index).isSuccess() )
          { ATH_MSG_WARNING("Failed caching to index " << index 
            << " for parameter " << coll->getParName()
              << " (data string version 2)."
              << " Likely tried to recache to same index twice."
              << " Likely a bug when the data was orginally put in COOL.");
          }
          if ( msgLvl(MSG::DEBUG) ) numUpdated++;
          if ( cnt==(numEntries-1) )
          { ATH_MSG_ERROR("Something wrong with <END_DATA> in"
            << " COOL tag in cacheVersion2");
            return StatusCode::RECOVERABLE;
          } } } } }  //  cat=="ASM"

          if ( msgLvl(MSG::DEBUG) ) ATH_MSG_DEBUG("Number updated is " << numUpdated);
          return StatusCode::SUCCESS;
  }  //  end cacheVersion2


  //-------------------------------------------------------------------
  int CscCoolStrSvc::swapChamberLayerReturnHash(const Identifier & id) const
  {
    int stationName = m_idHelperSvc->cscIdHelper().stationName(id);
    int stationEta = m_idHelperSvc->cscIdHelper().stationEta(id); 
    int stationPhi = m_idHelperSvc->cscIdHelper().stationPhi(id); 
    int chamberLayer = m_idHelperSvc->cscIdHelper().chamberLayer(id) == 1 ? 2 : 1; //Swap chamber layer
    int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(id); 
    int wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(id);
    int strip = m_idHelperSvc->cscIdHelper().strip(id);
    Identifier newId = m_idHelperSvc->cscIdHelper().channelID(stationName, stationEta, stationPhi, 
        chamberLayer, wireLayer, measuresPhi, strip);
    IdentifierHash hash;
    m_idHelperSvc->cscIdHelper().get_channel_hash(newId, hash);
    ATH_MSG_DEBUG("swap chamber layer " << m_idHelperSvc->cscIdHelper().show_to_string(id) 
          << " to " << m_idHelperSvc->cscIdHelper().show_to_string(newId) << " (" << hash << ")");
    return (int)hash;
  }

  //-------------------------------------------------------------------
  StatusCode CscCoolStrSvc::offlineToOnlineId(const Identifier & id, unsigned int &onlineId) const
  {

    onlineId = 0;
    //Phi,wireLayer,and strip all are offset by one between the two schemes.
    //Also, station name is 50 or 51 in Identifiers, but only 0 or 1 in 
    //the online id.
    int stationName  	((m_idHelperSvc->cscIdHelper().stationName(id) -50)&0x1 );		// 0001 0000 0000 0000 0000
    int phi =   		(m_idHelperSvc->cscIdHelper().stationPhi(id) - 1)&0x7  ;		    // 0000 1110 0000 0000 0000
    int eta = 		((m_idHelperSvc->cscIdHelper().stationEta(id) == 1) ? 1:0) &0x1;  // 0000 0001 0000 0000 0000
    int chamLay = 		(m_idHelperSvc->cscIdHelper().chamberLayer(id)-1) &0x1;		    // 0000 0000 1000 0000 0000
    int wireLay = 		(m_idHelperSvc->cscIdHelper().wireLayer(id)-1) &0x3;		      // 0000 0000 0110 0000 0000
    int measuresPhi = 	(m_idHelperSvc->cscIdHelper().measuresPhi(id) &0x1);		    // 0000 0000 0001 0000 0000
    int strip;     		                                          // 0000 0000 0000 1111 1111

    //Online and offline phi ids are flipped on A wheel
    if(m_onlineOfflinePhiFlip && measuresPhi && eta == 1){
      strip = (48 - (m_idHelperSvc->cscIdHelper().strip(id))) & 0xff;  
    }
    else {
      strip = (m_idHelperSvc->cscIdHelper().strip(id)-1) & 0xff;     		     
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
    int stationEta  = m_idHelperSvc->cscIdHelper().stationEta(id);
    std::string stationEtaString  = (stationEta == 1 ? "A":"C");
    int stationPhi  = m_idHelperSvc->cscIdHelper().stationPhi(id);
    int stationName = m_idHelperSvc->cscIdHelper().stationName(id);
    int wireLayer   = m_idHelperSvc->cscIdHelper().wireLayer(id);
    int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(id);
    int strip       = m_idHelperSvc->cscIdHelper().strip(id);

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
    int stationName  	((m_idHelperSvc->cscIdHelper().stationName(id) -50)&0x1 );		// 0001 0000 0000 0000 0000
    int phi =   		(m_idHelperSvc->cscIdHelper().stationPhi(id) - 1)&0x7  ;		// 0000 1110 0000 0000 0000
    int eta = 		((m_idHelperSvc->cscIdHelper().stationEta(id) == 1) ? 1:0) &0x1;  	// 0000 0001 0000 0000 0000
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

    elementId = m_idHelperSvc->cscIdHelper().elementID(stationName,eta,phi);
    // The following call of channelID with check=true ensures that the identifier is checked to be physically valid.
    // This is currently required to be checked when running with layouts which do not contain all CSCs anymore, since the
    // CSCCool database contains still all CSCs. A clean fix would be to have a dedicated database for every layout.
    bool isValid = true;
    channelId = m_idHelperSvc->cscIdHelper().channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip,true,&isValid);
    static bool conversionFailPrinted = false;
    if (!isValid) {
      if (!conversionFailPrinted) {
        ATH_MSG_WARNING("Failed to retrieve offline identifier from online identifier " << onlineId
                              << ". This is likely due to the fact that the CSCCool database contains more entries than "
                              << "the detector layout.");
        conversionFailPrinted = true;
      }
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::onlineToOfflineElementId(const unsigned int & onlineId, Identifier &elementId) const
  {
    int stationName =       ((onlineId >> 16)&0x1) + 50;
    int phi =               ((onlineId >> 13)&0x7)+1;
    int eta =               ((((onlineId >> 12)&0x1) == 1) ? 1:-1);

    elementId = m_idHelperSvc->cscIdHelper().elementID(stationName,eta,phi);

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

    chanId = m_idHelperSvc->cscIdHelper().channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip);

    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  unsigned int CscCoolStrSvc::getLayerHash( const Identifier & id) const 
  {
    unsigned int stationName = m_idHelperSvc->cscIdHelper().stationName(id);
    if(m_idHelperSvc->cscIdHelper().stationName(id) >= 50 ) stationName = stationName - 50;
    else {
      ATH_MSG_ERROR("stationName: " << stationName << " is not CSC - emergency stop.");
      throw std::runtime_error(Form("File: %s, Line: %d\nCscCoolStrSvc::getLayerHash() - given identifier is no CSC identifier", __FILE__, __LINE__));
    }
    unsigned int stationEta = (m_idHelperSvc->cscIdHelper().stationEta(id) == 1 ? 1 :0);
    unsigned int stationPhi = m_idHelperSvc->cscIdHelper().stationPhi(id) -1; 
    unsigned int wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(id) -1;
    unsigned int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(id);

    return m_layerHashes[stationName][stationEta][stationPhi][wireLayer][measuresPhi];
  }


  //-----------------------------------------------------------------------------------
  unsigned int CscCoolStrSvc::getChamberCoolChannel( const Identifier & id) const
  {

    unsigned int stationName = m_idHelperSvc->cscIdHelper().stationName(id) -50;
    unsigned int eta = (m_idHelperSvc->cscIdHelper().stationEta(id) == 1 ? 1 : 0);
    unsigned int phi = m_idHelperSvc->cscIdHelper().stationPhi(id) -1; 

    if(stationName > 1
        || phi > 7
        || eta > 1
      ) {
      ATH_MSG_ERROR("when creating chamber cool channel, inputs were:\nstationName: " 
        << stationName 
        << "\nPhi: " << phi
        << "\neta: " << eta);
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
      ATH_MSG_ERROR("Tried to lookup online id from layer hash " 
        << layerHash <<". Max is " << m_onlineChannelIdsFromLayerHash.size());
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

    chamCoolChan = m_chamberCoolChannels[stationName][eta][phi];
    if(chamCoolChan < 1 || chamCoolChan > 32) {
      ATH_MSG_ERROR("created chamber cool channel is " 
        << chamCoolChan);
      return StatusCode::RECOVERABLE;
    }
    return StatusCode::SUCCESS;
  }

  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::coolChamberChannelToOnlineId(const unsigned int & chamCoolChan, unsigned int & onlineId ) const {

    if(chamCoolChan > m_onlineChannelIdsFromChamberCoolChannel.size()
        || chamCoolChan ==0 ) {
      ATH_MSG_ERROR("Requested online ID for chamber Cool Channel " << chamCoolChan 
        <<"which can't be more than " << m_onlineChannelIdsFromChamberCoolChannel.size());
    }
    onlineId = m_onlineChannelIdsFromChamberCoolChannel[chamCoolChan-1];
    return StatusCode::SUCCESS;
  }

  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::getParCat(const std::string & parName, std::string & val) const{
    std::map<std::string, CscCondDataCollectionBase*>::const_iterator itr = m_parNameMap.find(parName);
    if(itr == m_parNameMap.end())
      return StatusCode::RECOVERABLE;

    val = itr->second->getParCat();
    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::getParDataType(const std::string & parName, std::string & val) const {
    std::map<std::string, CscCondDataCollectionBase*>::const_iterator itr = m_parNameMap.find(parName);
    if(itr == m_parNameMap.end())
      return StatusCode::RECOVERABLE;

    val = itr->second->getParDataType();
    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::getParFolder(const std::string & parName, std::string & val) const { 
    std::map<std::string, CscCondDataCollectionBase*>::const_iterator itr = m_parNameMap.find(parName);
    if(itr == m_parNameMap.end())
      return StatusCode::RECOVERABLE;

    val = itr->second->getParFolder();
    return StatusCode::SUCCESS;
  }


  //-----------------------------------------------------------------------------------
  StatusCode CscCoolStrSvc::getParNumHashes(const std::string & parName, unsigned int & val) const { 
    std::map<std::string, CscCondDataCollectionBase*>::const_iterator itr = m_parNameMap.find(parName);
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
        ATH_MSG_ERROR("Unknown idString of " << idString 
          << " asked for ENDCAP cateogry.");
        return StatusCode::RECOVERABLE;
      }

      return StatusCode::SUCCESS;
    }   

    std::istringstream ss(idString);
    unsigned int chanAddress;
    ss >> std::hex >> chanAddress;

    //remaining categories need offline identifiers
    Identifier chamberId;
    Identifier channelId;
    if(!onlineToOfflineIds(chanAddress, chamberId, channelId).isSuccess()){
      index = UINT_MAX;
      return StatusCode::SUCCESS;
    }

    if(cat == "CHAMBER")
    {
      IdentifierHash chamberHash;
      m_idHelperSvc->cscIdHelper().get_geo_module_hash(chamberId,chamberHash);
      index = (unsigned int)chamberHash; 
    } 
    else if(cat == "LAYER")
    {
      index = getLayerHash(channelId);
    }
    else if(cat == "CHANNEL")
    {
      IdentifierHash chanHash;
      m_idHelperSvc->cscIdHelper().get_channel_hash(channelId, chanHash);
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
        ATH_MSG_ERROR("Requested index " << index 
          << " can't be converted to a string Id for the category " << cat);
        return StatusCode::RECOVERABLE;
      }
    }

    //remaining categories need online identifiers
    unsigned int onlineId = 0;
    std::stringstream ss;
    if(cat == "CHAMBER")
    {

      Identifier chamberId;
      m_idHelperSvc->cscIdHelper().get_id(IdentifierHash(index), chamberId, &m_moduleContext);
      if(!offlineElementToOnlineId(chamberId, onlineId).isSuccess()) {
        ATH_MSG_ERROR("Failed converting chamber identifier to online id during stringId gen. ");
        return StatusCode::RECOVERABLE;
      }
    } 
    else if(cat == "LAYER")
    {
      unsigned int onlineId;
      if(!layerHashToOnlineId(index, onlineId)){
        ATH_MSG_ERROR("Failed at getting online id from layer hash during stringId gen");
      }
    }
    else if(cat == "CHANNEL")
    {
      Identifier channelId;
      m_idHelperSvc->cscIdHelper().get_id(IdentifierHash(index), channelId, &m_channelContext);
      if(!offlineToOnlineId(channelId, onlineId).isSuccess()) {
        ATH_MSG_ERROR("Failed converting chamber identifier to online id during stringId gen. ");
        return StatusCode::RECOVERABLE;
      }
    }

    ss << std::hex << std::setfill('0') << std::setw(5) <<  onlineId << std::dec;
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
      ATH_MSG_FATAL("ASM  number  \"" << asmNum << "\" is invalid. It needs to end in a number from 1-5.");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

} //end MuonCalib namespace


