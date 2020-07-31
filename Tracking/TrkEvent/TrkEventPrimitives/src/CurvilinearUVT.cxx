/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CurvilinearUVT.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkEventPrimitives/CurvilinearUVT.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::CurvilinearUVT& uvt)
{ 
    sl <<  "Trk::CuvilinearUVT - curvilinear frame (u,v,t)" << std::endl;
    sl << "          u = " << uvt.curvU() << std::endl;
    sl << "          v = " << uvt.curvV() << std::endl;
    sl << "          t = " << uvt.curvT() << std::endl;
    
    return sl;   
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::CurvilinearUVT& uvt)
{ 
    sl <<  "Trk::CuvilinearUVT - curvilinear frame (u,v,t)" << std::endl;
    sl << "          u = " << uvt.curvU() << std::endl;
    sl << "          v = " << uvt.curvV() << std::endl;
    sl << "          t = " << uvt.curvT() << std::endl;    

    return sl;
}   

