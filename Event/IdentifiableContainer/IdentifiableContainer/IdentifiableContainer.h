/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_IDENTIFIABLECONTAINER_IDENTIFIABLECONTAINER_H
#define EVENT_IDENTIFIABLECONTAINER_IDENTIFIABLECONTAINER_H

#include "AthContainers/DataVector.h"

#include <boost/container/small_vector.hpp>
#include <unordered_map>

#include <algorithm>
#include <functional>
#include <optional>
#include <utility>

namespace IdCont {
    /// @class IdentifiableContainer
    /// Class implementing an identifiable container
    /// @details It is templated on the type of the objects
    /// in the container (value_t), and the type of the identifier (identifier_t)

    template <typename value_t, typename identifier_t, size_t inline_size = 10>
    class IdentifiableContainer {

    public:
        using container_t = DataVector<value_t>;
        using Iterator = typename container_t::const_iterator;
        using Range = std::pair<Iterator, Iterator>;

        /// Constructor
        /// @tparam values Container type, based on value_t
        /// @tparam mapper Function used to access the identifier_t
        /// @brief At construction, a map is filled with identifiers and
        /// corresponding ranges of objects in the container
        /// @note The map has to be built as soon as the container is complete.
        /// If changes in the container happen the ranges may be invalidated.
        IdentifiableContainer(const container_t* values,
                              std::function<identifier_t(const value_t&)> mapper) {
            std::optional<identifier_t> prev = std::nullopt;
            Iterator groupStart = values->begin();
            for (Iterator it = values->begin(); it != values->end(); ++it) {
                identifier_t id = mapper(**it);
                if (prev && (*prev) != id) {
                    m_ranges[*prev].emplace_back(groupStart, it);
                    groupStart = it;
                }
                prev = id;
            }
            m_ranges[*prev].emplace_back(groupStart, values->end());
        }

        /// Default destructor
        ~IdentifiableContainer() = default;
        /// Copy constructor, deleted
        IdentifiableContainer(const IdentifiableContainer&) = delete;
        IdentifiableContainer& operator=(const IdentifiableContainer&) = delete;

        /// Function to return the list of ranges corresponding to a given identifier
        /// @tparam identifier Input identifier, used for returning
        /// the list of ranges from the map.
        /// @note the identifier must be contained in the map.
        /// Check if it is present first with isIdentifierPresent.
        boost::container::small_vector<Range, inline_size> rangesForIdentifierDirect(const identifier_t& identifier) const {
            return m_ranges.at(identifier);
        }

        /// Function to verify if  a given identifier is present in the map,
        /// i.e. was present in the input container.
        /// @tparam identifier Input identifier to check.
        bool isIdentifierPresent(const identifier_t& identifier) {
            if (m_ranges.find(identifier) == m_ranges.end())
                return false;
            return true;
        }

        /// Function to return the list of ranges for all identifiers in the imput container.
        std::unordered_map<identifier_t, boost::container::small_vector<Range, inline_size>> allRanges() const {
            return m_ranges;
        }

    private:
        std::unordered_map<identifier_t, boost::container::small_vector<Range, inline_size>> m_ranges;

    };

} // end of namespace

#endif
