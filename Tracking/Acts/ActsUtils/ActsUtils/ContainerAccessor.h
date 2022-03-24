/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKING_ACTS_ACTSUTILS_CONTAINERACCESSOR_H
#define TRACKING_ACTS_ACTSUTILS_CONTAINERACCESSOR_H

#include "AthContainers/DataVector.h"

#include <boost/container/small_vector.hpp>
#include <vector>
#include <unordered_map>

#include <algorithm>
#include <functional>
#include <optional>
#include <utility>

/// @class ContainerAccessor
/// Class implementing how to access a container
/// @details It is templated on the type of the objects
/// in the container (value_t), and the type of the identifier (identifier_t)

template <typename value_t, typename identifier_t, size_t inline_size = 10>
class ContainerAccessor {

public:
    using container_t = DataVector<value_t>;
    using Iterator = typename container_t::const_iterator;
    using Range = std::pair<Iterator, Iterator>;

    /// Constructor
    /// @tparam values Container type, based on value_t
    /// @tparam mapper Function used to access the identifier_t
    /// @param max_ids Expected maximum number of identifiers,
    /// to reserve enough space in the map and avoid re-hashing
    /// @brief At construction, a map is filled with identifiers and
    /// corresponding ranges of objects in the container
    /// @note The map has to be built as soon as the container is complete.
    /// If changes in the container happen the ranges may be invalidated.
    ContainerAccessor(const container_t& values,
                          std::function<identifier_t(const value_t&)> mapper, size_t max_ids=2000) {
        m_ranges.reserve(max_ids);
        std::optional<identifier_t> prev = std::nullopt;
        Iterator groupStart = values.begin();
        for (Iterator it = values.begin(); it != values.end(); ++it) {
            identifier_t id = mapper(**it);
            if (prev && (*prev) != id) {
                m_ranges[*prev].emplace_back(groupStart, it);
                groupStart = it;
            }
            prev = id;
        }
        if (prev)
            m_ranges[*prev].emplace_back(groupStart, values.end());
    }

    /// Default destructor
    ~ContainerAccessor() = default;
    /// Copy constructor, deleted
    ContainerAccessor(const ContainerAccessor&) = delete;
    ContainerAccessor& operator=(const ContainerAccessor&) = delete;

    /// Function to return the list of ranges corresponding to a given identifier
    /// @tparam identifier Input identifier, used for returning
    /// the list of ranges from the map.
    /// @note the identifier must be contained in the map.
    /// Check if it is present first with isIdentifierPresent.
    const boost::container::small_vector<Range, inline_size> rangesForIdentifierDirect(const identifier_t& identifier) const {
        boost::container::small_vector<Range, inline_size> rangesForId = m_ranges.at(identifier);
        return rangesForId;
    }

    /// Function to verify if  a given identifier is present in the map,
    /// i.e. was present in the input container.
    /// @tparam identifier Input identifier to check.
    bool isIdentifierPresent(const identifier_t& identifier) const {
        if (m_ranges.find(identifier) == m_ranges.end())
            return false;
        return true;
    }

    /// Function to return all available identifier (i.e. keys in the map)
    std::vector<identifier_t> allIdentifiers() const {
        std::vector<identifier_t> ids{};
        ids.reserve(m_ranges.size());
        for (const auto& id_range : m_ranges)
            ids.push_back(id_range.first);
        return ids;
    };

private:
    std::unordered_map<identifier_t, boost::container::small_vector<Range, inline_size>> m_ranges;

};

#endif // TRACKING_ACTS_ACTSUTILS_CONTAINERACCESSOR_H
