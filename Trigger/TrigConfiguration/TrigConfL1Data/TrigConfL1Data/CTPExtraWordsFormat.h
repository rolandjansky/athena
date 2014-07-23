/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CTPEXTRAWORDSFORMAT_H
#define CTPEXTRAWORDSFORMAT_H

/**
 * @file   CTPExtraWordsFormat.h
 * @brief  Definition of the CTP extra words payload format
 * @author Frank Winklmeier
 *
 * $Id: $
 */

#include <vector>
#include <string>
#include <map>
#include <stdint.h>
#include <ostream>
#include <stdexcept>
#include <algorithm>

namespace CTP {

  typedef uint16_t FolderIndex;
  typedef uint16_t LumiBlock;
  typedef std::vector<uint32_t>::iterator PayloadIterator;

  /**
   * Exception thrown if payload is above maximum length
   */
  class ExtraPayloadTooLong : public std::runtime_error {
  public:
    ExtraPayloadTooLong() : std::runtime_error("ExtraPayloadTooLong") { }
  };

  /**
   * A folder entry in the CTP extra payload consisting of
   *  1) 16 bits: lumiblock of folder update
   *  2) 16 bits: folder index
   */
  struct FolderEntry {
    FolderEntry() : lumiBlock(0), folderIndex(0) {}
    
    FolderEntry(uint32_t payload) :
      lumiBlock((payload & 0xffff0000) >> 16),
      folderIndex(payload & 0xffff) {}
    
    uint32_t serialize() const {
      return (lumiBlock << 16) | folderIndex;
    }

    /// Size of payload (in 32-bit words)
    std::size_t size() const { return 1; }
    
    LumiBlock lumiBlock;
    FolderIndex folderIndex;
  };

  /**
   * Class defining the data format of the CTP extra payload
   *
   * The current data format is as follows:
   *   word (32 bit)    comment
   *       0          L1 prescale key
   *       1          number of COOL folders (N)
   *       2..2+N     N FolderEntry (see above class)
   *    
   */
  class ExtraPayload {
  public:

    /**
     * Default constructor
     * Use to create empty payload object
     * @param maxPayloadWords Maximum payload length in (32bit) words
     */
    ExtraPayload(std::size_t maxPayloadWords = 50) : m_l1psk(0), m_maxPayload(maxPayloadWords) {}

    /**
     * Constructor
     * Initialize payload object with existing payload
     * @param payload Payload words
     * @param maxPayloadWords Maximum payload length in (32bit) words     
     */
    ExtraPayload(const std::vector<uint32_t>& payload, std::size_t maxPayloadWords = 50);

    
    uint32_t getL1PSK() const { return m_l1psk; }
    void setL1PSK(uint32_t l1psk) { m_l1psk = l1psk; }
    
    /**
     * Return COOL folder updates
     */
    const std::map<FolderIndex, FolderEntry>& getFolderUpdates() const { return m_entries; }
    
    /**
     * Update COOL folder
     * Throws ExtraPayloadTooLong if there is no space in the payload
     */
    void updateFolder(FolderEntry& e);
    
    /**
     * Size of payload (in 32-bit words)
     */
    std::size_t size() const;
  
    /**
     * Serialize payload
     * @return vector with 32-bit words
     */
    std::vector<uint32_t> serialize() const;

    /**
     * Serialize payload into existing vector
     * @param first iterator pointing to a vector with sufficient space for payload (@see size())
     */
    void serialize(PayloadIterator first) const;
    
  private:
    
    std::map<FolderIndex, FolderEntry> m_entries;
    uint32_t    m_l1psk;
    std::size_t m_maxPayload;
  };
  
} //namespace CTP

std::ostream& operator <<(std::ostream& os, const CTP::ExtraPayload& p);

#endif
