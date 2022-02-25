/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTSignature
#define TrigConf_HLTSignature

#include <iosfwd>
#include <fstream>
#include <string>
#include <vector>

namespace TrigConf {

   class HLTSignature;
   class HLTTriggerElement;

   std::ostream & operator<<(std::ostream &, const HLTSignature &);

   /**@brief HLT signature configuration information
    * 
    * A signature in the HLT defines the trigger pattern that must be
    * fullfilled if the HLT trigger is to pass a certain step in the
    * execution of the trigger chain. A signature consists of a list of
    * trigger elements and multiplicities and a logic how the trigger
    * elements are combined. Currently only simple 'OR' combinations
    * are allowed.
    */
   class HLTSignature {

   public:

      /**@brief default constructor*/
      HLTSignature( void );

      /**@brief constructor with configuration data
       *
       * @param signature_counter signature counter (step in the chain)
       *
       * @param logic signature logic (how to combine trigger elements - currently only '1'='OR' allowed)
       *
       * @param outputTEs list of trigger elements that form the signature
       */
      HLTSignature( unsigned int signature_counter, int logic,
                    std::vector<HLTTriggerElement*>&& outputTEs  );

      /**@brief copy constructor
       *
       * deep copy of all TEs
       **/
      HLTSignature( const HLTSignature& o);

      /**@brief destructor*/
      ~HLTSignature( void );

      // accessors
      unsigned int                           signature_counter() const { return m_signature_counter; } ///< accessor to the signature counter
      unsigned int                           step() const { return m_signature_counter; } ///< accessor to the signature counter
      int                                    logic() const { return m_logic; }             ///< accessor to the signature logic
      std::vector<HLTTriggerElement*>&       outputTEs() { return m_outputTEs; }         ///< accessor to the list of trigger elements
      const std::vector<HLTTriggerElement*>& outputTEs() const { return m_outputTEs; }         ///< const accessor to the list of trigger elements
      const std::string&                     label() const { return m_label; }             ///< accessor to the signature label

      void set_signature_counter( unsigned int sc ) { m_signature_counter = sc; } 
      void set_logic( int logic ) { m_logic = logic; }
      void set_outputTEs( const std::vector<HLTTriggerElement*>& outputTEs ) { m_outputTEs = outputTEs; }
      void set_label( const std::string& label ) { m_label = label; }

      /**@brief print method*/
      void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ofstream & xmlfile);

   private:

      unsigned int                    m_signature_counter; ///< signature counter (step in the chain)                             
      int                             m_logic;             ///< signature logic (how to combine trigger elements)
      std::vector<HLTTriggerElement*> m_outputTEs;         ///< list of trigger elements (same elements can appear multiple times to indicate their multiplicities)
      std::string                     m_label;             ///< signature label                                  
      //int                             m_predecessor_id;    ///< ID of the previous signature (old steering, depreciated)

      friend std::ostream & operator<<(std::ostream &, const HLTSignature &);
   };

}

#endif
