// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: errorcheck.h 299732 2014-03-27 17:41:34Z krasznaa $
#ifndef CPTOOLTESTS_ERRORCHECK_H
#define CPTOOLTESTS_ERRORCHECK_H

#define CHECK( ARG )                                     \
        do {                                                  \
            const bool result = ARG;                           \
            if( ! result ) {                                   \
                ::Error( APP_NAME, "Failed to execute: \"%s\"", \
                        #ARG );                                \
                        return 1;                                       \
            }                                                  \
        } while( false )

#endif // CPTOOLTESTS_ERRORCHECK_H
