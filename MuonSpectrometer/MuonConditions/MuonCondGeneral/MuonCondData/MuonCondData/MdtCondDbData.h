/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_MDTCONDDBDATA_H
#define MUONCONDDATA_MDTCONDDBDATA_H

//STL includes
#include <string>
#include <vector>

//Athena includes
#include "MuonIdHelpers/MdtIdHelper.h"
#include "Identifier/Identifier.h"
//#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaKernel/CondCont.h" 
#include "AthenaKernel/BaseInfo.h" 


//forward declarations
class Identifier;
class MdtIdHelper;


class MdtCondDbData {

  friend class MdtCondDbAlg;

public:

    MdtCondDbData();
    virtual ~MdtCondDbData() = default;

    virtual void setDeadTube      (std::string, Identifier);
    virtual void setDeadLayer     (std::string, Identifier);
    virtual void setDeadMultilayer(std::string, Identifier);
    virtual void setDeadStation   (std::string, Identifier);
    virtual void setDeadChamber   (Identifier);

    virtual void setNoisyTube      (Identifier);
    virtual void setNoisyLayer     (Identifier);
    virtual void setNoisyMultilayer(Identifier);
    virtual void setNoisyStation   (Identifier);
    virtual void setNoisyChamber   (Identifier);
   
    virtual const std::vector<std::string>& getDeadTubes      () const;
    virtual const std::vector<std::string>& getDeadLayers     () const;
    virtual const std::vector<std::string>& getDeadMultilayers() const;
    virtual const std::vector<std::string>& getDeadStations   () const;
    virtual const std::vector<std::string>& getDeadChambers   () const;
    
    virtual const std::vector<Identifier>& getDeadTubesId      () const;
    virtual const std::vector<Identifier>& getDeadLayersId     () const;
    virtual const std::vector<Identifier>& getDeadMultilayersId() const;
    virtual const std::vector<Identifier>& getDeadStationsId   () const;
    virtual const std::vector<Identifier>& getDeadChambersId   () const;

    virtual const std::vector<std::string>& getNoisyTubes      () const;
    virtual const std::vector<std::string>& getNoisyLayers     () const;
    virtual const std::vector<std::string>& getNoisyMultilayers() const;
    virtual const std::vector<std::string>& getNoisyStations   () const;
    virtual const std::vector<std::string>& getNoisyChambers   () const;
    
    virtual const std::vector<Identifier>& getNoisyTubesId      () const;
    virtual const std::vector<Identifier>& getNoisyLayersId     () const;
    virtual const std::vector<Identifier>& getNoisyMultilayersId() const;
    virtual const std::vector<Identifier>& getNoisyStationsId   () const;
    virtual const std::vector<Identifier>& getNoisyChambersId   () const;
  
    virtual bool isGood          (const Identifier & Id) const;
    virtual bool isGoodChannel   (const Identifier & Id) const;
    virtual bool isGoodTube      (const Identifier & Id) const;
    virtual bool isGoodLayer     (const Identifier & Id) const;
    virtual bool isGoodMultilayer(const Identifier & Id) const;
    virtual bool isGoodStation   (const Identifier & Id) const;
    virtual bool isGoodChamber   (const Identifier & Id) const;


 
private:

    std::vector<std::string> m_cachedDeadTubes;
    std::vector<std::string> m_cachedDeadLayers;
    std::vector<std::string> m_cachedDeadMultilayers;
    std::vector<std::string> m_cachedDeadStations;
    std::vector<std::string> m_cachedDeadChambers;

    std::vector<Identifier> m_cachedDeadTubesId;
    std::vector<Identifier> m_cachedDeadLayersId;
    std::vector<Identifier> m_cachedDeadMultilayersId;
    std::vector<Identifier> m_cachedDeadStationsId;
    std::vector<Identifier> m_cachedDeadChambersId;
 
    std::vector<std::string> m_cachedNoisyTubes;
    std::vector<std::string> m_cachedNoisyLayers;
    std::vector<std::string> m_cachedNoisyMultilayers;
    std::vector<std::string> m_cachedNoisyStations;
    std::vector<std::string> m_cachedNoisyChambers;

    std::vector<Identifier> m_cachedNoisyTubesId;
    std::vector<Identifier> m_cachedNoisyLayersId;
    std::vector<Identifier> m_cachedNoisyMultilayersId;
    std::vector<Identifier> m_cachedNoisyStationsId;
    std::vector<Identifier> m_cachedNoisyChambersId;

    std::vector<std::string> m_emptyNames;
    std::vector<Identifier> m_emptyIds; 

    const MdtIdHelper * m_idHelper;

};

CLASS_DEF( MdtCondDbData, 58088442, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<MdtCondDbData>, 62077248, 0)

#endif
