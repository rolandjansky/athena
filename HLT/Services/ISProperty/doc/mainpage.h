/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

The ISProperty packages allows the user to collect a set of Gaudi properties from a local or a remote class and publish them as an IS object to an IS server. 
A bi-directional synchronisation mechanism is installed automatically as well as OKS schema files describing the ISObject reflecting the property set.

<H2>Declaring IS properties</H2>
<H3>in the source code</H3>
If the object that collects the properties is derived from ISPropertyMgr, the user can use these three templated functions to declare a Property
\li Declare an property the will be available both in IS as well as in Gaudi (i.e. as jobOption)
\code
template<class TYPE> Property* declareISProperty (const std::string &pname, TYPE &value, const std::string &doc="none")
\endcode
\li Declare a property that will only be available in IS, but not in Gaudi (i.e. as jobOption).
\code
template<class TYPE> Property* declareISOnlyProperty (const std::string &pname, TYPE &property, const std::string &doc="none")
\endcode
\li	Declare a remote IS property, i.e. a Gaudi property from another Service/Algorithm/AlgTool. 
\code
template<class TYPE> Property* declareISRemoteProperty (const std::string &pname, GaudiHandle< TYPE > &remoteHandle)
\endcode

<H3>in the requirements file</H3>
A use statement for the ISProperty package needs to be included in the \c requirements file
\code
use ISProperty ISProperty-* HLT/Services
\endcode
An OKS schema file describing all properties declared in the way described above is automatically generated from the \c gen_oks_schema macro in the \c requirements file
\code
apply_pattern gen_oks_schema library=YourLibrary
\endcode

<H3>in the jobOptions</H3>
In order to control on which IS server in which Partition the property set is published, the ISPropertMgr has a ServiceHandle to an ISPropertySvc service, 
which default to the instance with name "ISPropertySvc". Hence, the server and partition can be specified in the jobOptions like this:
\code
from ISProperty.ISPropertyConf import ISPropertySvc
ServiceMgr += ISPropertySvc(PartitionName="MyPartition", ServerName="MyServer")
\endcode 
The default Partition name is \c "ATLAS", the default IS server name is \c "Athena".
If an object with the same name and type already exists at the IS server, the ISPropertyMgr will by default return a \c StatusCode::FAILURE from 
initialization causing athena to quit. This can be changed by setting
\code
ServiceMgr.ISPropertySvc.ForcePublish=True
\endcode
in which case any previously existing IS object will be overwritten. This is usefull e.g if the athena process needs to be restarted after it died unexpectedly.

<H2>Publishing IS properties</H2>
Once the set of properties is declared, it can be published and unpublished on the IS side using the 
\code
StatusCode publishProperties()
StatusCode unpublishProperties()
\endcode

\b NOTE: it is not neccessary to call \c publishProperties each time a property is updated on the Gaudi side. Updating will happen automatically if
\li the property that is changed is of type \c SimplyProperty, such as e.g. \c LongProperty, \c FloatProperty, \c BoolProperty,...
\li the property is changed using <tt>template<class TYPE> setProperty( std::string name, const TYPE& value) </tt>

\b NOTE: if one property of the property set triggers an update, this means that \b all properties of the
set will be updated on the IS side. To avoid having too many updates, it may be usefull to only declare one property as a \c SimpleProperty and
make sure this is updated last in the sequence of events

\b NOTE: the update rate of a property set can be limited by setting the \c m_updateTime parameter of the ISPropertyMgr class. This holds the minimum time that 
in seconds that needs to elapse inbetween to updates of a property set. To allow changing this parameter w/o recompilation,
it may be usefull to declare it as a Gaudi property
\code
declareProperty("UpdateTime",m_updateTime=1.,"Minimum time after which the IS object can be updated on the athena side");
\endcode
If the update time is set to zero, the IS object will be update every time.

\b NOTE: if you have several instances of athena publishing to the same IS server, you need to make sure each of the generated IS object gets a unique name.
This can be achieved in two ways
\li by giving the Gaudi object that inherits from ISPropertMgr a unique name
\li by setting the \c m_uniqueName parameter of the ISPropertyMgr class to true. In this case, a unique name will be generated from
\code
/GaudiComponentName/hostname/processID
\endcode

<H2>Handling updates on the IS side</H2>
Each time an IS object is changed at the IS server, the update handers for all properties belonging to the property set in that object will be notified. 
To react on changes on the IS side, it is thus enough to declare an update handler for these properties in the constructor
\code
  //  declare an IS property
  Property* myProp = declareISProperty("MyInt", m_myInt, "My integer value");
  // Add an update handler
  myProp->declareUpdateHandler( &ISPropTestAlg::updateHandler, this );
\endcode
and the write a handler to deal with the updates
\code
void ISPropTestAlg::updateHandler(Property& p)
{
  // do something each time MyInt is changed 
}
\endcode




@mainpage

@htmlinclude used_packages.html

@include requirements

*/
