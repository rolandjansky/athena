/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WINDOWS_H
#define WINDOWS_H

#define THRES 5    // thershold number for the Z-Trigger windows
#define NBINS 50   // eta bins for the Z-Trigger windows
#define BOUND 2    // boundaries of the Z-Trigger windows


int give_nearest_bin(const float(*)[NBINS][BOUND],int,int,int,int);

int windows(float,float,int,int);

#endif
