/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_TGCCONDDBDATA_H
#define MUONCONDDATA_TGCCONDDBDATA_H

//STL includes
#include <string>
#include <vector>

//Athena includes
#include "Identifier/Identifier.h"
#include "AthenaKernel/CondCont.h" 
#include "AthenaKernel/BaseInfo.h" 


//forward declarations
class Identifier;


class TgcCondDbData {

  friend class TgcCondDbAlg;

public:

    TgcCondDbData();
    virtual ~TgcCondDbData() = default;

    void setDeadStation(Identifier);

    const std::vector<Identifier>& getDeadStationsId() const;

    bool isGood          (const Identifier & Id) const;
    bool isGoodStation   (const Identifier & Id) const;


 
private:

    std::vector<Identifier> m_cachedDeadStationsId;

    std::vector<std::string> m_emptyNames;
    std::vector<Identifier> m_emptyIds; 

    static bool Compare(const Identifier &a, const Identifier &b) {return (a>b);}

};

CLASS_DEF( TgcCondDbData, 130737053, 1)
CLASS_DEF( CondCont<TgcCondDbData>, 178252645, 0)

#endif
