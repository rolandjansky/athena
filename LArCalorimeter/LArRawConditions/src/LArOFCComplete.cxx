/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRawConditions/LArOFCComplete.h" 

const std::vector<float> LArOFCComplete::m_empty;

LArOFCComplete::LArOFCComplete(){ 
}

LArOFCComplete::~LArOFCComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArOFCComplete::set(const HWIdentifier& CellID, int gain,
			 const std::vector<std::vector<float> >& vOFC_a,
			 const std::vector<std::vector<float> >& vOFC_b,
			 float timeOffset,float timeBinWidth)
{
  LArOFCP1 t (timeOffset, timeBinWidth, vOFC_a, vOFC_b);
  setPdata(CellID, t, gain) ; 
}

/* retrieve OFC_a coefficients for a given tbin using online ID ****************************
 */
LArOFCComplete::OFCRef_t
LArOFCComplete::OFC_a(const HWIdentifier& CellID,
                      int gain,
                      int tbin) const 
{ 
  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid() || tbin<0 || tbin>=(int)t.OFC_aSize())
    return m_empty;
  return t.OFC_a (tbin);
}

/* retrieve OFC_b coefficients for a given tbin using online ID ****************************
 */
LArOFCComplete::OFCRef_t
LArOFCComplete::OFC_b(const HWIdentifier& CellID,
                      int gain,
                      int tbin) const 
{
  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid() || tbin<0 || tbin>=(int)t.OFC_bSize())
    return m_empty;
  return  t.OFC_b (tbin);
}


//retrieving time offset using online ID
float LArOFCComplete::timeOffset(const HWIdentifier&  CellID, int gain) const {
  if (gain<0 || gain>3) {
    std::cout << "ERROR! LArOFCComplete::timeOffset time offset called for gain " << gain << std::endl;
    std::cout.flush();
    return 0;
  }

  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid()) return 0;
  return  t.timeOffset();
}

//For the TB / cosmic case: retrieve the number of time-bins (aka "phases")
unsigned LArOFCComplete::nTimeBins(const HWIdentifier&  CellID, int gain) const {
  if (gain<0 || gain>3) {
    std::cout << "ERROR! LArOFCComplete::nTimeBins time offset called for gain " << gain << std::endl;
    std::cout.flush();
  }
  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid()) return 0;
  return t.OFC_aSize(); //Check also b-coefficient?
}

  //For the TB / cosmic case: retrieve the width of the time bin (default 24 bins in 25 ns)
float LArOFCComplete::timeBinWidth(const HWIdentifier&  CellID, int gain) const {
  if (gain<0 || gain>3) {
    std::cout << "ERROR! LArOFCComplete::timeBinWidth time offset called for gain " << gain << std::endl;
    std::cout.flush();
  }
  CONTAINER::ConstReference t = get(CellID,gain) ; 
  if (!t.isValid()) return 0;
  return t.timeBinWidth();
}


#include <stdio.h>
// dump entire LArOFCComplete structure to an ascii file dumpOFC.txt
// useful for debug
void LArOFCComplete::dumpOFC(std::string output_file_name) const {
  FILE* f = fopen( output_file_name.c_str() , "w" ) ;
  if (!f) {
    std::cerr << "Can't open output file: " << output_file_name << "\n";
    return;
  }

  for (unsigned int iGain = minGain(); iGain < nGains(); ++iGain) {

      ConstConditionsMapIterator it_chan   = begin(iGain);
      ConstConditionsMapIterator it_chan_e = end  (iGain);

      for ( ; it_chan !=it_chan_e ;++it_chan  ) {
	  const LArOFCP1&   chunkOFC = *it_chan ;
	  unsigned int     chID     = it_chan.channelId().get_identifier32().get_compact();
	  fprintf( f , "---------------------- Gain = %1u ; Channel = %10x ----------------------\n", 
		   iGain, chID ) ;
	  unsigned nTbin = chunkOFC.OFC_aSize() ;
	  assert( nTbin == chunkOFC.OFC_bSize() ) ;
	  //      fprintf( f , "\t bin \t a-coefficients \t \t \t \t \t \t b-coefficients \n" ) ;
	  for ( unsigned kBin=0 ; kBin<nTbin ; kBin++ ) {
              unsigned nSample = (chunkOFC.OFC_a (kBin)).size() ;
	      assert( nSample == (chunkOFC.OFC_b (kBin)).size() ) ;
	      fprintf( f , "\t%2u\t |" , kBin ) ;
	      for ( unsigned kSample=0 ; kSample<nSample ; kSample++ ) {
                fprintf( f , "\t%8.3f" , chunkOFC.OFC_a(kBin)[kSample] ) ;
	      }
	      fprintf( f , "\t |" ) ;
	      for ( unsigned kSample=0 ; kSample<nSample ; kSample++ ) {
                fprintf( f , "\t%8.3f" , chunkOFC.OFC_b(kBin)[kSample] ) ;
	      }
	      fprintf( f , "\n" ) ;
	  }
      }
  }
  fclose(f) ;
}


