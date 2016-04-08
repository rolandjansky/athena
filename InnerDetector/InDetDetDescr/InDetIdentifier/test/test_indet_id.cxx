/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/InnerDetector/InDetDetDescr/InDetIdentifier/test/test_indet_id.cxx,v 1.11.2.1 2009-03-26 21:54:32 jtseng Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h" 
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SiliconID.h"
#include "GaudiKernel/System.h" 
#include <iostream> 
 
typedef Identifier::size_type  size_type ;

// static void tab (size_t level) 
// { 
//     for (size_t i = 0; i < level; ++i) std::cout << " "; 
// } 
 
//  static ExpandedIdentifier
//  pixel_id ( int barrel_ec,  
//  	   int layer_disk, 
//  	   int phi_module, 
//  	   int eta_module,      
//  	   int phi_index,
//  	   int eta_index) 
//  {
//      // Build identifier
//      ExpandedIdentifier result;
//      result << 1 << barrel_ec << layer_disk << phi_module << eta_module;
//      result << phi_index << eta_index;

//      assert (result.last_error () == ExpandedIdentifier::none);

//      return result;
//}


static void
check_pixel_timing(IdDictMgr& idd)
{
    
    std::cout << "=========>  Begin  check_pixel_timing " << std::endl;
    std::cout << "=========>  Begin  check_pixel_timing " << std::endl;

    // Check the timing for pixel 

    PixelID pixel_id;

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

    if(pixel_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_neighbour_timing - cannot init from dict" << std::endl;
    }

    std::cout << "Timing for init from dictionary " << std::endl;
    
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;


    /// Loop over all channel hashes
    size_type hash_max = pixel_id.wafer_hash_max ();

    std::cout << "Number of channels " <<  hash_max
	      << std::endl;

    int nloops = 1000000;
    float num     = hash_max*nloops;
    Identifier::value_type n = 0;


    /// Empty loop

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;

    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    n += i;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Empty loop " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    float user    = (float)deltaUser/num;
    float kernel  = (float)deltaKernel/num;
    float elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

    nloops = 30000;
    num    = hash_max*nloops;

    
    /// Wafer Identifier access

    /// Store in object the measured times
    Identifier wafer_id;
    IdContext channelContext = pixel_id.wafer_context();
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = pixel_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = pixel_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    n += pixel_id.wafer_id(i).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
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
	    wafer_id = pixel_id.wafer_id(i).get_compact();
	    n += pixel_id.barrel_ec(wafer_id);
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
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and layer_disk field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		pixel_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = pixel_id.wafer_id(i).get_compact();
	    n += pixel_id.layer_disk(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access layer_disk field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

    /// Identifier and phi_module field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id = pixel_id.wafer_id(i).get_compact();
	    n += pixel_id.phi_module(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access phi_module field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and eta_module field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id = pixel_id.wafer_id(i).get_compact();
	    n += pixel_id.eta_module(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access eta_module field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 3000;
    num    = hash_max*nloops;

    /// Identifier access and hash access again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		pixel_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = pixel_id.wafer_id(i).get_compact();
	    n += pixel_id.wafer_hash(wafer_id);
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
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

    nloops = 30000;

    /// Identifier and wafer fields access

    /// Store in object the measured times
    int barrel_ec;
    int layer_disk;
    int phi_module;
    int eta_module;
    
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    nloops = 3000;
    num     = hash_max*nloops;

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id   = pixel_id.wafer_id(i);
	    barrel_ec  = pixel_id.barrel_ec(wafer_id);
	    layer_disk = pixel_id.layer_disk(wafer_id);
	    phi_module = pixel_id.phi_module(wafer_id);
	    eta_module = pixel_id.eta_module(wafer_id);
	    n         += barrel_ec + layer_disk + eta_module + phi_module;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and wafer fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// Identifier and wafer fields access and wafer ID again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		pixel_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = pixel_id.wafer_id(i);
	    barrel_ec  = pixel_id.barrel_ec(wafer_id);
	    layer_disk = pixel_id.layer_disk(wafer_id);
	    phi_module = pixel_id.phi_module(wafer_id);
	    eta_module = pixel_id.eta_module(wafer_id);
	    n         += barrel_ec + layer_disk + eta_module + phi_module +
              pixel_id.wafer_id(barrel_ec, layer_disk, phi_module, eta_module).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and wafer fields and wafer ID again " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    



    // Save pixel ids

    nloops = 100000;

    unsigned int npixids = 20000;
    num     = npixids*nloops;
    Identifier pixid;
//    Identifier pixid1;

    std::vector<Identifier> pixids(npixids);
    //Identifier pixids[20000];

    PixelID::const_expanded_id_iterator	first_pixel = pixel_id.pixel_begin();  
    PixelID::const_expanded_id_iterator	last_pixel  = pixel_id.pixel_end();
    unsigned int nids = 0;
    for (; first_pixel != last_pixel && nids < npixids; ++first_pixel, ++nids) {
	const ExpandedIdentifier& exp_id = *first_pixel;
	pixids[nids] =  pixel_id.pixel_id (exp_id[2],
					   exp_id[3], 
					   exp_id[4],
					   exp_id[5],
					   exp_id[6],
					   exp_id[7]);
    }


    nloops = 10000;
    num    = npixids*nloops;

    /// pixel Identifier access - empty loop

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = pixel_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = pixel_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<npixids; ++i) {
	    ++n;
	    n += i;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get pixel ID empty loop " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;


    /// Get pixel id

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = pixel_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = pixel_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<npixids; ++i) {
	    ++n;
	    pixid = pixids[i];
	    n += pixid.get_compact();
//  	    pixid1 = pixids[npixids - i + 1];
//  	    n += pixid1;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get pixel ID from vector " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;

    int phi_index;
    int eta_index;

    nloops = 1000;
    num    = npixids*nloops;

    
    /// pixel Identifier field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<npixids; ++i) {
	    ++n;
	    pixid = pixids[i];
	    barrel_ec  = pixel_id.barrel_ec(pixid);
	    layer_disk = pixel_id.layer_disk(pixid);
	    phi_module = pixel_id.phi_module(pixid);
	    eta_module = pixel_id.eta_module(pixid);
	    phi_index  = pixel_id.phi_index(pixid);
	    eta_index  = pixel_id.eta_index(pixid);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + 
              eta_index + phi_index + pixid.get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get pixel ID fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 300;
    num    = npixids*nloops;

    /// pixel Identifier field access and id back

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<npixids; ++i) {
	    ++n;
	    pixid = pixids[i];
	    barrel_ec  = pixel_id.barrel_ec(pixid);
	    layer_disk = pixel_id.layer_disk(pixid);
	    phi_module = pixel_id.phi_module(pixid);
	    eta_module = pixel_id.eta_module(pixid);
	    phi_index  = pixel_id.phi_index(pixid);
	    eta_index  = pixel_id.eta_index(pixid);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + 
              eta_index + phi_index + pixid.get_compact() +
              pixel_id.pixel_id(barrel_ec, 
                                layer_disk,
                                phi_module,
                                eta_module,
                                phi_index,
                                eta_index).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get pixel ID fields and pixel id back " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    nloops = 1000;
    num    = npixids*nloops;

    /// pixel Identifier get wafer id

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<npixids; ++i) {
	    ++n;
	    pixid = pixids[i];
	    n         +=  pixid.get_compact() + pixel_id.wafer_id(pixid).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get pixel ID then wafer id " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// pixel Identifier get wafer id and eta/phi index

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<npixids; ++i) {
	    ++n;
	    pixid = pixids[i];
	    phi_index  = pixel_id.phi_index(pixid);
	    eta_index  = pixel_id.eta_index(pixid);
	    n         += eta_index + phi_index + pixid.get_compact() +
              pixel_id.wafer_id(pixid).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get pixel, wafer id and and eta/phi index" << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// pixel Identifier get wafer id and eta/phi index and pix id again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<npixids; ++i) {
	    ++n;
	    pixid = pixids[i];
	    phi_index  = pixel_id.phi_index(pixid);
	    eta_index  = pixel_id.eta_index(pixid);
	    wafer_id   = pixel_id.wafer_id(pixid);
	    n         += eta_index + phi_index + pixid.get_compact() + wafer_id.get_compact() +
              pixel_id.pixel_id(wafer_id, eta_index, phi_index).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get pixel, wafer id and and eta/phi index and pixid again" << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

}

static void
check_sct_timing(IdDictMgr& idd)
{
    
    std::cout << "=========>  Begin  check_sct_timing " << std::endl;
    std::cout << "=========>  Begin  check_sct_timing " << std::endl;

    // Check the timing for sct 

    SCT_ID sct_id;

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

    if(sct_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_neighbour_timing - cannot init from dict" << std::endl;
    }

    std::cout << "Timing for init from dictionary " << std::endl;
    
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;


    /// Loop over all channel hashes
    size_type hash_max = sct_id.wafer_hash_max ();

    std::cout << "Number of channels " <<  hash_max
	      << std::endl;

    int nloops = 10000;
    float num  = hash_max*nloops;
    Identifier::value_type n = 0;


    /// Empty loop

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;

    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    n += i;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Empty loop " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    float user    = (float)deltaUser/num;
    float kernel  = (float)deltaKernel/num;
    float elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Wafer Identifier access

    /// Store in object the measured times
    Identifier wafer_id;
    IdContext channelContext = sct_id.wafer_context();
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = sct_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = sct_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    n += sct_id.wafer_id(i).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
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
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.barrel_ec(wafer_id);
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
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and layer_disk field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		sct_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.layer_disk(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access layer_disk field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

    /// Identifier and phi_module field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		sct_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.phi_module(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access phi_module field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and eta_module field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.eta_module(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access eta_module field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and side field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.side(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access side field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 1000;
    num  = hash_max*nloops;

    /// Identifier access and hash access again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		sct_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.wafer_hash(wafer_id);
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
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

    nloops = 3000;
    num    = hash_max*nloops;

    /// Identifier and wafer fields access

    /// Store in object the measured times
    int barrel_ec;
    int layer_disk;
    int phi_module;
    int eta_module;
    int side;
    
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id   = sct_id.wafer_id(i);
	    barrel_ec  = sct_id.barrel_ec(wafer_id);
	    layer_disk = sct_id.layer_disk(wafer_id);
	    phi_module = sct_id.phi_module(wafer_id);
	    eta_module = sct_id.eta_module(wafer_id);
	    side       = sct_id.side(wafer_id);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + side;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and wafer fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    nloops = 1000;
    num    = hash_max*nloops;


    /// Identifier and wafer fields access and wafer ID again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		sct_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = sct_id.wafer_id(i);
	    barrel_ec  = sct_id.barrel_ec(wafer_id);
	    layer_disk = sct_id.layer_disk(wafer_id);
	    phi_module = sct_id.phi_module(wafer_id);
	    eta_module = sct_id.eta_module(wafer_id);
	    side       = sct_id.side(wafer_id);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + side +
              sct_id.wafer_id(barrel_ec, layer_disk, phi_module, eta_module, side).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and wafer fields and wafer ID again " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 100000;

    // Save sct ids

    unsigned int nstripids = 20000;
    num     = nstripids*nloops;
    Identifier stripid;
    std::vector<Identifier> stripids(nstripids);

    SCT_ID::const_expanded_id_iterator	first_sct = sct_id.strip_begin();  
    SCT_ID::const_expanded_id_iterator	last_sct  = sct_id.strip_end();
    unsigned int nids = 0;
    for (; first_sct != last_sct && nids < nstripids; ++first_sct, ++nids) {
	const ExpandedIdentifier& exp_id = *first_sct;
	stripids[nids] =  sct_id.strip_id (exp_id[2],
					   exp_id[3], 
					   exp_id[4],
					   exp_id[5],
					   exp_id[6],
					   exp_id[7]);
    }

    /// sct Identifier access - empty loop

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = sct_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = sct_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    n += i;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID empty loop " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;

    nloops = 10000;
    num    = nstripids*nloops;

    /// Get sct id

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = sct_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = sct_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    n += stripid.get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID from vector " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;

    nloops = 1000;
    num    = nstripids*nloops;

    int strip;
    
    /// sct Identifier field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    barrel_ec  = sct_id.barrel_ec(stripid);
	    layer_disk = sct_id.layer_disk(stripid);
	    phi_module = sct_id.phi_module(stripid);
	    eta_module = sct_id.eta_module(stripid);
	    side       = sct_id.side(stripid);
	    strip      = sct_id.strip(stripid);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + 
              side + strip + stripid.get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 300;
    num    = nstripids*nloops;

    /// sct Identifier field access and id back

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    barrel_ec  = sct_id.barrel_ec(stripid);
	    layer_disk = sct_id.layer_disk(stripid);
	    phi_module = sct_id.phi_module(stripid);
	    eta_module = sct_id.eta_module(stripid);
	    side       = sct_id.side(stripid);
	    strip      = sct_id.strip(stripid);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + 
              side + strip + stripid .get_compact()+ 
              sct_id.strip_id(barrel_ec, 
                              layer_disk,
                              phi_module,
                              eta_module,
                              side,
                              strip).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID fields and sct id back " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// sct Identifier get wafer id

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    n         +=  stripid.get_compact() + sct_id.wafer_id(stripid).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID then wafer id " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// sct Identifier get wafer id and eta/phi index

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    strip      = sct_id.strip(stripid);
	    n         += strip + stripid.get_compact() +
              sct_id.wafer_id(stripid).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct, wafer id and and eta/phi index" << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// sct Identifier get wafer id and eta/phi index and strip id again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    strip      = sct_id.strip(stripid);
	    wafer_id   = sct_id.wafer_id(stripid);
	    n         += strip + stripid.get_compact() + wafer_id.get_compact() +
              sct_id.strip_id(wafer_id, strip).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct, wafer id and and eta/phi index and stripid again" << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

}

static void
check_trt_timing(IdDictMgr& idd)
{
    
    std::cout << "=========>  Begin  check_trt_timing " << std::endl;
    std::cout << "=========>  Begin  check_trt_timing " << std::endl;

    // Check the timing for trt 

    TRT_ID trt_id;

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

    if(trt_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_neighbour_timing - cannot init from dict" << std::endl;
    }

    std::cout << "Timing for init from dictionary " << std::endl;
    
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;


    /// Loop over all channel hashes
    size_type hash_max = trt_id.straw_layer_hash_max ();

    std::cout << "Number of channels " <<  hash_max
	      << std::endl;

    int nloops = 3000;
    float num  = hash_max*nloops;
    Identifier::value_type n = 0;


    /// Empty loop

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;

    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    n += i;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Empty loop " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    float user    = (float)deltaUser/num;
    float kernel  = (float)deltaKernel/num;
    float elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Straw_Layer Identifier access

    /// Store in object the measured times
    Identifier layer_id;
    IdContext channelContext = trt_id.straw_layer_context();
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = trt_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = trt_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    n += trt_id.layer_id(i).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
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
	    layer_id = trt_id.layer_id(i);
	    n += trt_id.barrel_ec(layer_id);
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
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and layer_or_wheel field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    layer_id = trt_id.layer_id(i);
	    n += trt_id.layer_or_wheel(layer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access layer_or_wheel field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

    /// Identifier and phi_module field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    layer_id = trt_id.layer_id(i);
	    n += trt_id.phi_module(layer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access phi_module field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and straw_layer field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    layer_id = trt_id.layer_id(i);
	    n += trt_id.straw_layer(layer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access straw_layer field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 300;
    num    = hash_max*nloops;
    

    /// Identifier access and hash access again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    layer_id = trt_id.layer_id(i);
	    n += trt_id.straw_layer_hash(layer_id);
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
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    /// Identifier and straw_layer fields access

    /// Store in object the measured times
    int barrel_ec;
    int layer_or_wheel;
    int phi_module;
    int straw_layer;
    
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    nloops = 1000;
    num     = hash_max*nloops;

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    layer_id       = trt_id.layer_id(i);
	    barrel_ec      = trt_id.barrel_ec(layer_id);
	    layer_or_wheel = trt_id.layer_or_wheel(layer_id);
	    phi_module     = trt_id.phi_module(layer_id);
	    straw_layer    = trt_id.straw_layer(layer_id);
	    n         += barrel_ec + layer_or_wheel + straw_layer + phi_module;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and straw_layer fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    nloops = 300;
    num     = hash_max*nloops;

    /// Identifier and straw_layer fields access and straw_layer ID again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    layer_id       = trt_id.layer_id(i);
	    barrel_ec      = trt_id.barrel_ec(layer_id);
	    layer_or_wheel = trt_id.layer_or_wheel(layer_id);
	    phi_module     = trt_id.phi_module(layer_id);
	    straw_layer    = trt_id.straw_layer(layer_id);
	    n         += barrel_ec + layer_or_wheel + straw_layer + phi_module +
              trt_id.layer_id(barrel_ec, layer_or_wheel, phi_module, straw_layer).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and straw_layer fields and straw_layer ID again " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    nloops = 10000;

    // Save trt ids

    unsigned int nstrawids = 20000;
    num     = nstrawids*nloops;
    Identifier strawid;
    std::vector<Identifier> strawids(nstrawids);

    TRT_ID::const_expanded_id_iterator	first_trt = trt_id.straw_begin();  
    TRT_ID::const_expanded_id_iterator	last_trt  = trt_id.straw_end();
    unsigned int nids = 0;
    for (; first_trt != last_trt && nids < nstrawids; ++first_trt, ++nids) {
	const ExpandedIdentifier& exp_id = *first_trt;
	strawids[nids] =  trt_id.straw_id (exp_id[2],
					   exp_id[3], 
					   exp_id[4],
					   exp_id[5],
					   exp_id[6]);
    }

    /// trt Identifier access - empty loop

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = trt_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = trt_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstrawids; ++i) {
	    ++n;
	    n += i;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get trt ID empty loop " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;

    nloops = 10000;
    num    = nstrawids*nloops;


    /// Get trt id

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = trt_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = trt_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstrawids; ++i) {
	    ++n;
	    strawid = strawids[i];
	    n += strawid.get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get trt ID from vector " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;

    nloops = 1000;
    num    = nstrawids*nloops;

    int straw;
    
    /// trt Identifier field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstrawids; ++i) {
	    ++n;
	    strawid        = strawids[i];
	    barrel_ec      = trt_id.barrel_ec(strawid);
	    layer_or_wheel = trt_id.layer_or_wheel(strawid);
	    phi_module     = trt_id.phi_module(strawid);
	    straw_layer    = trt_id.straw_layer(strawid);
	    straw          = trt_id.straw(strawid);
	    n         += barrel_ec + layer_or_wheel + straw_layer + phi_module + 
              straw + strawid.get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get trt ID fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    nloops = 300;
    num    = nstrawids*nloops;

    /// trt Identifier field access and id back

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstrawids; ++i) {
	    ++n;
	    strawid = strawids[i];
	    barrel_ec  = trt_id.barrel_ec(strawid);
	    layer_or_wheel = trt_id.layer_or_wheel(strawid);
	    phi_module = trt_id.phi_module(strawid);
	    straw_layer = trt_id.straw_layer(strawid);
	    straw          = trt_id.straw(strawid);
	    n         += barrel_ec + layer_or_wheel + straw_layer + phi_module + 
              straw + strawid.get_compact() +
		trt_id.straw_id(barrel_ec, 
				layer_or_wheel,
				phi_module,
				straw_layer,
				straw).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get trt ID fields and straw id back " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 3000;
    num    = nstrawids*nloops;
    

    /// trt Identifier get straw_layer id

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstrawids; ++i) {
	    ++n;
	    strawid = strawids[i];
	    n         +=  strawid.get_compact() + trt_id.layer_id(strawid).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get trt ID then straw_layer id " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// trt Identifier get straw_layer id and straw index

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstrawids; ++i) {
	    ++n;
	    strawid = strawids[i];
	    straw   = trt_id.straw(strawid);
	    n         += straw + strawid.get_compact() +
              trt_id.layer_id(strawid).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get trt, straw_layer id and straw" << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 1000;
    num    = nstrawids*nloops;
    

    /// trt Identifier get straw_layer id and eta/phi index and straw id again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstrawids; ++i) {
	    ++n;
	    strawid = strawids[i];
	    straw      = trt_id.straw(strawid);
	    layer_id   = trt_id.layer_id(strawid);
	    n         += straw + strawid.get_compact() + layer_id.get_compact() +
              trt_id.straw_id(layer_id, straw).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get trt, straw_layer id and straw and strawid again" << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

}


static void
check_pixel_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_pixel_decoding" << std::endl;
    std::cout << "=========>  check_pixel_decoding" << std::endl;

    PixelID pixel_id;

    if(pixel_id.initialize_from_dictionary (idd)) {
	std::cout << "check_pixel_decoding - cannot init from dict" << std::endl;
    }


    pixel_id.test_wafer_packing	();

}

static void
check_sct_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_sct_decoding" << std::endl;
    std::cout << "=========>  check_sct_decoding" << std::endl;

    SCT_ID sct_id;

    if(sct_id.initialize_from_dictionary (idd)) {
	std::cout << "check_sct_decoding - cannot init from dict" << std::endl;
    }

    sct_id.set_do_checks(true);

    sct_id.test_wafer_packing	();

}

static void
check_trt_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_trt_decoding" << std::endl;
    std::cout << "=========>  check_trt_decoding" << std::endl;

    TRT_ID trt_id;

    if(trt_id.initialize_from_dictionary (idd)) {
	std::cout << "check_trt_decoding - cannot init from dict" << std::endl;
    }


    std::cout << "=========>  straw hash max" << trt_id.straw_hash_max() << std::endl;


    trt_id.test_trt_ids	();

    // Check is_trt
    PixelID pixel_id;
    if(pixel_id.initialize_from_dictionary (idd)) {
	std::cout << "check_pixel_decoding - cannot init from dict" << std::endl;
    }

    Identifier id = pixel_id.pixel_id(0, 2, 0, 5, 211, 40);
    std::cout << "=========>  pixel id " << trt_id.show_to_string(id) << std::endl;
    std::cout << "=========>  is_trt "   << trt_id.is_trt(id) << std::endl;
    std::cout << "=========>  is_blayer "   << pixel_id.is_blayer(id) << std::endl;
    id = pixel_id.pixel_id(0, 0, 0, 5, 211, 40);
    std::cout << "=========>  pixel id " << trt_id.show_to_string(id) << std::endl;
    std::cout << "=========>  is_trt "   << trt_id.is_trt(id) << std::endl;
    std::cout << "=========>  is_blayer "   << pixel_id.is_blayer(id) << std::endl;
    id = pixel_id.pixel_id(2, 0, 0, 5, 211, 40);
    std::cout << "=========>  pixel id " << trt_id.show_to_string(id) << std::endl;
    std::cout << "=========>  is_trt "   << trt_id.is_trt(id) << std::endl;
    std::cout << "=========>  is_blayer "   << pixel_id.is_blayer(id) << std::endl;
    id = pixel_id.pixel_id(0, 1, 0, 5, 211, 40);
    std::cout << "=========>  pixel id " << trt_id.show_to_string(id) << std::endl;
    std::cout << "=========>  is_trt "   << trt_id.is_trt(id) << std::endl;
    std::cout << "=========>  is_blayer "   << pixel_id.is_blayer(id) << std::endl;


}

static void
check_silicon_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_silicon_decoding" << std::endl;
    std::cout << "=========>  check_silicon_decoding" << std::endl;

    PixelID pixel_id;
    
    SCT_ID sct_id;

    if(pixel_id.initialize_from_dictionary (idd)) {
	std::cout << "check_silicon_decoding - cannot init pixel from dict" << std::endl;
    }

    if(sct_id.initialize_from_dictionary (idd)) {
	std::cout << "check_silicon_decoding - cannot init sct from dict" << std::endl;
    }

    SiliconID silicon_id(&pixel_id, &sct_id);
    
    if(silicon_id.initialize_from_dictionary (idd)) {
	std::cout << "check_silicon_decoding - cannot init silicon from dict" << std::endl;
    }

    silicon_id.test_wafer_hashes	();
}

int main (int argc, char* argv[])  
{  
    if (argc < 2) return (1);  
  

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );



    IdDictParser parser;  

    
//    parser.register_external_entity("InnerDetector", "InDetIdDictFiles/IdDictInnerDetector_IBL-01.xml");
//    parser.register_external_entity("LArCalorimeter", "IdDictLArCalorimeter_H8_2004.xml");
//    parser.register_external_entity("InnerDetector", "IdDictInnerDetector_slhc.xml");
//    parser.register_external_entity("InnerDetector", "IdDictInnerDetector_Cosmic.xml");
//    parser.register_external_entity("InnerDetector", "IdDictInnerDetector_CTB2004.xml");

    IdDictMgr& idd = parser.parse (argv[1]);  
//    IdDictMgr& idd = parser.parse (argv[1], "initial_layout");  


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


    check_sct_decoding(idd);
    check_trt_decoding(idd);
    check_silicon_decoding(idd);
    check_pixel_decoding(idd);
    check_pixel_timing(idd);
    check_sct_decoding(idd);
    check_sct_timing(idd);
    check_trt_decoding(idd);
    check_trt_timing(idd);

    return 0;  
}  
