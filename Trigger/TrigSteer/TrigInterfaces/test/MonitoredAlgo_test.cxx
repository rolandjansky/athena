/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/IMonitoredAlgo.h"
#include "DataModel/DataVector.h"

//////////////////////////////////////////////////////////////////
// look here
// this is the way 
//////////////////////////////////////////////////////////////////

/// to fill with something collection like DataVector
class DummyTrack {
public:
  DummyTrack () {
    m_hits = 12;
    m_eta = 0.47; 
    m_phi = 1.32;
  }
  int getNhits() const { return m_hits; }
  float getTrackPhi() const { return m_phi; }
  float getTrackEta() const { return m_eta; }
private:
  int m_hits;
  float m_eta;
  float m_phi;
};

class DummyCluster {
public:
  DummyCluster() { m_eta = 0.45; m_phi = 1.4; }
  float m_eta;
  float m_phi;
};

/// Some dummy HLT::ErrorCode-like object
struct ErrorCode {
  ErrorCode() {}
  ErrorCode(int e) : m_e(e) {}
  operator int() const { return m_e; }
  int m_e;
};


// custom monitoring implementation
class MonOfDelta : public IMonitoredAlgo::IGetter {
public:
  MonOfDelta( const DataVector<DummyTrack>*& y, const DummyCluster*& z)  : m_y(y), m_z(z)  {}
  virtual unsigned size() const { return m_y->size(); } // 
  virtual double get(unsigned pos) const { return (*m_y)[pos]->getTrackPhi() - m_z->m_phi; } // pos will be given from 0 up to the size()-1.
private:
  const DataVector<DummyTrack>*& m_y;
  const DummyCluster*& m_z;
};

// 
float getTrackX0( const DummyTrack* t) {
  return t->getTrackPhi();
}

float getTrackX1( const DummyTrack* t) {
  return t->getTrackEta();
}


class TrkCollection : public DataVector<DummyTrack> {};




class SomeAlgo : public IMonitoredAlgo {
public:
  SomeAlgo() {
    // some normal c'tor stuff
    const DataVector<DummyTrack>* trk =  new DataVector<DummyTrack>;
    TrkCollection* trkcoll =  new TrkCollection();
    DummyTrack special;
    const DummyTrack* specialPtr =  new DummyTrack();
    const DummyCluster *clu  = new DummyCluster();
    
    declareMonitoredVariable( "a", m_a);
    declareMonitoredVariable( "b", m_b, 2);
    declareMonitoredStdContainer( "x", m_just_doubles);
    declareMonitoredStdContainer( "y", m_just_doubles, AutoClear);


    declareMonitoredVariable( "error1", m_err);
    declareMonitoredVariable( "error2", m_err, ErrorCode(1));

    //
    //declareMonitoredCollection( "trk", trk);
    //
    declareMonitoredCollection( "Ntracks", trk, &DummyTrack::getNhits );
    declareMonitoredCollection<DummyTrack>( "NtracksColl", trkcoll, &DummyTrack::getNhits );

    //
    declareMonitoredCollection( "CustemFunGet0", trk, &getTrackX0 );
    //
    declareMonitoredCollection( "CustemFunGet1", trk, &getTrackX1 );

    // static functions in scope of SomeAlgo
    declareMonitoredCollection( "CustemStaticFunGet0", trk, &SomeAlgo::InnerGetTrackX0 );
    declareMonitoredCollection( "CustemStaticFunGet1", trk, &SomeAlgo::InnerGetTrackX1 );

    // vector with values
    declareMonitoredCollection("col_by_val", m_tracks_by_val, &DummyTrack::getNhits);


    //use member function to get an access
    declareMonitoredObject("ObjectByMember", *this, &SomeAlgo::getB );
    //obj gien by ref  
    declareMonitoredObject("SpecialByMember", special, &DummyTrack::getNhits );
    //
    declareMonitoredObject("SpecialByExternal", special,  &getTrackX1 );
    // obj given by ptr
    declareMonitoredObject("SpecialPtrByMember", specialPtr, &DummyTrack::getNhits );
    //
    declareMonitoredObject("SpecialPtrByExternal", specialPtr,  &getTrackX1 );
    // 
    declareMonitoredObject("SizeOfCollection", trk, &DataVector<DummyTrack>::size );
    // custom IGetter class
    declareMonitoredCustomVariable ("Deltas", new MonOfDelta(trk, clu) );

  }

  void updateA(int x) {m_a = x;}
  void updateB(float x) {m_b = x;}
  float getB() const {return m_b;}
  int getC(int a) const {return int(m_b*a);}

  void appendToX(double a) { m_just_doubles.push_back(a); }

private:

  static float InnerGetTrackX0(  const DummyTrack* t) {
    return t->getTrackPhi();
  }
  static float InnerGetTrackX1(  const DummyTrack* t) {
    return t->getTrackEta();
  }


  int m_a;
  float m_b;
  std::vector<double> m_just_doubles;
  std::vector<DummyTrack> m_tracks_by_val;
  ErrorCode m_err;
};
//////////////////////////////////////////////////////////////////


int main () {
  //  try instantations of the diffent monitoring types
  SomeAlgo algo;
}
