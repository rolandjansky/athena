/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LoadOutLay_H
#define LoadOutLay_H
extern "C" void resetloadoutlay_(int& OUTNTOT);
extern "C" void loadoutlay_(int& OUTJTYPi, int& OFFJFFi,int& OFFJZZi, int& OFFJOBJi);
extern "C" void resetloadoutlaycscwirelayer_(int& OUTNTOT);
extern "C" void loadoutlaycscwirelayer_(int& OUTJTYPi, int& OFFJFFi,int& OFFJZZi, int& OFFJOBJi, int& OFFJsli);
extern "C" void resetloadoutlaymdtoff_(int& OUTNTOT);
extern "C" void loadoutlaymdtoff_(int& OUTJTYPi, int& OFFJFFi,int& OFFJZZi, int& OFFJOBJi, int& OFFJsli, int& OFFJtubei);
extern "C" void resetloadoutlayrpcoff_(int& OUTNTOT);
extern "C" void loadoutlayrpcoff_(int& OUTJTYPi, int& OFFJFFi,int& OFFJZZi, int& OFFJOBJi, int& OFFJsplii, int& OFFJsli, int& OFFJszi, int& OFFJstrii);
extern "C" void resetloadoutlaycscoff_(int& OUTNTOT);
extern "C" void loadoutlaycscoff_(int& OUTJTYPi, int& OFFJFFi,int& OFFJZZi, int& OFFJOBJi, int& OFFJsli, int& OFFJtubei, double& OFFSZflagi);
extern "C" void resetloadoutlaytgcoff_(int& OUTNTOT);
extern "C" void loadoutlaytgcoff_(int& OUTJTYPi, int& OFFJFFi,int& OFFJZZi, int& OFFJOBJi, int& OFFJsplii, int& OFFJsli, int& OFFJszi, int& OFFJstrii);
extern "C" void filldeadtubesamdc_();
extern "C" void filldeadchannels_();
#endif
