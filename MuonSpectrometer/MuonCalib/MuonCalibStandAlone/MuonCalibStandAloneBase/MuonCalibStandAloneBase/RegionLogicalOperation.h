/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RegionLogicalOperation_H
#define RegionLogicalOperation_H

#include "RegionSelectorBase.h"

// c - c++
#include "vector"

namespace MuonCalib {

    class MuonFixedId;

    class RegionLogicalOperation : public RegionSelectorBase {
    public:
        /** Constructor */
        RegionLogicalOperation() = default;
        /** Destructor */
        virtual ~RegionLogicalOperation() = default;
        /** Add a region to the operation
                @param region: the region to be added
                @param invert: inverted logic for this region
        Returns false when number of operators is insufficient
        */
        bool AddRegion(std::unique_ptr<RegionSelectorBase> region, bool invert = false);
        /** Set next operator
                @param op: true=or false=and
        return false if number of operators is too high
                */
        bool AddOperator(bool op);
        /** check for surplus operators */
        bool SurplusOperator() const {
            if (m_operator.size() > m_regions.size() - 1) return true;
            return false;
        }
        /** return the reslut of the logical operation*/
        bool Result(const MuonFixedId &region) const;
        void Print(std::ostream &os) const;

    private:
        //! region data
        std::vector<std::unique_ptr<RegionSelectorBase> > m_regions;
        //! region operator - true=or false=and
        std::vector<bool> m_operator;
        //! inverse region selection
        std::vector<bool> m_inverse;
    };

}  // namespace MuonCalib
#endif
