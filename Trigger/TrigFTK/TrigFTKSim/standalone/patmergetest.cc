/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Author: Stefan Schmitt, DESY
 *  Date: 2013/12/02
 *
 * example program to write single patterns to a root file
 *
 */

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <TRandom3.h>
#include "TrigFTKSim/FTKPatternBySector.h"
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/ftk_dcap.h"

using namespace std;

int main(int argc, char const *argv[]) {
   int NLAYER=8;
   TDirectory *outFile=FTKRootFile::Instance()->CreateRootFile("test.root");
   ftk_dcap::ostream *bzOutFile=ftk_dcap::open_for_write("test.patt.bz2");

   FTKPatternBySectorWriter *patternWriter=
      new FTKPatternBySectorWriter(*outFile);
   patternWriter->SetNLayers(NLAYER);

   int sector=0;
   FTKPatternWithCoverage pattern(NLAYER);

   TRandom *rnd=new TRandom3(1234);

   int NPATTERN=100000;
   (*bzOutFile)<<NPATTERN<<" "<<NLAYER<<"\n";

   // create some random patterns
   for(int i=0;i<NPATTERN;i++) {
      // sector number 
      sector=rnd->Poisson(2.5);
      // hit numbers for each layer
      for(int layer=0;layer<NLAYER;layer++) {
         int hitNumber=(layer+1)*1000+sector*20+rnd->Poisson(0.5);
         pattern.SetHit(layer,hitNumber);
      }
      // coverage
      pattern.SetCoverage(1);
      // store to root file
      patternWriter->AppendPattern(sector,pattern);
      // store to bz file
      (*bzOutFile)<<i;
      for(int layer=0;layer<NLAYER;layer++) {
         (*bzOutFile)<<" "<<pattern.GetHitPattern().GetHit(layer);
      }
      (*bzOutFile)<<" "<<sector<<" "<<pattern.GetCoverage()<<"\n";
   }
   delete patternWriter;

   bzOutFile->flush();
   delete bzOutFile;

   return 0;
}
