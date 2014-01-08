/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef getindfromamdc_H
#define getindfromamdc_H
extern "C" void getmdtindfromamdc_( char* STANAME, int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube, int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube );
extern "C" void getcscindfromamdc_( char* STANAME, int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube, double& SZflag, int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer, int& MeasuresPhi, int& Strip );
extern "C" void getrpcindfromamdc_( char* STANAME, int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri, int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi, int& GasGap, int& MeasuresPhi, int& Strip );
extern "C" void gettgcindfromamdc_( char* STANAME, int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri, int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel );
extern "C" void getnberofmdtmultilayers_( char* STANAME, int& Jff, int& Jzz,                         int& NberOfMDTMultilayers ) ;
extern "C" void getnberofmdtlayers_(      char* STANAME, int& Jff, int& Jzz, int& jMuLay,            int& NberOfMDTLayers      ) ;
extern "C" void getnberofmdttubes_(       char* STANAME, int& Jff, int& Jzz, int& jMuLay, int& jLay, int& NberOfMDTTubes       ) ;
#endif
