/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETEvent/MissingETComposition.h"

MissingETComposition::MissingETComposition() : Navigable<INavigable4MomentumCollection,MissingETHandlers::Weight>() {}

MissingETComposition::~MissingETComposition() {}

namespace MissingETHandlers {


Weight operator*(const Weight& w1, const Weight& w2)
{
  return Weight(w1.wpx()*w2.wpx(),w1.wpy()*w2.wpy(),w1.wet()*w2.wet());
}

Weight operator*(const Weight& w, double scale)
{
  return Weight(scale*w.wpx(),scale*w.wpy(),scale*w.wet());
}

Weight operator*(double scale, const Weight& w)
{
  return Weight(scale*w.wpx(),scale*w.wpy(),scale*w.wet());
}

Weight operator/(const Weight& w1, const Weight& w2)
{
  return Weight(w1.wpx()/w2.wpx(),w1.wpy()/w2.wpy(),w1.wet()/w2.wet()); 
}

Weight operator+(const Weight& w1, const Weight& w2)
{
  return Weight(w1.wpx()+w2.wpx(),w1.wpy()+w2.wpy(),w1.wet()+w2.wet());
}

Weight operator-(const Weight& w1, const Weight& w2)
{
  return Weight(w1.wpx()-w2.wpx(),w1.wpy()-w2.wpy(),w1.wet()-w2.wet());
}


} // namespace MissingETHandlers
