/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of MdtROD_Encoder class

#include "MdtROD_Encoder.h"

#include <math.h>

#include <algorithm>
#include <map>

#include "MDT_Hid2RESrcID.h"
#include "MdtAmtReadOut.h"
#include "MdtCsmReadOut.h"
#include "MdtHptdcReadOut.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "StoreGate/StoreGateSvc.h"


MdtROD_Encoder::MdtROD_Encoder() {
    if (!m_mdtIdHelper.retrieve().isSuccess()) {
        throw std::runtime_error("MdtROD_Encoder -- Failed to initialize the IdHelperSvc");
    }
}
// Add a CSM
void MdtROD_Encoder::add(const MdtCsm* csm) {
    m_vMdtCsm.push_back(csm);   
}

// Clear the vector of CSMs
void MdtROD_Encoder::clear() {
    m_vMdtCsm.erase(m_vMdtCsm.begin(), m_vMdtCsm.end());   
}

/** convert all MDT CSMs in the current list to
  a vector of 32bit words
*/

void MdtROD_Encoder::fillROD(std::vector<uint32_t>& v) {    

    MdtAmtReadOut amtReadOut{};
    MdtHptdcReadOut hptdcReadOut{};
    MdtCsmReadOut csmReadOut{};
  
    uint32_t mrod_wcnt{0};

    using hit_vector = std::vector<const MdtAmtHit*>;
    using tdc_map = std::map<uint16_t, hit_vector>;
    using masked_map = std::map<uint16_t, uint32_t>;

    tdc_map tdcMap;
    masked_map maskedMap;

    // Insert the Beginning of Block Word (Lvl1ID not set yet)
    uint32_t lvl1Id = 0;
    v.push_back(csmReadOut.makeBOB(lvl1Id));

    // make the body of the ROD
    // Loop on the CSMs
    for ( const MdtCsm* csm :  m_vMdtCsm) {
     
        const bool isHPTDC = m_mdtIdHelper->hasHPTDC(csm->identify());

        uint16_t ctwc = 0;  // Trailer word count initialized

        v.push_back(csmReadOut.makeLWC(0));                            // Link Word Count
        v.push_back(csmReadOut.makeBOL(csm->MrodId(), csm->CsmId()));  // Beginning of link

        v.push_back(csmReadOut.makeTLP(0));
        ++ctwc;  // Trailer word count starts from TLP

        // Group the hits of this CSM according to the TDC number
        tdcMap.clear();
        maskedMap.clear();
        for (const MdtAmtHit* amt : *csm) {
            uint16_t tdcNum = amt->tdcId();

            if (!amt->isMasked()) {
                tdcMap[tdcNum].push_back(amt);
            } else {
                maskedMap[tdcNum] = maskedMap[tdcNum] | (int)pow(2, amt->channelId());
            }
        }

        // Loop on the TDCs and build the corresponding MROD blocks
        uint16_t jt = 0;
        tdc_map::const_iterator it_tdc = tdcMap.begin();
        for (; it_tdc != tdcMap.end(); ++it_tdc) {
            // Beginning of TDC header word
            // Need to fix event counter and bcid
            v.push_back((isHPTDC ? hptdcReadOut.makeBOT((*it_tdc).first, 0, 0) : amtReadOut.makeBOT((*it_tdc).first, 0, 0)));
            ++ctwc;

            uint32_t maskedFlags = maskedMap[(*it_tdc).first];
            uint16_t wcnt = 0;  // TDC word count

            // Masked channels flags
            if (maskedFlags != 0) {
                v.push_back((isHPTDC ? 0 : amtReadOut.makeTMC(jt, maskedFlags)));
                ++ctwc;
                ++wcnt;
            }

            // Loop on the hits for this TDC
            hit_vector::const_iterator it_amtvec = (*it_tdc).second.begin();
            for (; it_amtvec != (*it_tdc).second.end(); ++it_amtvec) {
                uint16_t chan = (*it_amtvec)->channelId();                

                uint16_t coarse = (*it_amtvec)->coarse();
                uint16_t fine = (*it_amtvec)->fine();
                uint16_t width = (*it_amtvec)->width();
                uint16_t tdcId = (*it_amtvec)->tdcId();

                // Add a "Single Measurement" word
                // v.push_back( isHPTDC ? hptdcReadOut.makeTSM(tdcId, chan, leading, coarse, fine)
                //                    : amtReadOut.makeTSM(jt, chan, leading, errflag, coarse, fine) );

                // Add a combined measurement data word
                v.push_back(
                    (isHPTDC ? hptdcReadOut.makeTCM(tdcId, chan, width, coarse, fine) : amtReadOut.makeTCM(jt, chan, width, coarse, fine)));

                ++ctwc;  // CSM word count
                ++wcnt;  // TDC word count
            }

            uint16_t jt{0}, tdcId{0}, ecnt{0};  // Event counter
            // End of TDC trailer
            v.push_back((isHPTDC ? hptdcReadOut.makeEOT(tdcId, ecnt, wcnt + 2) : amtReadOut.makeEOT(jt, ecnt, wcnt + 2)));
            ++ctwc;
        }

        // Last word of each csm: TWC trailer word count
        uint16_t ecnt = 0;
        v.push_back(csmReadOut.makeTWC(ecnt, ctwc));

        // Update the MROD word count
        mrod_wcnt += ctwc;
        mrod_wcnt += 2;
    }

    // Close the MROD: MROD word count in EOB
    v.push_back(csmReadOut.makeEOB(mrod_wcnt));
  
}
