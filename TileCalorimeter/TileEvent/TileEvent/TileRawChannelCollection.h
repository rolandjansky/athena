/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileRawChannelCollection_H       
#define TileRawChannelCollection_H       
                                         
#include "TileEvent/TileRawChannel.h" 
#include "TileEvent/TileRawDataCollection.h"
                                         
class TileRawChannelCollection : public TileRawDataCollection<TileRawChannel>
{
  friend class TileRawChannelContainerCnv;
public:

  typedef TileRawDataCollection<TileRawChannel> MyBase;
  typedef MyBase::ID ID;
  typedef TileRawChannel TElement;

  TileRawChannelCollection ( ID id, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS )
    : MyBase(id,ownPolicy),
      m_FragGlobalCRC(0),
      m_FragDSPBCID(0xDEAD),
      m_FragBCID(0),
      m_FragMemoryPar(0),
      m_FragSstrobe(0),
      m_FragDstrobe(0),
      m_FragHeaderBit(0),
      m_FragHeaderPar(0),
      m_FragSampleBit(0),
      m_FragSamplePar(0),
      m_FragFEChipMask(0xFFFF),
      m_FragRODChipMask(0xFFFF)
    { }
  // Alternate constructor for use with ConstDataVector.
  TileRawChannelCollection ( SG::OwnershipPolicy ownPolicy, ID id )
    : TileRawChannelCollection (id, ownPolicy)
  { }
  TileRawChannelCollection ( SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS )
    : MyBase(ownPolicy),
      m_FragGlobalCRC(0),
      m_FragDSPBCID(0xDEAD),
      m_FragBCID(0),
      m_FragMemoryPar(0),
      m_FragSstrobe(0),
      m_FragDstrobe(0),
      m_FragHeaderBit(0),
      m_FragHeaderPar(0),
      m_FragSampleBit(0),
      m_FragSamplePar(0),
      m_FragFEChipMask(0xFFFF),
      m_FragRODChipMask(0xFFFF)
    { }


  /**
   * @brief Copy constructor.
   * @param other Collection to copy.
   * Does a deep copy of the contents.
   */
  TileRawChannelCollection (const TileRawChannelCollection& other);

  /**
   * @brief Move constructor.
   * @param other Collection to move.
   * Move the contents from other collection.
   */
  TileRawChannelCollection(TileRawChannelCollection&& other) = default;


  ~TileRawChannelCollection() { }

  /**
   * Clear everything for next event
   */
  void clear ();

  /**
   * Various set methods
   */
  void setFragGlobalCRC(uint32_t globalcrc) { m_FragGlobalCRC = globalcrc; }
  void setFragDSPBCID(uint32_t bcid) { m_FragDSPBCID = bcid; }
  void setFragBCID(uint32_t bcid) { m_FragBCID = bcid; }
  void setFragMemoryPar(uint32_t memorypar) { m_FragMemoryPar = memorypar; }
  void setFragSstrobe(uint32_t sstrobe) { m_FragSstrobe = sstrobe; }
  void setFragDstrobe(uint32_t dstrobe) { m_FragDstrobe = dstrobe; }
  void setFragHeaderBit(uint32_t headerbit) { m_FragHeaderBit = headerbit; }
  void setFragHeaderPar(uint32_t headerpar) { m_FragHeaderPar = headerpar; }
  void setFragSampleBit(uint32_t samplebit) { m_FragSampleBit = samplebit; }
  void setFragSamplePar(uint32_t samplepar) { m_FragSamplePar = samplepar; }
  void setFragFEChipMask(uint32_t fechipmask) { m_FragFEChipMask = fechipmask; }
  void setFragRODChipMask(uint32_t rodchipmask) { m_FragRODChipMask = rodchipmask; }

  /**
   * Various get methods
   */
  inline uint32_t getFragGlobalCRC() const { return m_FragGlobalCRC; }
  inline uint32_t getFragDSPBCID() const { return m_FragDSPBCID; }
  inline uint32_t getFragBCID() const { return m_FragBCID; }
  inline uint32_t getFragMemoryPar() const { return m_FragMemoryPar; }
  inline uint32_t getFragSstrobe() const { return m_FragSstrobe; }
  inline uint32_t getFragDstrobe() const { return m_FragDstrobe; }
  inline uint32_t getFragHeaderBit() const { return m_FragHeaderBit; }
  inline uint32_t getFragHeaderPar() const { return m_FragHeaderPar; }
  inline uint32_t getFragSampleBit() const { return m_FragSampleBit; }
  inline uint32_t getFragSamplePar() const { return m_FragSamplePar; }
  inline uint32_t getFragFEChipMask() const { return m_FragFEChipMask; }
  inline uint32_t getFragRODChipMask() const { return m_FragRODChipMask; }

 private:

  uint32_t m_FragGlobalCRC;
  uint32_t m_FragDSPBCID;
  uint32_t m_FragBCID;
  uint32_t m_FragMemoryPar;
  uint32_t m_FragSstrobe;
  uint32_t m_FragDstrobe;
  uint32_t m_FragHeaderBit;
  uint32_t m_FragHeaderPar;
  uint32_t m_FragSampleBit;
  uint32_t m_FragSamplePar;
  uint32_t m_FragFEChipMask;
  uint32_t m_FragRODChipMask;

};

CLASS_DEF(TileRawChannelCollection,2928,0)
           
#endif

