/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_TrigConfData
#define TrigConf_TrigConfData

#include <iosfwd>
#include <string>

namespace TrigConf {  

   class TrigConfData {
   public:
      TrigConfData(const std::string& name = "");
      virtual ~TrigConfData() = default;

      // getters
      int superMasterTableId() const {return (int)m_smk;}
      unsigned int smk() const {return m_smk;}
      unsigned int id() const {return m_id;}
      const std::string&  name() const {return m_name;}
      const std::string&  comment() const {return m_comment;}
      unsigned int version() const {return m_version;}

      // setters
      void setSuperMasterTableId(int id) {m_smk=id;}
      void setSMK(int id) {m_smk=id;}
      void setId(unsigned int id) { m_id=id; }
      void setName( const std::string& name ){ m_name = name;}
      void setVersion( unsigned int version ){ m_version = version;}
      void setComment( const std::string& c ){ m_comment = c;}

      virtual void print(const std::string& indent="", unsigned int detail=1) const = 0;

      void printNameIdV(const std::string& indent="") const;

      virtual std::string __str__() const;
      
   protected:
      std::ostream & indent(std::ostream& o, int lvl, int size) const;
      
   private:
      unsigned int m_smk;
      unsigned int m_id;
      std::string  m_name;
      unsigned int m_version;
      std::string  m_comment;

   };

   std::ostream & operator<<(std::ostream &, const TrigConfData &);
}

#endif
