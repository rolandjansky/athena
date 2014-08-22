/*
 *  SCT_ClusterStruct.h
 *  
 *
 *  Created by Shaun Roe on 24/05/2005.
 *  Copyright (c) 2005 CERN. All rights reserved.
 *
 */

#ifndef SCT_ClusterStruct_H
#define SCT_ClusterStruct_H
///Simple struct to hold the first strip and number of strips in a cluster
struct SCT_ClusterStruct{
	unsigned int firstStrip;
	unsigned int numberOfStrips;
};

#endif

