/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArABBADecoder.h"
//#include "eformat/index.h"
#include "LArByteStream/ABBAMapping.h"

static const InterfaceID IID_ILArABBADecoder("LArABBADecoder", 1, 0);


using namespace OFFLINE_FRAGMENTS_NAMESPACE;

LArABBADecoder::LArABBADecoder(const std::string& type, const std::string& name,
			       const IInterface* parent ) 
  : AthAlgTool(type,name,parent) 
{
  declareInterface< LArABBADecoder  >( this );

}

const InterfaceID& LArABBADecoder::interfaceID( )
{ return IID_ILArABBADecoder;  }



LArABBADecoder::~LArABBADecoder() {}


StatusCode LArABBADecoder::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode LArABBADecoder::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode LArABBADecoder::convert(const RawEvent* re, LArDigitContainer* coll) {

  bool ret=false;
  // Check fragment validity:
  try {
    ret=re->check();
  } 
  catch (eformat::Issue& ex) {
    msg(MSG::WARNING) << "Exception while checking eformat fragment validity: " << ex.what() << endreq; 
    ret=false;
  }
  if (!ret) {
    msg(MSG::ERROR) << "Got invalid RawEvent fragment" << endreq;
    return StatusCode::FAILURE;
  }
  
  //Build TOC
  std::map<eformat::SubDetectorGroup, std::vector<const uint32_t*> > robIndex;
  eformat::helper::build_toc(*re, robIndex );
  for (auto mapit : robIndex) std::cout << "Rob Index subdetgroup is " << std::hex << mapit.first << std::endl; 	
  std::map<eformat::SubDetectorGroup, std::vector<const uint32_t*> >::const_iterator robIt = robIndex.find(eformat::LAR);
  if (robIt!=robIndex.end()) {
      const std::vector<const uint32_t*>& robs = robIt->second;
      for (const uint32_t* pRob :robs) {
	try {
	  ROBFragment robFrag(pRob);
	  //fillCollection(&pRob,coll);
	    fillCollection(&robFrag,coll);
	}	
	catch (eformat::Issue& ex) {
	  ATH_MSG_WARNING ( " exception thrown by ROBFragment, badly corrupted event. Abort decoding " );
	  coll->clear();
	  break;
	}
      }//end loop over robs
  }//end if got LAr-RODs   
  return StatusCode::SUCCESS;
}


void LArABBADecoder::fillCollection(const ROBFragment* robFrag, LArDigitContainer* coll) {

  const CaloGain::CaloGain dummyGain=CaloGain::LARHIGHGAIN;
  LArDigit * scDigit=0 ;

  //FIXME: Check validity
  uint32_t nData = robFrag->rod_ndata(); 
  const uint32_t* p=robFrag->rod_data();
  if (nData < 3) {
	    ATH_MSG_WARNING ( "Error reading bytestream event: "
			      << "Empty ROD block (less than 3 words) for source ID " << robFrag->rod_source_id() );
   return;
	  }
  else
  //int rob_source_id;
  robFrag->rod_source_id();
  //std::cout << "rob_source_id: " << robFrag->rob_source_id() << std::endl;
  //std::cout << "rod_source_id: " << robFrag->rod_source_id() << std::endl;
  unsigned int n = robFrag->payload_size_word();
  //std::cout << "payload size: " << n << std::endl;

     std::vector<std::string> string; 
     int x=0x0;
      int nsamples = p[0];
      //std::cout << "nsamples :" << std::hex << nsamples << std::endl;
      for(unsigned int s=1;s<n-13;s+=6) {
	//std::cout << "Fragmentsize :" << n << std::endl;
        std::string ss;
	for (unsigned int k=0;k<6;++k){
	x = p[s+k];
	//std::cout << "x :" << x << std::endl;
        std::ostringstream oss;
	oss << std::hex << x;
        if (oss.str().size() == 7){
	   //std::cout << "oss: " << oss.str() << std::endl;
 	   //std::cout << "here: " << ss << std::endl; 
	   ss.append("0");
           //std::cout << "adding 0: " << ss << std::endl;
	}
	ss += oss.str();
	//std::cout << "oss: " << oss.str() << ", oss_size: " << oss.str().size() << std::endl;
          }
     //std::cout << "ss: " << ss << "ss_size:" << ss.size() <<std::endl;
     string.push_back(ss);  
     } 
//std::cout << "string size: "<<string.size() << std::endl;



int nFibers = 20;
int nchannels = 8;
int Nabbachannels = nFibers * nchannels;

int fiberSeq[16] = {0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1};
int adcSeq[16] =   {1,0,1,0,3,2,3,2,5,4,5,4,7,6,7,6};

std::vector< std::vector< std::vector<int> > > abbasamples;
abbasamples.resize(nFibers);

std::vector<std::vector<short>> abba_channel_samples;
abba_channel_samples.resize(Nabbachannels);

for (int fib=0;fib < nFibers; ++fib){
    abbasamples[fib].resize(nchannels);
	for (int ch = 0; ch < nchannels; ++ch){
	    abbasamples[fib][ch].resize(nsamples);
	    int Nchannel = fib*8 + ch;
	    abba_channel_samples[Nchannel].resize(nsamples);    
	}
   }
	
//std::cout << "x: " << abbasamples.size() << ", y: " << abbasamples[0].size() << ", z: " << abbasamples[0][0].size() << std::endl;

 for (int fib=0;fib < nFibers; fib+=2){
        for (int sam=0; sam < nsamples; sam++){ 
            int zx = sam + (fib/2)*nsamples;
            std::string wx = string[zx];
            int k = 0;
            for (int m = 0; m < 16; ++m){ //16 size of fiberSeq array
		int fiber_number = fiberSeq[m];
		int adc_number = adcSeq[m];
                int idx = k*3;
		std::stringstream ss;
		char mysample[3];
		//std::cout << "idx: " << idx << std::endl;
		for (int j = idx; j < idx+3; ++j){
		    int i = j%3;
		    //std::cout << "j: " << j << "i: " << i << " wx_size: " << wx.size() << std::endl;
                    mysample[i] = wx[j];
 		}
		ss << mysample;
		//std::cout << "ss: " << ss << std::endl;
		short mysample_int;
		ss >> std::hex >> mysample_int;
                int fb = fiber_number+fib;
                int ch = adc_number;
		int abbachannel = fb*8+ch;
		k++;
	        //std::cout << "fiber: " << fb << std::endl;
	        //std::cout << "channel: " << ch << std::endl;
	        //std::cout << "samples: " << sam << std::endl;
	        //std::cout << "mysample int: " << mysample_int << std::endl;
	        //std::cout << "k: " << k << std::endl;
     	        abbasamples[fb][ch][sam] = mysample_int;
		abba_channel_samples[abbachannel][sam] = mysample_int;
		//std::cout << "abbasamples: " << abbasamples[fb][ch][sam] << std::endl;
		//std::cout << "abba_channel_samples, abbachannel, sam: " << abba_channel_samples[abbachannel][sam] <<", " << abbachannel << ", " << sam <<std::endl;
            }
        }
    }
//std::cout << "size: " << abba_channel_samples.size() << std::endl;

std::map<int, HWIdentifier> abba_mapping;


int iphi = 0;
switch (robFrag->rod_source_id())
{
case 0x410511:
  iphi=19;
  break;
////modified for data taken between march 31 and apr 13
//case 0x410522:
case 0x410512:
  iphi=18;
  break;
case 0x410521:
  iphi=20;
  break;
////case 0x410512:
case 0x410522:
  iphi=21;
  break;

default:
msg(MSG::ERROR) << "ROD_source_id not found" << endmsg;
}

// Dumping of full event data
/*for (int fib = 0; fib < nFibers; ++fib) {
  std::cout << std::dec << "fibre: " << fib << std::endl;
  for (int ch = 0; ch < nchannels; ++ch) {
    std::cout << "channel: " << ch << " -- ";
    for (unsigned int sampl = 0; sampl < abbasamples[fib][ch].size(); ++sampl) {
      std::cout << abbasamples[fib][ch][sampl] << "  ";
    }
    std::cout << std::endl;
  }
}*/

// k-code detection and removal
/*std::vector<bool> k_code_detected;
std::vector<bool> k_code_expected;

k_code_detected.resize(nchannels);
k_code_expected.resize(nchannels);

k_code_expected.at(0) = false;
k_code_expected.at(1) = false;
k_code_expected.at(2) = true;
k_code_expected.at(3) = true;
k_code_expected.at(4) = true;
k_code_expected.at(5) = true;
k_code_expected.at(6) = false;
k_code_expected.at(7) = true;

for (int fib = 0; fib < nFibers; ++fib) {
  for (unsigned int sampl = 1; sampl < abbasamples[fib][0].size()-1; ++sampl) {
    for (int ch = 0; ch < nchannels; ++ch) {
      if (2 * abbasamples[fib][ch][sampl] - (abbasamples[fib][ch][sampl-1] + abbasamples[fib][ch][sampl+1]) > 3000) k_code_detected.at(ch) = true;
      else k_code_detected.at(ch) = false;
    }
    
    // General output in case of all channels match k-code requirement
    bool kcode_match = true;
  
    for (int ch = 0; ch < nchannels; ++ch) {
      if (!(k_code_expected.at(ch) == k_code_detected.at(ch))) kcode_match = false;
    }
    
    // General output in case of one channel matching k-code requirement
    //bool kcode_match = false;
    //
    //for (int ch = 0; ch < nchannels; ++ch) {
    //  if (k_code_detected.at(ch)) kcode_match = true;
    //}
    
    if (kcode_match) {
      std::cerr << "Detected k-code in fibre " << fib << " (iphi = " << iphi << ") at sampl " << sampl << " with the following properties:" << std::endl;
      
      for (int ch_print = 0; ch_print < nchannels; ++ch_print) {
        std::cerr << "channel: " << ch_print << " -- ";
        for (unsigned int sampl_print = sampl-1; sampl_print < sampl+2; ++sampl_print) {
          std::cerr << abbasamples[fib][ch_print][sampl_print] << "  ";
        }
        std::cerr << std::endl;
      }
    }
  }
}*/


ABBAMapping::fill(&abba_mapping, iphi);


for (std::size_t index_abba_channel_samples = 0; index_abba_channel_samples < abba_channel_samples.size(); ++index_abba_channel_samples) {
  auto abba_mapping_search = abba_mapping.find(index_abba_channel_samples);
  if (abba_mapping_search == abba_mapping.end()) {
    msg(MSG::WARNING) << "No mapping for index_abba_channel_samples: "<<std::dec<<index_abba_channel_samples << endmsg;
    continue;
  }
    
    HWIdentifier channelID = (*abba_mapping_search).second;
    //std::cout << index_abba_channel_samples << ", channelID: " << channelID << std::endl;
    std::vector<short> adcValues;
    adcValues = abba_channel_samples[index_abba_channel_samples];
    //std::cout << "size adcValues: " << adcValues.size() << std::endl;
    scDigit=new LArDigit(channelID,dummyGain,adcValues);

    //std::cout << "channelID: "<< channelID << std::endl;
    //std::cout << "adcValues size: "<< adcValues.size() << std::endl;

/*
if ((adcValues.at(0) < 800) ||(adcValues.at(1) < 800) ||(adcValues.at(2) < 800)||(adcValues.at(3) < 800) ||(adcValues.at(4) < 800) ||(adcValues.at(5) < 800)||(adcValues.at(6) < 800)){
//std::cout << "abba_channel_samples[0], [1], [2], ...[6]: " << adcValues.at(0) << ", " << adcValues.at(1) << ", " << adcValues.at(2) << ", " <<adcValues.at(3) << ", " <<adcValues.at(4) << ", " << adcValues.at(5)<< ", " <<adcValues.at(6) <<std::endl; 
}
if ((adcValues.at(7) < 800) ||(adcValues.at(8) < 800) ||(adcValues.at(9) < 800)||(adcValues.at(10) < 800) ||(adcValues.at(11) < 800) ||(adcValues.at(12) <800)){
//std::cout << "abba_channel_samples[7]...[12]" << adcValues.at(7) << ", " <<adcValues.at(8) << ", " << adcValues.at(9) << ", " <<  adcValues.at(10) << ", " <<  adcValues.at(11) << ", "  <<adcValues.at(12)<<std::endl;
}
if ((adcValues.at(13) < 800) ||(adcValues.at(14) < 800) ||(adcValues.at(15) < 800)||(adcValues.at(16) < 800) ||(adcValues.at(17) < 800) ||(adcValues.at(18) < 800)){
//std::cout << "abba_channel_samples[13]...[18]" << adcValues.at(13) << ", " <<adcValues.at(14) << ", " << adcValues.at(15) << ", " <<  adcValues.at(16) << ", " <<  adcValues.at(17) << ", "  <<adcValues.at(18)<<std::endl;
}
if ((adcValues.at(19) < 800) ||(adcValues.at(20) < 800) ||(adcValues.at(21) < 800)||(adcValues.at(22) < 800) ||(adcValues.at(23) < 800) ||(adcValues.at(24) < 800)){
//std::cout << "abba_channel_samples[19]...[24]" << adcValues.at(19) << ", " <<adcValues.at(20) << ", " << adcValues.at(21) << ", " <<  adcValues.at(22) << ", " <<  adcValues.at(23) << ", "  <<adcValues.at(24)<<std::endl;
}
if ((adcValues.at(25) < 800) ||(adcValues.at(26) < 800) ||(adcValues.at(27) < 800)||(adcValues.at(28) < 800) ||(adcValues.at(29) < 800) ||(adcValues.at(30) < 800)){
//std::cout << "abba_channel_samples[25]...[30]" << adcValues.at(25) << ", " <<adcValues.at(26) << ", " << adcValues.at(27) << ", " <<  adcValues.at(28) << ", " <<  adcValues.at(29) << ", "  <<adcValues.at(30)<<std::endl;
}
if ((adcValues.at(31) < 800) ||(adcValues.at(32) < 800) ||(adcValues.at(33) < 800)||(adcValues.at(34) < 800) ||(adcValues.at(35) < 800) ||(adcValues.at(36) < 800)){
//std::cout << "abba_channel_samples[31]...[36]" << adcValues.at(31) << ", " <<adcValues.at(32) << ", " << adcValues.at(33) << ", " <<  adcValues.at(34) << ", " <<  adcValues.at(35) << ", "  <<adcValues.at(36)<<std::endl;
}
if ((adcValues.at(37) < 800) ||(adcValues.at(38) < 800) ||(adcValues.at(39) < 800)||(adcValues.at(40) < 800) ||(adcValues.at(41) < 800) ||(adcValues.at(42) < 800)){
//std::cout << "abba_channel_samples[37]...[42]" << adcValues.at(37) << ", " <<adcValues.at(38) << ", " << adcValues.at(39) << ", " <<  adcValues.at(40) << ", " <<  adcValues.at(41) << ", "  <<adcValues.at(42)<<std::endl;
}
if ((adcValues.at(43) < 800) ||(adcValues.at(44) < 800) ||(adcValues.at(45) < 800)||(adcValues.at(46) < 800) ||(adcValues.at(47) < 800) ||(adcValues.at(48) < 800)){
//std::cout << "abba_channel_samples[43]...[48]" << adcValues.at(43) << ", " <<adcValues.at(44) << ", " << adcValues.at(45) << ", " <<  adcValues.at(46) << ", " <<  adcValues.at(47) << ", "  <<adcValues.at(48)<<std::endl;
}
if ((adcValues.at(49) < 800) ||(adcValues.at(50) < 800) ||(adcValues.at(51) < 800)||(adcValues.at(52) < 800) ||(adcValues.at(53) < 800) ||(adcValues.at(54) < 800)){
//std::cout << "abba_channel_samples[49]...[54]" << adcValues.at(49) << ", " <<adcValues.at(50) << ", " << adcValues.at(51) << ", " <<  adcValues.at(52) << ", " <<  adcValues.at(53) << ", "  <<adcValues.at(54)<<std::endl;
}
if ((adcValues.at(55) < 800) ||(adcValues.at(56) < 800) ||(adcValues.at(57) < 800)||(adcValues.at(58) < 800) ||(adcValues.at(59) < 800) ||(adcValues.at(59) < 800)){
//std::cout << "abba_channel_samples[55]...[60]" << adcValues.at(55) << ", " <<adcValues.at(56) << ", " << adcValues.at(57) << ", " <<  adcValues.at(58) << ", " <<  adcValues.at(59) << ", "  <<adcValues.at(59)<<std::endl;
}
*/


 
    coll->push_back(scDigit);
    //std::cout << "pushback scDigit to coll" << std::endl;
  }

abbasamples.clear();

  return;


}
