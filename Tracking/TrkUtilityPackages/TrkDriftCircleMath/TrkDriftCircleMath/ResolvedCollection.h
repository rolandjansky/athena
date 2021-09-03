/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_RESOLVEDCOLLECTION_H
#define DCMATH_RESOLVEDCOLLECTION_H

#include <iostream>
#include <vector>

namespace TrkDriftCircleMath {

    template <class Data, class IsSubset> class ResolvedCollection {
    public:
        typedef std::vector<Data> DataVec;
        enum SubsetState { SubSet = 0, SuperSet = 1, Different = 2 };

    public:
        const DataVec& data() const { return m_data; }
        DataVec& data() { return m_data; }

        void clear() { m_data.clear(); }

        void set(DataVec& data) { m_data.swap(data); }

        bool insert(const Data& data) {
            // add data if collection is empty
            if (m_data.empty()) {
                m_data.push_back(data);
                return true;
            }

            IsSubset isSubset;

            bool inserted(false);

            // loop over data vector compare new element with existing ones
            for (unsigned int i = 0; i < m_data.size(); ++i) {
                // get subset state
                int state = isSubset(data, m_data[i]);

            
                // do nothing in case the new element is a subset of an already inserted element
                if (state == SubSet) {
                    return false;

                    // if the new element is a super set of an existing on replace it
                    // check all existing elements and keep track of number of replaces
                } else if (state == SuperSet) {
                    // if not inserted yet replace the subset
                    if (!inserted) {
                        inserted = true;
                        m_data[i] = data;
                    } else {
                        // replace current element with last and restart loop
                        if (i == m_data.size() - 1) {
                            m_data.pop_back();
                        } else {
                            m_data[i] = m_data.back();
                            m_data.pop_back();
                            --i;  // here we go back to also check the copied element
                        }
                    }
                }
            }

            // check if new element was SuperSet of existing one
            if (!inserted) {
                // insert new element
                m_data.push_back(data);
            }
            return true;
        }

    private:
        std::vector<Data> m_data{};
    };

}  // namespace TrkDriftCircleMath

#endif
