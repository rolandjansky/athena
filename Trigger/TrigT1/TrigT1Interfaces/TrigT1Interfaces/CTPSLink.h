/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_CTPSLINK_H
#define TRIGT1INTERFACES_CTPSLINK_H

// std include(s):
#include <stdint.h>

// STL include(s):
#include <vector>
#include <string>


// tdaq-common includes for CTP format definition
#include "CTPfragment/CTPdataformatVersion.h"

namespace LVL1CTP {

   /**
    * 
    *   @short CTP RoI output
    *
    *          Class used to send the RoI data from the CTP simulation to
    *          the RoIB simulation. It models the SLink connection.
    *
    *          One vector is used to store the header, data elements and
    *          trailer.
    *
    *          This class represents a cable connection between CTP and 
    *          RoIBuilder and is only used in the simulation!
    *
    *     @see LVL1CTP::CTPSimulation
    *     @see ROIB::RoIBuilder
    *
    *  @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
    *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 707787 $
    * $Date: 2015-11-13 16:09:29 +0100 (Fri, 13 Nov 2015) $
    */
   class CTPSLink {

   public:

      /* constructor, destructor */

      //! constructor initializing data content from given vector
      CTPSLink( const std::vector<uint32_t>& roiVec, unsigned int ctpVersionNumber );
      //! empty default destructor
      ~CTPSLink();

      // no copy or assignment allowed
      CTPSLink(CTPSLink const&) = delete;
      CTPSLink& operator=(CTPSLink const&) = delete;
     
      /* data format */

      //! number of words in header
      unsigned int getHeaderSize() const;
      //! number of words in trailer
      unsigned int getTrailerSize() const;
      //! number of words in data
      unsigned int getDataElementSize() const;
      //! toal number of words in object
      unsigned int getSize() const;

      //! retrieve CTP version number
      unsigned int getCTPVersionNumber() const { return m_ctpVersionNumber;}
     
      /* access data content */

      //! get raw header content
      const std::vector<uint32_t> getHeader() const;           
      //! get raw trailer content
      const std::vector<uint32_t> getTrailer() const;
      //! get raw data content
      const std::vector<uint32_t> getDataElements() const;
      //! get full raw data content
      const std::vector<uint32_t> getCTPToRoIBWords() const;

      /* access the trigger information */

      //! get trigger result before prescale
      const std::vector<uint32_t> getTBP() const;
      //! get trigger result after prescale
      const std::vector<uint32_t> getTAP() const;
      //! get trigger result after veto
      const std::vector<uint32_t> getTAV() const;
      //! get trigger accept before prescale
      bool getAcceptBP() const;
      //! get trigger accept after prescale
      bool getAcceptAP() const;
      //! get trigger accept after veto
      bool getAcceptAV() const;
      //! get standard trigger accept
      bool getAccept() const { return getAcceptAV(); }

      /* debug object content */

      //! dump raw object content to string
      const std::string dump() const;
      //! print object content in a human readable form to string
      const std::string print(const bool longFormat = false) const;

      /* data format constants */
      //static const unsigned int wordsPerCTPSLink;
     unsigned int getNumWordsPerCTPSLink() const { return m_wordsPerCTPSLink; }

   private:

      /** this vector contains the header, data elements and trailer */
      const std::vector<uint32_t> m_CTPSLinkVector;  //!< vector of words

      unsigned int m_ctpVersionNumber;
      CTPdataformatVersion *  m_ctpVersion;
      
      unsigned int m_wordsPerHeader;         //!< number of words per header
      unsigned int m_wordsPerDataElement;    //!< number of words per data element
      unsigned int m_wordsPerTrailer;        //!< number of words per trailer
      unsigned int m_wordsPerCTPSLink;       //!< number of words per CTPSLink

      //! convert data contetn into string (used by dump and print)
      const std::string convert(std::vector<uint32_t> data,
                                const bool longFormat = false) const;


   }; // class CTPSlink

} // namespace LVL1CTP

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LVL1CTP::CTPSLink, 6013, 0 )

#endif // TRIGT1INTERFACES_CTPSLINK_H
