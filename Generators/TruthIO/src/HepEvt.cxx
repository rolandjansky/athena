/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


extern "C" {
const unsigned int hepevt_bytes_allocation_ATLAS =
                sizeof(long int) * ( 2 + 6 * 10000 )
                + sizeof(double) * ( 9 * 10000 );
struct {
	char data[hepevt_bytes_allocation_ATLAS];
    } hepevt_;
}