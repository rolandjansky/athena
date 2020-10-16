/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_CSCICOOLSTRSVC_H
#define MUONCONDSVC_CSCICOOLSTRSVC_H
// ICoolStrFileSvc.h - interact with COOL conditions database for simple
// string data

#include "GaudiKernel/Service.h"

#include <string>
#include <vector>
#include "GaudiKernel/IInterface.h"

//#include "MuonCondData/CscCalibData.h"

class Identifier;


namespace MuonCalib {
  class CscCondDataContainer;
  class CscCondDataCollectionBase;

  class CscICoolStrSvc : virtual public IInterface {

    public:
      static const InterfaceID& interfaceID();
    
      /**mergeAndSubmitCondDataContainer merges the data in newCont with what is currently cached from COOL.
      Then, it submits the new merged data for entry to cool (as of this writing the final write is done
      with OutputCOnditionsAlg)
     */
    virtual StatusCode mergeAndSubmitCondDataContainer(const CscCondDataContainer * newCont) = 0;

    /**checkParameter returns 1 if parameter exists at that index, 0 if not
      */
    
    virtual int checkIndex(const std::string & parName, unsigned int& index) const =0;

    /**getParameter retrieves a particular parameter from the cached database. If an invalid parname is 
      given, or if the requested strip is not in the database, it will return a default value for the
      retData.

      They are essentially wrappers of the getParameterTemplated function.

      @param retData data element returned
      @param parName string  defining what parameter it is you want
      @param index index indicating what part of detector you want. Usually a hash Id, such as a channel or chamber hash.
     */
    virtual StatusCode getParameter(uint8_t &retData, const std::string &parName, const unsigned int & index) const =0;
    virtual StatusCode getParameter(uint16_t &retData, const std::string &parName, const unsigned int & index) const =0;
    virtual StatusCode getParameter(uint32_t &retData, const std::string &parName, const unsigned int & index) const =0;
    virtual StatusCode getParameter(int &retData, const std::string &parName, const unsigned int & index) const =0;
    virtual StatusCode getParameter(float &retData, const std::string &parName, const unsigned int & index) const =0;
    virtual StatusCode getParameter(bool &retData, const std::string &parName, const unsigned int & index) const =0;
    
    /* direct access to avoid string look-ups */
    virtual bool getRMS( float& val, const unsigned int & index) const =0;
    virtual bool getSlope( float& val, const unsigned int & index) const =0;
    virtual bool getNoise( float& val, const unsigned int & index) const =0;
    virtual bool getF001( float& val, const unsigned int & index) const =0;
    virtual bool getPedestal( float& val, const unsigned int & index) const =0;
    virtual bool getT0Phase( bool& val, const unsigned int & index) const =0;
    virtual bool getT0Base( float& val, const unsigned int & index) const =0;
    virtual bool getStatus(uint32_t &status, const unsigned int & stripID) const =0;
    
    /**getStatus returns the status from the local cache of the database
      Although this can be retrieved by getParameter() now, this is retained for 
      backwards compatibility.
      @param status status flag is stored here.
      @param stripID hashID for strip you want status from
      @param db database to retrieve status from, "pulser" or "run"
     */
    virtual StatusCode getStatus(uint8_t &status, const unsigned int & stripID) const=0;


    /**Layer hash used in index for getParameter retrieval of layer information*/
    virtual unsigned int getLayerHash( const Identifier & id) const=0;
    virtual StatusCode onlineIdToLayerHash(const unsigned int & onlineId, int & layerHash ) const=0;
    virtual StatusCode layerHashToOnlineId(const unsigned int & layerHash, unsigned int & onlineId) const=0;

    /**Gets the cool channel for a given chamber*/
    virtual unsigned int getChamberCoolChannel( const Identifier & id) const=0;
    virtual StatusCode onlineIdToCoolChamberChannel(const unsigned int & onlineId, unsigned int & chamCoolChan ) const=0;
    virtual StatusCode coolChamberChannelToOnlineId(const unsigned int & chamCoolChan, unsigned int &  ) const=0;

    /**Get information about a given parameter by name*/
    virtual StatusCode getParFolder(const std::string & name, std::string & folder) const =0;
    virtual StatusCode getParCat(const std::string & name, std::string & cat) const =0;
    virtual StatusCode getParDataType(const std::string & name, std::string & dataType) const =0;
    virtual StatusCode getParNumHashes(const std::string & name, unsigned int& maxIndex) const =0;

    /**Get index from the string identifier used in the cool database (as well as in 
      input files used by the CscCoolReadWriteStr algorithm in MuonCondCool Package)*/
    virtual StatusCode stringIdToIndex(const std::string & idString, const std::string & cat, unsigned int & index) const=0;
    virtual StatusCode indexToStringId(const unsigned int & index, const std::string & cat, std::string & stringId) const=0;

    /**Extract information about category and put into collection*/  
    virtual StatusCode  initCscCondDataCollection(CscCondDataCollectionBase * coll)const=0;

    /**Conversions between online and offline identifiers*/
    virtual StatusCode offlineToOnlineId(const Identifier & id, unsigned int &onlineId) const=0;
    virtual StatusCode offlineElementToOnlineId(const Identifier & id, unsigned int &onlineId) const =0;
    virtual StatusCode onlineToOfflineIds(const unsigned int & onlineId, Identifier &elementId, Identifier &channelId) const=0;
    virtual StatusCode onlineToOfflineElementId(const unsigned int & onlineId, Identifier &elementId) const=0;
    virtual StatusCode onlineToOfflineChannelId(const unsigned int & onlineId, Identifier &channelId) const=0;


    /**Get details from ASM Number
      since is inclusive
      until is exclusive
      */
    virtual StatusCode getAsmScope(int asmNum, int &measuresPhi,  int & layerSince, int & layerUntil, int & stripSince , int & stripUntil)=0;

    virtual int Access(std::vector<std::string>* strVec=0,std::vector<int>* intVec=0, std::vector<float>* floatVector=0) =0;
  };


  inline const InterfaceID& CscICoolStrSvc::interfaceID() {
    static const InterfaceID IID_CscICoolStrSvc("CscICoolStrSvc",1,1);
    return IID_CscICoolStrSvc;
  }
}
#endif // MUONCONDSVC_CSCICOOLSTRSVC_H
