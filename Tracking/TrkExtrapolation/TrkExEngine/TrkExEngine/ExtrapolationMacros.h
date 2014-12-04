/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IExtrapolationMacros.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef EXSCREENOUTPUTDEFS
#define EXSCREENOUTPUTDEFS 1
#define EX_MSG_INFO(nav, x)    ATH_MSG_INFO(m_sopPrefix << std::setw(4) << nav << m_sopPostfix << x)
#define EX_MSG_DEBUG(nav, x)   ATH_MSG_DEBUG(m_sopPrefix << std::setw(4) << nav << m_sopPostfix << x)
#define EX_MSG_VERBOSE(nav, x) ATH_MSG_VERBOSE(m_sopPrefix << std::setw(4) << nav << m_sopPostfix << x)
#define EX_MSG_WARNING(nav, x) ATH_MSG_WARNING(m_sopPrefix << std::setw(4) << nav << m_sopPostfix << x)
#define EX_MSG_FATAL(nav, x)   ATH_MSG_FATAL(m_sopPrefix << std::setw(4) << nav << m_sopPostfix << x)
#endif

#ifndef TRKEXENINGE_OUTPUTHELPER 
#define TRKEXENINGE_OUTPUTHELPER 
#define OH_CHECKFOUND(object) ( object ? "found" : "not found")
#endif

#ifndef TRKEXENGINE_EXCODECHECKS
#define TRKEXENGINE_EXCODECHECKS
#define CHECK_ECODE_CONTINUE(ecell, ecode) if (!ecode.inProgress()) { EX_MSG_VERBOSE(ecell.navigationStep, ecode.toString() << " triggers return."); return ecode; }
#define CHECK_ECODE_SUCCESS(ecell, ecode) if (ecode.isSuccess()) { EX_MSG_VERBOSE(ecell.navigationStep, ecode.toString() << " stops extrapolation sequence."); return ecode; }
#endif

