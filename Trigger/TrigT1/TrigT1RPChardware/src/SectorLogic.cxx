/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************//
//*                                                                          *//
//*            Original code from Andrea Salamon                             *//
//*                                                                          *//
//****************************************************************************//
//
// 20/01/2004     A. Nisati      First code cleaning + use of BaseObject
// 17/07/2009     A. Salamon     Added output from all BCs + fixed one bug
//                               (mask for bcid output in SectorLogic::output)
// 21/02/2011     M. Corradi &   Fixed overlap resolution algorithm for new
//                A. Salamon     RoI geometry
//
//****************************************************************************//
//
#include "TrigT1RPChardware/SectorLogic.h"

using namespace std;

//
// Some usefull functions
//****************************************************************************//
// returns bits from lsb to msb right aligned
CMAword getbits(CMAword x, int msb, int lsb) {
  return ~(~0u << (msb - lsb + 1)) & (x >> lsb);
}
//****************************************************************************//
// returns bits from lsb to msb right aligned
// sets bits between lsb and msb of x to first (msb - lsb + 1) right 
// bits of y, remaining bits are left unchanged
unsigned setbits(unsigned x, int msb, int lsb, unsigned y) {
  unsigned mask=0u;
  mask = (~0u << (msb + 1)) | ~(~0u << lsb);
  return (x & mask) | (y << lsb);
}
//****************************************************************************//
// returns bits from lsb to msb right aligned
// ### DataFromPad ###

// constructor of the class
// sets all the fields to 0
DataFromPad::DataFromPad(void) {
  bcid = 0;
  r = 0;
  oveta = 0;
  ovphi = 0;
  opl = 0;
  pt = 0;
  roi = 0;
  ntrig = 0;
  sign = 0;
}
//****************************************************************************//
// prints the mask for DataFromPad
ostream &dfpa(ostream &stream, int indent, int whitesp, int ntimes) {
  int iind=0,iwhit=0,itime=0;
  for(iind=0;iind<=indent-1;iind++) stream << " ";
  for(itime=0;itime<=ntimes-1;itime++) {
    for(iwhit=0;iwhit<=whitesp-1;iwhit++) stream << " ";
    stream << "b o r o o p m n s";
  }
  stream << endl;
  for(iind=0;iind<=indent-1;iind++) stream << " ";
  for(itime=0;itime<=ntimes-1;itime++) {
    for(iwhit=0;iwhit<=whitesp-1;iwhit++) stream << " ";
    stream << "c p s e f t t t g";
  }
  stream << endl;
  return stream;
}
//****************************************************************************//
// overload of the << operator for DataFromPad
ostream &operator<<(ostream &stream, DataFromPad o) {
  stream.width(3);
  stream.fill('x');
  stream << o.bcid << " ";
  stream.width(1);
  stream.fill(' ');
  stream << o.opl << " ";
  stream << o.r << " ";
  stream << o.oveta << " ";
  stream << o.ovphi << " ";
  stream << o.pt << " ";
  stream << o.roi << " ";
  stream << o.ntrig << " ";
  stream << o.sign;
  return stream;
}
//****************************************************************************//
// ### OutputFromSectorLogic ###

// constructor of the class
// sets all the fields to 0
OutputFromSectorLogic::OutputFromSectorLogic(void) {
  ntrig = 0;
  roi1 = 0;
  pad1 = 0;
  r1 = 0;
  ovf1 = 0;
  ove1 = 0;
  roi2 = 0;
  pad2 = 0;
  r2 = 0;
  ovf2 = 0;
  ove2 = 0;
  pt1 = 0;
  pt2 = 0;
  ntrig1 = 0;
  ntrig2 = 0;
  bcid = 0;
  sign1 = 0;
  sign2 = 0;
}
//****************************************************************************//
// prints the mask for OutputFromSectorLogic
ostream &ofsla(ostream &stream, int indent, int whitesp, int ntimes) {
  int iind=0,iwhit=0,itime=0;
  for(iind=0;iind<=indent-1;iind++) stream << " ";
  for(itime=0;itime<=ntimes-1;itime++) {
    for(iwhit=0;iwhit<=whitesp-1;iwhit++) stream << " ";
    stream << "ss  bnnppoorproorprn";
  }
  stream << endl;
  for(iind=0;iind<=indent-1;iind++) stream << " ";
  for(itime=0;itime<=ntimes-1;itime++) {
    for(iwhit=0;iwhit<=whitesp-1;iwhit++) stream << " ";
    stream << "gg  cttttefsdiefsdit";
  }
  stream << endl;
  for(iind=0;iind<=indent-1;iind++) stream << " ";
  for(itime=0;itime<=ntimes-1;itime++) {
    for(iwhit=0;iwhit<=whitesp-1;iwhit++) stream << " ";
    stream << "21   21212222211111 ";
  }
  stream << endl;
  return stream;
}
//****************************************************************************//
// overload of the << operator 
ostream &operator<<(ostream &stream, OutputFromSectorLogic &o) {
  //    stream << "x x ";
  stream.setf(ios::dec, ios::basefield);
  //  stream << setw(3);
  stream << o.sign2;
  stream << o.sign1;
  stream.width(3);
  stream.fill('x');
  stream << o.bcid;
  stream.width(1);
  stream.fill(' ');
  stream << o.ntrig2;
  stream << o.ntrig1;
  stream << o.pt2;
  stream << o.pt1;
  stream << o.ove2;
  stream << o.ovf2;
  stream << o.r2;
  stream << o.pad2;
  stream << o.roi2;
  stream << o.ove1;
  stream << o.ovf1;
  stream << o.r1;
  stream << o.pad1;
  stream << o.roi1;
  stream << o.ntrig;
  return stream;
}
//****************************************************************************//

// ### InternalRegister ###

// standard constructor of the class
InternalRegister::InternalRegister(void) {

}

// overload of the << operator 
ostream &operator<<(ostream &stream, InternalRegister &o) {
  int j=0;
  for(j=0;j<=7;j++) {
    stream << "pad[" << j << "] : ";
    stream << o.pad[j] << endl;
  }
  stream.setf(ios::hex, ios::basefield);
  stream << "tile   : " << o.tile << endl;
  stream << "sl out : " << o.out;    
  stream.setf(ios::hex, ios::basefield);
  return stream;
}
//****************************************************************************//
// ### SectorLogic ###

// standard constructor of the class
SectorLogic::SectorLogic(int run, int event, CMAword debug, ubit16 subsys, 
                         ubit16 sect, bool oldSimulation) : BaseObject(Hardware,"SectorLogic") {

  // identificazione sector logic di aleandro
  m_run = run;
  m_event = event;
  m_debug = debug;
  m_subsys = subsys;
  m_sect = sect;
  m_oldSimulation = oldSimulation;
  
  //
  // define nBunMax
  //
  nBunMax = NOBXS;
  // reset TC and OPL check parameters
  // EnableTCCheck and EnableOPLCheck
  EnableTCCheckLow  = 0x00000000;
  EnableTCCheckHigh = 0x00000000;
  EnableOPLCheck    = 0x00000000;
  // SetTCCheck and SetOPLCheck
  int j=0;
  int k=0;
  for(j=0;j<=7;j++) {
    // low pT
    for(k=0;k<=2;k++) {
      SetTCCheck[j][k]=0;
      SetOPLCheck[j][k]=0;
    }
    // high pT
    for(k=3;k<=5;k++) {
      SetTCCheck[j][k]=0;
    }
  }

}
//****************************************************************************//
// destructor of the class
SectorLogic::~SectorLogic(void) {
  
}
//****************************************************************************//

CMAword  SectorLogic::outputToMuCTPI(int deltaBC) {
    int bunchID=BCZERO+deltaBC;
    if( bunchID < nBunMax && bunchID >=0 ) {
        ubit16 bxsafe = (ubit16)bunchID;
        return  output(bxsafe);
    } else {
        DISP << "warning : bunchID out of range, " << bunchID ;
        DISP_WARNING;
        return 0;
    }
}

// OLD VERSION
/*
CMAword SectorLogic::outputToMuCTPI(ubit16 bunchID) {
  ubit16 bxsafe=BCZERO;
  if( bunchID <= nBunMax-1 ) {
    bxsafe=bunchID;
  } else {
      DISP << "warning : bunchID out of range, set to default value" << bxsafe ;
    DISP_WARNING;
  }
  return output(bxsafe);
}
*/

// initializes the array
void SectorLogic::init(void) {

}
//****************************************************************************//
// @@@@@ check the internal registers of the Sector Logic
void SectorLogic::check(void) {

  int bcid;

  for(bcid=0;bcid<=nBunMax-1;bcid++) {

    DISP << "LowPtFilter_in BCID is " << LowPtFilter_in[bcid].out.bcid << endl
         << "LowPtFilter_out BCID is " << LowPtFilter_out[bcid].out.bcid << endl

         << "TileCalConfirm_in BCID is " << TileCalConfirm_in[bcid].out.bcid << endl
         << "TileCalConfirm_out BCID is " << TileCalConfirm_out[bcid].out.bcid << endl

         << "SolveEtaOverlap_in BCID is " << SolveEtaOverlap_in[bcid].out.bcid << endl
         << "SolveEtaOverlap_out BCID is " << SolveEtaOverlap_out[bcid].out.bcid << endl

         << "SortHighest_in BCID is " << SortHighest_in[bcid].out.bcid << endl
         << "SortHighest_out BCID is " << SortHighest_out[bcid].out.bcid << endl

         << "Sort2ndHighest_in BCID is " << Sort2ndHighest_in[bcid].out.bcid << endl
         << "Sort2ndHighest_out BCID is " << Sort2ndHighest_out[bcid].out.bcid << endl;
     DISP_DEBUG;

  }

}
//****************************************************************************//
// tile cal check configuration
void SectorLogic::LoadTCCheck(CMAword EnableTCCheckLow_in, 
                              CMAword EnableTCCheckHigh_in, 
			      CMAword SetTCCheck_in[8][6]) {
  EnableTCCheckLow=EnableTCCheckLow_in;
  EnableTCCheckHigh=EnableTCCheckHigh_in;
  int i=0;
  int j=0;
  for(i=0;i<=7;i++) {
    for(j=0;j<=5;j++) {
      SetTCCheck[i][j]=SetTCCheck_in[i][j];
    }
  }
}
//****************************************************************************//
// opl check configuration
void SectorLogic::LoadOPLCheck(CMAword EnableOPLCheck_in, 
                               ubit16 SetOPLCheck_in[8][3]) {
  EnableOPLCheck=EnableOPLCheck_in;
  int i=0;
  int j=0;
  for(i=0;i<=7;i++) {
    for(j=0;j<=2;j++) {
      SetOPLCheck[i][j]=SetOPLCheck_in[i][j];
    }
  }
}
//****************************************************************************//
// @@@@@ input, output and clock methods @@@@@
void SectorLogic::dbginput(ubit16 bx, DataFromPad from_pad[8], 
                           CMAword from_tile_cal){

  //  int BCID_now = bx%8;

  int ipad=0;
  for(ipad=0;ipad<=7;ipad++) {
    InFromPad[bx][ipad]=from_pad[ipad];
  }
  InFromTileCal[bx]=from_tile_cal;

}
//****************************************************************************//
void SectorLogic::load(ubit16 padAdd, ubit16 BX, ubit16 RoIAdd, ubit16 pT, 
                       ubit16 OPL, ubit16 overlapPhi, ubit16 overlapEta, 
		       ubit16 RoiAmbiguity, ubit16 BCIDcounter){

  InFromPad[BX][padAdd].bcid=BCIDcounter;
  InFromPad[BX][padAdd].r=RoiAmbiguity;
  InFromPad[BX][padAdd].oveta=overlapEta;
  InFromPad[BX][padAdd].ovphi=overlapPhi;
  InFromPad[BX][padAdd].opl=OPL;
  InFromPad[BX][padAdd].pt=pT;
  InFromPad[BX][padAdd].roi=RoIAdd;

  InFromTileCal[BX]=0xff;

  /*
  DISP << "input from pad : BC = " << BX << " padAdd = " << padAdd
       << " pT = "<<  pT << " roi = " << RoIAdd << " bcid = " << BCIDcounter;
  DISP_DEBUG;
  */
}
//****************************************************************************//
OutputFromSectorLogic SectorLogic::dbgoutput(ubit16 i){
  return OutFromSectorLogic[i];
}
//****************************************************************************//
CMAword SectorLogic::output(ubit16 i){
  CMAword fmtout=0;

  ubit16 slroi1=0;
  ubit16 slroi2=0;

  if (m_oldSimulation)  {
      if(OutFromSectorLogic[i].pt1) // aleandro addendum 6-10-2003
          slroi1 = (OutFromSectorLogic[i].pad1)*4 + (OutFromSectorLogic[i].roi1) + 1;
      if(OutFromSectorLogic[i].pt2) // aleandro addendum 6-10-2003
          slroi2 = (OutFromSectorLogic[i].pad2)*4 + (OutFromSectorLogic[i].roi2) + 1;
  } else {
      if(OutFromSectorLogic[i].pt1) // aleandro addendum 6-10-2003
          slroi1 = (OutFromSectorLogic[i].pad1)*4 + (OutFromSectorLogic[i].roi1);
      if(OutFromSectorLogic[i].pt2) // aleandro addendum 6-10-2003
          slroi2 = (OutFromSectorLogic[i].pad2)*4 + (OutFromSectorLogic[i].roi2);      
 }

  // MC 2015/7/7 add bc information
  ubit16 bc = 0;
  if (i>BCZERO) {
      bc = i-BCZERO;
  } else if (i<BCZERO) {
      bc = i+8-BCZERO;
  }
  

  if (OutFromSectorLogic[i].pt1==0) OutFromSectorLogic[i].pt1=7;
  if (OutFromSectorLogic[i].pt2==0) OutFromSectorLogic[i].pt2=7;

  fmtout = fmtout | (((OutFromSectorLogic[i].ntrig>2) & 0x01) <<  0); // >2 candidates in a sector
  fmtout = fmtout | (( slroi1                         & 0x1f) <<  1); // ROI1
  fmtout = fmtout | (( 0                              & 0x03) <<  6); // Reserved1
  fmtout = fmtout | (( OutFromSectorLogic[i].ovf1     & 0x01) <<  8); // Phi overlap1
  fmtout = fmtout | (( OutFromSectorLogic[i].ove1     & 0x01) <<  9); // Eta overlap1
  fmtout = fmtout | (( slroi2                         & 0x1f) << 10); // ROI2
  fmtout = fmtout | (( 0                              & 0x03) << 15); // Reserved2
  fmtout = fmtout | (( OutFromSectorLogic[i].ovf2     & 0x01) << 17); // Phi overlap2
  fmtout = fmtout | (( OutFromSectorLogic[i].ove2     & 0x01) << 18); // Eta overlap2
  fmtout = fmtout | (( OutFromSectorLogic[i].pt1      & 0x07) << 19); // Pt1
  fmtout = fmtout | (( OutFromSectorLogic[i].pt2      & 0x07) << 22); // Pt2
  fmtout = fmtout | (( OutFromSectorLogic[i].ntrig1   & 0x01) << 25); // >1 candidate in ROI1
  fmtout = fmtout | (( OutFromSectorLogic[i].ntrig2   & 0x01) << 26); // >1 candidate in ROI2
  fmtout = fmtout | (( bc                             & 0x07) << 27); // BCID
  fmtout = fmtout | (( OutFromSectorLogic[i].sign1    & 0x01) << 30); // Candidate1 sign
  fmtout = fmtout | (( OutFromSectorLogic[i].sign2    & 0x01) << 31); // Candidate2 sign

  return fmtout;

}
//****************************************************************************//
void SectorLogic::execute(){

  // executes sector logic algorithm for all input bunches
  int ibx=0;
  for (ibx=0; ibx<=nBunMax-1; ibx++) {

    // the content of the SL input registers are copied in the 1st input 
    // register
    int ipad=0;
    for(ipad=0;ipad<=7;ipad++) {
      LowPtFilter_in[ibx].pad[ipad] = InFromPad[ibx][ipad];
    }
    LowPtFilter_in[ibx].tile = InFromTileCal[ibx];

    // @@@@@ 1st step registers
    // @@@@@ low Pt filter
    // if there is a low Pt track in the pad and the OPL check option is on
    // looks for OPL confirmation in the other pads mapped for OPL check
    LowPtFilter_in[ibx].out.bcid = LowPtFilter_in[ibx].pad[0].bcid;
    LowPtFilter_out[ibx] = LowPtFilter_in[ibx];
    int i1=0;
    // external cycle on the pads
    int OPLfrompads=0;
    for(i1=0;i1<=7;i1++) {
      OPLfrompads=setbits(OPLfrompads,i1,i1,LowPtFilter_in[ibx].pad[i1].opl);
    }
    // external cycle on the pads
    for(i1=0;i1<=7;i1++) {
      // check for low Pt track in the pad
      int Pt_reg1=0;
      Pt_reg1=LowPtFilter_in[ibx].pad[i1].pt;
      if( 0<Pt_reg1 && Pt_reg1<=3 ) {
	// check for OPL check option for the given pad (depends on track Pt!!)
	if( getbits(EnableOPLCheck,i1*4+Pt_reg1,i1*4+Pt_reg1) == 1 ) {
	  int OPLCheck=0;
	  // check for OPL confirmation in all mapped pads
	  OPLCheck = OPLfrompads & SetOPLCheck[i1][Pt_reg1-1];
	  // if the Pt track were not confirmed the track Pt is reset to 0
	  if(OPLCheck==0) LowPtFilter_out[ibx].pad[i1].pt = 0;
	}
      }    
    }


    // @@@@@ 2nd step registers
    // @@@@@ tile cal confirmation
    // if there is track in the pad and the tile cal confirmation option is on
    // looks for the tile cal confirmation in the mapped tile cal signals
    TileCalConfirm_in[ibx] = LowPtFilter_out[ibx];
    TileCalConfirm_out[ibx] = TileCalConfirm_in[ibx];
    int j1=0;
    // external cycle on the pads
    for(j1=0;j1<=7;j1++) {
      // check for low Pt track in the pad
      int Pt_reg2=0;
      Pt_reg2=TileCalConfirm_in[ibx].pad[j1].pt;
      if( 0<Pt_reg2 && Pt_reg2<=6 ) {
	// check for TileCal confirmation option for the given pad 
	// (depends on track Pt !!)
	// chose EnableTCCheck depending on pT
	sbit32 EnableTCCheck=0;
	if(0<Pt_reg2&&Pt_reg2<=3) {
	  EnableTCCheck=EnableTCCheckLow;
	}
	else {
	  EnableTCCheck=EnableTCCheckHigh;
	}
	if( getbits(EnableTCCheck,j1*4+Pt_reg2,j1*4+Pt_reg2) == 1 ) {
	  int TileCalCheck=0;
	  // check for TileCal confirmation in all the pads mapped
	  TileCalCheck = TileCalConfirm_in[ibx].tile & SetTCCheck[j1][Pt_reg2-1];
	  // if the Pt track has not been confirmed the track Pt is reset to 0
	  if(TileCalCheck==0) {
	    TileCalConfirm_out[ibx].pad[j1].pt = 0;
	  }
	}
      }    
    }


    // @@@@@ 3rd step registers
    // @@@@@ solve eta overlaps within sector
    /* ALGORITMO DI SELEZIONE DELLE SOGLIE:
       TRA DUE TRACCE IN OVERLAP PASSA LA TRACCIA CON SOGLIA PIU' ALTA
       A PARITA' DI SOGLIA PASSA QUELLA CON ETA MINORE */
    SolveEtaOverlap_in[ibx] = TileCalConfirm_out[ibx];
    SolveEtaOverlap_out[ibx] = SolveEtaOverlap_in[ibx];
    // first check : if the overlap flag is on there must be a valid track 
    // in the pad, otherwise send a warning
    int k1=0;
    for(k1=0;k1<=7;k1++) {
      if( SolveEtaOverlap_out[ibx].pad[k1].oveta &&  SolveEtaOverlap_out[ibx].pad[k1].pt==0 ) {
	DISP << "warning :\n"
	     << "pad # " << k1 << " bcid # " << ibx << " has eta overlap flag on but no triggered track\n";
	DISP_DEBUG;
      }
    }
    // run the overlap resolution algorithm on EVEN and then on ODD pads
    int kk=0;
    for(kk=0;kk<=1;kk++) {
      int k3=0;
      for(k3=kk;k3<=6-kk;k3=k3+2) {
	if( SolveEtaOverlap_out[ibx].pad[k3].oveta || SolveEtaOverlap_out[ibx].pad[k3+1].oveta ) {
	  if( SolveEtaOverlap_out[ibx].pad[k3].oveta && SolveEtaOverlap_out[ibx].pad[k3+1].oveta ) {
	    if( (SolveEtaOverlap_out[ibx].pad[k3].roi==2 && SolveEtaOverlap_out[ibx].pad[k3+1].roi==3) || 
		(SolveEtaOverlap_out[ibx].pad[k3].roi==0 && SolveEtaOverlap_out[ibx].pad[k3+1].roi==1) ||
		(SolveEtaOverlap_out[ibx].pad[k3].roi==3 && SolveEtaOverlap_out[ibx].pad[k3+1].roi==2) || 
		(SolveEtaOverlap_out[ibx].pad[k3].roi==1 && SolveEtaOverlap_out[ibx].pad[k3+1].roi==0) ) {
	      // set to 0 the lower Pt
	      if( SolveEtaOverlap_out[ibx].pad[k3].pt>=SolveEtaOverlap_out[ibx].pad[k3+1].pt ) {
		SolveEtaOverlap_out[ibx].pad[k3+1].pt = 0;
		SolveEtaOverlap_out[ibx].pad[k3+1].oveta = 0;
	      }
	      else {
		SolveEtaOverlap_out[ibx].pad[k3].pt = 0;
		SolveEtaOverlap_out[ibx].pad[k3].oveta = 0;
	      }
	    }
	    else {
	      DISP << "warning :\n"
		   << "pads " << k3 << " and " << k3+1 << " have eta overlap flags on with wrong RoIs\n";
	      DISP_DEBUG;
	    }
	  }
	}
      }
    }


    // @@@@@ 4th step registers
    // @@@@@ sort highest track
    SortHighest_in[ibx] = SolveEtaOverlap_out[ibx];
    SortHighest_out[ibx] = SortHighest_in[ibx];
    int Pt_reg4=0;
    int ROI_reg4=0;
    int pad_reg4=0;
    int l1=0;
    // external cycle on the pads
    for(l1=0;l1<=7;l1++) {
      // check for a track with higher Pt
      if( SortHighest_in[ibx].pad[l1].pt>Pt_reg4 ) {
	Pt_reg4 = SortHighest_in[ibx].pad[l1].pt;
	ROI_reg4 = SortHighest_in[ibx].pad[l1].roi;
	pad_reg4=l1;
      }
    }
    // if there is a validated track
    if(Pt_reg4>0) {
      // put the result in the output part of the register
      SortHighest_out[ibx].out.pt1 = Pt_reg4;
      SortHighest_out[ibx].out.pad1 = pad_reg4;
      SortHighest_out[ibx].out.roi1 = ROI_reg4;
      SortHighest_out[ibx].out.ovf1 = SortHighest_in[ibx].pad[pad_reg4].ovphi;
      SortHighest_out[ibx].out.ove1 = SortHighest_in[ibx].pad[pad_reg4].oveta;
      SortHighest_out[ibx].out.ntrig1 = SortHighest_in[ibx].pad[pad_reg4].ntrig;
      SortHighest_out[ibx].out.sign1 = SortHighest_in[ibx].pad[pad_reg4].sign;
      // set to 1 the number of valid tracks
      SortHighest_out[ibx].out.ntrig = 1;
      // and put the ouput internal register to 0
      // otherwise the track will be counted twice !!!!!
      SortHighest_out[ibx].pad[pad_reg4].pt = 0;
      SortHighest_out[ibx].pad[pad_reg4].roi = 0;
    }

 
    // @@@@@ 5th step registers
    // @@@@@ sort 2nd highest track
    Sort2ndHighest_in[ibx] = SortHighest_out[ibx];
    Sort2ndHighest_out[ibx] = Sort2ndHighest_in[ibx];
    int Pt_reg5=0;
    int ROI_reg5=0;
    int pad_reg5=0;
    int m1=0;
    // external cycle on the pads
    for(m1=0;m1<=7;m1++) {
      // check for a track with 2nd higer Pt
      if( Sort2ndHighest_in[ibx].pad[m1].pt>Pt_reg5 && 
            (Sort2ndHighest_in[ibx].pad[m1].pt!=Pt_reg4 || 
	      Sort2ndHighest_in[ibx].pad[m1].roi!=ROI_reg4 ||
	      m1!=pad_reg4 ) ) {
	Pt_reg5 = Sort2ndHighest_in[ibx].pad[m1].pt;
	ROI_reg5 = Sort2ndHighest_in[ibx].pad[m1].roi;
	pad_reg5=m1;
      }
    }

    // if there is a validated track
    if(Pt_reg5>0) {
      // put the result in the output part of the register
      Sort2ndHighest_out[ibx].out.pt2 = Pt_reg5;
      Sort2ndHighest_out[ibx].out.pad2 = pad_reg5;
      Sort2ndHighest_out[ibx].out.roi2 = ROI_reg5;
      Sort2ndHighest_out[ibx].out.ovf2 = Sort2ndHighest_in[ibx].pad[pad_reg5].ovphi;
      Sort2ndHighest_out[ibx].out.ove2 = Sort2ndHighest_in[ibx].pad[pad_reg5].oveta;
      Sort2ndHighest_out[ibx].out.ntrig2 = Sort2ndHighest_in[ibx].pad[pad_reg5].ntrig;
      Sort2ndHighest_out[ibx].out.sign2 = Sort2ndHighest_in[ibx].pad[pad_reg5].sign;
      // set to 2 the number of valid tracks
      Sort2ndHighest_out[ibx].out.ntrig = 2;
      // and put the ouput internal register to 0
      // othrwise the track will be counted twice !!!!!
      Sort2ndHighest_out[ibx].pad[pad_reg5].pt = 0;
      Sort2ndHighest_out[ibx].pad[pad_reg5].roi = 0;
    }

    // check for other candidates in sector
    for(m1=0;m1<=7;m1++) {
      if( Sort2ndHighest_out[ibx].pad[m1].pt>0 ) (Sort2ndHighest_out[ibx].out.ntrig)++;
    }

    // the content of the 5th output internal register is copied in the SL output register
    OutFromSectorLogic[ibx] = Sort2ndHighest_out[ibx].out;

  }

}
//****************************************************************************//
// overload of the << operator 
ostream &operator<<(ostream &stream, SectorLogic &o) {

  stream << "@@@@@@@@@@ event and sector logic identification @@@@@@@@@@\n\n";

  stream << "run    = " << o.m_run << endl;
  stream << "event  = " << o.m_event << endl;
  stream << "debug  = " << o.m_debug << endl;
  stream << "subsys = " << o.m_subsys << endl;
  stream << "sect   = " << o.m_sect << endl;

  stream << endl;

  // print all the parameters of the sector logic board
  stream << "@@@@@@@@@@ sector logic configuration parameters @@@@@@@@@@\n\n";

  // tccheck
  stream.setf(ios::hex, ios::basefield);
  stream << "EnableTCCheckLow   : ";
  stream.width(8);
  stream.fill('0');
  stream << o.EnableTCCheckLow << endl;
  stream << "EnableTCCheckHigh  : ";
  stream.width(8);
  stream.fill('0');
  stream << o.EnableTCCheckHigh << endl;
  int jj=0;
  int kk=0;
  for(jj=0;jj<=7;jj++) {
    stream << "SetTCCheck pad[";
    stream << jj;
    stream << "]  : ";
    for(kk=5;kk>=0;kk--) {
      stream.width(8);
      stream.fill('0');
      stream << o.SetTCCheck[jj][kk] << "  ";
    }
    stream << endl;
  }
  stream << endl;

  // opl check
  stream << "EnableOPLCheck     : ";
  stream.width(8);
  stream.fill('0');
  stream << o.EnableOPLCheck << endl;
  for(jj=0;jj<=7;jj++) {
    stream << "SetOPLCheck pad[";
    stream << jj;
    stream << "] : ";
    for(kk=2;kk>=0;kk--) {
      stream.width(2);
      stream.fill('0');
      stream << o.SetOPLCheck[jj][kk] << "  ";
    }
    stream << endl;
  }
  stream << endl;

  stream.width(1);
  stream.fill(' ');

  // internal register (input register)
  InternalRegister* intreginp[5];
  intreginp[0] = o.LowPtFilter_in;
  intreginp[1] = o.TileCalConfirm_in;
  intreginp[2] = o.SolveEtaOverlap_in;
  intreginp[3] = o.SortHighest_in;
  intreginp[4] = o.Sort2ndHighest_in;
  // internal register (output register)
  InternalRegister* intregoutp[5];
  intregoutp[0] = o.LowPtFilter_out;
  intregoutp[1] = o.TileCalConfirm_out;
  intregoutp[2] = o.SolveEtaOverlap_out;
  intregoutp[3] = o.SortHighest_out;
  intregoutp[4] = o.Sort2ndHighest_out;

  int ibx=0;

  // print the input registers of the sector logic board  
  stream << "@@@@@@@@@@ sector logic input registers @@@@@@@@@@\n\n";

  // print input from pads
  stream.setf(ios::dec, ios::basefield);
  dfpa(stream,8,8,NOBXS);
  int ipad=0;
  for(ipad=0;ipad<=7;ipad++) {
    stream << "pad[" << ipad << "] :";
    for(ibx=0;ibx<=NOBXS-1;ibx++) {
      stream << "      ";
      stream << o.InFromPad[ibx][ipad];
    }
    stream << endl;
  }

  // print input from tilecal
  stream.setf(ios::hex, ios::basefield);
  stream << "tile   :";
  for(ibx=0;ibx<=NOBXS-1;ibx++) {
    stream << "                 ";
  stream.width(8);
  stream.fill('0');
    stream << o.InFromTileCal[ibx];
  }
  stream.width(1);
  stream.fill(' ');
  stream << endl << endl;

  // print all  sector logic internal registers
  stream << "@@@@@@@@@@ sector logic internal registers @@@@@@@@@@\n\n";

  int ireg=0;
  for(ireg=0;ireg<=4;ireg++) {

    // input registers
    stream << "internal registers # " << ireg+1 << " (input)" << endl;
    // DataFromPad
    stream.setf(ios::dec, ios::basefield);
    dfpa(stream,8,8,NOBXS);
    for(ipad=0;ipad<=7;ipad++) {
      stream << "pad[" << ipad << "] :";
      for(ibx=0;ibx<=NOBXS-1;ibx++) {
	stream << "      ";
	stream << (intreginp[ireg]+ibx)->pad[ipad];
      }
      stream << endl;
    }
    // Tile Cal
    stream.setf(ios::hex, ios::basefield);
    stream << "tile   :" ;
    for(ibx=0;ibx<=NOBXS-1;ibx++) {
      stream << "                 ";
  stream.width(8);
  stream.fill('0');
      stream << (intreginp[ireg]+ibx)->tile;
    }
  stream.width(1);
  stream.fill(' ');
    stream << endl;
    // Sector Logic Output
    stream.setf(ios::dec, ios::basefield);
    stream << "sl out :";
    for(ibx=0;ibx<=NOBXS-1;ibx++) {
      stream << "     ";
      stream << (intreginp[ireg]+ibx)->out;    
    }
    stream << endl;
    stream << endl;
	
    // output registers
    stream << "internal registers # " << ireg+1 << " (output)" << endl;
    // DataFromPad
    stream.setf(ios::dec, ios::basefield);
    dfpa(stream,8,8,NOBXS);
    for(ipad=0;ipad<=7;ipad++) {
      stream << "pad[" << ipad << "] :";
      for(ibx=0;ibx<=NOBXS-1;ibx++) {
	stream << "      ";
	stream << (intregoutp[ireg]+ibx)->pad[ipad];
      }
      stream << endl;
    }
    // Tile Cal
    stream.setf(ios::hex, ios::basefield);
    stream << "tile   :";
    for(ibx=0;ibx<=NOBXS-1;ibx++) {
      stream << "                 ";
  stream.width(8);
  stream.fill('0');
      stream << (intregoutp[ireg]+ibx)->tile;
    }
  stream.width(1);
  stream.fill(' ');
    stream << endl;
    // Sector Logic Output
    stream.setf(ios::dec, ios::basefield);
    stream << "sl out :";
    for(ibx=0;ibx<=NOBXS-1;ibx++) {
      stream << "     ";
      stream << (intregoutp[ireg]+ibx)->out;    
    }
    stream << endl << endl;

  }

  // print the output registers of the sector logic board
  stream.setf(ios::dec, ios::basefield);
  stream << "@@@@@@@@@@ sector logic output register @@@@@@@@@@\n\n";
  ofsla(stream,8,5,NOBXS);
  stream << "        ";
  for(ibx=0;ibx<=NOBXS-1;ibx++) {
    stream << "     ";
    stream << o.OutFromSectorLogic[ibx];
  }

  return stream;
  
}
//****************************************************************************//
