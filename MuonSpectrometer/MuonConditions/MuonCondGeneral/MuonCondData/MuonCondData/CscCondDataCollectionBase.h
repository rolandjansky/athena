/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCONDDATACOLLECTIONBASE_H 
#define CSCCONDDATACOLLECTIONBASE_H
/**
        @class CscCondDataCollectionBase

        @author lampen@physics.arizona.edu

        @brief Base class for CscCondDataCollection.. 

        Exists so that CscCondDataCollection objects of different types can be stored in the same vector, map, etc. 
**/

#include "AthContainers/DataVector.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonCondData/CscCondData.h"

#include <sstream>

namespace MuonCalib {
  class CscCondDataCollectionBase 
  {
    public:
      CscCondDataCollectionBase();

      virtual ~CscCondDataCollectionBase();


      virtual void reset() = 0;

      //Set user defined parameters
      virtual void setParName( const std::string & parName) {
        m_parName = parName;
      }
      virtual void setParCat( const std::string & parCat) {
        m_parCat = parCat;
      }
      virtual void setParSGKey( const std::string & parSGKey) {
        m_parSGKey = parSGKey;
      }
      /*virtual void setParDataType(const std::string & dataType) {
        m_parDataType = dataType;
        }*/
      virtual void setParFolder( const std::string & parFolder) {
        m_parFolder = parFolder;
      }

      virtual void setParDefault(const std::string & ) = 0;

      //Set info derived from user parameters
      virtual void setNumCoolChan(const int & nc) {
        m_numCoolChan = nc;
      }

      //Reinitializes internal DataVector to point to empty entries, but 
      //keeps size the same (if size was set with setSize())
      virtual void setSize(const unsigned int & size) = 0;

      //Get user defined parameters 
      const inline std::string & getParName() const { return m_parName; }
      const inline std::string & getParCat() const { return m_parCat; }
      const inline std::string & getParSGKey() const { return m_parSGKey; }
      //const std::string & getParDataType() const { return m_parDataType; }
      virtual std::string getParDataType() const =0; 
      const inline std::string & getParFolder() const { return m_parFolder; }


      //get info derived from user defined parameters
      const inline unsigned int & getNumCoolChan() const { return m_numCoolChan; } 

      //Get size
      virtual size_t getSize() const = 0; 

      //record an entry to the DataVector 
      virtual StatusCode recordFromSS(std::istringstream & ss, const unsigned int & index) = 0;

      //check that there is a non-null pointer at index
      virtual int check(unsigned int & index) const =0;


      int setBitInfo(int numBits = 0, int offset =0 ); 

      int getNumBits() { return m_numBits;}
      int getBitOffset() { return m_bitOffset;}

    protected: 

      std::string m_parName;
      std::string m_parFolder;
      std::string m_parSGKey;
      std::string m_parCat;
      std::string m_parDataType;

      unsigned int m_numCoolChan;
      unsigned int m_size;//used in derived class

      int m_numBits;
      int m_bitOffset;
  };


}

//CLASS_DEF(MuonCalib::CscCondDataCollection, 1173850174, 1)

#endif
