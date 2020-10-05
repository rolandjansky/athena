/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServiceMaterial_H
#define ServiceMaterial_H

#include <string>
#include <vector>
#include <iosfwd>

class ServiceMaterial {
public:

  /** Structure holding one service material. The name must be a valid material name,
      the number is the number of times the service is present (e.g. number of cables),
      the weight is the weight per unit of length if the flag "linear" is true, 
      and simply the weight otherwise.
      If the weight is zero it will be taken from the material DB
   */
  struct Entry {
    Entry( const std::string& nam, int num, bool lin) : name(nam), number(num), linear(lin), weight(1) {}
    Entry( const std::string& nam, int num, bool lin, double w) : name(nam), number(num), linear(lin), weight(w) {}

    std::string name;
    int         number;
    bool        linear;
    double      weight;
  };

  typedef std::vector< Entry>              EntryContainer;
  typedef EntryContainer::const_iterator   EntryIter;

  ServiceMaterial() {}

  ServiceMaterial( const std::string& name, const std::vector< Entry>& cmp) :
    m_name(name), m_components(cmp) {}

  ServiceMaterial( const std::string& name) :
    m_name(name), m_components() { m_components.reserve(6);}

  /// Constructor for elementary material (no components)
  // ServiceMaterial( const std::string& name, double lw) :
  //   m_name(name), m_linearWeight(lw), m_components() {}

  const std::string& name() const {return m_name;}

  const std::vector< Entry>& components() const {return m_components;}

  void addEntry( const Entry& e) { m_components.push_back( e);}

  void addEntry( std::string nam, int num, bool lin, double w=1) {
    m_components.push_back( Entry( nam, num, lin, w));
  }

  //ServiceMaterial operator*( double len) const {return ServiceMaterial( name(), len*linearWeight(), m_components);}
  void multiply( int factor) {
    for ( std::vector< Entry>::iterator i=m_components.begin(); i!=m_components.end(); ++i) {
      i->number *= factor;
    }
  }

  void dump( std::ostream& os) const;

private:

  std::string             m_name;
  std::vector< Entry>     m_components;

};

#endif
