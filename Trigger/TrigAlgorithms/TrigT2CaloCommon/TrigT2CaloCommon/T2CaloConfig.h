/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOCOMMON_T2CALOCONFIG_H
#define TRIGT2CALOCOMMON_T2CALOCONFIG_H

// ********************************************************************
//
// NAME:     T2CaloConfig.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
//
// AUTHOR:   Carlos Osuna <Carlos.Osuna@ifae.es>
//
// CREATED: Aug-05
//
// DESCRIPTION: Class to store properties of sizes of different regions
//        needed for tau calculations. Those properties are set through 
//        T2Calo jobOptions.
//  
// ********************************************************************


class T2CaloConfig {

  friend class T2CaloBase;

  public:

    T2CaloConfig() {};
    ~T2CaloConfig() {};

    int nStripetaEM0() const;
    int nStripphiEM0() const;
    int nStripetaEM1() const;
    int nStripphiEM1() const;
    int nStripetaEM2() const;
    int nStripphiEM2() const;
    int nStripetaEM3() const;
    int nStripphiEM3() const;

    int nStripetaHAD1() const;
    int nStripphiHAD1() const;
    int nStripetaHAD2() const;
    int nStripphiHAD2() const;
    int nStripetaHAD3() const;
    int nStripphiHAD3() const;

    int nStripetaEM0nar() const;
    int nStripphiEM0nar() const;
    int nStripetaEM1nar() const;
    int nStripphiEM1nar() const;
    int nStripetaEM2nar() const;
    int nStripphiEM2nar() const;
    int nStripetaEM3nar() const;
    int nStripphiEM3nar() const;

    int nStripetaEM0wid() const;
    int nStripphiEM0wid() const;
    int nStripetaEM1wid() const;
    int nStripphiEM1wid() const;
    int nStripetaEM2wid() const;
    int nStripphiEM2wid() const;
    int nStripetaEM3wid() const;
    int nStripphiEM3wid() const;

    int nStripetaEM0nor()  const;
    int nStripphiEM0nor()  const;
    int nStripetaEM1nor()  const;
    int nStripphiEM1nor()  const;
    int nStripetaEM2nor()  const;
    int nStripphiEM2nor()  const;
    int nStripetaEM3nor()  const;
    int nStripphiEM3nor()  const;


    int  nStripetaHAD1nar() const;
    int  nStripphiHAD1nar() const;
    int  nStripetaHAD2nar() const;
    int  nStripphiHAD2nar() const;
    int  nStripetaHAD3nar() const;
    int  nStripphiHAD3nar() const;
    int  nStripetaHAD1wid() const;
    int  nStripphiHAD1wid() const;
    int  nStripetaHAD2wid() const;
    int  nStripphiHAD2wid() const;
    int  nStripetaHAD3wid() const;
    int  nStripphiHAD3wid() const;

    int  nStripetaHAD1nor()  const;
    int  nStripphiHAD1nor()  const;
    int  nStripetaHAD2nor()  const;
    int  nStripphiHAD2nor()  const;
    int  nStripetaHAD3nor()  const;
    int  nStripphiHAD3nor()  const;

    
    void set_nStripetaEM0(int);
    void set_nStripphiEM0(int);
    void set_nStripetaEM1(int);
    void set_nStripphiEM1(int);
    void set_nStripetaEM2(int);
    void set_nStripphiEM2(int);
    void set_nStripetaEM3(int);
    void set_nStripphiEM3(int);

    void set_nStripetaHAD1(int);
    void set_nStripphiHAD1(int);
    void set_nStripetaHAD2(int);
    void set_nStripphiHAD2(int);
    void set_nStripetaHAD3(int);
    void set_nStripphiHAD3(int);

    void set_nStripetaEM0nar(int);
    void set_nStripphiEM0nar(int);
    void set_nStripetaEM1nar(int);
    void set_nStripphiEM1nar(int);
    void set_nStripetaEM2nar(int);
    void set_nStripphiEM2nar(int);
    void set_nStripetaEM3nar(int);
    void set_nStripphiEM3nar(int);

    void set_nStripetaEM0wid(int);
    void set_nStripphiEM0wid(int);
    void set_nStripetaEM1wid(int);
    void set_nStripphiEM1wid(int);
    void set_nStripetaEM2wid(int);
    void set_nStripphiEM2wid(int);
    void set_nStripetaEM3wid(int);
    void set_nStripphiEM3wid(int);

    void set_nStripetaEM0nor(int);
    void set_nStripphiEM0nor(int);
    void set_nStripetaEM1nor(int);
    void set_nStripphiEM1nor(int);
    void set_nStripetaEM2nor(int);
    void set_nStripphiEM2nor(int);
    void set_nStripetaEM3nor(int);
    void set_nStripphiEM3nor(int);


    void set_nStripetaHAD1nar(int);
    void set_nStripphiHAD1nar(int);
    void set_nStripetaHAD2nar(int);
    void set_nStripphiHAD2nar(int);
    void set_nStripetaHAD3nar(int);
    void set_nStripphiHAD3nar(int);
    void set_nStripetaHAD1wid(int);
    void set_nStripphiHAD1wid(int);
    void set_nStripetaHAD2wid(int);
    void set_nStripphiHAD2wid(int);
    void set_nStripetaHAD3wid(int);
    void set_nStripphiHAD3wid(int);

    void set_nStripetaHAD1nor(int);
    void set_nStripphiHAD1nor(int);
    void set_nStripetaHAD2nor(int);
    void set_nStripphiHAD2nor(int);
    void set_nStripetaHAD3nor(int);
    void set_nStripphiHAD3nor(int);


  protected:
    int  m_nStripetaEM0;
    int  m_nStripphiEM0;
    int  m_nStripetaEM1;
    int  m_nStripphiEM1;
    int  m_nStripetaEM2;
    int  m_nStripphiEM2;
    int  m_nStripetaEM3;
    int  m_nStripphiEM3;

    int  m_nStripetaHAD1;
    int  m_nStripphiHAD1;
    int  m_nStripetaHAD2;
    int  m_nStripphiHAD2;
    int  m_nStripetaHAD3;
    int  m_nStripphiHAD3;

    int  m_nStripetaEM0nar;
    int  m_nStripphiEM0nar;
    int  m_nStripetaEM1nar;
    int  m_nStripphiEM1nar;
    int  m_nStripetaEM2nar;
    int  m_nStripphiEM2nar;
    int  m_nStripetaEM3nar;
    int  m_nStripphiEM3nar;

    int  m_nStripetaEM0wid;
    int  m_nStripphiEM0wid;
    int  m_nStripetaEM1wid;
    int  m_nStripphiEM1wid;
    int  m_nStripetaEM2wid;
    int  m_nStripphiEM2wid;
    int  m_nStripetaEM3wid;
    int  m_nStripphiEM3wid;

    int  m_nStripetaEM0nor;
    int  m_nStripphiEM0nor;
    int  m_nStripetaEM1nor;
    int  m_nStripphiEM1nor;
    int  m_nStripetaEM2nor;
    int  m_nStripphiEM2nor;
    int  m_nStripetaEM3nor;
    int  m_nStripphiEM3nor;


    int  m_nStripetaHAD1nar;
    int  m_nStripphiHAD1nar;
    int  m_nStripetaHAD2nar;
    int  m_nStripphiHAD2nar;
    int  m_nStripetaHAD3nar;
    int  m_nStripphiHAD3nar;

    int  m_nStripetaHAD1wid;
    int  m_nStripphiHAD1wid;
    int  m_nStripetaHAD2wid;
    int  m_nStripphiHAD2wid;
    int  m_nStripetaHAD3wid;
    int  m_nStripphiHAD3wid;

    int  m_nStripetaHAD1nor;
    int  m_nStripphiHAD1nor;
    int  m_nStripetaHAD2nor;
    int  m_nStripphiHAD2nor;
    int  m_nStripetaHAD3nor;
    int  m_nStripphiHAD3nor;


};

inline int T2CaloConfig::nStripetaEM0()  const { return m_nStripetaEM0; }
inline int T2CaloConfig::nStripphiEM0()  const { return m_nStripphiEM0; }
inline int T2CaloConfig::nStripetaEM1()  const { return m_nStripetaEM1; }
inline int T2CaloConfig::nStripphiEM1()  const { return m_nStripphiEM1; }
inline int T2CaloConfig::nStripetaEM2()  const { return m_nStripetaEM2; }
inline int T2CaloConfig::nStripphiEM2()  const { return m_nStripphiEM2; }
inline int T2CaloConfig::nStripetaEM3()  const { return m_nStripetaEM3; }
inline int T2CaloConfig::nStripphiEM3()  const { return m_nStripphiEM3; }

inline int T2CaloConfig::nStripetaHAD1()  const { return m_nStripetaHAD1; }
inline int T2CaloConfig::nStripphiHAD1()  const { return m_nStripphiHAD1; }
inline int T2CaloConfig::nStripetaHAD2()  const { return m_nStripetaHAD2; }
inline int T2CaloConfig::nStripphiHAD2()  const { return m_nStripphiHAD2; }
inline int T2CaloConfig::nStripetaHAD3()  const { return m_nStripetaHAD3; }
inline int T2CaloConfig::nStripphiHAD3()  const { return m_nStripphiHAD3; }

inline int T2CaloConfig::nStripetaEM0nar()  const { return m_nStripetaEM0nar; }
inline int T2CaloConfig::nStripphiEM0nar()  const { return m_nStripphiEM0nar; }
inline int T2CaloConfig::nStripetaEM1nar()  const { return m_nStripetaEM1nar; }
inline int T2CaloConfig::nStripphiEM1nar()  const { return m_nStripphiEM1nar; }
inline int T2CaloConfig::nStripetaEM2nar()  const { return m_nStripetaEM2nar; }
inline int T2CaloConfig::nStripphiEM2nar()  const { return m_nStripphiEM2nar; }
inline int T2CaloConfig::nStripetaEM3nar()  const { return m_nStripetaEM3nar; }
inline int T2CaloConfig::nStripphiEM3nar()  const { return m_nStripphiEM3nar; }

inline int T2CaloConfig::nStripetaEM0wid()  const { return m_nStripetaEM0wid; }
inline int T2CaloConfig::nStripphiEM0wid()  const { return m_nStripphiEM0wid; }
inline int T2CaloConfig::nStripetaEM1wid()  const { return m_nStripetaEM1wid; }
inline int T2CaloConfig::nStripphiEM1wid()  const { return m_nStripphiEM1wid; }
inline int T2CaloConfig::nStripetaEM2wid()  const { return m_nStripetaEM2wid; }
inline int T2CaloConfig::nStripphiEM2wid()  const { return m_nStripphiEM2wid; }
inline int T2CaloConfig::nStripetaEM3wid()  const { return m_nStripetaEM3wid; }
inline int T2CaloConfig::nStripphiEM3wid()  const { return m_nStripphiEM3wid; }

inline int T2CaloConfig::nStripetaEM0nor()   const { return m_nStripetaEM0nor; }
inline int T2CaloConfig::nStripphiEM0nor()   const { return m_nStripphiEM0nor; }
inline int T2CaloConfig::nStripetaEM1nor()   const { return m_nStripetaEM1nor; }
inline int T2CaloConfig::nStripphiEM1nor()   const { return m_nStripphiEM1nor; }
inline int T2CaloConfig::nStripetaEM2nor()   const { return m_nStripetaEM2nor; }
inline int T2CaloConfig::nStripphiEM2nor()   const { return m_nStripphiEM2nor; }
inline int T2CaloConfig::nStripetaEM3nor()   const { return m_nStripetaEM3nor; }
inline int T2CaloConfig::nStripphiEM3nor()   const { return m_nStripphiEM3nor; }


inline int T2CaloConfig::nStripetaHAD1nar()  const { return m_nStripetaHAD1nar; }
inline int T2CaloConfig::nStripphiHAD1nar()  const { return m_nStripphiHAD1nar; }
inline int T2CaloConfig::nStripetaHAD2nar()  const { return m_nStripetaHAD2nar; }
inline int T2CaloConfig::nStripphiHAD2nar()  const { return m_nStripphiHAD2nar; }
inline int T2CaloConfig::nStripetaHAD3nar()  const { return m_nStripetaHAD3nar; }
inline int T2CaloConfig::nStripphiHAD3nar()  const { return m_nStripphiHAD3nar; }

inline int T2CaloConfig::nStripetaHAD1wid()  const { return m_nStripetaHAD1wid; }
inline int T2CaloConfig::nStripphiHAD1wid()  const { return m_nStripphiHAD1wid; }
inline int T2CaloConfig::nStripetaHAD2wid()  const { return m_nStripetaHAD2wid; }
inline int T2CaloConfig::nStripphiHAD2wid()  const { return m_nStripphiHAD2wid; }
inline int T2CaloConfig::nStripetaHAD3wid()  const { return m_nStripetaHAD3wid; }
inline int T2CaloConfig::nStripphiHAD3wid()  const { return m_nStripphiHAD3wid; }

inline int T2CaloConfig::nStripetaHAD1nor()   const { return m_nStripetaHAD1nor; }
inline int T2CaloConfig::nStripphiHAD1nor()   const { return m_nStripphiHAD1nor; }
inline int T2CaloConfig::nStripetaHAD2nor()   const { return m_nStripetaHAD2nor; }
inline int T2CaloConfig::nStripphiHAD2nor()   const { return m_nStripphiHAD2nor; }
inline int T2CaloConfig::nStripetaHAD3nor()   const { return m_nStripetaHAD3nor; }
inline int T2CaloConfig::nStripphiHAD3nor()   const { return m_nStripphiHAD3nor; }




inline  void T2CaloConfig::set_nStripetaEM0(int x)   { m_nStripetaEM0 = x; }
inline  void T2CaloConfig::set_nStripphiEM0(int x)   { m_nStripphiEM0 = x; }
inline  void T2CaloConfig::set_nStripetaEM1(int x)   { m_nStripetaEM1 = x; }
inline  void T2CaloConfig::set_nStripphiEM1(int x)   { m_nStripphiEM1 = x; }
inline  void T2CaloConfig::set_nStripetaEM2(int x)   { m_nStripetaEM2 = x; }
inline  void T2CaloConfig::set_nStripphiEM2(int x)   { m_nStripphiEM2 = x; }
inline  void T2CaloConfig::set_nStripetaEM3(int x)   { m_nStripetaEM3 = x; }
inline  void T2CaloConfig::set_nStripphiEM3(int x)   { m_nStripphiEM3 = x; }

inline  void T2CaloConfig::set_nStripetaHAD1(int x)   { m_nStripetaHAD1 = x; }
inline  void T2CaloConfig::set_nStripphiHAD1(int x)   { m_nStripphiHAD1 = x; }
inline  void T2CaloConfig::set_nStripetaHAD2(int x)   { m_nStripetaHAD2 = x; }
inline  void T2CaloConfig::set_nStripphiHAD2(int x)   { m_nStripphiHAD2 = x; }
inline  void T2CaloConfig::set_nStripetaHAD3(int x)   { m_nStripetaHAD3 = x; }
inline  void T2CaloConfig::set_nStripphiHAD3(int x)   { m_nStripphiHAD3 = x; }

inline  void T2CaloConfig::set_nStripetaEM0nar(int x)   { m_nStripetaEM0nar = x; }
inline  void T2CaloConfig::set_nStripphiEM0nar(int x)   { m_nStripphiEM0nar = x; }
inline  void T2CaloConfig::set_nStripetaEM1nar(int x)   { m_nStripetaEM1nar = x; }
inline  void T2CaloConfig::set_nStripphiEM1nar(int x)   { m_nStripphiEM1nar = x; }
inline  void T2CaloConfig::set_nStripetaEM2nar(int x)   { m_nStripetaEM2nar = x; }
inline  void T2CaloConfig::set_nStripphiEM2nar(int x)   { m_nStripphiEM2nar = x; }
inline  void T2CaloConfig::set_nStripetaEM3nar(int x)   { m_nStripetaEM3nar = x; }
inline  void T2CaloConfig::set_nStripphiEM3nar(int x)   { m_nStripphiEM3nar = x; }

inline  void T2CaloConfig::set_nStripetaEM0wid(int x)   { m_nStripetaEM0wid = x; }
inline  void T2CaloConfig::set_nStripphiEM0wid(int x)   { m_nStripphiEM0wid = x; }
inline  void T2CaloConfig::set_nStripetaEM1wid(int x)   { m_nStripetaEM1wid = x; }
inline  void T2CaloConfig::set_nStripphiEM1wid(int x)   { m_nStripphiEM1wid = x; }
inline  void T2CaloConfig::set_nStripetaEM2wid(int x)   { m_nStripetaEM2wid = x; }
inline  void T2CaloConfig::set_nStripphiEM2wid(int x)   { m_nStripphiEM2wid = x; }
inline  void T2CaloConfig::set_nStripetaEM3wid(int x)   { m_nStripetaEM3wid = x; }
inline  void T2CaloConfig::set_nStripphiEM3wid(int x)   { m_nStripphiEM3wid = x; }

inline  void T2CaloConfig::set_nStripetaEM0nor(int x)   { m_nStripetaEM0nor = x; }
inline  void T2CaloConfig::set_nStripphiEM0nor(int x)   { m_nStripphiEM0nor = x; }
inline  void T2CaloConfig::set_nStripetaEM1nor(int x)   { m_nStripetaEM1nor = x; }
inline  void T2CaloConfig::set_nStripphiEM1nor(int x)   { m_nStripphiEM1nor = x; }
inline  void T2CaloConfig::set_nStripetaEM2nor(int x)   { m_nStripetaEM2nor = x; }
inline  void T2CaloConfig::set_nStripphiEM2nor(int x)   { m_nStripphiEM2nor = x; }
inline  void T2CaloConfig::set_nStripetaEM3nor(int x)   { m_nStripetaEM3nor = x; }
inline  void T2CaloConfig::set_nStripphiEM3nor(int x)   { m_nStripphiEM3nor = x; }


inline  void T2CaloConfig::set_nStripetaHAD1nar(int x)   { m_nStripetaHAD1nar = x; }
inline  void T2CaloConfig::set_nStripphiHAD1nar(int x)   { m_nStripphiHAD1nar = x; }
inline  void T2CaloConfig::set_nStripetaHAD2nar(int x)   { m_nStripetaHAD2nar = x; }
inline  void T2CaloConfig::set_nStripphiHAD2nar(int x)   { m_nStripphiHAD2nar = x; }
inline  void T2CaloConfig::set_nStripetaHAD3nar(int x)   { m_nStripetaHAD3nar = x; }
inline  void T2CaloConfig::set_nStripphiHAD3nar(int x)   { m_nStripphiHAD3nar = x; }

inline  void T2CaloConfig::set_nStripetaHAD1wid(int x)   { m_nStripetaHAD1wid = x; }
inline  void T2CaloConfig::set_nStripphiHAD1wid(int x)   { m_nStripphiHAD1wid = x; }
inline  void T2CaloConfig::set_nStripetaHAD2wid(int x)   { m_nStripetaHAD2wid = x; }
inline  void T2CaloConfig::set_nStripphiHAD2wid(int x)   { m_nStripphiHAD2wid = x; }
inline  void T2CaloConfig::set_nStripetaHAD3wid(int x)   { m_nStripetaHAD3wid = x; }
inline  void T2CaloConfig::set_nStripphiHAD3wid(int x)   { m_nStripphiHAD3wid = x; }

inline  void T2CaloConfig::set_nStripetaHAD1nor(int x)   { m_nStripetaHAD1nor = x; }
inline  void T2CaloConfig::set_nStripphiHAD1nor(int x)   { m_nStripphiHAD1nor = x; }
inline  void T2CaloConfig::set_nStripetaHAD2nor(int x)   { m_nStripetaHAD2nor = x; }
inline  void T2CaloConfig::set_nStripphiHAD2nor(int x)   { m_nStripphiHAD2nor = x; }
inline  void T2CaloConfig::set_nStripetaHAD3nor(int x)   { m_nStripetaHAD3nor = x; }
inline  void T2CaloConfig::set_nStripphiHAD3nor(int x)   { m_nStripphiHAD3nor = x; }

#endif
