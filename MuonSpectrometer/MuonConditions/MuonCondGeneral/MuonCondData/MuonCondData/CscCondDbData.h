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
//#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaKernel/BaseInfo.h" 


//forward declarations
class Identifier;
class IdentifierHash;



class CscCondDbData {

  friend class CscCondDbAlg;

public:

    CscCondDbData();
    virtual ~CscCondDbData() = default;

    virtual void setChannelStatus(IdentifierHash, int);
    virtual void setDeadChannelHash(IdentifierHash);

    virtual void setDeadLayer     (std::string, Identifier);
    virtual void setDeadStation   (std::string, Identifier);
   
    virtual const std::vector<std::string>& getDeadLayers     () const;
    virtual const std::vector<std::string>& getDeadStations   () const;
    
    virtual const std::vector<Identifier>& getDeadLayersId     () const;
    virtual const std::vector<Identifier>& getDeadStationsId   () const;

    virtual const std::vector<unsigned int>& getDeadChannelsHash() const;
    
    virtual const int& getChannelStatus(IdentifierHash) const;

    virtual bool isGood           (const Identifier &    ) const;
    virtual bool isGoodLayer      (const Identifier &    ) const;
    virtual bool isGoodStation    (const Identifier &    ) const;

    virtual bool isGoodChannelHash(const IdentifierHash &) const;


 
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
