/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_CTPFiles
#define TrigConf_CTPFiles

#include <string>
#include <inttypes.h>
#include <sys/types.h>
#include <vector>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
   
   class CTPFiles : public L1DataBaseclass {
      
   public:
      static u_int ALL_CTPCORELUT_SIZE;      
      static u_int ALL_CTPCORECAM_SIZE;
      
      static u_int ALL_CTPINMONSEL_SIZE;
      static u_int ALL_CTPINMONDEC_SIZE;

      static u_int CTPMON_MUX_OUTPUT_NUMBER;
      static u_int CTPMON_ADDRESS_SELECTOR_NUMBER;
      static u_int ALL_CTPMONSELECTOR_SIZE;
      static u_int ALL_CTPMONDECODER_SIZE;

     
      CTPFiles();
      ~CTPFiles();

      // Accessors
      void setFileSizes(unsigned int run);

      // return copy of vector
      std::vector<uint32_t> ctpcoreLUT();
      void setCtpcoreLUT( const std::vector<u_int>& vec);

      // return copy of vector
      std::vector<uint32_t> ctpcoreCAM();
      void setCtpcoreCAM( const std::vector<u_int>& vec );
      
      // return copy of vector
      std::vector<uint32_t> ctpinMonSelectorSlot7();
      void setCtpinMonSelectorSlot7( const std::vector<u_int>& vec );
      
      std::vector<uint32_t> ctpinMonSelectorSlot8();
      void setCtpinMonSelectorSlot8( const std::vector<u_int>& vec );
     
      std::vector<uint32_t> ctpinMonSelectorSlot9();
      void setCtpinMonSelectorSlot9( const std::vector<u_int>& vec );

      // return copy of vector
      std::vector<uint32_t> ctpmonSelector();
      void setCtpmonSelector( const std::vector<u_int>& vec );
          
      // return copy of vector
      std::vector<uint32_t> ctpinMonDecoderSlot7();
      void setCtpinMonDecoderSlot7( const std::vector<u_int>& vec );
      
      // return copy of vector
      std::vector<uint32_t> ctpinMonDecoderSlot8();
      void setCtpinMonDecoderSlot8( const std::vector<u_int>& vec );
     
      // return copy of vector
      std::vector<uint32_t> ctpinMonDecoderSlot9();
      void setCtpinMonDecoderSlot9( const std::vector<u_int>& vec );

      // return copy of vector
      std::vector<uint32_t> ctpmonDecoder();
      void setCtpmonDecoder( const std::vector<u_int>& vec );

      int lvl1MasterTableId() const {return m_Lvl1MasterTableId;}
      void setLvl1MasterTableId (const int& id){m_Lvl1MasterTableId=id;}

      int smxId() const {return m_smxId;}
      void setSmxId (const int& id){m_smxId=id;}

      std::string smxName() const {return m_smxName;}
      void setSmxName (const std::string& str) {m_smxName = str;}

      const std::string& ctpinSmxVhdlSlot7() const { return m_ctpinSmxVhdlSlot7; }
      const std::string& ctpinSmxVhdlSlot8() const { return m_ctpinSmxVhdlSlot8; }
      const std::string& ctpinSmxVhdlSlot9() const { return m_ctpinSmxVhdlSlot9; }
      const std::string& ctpinSmxSvfiSlot7() const { return m_ctpinSmxSvfiSlot7; }
      const std::string& ctpinSmxSvfiSlot8() const { return m_ctpinSmxSvfiSlot8; }
      const std::string& ctpinSmxSvfiSlot9() const { return m_ctpinSmxSvfiSlot9; }
      const std::string& ctpinSmxOutput()    const { return m_ctpinSmxOutput; }

      void setCtpinSmxVhdlSlot7(const std::string& str) {m_ctpinSmxVhdlSlot7 = str;}
      void setCtpinSmxVhdlSlot8(const std::string& str) {m_ctpinSmxVhdlSlot8 = str;}
      void setCtpinSmxVhdlSlot9(const std::string& str) {m_ctpinSmxVhdlSlot9 = str;}
      void setCtpinSmxSvfiSlot7(const std::string& str) {m_ctpinSmxSvfiSlot7 = str;}
      void setCtpinSmxSvfiSlot8(const std::string& str) {m_ctpinSmxSvfiSlot8 = str;}
      void setCtpinSmxSvfiSlot9(const std::string& str) {m_ctpinSmxSvfiSlot9 = str;}
      void setCtpinSmxOutput(const std::string& str) {m_ctpinSmxOutput = str;}

      bool loadCtpcoreFiles() const {return m_LoadCtpcoreFiles;}
      void setLoadCtpcoreFiles (bool flag = true) { m_LoadCtpcoreFiles=flag; }
      bool loadCtpinFiles() const {return m_LoadCtpinFiles;}
      void setLoadCtpinFiles (bool flag = true) { m_LoadCtpinFiles=flag; }
      bool loadCtpmonFiles() const {return m_LoadCtpmonFiles;}
      void setLoadCtpmonFiles (bool flag = true) { m_LoadCtpmonFiles=flag; }

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      
   private:
      int                   m_Lvl1MasterTableId;
      std::vector<uint32_t> m_ctpcoreLUT;
      std::vector<uint32_t> m_ctpcoreCAM;
      std::vector<uint32_t> m_ctpinMonSelectorSlot7;
      std::vector<uint32_t> m_ctpinMonSelectorSlot8;
      std::vector<uint32_t> m_ctpinMonSelectorSlot9;
      std::vector<uint32_t> m_ctpmonSelector;
      std::vector<uint32_t> m_ctpinMonDecoderSlot7;
      std::vector<uint32_t> m_ctpinMonDecoderSlot8;
      std::vector<uint32_t> m_ctpinMonDecoderSlot9;
      std::vector<uint32_t> m_ctpmonDecoder;

      int m_smxId;
      std::string m_smxName;
      std::string m_ctpinSmxVhdlSlot7;
      std::string m_ctpinSmxVhdlSlot8;
      std::string m_ctpinSmxVhdlSlot9;
      std::string m_ctpinSmxSvfiSlot7;
      std::string m_ctpinSmxSvfiSlot8;
      std::string m_ctpinSmxSvfiSlot9;
      std::string m_ctpinSmxOutput;

      bool m_LoadCtpcoreFiles;
      bool m_LoadCtpinFiles;
      bool m_LoadCtpmonFiles;
   };
}
#endif 
  
