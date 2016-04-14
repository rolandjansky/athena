/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DriftCircle.cxx
//   Implementation file for class TRT_DriftCircle
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "InDetPrepRawData/TRT_DriftCircle.h"
// forward declares
#include "GaudiKernel/MsgStream.h"

namespace InDet{


// Constructor with parameters:
TRT_DriftCircle::TRT_DriftCircle( 
	const Identifier &Id, 
	const Amg::Vector2D& driftRadius,
	const std::vector<Identifier>& rdoList,
	const Amg::MatrixX* errDriftRadius,
	const InDetDD::TRT_BaseElement* detEl,
        const unsigned int word
	)
	:
	PrepRawData(Id, driftRadius, rdoList, errDriftRadius), //call base class constructor
	m_detEl(detEl),
        m_word(word)
{
}

TRT_DriftCircle::TRT_DriftCircle( 
	const Identifier &Id, 
	const Amg::Vector2D& driftRadius,
	const Amg::MatrixX* errDriftRadius,
	const InDetDD::TRT_BaseElement* detEl,
        const unsigned int word
	)
	:
	PrepRawData(Id, driftRadius, errDriftRadius), //call base class constructor
	m_detEl(detEl),
        m_word(word)
{
}

TRT_DriftCircle::TRT_DriftCircle( 
	const Identifier &Id, 
	const Amg::Vector2D& driftRadius,
	std::vector<Identifier>&& rdoList,
	std::unique_ptr<const Amg::MatrixX> errDriftRadius,
	const InDetDD::TRT_BaseElement* detEl,
        const unsigned int word
	)
	:
	PrepRawData(Id, driftRadius,
                    std::move(rdoList),
                    std::move(errDriftRadius)), //call base class constructor
	m_detEl(detEl),
        m_word(word)
{
}

// Destructor:
TRT_DriftCircle::~TRT_DriftCircle()
{
	//don't delete detector element as it does not belong to this class.
}


// Default constr
TRT_DriftCircle::TRT_DriftCircle()
	:
	PrepRawData(),
	m_detEl( 0 ),
        m_word( 0)

{ 
}

//Copy constructor:
TRT_DriftCircle::TRT_DriftCircle(const TRT_DriftCircle & RIO)
	:
	PrepRawData( RIO ),
	m_detEl(RIO.m_detEl), 
        m_word( RIO.m_word)

{
}


double TRT_DriftCircle::driftTime(bool& valid) const
{
  /*
    std::cout << " TRT_DriftCircle ERROR - driftTime(valid) is depreciated "
             << std::endl;
    std::cout << "  use  rawDriftTime() and driftTimeValid() instead  " << std::endl;
  */
  valid = driftTimeValid();
  return rawDriftTime();
}


//assignment operator
TRT_DriftCircle& TRT_DriftCircle::operator=(const TRT_DriftCircle& RIO)
{
  if (&RIO !=this) {
    m_word = RIO.m_word;
    *static_cast<Trk::PrepRawData*>(this) = RIO;
  }
	
  return *this;
}


    MsgStream& TRT_DriftCircle::dump( MsgStream&    stream) const
    {
        stream << "TRT_DriftCircle object"<<std::endl;
        stream << "Level (true/false)		 "  << highLevel() << std::endl;
        stream << "Valid (true/false)		 "  << driftTimeValid() << std::endl;
        stream << "timeOverThreshold:               "  << timeOverThreshold()  << std::endl;
        stream << "driftTime:                       "  << rawDriftTime() << std::endl;
        stream << "dataWord:                        "  << m_word       << std::endl;
        stream << "Base class (PrepRawData):" << std::endl;
        this->PrepRawData::dump(stream);

        return stream;
    }

    std::ostream& TRT_DriftCircle::dump( std::ostream&    stream) const
    {
        stream << "TRT_DriftCircle object"<<std::endl;
        stream << "Level (true/false)		 "  << highLevel() << std::endl;
        stream << "Valid (true/false)		 "  << driftTimeValid() << std::endl;
        stream << "timeOverThreshold:               "  << timeOverThreshold()  << std::endl;
        stream << "driftTime:                       "  << rawDriftTime() << std::endl;
        stream <<  "dataWord:                        "  <<m_word  << std::endl;

        stream <<  "Base class (PrepRawData):" << std::endl;
        this->PrepRawData::dump(stream);

        return stream;
    } 

    MsgStream&    operator << (MsgStream& stream,    const TRT_DriftCircle& prd)
    {
        return prd.dump(stream);
    }

    std::ostream& operator << (std::ostream& stream, const TRT_DriftCircle& prd)
    {
        return prd.dump(stream);
    }

}//end of ns

