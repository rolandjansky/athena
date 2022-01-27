/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHHISTO2DCONTAINER_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHHISTO2DCONTAINER_H

#include "MuonHoughPatternEvent/MuonHoughHisto2D.h"

class MuonHoughHisto2DContainer {
public:
    /** constructor */
    MuonHoughHisto2DContainer();
    /** destructor */
    virtual ~MuonHoughHisto2DContainer() = default;

    /** return histogram at place id */
    MuonHoughHisto2D* getHisto(int id) const;

    /** return maximum of container
     * gives first id histogram (sector) and then maximumbin */
    std::pair<int, int> getMaximumBinnumber(unsigned int maximum_number, bool which_segment, int printlevel = 0) const;

    /** resets histograms */
    void reset() const;
    /** returns size of container */
    int size() const;
    /** reserve a certain size (memory) for m_histos */
    void reserve(int);
    /** push_back a histogram */
    void push_back(MuonHoughHisto2D*);

private:
    /** vector of MuonHoughHisto2D* */
    std::vector<MuonHoughHisto2D*> m_histos;
};

inline int MuonHoughHisto2DContainer::size() const { return m_histos.size(); }
inline MuonHoughHisto2D* MuonHoughHisto2DContainer::getHisto(int id) const { return m_histos[id]; }
inline void MuonHoughHisto2DContainer::reserve(int size) { m_histos.reserve(size); }
inline void MuonHoughHisto2DContainer::push_back(MuonHoughHisto2D* histo) { m_histos.push_back(histo); }

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHHISTO2DCONTAINER_H
