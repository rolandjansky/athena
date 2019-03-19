/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/Calorimeter/CaloIdentifier/test/test_lar_id.cxx,v 1.25 2007-02-19 15:56:25 fledroit Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h" 
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "GaudiKernel/System.h" 
#include <iostream> 
 
typedef Identifier::size_type  size_type ;

#if 0
static void tab (size_t level) 
{ 
    for (size_t i = 0; i < level; ++i) std::cout << " "; 
} 
#endif
 
void
check_lar_neighbour_timing(IdDictMgr& idd)
{
    
    std::cout << "=====================================>  Begin  check_lar_neighbour_timing " << std::endl;
    std::cout << "=====================================>  Begin  check_lar_neighbour_timing " << std::endl;

    // Check the timing for lar em

    LArEM_ID em_id;

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

    if(em_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_neighbour_timing - cannot init from dict" << std::endl;
    }

    std::cout << "Timing for init from dictionary " << std::endl;
    
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    ///
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    ///
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;


    /// Loop over all channel hashes
    size_type hash_max = em_id.channel_hash_max ();

    std::cout << "Number of channels " <<  hash_max
	      << std::endl;

    int nloops = 300;
    long long n = 0;


    /// Empty loop

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;

    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    float num     = hash_max*nloops;
    float inv_num     = 1. / num;
    std::cout << "Empty loop " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    float user    = (float)deltaUser * inv_num;
    float kernel  = (float)deltaKernel * inv_num;
    float elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier access

    /// Store in object the measured times
    Identifier channel_id;
    IdContext channelContext = em_id.channel_context();
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = em_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = em_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    n += em_id.eta(channel_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash (plus eta) " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier access from vector

    const std::vector<Identifier>& channel_ids = em_id.channel_ids();

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = em_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = em_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = channel_ids[i];
	    n += em_id.eta(channel_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash (plus eta) " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and barrel_ec field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    n += em_id.eta(channel_id) + em_id.barrel_ec(channel_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access barrel_ec field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and sampling field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    n += em_id.eta(channel_id) + em_id.sampling(channel_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access sampling field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

    /// Identifier and region field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    n += em_id.eta(channel_id) + em_id.region(channel_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access region field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and eta field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    n += em_id.phi(channel_id) + em_id.eta(channel_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access eta field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and phi field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    n += em_id.eta(channel_id) + em_id.phi(channel_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access phi field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and region id from channel id

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    Identifier region_id;

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    n += em_id.eta(channel_id) + em_id.region_id(channel_id).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and get region id from channel id " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    

    /// Identifier access and hash access again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    n += em_id.eta(channel_id) + 
		em_id.channel_hash_binary_search(channel_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and get hash back again " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// Identifier access and hash access again calc

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    n += em_id.eta(channel_id) + em_id.channel_hash(channel_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and get hash back again, calc " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    
    /// Identifier access, for region

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    size_type reg_hash_max = em_id.region_hash_max ();
    IdContext regionContext = em_id.region_context();
    
    nloops = 1000;

    float num_reg     = reg_hash_max*100*nloops;
    float inv_num_reg     = 1. / num_reg;

    std::cout << "Number of region " <<  reg_hash_max
	      << std::endl;

    n = 0;
    for (int j = 0; j < 100*nloops; ++j) {
	for (size_type i=0; i<reg_hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, region_id, &regionContext);
	    region_id = em_id.region_id(i);
	    n        += em_id.region(region_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash for region " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num_reg << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num_reg;
    kernel  = (float)deltaKernel * inv_num_reg;
    elapsed = (float)deltaElapsed * inv_num_reg;
    std::cout << "tries, user, kernel, elapsed " 
	      << num_reg << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// Identifier access and hash access again, for region

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < 100*nloops; ++j) {
	for (size_type i=0; i<reg_hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, region_id, &regionContext);
	    region_id = em_id.region_id(i);
	    n        += em_id.region(region_id) + em_id.region_hash(region_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash for region and get hash back again " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num_reg << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num_reg;
    kernel  = (float)deltaKernel * inv_num_reg;
    elapsed = (float)deltaElapsed * inv_num_reg;
    std::cout << "tries, user, kernel, elapsed " 
	      << num_reg << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    



    /// Identifier and channel fields access

    /// Store in object the measured times
    int barrel_ec;
    int sampling;
    int region;
    int eta;
    int phi;
    
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    nloops = 100;
    num     = hash_max*nloops;

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    barrel_ec = em_id.barrel_ec(channel_id);
	    sampling  = em_id.sampling(channel_id);
	    region    = em_id.region(channel_id);
	    eta       = em_id.eta(channel_id);
	    phi       = em_id.phi(channel_id);
	    n         += barrel_ec + sampling + region + eta + phi;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and channel fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// Identifier and channel fields access and channel ID again

    Identifier ch_id;
    
    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    barrel_ec = em_id.barrel_ec(channel_id);
	    sampling  = em_id.sampling(channel_id);
	    region    = em_id.region(channel_id);
	    eta       = em_id.eta(channel_id);
	    phi       = em_id.phi(channel_id);
	    ch_id     = em_id.channel_id(barrel_ec, sampling, region, eta, phi);
	    n        += barrel_ec + sampling + region + eta + phi + ch_id.get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and channel fields and channel ID again " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and channel fields access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    region_id = em_id.region_id(channel_id);
	    eta       = em_id.eta(channel_id);
	    phi       = em_id.phi(channel_id);
	    n         += eta + phi;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID, region id and eta/phi fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// Identifier and channel fields access and channel ID again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		em_id.get_id(i, channel_id, &channelContext);
	    channel_id = em_id.channel_id(i);
	    region_id = em_id.region_id(channel_id);
	    eta       = em_id.eta(channel_id);
	    phi       = em_id.phi(channel_id);
	    n         += eta + phi + em_id.channel_id(region_id, eta, phi).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and eta/phi fields and channel ID again " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    /// Access to neighbours

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    try {
	for (size_type i=0; i<hash_max; ++i) {
	    std::vector<IdentifierHash> neighbourList;
	    em_id.get_neighbours(i, LArNeighbours::all3D, neighbourList);
	}
    }
    
    catch ( LArID_Exception &excpt ) {
	std::cout << "*** LArID_Exception caught: " << excpt.message() 
		  << std::endl
		  << "***   error code: " << excpt.code()   
		  << std::endl;
    }       

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    num     = hash_max;
    std::cout << "Get all neighbours " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser * inv_num;
    kernel  = (float)deltaKernel * inv_num;
    elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    


}

void
check_lar_timing(IdDictMgr& idd)
{
    
    // Check the timing for lar em

    std::cout << "=====================================>  check_lar_timing" << std::endl;
    std::cout << "=====================================>  check_lar_timing" << std::endl;

    LArEM_ID em_id;

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

    if(em_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_timing - cannot init from dict" << std::endl;
    }

    std::cout << "Timing for init from dictionary " << std::endl;
    
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    ///
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    ///
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;

    // regions
    IdContext regionContext = em_id.region_context();
    std::vector<Identifier>::const_iterator itId = em_id.reg_begin();
//    std::vector<Identifier>::const_iterator itIdEnd = em_id.reg_end();
  
//    for(; itId!=itIdEnd;++itId){

    Identifier reg_id = *itId;

    em_id.show(reg_id);

    std::cout << "eta min, max, phi max " <<  em_id.eta_min(reg_id) << " "
	      <<  em_id.eta_max(reg_id) << " "
	      <<  em_id.phi_max(reg_id) << " "
	      << std::endl;
    int eta_min = em_id.eta_min(reg_id);
    int eta_max = em_id.eta_max(reg_id);
    int phiMin = 0;
    int phiMax = em_id.phi_max(reg_id);


    // Switch between doing checks or not
    //em_id.set_do_checks(true);
    
    std::cout << "Do checks " << em_id.do_checks() << std::endl;

    // Switch between initializing neighbours or not
    //    em_id.set_do_neighbours(false);
    
    std::cout << "Do neighbours " << em_id.do_neighbours() << std::endl;


    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    int n=0;
    try {
	
	for (int i=0; i<1000; ++i) {
	    for (int eta=eta_min; eta <= eta_max; ++eta) {
		for (int phi=phiMin; phi <= phiMax; ++phi) {

//  		    eta = 500;
//  		    std::cout << " eta " << eta << std::endl;
		
                    /*Identifier channel_id =*/ em_id.channel_id(reg_id, eta, phi);
		    n++;

		
//  		int bec  = em_id.barrel_ec(reg_id);
//  		int samp = em_id.sampling(reg_id);
//  		int reg  = em_id.region(reg_id);
//  //  		int eta  = em_id.eta(channel_id);
//  //  		int phi  = em_id.phi(channel_id);

//  		std::cout << "bec, samp, reg " << std::dec << bec << " " << samp << " " << reg << std::endl;

      
//  		Identifier chanId = em_id.channel_id (bec, samp, reg, eta, phi);
		
//  		std::cout << std::hex << channel_id.get_compact() << " " 
//  			  << chanId.get_compact() << " " 
//  			  << channel_id.get_compact()  - chanId.get_compact()
//  			  << std::endl;


		}
	    }
	}
    }
    
    catch ( LArID_Exception &excpt ) {
	std::cout << "*** LArID_Exception caught: " << excpt.message() 
		  << std::endl
		  << "***   error code: " << excpt.code()   
		  << std::endl;
    }       

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << n << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;
    float inv_num     = 1. / n;
    float user    = (float)deltaUser * inv_num;
    float kernel  = (float)deltaKernel * inv_num;
    float elapsed = (float)deltaElapsed * inv_num;
    std::cout << "tries, user, kernel, elapsed " 
	      << n << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
}

//------------------------------------------------------------------------------------------------

static void
check_lar_em_decoding(IdDictMgr& idd)
{
    
    std::cout << "=====================================>  check_lar_em_decoding" << std::endl;
    std::cout << "=====================================>  check_lar_em_decoding" << std::endl;

    LArEM_ID em_id;

    bool voisins = em_id.do_neighbours();
    std::cout << "voisins0 = " << voisins << std::endl;
    em_id.set_do_neighbours(false);
    voisins = em_id.do_neighbours();
    std::cout << "voisins1 = " << voisins << std::endl;

    bool error = false;

    std::cout << "calling initialize_from_dictionary " << std::endl;
    if(em_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_em_decoding - cannot init from dict" << std::endl;
	error = true;
    }

    std::cout << "dictionary version = " << em_id.dictionaryVersion() << std::endl;
    
    bool verif = em_id.do_checks();
    std::cout << "verif0 = " << verif << std::endl;
    em_id.set_do_checks(true);
    verif = em_id.do_checks();
    std::cout << "verif1 = " << verif << std::endl;


    // test of hash calculation
    
    // channels
    IdContext channelContext = em_id.channel_context();
    std::vector<Identifier>::const_iterator itId = em_id.em_begin();
    std::vector<Identifier>::const_iterator itIdEnd = em_id.em_end();
  
    int nChan=0;

  
    for(; itId!=itIdEnd;++itId){

	Identifier ch_id = *itId;
	if(em_id.is_lar_em(ch_id)) {
      
	    nChan++;
	    IdentifierHash hashId;
	    int not_ok = em_id.get_hash ( ch_id, hashId, &channelContext);
	    if(not_ok) {
		std::cout <<  "EM: error in get_hash, ch_id = "
			  << em_id.show_to_string(ch_id)
			  << std::endl;
		error = true;
	    }
      
	    Identifier ch_id2;
	    not_ok =  em_id.get_id ( hashId, ch_id2, &channelContext);
	    if(not_ok) {
		std::cout
		    <<  "EM: error in get_id, hashId = "
		    << hashId
		    << std::endl;	
		error = true;
	    }
      
	    if ( ch_id2 != ch_id ) {
		std::cout
		    << " EM: ch_id2 incorrect: " <<  em_id.show_to_string(ch_id2) 
		    << "         should be " <<  em_id.show_to_string(ch_id) 
		    << "           hasId = " << hashId
		    << std::endl;
		error = true;
	    }
      
	    IdentifierHash hashId2 = em_id.channel_hash(ch_id);
	    if(hashId2!=hashId) {
	      error=true;
	    }

	    int bec  = em_id.barrel_ec(ch_id);
	    int samp = em_id.sampling(ch_id);
	    int reg  = em_id.region(ch_id);
	    int eta  = em_id.eta(ch_id);
	    int phi  = em_id.phi(ch_id);
      
	    Identifier chanId ;
	    try {
	      chanId = em_id.channel_id (bec, samp, reg, eta, phi);
	    }
	    catch ( LArID_Exception &excpt ) {
	      std::cout << "*** LArID_Exception caught: " << excpt.message() 
			<< std::endl
			<< "***   error code: " << excpt.code()   
			<< std::endl;
	    }       

	    if ( chanId != ch_id ) {
		std::cout
		    << "EM: chanId incorrect: " <<  em_id.show_to_string(chanId) 
		    << "         should be " <<  em_id.show_to_string(ch_id) 
		    << " input val of bec, samp, reg, eta, phi= " << bec << " " << samp << " " << reg << " " << eta << " " << phi
		    << std::endl;
		error = true;
	    }
      
	    Identifier regId = em_id.region_id (bec, samp, reg);
	    Identifier chanId2 = em_id.channel_id (regId, eta, phi);
	    if ( chanId2 != chanId ) {
		std::cout << "EM: chanId2 incorrect: " <<  em_id.show_to_string(chanId2) 
			  << "          should be " <<  em_id.show_to_string(chanId) 
			  << "          regId     " <<  em_id.show_to_string(regId) 
			  << " input val of bec, samp, reg, eta, phi= " << bec << " " << samp << " " << reg << " " << eta << " " << phi
			  << std::endl;
		error = true;
	    }
	}
    }

    // regions
    itId = em_id.reg_begin();
    itIdEnd = em_id.reg_end();
  
    for(; itId!=itIdEnd;++itId){
	Identifier regId = *itId;
	IdentifierHash regHash = em_id.region_hash(regId);
	std::cout << "EM region, eta0, phi0 = " << em_id.show_to_string(regId) 
		  << " " << em_id.eta0(regHash) 
		  << " " << em_id.phi0(regHash) 
		  << std::endl;
	Identifier regId2 = em_id.region_id(regHash);
	if(regId2 != regId) {
	  std::cout << "EM: regId2 incorrect: " << em_id.show_to_string(regId2)
		    << "         should be    " << em_id.show_to_string(regId)
		    << " hash reg =            " << regHash
		    << std::endl;
	}
	/*
	int phiMin0 = em_id.phi_min(regId);
	int phiMin1 = em_id.phi_min_opt(regId);
	if(phiMin0 != phiMin1) {
	  std::cout << " optimised phi_min giving result different from std phi_min : "
		    << phiMin1 << " != " << phiMin0 << std::endl;
	  error = true;
	}
	*/
    }

    if(!error) {
      std::cout << "lar em decoding tests ok, nChan= " << nChan << std::endl;
    }

    std::cout << "finally check the exception throwing... " << std::endl;
    try {
      /*Identifier wrongRegionId =*/ em_id.region_id (0,99,0); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      /*Identifier wrongChannelId =*/ em_id.channel_id (0,99,0,0,0); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      Identifier goodRegionId = em_id.region_id (1,0,0); 
      /*Identifier wrongChannelId =*/ em_id.channel_id (goodRegionId,0,-99); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    std::cout << "... which is ok " << std::endl;
}

static void
check_lar_hec_decoding(IdDictMgr& idd)
{
    
    std::cout << "=====================================>  check_lar_hec_decoding" << std::endl;
    std::cout << "=====================================>  check_lar_hec_decoding" << std::endl;

    LArHEC_ID hec_id;

    bool voisins = hec_id.do_neighbours();
    std::cout << "voisins0 = " << voisins << std::endl;
    hec_id.set_do_neighbours(false);
    voisins = hec_id.do_neighbours();
    std::cout << "voisins1 = " << voisins << std::endl;

    if(hec_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_hec_decoding - cannot init from dict" << std::endl;
    }

    std::cout << "dictionary version = " << hec_id.dictionaryVersion() << std::endl;
    
    bool verif = hec_id.do_checks();
    std::cout << "verif0 = " << verif << std::endl;
    hec_id.set_do_checks(true);
    verif = hec_id.do_checks();
    std::cout << "verif1 = " << verif << std::endl;


    // test of hash calculation
    
    // channels
    IdContext channelContext = hec_id.channel_context();
    std::vector<Identifier>::const_iterator itId = hec_id.hec_begin();
    std::vector<Identifier>::const_iterator itIdEnd = hec_id.hec_end();
  
    int nChan=0;

    bool error = false;
  
    for(; itId!=itIdEnd;++itId){

	Identifier ch_id = *itId;
	if(hec_id.is_lar_hec(ch_id)) {
      
	    nChan++;
	    IdentifierHash hashId;
	    int not_ok = hec_id.get_hash ( ch_id, hashId, &channelContext);
	    if(not_ok) {
		std::cout <<  "HEC: error in get_hash, ch_id = "
			  << hec_id.show_to_string(ch_id)
			  << std::endl;
		error = true;
	    }
      
	    Identifier ch_id2;
	    not_ok =  hec_id.get_id ( hashId, ch_id2, &channelContext);
	    if(not_ok) {
		std::cout
		    <<  "HEC: error in get_id, hashId = "
		    << hashId
		    << std::endl;	
		error = true;
	    }
      
	    if ( ch_id2 != ch_id ) {
		std::cout
		    << " HEC: ch_id2 incorrect: " <<  hec_id.show_to_string(ch_id2) 
		    << "         should be " <<  hec_id.show_to_string(ch_id) 
		    << "           hasId = " << hashId
		    << std::endl;
		error = true;
	    }
      
	    int bec  = hec_id.pos_neg(ch_id);
	    int samp = hec_id.sampling(ch_id);
	    int reg  = hec_id.region(ch_id);
	    int eta  = hec_id.eta(ch_id);
	    int phi  = hec_id.phi(ch_id);
      
	    Identifier chanId = hec_id.channel_id (bec, samp, reg, eta, phi);
	    if ( chanId != ch_id ) {
		std::cout
		    << "HEC: chanId incorrect: " <<  hec_id.show_to_string(chanId) 
		    << "         should be " <<  hec_id.show_to_string(ch_id) 
		    << " input val of pn, samp, reg, eta, phi= " << bec << " " << samp << " " << reg << " " << eta << " " << phi
		    << std::endl;
		error = true;
	    }
      
	    Identifier regId = hec_id.region_id (bec, samp, reg);
	    Identifier chanId2 = hec_id.channel_id (regId, eta, phi);
	    if ( chanId2 != chanId ) {
		std::cout << "HEC: chanId2 incorrect: " <<  hec_id.show_to_string(chanId2) 
			  << "          should be " <<  hec_id.show_to_string(chanId) 
			  << "          regId     " <<  hec_id.show_to_string(regId) 
			  << " input val of pn, samp, reg, eta, phi= " << bec << " " << samp << " " << reg << " " << eta << " " << phi
			  << std::endl;
		error = true;
	    }
	}
    }
    // regions
    itId = hec_id.reg_begin();
    itIdEnd = hec_id.reg_end();
  
    for(; itId!=itIdEnd;++itId){
	Identifier regId = *itId;
	IdentifierHash regHash = hec_id.region_hash(regId);
	std::cout << "HEC region, eta0, phi0 = " << hec_id.show_to_string(regId) 
		  << " " << hec_id.eta0(regHash) 
		  << " " << hec_id.phi0(regHash) 
		  << std::endl;
	/*
	int phiMin0 = hec_id.phi_min(regId);
	int phiMin1 = hec_id.phi_min_opt(regId);
	if(phiMin0 != phiMin1) {
	  std::cout << " optimised phi_min giving result different from std phi_min : "
		    << phiMin1 << " != " << phiMin0 << std::endl;
	  error = true;
	}
	*/
    }

    if(!error) {
      std::cout << "lar hec decoding tests ok, nChan= " << nChan << std::endl;
    }

    std::cout << "finally check the exception throwing... " << std::endl;
    try {
      /*Identifier wrongRegionId =*/ hec_id.region_id (0,99,0); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      /*Identifier wrongChannelId =*/ hec_id.channel_id (0,99,0,0,0); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      Identifier goodRegionId = hec_id.region_id (2,0,0); 
      /*Identifier wrongChannelId =*/ hec_id.channel_id (goodRegionId,0,-99); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    std::cout << "... which is ok " << std::endl;
}


static void
check_lar_fcal_decoding(IdDictMgr& idd)
{
    
    std::cout << "=====================================>  check_lar_minifcal_decoding" << std::endl;
    std::cout << "=====================================>  check_lar_minifcal_decoding" << std::endl;

    LArMiniFCAL_ID minifcal_id;


    bool voisinsMF = minifcal_id.do_neighbours();
    std::cout << "voisinsMF0 = " << voisinsMF << std::endl;
    minifcal_id.set_do_neighbours(false);
    voisinsMF = minifcal_id.do_neighbours();
    std::cout << "voisinsMF1 = " << voisinsMF << std::endl;

    if(minifcal_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_minifcal_decoding - cannot init from dict" << std::endl;
    }
    std::cout << "dictionary version (MiniFcal) = " << minifcal_id.dictionaryVersion() << std::endl;

    bool verifm = minifcal_id.do_checks();
    std::cout << "verif0 = " << verifm << std::endl;
    minifcal_id.set_do_checks(true);
    verifm = minifcal_id.do_checks();
    std::cout << "verif1 = " << verifm << std::endl;

    IdContext mfchannelContext = minifcal_id.channel_context();
    std::vector<Identifier>::const_iterator itIdMF = minifcal_id.minifcal_begin();
    std::vector<Identifier>::const_iterator itIdEndMF = minifcal_id.minifcal_end();
  
    int nChanMF=0;
    int nChanMF2=0;

    bool errorMF = false;
  
    for(; itIdMF!=itIdEndMF;++itIdMF){

	Identifier ch_id = *itIdMF;
	nChanMF2++;
	if(minifcal_id.is_lar_fcal(ch_id)) {
      
	    nChanMF++;
	    IdentifierHash hashId;
	    int not_ok = minifcal_id.get_hash ( ch_id, hashId, &mfchannelContext);
	    if(not_ok) {
		std::cout <<  "MINIFCAL: errorMF in get_hash, ch_id = "
			  << minifcal_id.show_to_string(ch_id)
			  << std::endl;
		errorMF = true;
	    }
      
	    Identifier ch_id2;
	    not_ok =  minifcal_id.get_id ( hashId, ch_id2, &mfchannelContext);
	    if(not_ok) {
		std::cout
		    <<  "MINIFCAL: errorMF in get_id, hashId = "
		    << hashId
		    << std::endl;	
		errorMF = true;
	    }
      
	    if ( ch_id2 != ch_id ) {
		std::cout
		    << " MINIFCAL: ch_id2 incorrect: " <<  minifcal_id.show_to_string(ch_id2) 
		    << "         should be " <<  minifcal_id.show_to_string(ch_id) 
		    << "           hasId = " << hashId
		    << std::endl;
		errorMF = true;
	    }
      
	    int bec  = minifcal_id.pos_neg(ch_id);
	    int samp = minifcal_id.module(ch_id);
	    int depth = minifcal_id.depth(ch_id);
	    int eta  = minifcal_id.eta(ch_id);
	    int phi  = minifcal_id.phi(ch_id);
      
	    Identifier chanId = minifcal_id.channel_id (bec, samp, depth, eta, phi);
	    if ( chanId != ch_id ) {
		std::cout
		    << "MINIFCAL: chanId incorrect: " <<  minifcal_id.show_to_string(chanId) 
		    << "         should be " <<  minifcal_id.show_to_string(ch_id) 
		    << " input val of pn, mod, depth, eta, phi= " << bec << " " << samp << " " << depth << " " << eta << " " << phi
		    << std::endl;
		errorMF = true;
	    }
      
	    Identifier regId = minifcal_id.module_id (bec, samp);
	    Identifier chanId2 = minifcal_id.channel_id (regId, depth, eta, phi);
	    if ( chanId2 != chanId ) {
		std::cout << "MINIFCAL: chanId2 incorrect: " <<  minifcal_id.show_to_string(chanId2) 
			  << "          should be " <<  minifcal_id.show_to_string(chanId) 
			  << "          modId     " <<  minifcal_id.show_to_string(regId) 
			  << " input val of pn, mod, depth, eta, phi= " << bec << " " << samp << " " << depth << " " << eta << " " << phi
			  << std::endl;
		errorMF = true;
	    }
	}
    }

    // regions
    itIdMF = minifcal_id.mod_begin();
    itIdEndMF = minifcal_id.mod_end();
  
    for(; itIdMF!=itIdEndMF;++itIdMF){
	Identifier modId = *itIdMF;
	IdentifierHash modHash = minifcal_id.module_hash(modId);
	std::cout << "MINIFCAL region, eta0, phi0 = " << minifcal_id.show_to_string(modId) 
		  << " " << minifcal_id.eta0(modHash) 
		  << " " << minifcal_id.phi0(modHash) 
		  << std::endl;
	
    }

    if(!errorMF) {
      std::cout << "lar minifcal decoding tests ok, nChanMF= " << nChanMF << "    and really all: " <<  nChanMF2 << std::endl;
    }
    std::cout << "finally check the exception throwing... " << std::endl;
    try {
      /*Identifier wrongModuleId =*/ minifcal_id.module_id (0,99); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      /*Identifier wrongChannelId =*/ minifcal_id.channel_id (0,99,0,0,0); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      Identifier goodModuleId = minifcal_id.module_id (2,0); 
      /*Identifier wrongChannelId =*/ minifcal_id.channel_id (goodModuleId,0, 99, 0); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    std::cout << "... which is ok  ...? " << std::endl;


    std::cout << "=====================================>  check_lar_fcal_decoding" << std::endl;
    std::cout << "=====================================>  check_lar_fcal_decoding" << std::endl;

    LArFCAL_ID fcal_id;

    bool voisins = fcal_id.do_neighbours();
    std::cout << "voisins0 = " << voisins << std::endl;
    fcal_id.set_do_neighbours(false);
    voisins = fcal_id.do_neighbours();
    std::cout << "voisins1 = " << voisins << std::endl;

    if(fcal_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_fcal_decoding - cannot init from dict" << std::endl;
    }

    std::cout << "dictionary version = " << fcal_id.dictionaryVersion() << std::endl;
    
    bool verif = fcal_id.do_checks();
    std::cout << "verif0 = " << verif << std::endl;
    fcal_id.set_do_checks(true);
    verif = fcal_id.do_checks();
    std::cout << "verif1 = " << verif << std::endl;


    // test of hash calculation
    
    // channels
    IdContext channelContext = fcal_id.channel_context();
    std::vector<Identifier>::const_iterator itId = fcal_id.fcal_begin();
    std::vector<Identifier>::const_iterator itIdEnd = fcal_id.fcal_end();
  
    int nChan=0;
    int nChan2=0;

    bool error = false;
  
    for(; itId!=itIdEnd;++itId){

	Identifier ch_id = *itId;
	nChan2++;
	if(fcal_id.is_lar_fcal(ch_id)) {
      
	    nChan++;
	    IdentifierHash hashId;
	    int not_ok = fcal_id.get_hash ( ch_id, hashId, &channelContext);
	    if(not_ok) {
		std::cout <<  "FCAL: error in get_hash, ch_id = "
			  << fcal_id.show_to_string(ch_id)
			  << std::endl;
		error = true;
	    }
      
	    Identifier ch_id2;
	    not_ok =  fcal_id.get_id ( hashId, ch_id2, &channelContext);
	    if(not_ok) {
		std::cout
		    <<  "FCAL: error in get_id, hashId = "
		    << hashId
		    << std::endl;	
		error = true;
	    }
      
	    if ( ch_id2 != ch_id ) {
		std::cout
		    << " FCAL: ch_id2 incorrect: " <<  fcal_id.show_to_string(ch_id2) 
		    << "         should be " <<  fcal_id.show_to_string(ch_id) 
		    << "           hasId = " << hashId
		    << std::endl;
		error = true;
	    }
      
	    int bec  = fcal_id.pos_neg(ch_id);
	    int samp = fcal_id.module(ch_id);
	    int eta  = fcal_id.eta(ch_id);
	    int phi  = fcal_id.phi(ch_id);
      
	    Identifier chanId = fcal_id.channel_id (bec, samp, eta, phi);
	    if ( chanId != ch_id ) {
		std::cout
		    << "FCAL: chanId incorrect: " <<  fcal_id.show_to_string(chanId) 
		    << "         should be " <<  fcal_id.show_to_string(ch_id) 
		    << " input val of pn, mod, eta, phi= " << bec << " " << samp << " " << eta << " " << phi
		    << std::endl;
		error = true;
	    }
      
	    Identifier regId = fcal_id.module_id (bec, samp);
	    Identifier chanId2 = fcal_id.channel_id (regId, eta, phi);
	    if ( chanId2 != chanId ) {
		std::cout << "FCAL: chanId2 incorrect: " <<  fcal_id.show_to_string(chanId2) 
			  << "          should be " <<  fcal_id.show_to_string(chanId) 
			  << "          modId     " <<  fcal_id.show_to_string(regId) 
			  << " input val of pn, mod, eta, phi= " << bec << " " << samp << " " << eta << " " << phi
			  << std::endl;
		error = true;
	    }
	}
    }
    // regions
    itId = fcal_id.mod_begin();
    itIdEnd = fcal_id.mod_end();
  
    for(; itId!=itIdEnd;++itId){
	Identifier modId = *itId;
	IdentifierHash modHash = fcal_id.module_hash(modId);
	std::cout << "FCAL region, eta0, phi0 = " << fcal_id.show_to_string(modId) 
		  << " " << fcal_id.eta0(modHash) 
		  << " " << fcal_id.phi0(modHash) 
		  << std::endl;
	/*
	int phiMin0 = fcal_id.phi_min(modId);
	int phiMin1 = fcal_id.phi_min_opt(modId);
	if(phiMin0 != phiMin1) {
	  std::cout << " optimised phi_min giving result different from std phi_min : "
		    << phiMin1 << " != " << phiMin0 << std::endl;
	  error = true;
	}
	*/
    }

    if(!error) {
      std::cout << "lar fcal decoding tests ok, nChan= " << nChan << "    and really all: " <<  nChan2 << std::endl;
    }
    std::cout << "finally check the exception throwing... " << std::endl;
    try {
      /*Identifier wrongModuleId =*/ fcal_id.module_id (0,99); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      /*Identifier wrongChannelId =*/ fcal_id.channel_id (0,99,0,0); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    try {
      Identifier goodModuleId = fcal_id.module_id (2,1); 
      /*Identifier wrongChannelId =*/ fcal_id.channel_id (goodModuleId,0,-99); 
    }
    catch(LArID_Exception & except){
      std::cout << (std::string) except << std::endl ;
    }
    std::cout << "... which is ok " << std::endl;
}


static void check_lar_neighbour(IdDictMgr& idd)
{
    
    std::cout << "=====================================>  check_lar_neighbour" << std::endl;
    std::cout << "=====================================>  check_lar_neighbour" << std::endl;

    LArEM_ID em_id;
  
    bool voisins = em_id.do_neighbours();
    std::cout << "voisins0 = " << voisins << std::endl;
    em_id.set_do_neighbours(true);
    voisins = em_id.do_neighbours();
    std::cout << "voisins1 = " << voisins << std::endl;

    if(em_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_neighbour - cannot init from dict" << std::endl;
    }

    std::vector<IdentifierHash> neighbourList;
    IdentifierHash hash_min = 999999 ;
    IdentifierHash hash_max = 0 ;
    for (unsigned int iCell = 0 ; iCell < em_id.channel_hash_max(); ++iCell){
      /*Identifier cellId =*/ em_id.channel_id(iCell);

      em_id.get_neighbours(iCell, LArNeighbours::all3D, neighbourList);

      for (IdentifierHash neighbourHash : neighbourList) {
	if(neighbourHash < hash_min ) {
	  hash_min = neighbourHash;
	}
	if(neighbourHash > hash_max) {
	  hash_max = neighbourHash;
	}

      }
    }

    std::cout << " = = = = = = = = = = = "  << std::endl;
    std::cout << "lar_neighbour tests: " 
	      << " min em neighbour index = " << hash_min
	      << " max em neighbour index = " << hash_max
	      << " channel_hash_max = " << em_id.channel_hash_max()
	      << std::endl;
    std::cout << " = = = = = = = = = = = "  << std::endl;

    LArHEC_ID hec_id;
  
    voisins = hec_id.do_neighbours();
    std::cout << "voisins0 = " << voisins << std::endl;
    hec_id.set_do_neighbours(true);
    voisins = hec_id.do_neighbours();
    std::cout << "voisins1 = " << voisins << std::endl;

    if(hec_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_neighbour - cannot init from dict" << std::endl;
    }

    std::vector<IdentifierHash> neighbourList2;
    IdentifierHash hash_min2 = 999999 ;
    IdentifierHash hash_max2 = 0 ;
    for (unsigned int iCell = 0 ; iCell < hec_id.channel_hash_max(); ++iCell){
      /*Identifier cellId =*/ hec_id.channel_id(iCell);

      hec_id.get_neighbours(iCell, LArNeighbours::all3D, neighbourList2);

      for (IdentifierHash neighbourHash : neighbourList2) {
	if(neighbourHash < hash_min2 ) {
	  hash_min2 = neighbourHash;
	}
	if(neighbourHash > hash_max2) {
	  hash_max2 = neighbourHash;
	}

      }
    }

    std::cout << " = = = = = = = = = = = "  << std::endl;
    std::cout << "lar_neighbour tests: " 
	      << " min hec neighbour index = " << hash_min2
	      << " max hec neighbour index = " << hash_max2
	      << " channel_hash_max = " << hec_id.channel_hash_max()
	      << std::endl;
    std::cout << " = = = = = = = = = = = "  << std::endl;

    LArFCAL_ID fcal_id;
  
    voisins = fcal_id.do_neighbours();
    std::cout << "voisins0 = " << voisins << std::endl;
    fcal_id.set_do_neighbours(true);
    voisins = fcal_id.do_neighbours();
    std::cout << "voisins1 = " << voisins << std::endl;

    if(fcal_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_neighbour - cannot init from dict" << std::endl;
    }

    std::vector<IdentifierHash> neighbourList3;
    IdentifierHash hash_min3 = 999999 ;
    IdentifierHash hash_max3 = 0 ;
    for (unsigned int iCell = 0 ; iCell < fcal_id.channel_hash_max(); ++iCell){
      /*Identifier cellId =*/ fcal_id.channel_id(iCell);

      fcal_id.get_neighbours(iCell, LArNeighbours::all3D, neighbourList3);

      for (IdentifierHash neighbourHash : neighbourList3) {
	if(neighbourHash < hash_min3 ) {
	  hash_min3 = neighbourHash;
	}
	if(neighbourHash > hash_max3) {
	  hash_max3 = neighbourHash;
	}

      }
    }

    std::cout << " = = = = = = = = = = = "  << std::endl;
    std::cout << "lar_neighbour tests: " 
	      << " min fcal neighbour index = " << hash_min3
	      << " max fcal neighbour index = " << hash_max3
	      << " channel_hash_max = " << fcal_id.channel_hash_max()
	      << std::endl;
    std::cout << " = = = = = = = = = = = "  << std::endl;
}


int main (int argc, char* argv[])  
{  
    if (argc < 2) return (1);  
  

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );



    IdDictParser parser;  

    //<<<<<<< test_lar_id.cxx
    // standard dict:
    //std::string lArIDFileName = "IdDictLArCalorimeter_DC3-05-Comm-01.xml";
    // For sLHC:
    // std::string lArIDFileName = "IdDictLArCalorimeter_sLHC-MiniFcal-00.xml";
    //    std::string lArIDFileName = "IdDictLArCalorimeter_H8_2004.xml";
    //    std::string lArIDFileName = "IdDictLArCalorimeter_H6_2004.xml";
    // parser.register_external_entity("LArCalorimeter", lArIDFileName); 
    std::string caloIDFileName = "IdDictCalorimeter_L1Onl.xml";  
    parser.register_external_entity("Calorimeter", caloIDFileName); 
    IdDictMgr& idd = parser.parse (argv[1]);  
//    IdDictMgr& idd = parser.parse (argv[1], "initial_layout");  


    // Set some default file names for neighbours (RDS 12/2009):
    idd.add_metadata("FULLATLASNEIGHBORS",  "SuperCaloNeighbours-DC3-05-Comm-01.dat");  
    idd.add_metadata("FCAL2DNEIGHBORS",     "FCal2DNeighbors-DC3-05-Comm-01.txt");  
    idd.add_metadata("FCAL3DNEIGHBORSNEXT", "FCal3DNeighborsNext-DC3-05-Comm-01.txt");  
    idd.add_metadata("FCAL3DNEIGHBORSPREV", "FCal3DNeighborsPrev-DC3-05-Comm-01.txt");  
    idd.add_metadata("TILENEIGHBORS",       "TileNeighbour_reduced.txt");  

    std::cout << "Timing for parsing dictionaries " << std::endl;
    
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    ///
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    ///
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;




    std::cout << "got dict mgr " << std::endl;
//      std::cout << "regenerate for tag = initial_layout " << std::endl;
//      idd.generate_implementation ("initial_layout");  

    check_lar_em_decoding(idd);

    check_lar_hec_decoding(idd);

    check_lar_fcal_decoding(idd);

    check_lar_neighbour(idd);

    //    check_lar_timing(idd);

    //    check_lar_neighbour_timing(idd);
    
    return 0;  
}  
  
 
 
 
 
 
