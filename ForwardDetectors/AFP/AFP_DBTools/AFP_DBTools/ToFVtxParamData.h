/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   ToFVtxParamData.h
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2022-06-03
 * 
 * @baief  Header file for class ToFVtxParamData.
 * 
 */


#ifndef AFP_DBTOOLS_TOFVTXPARAMDATA_H
#define AFP_DBTOOLS_TOFVTXPARAMDATA_H


namespace AFP
{

  /// Class storing information about alignment.
  class ToFVtxParamData 
  {
  public:
  
    ToFVtxParamData(int st): m_stationID(st), m_timeGlobalOffset(0.), m_timeOffset{0.,0.,0.,0.}, m_timeSlope{0.,0.,0.,0.}, m_trainEdge{0.,0.,0.,0.,0.} {}
    ToFVtxParamData(int st, double go, const std::vector<double>& o, const std::vector<double>& s, const std::vector<double>& e) :  m_stationID(st), m_timeGlobalOffset(go),  m_timeOffset(o), m_timeSlope(s), m_trainEdge(e) {}
    
    
    /// ID of the station to which parameters apply (see xAOD::AFPStationID)
    int stationID () const {return m_stationID;}

    /// Global offset of the whole station
    double timeGlobalOffset () const {return m_timeGlobalOffset;}

    /// Time offsets for the trains
    std::vector<double> timeOffset () const {return m_timeOffset;}
    double timeOffset(int tr) const {return m_timeOffset.at(tr);}
    
    /// Time slopes for the trains
    std::vector<double> timeSlope () const {return m_timeSlope;}
    double timeSlope(int tr) const {return m_timeSlope.at(tr);}
    
     /// Train edges; the end of n-th train is also the beginning of the (n+1)-th train
    std::vector<double> trainEdge () const {return m_trainEdge;}
    double trainEdge(int tr) const {return m_trainEdge.at(tr);}

  private:
    int m_stationID;
    double m_timeGlobalOffset;
    std::vector<double> m_timeOffset;
    std::vector<double> m_timeSlope;
    std::vector<double> m_trainEdge;
  };

} // namespace AFP

#endif	//  AFP_DBTOOLS_TOFVTXPARAMDATA_H
