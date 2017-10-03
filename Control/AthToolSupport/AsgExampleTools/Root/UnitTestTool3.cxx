//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgExampleTools/UnitTestTool3.h>

#include <AsgExampleTools/IUnitTestTool1.h>
#include <AsgExampleTools/IUnitTestTool2.h>

//
// method implementations
//

namespace asg
{
  UnitTestTool3 ::
  UnitTestTool3 (const std::string& val_name)
    : AsgTool (val_name),
      m_subtool1 ("asg::UnitTestTool1/subtool1", this),
      m_subtool2 ("asg::UnitTestTool2/subtool2", this)
  {
    declareProperty ("propertyName", m_propertyName = "regPublicHandle");
    declareProperty ("usePublic", m_usePublic);
    declareProperty ("subtool0", m_subtool0);

    m_subtool1.setAllowEmpty ();
    m_subtool1.declarePropertyFor (this, "subtool1");
  }



  StatusCode UnitTestTool3 ::
  initialize ()
  {
    if (!m_usePublic)
    {
      ANA_CHECK (m_subtool1.setProperty ("propertyInt", 42));
      // ANA_CHECK (m_subtool1.initialize ());
      ANA_CHECK (m_subtool2.setProperty (m_propertyName, m_subtool1));
    } else
    {
      ANA_MSG_INFO ("using public handle");
      ANA_CHECK (m_subtool2.setProperty (m_propertyName, m_subtool0));
    }
    ANA_CHECK (m_subtool2.setProperty ("allowEmpty", true));
    ANA_CHECK (m_subtool2.initialize ());

    return StatusCode::SUCCESS;
  }



  const IUnitTestTool1 *UnitTestTool3 ::
  getSubtool () const
  {
    if (m_usePublic)
      return &*m_subtool0;
    else
      return m_subtool1.get();
  }



  const IUnitTestTool1 *UnitTestTool3 ::
  getSubsubtool () const
  {
    return m_subtool2->getToolHandle (m_propertyName);
  }

  bool UnitTestTool3 ::
  subsubtoolEmpty () const
  {
    return m_subtool2->toolHandleEmpty (m_propertyName);
  }
}
