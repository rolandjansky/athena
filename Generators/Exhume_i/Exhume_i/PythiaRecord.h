/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////PythiaInterface.h
#ifndef PYTHIA_RECORD_HH
#define PYTHIA_RECORD_HH

struct PPYDAT1{
  int mstu[200];
  double paru[200];
  int mstj[200];
  double parj[200];

};

struct PPYMSSM{
  int imss[100];
  double rmss[100];
};

struct PPYDAT2{
  int kchg[4][500];
  double pmas[4][500];
  double parf[2000];
  double vkcm[4][4];
};

struct PPYDAT3{
  int mdcy[3][500];
  int mdme[2][8000];
  double brat[8000];
  int kfdp[5][8000];

};

struct PPYJETS{
  int   n;	
  int npad;
  int   k[5][4000];
  double p[5][4000];
  double v[5][4000];
};

struct PPYPARS{
  int mstp[200];
  double parp[200];
  int msti[200];
  double pari[200];
};
struct PHEPEVT{
  int   nevhep;         
  int   nhep;
  int   isthep[4000];
  int   idhep[4000];
  int   jmohep[4000][2];
  int   jdahep[4000][2];
  double phep[4000][5];
  double vhep[4000][4];
};
  

//declaration of the structs in pythia
extern"C"{
  extern struct PPYJETS pyjets_;
  extern struct PPYPARS pypars_;
  //extern struct PHEPEVT hepevt_;
  extern struct PPYDAT1 pydat1_;
  extern struct PPYDAT2 pydat2_;
  extern struct PPYDAT3 pydat3_;
  extern struct PPYMSSM pymssm_;
}

//declaration of pythia functions for user input

extern "C"{
  int pycomp_(int&);
  void py1ent_(int&,int&,double&,double&, double&);
  void pyjoin_(int&,int[2]);
  void pyexec_();
  void pyhepc_(int&);
  void pylist_(int&);
  void pyinre_();
  void pymsin_();
  void pyshow_(int&,int&,double&);
  void pyresd_(int&);
}


#endif

