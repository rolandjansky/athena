/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHHISTO2D_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHHISTO2D_H

#include <memory.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <set>
#include <vector>
class TH2F;

/** Histogram class, similar to Root's TH2D.
    Binnumbering is similar to Root's numbering. Starting in left corner to right upper corner
    Remind the over/under flow bins */
class MuonHoughHisto2D {
public:
    /** destructor */
    ~MuonHoughHisto2D();
    /** constructor */
    MuonHoughHisto2D(int nbinsx, double xmin, double xmax, int nbinsy, double ymin, double ymax, int number_of_maxima = 1);

    /** fill 2d histogram with point (x,y) with weight w, return binnumber filled */
    int fill(double x, double y, double weight = 1.);

    /** fill bin with weight w */
    void fill(int binnumber, double weight = 1.);

    /** returns number of bins x coordinate */
    int getNbinsX() const;
    /** returns number of bins y coordinate */
    int getNbinsY() const;

    /** returns min x axis */
    double getXmin() const;
    /** returns max x axis */
    double getXmax() const;

    /** return binwidth x axis */
    double getBinWidthX() const;

    /** return binwidth y axis */
    double getBinWidthY() const;

    /** returns x axis */
    double getBinContent(int binnumber) const;

    /** clears histogram and bins_above_threshold */
    void reset();

    /** find maxima in histogram */
    void findMaxima(int printlevel = 0, bool which_segment = 0);

    /** returns binnumber and maximum of histogram (doesn't use m_bins_above_threshold) */
    std::pair<int, double> getMax() const;
    /** returns binnumber and maximum of maximum number maximum_number*/
    std::pair<int, double> getMaximumBin(unsigned int maximum_number = 0, bool which_segment = 0, int printlevel = 0);
    /** returns coords of maximum number maximum_number*/
    std::pair<double, double> getCoordsMaximum(unsigned int maximum_number = 0, bool which_segment = 0, int printlevel = 0);

    /** check when searching for several maxima if binnumber is close to an earlier found maximum */
    bool checkIfMaximumAlreadyUsed(int binnumber) const;
    /** check if binnumber is a maximum */
    bool checkIfMaximum(int binnumber, double& maximum, int& maxbin, bool which_segment = 0, int printlevel = 0) const;

    /** calculates the distance in binwidths between two binnumbers ("Manhattan metric") */
    int distanceBins(int binnumber1, int binnumber2) const;

    /** return value of maximum bin */
    double getMaximum(unsigned int maximum_number = 0, bool which_segment = 0, int printlevel = 0);
    /** return maximum binnumber */
    int getMaxBin(unsigned int maximum_number = 0, bool which_segment = 0, int printlevel = 0);

    /** return the total content of binarea (default: content of bin) */
    double content_Bin_Area(int binnumber) const;

    // histogram functions:

    /** gives coordinates for certain binnumber */
    std::pair<double, double> binnumberToCoords(int binnumber, int printlevel = 0) const;  // <x-point, y-point>
    /** gives x-coordinate for certain binnumber */
    double binnumberToXCoord(int binnumber) const;
    /** gives y-coordinate for certain binnumber */
    double binnumberToYCoord(int binnumber) const;

    /** converts coordinates to a binnumber */
    int coordToBinnumber(double x, double y) const;
    /** converts x-coordinates to binx */
    int coordToBinx(double x) const;
    /** converts y-coordinates to biny */
    int coordToBiny(double y) const;

    /** checks if bin is in histogram or if it is an under/overflow bin (unused)
     * 0 is in histo
     * 1 x underflow
     * 2 x overflow
     * 3 y underflow
     * 4 y overflow */
    int binInHistogram(unsigned int bin) const;

    /** set threshold for maxima in histogram */
    void setThreshold(double threshold);
    /** returns threshold for maxima in histogram */
    double getThreshold() const;

    /** intialises a root histogram of MuonHoughHisto2D and fill it, used for debugging purposes */
    TH2F* bookAndFillRootHistogram(const std::string& hname) const;

    void setMaximumIsValid(bool flag) { m_maximumIsValid = flag; }

private:
    /** initialises private members, called in constructor */
    void init();

    /** resets histogram */
    void resetHisto();

    /** actual storage of bin values */
    unsigned int* m_histBuffer{};
    /** size of array */
    unsigned int m_size;

    /** number of x bins */
    unsigned int m_nbinsx;
    /** number of x bins + 2 , used for cpu speedup*/
    unsigned int m_nbinsx_plus2;
    /** minimum x coordinate */
    double m_xmin;
    /** maximum x coordinate */
    double m_xmax;
    /** number of y bins */
    unsigned int m_nbinsy;
    /** number of y bins + 2 , used for cpu speedup*/
    unsigned int m_nbinsy_plus2;
    /** minimum y coordinate */
    double m_ymin;
    /** maximum y coordinate */
    double m_ymax;

    /** binwidth x axis */
    double m_binwidthx;
    /** binwidth y axis */
    double m_binwidthy;

    /** inv binwidth x axis used for cpu speedup */
    double m_invbinwidthx;
    /** inv binwidth y axis used for cpu speedup */
    double m_invbinwidthy;

    /** set of bins that are above threshold
     * used for speeding up searching for maxima */
    std::set<int> m_bins_above_threshold;

    /** binnumbers found as maxima */
    std::vector<int> m_maximumbins;

    /** number of maxima to be searched for */
    const int m_number_of_maxima;

    /** threshold for minimum content for a maximum */
    unsigned int m_scale;  // conversion from double -> unsigned int
    unsigned int m_threshold;

    /** minimum distance for a maximum to be away from another maximum */
    const int m_distance_to_next_maximum;

    /** check if search for maxima already performed */
    bool m_maxima_found;

    /** maximum */
    int m_maximumBin;
    unsigned int m_maximum;
    bool m_maximumIsValid;  // flag whether the maximum can be trusted
    /** set bin content of binnumber */
    void setBinContent(int binnumber, double value);
    /** set bin content of binnumber corresponding to coordinates */
    void setBinContent(int binx, int biny, double value);

    MuonHoughHisto2D& operator=(const MuonHoughHisto2D& right);
    MuonHoughHisto2D(const MuonHoughHisto2D&);
};

inline int MuonHoughHisto2D::fill(double x, double y, double w) {
    int binnumber = coordToBinnumber(x, y);
    fill(binnumber, w);
    return binnumber;
}

inline void MuonHoughHisto2D::fill(int binnumber, double weight) {
    unsigned int& binval = m_histBuffer[binnumber];
    binval += m_scale * weight;
    if (binval > m_maximum) {
        m_maximum = binval;
        m_maximumBin = binnumber;
    }
}
inline void MuonHoughHisto2D::setBinContent(int binnumber, double value) { m_histBuffer[binnumber] = m_scale * value; }
inline double MuonHoughHisto2D::getBinContent(int binnumber) const { return m_histBuffer[binnumber] / m_scale; }
inline double MuonHoughHisto2D::content_Bin_Area(int binnumber) const { return getBinContent(binnumber); }
inline int MuonHoughHisto2D::getNbinsX() const { return m_nbinsx; }
inline int MuonHoughHisto2D::getNbinsY() const { return m_nbinsy; }
inline double MuonHoughHisto2D::getXmin() const { return m_xmin; }
inline double MuonHoughHisto2D::getXmax() const { return m_xmax; }
inline double MuonHoughHisto2D::getBinWidthX() const { return m_binwidthx; }
inline double MuonHoughHisto2D::getBinWidthY() const { return m_binwidthy; }
inline void MuonHoughHisto2D::setThreshold(double threshold) { m_threshold = m_scale * threshold; }
inline double MuonHoughHisto2D::getThreshold() const { return m_threshold; }
inline double MuonHoughHisto2D::getMaximum(unsigned int maximum_number, bool which_segment, int printlevel) {
    return getMaximumBin(maximum_number, which_segment, printlevel).second;
}
inline int MuonHoughHisto2D::getMaxBin(unsigned int maximum_number, bool which_segment, int printlevel) {
    return getMaximumBin(maximum_number, which_segment, printlevel).first;
}
inline int MuonHoughHisto2D::coordToBinx(double x) const { return static_cast<int>((x - m_xmin) * m_invbinwidthx) + 1; }
inline int MuonHoughHisto2D::coordToBiny(double y) const { return static_cast<int>((y - m_ymin) * m_invbinwidthy) + 1; }
inline int MuonHoughHisto2D::coordToBinnumber(double x, double y) const {
    int biny = coordToBiny(y);
    int binx = coordToBinx(x);
    return binx + biny * m_nbinsx_plus2;
}
inline double MuonHoughHisto2D::binnumberToXCoord(int binnumber) const {
    return m_xmin + m_binwidthx * (-0.5 + (binnumber) % m_nbinsx_plus2);
}
inline double MuonHoughHisto2D::binnumberToYCoord(int binnumber) const {
    return m_ymin + m_binwidthy * (-0.5 + (binnumber) / m_nbinsx_plus2);
}
inline void MuonHoughHisto2D::resetHisto() { memset(m_histBuffer, 0, sizeof(unsigned int) * m_size); }

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHHISTO2D_H
