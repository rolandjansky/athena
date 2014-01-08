/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyCofiti_H
#define MuonboyCofiti_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyCofiti

   this class gives access to reconstruction results

  @author samusog@cern.ch
  
  */

class MuonboyCofiti{
public:
    MuonboyCofiti();
   ~MuonboyCofiti();

public:
///////////////////////////////////

   int    GetISCI();
   double GetDZT1();
   double GetDS1 ();
   double GetDZT2();
   double GetDS2 ();
   double GetSMU ();
   double GetCFI ();
   double GetSFI ();
   double GetCFA ();
   double GetSFA ();
   double GetZ1  ();
   double GetS1  ();
   double GetT1  ();
   double GetX1  ();
   double GetY1  ();
   double GetZ2  ();
   double GetS2  ();
   double GetT2  ();
   double GetX2  ();
   double GetY2  ();
   
   void SetISCI(int    ISCIi);
   void SetDZT1(double DZT1i);
   void SetDS1 (double DS1i );
   void SetDZT2(double DZT2i);
   void SetDS2 (double DS2i );
   void SetSMU (double SMUi );
   void SetCFI (double CFIi );
   void SetSFI (double SFIi );
   void SetCFA (double CFAi );
   void SetSFA (double SFAi );
   void SetZ1  (double Z1i  );
   void SetS1  (double S1i  );
   void SetT1  (double T1i  );
   void SetX1  (double X1i  );
   void SetY1  (double Y1i  );
   void SetZ2  (double Z2i  );
   void SetS2  (double S2i  );
   void SetT2  (double T2i  );
   void SetX2  (double X2i  );
   void SetY2  (double Y2i  );
   
   
   int GetISENFI();
   int GetIFAST ();
   
   void SetISENFI(int ISENFIi);
   void SetIFAST (int IFASTi );
   
   
   double GetRCHAMA();
   double GetRCHAMI();
   double GetZCHAMA();
   double GetZCHAMI();
   
   void SetRCHAMA(double RCHAMAi);
   void SetRCHAMI(double RCHAMIi);
   void SetZCHAMA(double ZCHAMAi);
   void SetZCHAMI(double ZCHAMIi);
   
   
   double GetVTHETAPHI(int Ipara);
   double GetVPERIGEE(int Ipara);
   
   void SetVTHETAPHI(double VTHETAPHIi,int Ipara);
   void SetVPERIGEE (double VPERIGEEi ,int Ipara);
   

   double GetVDEB(int Ivdeb,int Ipara);

   void   SetVDEB (double VDEBi,int Ivdeb,int Ipara);
   
   
   double GetVEND(int Ipara);

private:
///////////////////////////////////

   static const int NotDef = -1;

};

#endif
