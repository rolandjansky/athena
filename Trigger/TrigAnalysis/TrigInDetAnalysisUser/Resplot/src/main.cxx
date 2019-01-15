/**
 **     @file    main.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:27 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <cmath>
#include <vector>

#include "TFile.h"
#include "TStyle.h"
#include "TMath.h"

#include "Resplot.h"

#include "Random.h"


int main() { 

  std::cout << "main()" << std::endl;

  TFile f("out.root", "recreate");


  int     Nxbins   = 10;
  double  xlimitlo = -10;
  double  xlimithi =  10;

  int     Nybins    = 300;
  double  ylimitlo = -30;
  double  ylimithi =  30;


  Resplot  plot1("plot1", Nxbins, xlimitlo, xlimithi,   Nybins, ylimitlo, ylimithi );
  Resplot  plot2("plot2", Nxbins, xlimitlo, xlimithi,   Nybins, ylimitlo, ylimithi );

  std::cout << "running" << std::endl;

  Random& rnd = Random::RandomBuilder();


  // loop over some "events"
  for ( int i=0 ; i<10000 ; i++ ) { 

    if ( (i%1000)==0 ) std::cout << "process event " << i << std::endl;

    double x0 = 20*rnd.uniform()-10;
    double y0 = (2+x0*x0/20)*rnd.gauss();
    
    plot1.Fill(x0, y0);
    plot2.Fill(x0, y0);
  
  }

  std::cout << "finalising" << std::endl;

  plot1.Finalise(Resplot::FitNull95);

  std::cout << __LINE__ << std::endl;

  plot2.Finalise(Resplot::FitGaussian);
  
  plot1.Write();
  plot2.Write();


  f.Write();


  std::cout << "closing f" << std::endl; 
  f.Close();


  std::cout << "operations" << std::endl; 

  return 0;
}
