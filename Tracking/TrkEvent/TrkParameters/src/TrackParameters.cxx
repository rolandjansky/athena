/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkParameters/TrackParameters.h"

namespace Trk {

 //explicit intantiation
 template class ParametersBase<5,Charged>;

 template class ParametersT<5,Charged,PlaneSurface>;
 template class ParametersT<5,Charged,CylinderSurface>;
 template class ParametersT<5,Charged,DiscSurface>;
 template class ParametersT<5,Charged,ConeSurface>;
 template class ParametersT<5,Charged,PerigeeSurface>;
 template class ParametersT<5,Charged,StraightLineSurface>;
 template class CurvilinearParametersT<5,Charged,PlaneSurface>;

}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const Trk::TrackParameters& pars)
{ return pars.dump(sl); }

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Trk::TrackParameters& pars)
{ return pars.dump(sl); }

