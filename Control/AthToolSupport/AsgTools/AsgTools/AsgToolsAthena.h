// Dear emacs, this is -*- c++ -*-
// $Id: AsgToolsAthena.h 615228 2014-09-05 10:19:06Z krasznaa $
/// @file AsgTools/AsgToolsAthena.h
/// @short File making it clear to checkreq.py that the requirements file is OK
///
/// The source code in this package uses a lot of conditional compilation,
/// which just throws checkreq.py off. So it prints warnings where it shouldn't.
/// This header should never actually be included in any code, it's just here
/// to make it clear to checkreq.py that some of the Athena packages do get
/// used publicly by the code.
///
/// $Revision: 615228 $
/// $Date: 2014-09-05 12:19:06 +0200 (Fri, 05 Sep 2014) $
///
#ifndef ASGTOOLS_ASGTOOLSATHENA_H
#define ASGTOOLS_ASGTOOLSATHENA_H

// Make sure that nobody actually tries to use this header:
#warning "This header is for technical purposes, one shouldn't include it"

// Include headers from all the publicly used Athena packages:
#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "SGTools/CLASS_DEF.h"

#endif // ASGTOOLS_ASGTOOLSATHENA_H
