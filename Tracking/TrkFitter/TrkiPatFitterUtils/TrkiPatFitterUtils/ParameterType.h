/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 enumerated type of fitted parameters
 ***************************************************************************/

#ifndef TRKIPATFITTERUTILS_PARAMETERTYPE_H
# define TRKIPATFITTERUTILS_PARAMETERTYPE_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< PUBLIC DEFINES                                                 >>>>>>

class MsgStream;

namespace Trk
{
    enum  ParameterType
    {
	D0,
	Z0,
	Phi0,
	Theta0,
	QOverP0,
	QOverP1,
	ParameterTypes
    };

    MsgStream& operator<< (MsgStream& log, ParameterType type);

}	// end of namespace

#endif	// TRKIPATFITTERUTILS_PARAMETERTYPE_H
