/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONSIMUTRACK_H
#define MUONSIMUTRACK_H

#include<iostream>
#include"TrigT1RPClogic/RPCtrigDataObject.h"

/*

#define l1event l1event_

extern "C" struct gflag{
	int EventNum;
} l1event;


#define gcflag gcflag_
extern "C" struct
{
  int IDEBUG;
  int IDEMIN;
  int IDEMAX;
  int ITEST;
  int IDRUN;
  int IDEVT;
  int IEORUN;
  int IEOTRI;
  int IEVENT;
  int ISWIT[10];
  int IFINIT[20];
  int NEVENT;
  int NRNDM[2];
} gcflag;
*/

struct SimuTrackData
{
    int event_number;
    int vertex_number;
    int track_number;
    
    float Px;
    float Py;
    float Pz;
    float Energy;
};


class MuonSimuTrack : public RPCtrigDataObject
{
    private:
    int m_event_number;
    int m_vertex_number;
    int m_track_number;

    float m_Px;
    float m_Py;
    float m_Pz;
    float m_Energy;

    public:
    MuonSimuTrack(int,const SimuTrackData&);
    MuonSimuTrack(const MuonSimuTrack&);
    ~MuonSimuTrack() {}

    MuonSimuTrack operator = (const MuonSimuTrack&);

    public:
    int event_number(void) const     {return m_event_number;}
    int vertex_number(void) const    {return m_vertex_number;}
    int track_number(void) const     {return m_track_number;}

    float Pt(void) const;
    float generation_theta(void) const;
    float generation_eta(void) const; 
    float generation_phi(void) const; 

    float Px(void) const {return m_Px;}
    float Py(void) const {return m_Py;}
    float Pz(void) const {return m_Pz;}
    float Energy(void) const {return m_Energy;}

    void Print(std::ostream&,bool) const;
};


template <class X> X& operator<<(X& stream,const MuonSimuTrack& track)
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[10000];
    for (int i=0;i<10000;++i) buffer[i] = '\0';
    __osstream display(buffer,10000);
#endif

    track.Print(display,false);
    stream << display.str();
    return stream;
}


#endif
