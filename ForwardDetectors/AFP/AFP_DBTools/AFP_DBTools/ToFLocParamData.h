/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   ToFLocParamData.h
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2022-06-03
 * 
 * @baief  Header file for class ToFLocParamData.
 * 
 */


#ifndef AFP_DBTOOLS_TOFLOCPARAMDATA_H
#define AFP_DBTOOLS_TOFLOCPARAMDATA_H


namespace AFP
{

  /// Class storing information about alignment.
  class ToFLocParamData 
  {
  public:
  
    ToFLocParamData(int st, int tr, int ba): m_stationID(st), m_trainID(tr), m_barID(ba), m_barWeight(0.), m_barTimeOffset(0.) {}
    ToFLocParamData(int st, int tr, int ba, double w, double t) :  m_stationID(st), m_trainID(tr), m_barID(ba), m_barWeight(w), m_barTimeOffset(t) {}
    
  
    /// ID of the station to which parameters apply (see xAOD::AFPStationID)
    int stationID () const {return m_stationID;}

    /// ID of the train in the station to which parameters apply
    int trainID () const {return m_trainID;}
    
    /// ID of the bar in the train to which parameters apply
    int barID () const {return m_barID;}

    /// Weight for this particular bar
    double barWeight () const {return m_barWeight;}

    /// Time offset for this particular bar
    double barTimeOffset () const {return m_barTimeOffset;}

  private:
    int m_stationID;
    int m_trainID;
    int m_barID;
    double m_barWeight;
    double m_barTimeOffset;
  };

} // namespace AFP

#endif  //  AFP_DBTOOLS_TOFLOCPARAMDATA_H
