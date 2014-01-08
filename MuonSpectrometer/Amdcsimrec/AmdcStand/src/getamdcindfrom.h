/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef getamdcindfrom_H
#define getamdcindfrom_H
extern "C" void getamdcindfrommdt_( char* STANAME, int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube, int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube );
extern "C" void getamdcindfromcsc_( char* STANAME, int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer, int& MeasuresPhi, int& Strip, int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube, double& SZflag );
extern "C" void getamdcindfromrpc_( char* STANAME, int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi, int& GasGap, int& MeasuresPhi, int& Strip, int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri );
extern "C" void getamdcindfromtgc_( char* STANAME, int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel, int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri );
#endif
