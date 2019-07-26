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

    void setDeadTube      (std::string, Identifier);
    void setDeadLayer     (std::string, Identifier);
    void setDeadMultilayer(std::string, Identifier);
    void setDeadStation   (std::string, Identifier);
    void setDeadChamber   (Identifier);

    void setNoisyTube      (Identifier);
    void setNoisyLayer     (Identifier);
    void setNoisyMultilayer(Identifier);
    void setNoisyStation   (Identifier);
    void setNoisyChamber   (Identifier);
   
    const std::vector<std::string>& getDeadTubes      () const;
    const std::vector<std::string>& getDeadLayers     () const;
    const std::vector<std::string>& getDeadMultilayers() const;
    const std::vector<std::string>& getDeadStations   () const;
    const std::vector<std::string>& getDeadChambers   () const;
    
    const std::vector<Identifier>& getDeadTubesId      () const;
    const std::vector<Identifier>& getDeadLayersId     () const;
    const std::vector<Identifier>& getDeadMultilayersId() const;
    const std::vector<Identifier>& getDeadStationsId   () const;
    const std::vector<Identifier>& getDeadChambersId   () const;

    const std::vector<std::string>& getNoisyTubes      () const;
    const std::vector<std::string>& getNoisyLayers     () const;
    const std::vector<std::string>& getNoisyMultilayers() const;
    const std::vector<std::string>& getNoisyStations   () const;
    const std::vector<std::string>& getNoisyChambers   () const;
    
    const std::vector<Identifier>& getNoisyTubesId      () const;
    const std::vector<Identifier>& getNoisyLayersId     () const;
    const std::vector<Identifier>& getNoisyMultilayersId() const;
    const std::vector<Identifier>& getNoisyStationsId   () const;
    const std::vector<Identifier>& getNoisyChambersId   () const;
  
    bool isGood          (const Identifier & Id) const;
    bool isGoodChannel   (const Identifier & Id) const;
    bool isGoodTube      (const Identifier & Id) const;
    bool isGoodLayer     (const Identifier & Id) const;
    bool isGoodMultilayer(const Identifier & Id) const;
    bool isGoodStation   (const Identifier & Id) const;
    bool isGoodChamber   (const Identifier & Id) const;


 
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
