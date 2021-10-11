/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_CSCCONDDBDATA_H
#define MUONCONDDATA_CSCCONDDBDATA_H

//STL includes
#include <string>
#include <vector>

//Athena includes
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/BaseInfo.h"
#include "Identifier/IdContext.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

//forward declarations
class CscIdHelper;


class CscCondDbData {

  friend class CscCondDbAlg;

public:

    CscCondDbData();
    virtual ~CscCondDbData() = default;

    void loadParameters(const CscIdHelper*);
    void setParameters(bool); // keep in plural for future development

    void setChannelF001   (IdentifierHash, float);
    void setChannelNoise  (IdentifierHash, float);
    void setChannelPed    (IdentifierHash, float);
    void setChannelPSlope (IdentifierHash, float);
    void setChannelRMS    (IdentifierHash, float);
    void setChannelStatus (IdentifierHash, int  );
    void setChannelT0Base (IdentifierHash, float);
    void setChannelT0Phase(IdentifierHash, bool );

    void setDeadChannelHash(IdentifierHash);
    void setDeadLayer     (std::string_view, Identifier);
    void setDeadStation   (std::string_view, Identifier);
   
    const std::vector<std::string>& getDeadLayers     () const;
    const std::vector<std::string>& getDeadStations   () const;
    
    const std::vector<Identifier>& getDeadLayersId     () const;
    const std::vector<Identifier>& getDeadStationsId   () const;

    const std::vector<unsigned int>& getDeadChannelsHash() const;

    bool  hasChannelT0Phase(IdentifierHash) const;
    
    const float& getChannelF001   (IdentifierHash) const;
    const float& getChannelNoise  (IdentifierHash) const;
    const float& getChannelPed    (IdentifierHash) const;
    const float& getChannelPSlope (IdentifierHash) const;
    const float& getChannelRMS    (IdentifierHash) const;
    const int&   getChannelStatus (IdentifierHash) const;
    const float& getChannelT0Base (IdentifierHash) const;
    const bool&  getChannelT0Phase(IdentifierHash) const;

    StatusCode readChannelF001   (IdentifierHash, float&) const;
    StatusCode readChannelNoise  (IdentifierHash, float&) const;
    StatusCode readChannelPed    (IdentifierHash, float&) const;
    StatusCode readChannelPSlope (IdentifierHash, float&) const;
    StatusCode readChannelRMS    (IdentifierHash, float&) const;
    StatusCode readChannelStatus (IdentifierHash, int&  ) const;
    StatusCode readChannelT0Base (IdentifierHash, float&) const;
    StatusCode readChannelT0Phase(IdentifierHash, bool& ) const;

    bool isGood           (const Identifier &    ) const;
    bool isGoodLayer      (const Identifier &    ) const;
    bool isGoodStation    (const Identifier &    ) const;

    bool isGoodChannelHash(const IdentifierHash &) const;

    StatusCode indexToStringId(const CscIdHelper*, const unsigned int &, const std::string &, std::string &) const;
    StatusCode layerHashToOnlineId(const unsigned int &, unsigned int &) const;
    StatusCode offlineElementToOnlineId(const CscIdHelper*, const Identifier &, unsigned int &) const;
    StatusCode offlineToOnlineId(const CscIdHelper*, const Identifier &, unsigned int &) const;
    StatusCode onlineToOfflineIds(const CscIdHelper*, const unsigned int &, Identifier &, Identifier &) const;
 
private:

    bool m_onlineOfflinePhiFlip;

    IdContext m_channelContext;
    IdContext m_moduleContext;

    std::vector<unsigned int> m_onlineChannelIdsFromLayerHash;
    std::vector<unsigned int> m_onlineChannelIdsFromChamberCoolChannel;

    std::vector<std::string> m_cachedDeadLayers;
    std::vector<std::string> m_cachedDeadStations;

    std::vector<Identifier> m_cachedDeadLayersId;
    std::vector<Identifier> m_cachedDeadStationsId;

    std::vector<unsigned int> m_cachedDeadChannelsHash;

    std::map<unsigned int, float> m_cachedChannelsF001;
    std::map<unsigned int, float> m_cachedChannelsNoise;
    std::map<unsigned int, float> m_cachedChannelsPed;
    std::map<unsigned int, float> m_cachedChannelsPSlope;
    std::map<unsigned int, float> m_cachedChannelsRMS;
    std::map<unsigned int, int  > m_cachedChannelsStatus;
    std::map<unsigned int, float> m_cachedChannelsT0Base;
    std::map<unsigned int, bool > m_cachedChannelsT0Phase;

    std::vector<std::string> m_emptyNames;
    std::vector<Identifier> m_emptyIds; 
    std::vector<unsigned int> m_emptyHashs;

    const bool  m_nillbool  = false;
    const float m_nillfloat = -1;
    const int   m_nillint = -1;

public:

    // readChannelParam
    template <typename T> StatusCode readChannelParam(IdentifierHash hash, T& val, const std::string& parName) const{

        if     (parName == "f001"   ) { float theVal; return readChannelF001   (hash, theVal); val = theVal; }
        else if(parName == "noise"  ) { float theVal; return readChannelNoise  (hash, theVal); val = theVal; }
        else if(parName == "ped"    ) { float theVal; return readChannelPed    (hash, theVal); val = theVal; }
        else if(parName == "pslope" ) { float theVal; return readChannelPSlope (hash, theVal); val = theVal; }
        else if(parName == "rms"    ) { float theVal; return readChannelRMS    (hash, theVal); val = theVal; }
        else if(parName == "status" ) { int   theVal; return readChannelStatus (hash, theVal); val = theVal; }
        else if(parName == "t0base" ) { float theVal; return readChannelT0Base (hash, theVal); val = theVal; }
        else if(parName == "t0phase") { bool  theVal; return readChannelT0Phase(hash, theVal); val = theVal; }
        return StatusCode::FAILURE;
    }

};

CLASS_DEF( CscCondDbData, 10742890, 1)
CLASS_DEF( CondCont<CscCondDbData>, 43297256, 0)

#endif
