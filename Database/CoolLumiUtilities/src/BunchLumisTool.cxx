/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/BunchLumisTool.h"

#include "CoolKernel/IObject.h"
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "AthenaKernel/errorcheck.h"

// Total number of BCIDs in one turn - this must match value used in /TDAQ/OLC/BUNCHLUMIS for storage mode 1
const unsigned int TOTAL_LHC_BCIDS = 3564;

BunchLumisTool::BunchLumisTool(const std::string& type,
			     const std::string& name,
			     const IInterface* parent) :
  AthAlgTool(type, name, parent), 
  m_bunchLumisFolderName(""), // /TDAQ/OLC/BUNCHLUMIS"),
  m_reloadRawLumi(true),
  m_channel(0),
  m_avgRawLumi(0.),
  m_fillParamsTool("") // FillParamsTool")
{
  declareInterface<IBunchLumisTool>(this);
  declareProperty("BunchLumisFolderName", m_bunchLumisFolderName);
  declareProperty("FillParamsTool", m_fillParamsTool);

  m_rawLumi = std::vector<float>(TOTAL_LHC_BCIDS, 0.);
}

StatusCode
BunchLumisTool::initialize()
{
  ATH_MSG_DEBUG("BunchLumisTool::initialize() begin");

  if (m_bunchLumisFolderName.empty()) {
    // Not configured, may be OK
    ATH_MSG_INFO("BunchLumisFolderName.empty() is TRUE, skipping...");
  } else {
    ATH_MSG_INFO("BunchLumisTool::initialize() registering " << m_bunchLumisFolderName);

    // Setup callback on COOL folder change
    // Here, callback just sets m_reloadRawLumi flag, as many things may change at once

    // This must be done with the interface (IBunchLumisTool) so external code can use this to trigger
    // their own callbacks

    if (detStore()->contains<CondAttrListCollection>(m_bunchLumisFolderName)) {

      const DataHandle<CondAttrListCollection> aptr;

      CHECK(detStore()->regFcn(&IBunchLumisTool::updateCache, dynamic_cast<IBunchLumisTool*>(this) , aptr, m_bunchLumisFolderName));

      ATH_MSG_INFO( " Registered a callback for " << m_bunchLumisFolderName << " COOL folder " );
    } else {
      ATH_MSG_ERROR( " cannot find " << m_bunchLumisFolderName << " in DetectorStore" );
    }
  }


  // Also callback for FillParamsTool (needed to unpack certain packing schemes)
  if (m_fillParamsTool.empty()) {
    ATH_MSG_INFO( "FillParamsTool.empty() is TRUE, skipping...");
  } else {
    ATH_MSG_INFO( "Retrieving FillParamsTool handle" );
    CHECK(m_fillParamsTool.retrieve());

    // Setup callback on FillParamsTool change                                                                
    ATH_MSG_INFO( "Registering callback on IFillParamsTool::updateCache" );
    CHECK(detStore()->regFcn(&IFillParamsTool::updateCache, dynamic_cast<IFillParamsTool*>(&(*m_fillParamsTool)), &IBunchLumisTool::updateCache, dynamic_cast<IBunchLumisTool*>(this)));
  }

  ATH_MSG_DEBUG( "BunchLumisTool::initialize() end" );
  return StatusCode::SUCCESS;
}

// Access functions
const std::vector<float>& 
BunchLumisTool::rawLuminosity()
{
  if (m_reloadRawLumi) reloadRawLumi();
  return m_rawLumi;
}

float
BunchLumisTool::rawLuminosity(unsigned int bcid)
{
  if (bcid >= TOTAL_LHC_BCIDS) {
    ATH_MSG_WARNING( "Request for lumi with bcid " << bcid << " > " << TOTAL_LHC_BCIDS << "!" );
    return 0.;
  }

  if (m_reloadRawLumi) reloadRawLumi();
  return m_rawLumi[bcid];
}

float
BunchLumisTool::averageRawLuminosity()
{
  if (m_reloadRawLumi) reloadRawLumi();
  return m_avgRawLumi;
}

void
BunchLumisTool::setChannel(unsigned int channel)
{
  if (channel != m_channel) m_reloadRawLumi = true;
  m_channel = channel;
}

// Callback function
StatusCode
BunchLumisTool::updateCache( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  
  ATH_MSG_DEBUG( "in updateCache()");

  // Just set flag indicating that we need to recalculate things next time a data access is made
  m_reloadRawLumi = true;
  return StatusCode::SUCCESS;
}

void
BunchLumisTool::reloadRawLumi() 
{

  ATH_MSG_DEBUG( "in reloadRawLumi() for channel: " << m_channel);

  // Clear the request flag
  m_reloadRawLumi = false;

  // Clear old data
  m_avgRawLumi = 0.;
  m_rawLumi = std::vector<float>(TOTAL_LHC_BCIDS, 0.);

  if (m_bunchLumisFolderName.empty()) {
    ATH_MSG_WARNING("in reloadRawLumi() but BunchLumisFolderName.empty()!");
    return;
  }

  // Check data from FillParamsTool (comment out, not guaranteed to be configured)
  // ATH_MSG_DEBUG( "N LuminousBunches:" << m_fillParamsTool->nLuminousBunches() );
  // ATH_MSG_DEBUG( m_fillParamsTool->luminousBunches()[0] << " " <<  m_fillParamsTool->luminousBunches()[1] );

  // Get the raw data for the preferred channel
  const CondAttrListCollection* attrListColl = 0;
  StatusCode sc = detStore()->retrieve(attrListColl, m_bunchLumisFolderName);
  if (sc.isFailure() || !attrListColl) {
    ATH_MSG_WARNING( "attrListColl not found for " << m_bunchLumisFolderName );
    return;
  }

  // Loop over collection and find preferred channel                                                        
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
    cool::UInt32 channel = (*first).first;
    if (channel != m_channel) continue;
    break;
  }

  // Check if we didn't find the preferred channel                                                          
  if (first == last) {
    ATH_MSG_WARNING( "BUNCHLUMI data not found for channel " << m_channel << "!" <<endreq );
    return;
  }

  // attrList is the payload of /TDAQ/OLC/BUNCHLUMIS                                                        
  const coral::AttributeList& attrList = (*first).second;

  // Make sure the blob exists                                                                              
  if (attrList["BunchRawInstLum"].isNull()) {
    ATH_MSG_WARNING( "BunchRawInstLumi blob not found for channel " << m_channel << "!" );
    return;
  }

  const coral::Blob& blob = attrList["BunchRawInstLum"].data<coral::Blob>();
  if (blob.size() == 0) {
    ATH_MSG_WARNING( "BunchRawInstLumi blob found with zero size for channel " << m_channel << "!" );
    return;
  }

  // Make sure the scale factor exists (needed below to unpack integer blob schemes)                        
  if (attrList["AverageRawInstLum"].isNull()) {
    ATH_MSG_WARNING( "AverageRawInstLum value not found for channel " << m_channel << "!" );
    return;
  }
  m_avgRawLumi = attrList["AverageRawInstLum"].data<cool::Float>();

  // Don't need to check validity as average value would already be invalid?  Or data wouldn't exist?  we will see

  //                                                                                                        
  // Unpack raw luminosity data blob here                                                                   
  //                                                                                                        

  // Figure out Mika's blob packing mode (should really have a utility for this)                            
  // See description: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolOnlineData#Folder_TDAQ_OLC_BUNCHLUMIS
                                                                                                         
  const char* pchar = static_cast<const char*>(blob.startingAddress()); // First byte holds storage size and mode
  unsigned int bss = ((*pchar) % 100) / 10;  // Byte storage size
  unsigned int smod = ((*pchar) % 10);       // Storage mode 

  ATH_MSG_DEBUG( "BunchRawInstLumi blob found with storage mode " << smod << " and byte storage size " << bss );

  // Check blob length and point pchar to start of raw lumi data 
  unsigned int bloblength = 0;
  unsigned int nbcids = 0;

  // Used below for smod==2 scheme, also advance pchar past first byte
  const uint16_t* p16 = (const uint16_t*)(++pchar);

  // Treat different storage modes independently  
  switch (smod) {
  case 0:
    // Packed according to luminousBunches        
    // Make sure tool is configured
    if (m_fillParamsTool.empty()) {
      ATH_MSG_WARNING("Can't unpack using luminousBunches, as FillParamsTool is not configured!");
      return;
    }

    nbcids = m_fillParamsTool->nLuminousBunches();
    bloblength = bss * nbcids + 1;
    break;

  case 1:
    // Full orbit stored
    nbcids = TOTAL_LHC_BCIDS;
    bloblength = bss * nbcids + 1;
    break;

  case 2:
    // Self describing length, with 2-byte length followed by 2-byte BCID vector, then data
    nbcids = *p16++;
    bloblength = (2+bss)*nbcids + 3;  // 2-bytes for vector plus bss plus 2 bytes for vector length, plus one byte for packing          
    pchar += 2*(nbcids+1); // Advance pchar past bicd vector list to raw data, p16 used below to get BCID vector list
    // ATH_MSG_DEBUG( "Found mode 2 with " << nbcids << " BCIDs" );
    break;

  default:
    ATH_MSG_WARNING( "BunchRawInstLumi blob found with unknown storage mode " << smod << "!" );
    return;
  }
  
  // Check blob size against needed length.  Give up if these don't match 
  if (static_cast<cool::UInt32>(blob.size()) != bloblength) {
    ATH_MSG_WARNING( "BunchRawInstLumi blob found with length" << blob.size() << "in storage mode" << smod <<  ", expecting " << bloblength << "!" );
    return;
  }

  // Length is correct, read raw data according to packing scheme
  // Some schemes are relative and must be renormalized, while the float/double schemes are absolute values - *pay attention!*           
  std::vector<float> rawLumi;
  rawLumi.clear();

  // Must define pointers outside of switch
  const uint16_t* p2 = (const uint16_t*) pchar;
  const float* p4 = (const float*) pchar;
  const double* p8 = (const double*) pchar;

  // Different depending upon bytes allocated (this is ugly, but it works)
  // pchar is pointing to the start of the data (past first byte of blob)
  switch (bss) {

  case 1: // 1-byte integers, just use pchar  
    for (unsigned int i=0; i<nbcids; i++, pchar++) {
      float val = (*pchar) * m_avgRawLumi / pow(100, bss);
      rawLumi.push_back(val);
    }
    break;

  case 2: // 2-byte integers
    for (unsigned int i=0; i<nbcids; i++, p2++) {
      float val = (*p2) * m_avgRawLumi / pow(100, bss);
      rawLumi.push_back(val);
    }
    break;

  case 4: // 4-byte floats
    for (unsigned int i=0; i<nbcids; i++, p4++) rawLumi.push_back(*p4);
    break;

  case 8: // 8-byte doubles
    for (unsigned int i=0; i<nbcids; i++, p8++) rawLumi.push_back(*p8);
    break;

  default:
    ATH_MSG_WARNING( "BunchRawInstLumi blob found with unknown byte storage size " << bss << "!" );
    return;
  }

  // Now figure which BCIDs these values belong to and fill into vector indexed by BCID

  // Remember, nbcids was set before and the blob size was checked 
  switch (smod) {
  case 0:
    // Packed according to luminous bunches, fill accordingly
    // Checked before that FillParamsTool is configured, don't need to check again
    for (unsigned int i=0; i<nbcids; i++)
      m_rawLumi[m_fillParamsTool->luminousBunches()[i]] = rawLumi[i];
    break;

  case 1:
    // Packed according to full turn, just copy   
    m_rawLumi = rawLumi;
    break;

  case 2:
    // Packed according to private list, must read here.  p16 points to start of this data    
    for (unsigned int i=0; i<nbcids; i++, p16++) {
      m_rawLumi[*p16] = rawLumi[i];
      // ATH_MSG_DEBUG( "BCID: " << *p16 << " Lumi= " << rawLumi[i] );
    }
    break;

    // This error condition was delt with before   
    //default:
    //return;
  }


}

