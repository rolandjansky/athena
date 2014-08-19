/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ZHIST_H__
#define __ZHIST_H__
#include <vector>
#include <utility>
#include <set>
#include <algorithm>

class ZHist {
	public:
		ZHist(const std::vector<float>&);
		ZHist();
		~ZHist();
		float getMax() const;
		float getMaxSlidingWindow(const std::set<unsigned int> excludedBins = std::set<unsigned int>{}) const;
    const std::vector<float>& getAllMaxSlidingWindow(const unsigned int nVertices = 1);
		void fill(const float x);
		const std::vector<unsigned int>& entries() const {return m_entries;}
		const std::vector<float>& binEdges() const {return m_binEdges;}
		const std::vector<float>& vertices() const {return m_vertices;}
		unsigned int getOverflow() const {return m_overflow;}
		unsigned int getUnderflow() const {return m_underflow;}
		unsigned int getSum() const {return m_sum;}
		void clear();
	
	private:
		float getMidpoint(unsigned int) const;
    bool getMaxSlidingWindowPair(const std::set<unsigned int>& excludedBins,
                                 std::pair<unsigned int, float>& maxSlidingWindowPair) const;
		std::vector<float> m_binEdges;
		std::vector<unsigned int> m_entries;
		unsigned int m_underflow = 0;
		unsigned int m_overflow = 0;
		unsigned int m_sum = 0;
		bool m_valid = false;
		std::vector<float> m_vertices;
};

#endif
