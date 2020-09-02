/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_CSCCOOLSTRSVC_H
#define MUONCONDSVC_CSCCOOLSTRSVC_H

#include "MuonCondInterface/CscICoolStrSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCondData/CscCondDataContainer.h"
#include "MuonCondData/CscCondDataCollection.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

template <class TYPE> class SvcFactory;

namespace MuonCalib {

  /**
    @class CscCoolStrSvc

    @author lampen@physics.arizona.edu

    @brief COOL read/write routines

    Contains the major code to write and read calibration files to the database. 
   */
  class CscCoolStrSvc : public AthService, public virtual CscICoolStrSvc
  {
    friend class SvcFactory<CscCoolStrSvc>;

    public:
    CscCoolStrSvc(const std::string& name, ISvcLocator* svc);
    virtual ~CscCoolStrSvc();

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    virtual const InterfaceID& type() const;

    virtual StatusCode initialize();

    /**mergeAndSubmitCondDataContainer merges the data in newCont with what is currently cached from COOL.
      Then, it submits the new merged data for entry to cool (as of this writing the final write is done
      with OutputCOnditionsAlg)
     */
    virtual StatusCode mergeAndSubmitCondDataContainer(const CscCondDataContainer *  newCont);

    template<typename data_type>
      StatusCode mergeCollections(const CscCondDataCollectionBase * & newColl, const CscCondDataCollectionBase * & refColl);


    /**getParameter retrieves a particular parameter from the cached database. If an invalid parname is 
      given, or if the requested strip is not in the database, it will return a default value for the
      retData.

      They are essentially wrappers of the getParameterTemplated function.

      @param retData data element returned
      @param parName string  defining what parameter it is you want
      @param index index indicating what part of detector you want. Usually a hash Id, such as a channel or chamber hash.
     */
    virtual StatusCode getParameter(uint8_t &retData, const std::string &parName, const unsigned int & index) const ;
    virtual StatusCode getParameter(uint16_t &retData, const std::string &parName, const unsigned int & index) const ;
    virtual StatusCode getParameter(uint32_t &retData, const std::string &parName, const unsigned int & index) const ;
    virtual StatusCode getParameter(int &retData, const std::string &parName, const unsigned int & index) const ;
    virtual StatusCode getParameter(float &retData, const std::string &parName, const unsigned int & index) const ;
    virtual StatusCode getParameter(bool &retData, const std::string &parName, const unsigned int & index) const ;

    virtual bool getRMS( float& val, const unsigned int & index) const;
    virtual bool getSlope( float& val, const unsigned int & index) const;
    virtual bool getNoise( float& val, const unsigned int & index) const;
    virtual bool getF001( float& val, const unsigned int & index) const;
    virtual bool getPedestal( float& val, const unsigned int & index) const;
    virtual bool getT0Phase( bool& val, const unsigned int & index) const;
    virtual bool getT0Base( float& val, const unsigned int & index) const;
    virtual bool getStatus(uint32_t &status, const unsigned int & stripID) const;

    /**getStatus returns the status from the local cache of the database
      Although this can be retrieved by getParameter() now, this is retained for 
      backwards compatibility.
      @param status status flag is stored here.
      @param stripID hashID for strip you want status from
      @param db database to retrieve status from, "pulser" or "run"
     */
    virtual StatusCode getStatus(uint8_t &status, const unsigned int & stripID) const;


    /**Layer hash used in index for getParameter retrieval of layer information*/
    virtual unsigned int getLayerHash( const Identifier & id) const;
    virtual StatusCode onlineIdToLayerHash(const unsigned int & onlineId, int & layerHash ) const;
    virtual StatusCode layerHashToOnlineId(const unsigned int & layerHash, unsigned int & onlineId) const;

    /**Gets the cool channel for a given chamber*/
    virtual unsigned int getChamberCoolChannel( const Identifier & id) const;
    virtual StatusCode onlineIdToCoolChamberChannel(const unsigned int & onlineId, unsigned int & chamCoolChan ) const;
    virtual StatusCode coolChamberChannelToOnlineId(const unsigned int & chamCoolChan, unsigned int &  ) const;

    /**Get information about a given parameter by name*/
    virtual StatusCode getParFolder(const std::string & name, std::string & val) const ;
    virtual StatusCode getParCat(const std::string & name, std::string & val) const ;
    virtual StatusCode getParDataType(const std::string & name, std::string & val) const ;
    virtual StatusCode getParNumHashes(const std::string & name, unsigned int& maxIndex) const ;

    /**Get index from the string identifier used in the cool database (as well as in 
      input files used by the CscCoolReadWriteStr algorithm in MuonCondCool Package)*/
    virtual StatusCode stringIdToIndex(const std::string & idString, const std::string & cat, unsigned int & index) const;
    virtual StatusCode indexToStringId(const unsigned int & index, const std::string & cat, std::string & stringId) const;

    /**Extract information about category and put into collection*/  
    virtual StatusCode  initCscCondDataCollection(CscCondDataCollectionBase * coll)const;
    
    /** getChamberData retrieves a single cool channel's data*/
    virtual StatusCode getCoolChannelString(
        const CondAttrListCollection * atrc, int coolChannel, std::string& data) 
      const ;

    /**Conversions between online and offline identifiers (and offline to ASM)*/
    virtual StatusCode offlineToOnlineId(const Identifier & id, unsigned int &onlineId) const;
    virtual StatusCode offlineToAsmId(const Identifier & id, std::string & AsmId, unsigned int & iChamber, unsigned int & iASM) const;
    virtual StatusCode offlineElementToOnlineId(const Identifier & id, unsigned int &onlineId) const;
    virtual StatusCode onlineToOfflineIds(const unsigned int & onlineId, Identifier &elementId, Identifier &channelId) const;
    virtual StatusCode onlineToOfflineElementId(const unsigned int & onlineId, Identifier &elementId) const;
    virtual StatusCode onlineToOfflineChannelId(const unsigned int & onlineId, Identifier &channelId) const;
      

    /**Get details from ASM Number
      since is inclusive
      until is exclusive
      */
    StatusCode getAsmScope(int asmNum, int &measuresPhi,  int & layerSince, int & layerUntil, int & stripSince , int & stripUntil);

    //For access for patches
    virtual int Access(std::vector<std::string>* /* strVec*/,std::vector<int>* /*intVec*/, std::vector<float>* /*floatVector*/);

    private:

    /**callback functions are called whenever their folder in the database goes out of date*/
    virtual StatusCode callback( IOVSVC_CALLBACK_ARGS );

    //returns 1 if parameter exists at that index, 0 if not
    virtual int checkIndex(const std::string & parName, unsigned int& index) const;

    /**getParameterTemplated is a templated function where getting the parameter is actually done*/
    template <class data_type> StatusCode getParameterTemplated(data_type &retData, 
        const std::string& parName, const unsigned int & index) const;

    /**cacheParameter caches a parameter from the COOL database into the local mirror*/
    StatusCode cacheParameter(const std::string & parKey);

    StatusCode cache(std::istringstream &, CscCondDataCollectionBase * const);

    int swapChamberLayerReturnHash(const Identifier & id) const;

    bool getVal( float& val, const CscCondDataCollection<float>& coll, unsigned int index) const;
    bool getVal( bool& val, const CscCondDataCollection<bool>& coll, unsigned int index) const;
    bool getVal(uint32_t& val, const CscCondDataCollection<uint32_t>& coll, unsigned int index) const;

    private:

    /**p_detstore hold a pointer to the transient data storage*/
    StoreGateSvc* p_detstore;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    //maximum channel and chamber hashes (num of each -1). These are set
    //with a const_cast from the id helper
    unsigned int m_maxChamberHash; 
    const unsigned int m_maxChanHash; 

    /**dbCache is locally cached version of cool database*/
    CscCondDataContainer * m_dbCache;

    /**Map is an easier way to reference parameters by name than the dbCache*/
    std::map<std::string, CscCondDataCollectionBase*> m_parNameMap;
    std::map<std::string, CscCondDataCollectionBase*> m_parSGKeyMap;

    const CscCondDataCollection<float>* m_rmsCondData;
    const CscCondDataCollection<float>* m_slopeCondData;
    const CscCondDataCollection<float>* m_noiseCondData;
    const CscCondDataCollection<float>* m_f001CondData;
    const CscCondDataCollection<float>* m_pedestalCondData;
    const CscCondDataCollection<bool>*  m_t0PhaseCondData;
    const CscCondDataCollection<float>* m_t0BaseCondData;
    const CscCondDataCollection<uint32_t>* m_statusCondData;

    /**Flags*/
    bool m_preCache;

    mutable int m_numFailedRequests;
    int m_maxFailedRequests;

    std::vector<std::string> m_parNameVec, m_parSGKeyVec, m_parFolderVec,  m_parDataTypeVec, m_parCatVec,
      m_parDefaultVec;

    //eta, sector, measuresPhi, layer
    unsigned int m_layerHashes[2][2][8][4][2];
    std::vector<unsigned int> m_onlineChannelIdsFromLayerHash;
    unsigned int m_chamberCoolChannels[2][2][8];
    const unsigned int m_maxChamberCoolChannel;
    std::vector<unsigned int> m_onlineChannelIdsFromChamberCoolChannel;
    const unsigned int m_maxLayerHash;

    std::string m_defaultDatabaseReadVersion;
    std::string m_defaultChanStatusName;

    IdContext m_channelContext, m_moduleContext;

    //For patches:
    std::vector<float> * m_accfloats;
    std::vector<int> * m_accInts;
    std::vector<uint32_t> *m_accUints;
    std::vector<bool> *m_accBools;
    std::vector<void*> * m_accVoid;

    bool m_doMerge;
    bool m_doCaching;
    bool m_onlineOfflinePhiFlip;

    /**
     * The pslope is the gain of each CSC channel. It was intended to be retrieved by calibration with the pulser runs, 
     * but the pulses caused overload in the amplifiers because every channel fires at once. This leads to errors that are 
     * larger than the variation between channels. Consequently, the pslope is the same for all channels. 
     * In the future, one could try to calibrate from data. The support for the pslope in the database is maintained by having
     * a boolean property ReadPSlopeFromDatabase. If it is set to false, the value of the property PSlope is used for all channels.
     */
    bool m_pslopeFromDB;
    float m_pslope;
    /**
     * The CSC gain was originally 5.304 ADC counts per fC, but later increased to
     * 5.7 ADC counts per fC, so the pslope equals 1/5.7 = 0.175438
     */
    const float m_DEFAULT_PSLOPE = 0.175;  };
}

#include "CscCoolStrSvc.icc"
#endif // MUONCONDSVC_CSCCOOLSTRSVC_H
