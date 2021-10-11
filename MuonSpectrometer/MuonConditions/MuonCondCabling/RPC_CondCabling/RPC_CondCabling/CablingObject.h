/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVL1LOGICOBJECT_H
#define LVL1LOGICOBJECT_H

#include "MuonCablingTools/BaseObject.h"

class CablingObject : public BaseObject {
private:
    int m_number{0};
    int m_station{0};
    int m_sector_type{0};

public:
    struct cablingParameters {
        cablingParameters() = default;
        cablingParameters(const cablingParameters&) = default;
        cablingParameters& operator=(const cablingParameters&) = default;
        cablingParameters(cablingParameters&&) = default;

        int number{0};
        int station{0};
        int sectorType{0};
    };

    CablingObject(const cablingParameters&, const std::string&);
    CablingObject(const CablingObject&) = default;
    virtual ~CablingObject() = default;

    CablingObject& operator=(const CablingObject&) = default;

    int number() const;
    int station() const;
    int sector_type() const;

    [[nodiscard]] std::string error_header() const;
    [[nodiscard]] std::string no_connection_error(const std::string&, int) const;

    friend std::ostream& operator<<(std::ostream& stream, const CablingObject& obj);
};

#endif
