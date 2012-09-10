/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOENERGYSCANRESULTS_H
#define L1CALOENERGYSCANRESULTS_H

#include "CoralBase/Blob.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/ChanFitErrorCode.h"

/**
 * Class that holds transient information about the calibration for trigger towers.
 *
 * @author Veit Scharf <veit.scharf@kip.uni-heidelberg.de>
 */

class L1CaloEnergyScanResults 
{
 public:
    enum BlobStrategy {
      NotUsed = 0x0
    };

    L1CaloEnergyScanResults();
    L1CaloEnergyScanResults(unsigned int channelId,
                            double offset,
                            double slope,
                            double chi2,
                            unsigned char ndf,
                            coral::Blob blob,
                            unsigned char blobStrategy,
                            ChanFitErrorCode errorCode);
    L1CaloEnergyScanResults(const L1CaloCoolChannelId& channelId,
                            double offset,
                            double slope,
                            double chi2,
                            unsigned char ndf,
                            coral::Blob blob,
                            unsigned char blobStrategy,
                            ChanFitErrorCode errorCode);
    
    virtual ~L1CaloEnergyScanResults() {}

    inline const L1CaloCoolChannelId& channelId() const { return m_channelId; }
    inline unsigned char blobStrategy() const { return m_blobStrategy; }
    inline double offset() const { return m_offset; }
    inline double slope() const { return m_slope; }
    inline double chi2() const { return m_chi2; }
    inline unsigned char ndf() const { return m_ndf; }
    inline const coral::Blob& blob() const { return m_blob; }
    inline const ChanFitErrorCode& errorCode() const { return m_errorCode; }

 private:
    L1CaloCoolChannelId m_channelId;

    double m_offset;
    double m_slope;
    double m_chi2;
    unsigned char m_ndf;
    coral::Blob m_blob;
    unsigned char m_blobStrategy;
    ChanFitErrorCode m_errorCode;
};

#endif // L1CALOENERGYSCANRESULTS_H
