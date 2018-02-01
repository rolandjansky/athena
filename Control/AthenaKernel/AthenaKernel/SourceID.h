// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaKernel/SourceID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2018
 * @brief Type used to identify a metadata source.
 */


#ifndef ATHENAKERNEL_SOURCEID_H
#define ATHENAKERNEL_SOURCEID_H


namespace SG {


// Type used to identify a metadata source.
// It currently is a string representation of the GUID of the input file.
typedef std::string SourceID;


}

#endif // not ATHENAKERNEL_SOURCEID_H
