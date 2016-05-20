// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GaudiBackwardsCompatibility.h 724923 2016-02-18 18:48:07Z krasznaa $
#ifndef ATLASCMAKE_GAUDIBACKWARDSCOMPATIBILITY_H
#define ATLASCMAKE_GAUDIBACKWARDSCOMPATIBILITY_H

/// This file is here to provide a way of building the ATLAS offline software
/// against the latest version of Gaudi, until all the offline packages have
/// migrated away from using these macros
///
/// Note that this warning formalism is only supported by GCC >= 4.8, and recent
/// clang versions. But hopefully this file is not going to be here for long.

#define DECLARE_ALGORITHM(x)                                            \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )
#define DECLARE_NAMESPACE_ALGORITHM(n,x)                                \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )

#define DECLARE_AUDITOR(x)                                              \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )
#define DECLARE_NAMESPACE_AUDITOR(n,x)                                  \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )

#define DECLARE_GENERIC_CONVERTER(x)                                    \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )
#define DECLARE_NAMESPACE_GENERIC_CONVERTER(n,x)                        \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )
#define DECLARE_CONVERTER(x)                                            \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )
#define DECLARE_NAMESPACE_CONVERTER(n,x)                                \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )

#define DECLARE_SERVICE(x)                                              \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )
#define DECLARE_NAMESPACE_SERVICE(n,x)                                  \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )

#define DECLARE_ALGTOOL(x)                                              \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )
#define DECLARE_NAMESPACE_ALGTOOL(n,x)                                  \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )
#define DECLARE_TOOL(x)                                                 \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )
#define DECLARE_NAMESPACE_TOOL(n,x)                                     \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" )

#define DECLARE_FACTORY_ENTRIES(x)                                      \
   _Pragma( "GCC warning \"Macro is deprecated. Please remove this line.\"" ) \
   void x##_load()

#endif // ATLASCMAKE_GAUDIBACKWARDSCOMPATIBILITY_H
