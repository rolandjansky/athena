/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinUtility.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkDetDescrUtils/BinUtility.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const BinUtility& bgen)
{ return bgen.dump(sl); } 

std::ostream& Trk::operator << ( std::ostream& sl, const BinUtility& bgen)
{ return bgen.dump(sl); } 

