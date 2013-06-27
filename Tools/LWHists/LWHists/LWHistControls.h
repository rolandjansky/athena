/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWHistControls                      //
//                                                            //
//  Description: Static methods for controlling and           //
//               monitoring various aspects related to the    //
//               light-weight histograms (in particular       //
//               memory management. Typical clients should    //
//               not worry about these.                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWHISTCONTROLS_H
#define LWHISTCONTROLS_H

class LWHistControls {
public:

  //Whether or not calls to the various ::getROOTHist..() should
  //trigger a cleanup of the internal data structures apart from the
  //held ROOT histogram (i.e. effectively disallow further usage of
  //the LW histogram itself). Default is true:
  static bool cleanupOnGetROOT();
  static void setCleanupOnGetROOT(bool);

  //As absurd as it is to use ROOT histograms as the backend of LW
  //histograms, this is exactly what we will have to do when running
  //in the online environment (in that case getROOTHist will return
  //the backend - which is only deleted along with the LW histogram):
  static bool hasROOTBackend();
  static void setROOTBackend(bool);

  //For statistics (bytes):
  static long long getTotalPoolMemAllocated();//Obsolete! Use LWHistStats method instead
  static long long getTotalPoolMemUsed();//Obsolete! Use LWHistStats method instead
  //Not more info, but for convenience:
  static long long getMemUnusedButAllocatedInPools();//Obsolete! Use LWHistStats method instead
  static double poolWasteFraction();//0: perfect, 1: very bad//Obsolete! Use LWHistStats method instead

  //Call at the end of the job for a happy valgrind:
  static void releaseAllHeldMemory();

private:
  LWHistControls(){}
  ~LWHistControls(){}
  class Imp;

};

#endif
