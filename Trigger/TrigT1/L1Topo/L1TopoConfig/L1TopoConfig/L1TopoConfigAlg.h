/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TopoCore__L1TopoConfigAlg__
#define __TopoCore__L1TopoConfigAlg__

#include <iostream>
#include <vector>
#include <string>

namespace TXC {
  class L1TopoConfigAlg;
}

namespace TXC {
  
std::ostream & operator<<(std::ostream &, const TXC::L1TopoConfigAlg &);

   struct InputElement {
      InputElement(const std::string & name, const std::string & value, unsigned int position) : 
         name(name), value(value), position(position) {}
      std::string  name {""};
      std::string  value {""};
      unsigned int position {0};
   };

   struct OutputElement {
      OutputElement(const std::string & name, const std::string & value, unsigned int bits, const std::string & outname, unsigned int position) : 
         name(name), value(value), nbits(bits), outname(outname), position(position) {}
      std::string  name {""};
      std::string  value {""};
      unsigned int nbits {0};
      std::string  outname {""};
      unsigned int position {0};
   };
  
   struct FixedParameter {
      FixedParameter(const std::string & name, const std::string & value) : 
         name(name), value(value) {}
      std::string name {""};
      std::string value {""};
   };

   struct RegisterParameter {
      RegisterParameter(const std::string & name, const std::string & value, unsigned int position, unsigned int selection) : 
         name(name), value(value), position(position), selection(selection) {}
      std::string  name {""};
      std::string  value {""};
      unsigned int position {0};
      unsigned int selection {0};
   };



   class L1TopoConfigAlg {
   public:

      enum AlgKind { NONE = 0, SORT = 1, DECISION = 2 };
     
      // default constructor
      L1TopoConfigAlg(const std::string & name, const std::string & type); 

      // move constructor
      L1TopoConfigAlg(L1TopoConfigAlg&&) noexcept = default;
      L1TopoConfigAlg& operator=(L1TopoConfigAlg&&) noexcept = default;

      // destructor
      virtual ~L1TopoConfigAlg();
    
      // accessors for algo
      const std::string & name() const { return m_name; }
      unsigned int        algoID() const { return m_algoID; }
      std::string         fullname() const;

      const std::string & type() const { return m_type; }
      const std::string & output() const { return m_output; }
      bool isSortAlg() const { return m_kind == SORT; }
      bool isDecAlg() const { return m_kind == DECISION; }

      // accessors for algo inputs
      const std::vector<InputElement> & getInputs() const { return m_inputElements; }
      std::vector<std::string> getInputNames() const;
  
      // accessors for algo outputs
      const std::vector<OutputElement> & getOutputs() const { return m_outputElements; }
      std::vector<std::string> getOutputNames() const;
   
      // accessors for fixed paramters (those which can not be changed through registers)
      const std::vector<FixedParameter> & getFixedParameters() const { return m_fixedParameters; }

      // accessors for algo parameters
      const std::vector<RegisterParameter> & getParameters() const { return m_variableParameters; }


      // algorithm setters
      void setAlgName(const std::string & name) { m_name = name; }
      void setAlgType(const std::string & type) { m_type = type; }
      void setAlgOutput(const std::string & output) { m_output = output; }
      void setAlgoID( unsigned int algoID) { m_algoID = algoID; }
      void setAlgKind(AlgKind kind) { m_kind = kind; }

      // algorithm parameter setters
      void addInput(const std::string & name, const std::string &value, unsigned int position);
      void addOutput(const std::string &name, const std::string &value, unsigned int bits, const std::string & outname, unsigned int position);
      void addFixedParameter(const std::string &name, const std::string &value);
      void addParameter(const std::string &name, const std::string &value, unsigned int position, unsigned int selection);
    
   private:

      L1TopoConfigAlg() {};
    
      friend std::ostream & operator<<(std::ostream &, const TXC::L1TopoConfigAlg &);
    
      // algorithm attributes
      std::string m_name {""};
      std::string m_type {""};
      std::string m_output {""};
      unsigned int m_algoID {0};
      AlgKind m_kind { AlgKind::NONE };

      // parameter attributes
      std::vector<TXC::InputElement> m_inputElements;
      std::vector<TXC::OutputElement> m_outputElements;
      std::vector<TXC::FixedParameter> m_fixedParameters;
      std::vector<TXC::RegisterParameter> m_variableParameters;
    
   };

std::ostream & operator<<(std::ostream &, const InputElement&);
std::ostream & operator<<(std::ostream &, const OutputElement&);
std::ostream & operator<<(std::ostream &, const FixedParameter&);
std::ostream & operator<<(std::ostream &, const RegisterParameter&);

}


#endif /* defined(__TopoCore__TopoAlg__) */
