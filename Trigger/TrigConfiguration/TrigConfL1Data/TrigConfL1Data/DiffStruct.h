/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_DiffStruct
#define TrigConf_DiffStruct

#include <unordered_map>

#include <string>
#include <vector>

namespace TrigConf {
   class DiffStruct {
   public:
      DiffStruct(const std::string& tagname, const std::string& name="");
      ~DiffStruct();

      void check(const std::string& attname, const std::string& lval, const std::string& rval);
      void check(const std::string& attname, int lval, int rval);
      void check(const std::string& attname, unsigned int lval, unsigned int rval);
      void check(const std::string& attname, float lval, float rval);
      void check(const std::string& attname, bool lval, bool rval);

      void addSub(DiffStruct* sub);
      void addRightOnlySub(const std::string& tagname, const std::string& name);
      void addLeftOnlySub(const std::string& tagname, const std::string& name);

      bool empty() const;

      void writeXML(std::ofstream & xmlfile, const std::string& prefix="") const;

      struct AttDiff {
         AttDiff(const std::string& attname_, const std::string& lval_, const std::string& rval_) : attname(attname_), lval(lval_), rval(rval_) {}
         std::string attname;
         std::string lval;
         std::string rval;
      };
      std::string                tagname;
      std::string                name;
      std::vector<AttDiff>       attdiffs;
      std::vector<DiffStruct*>   subs;
      typedef std::unordered_map<std::string, std::string> OnlyMap_t;
      OnlyMap_t                  rightonly;
      OnlyMap_t                  leftonly;


   };
}

#endif
