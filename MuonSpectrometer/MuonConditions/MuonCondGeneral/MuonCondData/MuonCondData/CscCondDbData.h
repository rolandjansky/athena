/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_CSCCONDDBDATA_H
#define MUONCONDDATA_CSCCONDDBDATA_H

//STL includes
#include <string>
#include <vector>

//Athena includes
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/CondCont.h" 
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "AthenaKernel/BaseInfo.h" 


//forward declarations
class Identifier;
class IdentifierHash;



class CscCondDbData {

  friend class CscCondDbAlg;

public:

    CscCondDbData();
    virtual ~CscCondDbData() = default;

    void setChannelStatus(IdentifierHash, int);
    void setDeadChannelHash(IdentifierHash);

    void setDeadLayer     (std::string, Identifier);
    void setDeadStation   (std::string, Identifier);
   
    const std::vector<std::string>& getDeadLayers     () const;
    const std::vector<std::string>& getDeadStations   () const;
    
    const std::vector<Identifier>& getDeadLayersId     () const;
    const std::vector<Identifier>& getDeadStationsId   () const;

    const std::vector<unsigned int>& getDeadChannelsHash() const;
    
    const int& getChannelStatus(IdentifierHash) const;

    bool isGood           (const Identifier &    ) const;
    bool isGoodLayer      (const Identifier &    ) const;
    bool isGoodStation    (const Identifier &    ) const;

    bool isGoodChannelHash(const IdentifierHash &) const;


 
private:

    std::vector<std::string> m_cachedDeadLayers;
    std::vector<std::string> m_cachedDeadStations;

    std::vector<Identifier> m_cachedDeadLayersId;
    std::vector<Identifier> m_cachedDeadStationsId;

    std::vector<unsigned int> m_cachedDeadChannelsHash;
    std::map<unsigned int, int> m_cachedChannelsStatus;

    std::vector<std::string> m_emptyNames;
    std::vector<Identifier> m_emptyIds; 
    std::vector<unsigned int> m_emptyHashs;

    int m_nillint = 0;

};

CLASS_DEF( CscCondDbData, 10742890, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<CscCondDbData>, 43297256, 0)

#endif
