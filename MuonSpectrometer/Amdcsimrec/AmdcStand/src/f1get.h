/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef f1get_H
#define f1get_H
extern "C" void f1gettubepositionfrommdtindex_(  char* STANAME, int& StationEta, int& StationPhi, int& Multilayer  , int& TubeLayer, int& Tube       ,                                            double* XYZ1, double* XYZ2, double* SZT1, double* SZT2, double& Radius );
extern "C" void f1getstrippositionfromcscindex_( char* STANAME, int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer, int& MeasuresPhi, int& Strip,                                double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4, double* XYZCenter, double* SZT1, double* SZT2, double* SZT3, double* SZT4, double* SZTCenter );
extern "C" void f1getstrippositionfromrpcindex_( char* STANAME, int& StationEta, int& StationPhi, int& DoubletR    , int& DoubletZ , int& DoubletPhi , int& GasGap, int& MeasuresPhi, int& Strip, double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4,                    double* SZT1, double* SZT2, double* SZT3, double* SZT4 );
extern "C" void f1getchannelpositionfromtgcind_( char* STANAME, int& StationEta, int& StationPhi, int& GasGap      , int& IsStrip  , int& Channel    ,                                            double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4,                    double* SZT1, double* SZT2, double* SZT3, double* SZT4 );

extern "C" void f1getmdtindexfromtubeposition_(  char* STANAME, int& StationEta, int& StationPhi, int& Multilayer  , int& TubeLayer,                                                  double* XYZ, int& Tube    );
extern "C" void f1getcscindexfromstripposition_( char* STANAME, int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer, int& MeasuresPhi,                                double* XYZ, int& Strip   );
extern "C" void f1getrpcindexfromstripposition_( char* STANAME, int& StationEta, int& StationPhi, int& DoubletR    , int& DoubletZ , int& DoubletPhi , int& GasGap, int& MeasuresPhi, double* XYZ, int& Strip   );
extern "C" void f1gettgcindexfromchannelpos_(    char* STANAME, int& StationEta, int& StationPhi, int& GasGap      , int& IsStrip  ,                                                  double* XYZ, int& Channel );

extern "C" void f1getstationtype_( int& Jtyp, char* STANAME );

extern "C" void f1getstationdimensions_(      char* STANAME, int& Jgeo, int& Jcut, int& IsValid, double& DimLocX, double& DimLocY, double& DimLocZ, double& CenterLocX, double& CenterLocY, double& CenterLocZ );

extern "C" void f1getnumberofstationobjects_( char* STANAME, int& Jgeo, int& NbObj );
extern "C" void f1getstationobjectparam_(     char* STANAME, int& Jgeo, int& Job, int& GeomIsValid, char* TECNAME, int& GeomTechnoIndex, int& GeomSplitX, int& GeomSplitY, int& GeomShapeIndex, double& GeomDx, double& GeomDy, double& GeomDz, double& GeomWs, double& GeomWl, double& GeomLe, double& GeomEx, double& GeomD1, double& GeomD2, double& GeomD3 );

extern "C" void f1getstationpositionparam_(  char* STANAME, int& Jff, int& Jzz, int& PosiIsValid, int& PosiJgeo, int& PosiJcut, int& PosiIsBarrel, double& PosiPhi, double& PosiZ, double& PosiR, double& PosiS, double& PosiAlfa, double& PosiBeta, double& PosiGamma );

extern "C" void f1getnumberofcutoutobjects_( char* STANAME, int& Jgeo, int& Jcut, int& NbObj );
extern "C" void f1getstationcutoutobjparam_( char* STANAME, int& Jgeo, int& Jcut, int& JcutObj, int& CutJob,   double& CutDx, double& CutDy, double& CutWs, double& CutWl, double& CutLe, double& CutEx, double& CutAn );


extern "C" void f1getramdcvar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, double& Rvar );
extern "C" void f1getiamdcvar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, int& Ivar );
extern "C" void f1getcamdcvar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, int& Lvar,  char* CVAR );

#endif
