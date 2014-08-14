package atlantis.data;

import java.io.InputStream;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.event.AEventSource.ReadEventException;
import atlantis.globals.AGlobals;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;
import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;
import atlantis.utils.xml.AArrayParser;
import atlantis.utils.xml.AFloatArrayParser;
import atlantis.utils.xml.AIntArrayParser;
import atlantis.utils.xml.AStringArrayParser;
import atlantis.utils.xml.AXMLErrorHandler;
import atlantis.utils.xml.AXMLErrorHandler.ErrorState;
import java.util.Iterator;
import javax.xml.parsers.ParserConfigurationException;
import org.xml.sax.SAXNotRecognizedException;


/**
 * The reader which is used to read XML Event Files. The reader function creates
 * and returns an AEvent class. Event comes with tags which are arrays of
 * numbers. These arrays are treated by xml as character strings. The conversion
 * of the character strings to arrays of number is done here.
 */
public class AEventFromXML extends DefaultHandler {

  //the logger
  private final static ALogger logger = ALogger.getLogger(AEventFromXML.class);
  
  // Create an instance of this object - this makes sure the constructor is called.
  // FIXME: should not use constructor to initialize static state.
  private static final AEventFromXML instance = new AEventFromXML();
  
  //We also have our private error handler
  private static final AXMLErrorHandler xmlErrorHandler = new AXMLErrorHandler();
  
  //Parsing states at different XML levels
  private enum ParseState {
   WAITING_EVENT,
   WAITING_DATA,
   WAITING_ARRAY
  }
  private static ParseState parseState;
    
  //The XMLReader object
  private static XMLReader xmlReader;
  //The event that is being build
  private static AEvent event = null;
  //the source name
  private static String sourceName = null;
  
  //non-static parsing data members
  private AHashMap parameters = null;
  private String currentDataName = null;
  private String currentArrayName = null;
  private String  currentFullName = null;
  private int arrayLength = 0;
  private int currentArraySize = 0;
  private String storeGateKey = null;
  private AArrayParser arrayParser = null;
  
  private static boolean HLTIgnoredWarning = false;

 
  /**
   * Constructor
   * Creates an instance of AEventFromXML. It just sets up the XML reading
   * stuff but does not do any parsing of any XML file.
   */
  private AEventFromXML() { 

    try {
      //setup all the xml reader stuff
      SAXParserFactory factory = SAXParserFactory.newInstance();
      factory.setValidating(true);
      factory.setNamespaceAware(true);
      xmlReader = factory.newSAXParser().getXMLReader();
      xmlReader.setErrorHandler(xmlErrorHandler);
      xmlReader.setEntityResolver(this);
      xmlReader.setContentHandler(this);
    } catch (ParserConfigurationException pce){
      logger.error("Unable to initialize XML reader!", pce);
    } catch (SAXException sxe){
      logger.error("Unable to initialize XML reader!", sxe);    
    }
  } // AEventFromXML() ----------------------------------------------------
 

  /**
   * Checks if there was an error.
   * @return true or false 
   */
  private boolean hasError() {
    return (xmlErrorHandler.getErrorState() != ErrorState.NO_ERROR);
  }


  /**
   * Reads an XML event data (from files, from string converted to stream) and
   * returns the coresponding AEvent object.
   *
   * @param eventInputStream the XML data input stream.
   * @param sourceName the name of the event source that provided this data
   * @return an AEvent object containing all the data found in the file.
   * @throws ReadEventException 
   */
  public static AEvent read(InputStream eventInputStream, String sourceName) throws ReadEventException {

    //be verbose
    logger.info("Parsing event from " + sourceName);

    //set the initial error state
    xmlErrorHandler.setError(ErrorState.NO_ERROR,null);

    //store the source Name
    AEventFromXML.sourceName = sourceName;
    
    //Try to parse input stream
    try {   
      xmlReader.parse(new InputSource(eventInputStream));
    } catch (Exception e) {
      //In case of an error repackage as ReadEventException  
      logger.error("Parsing exception " + e.getMessage(),e);
      throw new ReadEventException("XML parsing exception", e);
    }

    //Finally construct the full event (including associations, etc.)
    //Check for runtime exceptions here in case we fail to finalize the event
    try {
      event.finalizeEvent();
    } catch (RuntimeException rte) {
      //rethrow as can not read event exception
      throw new ReadEventException("Inconsistent event data",rte);
    }

    return event;
  }

  /**
   * DefaultHandler Implementation (have a look at
   * org.xml.sax.helpers.DefaultHandler for more info)
   */
  @Override
  public void startDocument() {
    
    //if we are already in error state return immediately
    if (hasError()) return;
    
    //Clean up all local variables
    event = null;
    parameters = null;
    currentDataName = null;
    currentArrayName = null;
    currentFullName = null;
    arrayLength = 0;
    currentArraySize = 0;
    storeGateKey = null;
    arrayParser = null;
   
    //Set parse state to new event
    parseState = ParseState.WAITING_EVENT;

    //be verbose
    logger.debug("Start parsing document");

  } // startDocument() ----------------------------------------------------

  @Override
  public void endDocument() {
    
    //if we are already in error state return immediatly
    if (hasError()) return;
    
    //be verbose
    logger.debug("Finished parsing document w/o error");
    
  }

  /**
   * Implementation of DefaultHandler: start a new xml element
   * @param namespaceURI the namespace
   * @param localName the local name
   * @param qName the qualified name
   * @param atts the default or specified attributes
   */
  @Override
  public void startElement(String namespaceURI, String localName,
          String qName, Attributes atts) throws SAXException {

    //stop here if errors have occured before
    if (hasError()) return;
        
    try {
      //Now act depending on parse state
      switch (parseState) {
        
        //Waiting for an event to begin
        case WAITING_EVENT:
          //Try to read in event data
          if (localName.equals("Event")) {
            
            //Create a new event
            event = new AEvent(atts.getValue("eventNumber"),
                               atts.getValue("runNumber"),
                               atts.getValue("dateTime"),
                               sourceName,
			       atts.getValue("lumiBlock"),
			       atts.getValue("eventProperty"));	  

            
            //create a new data hashmap
            parameters = new AHashMap(15);
            
            //Advance to next parsing state
            parseState = ParseState.WAITING_DATA;
          
          } else {
            //The Event tag does not seem to be the first tag in this event
            logger.warn("Encountered tag "+localName+ " while waiting for <event/>!");
          }
          //Done waiting for event
          break;
        
        //Waiting for a data tag
        case WAITING_DATA:
          
          //Store the current object name
          currentDataName = localName;
          //Get number of entries expected here
          arrayLength = Integer.parseInt(atts.getValue("count"));
          
          // if storegate key doesn't exists as an attribute
          // at datatype, an empty string "" is retrieved (not null)
          storeGateKey = atts.getValue("storeGateKey");
          //Construct full name from datatype name and storeGateKey
          currentFullName = currentDataName + "-" + storeGateKey;
     
          // ignore data with storeGateKey containing "HLTAutoKey"
          // Its important that parseState is not set before,
          // so we arrive waiting for a new data tag when this error is cleared
          if (!AGlobals.instance().showHLTAutoKeys() && storeGateKey.indexOf("HLTAutoKey") >= 0) {
            //Set appropiate error state and continue
            xmlErrorHandler.setError(ErrorState.IGNORED_DATA, new Exception("Ignored HLTAutoKey data "+currentFullName));
            return;
          }
          
          parameters.clear();
          // add numData - number of data within the subtags,
          // extracted from count attribute
          parameters.put("numData", new Integer(arrayLength));
          parameters.put("storeGateKey", storeGateKey);
          parseState = ParseState.WAITING_ARRAY; 
          break;
          
        //Waiting for an data value array
        case WAITING_ARRAY:
          
          //store name of the array we are looking at
          currentArrayName = localName;
          
          if ((atts.getValue("type") == null) ||
              (atts.getValue("multiple") == null))
            throw new SAXNotRecognizedException("No array type or multiplicity available for "+localName);
          
          String type = atts.getValue("type");

          //get multiplicity if needed
          float multiple = Float.parseFloat(atts.getValue("multiple"));
          currentArraySize = Math.round(arrayLength * multiple);
         
          
          //Instanciate a new array parser
          if (type.equals("FLOAT")) {
            arrayParser = new AFloatArrayParser(currentArraySize);
          } else if (type.equals("INT")) {
            arrayParser = new AIntArrayParser(currentArraySize);
          } else if (type.equals("STRING")) {
            arrayParser = new AStringArrayParser(currentArraySize);
          } else {
            throw new SAXNotRecognizedException("Invalid array type: " + type);
          }
          
          break;
          
      } // switch
    } catch (SAXNotRecognizedException sre){
      //Catch unknown array tags
      xmlErrorHandler.setError(ErrorState.INVALID_DATA,sre);
      logger.debug("Encountered invalid subtag "+localName, sre);  
    } catch (Exception e) {
      //Catch any other error here:
      String msg = "Error reading file at " + currentFullName + ":" + currentArrayName;
      xmlErrorHandler.setError(ErrorState.FATAL_ERROR,e);
      logger.debug(msg, e);
      //Rethrow readevent exception with proper cause
      throw new SAXException(msg, e);
    }

  } 
  
  /**
   * Called at the end of each tag - this is where we actually construct the data
   * @param namespaceURI - the current namespace
   * @param localName - the local element name
   * @param qName - the qualified element name
   */
  @Override
  public void endElement(String namespaceURI, String localName, String qName) throws SAXException {
    
    /**
     * Check if an error had occured reading this element
     */
    if (hasError()) {
     
      String msg = null;
      
      switch (xmlErrorHandler.getErrorState()) {
        
        case UNKNOWN_TAG:
          //Ignore unknown tags, but show an error
     
          if (parseState == ParseState.WAITING_DATA)
            msg = "Unknown or obsolete data type <" + currentDataName + "> found";
          else if (parseState == ParseState.WAITING_ARRAY)
            //rely on exception error message for this one
            msg = xmlErrorHandler.getErrorCause().getMessage();
          else 
            //unknown tag at event level
            msg = "Unknown tag <" + currentDataName + "> while waiting for event."; 
          
          //Clear the error state and continue
          xmlErrorHandler.setError(ErrorState.NO_ERROR, null);
          break;
          
        case INVALID_DATA:
          msg = "Invalid data encountered reading "+currentDataName;
          //Clear the error state and continue
          xmlErrorHandler.setError(ErrorState.NO_ERROR, null);
          break;
   
        case IGNORED_DATA:
          
          //Only show warning message in the LogPane once
          if (!HLTIgnoredWarning) {
            AOutput.append("\"HLTAutoKey\" data is hidden, option '-a' to see them.",
                           ALogInterface.WARNING);
            //tell about each one also in the log
            logger.warn(xmlErrorHandler.getErrorCause().getMessage());
            HLTIgnoredWarning = true;
          }
          
          //Clear the error only when we reach the end of the data tag
          if (localName.equals(currentDataName))        
            xmlErrorHandler.setError(ErrorState.NO_ERROR, null);
          
          break;
      }
      
      //If there was a message, show it
      if (msg != null){
        // Now show this error in the log pane and console output
        logger.warn(msg,xmlErrorHandler.getErrorCause());
      }     
      //Return to parsing (if error cleared) or stop
      return;
    }
    
    /**
     * No error so far - check what we have been parsing
     */
    switch (parseState) {
      
      case WAITING_ARRAY:
        
        /**
         * End of data array - add to parameter list
         */
        if (localName.equals(currentArrayName)) {
          
          //Check for proper Array size
          if (arrayParser.getCount() != currentArraySize) {
            
            xmlErrorHandler.setError(ErrorState.INVALID_DATA, new Exception("Invalid array size"));
  
            String msg = "The number of data in <" + currentFullName + ">-<" + currentArrayName + "> is different than declared";
            logger.error(msg);
            AOutput.append("\n"+msg+"\n", ALogInterface.WARNING);
            return;
          }
          //Otherwise add to parameter list 
          parameters.put(currentArrayName, arrayParser.getArray());
          
          //done with that, return
          return;
        }
          
        /**
         * End of data tag - construct object
         */
        if (localName.equals(currentDataName)) {
          try {
            // Calo and muon datatypes sometimes provide a
            // storeGateKey, thus are (incorrectly !)
            // regarded as multiple collection datatypes
            // -> remove this storeGateKey
            if ("TILE".equals(currentDataName) ||
                    "LAr".equals(currentDataName) ||
                    "HEC".equals(currentDataName) ||
                    "FCAL".equals(currentDataName) ||
                    "MBTS".equals(currentDataName) ||
                    "MDT".equals(currentDataName) ||
                    "CSC".equals(currentDataName) ||
                    "RPC".equals(currentDataName) ||
                    "TGC".equals(currentDataName)) {
              parameters.put("storeGateKey", null);
            }

            // RDOs originally implemented as multiple collections,
            // then changed to non-mul-col. this is for backwards
            // compatibility with events where storeGateKey is
            // provided for PixelRDO and SCTRDO
            if ("PixelRDO".equals(currentDataName) ||
                    "SCTRDO".equals(currentDataName)) {
              parameters.put("storeGateKey", null);
            }

            checkPhiRange(parameters, currentFullName);

            if (currentDataName.equals("STr")) {
              event.add(new ASVxData(ASVxData.createSVx(parameters), event));
              event.add(new ASTrData(ASTrData.createSTr(parameters), event));
              event.add(new ASNPData(ASNPData.createSNP(parameters), event));
            } // STC - silicon cluster datatype
            else if (currentDataName.equals("STC")) {
              event.add(new ASiClusterData(parameters, event));
            } // SCTRDO - silicon cluster raw data object
            // discrepancy in naming STC x SCT ...
            else if (currentDataName.equals("SCTRDO")) {
              event.add(new ASiClusterRDOData(parameters, event));
            }  // PixCluster - pixel cluster datatype
            else if (currentDataName.equals("PixCluster")) {
              event.add(new APixelClusterData(parameters, event));
            }
            // ETMis backwards compatibility (after ETMis collections)
            // datatypes ETMis, CaloETMis, MuonETMis - storeGateKey
            // attribute is added here
            else if (currentDataName.equals("ETMis") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "ETMis");
              event.add(new AETMisData(parameters, event));
            } else if (currentDataName.equals("CaloETMis")) {
              parameters.put("storeGateKey", "CaloETMis");
              event.add(new AETMisData(parameters, event));
            } else if (currentDataName.equals("MuonETMis")) {
              parameters.put("storeGateKey", "MuonETMis");
              event.add(new AETMisData(parameters, event));
            } // Track collections, backwards compatibility for
            // IDScan, iPat, xKal datatypes - storeGateKey attribute
            // is added here
            else if (currentDataName.equals("Track")) {

              String key = (String) parameters.get("storeGateKey");
              if (key != null) {
                if (key.toLowerCase().indexOf("mboy") >= 0 || key.toLowerCase().indexOf("mugirl") >=0 || key.toLowerCase().indexOf("moore") >= 0 || key.toLowerCase().indexOf("momu") >= 0 || key.toLowerCase().indexOf("muon") >= 0 || key.toLowerCase().indexOf("staco") >= 0 || key.toLowerCase().indexOf("muid") >= 0 || key.toLowerCase().indexOf("mutag") >= 0 ) {
                  event.add(new AMuonTrackData(parameters, event));
                } else {
                  event.add(new AInDetTrackData(parameters, event));
                }
              } else {
                int[] trackAuthor = parameters.getUnsureIntArray("trackAuthor");
                if (trackAuthor != null && trackAuthor.length > 0) {
                  if ((trackAuthor[0] >= 7 && trackAuthor[0] <= 9) || (trackAuthor[0] >= 13 && trackAuthor[0] <= 17)) {
                    event.add(new AMuonTrackData(parameters, event));
                  } else {
                    event.add(new AInDetTrackData(parameters, event));
                  }
                } else {
                  event.add(new AInDetTrackData(parameters, event));
                }
              }
            } else if (currentDataName.equals("IDScan")) {
              parameters.put("storeGateKey", "TrigInDetTrack");
              event.add(new AInDetTrackData(parameters, event));
            } else if (currentDataName.equals("iPat")) {
              parameters.put("storeGateKey", "iPatTrack");
              event.add(new AInDetTrackData(parameters, event));
            } else if (currentDataName.equals("xKal")) {
              parameters.put("storeGateKey", "xKalTrack");
              event.add(new AInDetTrackData(parameters, event));
            } else if (currentDataName.equals("RTr")) // old tag
            {
              parameters.put("storeGateKey", "RTrTrack");
              event.add(new AInDetTrackData(parameters, event));
            } else if (currentDataName.equals("RMTr")) // old tag
            {
              parameters.put("storeGateKey", "RMTrTrack");
              event.add(new AMuonTrackData(parameters, event));
            } // ParticleJet backwards compatibility
            else if (currentDataName.equals("ParticleJet") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "ParticleJet cone4");
              // treat PatricleJet as Jet (JetRecJet)
              event.add(new AJetData(parameters, event));
            } else if (currentDataName.equals("Jet") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "JetRecJet");
              event.add(new AJetData(parameters, event));
            } // MSeg backwards compatibility
            else if (currentDataName.equals("MSeg")) {
              parameters.put("storeGateKey", "MooreSegment");

              // Instead of cotan(theta) we store theta itself.
              float[] cotTheta = parameters.getFloatArray("cotTheta");
              float[] theta = new float[cotTheta.length];
              for (int i = 0; i < cotTheta.length; i++) {
                theta[i] = (float) Math.atan(1. / cotTheta[i]);
              }
              parameters.put("theta", theta);

              // And phi0 is now called phi
              parameters.put("phi", parameters.getFloatArray("phi0"));

              event.add(new AMuonSegmentData(parameters, event));
            } // Segment backwards compatibility
            else if (currentDataName.equals("Segment")) {
              String key = (String) parameters.get("storeGateKey");
              if (key != null) {
                if (key.toLowerCase().indexOf("mboy") >= 0 || key.toLowerCase().indexOf("moore") >= 0 || key.toLowerCase().indexOf("momu") >= 0 || key.toLowerCase().indexOf("mdt") >= 0 || key.toLowerCase().indexOf("muon") >= 0) {
                  event.add(new AMuonSegmentData(parameters, event));
                } else {
                  event.add(new AInDetSegmentData(parameters, event));
                }
              }
            } // Cluster backwards compatibility
            // datatypes Cluster - storeGateKey attribute is added
            else if (currentDataName.equals("Cluster") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "DefaultCluster");
              event.add(new AClusterData(parameters, event));
            } //Electron backwards compatibility
            else if (currentDataName.equals("Electron") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "ElectronCollection");
              event.add(new AElectronData(parameters, event));
            } //Muon backwards compatibility
            else if (currentDataName.equals("Muon") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "MuonCollection");
              event.add(new AMuonData(parameters, event));
            } //Photon backwards compatibility
            else if (currentDataName.equals("Photon") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "PhotonCollection");
              event.add(new APhotonData(parameters, event));
            } //BJet backwards compatibility
            else if (currentDataName.equals("BJet") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "BJetCollection");
              event.add(new ABJetData(parameters, event));
            } //TauJet backwards compatibility
            else if (currentDataName.equals("TauJet") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "TauJetCollection");
              event.add(new ATauJetData(parameters, event));
            } //CompositeParticle backwards compatibility
            else if (currentDataName.equals("CompositeParticle") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "CompositeParticleCollection");
              event.add(new ACompositeParticleData(parameters, event));
            } //LVL1Result backwards compatibility
            else if (currentDataName.equals("LVL1Result") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "LVL1Result");
              event.add(new ALVL1ResultData(parameters, event));
            } //LVL1TriggerTower backwards compatibility
            else if (currentDataName.equals("LVL1TriggerTower") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "LVL1TriggerTower");
              event.add(new ALVL1TriggerTowerData(parameters, event));
            } //LVL1JetElement backwards compatibility
            else if (currentDataName.equals("LVL1JetElement") &&
                    "".equals(storeGateKey)) {
              parameters.put("storeGateKey", "LVL1JetElement");
              event.add(new ALVL1JetElementData(parameters, event));
            } else {
              //try to autoconstruct the corresponding data type
              //First of all get all constructors for this class name 
              Constructor[] cons = Class.forName("atlantis.data.A" + currentDataName + "Data").getDeclaredConstructors();
              //Flag if we found on
              boolean foundConstructor = false;
              //Now loop over constructors and pick the one that fits
              for (Constructor constructor : cons) {
                //Check for parameters
                Class[] params = constructor.getParameterTypes();
                if (params.length != 2) {
                  continue;
                //Check both types
                }
                if (!params[0].getName().endsWith("AHashMap")) {
                  continue;
                }
                if (!params[1].getName().endsWith("AEvent")) {
                  continue;
                //seems we found the proper constructor - go and build the class
                }
                foundConstructor = true;
                event.add((AData) constructor.newInstance(new Object[]{parameters, event}));
                //also end the loop here
                break;
              }
              if (!foundConstructor) {
                throw new ClassNotFoundException("Found no valid constructor for data type " + currentDataName);
              }
            } // End of autoconstructor
  
          
          //Collect all possible errors here
          } catch (Throwable t) {
            //Will hold the error message
            String msg = null;
            
            //Could not find a class for this data type 
            if (t instanceof ClassNotFoundException) {
              msg = "Class handling the type " + currentDataName + " not found";
              xmlErrorHandler.setError(ErrorState.INVALID_DATA, t);
            //Ran out ouf memory
            } else if (t instanceof OutOfMemoryError) {
              msg = "Atlantis ran out of memory while\n" + "reading in event data";
              xmlErrorHandler.setError(ErrorState.FATAL_ERROR, t);
            //Some weird Atlantis exception - check its error state
            } else if (t instanceof AAtlantisException) {
              msg = t.getMessage();
              //check Atlantis exception level
              if (((AAtlantisException)t).isFatal())
                xmlErrorHandler.setError(ErrorState.FATAL_ERROR, t);
              else
                xmlErrorHandler.setError(ErrorState.OTHER_ERROR, t);
            } else if (t instanceof InvocationTargetException) {
              msg = "Error when constructing " + currentFullName;
              xmlErrorHandler.setError(ErrorState.OTHER_ERROR, t);
            } else {
              msg = "Caught exception "+t.toString()+ " while constructing "+currentFullName;
              xmlErrorHandler.setError(ErrorState.OTHER_ERROR, t);
            }
         
            AOutput.append("\n" + msg + "\n", ALogInterface.WARNING);
            logger.error(msg, t);
          }

          /**
           * Now handle potential errors 
           */
          //if we can proceed processing, throw an error
          if (xmlErrorHandler.getErrorState() == ErrorState.FATAL_ERROR)
            throw new SAXException("Error parsing event at tag <"+currentDataName+">",
                    (Exception)xmlErrorHandler.getErrorCause());
          
          //Otherwise clear error state and continue
          xmlErrorHandler.setError(ErrorState.NO_ERROR, null);
          parseState = ParseState.WAITING_DATA;
          return;
        }
      
    }
  }
  
  /**
   * xml calls this routine whenever it finds CONTENT of type character array
   * (CDATA) this routine fills an array stored in the arrayParser *
   *
   * @param ch char[]
   * @param start int
   * @param length int
   */
  @Override
  public void characters(char[] ch, int start, int length) {
    
    //return immediately if we had an error before
    if (hasError()) return;
   
    //Make sure an arrayParser has been set before
    if (arrayParser == null){
      xmlErrorHandler.setError(ErrorState.FATAL_ERROR,new NullPointerException("ArrayParser object not defined"));
      logger.error("arrayParser is NULL in SAX DefaultHandler:characters");
      return;  
    }
    
    //try to parse the value array
    try {
      arrayParser.parse(ch, start, length);
    } catch (NumberFormatException nfe) {
      xmlErrorHandler.setError(ErrorState.INVALID_DATA,nfe);
      String msg = "Data in <" + currentFullName + ">-<" + currentArrayName +
                   "> contains invalid character \'" + nfe.getMessage() + "\'";
      logger.error(msg);
      
    } catch (IndexOutOfBoundsException oob) {
      xmlErrorHandler.setError(ErrorState.INVALID_DATA,oob);
      String msg = "The number of data in <" + currentFullName + ">-<" 
                   + currentArrayName + "> is more than declared";
      logger.error(msg);
    }

  } // characters() --------------------------------------------------------

  /**
   * Returns an entity resolver - should be event.dtd in case we read an event file
   * otherwise return null to use the default
   * @param publicId
   * @param systemId
   * @return an input source for the entity resolver
   */
  @Override
  public InputSource resolveEntity(String publicId, String systemId) {
    InputSource inputSource = null;
    String FILE_SEPAR = System.getProperty("file.separator");

    if (systemId.endsWith("event.dtd")) {
      String dtdFile = AGlobals.instance().getHomeDirectory() + "events" + FILE_SEPAR + "event.dtd";
      try {
        InputStream is = AUtilities.getFileAsStream(dtdFile);
        inputSource = new InputSource(is);
      } catch (AAtlantisException ae) {
        logger.error("reading " + dtdFile + " error: " + ae.getMessage(), ae);
      }
    }

    return inputSource;

  } // resolveEntity() ----------------------------------------------------

  /**
   * Corrects the Phi values to be in the range (0:2pi).
   * NOTE: This can still be done much nicer - but have you seen what it looked like before?!
   * @param p The AHashMap containing the float arrays to be checked.
   */
  private static void checkPhiRange(AHashMap p, String fullDataName) {

    //Loop over all keys in the Hashmap
    Iterator keyIter = p.keySet().iterator();
    while (keyIter.hasNext()) {
      //Check if this is a phi collection
      String keyName = (String) keyIter.next();
      //Only run on phi value arrays
      if (!keyName.startsWith("phi")) {
        continue;
      //Now  get the actual array of values
      }
      float[] phi = null;
      try {
        phi = (float[]) p.get(keyName);
      } catch (ClassCastException cce) {
        //ignore any non-floar arrays that start with "phi"
        continue;
      }
      //Check that we didn't get an empty entry
      if (phi == null) {
        continue;      //Now loop over all entries and correct them
      }
      for (int i = 0; i < phi.length; i++) {
        //Bring it down to the modulus of 2*Pi
        phi[i] = phi[i] % (float)AMath.TWO_PI;
        //Smaller zero is still possible 
        if (phi[i] < 0.) phi[i] += AMath.TWO_PI;
      }
    }
  } // checkPhiRange() -----------------------------------------------------
} // class AEventFromXML ====================================================
