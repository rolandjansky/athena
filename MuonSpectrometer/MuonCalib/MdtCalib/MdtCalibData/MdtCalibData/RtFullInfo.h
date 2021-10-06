/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_RTFULLINFO_H
#define MUONCALIB_RTFULLINFO_H

#include <iostream>
#include <string>

namespace MuonCalib {

    /** addiitonal information used in validation of a rt-relation */

    class RtFullInfo {
    public:
        RtFullInfo() : m_implementation("implementation"), m_numIt(0), m_numSeg(0), m_meanAng(0), m_rmsAng(0), m_convEst(0), m_qualEst(0) {
            m_rmsPos = 0.0;
            m_meanPos = 0.0;
        }

        RtFullInfo(std::string impl, int numIt, int numSeg, double meanAng, double rmsAng, double convTest, double qualTest) :
            m_implementation(impl),
            m_numIt(numIt),
            m_numSeg(numSeg),
            m_meanAng(meanAng),
            m_rmsAng(rmsAng),
            m_meanPos(0.0),
            m_rmsPos(0.0),
            m_convEst(convTest),
            m_qualEst(qualTest) {}
        std::string implementation() const { return m_implementation; }
        int numIt() const { return m_numIt; }
        int numSeg() const { return m_numSeg; }
        double meanAng() const { return m_meanAng; }
        double rmsAng() const { return m_rmsAng; }
        double meanPos() const { return m_meanPos; }
        double rmsPos() const { return m_rmsPos; }
        double convEst() const { return m_convEst; }
        double qualEst() const { return m_qualEst; }

        void setImplementation(std::string i) { m_implementation = i; }
        void setNumIt(int n) { m_numIt = n; }
        void setNumSeg(int n) { m_numSeg = n; }
        void setMeanAng(double a) { m_meanAng = a; }
        void setRmsAng(double a) { m_rmsAng = a; }
        void setMeanPos(double a) { m_meanPos = a; }
        void setRmsPos(double a) { m_rmsPos = a; }
        void setConvEst(double a) { m_convEst = a; }
        void setQualEst(double a) { m_qualEst = a; }

        std::istream& read(std::istream& is) {
            is >> m_implementation >> m_numIt >> m_numSeg >> m_meanAng >> m_rmsAng >> m_convEst >> m_qualEst;
            return is;
        }
        std::ostream& write(std::ostream& os) const {
            os << " " << m_implementation << " " << m_numIt << " " << m_numSeg << " " << m_meanAng << " " << m_rmsAng << " " << m_convEst
               << " " << m_qualEst;
            return os;
        }

        void write_forDB(FILE* frt) const;

    private:
        // additional information to be used for the validation
        /** name of the implementation */
        std::string m_implementation;
        /** number of iterations */
        int m_numIt;
        /** number of segments in last iteration */
        int m_numSeg;
        /** mean of segments angular distribution */
        double m_meanAng;
        /** rms of segments angular distribution */
        double m_rmsAng;
        /** mean of the track position */
        double m_meanPos;
        /** spread of the track position */
        double m_rmsPos;
        /** parameter measuring the difference between the results of last two iterations */
        double m_convEst;
        /** parameter measuring the smoothness of the rt or the quality of the parametrisation */
        double m_qualEst;
    };

    std::istream& operator>>(std::istream& is, RtFullInfo& data);
    std::ostream& operator<<(std::ostream& os, const RtFullInfo& data);

}  // namespace MuonCalib

#endif
