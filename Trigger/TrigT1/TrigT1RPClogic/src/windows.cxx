/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1RPClogic/windows.h"

#include "TrigT1RPClogic/large.h"
#include "TrigT1RPClogic/small.h"
#include "TrigT1RPClogic/foot.h"

int windows(float distance,float eta_pivot,int thr,int sector)
{
 
  int bin=0,s=0;
   
  for (int j=1;j<=50;j++)
  {
      if ((eta_pivot>=-1.1+(j-1)*0.044)&&(eta_pivot<-1.1+j*0.044))  bin = j;
  }
    
  int sector_type=((sector + 1)/2)%2;

  const float (*window)[NBINS][BOUND] = 0;


  if (sector_type == 0)
  {
      window = Large_win;
  }
  else if (sector_type == 1 &&
              !( sector==21 || sector==22 || sector==25 || sector==26 ))
  {
      window = Small_win;
  }
  else
  {
      window = Foot_win;
  }
   
      if(distance <= 0)
      {	  	 
	  if (!window[thr][bin-1][0])
	  {
	      bin = give_nearest_bin(window,thr,bin,0,1);
	  }
	   
	  if(distance>=window[thr][bin-1][0]) s = 1;
      }
      else
      {
	   if (!window[thr][bin-1][1])
	   {
	       bin = give_nearest_bin(window,thr,bin,1,1);
	   }
	   
	   if(distance<=window[thr][bin-1][1]) s = 1;
	}

   return s;
}

int give_nearest_bin(const float (*window)[NBINS][BOUND],int thr,int bin,int bound,
                     int step)
{
    int previous = (bin <= NBINS/2)? bin + step : bin - step;
    int next     = (bin <= NBINS/2)? bin - step : bin + step;
    
    if (window[thr][previous-1][bound]) return previous;
    if (window[thr][next-1][bound])     return next;
    
    if (next == 1 || next == NBINS) return next;
    
    return give_nearest_bin(window,thr,bin,bound,step+1);
}
