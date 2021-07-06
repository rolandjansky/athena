/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib__IConditionStorage_h
#define MuonCalib__IConditionStorage_h

// c - c++
#include "map"
#include "string"
#include "vector"

namespace coral {
    class AttributeList;
}

namespace MuonCalib {

    class SamplePoint;

    class IConditionsStorage {
    public:
        class TubeId {
        public:
            int ml, ly, tb;
            TubeId(int id_num);
            inline TubeId() : ml(0), ly(0), tb(0) {}
            bool operator<(const TubeId &other) const;
            bool operator>(const TubeId &other) const;
        };
        inline IConditionsStorage() {}
        inline virtual ~IConditionsStorage() {}
        // request rows from MDT_TUBE table
        inline virtual const std::vector<std::string> &RowsFromMdtTubeTable() const { return m_empty; }
        // request rows from MDT_TUBE_V table
        inline virtual const std::vector<std::string> &RowsFromMdtTubeVTable() const { return m_empty; }
        // request rows from MDT_TUBE_C table
        inline virtual const std::vector<std::string> &RowsFromMdtTubeCTable() const { return m_empty; }

        // call back function for every t0 chamber
        virtual bool StoreT0Chamber(const int &chamber, const std::map<TubeId, coral::AttributeList> &rows) = 0;
        // call back for rt
        virtual bool StoreRtChamber(const int &chamber, const std::map<int, SamplePoint> &points) = 0;

    private:
        std::vector<std::string> m_empty;
    };

}  // namespace MuonCalib

#endif
