// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file AthenaBaseComps/AthProperties.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2018
 * @brief pimpl-style holder for component properties.
 */


#ifndef ATHENABASECOMPS_ATHPROPERTIES_H
#define ATHENABASECOMPS_ATHPROPERTIES_H


/**
 * @brief pimpl-style holder for component properties.
 *
 * A wart on Gaudi properties has been that that you needed to declare the
 * property member in a different place than the declareProperty.  This has
 * been addressed by the new-style Property constructors that can implicitly
 * declare the properties.  The good news is that all the declarations are
 * now in one place.  The bad news is that it often seems to be the wrong
 * place: when one is reading the implementation for a class, it is frequently
 * jarring to have to go back to the header file in order to see if some
 * member is a property or not.
 *
 * However, using a variant of the pimpl idiom, we can move all the property
 * declarations into the implementation file.  Then we can see them easily,
 * while still having everying in one place.
 *
 * This class is intended to be used like this:
 *
 *@code
   class MySvcProps;
   class MySvc: extends<...> { ...
   private:
     std::unique_ptr<MySvcProps> m_props;

  class MySvcProps
    : public AthProperties<MySvc>
  {
  public:
    using base_class::base_class;

    Gaudi::Property<int> m_intProp (parent(), "IntProp", 14, "Some property");
  };

  MySvc::MySvc (const std::string& name, ISvcLocator* svc)
    : base_class (name, svc),
      m_props (new MySvcProps (this))
  { ...
 @endcode
 */
template <class PARENT>
class AthProperties
{
public:
  typedef AthProperties base_class;
  AthProperties (PARENT* parent) : m_parent (parent) {}
  PARENT* parent() { return m_parent; }


private:
  PARENT* m_parent;
};


#endif // not ATHENABASECOMPS_ATHPROPERTIES_H
