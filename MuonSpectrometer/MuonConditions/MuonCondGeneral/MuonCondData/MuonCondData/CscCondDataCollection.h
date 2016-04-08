/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCONDDATACOLLECTION_H 
#define CSCCONDDATACOLLECTION_H
/**
  @class CscCondDataCollection

  @author lampen@physics.arizona.edu

  @brief Contains all the information for a single CSC COOL parameter. 

  Holds conditions constants from the cool database. Designed for internal use by CscCoolStrSvc.
 **/
#include "DataModel/DataVector.h"
//#include "CLIDSvc/CLASS_DEF.h"
#include "MuonCondData/CscCondData.h"
#include "MuonCondData/CscCondDataCollectionBase.h"
#include <sstream>



namespace MuonCalib {
  //Might be a neater way to do this. For now, this lets the collection name its
  //type
  template<typename type> class typeNamer {
    public:
      std::string nameType() const {
        return "unknownType";
      }
  };
  
  template<> class typeNamer<uint8_t> {
    public:
      std::string nameType() const {
        return "uint8_t";
      }
  };
  
  template<> class typeNamer<uint16_t> {
    public:
      std::string nameType() const {
        return "uint16_t";
      }
  };
  
  template<> class typeNamer<uint32_t> {
    public:
      std::string nameType() const {
        return "uint32_t";
      }
  };
  
  template<> class typeNamer<int> {
    public:
      std::string nameType() const {
        return "int";
      }
  };
  
  template<> class typeNamer<float> {
    public:
      std::string nameType() const {
        return "float";
      }
  };
  
  template<> class typeNamer<bool> {
    public:
      std::string nameType() const {
        return "bool";
      }
  };
  
  template<> class typeNamer<double> {
    public:
      std::string nameType() const {
        return "double";
      }
  };
  
  template<> class typeNamer<std::string> {
    public:
      std::string nameType() const {
        return "string";
      }
  };

  template <typename data_type>
    class CscCondDataCollection :   public CscCondDataCollectionBase, public DataVector< CscCondData<data_type> >  {
      public:
        CscCondDataCollection(SG::OwnershipPolicy own = SG::OWN_ELEMENTS)
          : DataVector<CscCondData<data_type> >(own), m_parDefault(data_type()) {}
        virtual ~CscCondDataCollection() {}

        //Reinitializes internal DataVector to point to empty entries, but 
        //keeps size the same (if size was set with setSize())
        virtual inline void reset() {
          this->clear();
          this->resize(m_size);
          //std::cout << "Reset " << this->getParName() << ". Value of first index is " << (*this)[0] << std::endl;

        }

        //set parameter default from a string
        virtual void setParDefault(const std::string & parDefault) {
          std::istringstream ss(parDefault);
          ss >> m_parDefault;


        } 

        //virtual void print();
        virtual std::string getParDataType() const { 
          return m_typeNamer.nameType();
        }


        virtual const data_type & getParDefault() const { return m_parDefault;}

        virtual void setSize(const unsigned int & size) { 
          m_size = size;
          this->resize(size); 
        }
        virtual size_t getSize() const { return this->size(); }


        StatusCode recordFromSS(std::istringstream & ss,const unsigned int &index);
        StatusCode record(const data_type &, const int & index);
        
        //check that there is a non-null pointer at index
        virtual inline int check(unsigned int & index) const {
          if(index >= this->size())
            return 0;
          //std::cout << "checking index" << index << " of "  << this->getParName() << ". have " << (*this)[index] << "\n";

          if((*this)[index])
            return 1;
          else
            return 0;
        }
      
      private:
        typeNamer<data_type> m_typeNamer;
        data_type m_parDefault;


    };


  //Retrieves a single word from an istreamstream, stores the value into CscCondDataCollection
  template <typename data_type>
    inline StatusCode CscCondDataCollection<data_type>::recordFromSS(std::istringstream & ss, const unsigned int & index) {
      data_type data;
      ss >> data;
      return record(data, index);
    }

  //Store a bit of data into CscCondDataCollection
  template <typename data_type>
    inline StatusCode CscCondDataCollection<data_type>::record(const data_type & data, const int &index) {

      //std::cout << "Storing [" << index << "] - " << data << std::endl;
      //std::cout << "Currently we have pointer to: " << this->at(index) << std::endl;

      //Check to see if this channel is already used. Don't allow overwriting in order to 
      //catch bugs
      if(this->at(index))
        return StatusCode::RECOVERABLE;
      this->at(index) = new CscCondData<data_type>(data);
      return StatusCode::SUCCESS;
    }

  //CLASS_DEF(MuonCalib::CscCondDataContainer, 1173850174, 1)

}//end namespace MuonCalib
#endif
