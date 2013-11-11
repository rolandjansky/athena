/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file Tau1P3PExtraDetails_p1.cxx
///
/// Implementation of persistent Tau1P3PExtraDetails class
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess <thomas.burgess@cern.ch>
/// @author Lukasz Janyst
///
/// Created by Thomas Burgess on 2008-11-28
///
/// $Id: Tau1P3PExtraDetails_p1.cxx,v 1.4 2009-05-08 14:32:44 tburgess Exp $

#include "tauEventTPCnv/Tau1P3PExtraDetails_p1.h"

// Make sure that members added to this class since the first version
// get initialized.  (Otherwise, when reading an old version, they'll
// be left uninitialized.)
Tau1P3PExtraDetails_p1::Tau1P3PExtraDetails_p1() :
    m_sumPTTracks(-1111.),
    m_tracks(0)
{

}
