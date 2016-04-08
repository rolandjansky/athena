// Dear emacs, this is -*- c++ -*-
// $Id: AsgToolsDict.h 649187 2015-02-24 08:46:57Z krasznaa $
#ifndef ASGTOOLS_ASGTOOLSDICT_H
#define ASGTOOLS_ASGTOOLSDICT_H

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

// Local include(s):
#include "AsgTools/IAsgTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AsgMessaging.h"
#include "AsgTools/ToolStore.h"

// The following is only needed for standalone usage. In Athena the
// setProperty(...) function(s) come(s) from the AlgTool base class, with all
// the necessary dictionaries declared in GaudiKernel.
#ifdef ASGTOOL_STANDALONE

// Helper macro for declaring the setProperty functions to the dictionary:
#define SETPROPERTY_INSTAN( TYPE )                                            \
   template StatusCode asg::AsgTool::setProperty< TYPE >( const std::string&, \
                                                          const TYPE& )

// Declare all possible setProperty template instantiations to Reflex:
SETPROPERTY_INSTAN( bool );
SETPROPERTY_INSTAN( int );
SETPROPERTY_INSTAN( float );
SETPROPERTY_INSTAN( double );
SETPROPERTY_INSTAN( std::string );
SETPROPERTY_INSTAN( std::vector< int > );
SETPROPERTY_INSTAN( std::vector< float > );
SETPROPERTY_INSTAN( std::vector< std::string > );

// Make the compiler forget about this macro now...
#undef SETPROPERTY_INSTAN

#endif // ASGTOOL_STANDALONE
#endif // not ASGTOOLS_ASGTOOLSDICT_H
