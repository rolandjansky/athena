//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArLATOMEDecoder.h
 * @author Gen Tateno
 * @date June 2019
 * @brief Byte stream converter of LATOME.
 */

#ifndef LARBYTESTREAM_LARLATOMEDDECODER_H
#define LARBYTESTREAM_LARLATOMEDDECODER_H

#define N_LATOME_CHANNELS 320

#include <stdint.h>
#include "AthenaBaseComps/AthAlgTool.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawSCContainer.h"
#include "LArRawEvent/LArSCDigit.h"
#include "LArRawEvent/LArLATOMEHeaderContainer.h"
#include "LArCabling/LArLATOMEMapping.h"
#include "ByteStreamData/RawEvent.h"
#include "CxxUtils/unused.h"
#include "eformat/Version.h"
#include "eformat/Issue.h"
#include <vector>
#include <string>
#include <fstream>
#include "eformat/index.h"


/**
 * @brief Byte stream converter of LATOME.
 *
 * This tool decodes LATOME data and outputs ADC values and BCIDs.
 */

enum class MonDataType {
  RawADC=1,
  ADC=0,
  Energy=2,
  SelectedEnergy=3,
  Invalid=15
};

class LArLATOMEDecoder : public AthAlgTool {

public: 
  typedef unsigned int Word;
  typedef int NumWord;
  typedef int Packet;
  typedef int SuperCell;
  typedef int Sample;
  typedef int NumLATOME;
  typedef std::string Path;
  
  /** @brief Constructor*/
  LArLATOMEDecoder(const std::string& type, const std::string& name, const IInterface* parent);
  static const InterfaceID& interfaceID();
  /** @brief Destructor*/
  virtual ~LArLATOMEDecoder();
  /** @brief Initialize the converter*/
  virtual StatusCode initialize();
  /** @brief Finalize the converter*/
  virtual StatusCode finalize();
  /** @brief Converter*/
  StatusCode convert(const RawEvent* re, const LArLATOMEMapping *map,
                     LArDigitContainer* adc_coll,
		     LArDigitContainer* adc_bas_coll,
		     LArRawSCContainer* et_coll,
		     LArRawSCContainer* et_id_coll,
		     LArLATOMEHeaderContainer* header_coll) const;
  
private:

  /** @brief Maximum value of BCID*/
  static const int s_nBunches = 3564;
  /** @brief 4th bit should be this value if the word indicate ADC value.*/

  class LatomeRawData{

  public:
    std::vector<short> adc;
    std::vector<short> adc_bas;
    std::vector<int> et;
    std::vector<int> et_id;
    unsigned int latomeChannel;
  };

  class EventProcess {
  public:
    /// this should be the same as how we get the data, otherwise we will have bugs.
    /// use reinterpret_cast later to properly interpret the bit in other format if needed.
    typedef unsigned int Word;
    typedef int NumWord;
    typedef int Packet;
    typedef int SuperCell;
    typedef int Sample;
    typedef std::string Path;

    EventProcess(const LArLATOMEDecoder* decoderInput, const LArLATOMEMapping *map,
                 LArDigitContainer* adc_coll,
		 LArDigitContainer* adc_bas_coll,
		 LArRawSCContainer* et_coll,
		 LArRawSCContainer* et_id_coll,
		 LArLATOMEHeaderContainer* header_coll);
    
    /** @brief Execute decoding for an event*/

    void fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* pROB, const LArLATOMEMapping *map);

  private:
    bool compareOrSet(Word& param, Word value, bool compare);
    unsigned int decodeTrailer(const uint32_t* p, unsigned int offset);
    unsigned int decodeHeader(const uint32_t* p, unsigned int offset);
    unsigned int bytesPerChannel(MonDataType at0, MonDataType at1);
    void increaseWordShift(unsigned int& wordshift);
    void increaseByteShift(unsigned int& wordshift, unsigned int& byteshift);
    void decodeByte(unsigned int& byte, unsigned int wordshift, unsigned int byteshift, const uint32_t* p);
    void decodeWord(unsigned int& word, unsigned int& wordshift, unsigned int& byteshift, const uint32_t* p);
    void decodeChannel(unsigned int& wordshift, unsigned int& byteshift, const uint32_t* p,
		       MonDataType at0, MonDataType at1,
		       unsigned int& at0Data, unsigned int& at1Data, unsigned int& satData,
		       bool& at0val, bool& at1val);
    int signEnergy(unsigned int energy);
    /** @brief Pass ADC values from an event*/
    void fillRaw(const LArLATOMEMapping *map);
    void fillHeader();
    
    enum MODE {
      MON_HEADER, PAYLOAD, MON_TRAILER
    };

    /// some cached info to ease processing
    /// reading from data header
    // some of this values (in addition to some values from the rod header) should go to a LatomeHeader class (that replaces the current Mon class) and this class should be moved to another package and percistified (written into disk with pool format). This is not urgent and can be done later.
    static const Word m_monTrailerSize=2; /// this is fixed and not read from data
    static const Word s_monHeaderMarker=0xff1234ff;
    static const Word s_monCheckPoint=0xdeadbeef;
    std::vector<Word> m_packetEnd;

    // rod header
    Word m_latomeBCID;
    //// mon header
    Word m_latomeID;
    Word m_l1ID;
    Word m_nPackets;
    Word m_iPacket;
    Word m_nWordsPerPacket;
    Word m_monHeaderSize;
    Word m_region;
    Word m_nStreams;
    Word m_streamNumber;
    Word m_at0type;
    Word m_at1type;
    Word m_at0nBC;
    Word m_at1nBC;
    Word m_at0BC;
    Word m_at1BC;
    Word m_activeSC;
    Word m_nsc1;
    Word m_nsc2;
    Word m_nsc3;
    Word m_nsc4;
    Word m_nsc5;
    Word m_nsc6;
    bool m_headerDecoded;

    short m_nBC_rawADC;
    short m_nBC_ADC;
    short m_nBC_E;
    short m_nBC_EID;
    
    short m_BC_rawADC;
    short m_BC_ADC;
    short m_BC_E;
    short m_BC_EID;

    bool m_hasRawAdc;
    bool m_hasAdc;
    bool m_hasE;
    bool m_hasEID;

    
    const LArLATOMEDecoder *m_decoder;
    LArDigitContainer* m_adc_coll;
    LArDigitContainer* m_adc_bas_coll;
    LArRawSCContainer* m_et_coll;
    LArRawSCContainer* m_et_id_coll;
    LArLATOMEHeaderContainer* m_header_coll;
    
    // LATOME source ID is stored here:
    unsigned int m_nthLATOME = 0;

    std::vector<unsigned short> m_BCIDsInEvent;
    std::vector<LatomeRawData> m_rawValuesInEvent;
  };

  // propary of tool
  Path m_detailDumpFileName;
  Path m_ADCDumpFileName;  
  bool m_protectSourceId;
  
  /** @brief Detail dump file*/
  std::ofstream* ATH_UNUSED_MEMBER(m_detailDumpFile) = nullptr;
  /** @brief ADC dump file*/
  std::ofstream* ATH_UNUSED_MEMBER(m_ADCDumpFile) = nullptr;
};

#endif // LARBYTESTREAM_LARLATOMEDDECODER_H

