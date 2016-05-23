/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_SETFATALHANDLER_H
#define ATHENASERVICES_SETFATALHANDLER_H
/** @file SetFatalHandler.h
 *  @brief change traceback handling. It is in principle not needed but 
 *  the observation is that in most cases it helps recovering a meaningful 
 *  trace back.
 *
 *  @author Wim Lavrjisen
 *  
 *  $Id: SetFatalHandler.h,v 1.2 2007-06-16 00:55:22 calaf Exp $
 */

/** @class AthenaServices
 *  @brief FIXME should be namespace, but we need to access from python...
 */
class AthenaServices {
public:
/** @fn void AthenaServices::SetFatalHandler( int flags)
 *  @brief change traceback handling. It is in principle not needed but 
 *  the observation is that in most cases it helps recovering a meaningful 
 *  trace back.
 *
 *  @details
 *  see http://cern.ch/seal/workbook/sealbase.html
 *  
 *  @param flags a bit field. For example (from RecExCommon_topOptions.py) 
 * @code
 *  gbl.AthenaServices.SetFatalHandler(438)
 * @endcode
 * sets on the flags marked with an x
 *  @code
 USR1_DUMP_CORE        =   1
 FATAL_ON_QUIT         =   2 x
 FATAL_ON_INT          =   4 x
 FATAL_DUMP_CORE       =   8
 FATAL_DUMP_SIG        =  16 x
 FATAL_DUMP_STACK      =  32 x
 FATAL_DUMP_LIBS       =  64 
 FATAL_DUMP_CONTEXT    = 128 x
 FATAL_AUTO_EXIT       = 256 x
 *  @endcode
 *
 */
   static void SetFatalHandler( int flags = -1 );
};

#endif // !ATHENASERVICES_SETFATALHANDLER_H
