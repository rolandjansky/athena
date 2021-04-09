/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HistogramId_H
#define HistogramId_H

// c - c++
#include <map>
#include <string>
#include <utility>

namespace MuonCalib {

    class MuonFixedId;

    /** @class HistogramId
    Identifier class for drift time histograms. The class can be switched to the identification of single tubes, and different types of
    groupes of tubes.

    @author Felix.Rauscher@Physik.Uni-Muenchen.de
    */

    class HistogramId {
    public:
        /** constructor - does nothing*/
        inline HistogramId() : m_id(0, 0) {}

        /** initialize
          @param id MuonCalibFixedId of the tube
          @param sort_by (HistogramId :: TUBE, HistogramId :: LAYER ..)
         */
        void Initialize(const MuonFixedId &id, int sort_by);
        /** comparision operator defined so that this class can
        be used as a map index. The behaviour depends on the setting of the
        sort_by parameter of the Initialize function.
        */
        inline bool operator>(const HistogramId &other) const { return m_id > other.m_id; }
        /** comparision operator defined so that this class can
        be used as a map index. The behaviour depends on the setting of the
        sort_by parameter of the Initialize function.
        */
        inline bool operator<(const HistogramId &other) const { return m_id < other.m_id; }
        /** valid values of the sort_by argument of the Initialize function */
        static const int TUBE = 4, LAYER = 2, MULTILAYER = 1, CHAMBER = 0, MEZZ_CARD = 3;
        /** get numberic id */
        inline int getIdInt() const { return m_id.second; }
        /** get ascii histogram name */
        inline const std::string &HistogramName() const { return s_histogram_names[m_id]; }

    private:
        //! integer identity - value depends on the sort_by argument of the Initialize-Function
        std::pair<int, int> m_id;
        //! ascii histogram names
        static std::map<std::pair<int, int>, std::string> s_histogram_names;
    };

}  // namespace MuonCalib

#endif
