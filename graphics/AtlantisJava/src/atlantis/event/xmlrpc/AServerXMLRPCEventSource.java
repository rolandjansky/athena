package atlantis.event.xmlrpc;

import atlantis.utils.*;
import atlantis.canvas.ACanvas;
import atlantis.data.AEventFromXML;
import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.event.AEventSource;
import atlantis.globals.AGlobals;
import atlantis.graphics.encoders.AImageEncoder;
import java.io.ByteArrayInputStream;

/**
 *
 * @author Adam Davison
 */
public class AServerXMLRPCEventSource implements AEventSource{
    
    private static ALogger logger = ALogger.getLogger(AServerXMLRPCEventSource.class);
    
    private AEventManager m_aem;
    private ACanvas m_canv;
    private AImageEncoder m_ie;
    
    private AEvent m_event = null; // Current event
    private String m_sourcename = ""; // Source name

    private NavigationMode eventNavigationMode = NavigationMode.PUSH;
    
    public AServerXMLRPCEventSource() {
        m_aem = AEventManager.instance();
        m_canv = ACanvas.getCanvas();
        m_ie = new AImageEncoder();
    }

    public String getVersion() {
        return AGlobals.instance().getVersion();
    }
    
    public boolean isReady() {
        return true;
    }
    
    public boolean shutdown() {
        logger.info("Exiting atlantis...");
        System.exit(0);
        return true;
    }

    public NavigationMode getNavigationMode() {
      return eventNavigationMode;
    }

    public boolean supportsNavigationMode(NavigationMode mode) {
      if(mode == NavigationMode.PUSH)
        return true;
      return false;
    }

    public void setNavigationMode(NavigationMode mode) throws InvalidEventSourceException {
      if(mode != NavigationMode.PUSH)
        throw new InvalidEventSourceException("Mode '"+mode.name()+"' not supported by current source");
      eventNavigationMode = mode;
    }
 
    private void setEventFromBytes(byte[] eventdata, String sourcename) throws Exception {
        AEvent e = AEventFromXML.read(new ByteArrayInputStream(eventdata), sourcename);
        
        m_event = e;
        m_sourcename = sourcename;
        
        // Force update from event manager
        m_aem.setEventSource(this);
        m_aem.nextEvent();
    }
    
    private byte[] getSingleImage(int w, int h, String type, double s) throws Exception {
        if (type.equalsIgnoreCase("png")) {
            return m_ie.getPNGData(w, h);
        } else if (type.equalsIgnoreCase("scaledpng")) {
            return m_ie.getScaledPNGData(w, h, s);
        }
        
        throw new Exception("Unknown image format: " + type);
    }
    
    private byte[] getSingleImageAspect(int w, String type, double s) throws Exception {
        int h = m_canv.getRespectiveHeight(w);
        return getSingleImage(w, h, type, s);
    }
    
    public byte[][] generateDefaultImages(byte[] eventdata, String sourcename,
                int width, double thumb_scale) {
        
        byte ret[][] = new byte[2][];
        
        try {
            setEventFromBytes(eventdata, sourcename);
            ret[0] = getSingleImageAspect(width, "png", 0.0);
            ret[1] = getSingleImageAspect(width, "scaledpng", thumb_scale);
        } catch (Exception e) {
            logger.error("Error while handling request to generateDefaultImages()", e);
            return null;
        }
        
        return ret;
    }

    public AEvent nextEvent() throws NoMoreEventsException, InvalidEventSourceException, ReadEventException {
        if (m_event == null) {
            throw new InvalidEventSourceException();
        }
        return m_event;
    }

    public AEvent previousEvent() throws NoMoreEventsException, InvalidEventSourceException, ReadEventException {
        throw new NoMoreEventsException();
    }

    public String getSourceName() {
        return m_sourcename;
    }
    
    /*public byte[][] generateImages(byte[] eventdata, String sourcename,
                Integer nimgs, Object[] w, Object[] h, Object[] type, Object[] s) {
        
        byte ret[][] = new byte[nimgs][];
        
        try {
            setEventFromBytes(eventdata, sourcename);

            for (int i = 0; i < nimgs; i++) {
                // Handle cast of s better here
                ret[i] = getSingleImage((Integer) (w[i]), (Integer) (h[i]),
                        (String) (type[i]), (Double) (s[i]));
            }
            
        } catch (Exception e) {
            logger.error("Error while handling request to generateImages()", e);
            return null;
        }

        return ret;
    }*/
    
}
