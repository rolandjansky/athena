/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughHisto2DContainer.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

MuonHoughHisto2DContainer::MuonHoughHisto2DContainer() {}

std::pair<int, int> MuonHoughHisto2DContainer::getMaximumBinnumber(unsigned int maximum_number, bool which_segment, int printlevel) const {
    MsgStream log(Athena::getMessageSvc(), "MuonHoughHisto2DContainer::getMaximumBinnumber");
    // does only work for maximum_number==0!
    if (maximum_number != 0) {
        if (log.level() <= MSG::WARNING)
            log << MSG::WARNING << "WARNING: possibly maximum taken which is not " << maximum_number + 1 << "rd maximum" << endmsg;
    }

    double maximum = 0.;
    int maxid = -1;
    int maxbin = -1;

    for (int histoid = 0; histoid < size(); histoid++) {
        std::pair<int, double> histomax =
            getHisto(histoid)->getMaximumBin(0, which_segment, printlevel);  // binnumber and value of the area of the bin

        if (histomax.second > maximum) {
            maximum = histomax.second;
            maxid = histoid;
            maxbin = histomax.first;
        }
    }

    std::pair<int, int> coordsmaximum(maxid, maxbin);

    return coordsmaximum;
}

void MuonHoughHisto2DContainer::reset() const {
    for (int histoid = 0; histoid < size(); histoid++) { getHisto(histoid)->reset(); }
}
