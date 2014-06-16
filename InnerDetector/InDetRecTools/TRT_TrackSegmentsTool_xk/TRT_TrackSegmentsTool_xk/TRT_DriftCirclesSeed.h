/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_DriftCirclesSeed
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for trt drift circles seeds
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 18/11/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_DriftCirclesSeed_h
#define TRT_DriftCirclesSeed_h
#include <list>
#include "InDetPrepRawData/TRT_DriftCircle.h"

class MsgStream;

namespace InDet {

  class TRT_DriftCirclesSeed {
    
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
    
  public:
    
    TRT_DriftCirclesSeed();
    TRT_DriftCirclesSeed(const TRT_DriftCirclesSeed&);
    TRT_DriftCirclesSeed& operator = (const TRT_DriftCirclesSeed&);
    virtual ~TRT_DriftCirclesSeed();
    void erase();
    void add(const InDet::TRT_DriftCircle*&);
    const std::list<const InDet::TRT_DriftCircle*>& driftCircles() const;
    virtual MsgStream&    dump(MsgStream&    out) const ;
    virtual std::ostream& dump(std::ostream& out) const ;
    
    /////////////////////////////////////////////////////////////////////////////////
    // Protected data members
    /////////////////////////////////////////////////////////////////////////////////
    
  protected:
    
    std::list<const InDet::TRT_DriftCircle*> m_driftcircles;
 
  };

  MsgStream&    operator << (MsgStream&   ,const TRT_DriftCirclesSeed&);
  std::ostream& operator << (std::ostream&,const TRT_DriftCirclesSeed&); 

  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////
      
  inline TRT_DriftCirclesSeed::TRT_DriftCirclesSeed ()
    {
    }
 
  inline TRT_DriftCirclesSeed::~TRT_DriftCirclesSeed() 
    {
    }

  inline const std::list<const InDet::TRT_DriftCircle*>& 
    TRT_DriftCirclesSeed::driftCircles() const 
    {
      return this->m_driftcircles;
    }

  inline void TRT_DriftCirclesSeed::erase() 
    {
      m_driftcircles.erase(m_driftcircles.begin(),m_driftcircles.end());
    }

  inline void TRT_DriftCirclesSeed::add(const InDet::TRT_DriftCircle*& dc) 
    {
      m_driftcircles.push_back(dc);
    }
  
} // end of name space

#endif  // TRT_DriftCirclesSeed_h
