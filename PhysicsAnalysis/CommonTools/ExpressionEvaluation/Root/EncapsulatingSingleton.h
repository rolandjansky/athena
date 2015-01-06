/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// EncapsulatingSingleton.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef ENCAPSULATING_SINGLETON_H
#define ENCAPSULATING_SINGLETON_H

#include <stdexcept>

namespace ExpressionParsing {
  template <typename T> struct EncapsulatedDefault {
    typedef T type;
  };

  template <typename T> class EncapsulatingSingleton {
    public:
      static T *getInstance() {
        const EncapsulatingSingleton &instance = getSingleton();
        if (!instance.m_pointer) {
          throw std::runtime_error("EncapsulatingSingleton: Error - no instance loaded.");
        }
        return instance.m_pointer;
      }

      static void setInstance(T *proxyLoader) {
        EncapsulatingSingleton &instance = getSingleton();
        if (instance.m_pointer) {
          delete instance.m_pointer;
        }
        instance.m_pointer = proxyLoader;
      }

      static EncapsulatingSingleton &getSingleton() {
        static EncapsulatingSingleton instance;
        return instance;
      }

    private:
      EncapsulatingSingleton() { 
        typedef typename EncapsulatedDefault<T>::type Default;
        m_pointer = new Default();
      }

      ~EncapsulatingSingleton() {
        if (m_pointer) {
          delete m_pointer;
        }
      }

      EncapsulatingSingleton (const EncapsulatingSingleton&) = delete;
      EncapsulatingSingleton& operator= (const EncapsulatingSingleton&) = delete;

      T *m_pointer;
  };
}

#endif // ENCAPSULATING_SINGLETON_H
