/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_MDTCONDDBDATA_H
#define MUONCONDDATA_MDTCONDDBDATA_H

//STL includes
#include <string>
#include <vector>

//Athena includes
#include "AthenaKernel/CondCont.h" 
#include "AthenaKernel/BaseInfo.h" 

//forward declarations
class Identifier;
class MdtIdHelper;


class MdtCondDbData {

  friend class MdtCondDbAlg;

public:

    MdtCondDbData(const MdtIdHelper& id_helper);
    
    
    virtual ~MdtCondDbData() = default;

    void setDeadTube      (std::string_view, Identifier);
    void setDeadLayer     (std::string_view, Identifier);
    void setDeadMultilayer(std::string_view, Identifier);
    void setDeadStation   (std::string_view, Identifier);
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
  
    /// Returns if the identifier (tube/multiLayer/chamber) is masked
    /// in the conditions database
    bool isGood          (const Identifier & Id) const;
    /// Returns whether the particular tube has 
    ///been markes as bad in the database
    bool isGoodTube      (const Identifier & Id) const;
    /// Returns whether the corresponding tube layer is
    /// marked as bad in the database    
    bool isGoodLayer     (const Identifier & Id) const;
    bool isGoodMultilayer(const Identifier & Id) const;
    ///  Returns true if the chamber is switched off in the 
    ///  database
    bool isGoodStation   (const Identifier & Id) const;
    ///  Returns true if the complete chamber has not dead channels
    bool isGoodChamber   (const Identifier & Id) const;
    
    /// Alias of isGood (to be removed in a future MR)
    bool isGoodChannel   (const MdtIdHelper *, const Identifier & Id) const;

 
private:

    std::vector<std::string> m_cachedDeadTubes{};
    std::vector<std::string> m_cachedDeadLayers{};
    std::vector<std::string> m_cachedDeadMultilayers{};
    std::vector<std::string> m_cachedDeadStations{};
    std::vector<std::string> m_cachedDeadChambers{};

    std::vector<Identifier> m_cachedDeadTubesId{};
    std::vector<Identifier> m_cachedDeadLayersId{};
    std::vector<Identifier> m_cachedDeadMultilayersId{};
    std::vector<Identifier> m_cachedDeadStationsId{};
    std::vector<Identifier> m_cachedDeadChambersId{};
 
    std::vector<std::string> m_cachedNoisyTubes{};
    std::vector<std::string> m_cachedNoisyLayers{};
    std::vector<std::string> m_cachedNoisyMultilayers{};
    std::vector<std::string> m_cachedNoisyStations{};
    std::vector<std::string> m_cachedNoisyChambers{};

    std::vector<Identifier> m_cachedNoisyTubesId{};
    std::vector<Identifier> m_cachedNoisyLayersId{};
    std::vector<Identifier> m_cachedNoisyMultilayersId{};
    std::vector<Identifier> m_cachedNoisyStationsId{};
    std::vector<Identifier> m_cachedNoisyChambersId{};

    const  MdtIdHelper& m_id_helper;   

};

CLASS_DEF( MdtCondDbData, 58088442, 1)
CLASS_DEF( CondCont<MdtCondDbData>, 62077248, 0)

#endif
