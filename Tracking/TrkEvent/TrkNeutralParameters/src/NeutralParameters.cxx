/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParameters/NeutralParameters.h"

namespace Trk {

 //explicit instantiation
 template class ParametersBase<5,Neutral>;

 template class ParametersT<5,Neutral,PlaneSurface>;
 template class ParametersT<5,Neutral,CylinderSurface>;
 template class ParametersT<5,Neutral,DiscSurface>;
 template class ParametersT<5,Neutral,ConeSurface>;
 template class ParametersT<5,Neutral,PerigeeSurface>;
 template class ParametersT<5,Neutral,StraightLineSurface>;
 template class CurvilinearParametersT<5,Neutral,PlaneSurface>;

}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const Trk::NeutralParameters& pars)
{ return pars.dump(sl); }

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Trk::NeutralParameters& pars)
{ return pars.dump(sl); }
