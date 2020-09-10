/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TopoCore_L1TopoConfigOutputList
#define TopoCore_L1TopoConfigOutputList

#include <vector>
#include <string>
#include <unordered_set>

namespace TXC {
  

   class TriggerLine {
   public:
      TriggerLine(const std::string& name,  
                  const std::string& algoname,
                  unsigned int algoId,
                  unsigned int module,
                  unsigned int fpga,
                  unsigned int clock,
                  unsigned int bit) :
         m_name(name),
         m_algoname(algoname),
         m_algoId(algoId),
         m_module(module),
         m_fpga(fpga),
         m_clock(clock),
         m_bit(bit)
      {
         calcCounter();
      };
      const std::string & name()     const { return m_name; }
      const std::string & algoname() const { return m_algoname; }
      unsigned int        algoid()   const { return m_algoId; }
      unsigned int        module()   const { return m_module; }
      unsigned int        fpga()     const { return m_fpga; }
      unsigned int        clock()    const { return m_clock; }
      unsigned int        bit()      const { return m_bit; }
      unsigned int        counter()  const { return m_counter; } // unique counter between 0 and 191 - to be used everywhere

   private:
      std::string  m_name{""};
      std::string  m_algoname{""};
      unsigned int m_algoId{0};
      unsigned int m_module{0};
      unsigned int m_fpga{0};
      unsigned int m_clock{0};
      unsigned int m_bit{0};
      unsigned int m_counter{0};

      void calcCounter();

   };

   class OutputListElement {
   public:
      OutputListElement(const std::string & algoname, 
                        unsigned int algoId,
                        unsigned int module,
                        unsigned int fpga,
                        unsigned int clock,
                        unsigned int firstBit) : 
         m_algoname(algoname),
         m_algoId(algoId),
         m_module(module),
         m_fpga(fpga),
         m_clock(clock),
         m_firstBit(firstBit)
      {}
      
      const std::string & algoname() const { return m_algoname; }
      unsigned int        algoid()   const { return m_algoId; }
      unsigned int        module()   const { return m_module; }
      unsigned int        fpga()     const { return m_fpga; }
      unsigned int        clock()    const { return m_clock; }
      unsigned int        firstbit() const { return m_firstBit; }
   private:
      std::string  m_algoname {""};
      unsigned int m_algoId {0};
      unsigned int m_module {0};
      unsigned int m_fpga {0};
      unsigned int m_clock {0};
      unsigned int m_firstBit {0};
   };


   class L1TopoConfigOutputList {
   public:
    
      // default constructor
      L1TopoConfigOutputList() {};

      // destructor
      virtual ~L1TopoConfigOutputList() {};
    
      //move constructors
      L1TopoConfigOutputList(L1TopoConfigOutputList&&) noexcept = default;
      L1TopoConfigOutputList& operator=(L1TopoConfigOutputList&&) noexcept = default;

      // get all OutputList elements
      const std::vector<OutputListElement> & getOutputList() const { return m_outputListElements; }
   
      const std::vector<TriggerLine> & getTriggerLines() const { return m_triggerlines; }
   
      bool hasTrigger(const std::string & trigger) const;
      const TXC::TriggerLine & getTrigger(const std::string & trigger) const;
   
      void addOutputListElement(const OutputListElement & output);
    
      void addTriggerLine(const TriggerLine & trigger);
    
      void sort();

   private:
    
      // OutputList elements stored here
      std::vector<OutputListElement> m_outputListElements;

      std::vector<TriggerLine> m_triggerlines;

      TXC::TriggerLine m_undef{"UNDEF","",0,0,0,0,0};

      std::unordered_set<std::string> m_triggernames;
      std::unordered_set<unsigned int> m_triggercounters;
   };

std::ostream & operator<<(std::ostream &, const L1TopoConfigOutputList &);

std::ostream & operator<<(std::ostream &, const TriggerLine &);

} // end of namespace TXC


#endif 
