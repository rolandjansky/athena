/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILE_AsgTools_AsgToolsConf_h
#define FILE_AsgTools_AsgToolsConf_h

// Configuration file for the ASG tool environment.

// Flags:
//   ASGTOOL_STANDALONE - Flag indicating that the base standalone ASG tool
//     environment is to be provided.
//   ASGTOOL_ATHENA - Flag indicating that the Athena ASG tool environment
//     is to be provided.
//   ASGTOOL_TEVENT - Flag indicating that TEvent is available in standalone.
//     If this is not set, calls to the event store will fail. This flag is
//     se unless the user sets ASGTOOL_NOTEVENT.

#if !defined(ASGTOOL_STANDALONE) && !defined(ASGTOOL_ATHENA)
#ifdef XAOD_STANDALONE
#define ASGTOOL_STANDALONE
#ifndef ASGTOOL_NOTEVENT
#define ASGTOOL_TEVENT
#endif
#else
#define ASGTOOL_ATHENA
#endif
#endif

#if defined(ASGTOOL_STANDALONE) && defined(ASGTOOL_ATHENA)
#error "Only one of ASGTOOL_STANDALONE and  ASGTOOL_ATHENA may be defined."
#endif

#endif
