/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADER_P5_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADER_P5_H

/** @file DataHeader_p5.h
 *  @brief This file contains the class definition for the DataHeader_p5,
 *  DataHeaderForm_p5 and DataHeaderElement_p5 classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeader_p5.h,v 1.1 2009-04-21 21:48:34 gemmeren Exp $
 **/

#include <vector>
#include <string>

/** @class DataHeaderElement_p5
 *  @brief This class provides a persistent representation for the DataHeaderElement class.
 **/
class DataHeaderElement_p5 {
public: // Constructor and Destructor
   DataHeaderElement_p5();
   DataHeaderElement_p5(const DataHeaderElement_p5& rhs);
   ~DataHeaderElement_p5();

   DataHeaderElement_p5& operator=(const DataHeaderElement_p5& rhs);

   friend class DataHeaderElementCnv_p5;

   const std::string& token() const;
   long long int oid2() const;

private:
   std::string m_token;
   long long int m_oid2;
};

/** @class DataHeaderForm_p5
 *  @brief This class provides storage for the constant fields of the persistent DataHeader class.
 **/
class  DataHeaderForm_p5 {
public: // Constructor and Destructor
   DataHeaderForm_p5();
   DataHeaderForm_p5(const DataHeaderForm_p5& rhs);
   ~DataHeaderForm_p5();

   DataHeaderForm_p5& operator=(const DataHeaderForm_p5& rhs);

   const std::vector<std::string>& map() const;
   void insertMap(const std::string& element);

   const std::vector<unsigned int>& params(unsigned int entry) const;
   void insertParam(unsigned int param, unsigned int entry);

   unsigned int size() const;
   void resize(unsigned int size);

private:
   std::vector<std::string> m_map;
   std::vector<std::vector<unsigned int> > m_uints;
};

/** @class DataHeader_p5
 *  @brief This class provides a persistent representation for the DataHeader class.
 **/
class  DataHeader_p5 {
public: // Constructor and Destructor
   DataHeader_p5();
   DataHeader_p5(const DataHeader_p5& rhs);
   ~DataHeader_p5();

   DataHeader_p5& operator=(const DataHeader_p5& rhs);

   friend class DataHeaderCnv_p5;

   const std::vector<DataHeaderElement_p5>& elements() const;
   const std::string& dhFormToken() const;
   void setDhFormToken(const std::string& formToken,
                       const DataHeaderForm_p5& dhForm);
   const std::string& dhFormMdx() const;

private:
   std::vector<DataHeaderElement_p5> m_dataHeader;
   std::string m_dhFormToken;
   std::string m_dhFormMdx;
};

#endif
