/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbTestCoolDCS.cxx
// Algorithm to demonstrate simple reading of COOL DCS data into Athena
// Richard Hawkings started 9/9/05

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "IOVDbTestAlg/IOVDbTestCoolDCS.h"

IOVDbTestCoolDCS::IOVDbTestCoolDCS(const std::string& name,
				   ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  declareProperty("AttrListFolders",m_par_atrlist);
  declareProperty("AttrListCollFolders",m_par_atrcollist);
}

IOVDbTestCoolDCS::~IOVDbTestCoolDCS()
{}

StatusCode IOVDbTestCoolDCS::initialize() {

  ATH_MSG_INFO("DCS conditions data folders to be read as AthenaAttributeList (single channel)");
  for (std::vector<std::string>::const_iterator itr=m_par_atrlist.begin();
       itr!=m_par_atrlist.end();++itr) ATH_MSG_INFO(*itr);
  ATH_MSG_INFO("DCS conditions data folders to be read as CondAttrListCollection (multichannel)");
  for (std::vector<std::string>::const_iterator itr=m_par_atrcollist.begin();
       itr!=m_par_atrcollist.end();++itr) ATH_MSG_INFO(*itr);

  return StatusCode::SUCCESS;
}

StatusCode IOVDbTestCoolDCS::execute (const EventContext& ctx) const {
  // first print event number and time details
  int time=ctx.eventID().time_stamp();
  ATH_MSG_INFO("In run/event [" << ctx.eventID().run_number() <<
               "," << ctx.eventID().event_number() << "] timestamp " << time);
  // print the timestamp in UTC
  time_t ttime=static_cast<time_t>(time);
  ATH_MSG_INFO("Timestamp UTC: " << asctime(gmtime(&ttime)));

  // print all the AthenaAttributeList
  const AthenaAttributeList* atrlist = nullptr;;
  for (std::vector<std::string>::const_iterator itr=m_par_atrlist.begin();
       itr!=m_par_atrlist.end();++itr) {
    if (StatusCode::SUCCESS==detStore()->retrieve(atrlist,*itr)) {
      // the following code dumps the attribute list into a string for printing
      // to access individual elements by name, use e.g.
      // float var1=(*atrlist)["T04"].data<float>();
      // to get the value of a float column called T04 into var1
      std::ostringstream atrstring;
      atrlist->print(atrstring);
      ATH_MSG_INFO("Values for folder " << *itr << ":");
      ATH_MSG_INFO(atrstring.str());
    } else {
      ATH_MSG_ERROR("Could not retrieve AthenaAttributeList ");
    }
  }
   
  // print all CondAttrListCollection
  const CondAttrListCollection* atrlistcol;
  for (std::vector<std::string>::const_iterator itr=m_par_atrcollist.begin();
       itr!=m_par_atrcollist.end();++itr) {
    if (StatusCode::SUCCESS==detStore()->retrieve(atrlistcol,*itr)) {
      // loop over collection
      for (CondAttrListCollection::const_iterator citr=atrlistcol->begin();
	   citr!=atrlistcol->end();++citr) {
      // the following code dumps the attribute list into a string for printing
      // to access individual elements by name, use e.g.
      // float var1=(((*citr).second)["T04"]).data<float>();
      // to get the value of a float column called T04 into var1
	std::ostringstream atrstring;
	(*citr).second.toOutputStream(atrstring);
	ATH_MSG_INFO("Channel " << (*citr).first << " "
		     << atrstring.str());
      }
    } else {
      ATH_MSG_INFO("Could not retrieve CondAttrListCollection " << *itr);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode IOVDbTestCoolDCS::finalize()
{
  return StatusCode::SUCCESS;
}
