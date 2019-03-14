//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  LArRawDataContByteStreamTool.h
 * @author Remi Lafaye, Walter Lampl 
 * @date July, 2004
 * @brief Helper tool for conversion of raw data classes to/from LArByteStream 
 *        
 */
#ifndef LARBYTESTREAM_LARRAWDATACONTBYTESTREAMTOOL_H
#define LARBYTESTREAM_LARRAWDATACONTBYTESTREAMTOOL_H

#include <stdint.h>
#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/ToolHandle.h"
#include "LArByteStream/Hid2RESrcID.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "ByteStreamData/RawEvent.h" 
#include "LArByteStream/LArRodDecoder.h"

#include "CaloIdentifier/CaloGain.h"

#include "LArRawEvent/LArFebHeaderContainer.h"
// Map of ROBs need this
#include "eformat/index.h"

class LArRawChannelContainer; 
class LArDigitContainer;
class IByteStreamEventAccess;
class ICaloNoiseTool;

/** 
 *@class LArRawDataContByteStreamTool
 *@brief An AlgTool class to help converting LAr raw data objects to/from ByteStream
 *
 * This tool is used for reading as well as writing. It's main prupose is to 
 * find all LAr-Fragment in the full byte-stream event
 */
      
class LArRawDataContByteStreamTool: public AthAlgTool {
public:

  /** Constructor
      Standard AlgTool constructor
  */
   LArRawDataContByteStreamTool( const std::string& type, const std::string& name,
        const IInterface* parent ) ;

  /** Destructor 
  */ 
  ~LArRawDataContByteStreamTool() ;

  /** AlgTool InterfaceID
  */
  static const InterfaceID& interfaceID( ) ;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /** 
   * @brief Templated conversion class form Raw Event to a container
   * @param RawEvent: Full event in ByteStream format as defined in 
   * @c ByteStreamData/RawEvent.h
   * @param COLLECTION: Template for the raw data container. Can be the 
   * containers of LArRawChannel, LArDigit, LArCalibDigit, LArFebHeader 
   * or LArAccumulatedCalibDigit or LArAccumulatedDigit
   * @param gain: For fixed-gain mode (e.g. calibration digits), indicates 
   * which gain should be read
   * @return Gaudi StatusCode 
   * 
   * Iterates though all levels of the Atlas ByteStream format (aka eformat) 
   * to get all ROD fragments belonging to LAr. Methods from @c LArRodDecoder 
   * are used to deal with the individual ROD fragments. 
  */
  template <class COLLECTION >
  StatusCode convert(const RawEvent* re, COLLECTION* digit_cont, CaloGain::CaloGain gain);

  /** 
   * @brief Fill channels from LArDigitContainer to a FullEvent
   * @param digit_cont Pointer to LArDigitContainer (input)
   * @param fea Pointer to FullEventAssember (output)
   * @return Gaudi StatusCode 
   */
  StatusCode WriteLArDigits(const LArDigitContainer* digit_cont, FullEventAssembler<Hid2RESrcID> *fea);

 /** 
  * @brief Fill channels from LArCalibDigitContainer to a FullEvent
  * @param digit_cont Pointer to LArCalibDigitContainer (input)
  * @param fea Pointer to FullEventAssember (output)
  * @return Gaudi StatusCode 
  */
  StatusCode WriteLArCalibDigits(const LArCalibDigitContainer* digit_cont, FullEventAssembler<Hid2RESrcID> *fea);

  /** 
   * @brief Fill channels from LArRawChannelContainer to a FullEvent
   * @param digit_cont Pointer to LArRawChannelContainer (input)
   * @param fea Pointer to FullEventAssember (output)
   * @return Gaudi StatusCode 
   */
  StatusCode WriteLArRawChannels(const LArRawChannelContainer* CannelCont, FullEventAssembler<Hid2RESrcID> *fea);
 
private: 
  /** Prepare ROB index before conversion */
  StatusCode prepareRobIndex (const RawEvent* event);
 
  //StatusCode prepareWriting();
  /** 
   * @brief Check that all elements in a container have the same gain
   * @param coll Pointer to input container (template)
   * @return false if an incosistency is detected
   */
  template <class COLLECTION >
    bool checkGainConsistency(const COLLECTION* coll);
 
  Hid2RESrcID m_hid2re;       //!< Contains the mapping from channel to ROD
  LArRodDecoder *m_decoder;   //!< Pointer to RodDecoder class

  /** Pointer to @c LArRodBlockStructure base class. 
      Which concrete implementation is used depends on the value of 
      @c m_DSPRunMode and @c m_RodBlockVersion. Ony for writing. 
  */
  LArRodBlockStructure *m_RodBlockStructure;

  /** Indicateds which version of DSP code should be used for writing.
      This is equivalent to the DetectorEventType word in the ROD block header.
      Only for writing. 
   */
  unsigned m_DSPRunMode;

  /** Minor ROD block version
      This is equivalent to the lower 16 bits of the version word in the
      ROD block header. Only for writing.
   */
  unsigned short m_RodBlockVersion;

 /** JobOption to intitialize services needed only for writing
  */
  bool m_initializeForWriting;
  uint16_t m_subDetId;
  ToolHandle<ICaloNoiseTool> m_noisetool;
  double m_nfebsigma;
  // map with ROB group (LAr) to rob addresses
  std::map<eformat::SubDetectorGroup, std::vector<const uint32_t*> > m_robIndex;
  // last event processed
  uint32_t m_lastEvent;
  // want to process digits together with RawChannel
  bool m_includeDigits;
  // Name of Digit container to retrieve
  std::string m_DigitContName;
  
};


#endif
#include "LArByteStream/LArRawDataContByteStreamTool.icc" 




