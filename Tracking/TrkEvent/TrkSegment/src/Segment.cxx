/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Segment.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkSegment/Segment.h"
#include "TrkEventPrimitives/FitQuality.h"

unsigned int Trk::Segment::s_numberOfInstantiations=0;

// default constructor
Trk::Segment::Segment()
    : Trk::MeasurementBase(),
      m_fitQuality(nullptr),
      m_containedMeasBases( nullptr ),
      m_author(AuthorUnknown)
{
#ifndef NDEBUG
  s_numberOfInstantiations++; // new Segment, so increment total count
#endif
}

// copy constructor
Trk::Segment::Segment(const Trk::Segment& seg)
    : Trk::MeasurementBase(seg),
    m_fitQuality(seg.m_fitQuality ? seg.m_fitQuality->clone() : nullptr),
    m_containedMeasBases( new DataVector<const Trk::MeasurementBase>  ),
    m_author( seg.m_author)
{
    m_containedMeasBases->reserve(seg.m_containedMeasBases->size());
    for(const Trk::MeasurementBase *const measurement : *(seg.m_containedMeasBases)) {
       m_containedMeasBases->push_back(measurement->clone());
    }
#ifndef NDEBUG
  s_numberOfInstantiations++; // new Segment, so increment total count
#endif
}



// copy constructor
Trk::Segment::Segment(const Trk::LocalParameters& locpars,
                      const Amg::MatrixX& locerr,
                      DataVector<const MeasurementBase>* measurements,
                      FitQuality* fitqual, 
		      Author author )
    :
    Trk::MeasurementBase(locpars,locerr),
    m_fitQuality(fitqual),
    m_containedMeasBases(measurements),
    m_author(author)
{
#ifndef NDEBUG
  s_numberOfInstantiations++; // new Segment, so increment total count
#endif
}


// move constructor
Trk::Segment::Segment(Trk::Segment&& seg)
     : Trk::MeasurementBase(seg),
       m_author( seg.m_author)
{
     m_fitQuality = seg.m_fitQuality;
     seg.m_fitQuality = nullptr;
     m_containedMeasBases = seg.m_containedMeasBases;
     seg.m_containedMeasBases = nullptr;
#ifndef NDEBUG
     s_numberOfInstantiations++; // new Segment, so increment total count
#endif 
}


// destructor - child save
Trk::Segment::~Segment()
{

   delete m_containedMeasBases;
   delete m_fitQuality;

#ifndef NDEBUG
   s_numberOfInstantiations--; // delete Segment, so decrement total count
#endif
}


// assignment operator
Trk::Segment& Trk::Segment::operator=(const Trk::Segment& seg)
{
  if (this!=&seg){
    Trk::MeasurementBase::operator=(seg);
    delete m_fitQuality;
    
    m_fitQuality = seg.m_fitQuality ? seg.m_fitQuality->clone() : nullptr;
    if (seg.m_containedMeasBases) {
      if (!m_containedMeasBases) {
         m_containedMeasBases = new DataVector<const Trk::MeasurementBase>;
      }  
       else {
           m_containedMeasBases->clear();
       }
      m_containedMeasBases->reserve(seg.m_containedMeasBases->size());
      for(const Trk::MeasurementBase *const measurement : *(seg.m_containedMeasBases)) {
       m_containedMeasBases->push_back(measurement->clone());
      } 
    }
    else {
      delete m_containedMeasBases;
      m_containedMeasBases = nullptr; 
    }
    m_author = seg.m_author;
  }
  return (*this);
}


// move assignment operator
Trk::Segment& Trk::Segment::operator=(Trk::Segment&& seg) {
   if (this!=&seg){
     Trk::MeasurementBase::operator=(seg);
     delete m_fitQuality;
     m_fitQuality = seg.m_fitQuality;
     seg.m_fitQuality = nullptr;
     delete m_containedMeasBases;
     m_containedMeasBases = seg.m_containedMeasBases;
     seg.m_containedMeasBases = nullptr;
     m_author = seg.m_author;
   }
   return (*this);
 } 




unsigned int Trk::Segment::numberOfInstantiations()
{
  return s_numberOfInstantiations;
}


std::string Trk::Segment::dumpAuthor() const
{
  std::string author;
  switch( m_author ){
  case AuthorUnknown:
    author = "AuthorUnknown";
    break;
  case MooMdtSegmentMakerTool:
    author = "MooMdtSegmentMakerTool";
    break;
  case MooCscSegmentMakerTool:
    author = "MooCscSegmentMakerTool";
    break;
  case Muonboy:
    author = "Muonboy";
    break;
  case DCMathSegmentMaker:
    author = "DCMathSegmentMaker";
    break;
  case MDT_DHoughSegmentMakerTool:
    author = "MDT_DHoughSegmentMakerTool";
    break;
  case CSC_DHoughSegmentMakerTool:
    author = "CSC_DHoughSegmentMakerTool";
    break;
  case Csc2dSegmentMaker:
    author = "Csc2dSegmentMaker";
    break;
  case Csc4dSegmentMaker:
    author = "Csc4dSegmentMaker";
    break;
  case TRT_SegmentMaker:
    author = "TRT_SegmentMaker";
    break;
  default:
    author = "Unrecognised author, enum = " +  std::to_string(m_author);
    break;
  }
  return author;
}
