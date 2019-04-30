/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//
//NAME:     L1PSNumber.cxx
//PACKAGE:  TrigConfL1Data
//                                                     
//AUTHOR:   Tiago Perez (DESY) Tiago.Perez@desy.de
//CREATED:  19. Feb. 2010, 11:18 AM
//
//PURPOSE:  Auxiliar class to handle factional prescales. This class holds the
//  3 numbers needed to describe a fractional prescale as S. Haas and D. Berge
//  defined them.
//  Typically the constructor:
//          L1PSNumber(int64_t) from DB
//          L1PSNumber(float) or L1PSNumber(n,m,d) form XML
//  will be calledd and after that we can "get" the appropriate numbers
//  for the different arrays.
//////////////////////////////////////////////////////////////////////


#include "TrigConfL1Data/L1PSNumber.h"
#include <cmath>
#include <sstream>


namespace TrigConf {

    L1PSNumber::L1PSNumber() {
        n = 1;
        m = 1;
        d = 1;
        s = -1;
        psFloat = -1.0;
        psLong = -1;
    }

    /**
     * Simple constructor, sets only int.
     */
    L1PSNumber::L1PSNumber(const int ps) {
        if (ps >= 1) {
            n = ps;
            s = 1;
        } else if (ps < 1 && ps>-1) {
            //invalid ps, set to default -1
            n = 1;
            s = -1;
        } else {
            n = -ps;
            s = -1;
        }
        m = 0;
        d = 0;
        psFloat = (float) n;
        psLong = (int64_t) n;
    }

    /**
     * Constructor, with 64 bit integer.
     */
    L1PSNumber::L1PSNumber(const int64_t ps) {
        L1PSNumber::setInt64(ps);
    }

    /**
     * Constructor from N,M,D and S.
     */
    L1PSNumber::L1PSNumber(const int nn, const unsigned int mm,
            const unsigned int dd, const int ss = 1) {
        if (validate(nn, mm, dd, ss)) {
            psLong = makeLong(n, m, d);
        } else {
            psLong = -1;
        }
    }

    /**
     * Constructor from float.
     */
    L1PSNumber::L1PSNumber(const float ps) {
        int64_t i64 = decodeFloat(ps);
        setInt64(i64);
    }

    L1PSNumber::~L1PSNumber() {
    }

    /**
     * Checks whether the n,m,d combination is valid, i.e.
     * whether n >= 1 && 1 < (m+d+1)/(m+1) < 2.
     * Sets N, M, D & psFloat.
     */
    bool L1PSNumber::validate(int nn, unsigned int mm, unsigned int dd, int ss = 1) {
        bool ret = false;
        if (nn <= -1 || nn >= 1) {
            if (nn < 0) {
                n = -nn;
                s = -1;
            } else if (nn >= 1) {
                n = nn;
                s = ss;
            }
            m = mm;
            d = dd;
            psFloat = (float) s * n * (m + d + 1.0) / (m + 1.0);
            ret = true;
        } else {
            n = 1;
            s = -1;
            m = 0;
            d = 0;
            psFloat = -1.0;
            ret = false;
        }
        return ret;
    }

    /**
     * Sets the N,M,D values from a Int64_t prescale value.
     *
     * @param prescaleArray
     *            the prescale as a long.
     */
    void L1PSNumber::setInt64(const int64_t prescaleBitArray) {
        unsigned int nTemp = 1;
        unsigned int mTemp = 0;
        unsigned int dTemp = 0;
        int sTemp = -1;

        uint32_t psTemp = 1;
        // psLong = -1;
        // Take care of the sign.
        if (prescaleBitArray < 0) {
            sTemp = -1;
            psTemp = (uint32_t) (-1 * prescaleBitArray);
        } else {
            sTemp = 1;
            psTemp = (uint32_t) prescaleBitArray;
        }
        /**
         * Decodeonly if ps > 0. Else PS=1.
         */
        nTemp = psTemp&NMASK;
        mTemp = ((psTemp & MMASK) >> MSHIFT);
        dTemp = ((psTemp & DMASK) >> DSHIFT);
        L1PSNumber::validate(nTemp, mTemp, dTemp, sTemp);
        psLong = prescaleBitArray;
    }

    /**
     * Writes nicely the ps value.
     */
    std::string L1PSNumber::write() {
        std::stringstream ss;
        if (m == 0 && d == 0) {
            ss << getInt32();
        } else {
            ss << psFloat;
        }
        return ss.str();
    }

    /**
     * Calculates the NMD combination nearest to psF.
     * Returns the int64_t representation of NMD.
     */
    int64_t L1PSNumber::decodeFloat(const float psF) {
        int64_t ret = -1;
        int tmpN = (int) psF;
        float psOne = psF / (float) tmpN;
        float diff = 10;
        unsigned int mm = 0;
        unsigned int dd = 0;
        float tmpPS;
        float tmpDiff;
        // If the number is larger than 2^24 ret -1.
        if (unsigned (std::abs(tmpN)) <= NMASK) {
            // if psF is Int, ret psF.
            if (tmpN == psF) {
                ret = (int64_t) psF;
            } else {
                // Check the nearest Combination of N,M & D.
                for (int i = 0; i < auxLength; i++) {
                    unsigned int tmpM = psAuxValues[i][0];
                    unsigned int tmpD = psAuxValues[i][1];
                    tmpPS = (float) ((tmpM + tmpD + 1.0) / (tmpM + 1.0));
                    tmpDiff = fabs(tmpPS - psOne);
                    if (tmpPS * tmpN >= tmpN + 1) {
                        break;
                    }
                    if (tmpDiff < diff) {
                        diff = tmpDiff;
                        mm = tmpM;
                        dd = tmpD;
                    }
                }
                ret = L1PSNumber::encodeNMD(tmpN, mm, dd);
            }
        }
        return ret;
    }

    /**
     * Returns INT64_T prescale number with the correct sign as it is stored in
     * DB.
     */
    int64_t L1PSNumber::encodeNMD(const int nn, const unsigned int mm,
            const unsigned int dd) {
        int64_t ret = -1;
        unsigned int nTemp = 1;
        int sTemp = 1;
        if (nn < 0) {
            nTemp = -nn;
            sTemp = -1;
        } else {
            nTemp = nn;
            sTemp = 1;
        }
        ret = L1PSNumber::makeLong(nTemp, mm, dd);
        ret = sTemp*ret;
        return ret;
    }

    /**
     * Returns the POSITIVE int64_t of 3 ints.
     * Takes care that no number is longer than it should.
     */
    int64_t L1PSNumber::makeLong(unsigned int nn, unsigned int mm,
            unsigned int dd) {
        int64_t ret;
        ret = (mm & 0xF);
        ret = (ret << MSHIFT);
        ret = (ret | ((dd & 0xF) << DSHIFT));
        ret = (ret | (nn & NMASK));
        return ret;
    }


    /**
     * Help array containing the allowed MD combinations (for N=1). These will
     * be iterated in L1PSNumber::decodeFloat() to find the best aproximation.
     */
    const unsigned int L1PSNumber::psAuxValues[79][2] = {
        {15, 1},
        {14, 1},
        {13, 1},
        {12, 1},
        {11, 1},
        {10, 1},
        {9, 1},
        {8, 1},
        {15, 2},
        {14, 2},
        {13, 2},
        {12, 2},
        {11, 2},
        {10, 2},
        {15, 3},
        {14, 3},
        {13, 3},
        {8, 2},
        {12, 3},
        {15, 4},
        {14, 4},
        {10, 3},
        {13, 4},
        {9, 3},
        {12, 4},
        {15, 5},
        {14, 5},
        {13, 5},
        {10, 4},
        {15, 6},
        {12, 5},
        {14, 6},
        {11, 5},
        {13, 6},
        {15, 7},
        {8, 4},
        {10, 5},
        {12, 6},
        {14, 7},
        {15, 8},
        {14, 8},
        {12, 7},
        {10, 6},
        {8, 5},
        {15, 9},
        {13, 8},
        {11, 7},
        {14, 9},
        {12, 8},
        {15, 10},
        {10, 7},
        {13, 9},
        {14, 10},
        {15, 11},
        {12, 9},
        {9, 7},
        {13, 10},
        {10, 8},
        {14, 11},
        {15, 12},
        {12, 10},
        {8, 7},
        {13, 11},
        {14, 12},
        {15, 13},
        {10, 9},
        {11, 10},
        {12, 11},
        {13, 12},
        {14, 13},
        {15, 14},
        {8, 8},
        {9, 9},
        {10, 10},
        {11, 11},
        {12, 12},
        {13, 13},
        {14, 14},
        {15, 15}
    };

}
