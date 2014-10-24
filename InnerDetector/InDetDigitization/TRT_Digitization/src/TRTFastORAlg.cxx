/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTFastORAlg.h"

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/TRT_ID.h"

#include <iostream>
#include <sstream>
#include "TRT_Digitization/TRTFastOR.Patterns"
#include <math.h>

//___________________________________________________________________________
TRTFastORAlg::TRTFastORAlg ( ):
  m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc", "TRTFastOR"),
  tHistSvc( "THistSvc", "TRTFastOR" ),  // GP
  m_ntupleFileName("AANT"),  // GP
  m_ntupleTreeName("FastOR"),  // GP
  m_nt(0)  // GP
{
}

StatusCode TRTFastORAlg::initialize()
{
  m_eventNumber = -999;
  
  // ---------------------------
  // retrive pointer to THistSvc
  ATH_CHECK( tHistSvc.retrieve() );
  // ---------------------------
  // create tree and register it to THistSvc
  m_nt = new TTree(TString(m_ntupleTreeName), "FastOR");
  // NB: we must not delete the tree, this is done by THistSvc
  std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleTreeName;
  ATH_CHECK( tHistSvc->regTree(fullNtupleName, m_nt) );
  //-----------------
  // add items
  // Event number
  m_nt->Branch("EventNumber",     &m_eventNumber, "event_number/I");
  // number of chips with HT activity per FEB
  m_nt->Branch("nChips",    &m_nChips, "nChips[2][32][29][24]/b"  );
  m_nt->Branch("isHIP",     &m_isHIP,  "isHIP[2][32][29]/b"  );
  m_nt->Branch("isEle",     &m_isEle,  "isEle[2][32][29]/b"  );
  m_nt->Branch("isMuo",     &m_isMuo,  "isMuo[2][32][29]/b"  );


  return StatusCode::SUCCESS;
}

TRTFastORAlg::~TRTFastORAlg () { }

int TRTFastORAlg::TRTFastORAlg_getStrawId(std::vector<std::vector<int> >& m_fastOrHtDiscriminator,
					  std::vector<int>& m_chipIdVector,
					  std::vector<int>& m_phiIdVector,
					  std::vector<int>& barrel_ec_vector, 
					  std::vector<unsigned short>& m_particleFlag, 
					  int m_qval, int m_preproc)
{
  
  m_eventNumber = 1; //Fix me! does TRTDigitizationTool.cxx even access the actual event number
                     // or can we just use an event counter?
  
  

  // /////////////////////////////////////////////////////////////////////
  //                        Initialize arrays                         //
  // ///////////////////////////////////////////////////////////////////

  // This 3D array stores the number of chips per FE board that have HT hits
  static int  FEboard[3][2][32][29];
  // This 3D arrays store the number of chips per FE board that are crossed by HIPs/ele/mu
  static int  FEboard_isHIP[2][32][29];
  static int  FEboard_isEle[2][32][29];
  static int  FEboard_isMuo[2][32][29];

  // This 4D array stores the HT bits information in 24 bins, 
  // each equivalent to 3.125 ns "time". This array is used for 
  // preprocessing. 
  static int m_preprocessing_straw_HT_ternaryInfo[2][32][344][24];
  // This 3D array is used to store the chips with HT hits  
  static int Chippos[2][32][344][24];

  // These 3D arrays are used to store the chips that are crossed by HIPs/ele/mu 
  static int Chippos_isHIP[2][32][344];
  static int Chippos_isEle[2][32][344];
  static int Chippos_isMuo[2][32][344];

  // array stores the number of chips with HT bits high (3.125 ns "time") 
  // per FE board. This array is used for preprocessing. 
  static int m_preprocessing_Nchips_per3125[2][32][29][24];

  //vector of 29 FEboards for each bunch crossing
  static int phiStack[29][3];

  // Initialize all the arrays to zero - this means they can be static and the memory is allocated once!
  for ( int side = 0; side < 2; side++ ){
    for ( int phi = 0; phi < 32; phi++ ){
      for ( int chip = 0; chip < 344; chip ++ ){
	if ( chip < 29 ){
	  // using chip variable to Initialize FEB
	  for(int bx=0; bx<3; bx++){
	    FEboard[bx][side][phi][chip] = 0;
	  }
	  FEboard_isHIP[side][phi][chip] = 0;
          FEboard_isEle[side][phi][chip] = 0;
          FEboard_isMuo[side][phi][chip] = 0;

	  for (int bin = 0; bin < 24; bin++ ){
	    m_preprocessing_Nchips_per3125[side][phi][chip][bin] = 0;
	  } // close loop over "time" bins 
	}
	for (int bin = 0; bin < 24; bin++ ){
	  Chippos[side][phi][chip][bin] = 0;
	  m_preprocessing_straw_HT_ternaryInfo[side][phi][chip][bin] = 0;  
	}
        Chippos_isHIP[side][phi][chip] = 0;
        Chippos_isEle[side][phi][chip] = 0;
        Chippos_isMuo[side][phi][chip] = 0;
      } // close loop over chips  
    } // close loop over phi 
  } // close loop over side

  // /////////////////////////////////////////////////////////////////////
  //                      Fill the arrays                             //
  // ///////////////////////////////////////////////////////////////////
  for (unsigned int i = 0; i < m_chipIdVector.size(); i++ )
    {
      int chip = m_chipIdVector.at(i);
      int phi  = m_phiIdVector.at(i);
      int side = barrel_ec_vector.at(i);
      
      Chippos_isHIP[side][phi][chip] |=  ((m_particleFlag.at(i)>>15) & 0x1);
      // Electron or positron with E>10 GeV
      Chippos_isEle[side][phi][chip] |= 
	(((m_particleFlag.at(i)>>5) & 0x1) | ((m_particleFlag.at(i)>>6) & 0x1) );
      // Muon with E>10 GeV
      Chippos_isMuo[side][phi][chip] |=  ((m_particleFlag.at(i)>>8) & 0x1);


      // Fixme: .at() is slow, do we need it here?
      std::vector<int> straw_HT_ternaryInfo = m_fastOrHtDiscriminator.at(i);
      int ToHT = straw_isHIPcandidate(straw_HT_ternaryInfo);

      if (ToHT > 2 ) {
	// straw_HT_ternaryInfo: 1 element (bin) = 0.78125 ns. There are 160 bins for each HT-straw vector.
	// Only using the last 96 bins where the information for the 75ns (3 BC) is stored.
	int thebin = 0;
	int bin_i = 0;
	for (int bin=0; bin< 24; bin++ )
	  {
	    int newbin = 0;
	    for (int j=0; j <4; j++ )
	      {
		thebin = 64 + bin_i;
		bin_i++;
		if (straw_HT_ternaryInfo.at(thebin) == 1) {
		  newbin = 1;
		}
	      }
	    m_preprocessing_straw_HT_ternaryInfo[side][phi][chip][bin] |= newbin;
	    Chippos[side][phi][chip][bin] |= newbin;
	  }
      }
    }
  
  // Fixme?: For the moment loop over all the chips for debugging purposes,
  // Likely to go back to old approach of only looping over chips with HT, TBC
  // // Count chips for preprocessing
  //   for (unsigned int i = 0; i < m_chipIdVector.size(); i++ )
  //     {
  //       // Fixme: .at() is slow - do we need it here?
  //       std::vector<int> straw_HT_ternaryInfo = m_fastOrHtDiscriminator.at(i);
  //       int chip = m_chipIdVector.at(i);
  //       int phi  = m_phiIdVector.at(i);
  //       int side = barrel_ec_vector.at(i);
  //       int ToHT = straw_isHIPcandidate(straw_HT_ternaryInfo);

  for(int side=0; side<2;side++){
    for(int phi=0; phi<32; phi++){
      for(int chip=0; chip<344; chip++){
	
	// Fixme: to add the ToHT condition
	//if (ToHT > 2 ) {
	if (1) {
	  for (int bin = 0; bin < 24; bin++ )
	    {
	      // Saving some time?
	      if(Chippos[side][phi][chip][bin]==0) continue;
	      if ( (chip >= 0)  && (chip <= 9)  )   { // 10 chips
		m_preprocessing_Nchips_per3125[side][phi][0][bin] += Chippos[side][phi][chip][bin];
                if(bin==0){
                  FEboard_isHIP[side][phi][0] += Chippos_isHIP[side][phi][chip];
                  FEboard_isEle[side][phi][0] += Chippos_isEle[side][phi][chip];
                  FEboard_isMuo[side][phi][0] += Chippos_isMuo[side][phi][chip];
                }

	      }
	      if ( (chip >= 10)  && (chip <= 20)  ) { // 11 chips
		m_preprocessing_Nchips_per3125[side][phi][1][bin] += Chippos[side][phi][chip][bin];
		if(bin==0){
                  FEboard_isHIP[side][phi][1] += Chippos_isHIP[side][phi][chip];
                  FEboard_isEle[side][phi][1] += Chippos_isEle[side][phi][chip];
                  FEboard_isMuo[side][phi][1] += Chippos_isMuo[side][phi][chip];
                }
	      }
	      if ( (chip >= 21)  && (chip <= 35)  ) { // 15 chips
		m_preprocessing_Nchips_per3125[side][phi][2][bin] += Chippos[side][phi][chip][bin];
		if(bin==0){
                  FEboard_isHIP[side][phi][2] += Chippos_isHIP[side][phi][chip];
                  FEboard_isEle[side][phi][2] += Chippos_isEle[side][phi][chip];
                  FEboard_isMuo[side][phi][2] += Chippos_isMuo[side][phi][chip];
                }
	      }
	      if ( (chip >= 36)  && (chip <= 44)  ) { // 9 chips
		m_preprocessing_Nchips_per3125[side][phi][3][bin] += Chippos[side][phi][chip][bin];
		if(bin==0){
                  FEboard_isHIP[side][phi][3] += Chippos_isHIP[side][phi][chip];
                  FEboard_isEle[side][phi][3] += Chippos_isEle[side][phi][chip];
                  FEboard_isMuo[side][phi][3] += Chippos_isMuo[side][phi][chip];
                }
	      }
	      if ( (chip >= 45)  && (chip <= 53)  ) { // 9 chips
		m_preprocessing_Nchips_per3125[side][phi][4][bin] += Chippos[side][phi][chip][bin];
		if(bin==0){
                  FEboard_isHIP[side][phi][4] += Chippos_isHIP[side][phi][chip];
                  FEboard_isEle[side][phi][4] += Chippos_isEle[side][phi][chip];
                  FEboard_isMuo[side][phi][4] += Chippos_isMuo[side][phi][chip];
                }
	      }
	      if ( (chip >= 54)  && (chip <= 64)  ) { // 11 chips
		m_preprocessing_Nchips_per3125[side][phi][5][bin] += Chippos[side][phi][chip][bin];
		if(bin==0){
                  FEboard_isHIP[side][phi][5] += Chippos_isHIP[side][phi][chip];
                  FEboard_isEle[side][phi][5] += Chippos_isEle[side][phi][chip];
                  FEboard_isMuo[side][phi][5] += Chippos_isMuo[side][phi][chip];
                }
	      }
	      if ( (chip >= 65)  && (chip <= 76)  ) { // 12 chips
		m_preprocessing_Nchips_per3125[side][phi][6][bin] += Chippos[side][phi][chip][bin];
		if(bin==0){
                  FEboard_isHIP[side][phi][6] += Chippos_isHIP[side][phi][chip];
                  FEboard_isEle[side][phi][6] += Chippos_isEle[side][phi][chip];
                  FEboard_isMuo[side][phi][6] += Chippos_isMuo[side][phi][chip];
                }
	      }
	      if ( (chip >= 77)  && (chip <= 89)  ) { // 13 chips
		m_preprocessing_Nchips_per3125[side][phi][7][bin] += Chippos[side][phi][chip][bin];
		if(bin==0){
                  FEboard_isHIP[side][phi][7] += Chippos_isHIP[side][phi][chip];
                  FEboard_isEle[side][phi][7] += Chippos_isEle[side][phi][chip];
                  FEboard_isMuo[side][phi][7] += Chippos_isMuo[side][phi][chip];
                }
	      }
	      if ( (chip >= 90)  && (chip <= 103) ) { // 14 chips
		m_preprocessing_Nchips_per3125[side][phi][8][bin] += Chippos[side][phi][chip][bin];
		if(bin==0){
                  FEboard_isHIP[side][phi][8] += Chippos_isHIP[side][phi][chip];
                  FEboard_isEle[side][phi][8] += Chippos_isEle[side][phi][chip];
                  FEboard_isMuo[side][phi][8] += Chippos_isMuo[side][phi][chip];
                }
	      }
	      
	      //For the ECs
	      int chip_min = 104;
	      int chip_max = 115;
	      
	      for (int feb=9; feb <= 28; feb++ )
		{
		  if ( (chip >= chip_min) && (chip <= chip_max))
		    {
		      m_preprocessing_Nchips_per3125[side][phi][feb][bin]
			+= Chippos[side][phi][chip][bin]; // 12 chips for all the FEBs
		      if(bin==0){
                        FEboard_isHIP[side][phi][feb] += Chippos_isHIP[side][phi][chip];
                        FEboard_isEle[side][phi][feb] += Chippos_isEle[side][phi][chip];
                        FEboard_isMuo[side][phi][feb] += Chippos_isMuo[side][phi][chip];
                      }
		      break;
		    }
		  chip_min+=12;
		  chip_max+=12;
		}
	    }
	}
	
      }
    }
  } // END loop over side
  

  // Fill the ntuple arrays:
  for (int side = 0; side < 2; side++ ){
    for (int phi = 0; phi < 32; phi++ ){
      for (int feb = 0; feb < 29; feb++ ){
	for (int bin = 0; bin < 24; bin++ ){
	  m_nChips[side][phi][feb][bin]=
	    (UChar_t) m_preprocessing_Nchips_per3125[side][phi][feb][bin];
	  
	}
	m_isHIP[side][phi][feb]=(UChar_t)FEboard_isHIP[side][phi][feb];
	m_isEle[side][phi][feb]=(UChar_t)FEboard_isEle[side][phi][feb];
	m_isMuo[side][phi][feb]=(UChar_t)FEboard_isMuo[side][phi][feb];
      }
    }
  } // END loop over side
  
  // Below is a default version of the pre-processing 
  // and trigger logic, to be revisited     
  
  // Apply pre-processing
  for (int side = 0; side < 2; side++ ){
    for (int phi = 0; phi < 32; phi++ ){
      for (int feb = 0; feb < 29; feb++ ){
	int _min = 99;
	int _max = 0;
	int bx=0;
	for (int bin = 0; bin < 24; bin++ ){
	  if ( m_preprocessing_Nchips_per3125[side][phi][feb][bin] < _min){
	    _min = m_preprocessing_Nchips_per3125[side][phi][feb][bin];
	  }
	  if ( m_preprocessing_Nchips_per3125[side][phi][feb][bin] > _max){
	    _max = m_preprocessing_Nchips_per3125[side][phi][feb][bin];
	  }
	  if( (bin+1)%8==0){
	    // Defaul pre-processing: minimum of all sub-bins
	    if(m_preproc==1)
	      FEboard[bx][side][phi][feb] = _min; 
	    // Cross-check: take the value in last sub-bin
	    if(m_preproc==0)
	      FEboard[bx][side][phi][feb] = m_preprocessing_Nchips_per3125[side][phi][feb][bin];
	    // Cross-check: take maximum from sub-bins
	    if(m_preproc==2)
	      FEboard[bx][side][phi][feb] = _max;
	    _min = 99;
	    _max = 0;
	    bx++;
	  }
	}
      } // END loop over feb
    } // END loop over phi
  } // END loop over side  
  
    // based of the N active chips per FEB and the HIP patterns,
    // establish if there is a HIP candidate passing the trigger
  
    // This 2D array stores the trigger decision per side and phi stack
  
  static int TrigDec[2][32]; // Gets set to zero first thing a few lines from now

  for ( int side = 0; side < 2; side++ ) {
    for ( int phi = 0; phi < 32; phi++ ) {
      for ( int feb = 0; feb < 29; feb ++ ) {
	for(int bx=0; bx<3; bx++)
	  phiStack[feb][bx] = FEboard[bx][side][phi][feb];
      } // close loop over feb

	// Passes the trigger?
      TrigDec[side][phi] = 0;
      int ret=0;
      
      // Make m_qval dependent 
      int NallChips[29]  = { 10, 11, 15, 9, 9, 11, 12, 13, 14, // Barrel 
			     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,// EC A-wheels
			     12, 12, 12, 12, 12, 12, 12, 12}; // EC B-wheels
      int _Bar_trigPat[4][29];
      int _Tran_trigPat[4][29];
      for (int i=0; i<4; i++ ){
	for (int j=0; j<29; j++ ){
	  if(j<=8)
	    _Tran_trigPat[i][j] = std::ceil( NallChips[j]*m_qval/15.);
	  else
	    _Tran_trigPat[i][j] = m_qval;	    
	  _Bar_trigPat[i][j] = _Tran_trigPat[i][j] ;
	}
	
	//  std::cout << m_qval << " BAR -> ";
	//  for (int j=0; j<29; j++ ){
	//    std::cout << _Bar_trigPat[i][j];
	//  }
	//  std::cout << std::endl;
	
      }

      // For the barrel
      for (int i=0; i<4; i++ )
	{
	  // the arrays used for the HIP patterns are defined in the
	  // standalone file TRT_Digitization/TRTFastOR.Patterns
	  // Fixme: we prefer not to use includes like this
	  //        maybe code directly when the arrays are optimized.
	  ret = TrigPass( Bar_trigConf[i], _Bar_trigPat[i], phiStack );
	  if(ret>0) { 
	    //BarPassTrig[i] = 1; 
	    //BarMaskPassTrig[i] = ret;
	    TrigDec[side][phi] = 1;
	  }
	  
	  ret = TrigPass( Tran_trigConf[i], _Tran_trigPat[i], phiStack );
	  if(ret>0) { 
	    //TranPassTrig[i] = 1; 
	    //TranMaskPassTrig[i] = ret;
	    TrigDec[side][phi] = 1;
	  }
	  
	}
      
      // For the Endcap
      //      int q = 2;
      for (int i=0; i<17; i++ )
	{
	  ret = TrigPass_ec( EC_trigConf_set4[i], m_qval, phiStack ) ;
	  if(ret>0) { 
	    //EC4PassTrig[i] = 1; 
	    //EC4MaskPassTrig[i] = ret;
	    TrigDec[side][phi] = 1;
	  }

	}
      
    } // close loop over phi-stack
  } // close loop over side
  
    // store the trigger output decision in a 64 bit data word
    // the first 32 bits correspond to the 32 phi stacks on side A
    // and the remaining 32 bits to the phi stacks on side C.
  int output = 0;
  int p = 0;
  for (int side=0; side < 2; side++)  {
    for (int phi=0; phi < 32; phi++ ) {
      // fixed: pow() is slow, consider using bitwise operators?
      //      output += TrigDec[side][phi]*pow(2,p);
      output |= (TrigDec[side][phi] << p);
      p++;
    }
  }
  
  //std::cout << "END FastOR About to return " << std::hex << output << std::dec << "." << std::endl; 
  
  m_nt->Fill();   
  
  return output;
  
}

// calculate ToHT per straw
// fixme: actually this simply counts the number of set bits.
int TRTFastORAlg::straw_isHIPcandidate(std::vector<int> straw_HT_ternaryInfo)
{
  int ToHT = 0;
  for (unsigned int j = 0; j < straw_HT_ternaryInfo.size(); j++)
    {
      if (straw_HT_ternaryInfo.at(j) == 1) ToHT++; // fixme: .at() is slow, and we really dont need it here - right?
    }
  return ToHT;

}

int TRTFastORAlg::TrigPass( const int TrigPat[],  const int Thresh[], int chipsPerFEboard[][3] )
{
  int nPass = 0;  // number of boards passing trig per bc
  int nBoard = 0; // number of boards in trigger pattern
  
  for ( int i = 0; i < 29; i++ ){
    nBoard += TrigPat[i];
  }
  
  int PassBX[3]     = {0, 0, 0};
  for(int bx=0; bx<3; bx++){
    nPass = 0;
    for ( int j = 0; j < 29; j++ ){
      if( TrigPat[j] == 1 ){
	if( chipsPerFEboard[j][bx] >= Thresh[j] )
	  nPass += 1; 
      }
    }//end loop over boards
    if( nPass >= nBoard ){
      PassBX[bx] = 1;
    }
  }
  
  // fixme: need to require two bxing, not just 2nd and 3rd
  if((PassBX[1]==1 && PassBX[2]==1 )||
     (PassBX[1]==0 && PassBX[1]==1 )
     ){
    //    std::cout << "PassBX Bar: " << PassBX[0] << "-"<<PassBX[1]<<"-"<< PassBX[2] << std::endl;
    return 1;
  }  else 
    return 0;
  
}
 

int TRTFastORAlg::TrigPass_ec( const int TrigPat[], int Thresh, int chipsPerFEboard[][3] )
{
  int nPass  = 0;                  // number of boards passing trig per bc
  int nBoard = 0;                  // number of boards in trigger pattern
  
  for ( int i = 0; i < 29; i++ ){
    nBoard += TrigPat[i];
  }
  
  int PassBX[3]     = {0, 0, 0}; 
  
  for(int bx=0; bx<3; bx++){
    nPass = 0;
    for ( int j = 0; j < 29; j++ )
      {
	if( TrigPat[j] == 1 )
	  {
	    if( chipsPerFEboard[j][bx] >= Thresh )
	      nPass += 1;
	  }
      }//end loop over boards
    if( nPass >= nBoard ){
      PassBX[bx] = 1;
    }
  }
  if( (PassBX[1]==1 && PassBX[2]==1 ) || 
      (PassBX[0]==1 && PassBX[1]==1 ) ) 
    {
      //      std::cout << "PassBX EC:  " << PassBX[0] << "-"<<PassBX[1]<<"-"<< PassBX[2] << std::endl;
    return 1;
  }else 
    return 0;
  
}

