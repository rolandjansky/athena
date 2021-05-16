/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERIDENTITY_H
#define TRIGGERIDENTITY_H

#include <iostream>
#include <sstream>
typedef std::ostringstream osstream;

#define NTYPE 3
#define NTHRESHOLD 3

enum TrigType { None, Low, High };
enum Threshold { FirstTh, SecondTh, ThirdTh };

class TRIGGERidentity {
private:
    static const char s_type_tag[NTYPE][8];
    static const char s_threshold_tag[NTHRESHOLD][7];
    static const float s_pt_value[NTYPE][NTHRESHOLD];

private:
    TrigType m_type;
    Threshold m_threshold;
    bool m_overlap;

public:
    TRIGGERidentity();
    TRIGGERidentity(TrigType, Threshold, bool);
    TRIGGERidentity(const TRIGGERidentity&);

    TRIGGERidentity& operator=(const TRIGGERidentity&);
    bool operator==(const TRIGGERidentity&) const;
    bool operator!=(const TRIGGERidentity&) const;
    bool operator<(const TRIGGERidentity&) const;

    operator bool();
    bool operator!();

    TrigType type(void) const { return m_type; }
    Threshold threshold(void) const { return m_threshold; }
    bool overlap(void) const { return m_overlap; }
    float pt(void) const { return s_pt_value[m_type][m_threshold]; }

    bool searchForHigh(void) const;
    bool highest(TrigType) const;

    void Print(std::ostream&, bool) const;
};

template <class X> X& operator<<(X& stream, const TRIGGERidentity& data) {
    osstream display;

    data.Print(display, false);
    stream << display.str();
    return stream;
}

#endif
